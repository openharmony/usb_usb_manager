/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <set>

#include "usb_host_manager.h"
#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "hilog_wrapper.h"
#include "hisysevent.h"
#include "cJSON.h"
#include "usb_serial_reader.h"
#include "usb_device.h"
#include "usb_config.h"
#include "usb_interface.h"
#include "usb_errors.h"

#ifdef USB_NOTIFICATION_ENABLE
#include "usb_mass_storage_notification.h"
#endif

using namespace OHOS::AAFwk;
using namespace OHOS::EventFwk;
using namespace OHOS::HiviewDFX;

namespace OHOS {
namespace USB {
constexpr int32_t CLASS_PRINT_LENGTH = 2;
constexpr int32_t USAGE_IN_INTERFACE_CLASS = 0;
constexpr uint8_t DES_USAGE_IN_INTERFACE = 0x02;
std::map<int32_t, DeviceClassUsage> deviceUsageMap = {
    {0x00, {DeviceClassUsage(2, "Use class information in the Interface Descriptors")}},
    {0x01, {DeviceClassUsage(2, "Audio")}},
    {0x02, {DeviceClassUsage(3, "Communications and CDC Control")}},
    {0x03, {DeviceClassUsage(2, "HID(Human Interface Device)")}},
    {0x05, {DeviceClassUsage(2, "Physical")}},
    {0x06, {DeviceClassUsage(2, "Image")}},
    {0x07, {DeviceClassUsage(2, "Printer")}},
    {0x08, {DeviceClassUsage(2, "Mass Storage")}},
    {0x09, {DeviceClassUsage(1, "Hub")}},
    {0x0a, {DeviceClassUsage(2, "CDC-Data")}},
    {0x0b, {DeviceClassUsage(2, "Smart Card")}},
    {0x0d, {DeviceClassUsage(2, "Content Security")}},
    {0x0e, {DeviceClassUsage(2, "Video")}},
    {0x0f, {DeviceClassUsage(2, "Personal Healthcare")}},
    {0x10, {DeviceClassUsage(2, "Audio/Video Device")}},
    {0x11, {DeviceClassUsage(1, "Billboard Device Class")}},
    {0x12, {DeviceClassUsage(2, "USB Type-C Bridge Class")}}
};

std::map<UsbDeviceType, std::string> interfaceUsageMap = {
    {{UsbDeviceType(0x03, 0x01, 0x01, 0)}, "KeyBoard"},
    {{UsbDeviceType(0x03, 0x01, 0x02, 0)}, "Mouse/Table/Touch screen"},
};
UsbHostManager::UsbHostManager(SystemAbility *systemAbility)
{
    systemAbility_ = systemAbility;
}

UsbHostManager::~UsbHostManager()
{
    for (auto &pair : devices_) {
        delete pair.second;
    }
    devices_.clear();
}

void UsbHostManager::GetDevices(MAP_STR_DEVICE &devices)
{
    devices = devices_;
}

bool UsbHostManager::GetProductName(const std::string &deviceName, std::string &productName)
{
    auto iter = devices_.find(deviceName);
    if (iter == devices_.end()) {
        return false;
    }

    UsbDevice *dev = iter->second;
    if (dev == nullptr) {
        return false;
    }

    productName = dev->GetProductName();
    return true;
}

bool UsbHostManager::DelDevice(uint8_t busNum, uint8_t devNum)
{
    std::string name = std::to_string(busNum) + "-" + std::to_string(devNum);
    MAP_STR_DEVICE::iterator iter = devices_.find(name);
    if (iter == devices_.end()) {
        USB_HILOGF(MODULE_SERVICE, "name:%{public}s bus:%{public}hhu dev:%{public}hhu not exist", name.c_str(), busNum,
            devNum);
        return false;
    }
    USB_HILOGI(
        MODULE_SERVICE, "device:%{public}s bus:%{public}hhu dev:%{public}hhu erase ", name.c_str(), busNum, devNum);
    UsbDevice *devOld = iter->second;
    if (devOld == nullptr) {
        USB_HILOGE(MODULE_SERVICE, "invalid device");
        return false;
    }

    auto isSuccess = PublishCommonEvent(CommonEventSupport::COMMON_EVENT_USB_DEVICE_DETACHED, *devOld);
    if (!isSuccess) {
        USB_HILOGW(MODULE_SERVICE, "send device attached broadcast failed");
    }
#ifdef USB_NOTIFICATION_ENABLE
    UsbMassStorageNotification::GetInstance()->CancelNotification(devices_, *devOld, name);
#endif

    delete devOld;
    devices_.erase(iter);
    return true;
}

bool UsbHostManager::AddDevice(UsbDevice *dev)
{
    if (dev == nullptr) {
        USB_HILOGF(MODULE_SERVICE, "device is NULL");
        return false;
    }

    auto isSuccess = PublishCommonEvent(CommonEventSupport::COMMON_EVENT_USB_DEVICE_ATTACHED, *dev);
    if (!isSuccess) {
        USB_HILOGW(MODULE_SERVICE, "send device attached broadcast failed");
    }

    uint8_t busNum = dev->GetBusNum();
    uint8_t devNum = dev->GetDevAddr();
    std::string name = std::to_string(busNum) + "-" + std::to_string(devNum);
    MAP_STR_DEVICE::iterator iter = devices_.find(name);
    if (iter != devices_.end()) {
        USB_HILOGF(MODULE_SERVICE, "device:%{public}s bus:%{public}hhu dev:%{public}hhu already exist", name.c_str(),
            busNum, devNum);
        UsbDevice *devOld = iter->second;
        if (devOld != nullptr && devOld != dev) {
            delete devOld;
        }
        devices_.erase(iter);
    }
    USB_HILOGI(
        MODULE_SERVICE, "device:%{public}s bus:%{public}hhu dev:%{public}hhu insert", name.c_str(), busNum, devNum);
    devices_.insert(std::pair<std::string, UsbDevice *>(name, dev));

#ifdef USB_NOTIFICATION_ENABLE
    UsbMassStorageNotification::GetInstance()->SendNotification(*dev);
#endif
    return true;
}

bool UsbHostManager::PublishCommonEvent(const std::string &event, const UsbDevice &dev)
{
    Want want;
    want.SetAction(event);
    CommonEventData data(want);
    data.SetData(dev.getJsonString().c_str());
    CommonEventPublishInfo publishInfo;
    USB_HILOGI(MODULE_SERVICE, "send %{public}s broadcast device:%{public}s", event.c_str(),
        dev.getJsonString().c_str());
    ReportHostPlugSysEvent(event, dev);
    return CommonEventManager::PublishCommonEvent(data, publishInfo);
}

bool UsbHostManager::Dump(int fd, const std::string &args)
{
    if (args.compare("-a") != 0) {
        dprintf(fd, "args is not -a\n");
        return false;
    }

    dprintf(fd, "Usb Host all device list info:\n");
    for (const auto &item : devices_) {
        dprintf(fd, "usb host list info: %s\n", item.second->getJsonString().c_str());
    }
    return true;
}

int32_t UsbHostManager::GetDeviceDescription(int32_t baseClass, std::string &description, uint8_t &usage)
{
    auto iter = deviceUsageMap.find(baseClass);
    if (iter != deviceUsageMap.end()) {
        description = iter->second.description;
        usage = iter->second.usage;
    } else {
        description = "NA";
        usage = 1;
    }
    return UEC_OK;
}


std::string UsbHostManager::ConcatenateToDescription(const UsbDeviceType &interfaceType, const std::string& str)
{
    std::stringstream ss;
    ss << std::setw(CLASS_PRINT_LENGTH) << std::setfill('0') << std::hex << interfaceType.baseClass << "_";
    ss << std::setw(CLASS_PRINT_LENGTH) << std::setfill('0') << std::hex << interfaceType.subClass << "_";
    ss << std::setw(CLASS_PRINT_LENGTH) << std::setfill('0') << std::hex << interfaceType.protocol << ",";
    ss << str;
    return ss.str();
}

std::string UsbHostManager::GetInterfaceUsageDescription(const UsbDeviceType &interfaceType)
{
    std::string infUsageDes = "NA";
    auto infUsageIter = interfaceUsageMap.find(interfaceType);
    if (infUsageIter != interfaceUsageMap.end()) {
        return infUsageIter->second;
    }
    return infUsageDes;
}

int32_t UsbHostManager::GetInterfaceDescription(const UsbDevice &dev, std::string &description, int32_t &baseClass)
{
    std::set<UsbDeviceType> useInterfaceType;
    for (int32_t i = 0; i < dev.GetConfigCount(); i++) {
        USBConfig config;
        dev.GetConfig(i, config);
        for (uint32_t j = 0; j < config.GetInterfaceCount(); j++) {
            if (i != 0 || j != 0) {
                description += ";";
            }
            UsbInterface interface;
            config.GetInterface(j, interface);
            baseClass = interface.GetClass();
            UsbDeviceType interfaceType = {interface.GetClass(),
                interface.GetSubClass(), interface.GetProtocol(), 0};
                useInterfaceType.insert(interfaceType);
                std::string infUsageDes = GetInterfaceUsageDescription(interfaceType);
                description += ConcatenateToDescription(interfaceType, infUsageDes);
        }
    }
    return UEC_OK;
}

void UsbHostManager::ReportHostPlugSysEvent(const std::string &event, const UsbDevice &dev)
{
    std::string deviceUsageDes;
    uint8_t deviceUsage = 0;
    GetDeviceDescription(dev.GetClass(), deviceUsageDes, deviceUsage);
    std::string extUsageDes;
    int32_t intfBaseClass = 0;
    if (deviceUsage & DES_USAGE_IN_INTERFACE) {
        GetInterfaceDescription(dev, extUsageDes, intfBaseClass);
    }

    if (dev.GetClass() == USAGE_IN_INTERFACE_CLASS) {
        GetDeviceDescription(intfBaseClass, deviceUsageDes, deviceUsage);
    }
    USB_HILOGI(MODULE_SERVICE, "Host mode Indicates the insertion and removal information");
    HiSysEventWrite(HiSysEvent::Domain::USB, "PLUG_IN_OUT_HOST_MODE", HiSysEvent::EventType::BEHAVIOR,
        "DEVICE_NAME", dev.GetName(), "DEVICE_PROTOCOL", dev.GetProtocol(),
        "DEVICE_SUBCLASS", dev.GetSubclass(), "DEVICE_CLASS", dev.GetClass(),
        "DEVICE_CLASS_DESCRIPTION", deviceUsageDes, "INTERFACE_CLASS_DESCRIPTION", extUsageDes,
        "VENDOR_ID", dev.GetVendorId(), "PRODUCT_ID", dev.GetProductId(),
        "VERSION", dev.GetVersion(), "EVENT_NAME", event);
}
} // namespace USB
} // namespace OHOS
