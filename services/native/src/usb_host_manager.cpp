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
#include <thread>
#include <ipc_skeleton.h>

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
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "parameters.h"
#include "usbd_bulkcallback_impl.h"
#include "usb_descriptor_parser.h"
#include "usbd_transfer_callback_impl.h"
#include "usb_napi_errors.h"
#include "accesstoken_kit.h"

using namespace OHOS::AAFwk;
using namespace OHOS::EventFwk;
using namespace OHOS::HiviewDFX;
using namespace OHOS::HDI::Usb::V1_2;

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
constexpr uint32_t CURSOR_INIT = 18;
constexpr int32_t DESCRIPTOR_TYPE_STRING = 3;
constexpr int32_t DESCRIPTOR_VALUE_START_OFFSET = 2;
constexpr int32_t HALF = 2;
constexpr int32_t BIT_SHIFT_4 = 4;
constexpr int32_t BIT_HIGH_4 = 0xF0;
constexpr int32_t BIT_LOW_4 = 0x0F;
constexpr uint32_t MANAGE_INTERFACE_INTERVAL = 100;
constexpr uint32_t EDM_SA_MAX_TIME_OUT = 5000;
constexpr uint32_t EDM_SYSTEM_ABILITY_ID = 1601;
const std::u16string DESCRIPTOR = u"ohos.edm.IEnterpriseDeviceMgr";
constexpr uint32_t WITHOUT_USERID = 0;
constexpr uint32_t WITHOUT_ADMIN = 1;
constexpr uint32_t EMD_MASK_CODE = 20;
constexpr uint32_t DISABLE_USB = 1043;
constexpr uint32_t ALLOWED_USB_DEVICES = 1044;
constexpr uint32_t USB_STORAGE_DEVICE_ACCESS_POLICY = 1026;
constexpr uint32_t USB_DEVICE_ACCESS_POLICY = 1059;
constexpr int32_t WHITELIST_POLICY_MAX_DEVICES = 1000;
constexpr uint32_t EDM_SA_TIME_OUT_CODE = 9200007;
constexpr int32_t BASECLASS_INDEX = 0;
constexpr int32_t SUBCLASS_INDEX = 1;
constexpr int32_t PROTOCAL_INDEX = 2;
constexpr int32_t STORAGE_BASE_CLASS = 8;
constexpr int32_t GET_EDM_STORAGE_DISABLE_TYPE = 2;
constexpr int32_t RANDOM_VALUE_INDICATE = -1;
const int32_t IO_ERROR = -1;
const int32_t INVALID_PARAM = -2;
const int32_t NO_DEVICE = -4;
const int32_t NOT_FOUND = -5;
const int32_t ERROR_BUSY = -6;
const int32_t NO_MEM = -11;
#ifdef USB_MANAGER_PASS_THROUGH
const std::string SERVICE_NAME = "usb_host_interface_service";
#endif // USB_MANAGER_PASS_THROUGH
UsbHostManager::UsbHostManager(SystemAbility *systemAbility)
{
    systemAbility_ = systemAbility;
    usbRightManager_ = std::make_shared<UsbRightManager>();
    usbd_ = OHOS::HDI::Usb::V1_2::IUsbInterface::Get();
}

UsbHostManager::~UsbHostManager()
{
    for (auto &pair : devices_) {
        delete pair.second;
    }
    devices_.clear();
}

#ifdef USB_MANAGER_PASS_THROUGH
bool UsbHostManager::InitUsbHostInterface()
{
    USB_HILOGI(MODULE_USB_SERVICE, "InitUsbHostInterface in");
    usbHostInterface_ = HDI::Usb::V2_0::IUsbHostInterface::Get(SERVICE_NAME, true);
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "InitUsbHostInterface get usbHostInterface_ is nullptr");
        return false;
    }
    usbManagerSubscriber_ = new (std::nothrow) UsbManagerSubscriber();
    if (usbManagerSubscriber_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "lsq usbManagerSubscriber_ is nullptr");
        return false;
    }
    ErrCode ret = usbHostInterface_->BindUsbdHostSubscriber(usbManagerSubscriber_);
    USB_HILOGI(MODULE_USB_SERVICE, "entry InitUsbHostInterface ret: %{public}d", ret);
    return SUCCEEDED(ret);
}

void UsbHostManager::Stop()
{
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbHostInterface_ is nullptr");
        return;
    }
    usbHostInterface_->UnbindUsbdHostSubscriber(usbManagerSubscriber_);
    Memory::MemMgrClient::GetInstance().NotifyProcessStatus(getpid(), 1, 0, USB_SYSTEM_ABILITY_ID);
}

int32_t UsbHostManager::BindUsbdSubscriber(const sptr<HDI::Usb::V2_0::IUsbdSubscriber> &subscriber)
{
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::BindUsbdSubscriber usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbHostInterface_->BindUsbdHostSubscriber(subscriber);
}

int32_t UsbHostManager::UnbindUsbdSubscriber(const sptr<HDI::Usb::V2_0::IUsbdSubscriber> &subscriber)
{
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::UnbindUsbdSubscriber usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbHostInterface_->UnbindUsbdHostSubscriber(subscriber);
}
#endif // USB_MANAGER_PASS_THROUGH

// LCOV_EXCL_START
void UsbHostManager::UsbSubmitTransferDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    USB_HILOGI(MODULE_USBD, "UsbHostManager UsbSubmitTransferDeathRecipient enter");
    int32_t ret = service_->UsbCancelTransfer(devInfo_, endpoint_);
    if (ret == UEC_OK) {
        USB_HILOGI(MODULE_USBD, "UsbHostManager OnRemoteDied Close.");
        service_->Close(devInfo_.busNum, devInfo_.devAddr);
    }
}
// LCOV_EXCL_STOP

void UsbHostManager::ExecuteStrategy(UsbDevice *devInfo)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbHostManager::ExecuteStrategy start");
    if (!IsEdmEnabled()) {
        USB_HILOGE(MODULE_USB_SERVICE, "edm is not activate, skip");
        return;
    }
    bool isGlobalDisabled = false;
    std::vector<UsbDeviceType> disableType{};
    std::vector<UsbDeviceId> trustUsbDeviceIds{};

    int32_t ret = GetUsbPolicy(isGlobalDisabled, disableType, trustUsbDeviceIds);
    if (ret == UEC_SERVICE_EDM_SA_TIME_OUT_FAILED || ret == UEC_SERVICE_PREPARE_EDM_SA_FAILED) {
        USB_HILOGE(MODULE_USB_SERVICE, "EDM sa time out or prepare failed, ret = %{public}d", ret);
        return;
    }

    if (isGlobalDisabled) {
        ret = ManageGlobalInterfaceImpl(isGlobalDisabled);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ManageGlobalInterface failed");
        }
        return;
    }

    if (!disableType.empty()) {
        ret = ExecuteManageInterfaceType(disableType, true);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ExecuteManageInterfaceType failed");
        }
        return;
    }

    if (trustUsbDeviceIds.empty()) {
        USB_HILOGI(MODULE_USB_SERVICE, "trustUsbDeviceIds is empty, no devices disable");
        return;
    }
    ret = ExecuteManageDevicePolicy(trustUsbDeviceIds);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "ExecuteManageDevicePolicy failed");
    }
    return;
}

int32_t UsbHostManager::OpenDevice(uint8_t busNum, uint8_t devAddr)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::OpenDevice usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev dev = {busNum, devAddr};
    return usbHostInterface_->OpenDevice(dev);
#else
    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->OpenDevice(dev);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::Close(uint8_t busNum, uint8_t devAddr)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::Close usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev dev = {busNum, devAddr};
    return usbHostInterface_->CloseDevice(dev);
#else
    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->CloseDevice(dev);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::ResetDevice(uint8_t busNum, uint8_t devAddr)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::ResetDevice usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev dev = {busNum, devAddr};
    return usbHostInterface_->ResetDevice(dev);
#else
    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->ResetDevice(dev);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::ClaimInterface(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid, uint8_t force)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::ClaimInterface usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev dev = {busNum, devAddr};
    return usbHostInterface_->ClaimInterface(dev, interfaceid, force);
#else
    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->ClaimInterface(dev, interfaceid, force);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::SetInterface(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid, uint8_t altIndex)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::SetInterface usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev dev = {busNum, devAddr};
    return usbHostInterface_->SetInterface(dev, interfaceid, altIndex);
#else
    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->SetInterface(dev, interfaceid, altIndex);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::ReleaseInterface(uint8_t busNum, uint8_t devAddr, uint8_t interface)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::ReleaseInterface usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev dev = {busNum, devAddr};
    return usbHostInterface_->ReleaseInterface(dev, interface);
#else
    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->ReleaseInterface(dev, interface);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::SetActiveConfig(uint8_t busNum, uint8_t devAddr, uint8_t configIndex)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::SetActiveConfig usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev dev = {busNum, devAddr};
    return usbHostInterface_->SetConfig(dev, configIndex);
#else
    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->SetConfig(dev, configIndex);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::ManageGlobalInterface(bool disable)
{
    return ManageGlobalInterfaceImpl(disable);
}

int32_t UsbHostManager::ManageDevice(int32_t vendorId, int32_t productId, bool disable)
{
    return ManageDeviceImpl(vendorId, productId, disable);
}

int32_t UsbHostManager::ManageInterfaceType(const std::vector<UsbDeviceType> &disableType, bool disable)
{
    return ExecuteManageInterfaceType(disableType, disable);
}

int32_t UsbHostManager::UsbAttachKernelDriver(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::UsbAttachKernelDriver usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev dev = {busNum, devAddr};
    return usbHostInterface_->ManageInterface(dev, interfaceid, false);
#else
    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    return usbd_->ManageInterface(dev, interfaceid, false);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::UsbDetachKernelDriver(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::UsbDetachKernelDriver usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev dev = {busNum, devAddr};
    return usbHostInterface_->ManageInterface(dev, interfaceid, true);
#else
    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    return usbd_->ManageInterface(dev, interfaceid, true);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::ClearHalt(uint8_t busNum, uint8_t devAddr, uint8_t interfaceId, uint8_t endpointId)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::ClearHalt usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev dev = {busNum, devAddr};
    const HDI::Usb::V2_0::UsbPipe pipe = {interfaceId, endpointId};
    return usbHostInterface_->ClearHalt(dev, pipe);
#else
    const UsbDev dev = {busNum, devAddr};
    const UsbPipe pipe = {interfaceId, endpointId};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "ClearHalt: usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->ClearHalt(dev, pipe);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::GetDevices(std::vector<UsbDevice> &deviceList)
{
    USB_HILOGI(MODULE_USB_SERVICE, "list size %{public}zu", devices_.size());
    for (auto it = devices_.begin(); it != devices_.end(); ++it) {
        if (!(usbRightManager_->IsSystemAppOrSa())) {
            it->second->SetmSerial("");
        }
        deviceList.push_back(*it->second);
    }
    return UEC_OK;
}

int32_t UsbHostManager::GetDeviceInfo(uint8_t busNum, uint8_t devAddr, UsbDevice &dev)
{
    const UsbDev uDev = {busNum, devAddr};
    std::vector<uint8_t> descriptor;

    int32_t res = UEC_OK;
    int32_t ret = OpenDevice(busNum, devAddr);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetDeviceInfo OpenDevice failed ret=%{public}d", ret);
        return ret;
    }

    ret = GetDeviceInfoDescriptor(uDev, descriptor, dev);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetDeviceInfoDescriptor ret=%{public}d", ret);
        res = Close(busNum, devAddr);
        if (res != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "GetDeviceInfo CloseDevice failed res=%{public}d", res);
            return res;
        }
        return ret;
    }
    res = GetConfigDescriptor(dev, descriptor);
    if (res != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetConfigDescriptor ret=%{public}d", ret);
    }
    ret = Close(busNum, devAddr);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetDeviceInfo CloseDevice failed ret=%{public}d", ret);
        return ret;
    }
    return res;
}

int32_t UsbHostManager::GetDeviceInfoDescriptor(const HDI::Usb::V1_0::UsbDev &uDev, std::vector<uint8_t> &descriptor,
    UsbDevice &dev)
{
    int32_t ret = GetRawDescriptor(uDev.busNum, uDev.devAddr, descriptor);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetRawDescriptor failed ret=%{public}d busNum:%{public}d devAddr:%{public}d",
            ret, uDev.busNum, uDev.devAddr);
        return ret;
    }
    uint8_t *buffer = descriptor.data();
    uint32_t length = descriptor.size();
    if ((!buffer) || (length == 0)) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetRawDescriptor failed len=%{public}d busNum:%{public}d devAddr:%{public}d",
            length, uDev.busNum, uDev.devAddr);
        return UEC_SERVICE_INVALID_VALUE;
    }
    dev.SetBusNum(uDev.busNum);
    dev.SetDevAddr(uDev.devAddr);
    dev.SetName(std::to_string(uDev.busNum) + "-" + std::to_string(uDev.devAddr));

    ret = UsbDescriptorParser::ParseDeviceDescriptor(buffer, length, dev);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "ParseDeviceDescriptor failed ret=%{public}d", ret);
        return ret;
    }
    return ret;
}

int32_t UsbHostManager::GetConfigDescriptor(UsbDevice &dev, std::vector<uint8_t> &descriptor)
{
    std::vector<USBConfig> configs;
    int32_t ret = UsbDescriptorParser::ParseConfigDescriptors(descriptor, CURSOR_INIT, configs);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "ParseConfigDescriptors failed ret=%{public}d", ret);
        return ret;
    }
    dev.SetConfigs(configs);
    ret = FillDevStrings(dev);
    USB_HILOGI(MODULE_USB_SERVICE, "FillDevStrings ret=%{public}d", ret);
    return ret;
}

int32_t UsbHostManager::GetActiveConfig(uint8_t busNum, uint8_t devAddr, uint8_t &configIndex)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev dev = {busNum, devAddr};
    return usbHostInterface_->GetConfig(dev, configIndex);
#else
    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    return usbd_->GetConfig(dev, configIndex);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::GetRawDescriptor(uint8_t busNum, uint8_t devAddr, std::vector<uint8_t> &bufferData)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::GetRawDescriptor usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev dev = {busNum, devAddr};
    return usbHostInterface_->GetRawDescriptor(dev, bufferData);
#else
    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->GetRawDescriptor(dev, bufferData);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::GetFileDescriptor(uint8_t busNum, uint8_t devAddr, int32_t &fd)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::GetFileDescriptor usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev dev = {busNum, devAddr};
    return usbHostInterface_->GetDeviceFileDescriptor(dev, fd);
#else
    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->GetDeviceFileDescriptor(dev, fd);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::GetDeviceSpeed(uint8_t busNum, uint8_t devAddr, uint8_t &speed)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::GetDeviceSpeed usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev dev = {busNum, devAddr};
    return usbHostInterface_->GetDeviceSpeed(dev, speed);
#else
    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->GetDeviceSpeed(dev, speed);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::GetInterfaceActiveStatus(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid,
    bool &unactivated)
{
#ifdef USB_MANAGER_PASS_THROUGH
    const HDI::Usb::V2_0::UsbDev dev = {busNum, devAddr};
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::GetInterfaceActiveStatus usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbHostInterface_->GetInterfaceActiveStatus(dev, interfaceid, unactivated);
#else
    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->GetInterfaceActiveStatus(dev, interfaceid, unactivated);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::BulkTransferRead(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe,
    std::vector<uint8_t> &bufferData, int32_t timeOut)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::BulkTransferRead usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev &usbDev_ = reinterpret_cast<const HDI::Usb::V2_0::UsbDev &>(dev);
    const HDI::Usb::V2_0::UsbPipe &usbPipe_ = reinterpret_cast<const HDI::Usb::V2_0::UsbPipe &>(pipe);
    return usbHostInterface_->BulkTransferRead(usbDev_, usbPipe_, timeOut, bufferData);
#else
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    return usbd_->BulkTransferRead(dev, pipe, timeOut, bufferData);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::BulkTransferReadwithLength(const HDI::Usb::V1_0::UsbDev &dev,
    const HDI::Usb::V1_0::UsbPipe &pipe, int32_t length, std::vector<uint8_t> &bufferData, int32_t timeOut)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::BulkTransferReadwithLength usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev &usbDev_ = reinterpret_cast<const HDI::Usb::V2_0::UsbDev &>(dev);
    const HDI::Usb::V2_0::UsbPipe &usbPipe_ = reinterpret_cast<const HDI::Usb::V2_0::UsbPipe &>(pipe);
    return usbHostInterface_->BulkTransferReadwithLength(usbDev_, usbPipe_, timeOut, length, bufferData);
#else
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->BulkTransferReadwithLength(dev, pipe, timeOut, length, bufferData);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::BulkTransferWrite(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe,
    const std::vector<uint8_t> &bufferData, int32_t timeOut)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::BulkTransferWrite usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev &usbDev_ = reinterpret_cast<const HDI::Usb::V2_0::UsbDev &>(dev);
    const HDI::Usb::V2_0::UsbPipe &usbPipe_ = reinterpret_cast<const HDI::Usb::V2_0::UsbPipe &>(pipe);
    return usbHostInterface_->BulkTransferWrite(usbDev_, usbPipe_, timeOut, bufferData);
#else
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->BulkTransferWrite(dev, pipe, timeOut, bufferData);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::ControlTransfer(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbCtrlTransfer &ctrl,
    std::vector<uint8_t> &bufferData)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::ControlTransfer usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev &usbDev_ = reinterpret_cast<const HDI::Usb::V2_0::UsbDev &>(dev);
    const HDI::Usb::V2_0::UsbCtrlTransfer &usbCtrl_ = reinterpret_cast<const HDI::Usb::V2_0::UsbCtrlTransfer &>(ctrl);
    int32_t ret = UEC_SERVICE_INNER_ERR;
    std::lock_guard<std::mutex> guard(transferMutex_);
    if (((uint32_t)ctrl.requestType & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_DIR_OUT) {
        ret = usbHostInterface_->ControlTransferWrite(usbDev_, usbCtrl_, bufferData);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ControlTransfer ControlTransferWrite error ret:%{public}d", ret);
        }
    } else {
        bufferData.clear();
        ret = usbHostInterface_->ControlTransferRead(usbDev_, usbCtrl_, bufferData);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ControlTransfer ControlTransferRead error ret:%{public}d", ret);
        }
    }
    return ret;
#else
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = UEC_SERVICE_INNER_ERR;
    std::lock_guard<std::mutex> guard(transferMutex_);
    if (((uint32_t)ctrl.requestType & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_DIR_OUT) {
        ret = usbd_->ControlTransferWrite(dev, ctrl, bufferData);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ControlTransferWrite error ret:%{public}d", ret);
        }
    } else {
        bufferData.clear();
        ret = usbd_->ControlTransferRead(dev, ctrl, bufferData);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ControlTransferRead error ret:%{public}d", ret);
        }
    }
    return ret;
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::UsbControlTransfer(const HDI::Usb::V1_0::UsbDev &dev,
    const HDI::Usb::V1_2::UsbCtrlTransferParams &ctrlParams, std::vector<uint8_t> &bufferData)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::UsbControlTransfer usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev &usbDev_ = reinterpret_cast<const HDI::Usb::V2_0::UsbDev &>(dev);
    const HDI::Usb::V2_0::UsbCtrlTransferParams &usbCtrlParams_ =
        reinterpret_cast<const HDI::Usb::V2_0::UsbCtrlTransferParams &>(ctrlParams);
    int32_t ret = UEC_SERVICE_INNER_ERR;
    HDI::Usb::V2_0::UsbCtrlTransfer ctrl = {
        ctrlParams.requestType, ctrlParams.requestCmd, ctrlParams.value, ctrlParams.index, ctrlParams.timeout};

    std::lock_guard<std::mutex> guard(transferMutex_);
    if (((uint32_t)ctrlParams.requestType & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_DIR_OUT) {
        ret = usbHostInterface_->ControlTransferWrite(usbDev_, ctrl, bufferData);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ControlTransferWrite UsbControlTransfer error ret:%{public}d", ret);
        }
    } else {
        bufferData.clear();
        ret = usbHostInterface_->ControlTransferReadwithLength(usbDev_, usbCtrlParams_, bufferData);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ControlTransferWritewithLength error ret:%{public}d", ret);
        }
    }
    return ret;
#else
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = UEC_SERVICE_INNER_ERR;
    UsbCtrlTransfer ctrl = {
        ctrlParams.requestType, ctrlParams.requestCmd, ctrlParams.value, ctrlParams.index, ctrlParams.timeout};

    std::lock_guard<std::mutex> guard(transferMutex_);
    if (((uint32_t)ctrlParams.requestType & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_DIR_OUT) {
        ret = usbd_->ControlTransferWrite(dev, ctrl, bufferData);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ControlTransferWrite error ret:%{public}d", ret);
        }
    } else {
        bufferData.clear();
        ret = usbd_->ControlTransferReadwithLength(dev, ctrlParams, bufferData);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ControlTransferWritewithLength error ret:%{public}d", ret);
        }
    }
    return ret;
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::RequestQueue(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe,
    const std::vector<uint8_t> &clientData, const std::vector<uint8_t> &bufferData)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::RequestQueue usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev &usbDev_ = reinterpret_cast<const HDI::Usb::V2_0::UsbDev &>(dev);
    const HDI::Usb::V2_0::UsbPipe &usbPipe_ = reinterpret_cast<const HDI::Usb::V2_0::UsbPipe &>(pipe);
    return usbHostInterface_->RequestQueue(usbDev_, usbPipe_, clientData, bufferData);
#else
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->RequestQueue(dev, pipe, clientData, bufferData);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::RequestWait(const HDI::Usb::V1_0::UsbDev &dev, int32_t timeOut,
    std::vector<uint8_t> &clientData, std::vector<uint8_t> &bufferData)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::RequestWait usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev &usbDev_ = reinterpret_cast<const HDI::Usb::V2_0::UsbDev &>(dev);
    return usbHostInterface_->RequestWait(usbDev_, clientData, bufferData, timeOut);
#else
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->RequestWait(dev, clientData, bufferData, timeOut);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::RequestCancel(uint8_t busNum, uint8_t devAddr, uint8_t interfaceId, uint8_t endpointId)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::RequestCancel usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev dev = {busNum, devAddr};
    const HDI::Usb::V2_0::UsbPipe pipe = {interfaceId, endpointId};
    return usbHostInterface_->RequestCancel(dev, pipe);
#else
    const UsbDev dev = {busNum, devAddr};
    const UsbPipe pipe = {interfaceId, endpointId};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    return usbd_->RequestCancel(dev, pipe);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::UsbCancelTransfer(const HDI::Usb::V1_0::UsbDev &devInfo, const int32_t &endpoint)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::UsbCancelTransfer usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev &usbDev_ = reinterpret_cast<const HDI::Usb::V2_0::UsbDev &>(devInfo);
    return usbHostInterface_->UsbCancelTransfer(usbDev_, endpoint);
#else
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbd_->UsbCancelTransfer(devInfo, endpoint);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbCancelTransfer error ret:%{public}d", ret);
        return UsbSubmitTransferErrorCode(ret);
    }
    return ret;
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::UsbSubmitTransfer(const HDI::Usb::V1_0::UsbDev &devInfo, HDI::Usb::V1_2::USBTransferInfo &info,
    const sptr<IRemoteObject> &cb, sptr<Ashmem> &ashmem)
{
    int32_t ret = UEC_SERVICE_INVALID_VALUE;
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::UsbSubmitTransfer usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    sptr<UsbHostManager::UsbSubmitTransferDeathRecipient> submitRecipient =
        new UsbSubmitTransferDeathRecipient(devInfo, info.endpoint, this, cb);
    if (!cb->AddDeathRecipient(submitRecipient)) {
        USB_HILOGE(MODULE_USB_SERVICE, "add DeathRecipient failed");
        return UEC_SERVICE_INVALID_VALUE;
    }
    sptr<UsbTransferCallbackImpl> callbackImpl = new UsbTransferCallbackImpl(cb);
    const HDI::Usb::V2_0::UsbDev &usbDev_ = reinterpret_cast<const HDI::Usb::V2_0::UsbDev &>(devInfo);
    const HDI::Usb::V2_0::USBTransferInfo &usbInfo = reinterpret_cast<const HDI::Usb::V2_0::USBTransferInfo &>(info);
    ret = usbHostInterface_->UsbSubmitTransfer(usbDev_, usbInfo, callbackImpl, ashmem);
#else
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::UsbSubmitTransfer usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    sptr<UsbHostManager::UsbSubmitTransferDeathRecipient> submitRecipient =
        new UsbSubmitTransferDeathRecipient(devInfo, info.endpoint, this, cb);
    if (!cb->AddDeathRecipient(submitRecipient)) {
        USB_HILOGE(MODULE_USB_SERVICE, "add DeathRecipient failed");
        return UEC_SERVICE_INVALID_VALUE;
    }
    sptr<UsbdTransferCallbackImpl> callbackImpl = new UsbdTransferCallbackImpl(cb);
    ret = usbd_->UsbSubmitTransfer(devInfo, info, callbackImpl, ashmem);
#endif // USB_MANAGER_PASS_THROUGH
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager UsbSubmitTransfer error ret:%{public}d", ret);
        cb->RemoveDeathRecipient(submitRecipient);
        submitRecipient.clear();
        return UsbSubmitTransferErrorCode(ret);
    }
    return ret;
}

int32_t UsbHostManager::UsbSubmitTransferErrorCode(int32_t &error)
{
    switch (error) {
        case IO_ERROR:
            return USB_SUBMIT_TRANSFER_IO_ERROR;
        case INVALID_PARAM:
            return OHEC_COMMON_PARAM_ERROR;
        case NO_DEVICE:
            return USB_SUBMIT_TRANSFER_NO_DEVICE_ERROR;
        case NOT_FOUND:
            return USB_SUBMIT_TRANSFER_NOT_FOUND_ERROR;
        case ERROR_BUSY:
            return USB_SUBMIT_TRANSFER_RESOURCE_BUSY_ERROR;
        case NO_MEM:
            return USB_SUBMIT_TRANSFER_NO_MEM_ERROR;
        default:
            return USB_SUBMIT_TRANSFER_OTHER_ERROR;
    }
}

int32_t UsbHostManager::RegBulkCallback(const HDI::Usb::V1_0::UsbDev &devInfo, const HDI::Usb::V1_0::UsbPipe &pipe,
    const sptr<IRemoteObject> &cb)
{
    if (cb == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "cb is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    std::lock_guard<std::mutex> guard(hdiCbMutex_);
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostHdiCb_ == nullptr) {
        usbHostHdiCb_ = new UsbBulkCallbackImpl(cb);
    }
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::RegBulkCallback usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev &usbDev_ = reinterpret_cast<const HDI::Usb::V2_0::UsbDev &>(devInfo);
    const HDI::Usb::V2_0::UsbPipe &usbPipe_ = reinterpret_cast<const HDI::Usb::V2_0::UsbPipe &>(pipe);
    return usbHostInterface_->RegBulkCallback(usbDev_, usbPipe_, usbHostHdiCb_);
#else
    if (hdiCb_ == nullptr) {
        hdiCb_ = new UsbdBulkCallbackImpl(cb);
    }
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    return usbd_->RegBulkCallback(devInfo, pipe, hdiCb_);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::UnRegBulkCallback(const HDI::Usb::V1_0::UsbDev &devInfo, const HDI::Usb::V1_0::UsbPipe &pipe)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::UnRegBulkCallback usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    std::lock_guard<std::mutex> guard(hdiCbMutex_);
    usbHostHdiCb_ = nullptr;
    const HDI::Usb::V2_0::UsbDev &usbDev_ = reinterpret_cast<const HDI::Usb::V2_0::UsbDev &>(devInfo);
    const HDI::Usb::V2_0::UsbPipe &usbPipe_ = reinterpret_cast<const HDI::Usb::V2_0::UsbPipe &>(pipe);
    return usbHostInterface_->UnRegBulkCallback(usbDev_, usbPipe_);
#else
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    std::lock_guard<std::mutex> guard(hdiCbMutex_);
    hdiCb_ = nullptr;
    return usbd_->UnRegBulkCallback(devInfo, pipe);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::BulkRead(const HDI::Usb::V1_0::UsbDev &devInfo, const HDI::Usb::V1_0::UsbPipe &pipe,
    sptr<Ashmem> &ashmem)
{
    if (ashmem == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "BulkRead error ashmem");
        return UEC_SERVICE_INVALID_VALUE;
    }
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::BulkRead usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev &usbDev_ = reinterpret_cast<const HDI::Usb::V2_0::UsbDev &>(devInfo);
    const HDI::Usb::V2_0::UsbPipe &usbPipe_ = reinterpret_cast<const HDI::Usb::V2_0::UsbPipe &>(pipe);
    return usbHostInterface_->BulkRead(usbDev_, usbPipe_, ashmem);
#else
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    return usbd_->BulkRead(devInfo, pipe, ashmem);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::BulkWrite(const HDI::Usb::V1_0::UsbDev &devInfo, const HDI::Usb::V1_0::UsbPipe &pipe,
    sptr<Ashmem> &ashmem)
{
    if (ashmem == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "BulkWrite error ashmem");
        return UEC_SERVICE_INVALID_VALUE;
    }
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::BulkWrite usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev &usbDev_ = reinterpret_cast<const HDI::Usb::V2_0::UsbDev &>(devInfo);
    const HDI::Usb::V2_0::UsbPipe &usbPipe_ = reinterpret_cast<const HDI::Usb::V2_0::UsbPipe &>(pipe);
    return usbHostInterface_->BulkWrite(usbDev_, usbPipe_, ashmem);
#else
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    return usbd_->BulkWrite(devInfo, pipe, ashmem);
#endif // USB_MANAGER_PASS_THROUGH
}

int32_t UsbHostManager::BulkCancel(const HDI::Usb::V1_0::UsbDev &devInfo, const HDI::Usb::V1_0::UsbPipe &pipe)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::BulkCancel usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev &usbDev_ = reinterpret_cast<const HDI::Usb::V2_0::UsbDev &>(devInfo);
    const HDI::Usb::V2_0::UsbPipe &usbPipe_ = reinterpret_cast<const HDI::Usb::V2_0::UsbPipe &>(pipe);
    return usbHostInterface_->BulkCancel(usbDev_, usbPipe_);
#else
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    return usbd_->BulkCancel(devInfo, pipe);
#endif // USB_MANAGER_PASS_THROUGH
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

static std::string BcdToString(const std::vector<uint8_t> &bcd)
{
    std::string tstr;
    for (uint32_t i = 0; i < bcd.size(); ++i) {
        tstr += std::to_string((bcd[i] & BIT_HIGH_4) >> BIT_SHIFT_4);
        tstr += std::to_string((bcd[i] & BIT_LOW_4));
    }
    return tstr;
}

int32_t UsbHostManager::FillDevStrings(UsbDevice &dev)
{
    uint8_t busNum;
    uint8_t devAddr;
    uint8_t offsetValue = 8;

    busNum = dev.GetBusNum();
    devAddr = dev.GetDevAddr();
    uint16_t bcdUsb = dev.GetbcdUSB();
    const std::vector<uint8_t> bcdData {(bcdUsb & 0xff), ((bcdUsb >> offsetValue) & 0xff)};
    dev.SetVersion(BcdToString(bcdData));
    dev.SetManufacturerName(GetDevStringValFromIdx(busNum, devAddr, dev.GetiManufacturer()));
    dev.SetProductName(GetDevStringValFromIdx(busNum, devAddr, dev.GetiProduct()));
    dev.SetmSerial(GetDevStringValFromIdx(busNum, devAddr, dev.GetiSerialNumber()));
    USB_HILOGI(MODULE_USB_SERVICE,
        "iSerial:%{public}d Manufactur:%{public}s product:%{public}s "
        "version:%{public}s",
        dev.GetiSerialNumber(), dev.GetManufacturerName().c_str(), dev.GetProductName().c_str(),
        dev.GetVersion().c_str());

    std::vector<USBConfig> configs;
    configs = dev.GetConfigs();
    for (auto it = configs.begin(); it != configs.end(); ++it) {
        it->SetName(GetDevStringValFromIdx(busNum, devAddr, it->GetiConfiguration()));
        USB_HILOGI(MODULE_USB_SERVICE, "Config:%{public}d %{public}s", it->GetiConfiguration(), it->GetName().c_str());
        std::vector<UsbInterface> interfaces = it->GetInterfaces();
        for (auto itIF = interfaces.begin(); itIF != interfaces.end(); ++itIF) {
            itIF->SetName(GetDevStringValFromIdx(busNum, devAddr, itIF->GetiInterface()));
            USB_HILOGI(MODULE_USB_SERVICE, "interface:%{public}hhu %{public}s", itIF->GetiInterface(),
                itIF->GetName().c_str());
        }
        it->SetInterfaces(interfaces);
    }
    dev.SetConfigs(configs);

    return UEC_OK;
}

std::string UsbHostManager::GetDevStringValFromIdx(uint8_t busNum, uint8_t devAddr, uint8_t idx)
{
    const UsbDev dev = {busNum, devAddr};
    std::vector<uint8_t> strV;
    std::string strDesc = " ";

    if (idx == 0) {
        return strDesc;
    }

    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::usbd_ is nullptr");
        return nullptr;
    }
    int32_t ret = usbd_->GetStringDescriptor(dev, idx, strV);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "get string[%{public}hhu] failed ret:%{public}d", idx, ret);
        return strDesc;
    }
    size_t length = strV.size();
    if ((length < DESCRIPTOR_VALUE_START_OFFSET) || (strV[1] != DESCRIPTOR_TYPE_STRING)) {
        USB_HILOGI(MODULE_USB_SERVICE, "type or length error, len:%{public}zu", length);
        return strDesc;
    }

    uint16_t *tbuf = new (std::nothrow) uint16_t[length + 1]();
    if (tbuf == nullptr) {
        USB_HILOGI(MODULE_USB_SERVICE, "new failed");
        return strDesc;
    }

    for (uint32_t i = 0; i < length - DESCRIPTOR_VALUE_START_OFFSET; ++i) {
        tbuf[i] = strV[i + DESCRIPTOR_VALUE_START_OFFSET];
    }
    size_t bufLen = (length - DESCRIPTOR_VALUE_START_OFFSET) / HALF;
    size_t wstrLen = wcslen((wchar_t*)tbuf) <= bufLen ? wcslen((wchar_t*)tbuf) : bufLen;
    std::wstring wstr(reinterpret_cast<wchar_t *>(tbuf), wstrLen);
    strDesc = std::string(wstr.begin(), wstr.end());
    USB_HILOGI(MODULE_USB_SERVICE, "getString idx:%{public}d length:%{public}zu, str: %{public}s",
        idx, strDesc.length(), strDesc.c_str());
    delete[] tbuf;
    return strDesc;
}

bool UsbHostManager::IsEdmEnabled()
{
    std::string edmParaValue = OHOS::system::GetParameter("persist.edm.edm_enable", "false");
    USB_HILOGI(MODULE_USB_SERVICE, "edmParaValue is %{public}s", edmParaValue.c_str());
    return edmParaValue == "true";
}

int32_t UsbHostManager::ExecuteManageDevicePolicy(std::vector<UsbDeviceId> &whiteList)
{
    int32_t ret = UEC_INTERFACE_NO_MEMORY;
    USB_HILOGI(MODULE_USB_SERVICE, "list size %{public}zu", devices_.size());
    for (auto it = devices_.begin(); it != devices_.end(); ++it) {
        bool inWhiteList = false;
        for (auto dev : whiteList) {
            if (it->second->GetProductId() == dev.productId && it->second->GetVendorId() == dev.vendorId) {
                inWhiteList = true;
                break;
            }
        }
        if (inWhiteList) {
            ret = ManageDeviceImpl(it->second->GetVendorId(), it->second->GetProductId(), false);
        } else {
            ret = ManageDeviceImpl(it->second->GetVendorId(), it->second->GetProductId(), true);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(MANAGE_INTERFACE_INTERVAL));
    }
    if (ret != UEC_OK) {
        USB_HILOGI(MODULE_USB_SERVICE, "ManageDevice failed");
        return UEC_SERVICE_EXECUTE_POLICY_FAILED;
    }
    return UEC_OK;
}

int32_t UsbHostManager::ExecuteManageInterfaceType(const std::vector<UsbDeviceType> &disableType, bool disable)
{
    for (auto it = devices_.begin(); it != devices_.end(); ++it) {
        UsbDev dev = {it->second->GetBusNum(), it->second->GetDevAddr()};
        int32_t ret = OpenDevice(dev.busNum, dev.devAddr);
        if (ret != UEC_OK) {
            USB_HILOGW(MODULE_USB_SERVICE, "ExecuteManageInterfaceType open fail ret = %{public}d", ret);
        }
    }
    for (const auto &dev : disableType) {
        if (!dev.isDeviceType) {
            ExecuteManageDeviceType(disableType, disable, g_typeMap, false);
        } else {
            ExecuteManageDeviceType(disableType, disable, d_typeMap, true);
        }
    }
    for (auto it = devices_.begin(); it != devices_.end(); ++it) {
        UsbDev dev = {it->second->GetBusNum(), it->second->GetDevAddr()};
        int32_t ret = Close(dev.busNum, dev.devAddr);
        if (ret != UEC_OK) {
            USB_HILOGW(MODULE_USB_SERVICE, "ExecuteManageInterfaceType close fail ret = %{public}d", ret);
        }
    }
    return UEC_OK;
}

int32_t UsbHostManager::GetEdmPolicy(bool &IsGlobalDisabled, std::vector<UsbDeviceType> &disableType,
    std::vector<UsbDeviceId> &trustUsbDeviceIds)
{
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "fail to get SystemAbilityManager");
        return UEC_SERVICE_GET_SYSTEM_ABILITY_MANAGER_FAILED;
    }
    sptr<IRemoteObject> remote = sm->CheckSystemAbility(EDM_SYSTEM_ABILITY_ID);
    if (remote == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Get Edm SystemAbility failed.");
        return UEC_SERVICE_GET_EDM_SERVICE_FAILED;
    }
    int32_t ret = GetEdmGlobalPolicy(remote, IsGlobalDisabled);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetEdmGlobalPolicy failed.");
        return ret;
    }
    ret = GetEdmStroageTypePolicy(remote, disableType);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetEdmStroageTypePolicy failed.");
        return ret;
    }
    ret = GetEdmTypePolicy(remote, disableType);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetEdmTypePolicy failed.");
        return ret;
    }
    ret = GetEdmWhiteListPolicy(remote, trustUsbDeviceIds);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetEdmWhiteListPolicy failed.");
        return ret;
    }
    return UEC_OK;
}

int32_t UsbHostManager::GetUsbPolicy(bool &IsGlobalDisabled, std::vector<UsbDeviceType> &disableType,
    std::vector<UsbDeviceId> &trustUsbDeviceIds)
{
    auto startTime = std::chrono::steady_clock::now();
    bool isPolicyObtained = false;
    while (!isPolicyObtained) {
        int32_t ret = GetEdmPolicy(IsGlobalDisabled, disableType, trustUsbDeviceIds);
        if (ret == UEC_OK) {
            USB_HILOGI(MODULE_USB_SERVICE, "GetUsbPolicy succeed");
            break;
        } else if (ret == EDM_SA_TIME_OUT_CODE) {
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
            if (elapsedTime >= EDM_SA_MAX_TIME_OUT) {
                USB_HILOGE(MODULE_USB_SERVICE, "Time out, exit loop");
                return UEC_SERVICE_EDM_SA_TIME_OUT_FAILED;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(MANAGE_INTERFACE_INTERVAL));
        } else {
            USB_HILOGE(MODULE_USB_SERVICE, "EDM sa failed");
            return UEC_SERVICE_PREPARE_EDM_SA_FAILED;
        }
    }
    return UEC_OK;
}

int32_t UsbHostManager::GetEdmTypePolicy(sptr<IRemoteObject> remote, std::vector<UsbDeviceType> &disableType)
{
    if (remote == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Remote is nullpter.");
        return UEC_SERVICE_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(WITHOUT_USERID);
    data.WriteString("");
    data.WriteInt32(WITHOUT_ADMIN);
    uint32_t funcCode = (1 << EMD_MASK_CODE) | USB_DEVICE_ACCESS_POLICY;
    int32_t ErrCode = remote->SendRequest(funcCode, data, reply, option);
    int32_t ret = ERR_INVALID_VALUE;
    bool isSuccess = reply.ReadInt32(ret) && (ret == ERR_OK);
    if (!isSuccess) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetEdmTypePolicy failed. ErrCode =  %{public}d, ret = %{public}d",
            ErrCode, ret);
        return UEC_SERVICE_EDM_SEND_REQUEST_FAILED;
    }

    int32_t size = reply.ReadInt32();
    if (size > WHITELIST_POLICY_MAX_DEVICES) {
        USB_HILOGE(MODULE_USB_SERVICE, "EdmTypeList size=[%{public}d] is too large", size);
        return UEC_SERVICE_EDM_DEVICE_SIZE_EXCEED;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "GetEdmTypePolicy return size:%{public}d", size);
    for (int32_t i = 0; i < size; i++) {
        UsbDeviceType usbDeviceType;
        usbDeviceType.baseClass = reply.ReadInt32();
        usbDeviceType.subClass = reply.ReadInt32();
        usbDeviceType.protocol = reply.ReadInt32();
        usbDeviceType.isDeviceType = reply.ReadBool();
        disableType.emplace_back(usbDeviceType);
    }
    return UEC_OK;
}

int32_t UsbHostManager::GetEdmGlobalPolicy(sptr<IRemoteObject> remote, bool &IsGlobalDisabled)
{
    if (remote == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Remote is nullpter.");
        return UEC_SERVICE_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(WITHOUT_USERID);
    data.WriteString("");
    data.WriteInt32(WITHOUT_ADMIN);
    uint32_t funcCode = (1 << EMD_MASK_CODE) | DISABLE_USB;
    int32_t ErrCode = remote->SendRequest(funcCode, data, reply, option);
    int32_t ret = ERR_INVALID_VALUE;
    bool isSuccess = reply.ReadInt32(ret) && (ret == UEC_OK);
    if (!isSuccess) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetGlobalPolicy failed. ErrCode =  %{public}d, ret = %{public}d",
            ErrCode, ret);
        return UEC_SERVICE_EDM_SEND_REQUEST_FAILED;
    }

    reply.ReadBool(IsGlobalDisabled);
    return UEC_OK;
}

int32_t UsbHostManager::GetEdmStroageTypePolicy(sptr<IRemoteObject> remote, std::vector<UsbDeviceType> &disableType)
{
    if (remote == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Remote is nullpter.");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t stroageDisableType = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(WITHOUT_USERID);
    data.WriteString("");
    data.WriteInt32(WITHOUT_ADMIN);
    uint32_t funcCode = (1 << EMD_MASK_CODE) | USB_STORAGE_DEVICE_ACCESS_POLICY;
    int32_t ErrCode = remote->SendRequest(funcCode, data, reply, option);
    int32_t ret = ERR_INVALID_VALUE;
    bool isSuccess = reply.ReadInt32(ret) && (ret == ERR_OK);
    if (!isSuccess) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetEdmStroageTypePolicy failed. ErrCode =  %{public}d, ret = %{public}d",
            ErrCode, ret);
        return UEC_SERVICE_EDM_SEND_REQUEST_FAILED;
    }

    reply.ReadInt32(stroageDisableType);
    if (stroageDisableType == GET_EDM_STORAGE_DISABLE_TYPE) {
        UsbDeviceType usbDeviceType;
        usbDeviceType.baseClass = STORAGE_BASE_CLASS;
        usbDeviceType.isDeviceType = 0;
        disableType.emplace_back(usbDeviceType);
    }
    return UEC_OK;
}

int32_t UsbHostManager::GetEdmWhiteListPolicy(sptr<IRemoteObject> remote, std::vector<UsbDeviceId> &trustUsbDeviceIds)
{
    if (remote == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Remote is nullpter.");
        return UEC_SERVICE_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(WITHOUT_USERID);
    data.WriteString("");
    data.WriteInt32(WITHOUT_ADMIN);
    uint32_t funcCode = (1 << EMD_MASK_CODE) | ALLOWED_USB_DEVICES;
    int32_t ErrCode = remote->SendRequest(funcCode, data, reply, option);
    int32_t ret = ERR_INVALID_VALUE;
    bool IsSuccess = reply.ReadInt32(ret) && (ret == ERR_OK);
    if (!IsSuccess) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetEdmWhiteListPolicy failed. ErrCode =  %{public}d, ret = %{public}d",
            ErrCode, ret);
        return UEC_SERVICE_EDM_SEND_REQUEST_FAILED;
    }

    int32_t size = reply.ReadInt32();
    if (size > WHITELIST_POLICY_MAX_DEVICES) {
        USB_HILOGE(MODULE_USB_SERVICE, "EdmWhiteList size=[%{public}d] is too large", size);
        return UEC_SERVICE_EDM_DEVICE_SIZE_EXCEED;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "GetEdmWhiteListPolicy return size:%{public}d", size);
    for (int32_t i = 0; i < size; i++) {
        UsbDeviceId usbDeviceId;
        usbDeviceId.vendorId = reply.ReadInt32();
        usbDeviceId.productId = reply.ReadInt32();
        trustUsbDeviceIds.emplace_back(usbDeviceId);
    }
    return UEC_OK;
}

int32_t UsbHostManager::ManageInterface(const HDI::Usb::V1_0::UsbDev &dev, uint8_t interfaceId, bool disable)
{
#ifdef USB_MANAGER_PASS_THROUGH
    if (usbHostInterface_ == nullptr) {
        USB_HILOGE(MODULE_SERVICE, "UsbHostManager::ManageInterface usbHostInterface_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    const HDI::Usb::V2_0::UsbDev &usbDev_ = reinterpret_cast<const HDI::Usb::V2_0::UsbDev &>(dev);
    return usbHostInterface_->ManageInterface(usbDev_, interfaceId, disable);
#else
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->ManageInterface(dev, interfaceId, disable);
#endif
}

void UsbHostManager::ExecuteManageDeviceType(const std::vector<UsbDeviceType> &disableType, bool disable,
    const std::unordered_map<InterfaceType, std::vector<int32_t>> &map, bool isDev)
{
    std::vector<InterfaceType> interfaceTypes;
    for (const auto &dev : disableType) {
        bool isMatch = false;
        for (auto& [interfaceTypeValues, typeValues] : map) {
            if ((typeValues[0] == dev.baseClass) &&
                (typeValues[1] == -1 || typeValues[1] == dev.subClass) &&
                (typeValues[HALF] == -1 || typeValues[HALF] == dev.protocol)) {
                    isMatch = true;
                    interfaceTypes.emplace_back(interfaceTypeValues);
                    break;
            }
        }
        if (!isMatch) {
            USB_HILOGE(MODULE_USB_SERVICE, "is not in the type list, %{public}d, %{public}d, %{public}d",
                dev.baseClass, dev.subClass, dev.protocol);
        }
    }

    for (auto& [interfaceTypeValues, typeValues] : map) {
        bool canFind = false;
        for (auto disallowedValues : interfaceTypes) {
            if (interfaceTypeValues == disallowedValues) {
                canFind = true;
                break;
            }
        }
        if ((!isDev) && canFind) {
            ManageInterfaceTypeImpl(interfaceTypeValues, disable);
        }
        if ((!isDev) && (!canFind)) {
            ManageInterfaceTypeImpl(interfaceTypeValues, !disable);
        }
        if (isDev && canFind) {
            ManageDeviceTypeImpl(interfaceTypeValues, disable);
        }
        if (isDev && !canFind) {
            ManageDeviceTypeImpl(interfaceTypeValues, !disable);
        }
    }
}

int32_t UsbHostManager::ManageGlobalInterfaceImpl(bool disable)
{
    USB_HILOGI(MODULE_USB_SERVICE, "list size %{public}zu", devices_.size());
    for (auto it = devices_.begin(); it != devices_.end(); ++it) {
        UsbDev dev = {it->second->GetBusNum(), it->second->GetDevAddr()};
        uint8_t configIndex = 0;
        int32_t ret = OpenDevice(dev.busNum, dev.devAddr);
        if (ret != UEC_OK) {
            USB_HILOGW(MODULE_USB_SERVICE, "ManageGlobalInterfaceImpl open fail ret = %{public}d", ret);
            return ret;
        }
        if (GetActiveConfig(dev.busNum, dev.devAddr, configIndex)) {
            USB_HILOGW(MODULE_USB_SERVICE, "get device active config failed.");
            continue;
        }
        USBConfig configs;
        if (it->second->GetConfig(static_cast<uint8_t>(configIndex) - 1, configs)) {
            USB_HILOGW(MODULE_USB_SERVICE, "get device config info failed.");
            continue;
        }

        std::vector<UsbInterface> interfaces = configs.GetInterfaces();
        for (uint32_t i = 0; i < interfaces.size(); i++) {
            ManageInterface(dev, interfaces[i].GetId(), disable);
            std::this_thread::sleep_for(std::chrono::milliseconds(MANAGE_INTERFACE_INTERVAL));
        }
        ret = Close(dev.busNum, dev.devAddr);
        if (ret != UEC_OK) {
            USB_HILOGW(MODULE_USB_SERVICE, "ManageGlobalInterfaceImpl close fail ret = %{public}d", ret);
            return ret;
        }
    }
    return UEC_OK;
}

int32_t UsbHostManager::ManageDeviceImpl(int32_t vendorId, int32_t productId, bool disable)
{
    USB_HILOGI(MODULE_USB_SERVICE, "list size %{public}zu, vId: %{public}d, pId: %{public}d, b: %{public}d",
        devices_.size(), vendorId, productId, disable);
    for (auto it = devices_.begin(); it != devices_.end(); ++it) {
        if ((it->second->GetVendorId() == vendorId) && (it->second->GetProductId() == productId)) {
            UsbDev dev = {it->second->GetBusNum(), it->second->GetDevAddr()};
            uint8_t configIndex = 0;
            int32_t ret = OpenDevice(dev.busNum, dev.devAddr);
            if (ret != UEC_OK) {
                USB_HILOGW(MODULE_USB_SERVICE, "ManageGlobalInterfaceImpl open fail ret = %{public}d", ret);
                return ret;
            }
            if (GetActiveConfig(dev.busNum, dev.devAddr, configIndex)) {
                USB_HILOGW(MODULE_USB_SERVICE, "get device active config failed.");
                continue;
            }
            USBConfig configs;
            if (it->second->GetConfig(static_cast<uint8_t>(configIndex) - 1, configs)) {
                USB_HILOGW(MODULE_USB_SERVICE, "get device config info failed.");
                continue;
            }
            std::vector<UsbInterface> interfaces = configs.GetInterfaces();
            for (uint32_t i = 0; i < interfaces.size(); i++) {
                ManageInterface(dev, interfaces[i].GetId(), disable);
                std::this_thread::sleep_for(std::chrono::milliseconds(MANAGE_INTERFACE_INTERVAL));
            }
            ret = Close(dev.busNum, dev.devAddr);
            if (ret != UEC_OK) {
                USB_HILOGW(MODULE_USB_SERVICE, "ManageGlobalInterfaceImpl close fail ret = %{public}d", ret);
                return ret;
            }
        }
    }
    return UEC_OK;
}

int32_t UsbHostManager::ManageInterfaceTypeImpl(InterfaceType interfaceType, bool disable)
{
    auto iterInterface = g_typeMap.find(interfaceType);
    if (iterInterface == g_typeMap.end()) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::not find interface type");
        return UEC_SERVICE_INVALID_VALUE;
    }
    for (auto it = devices_.begin(); it != devices_.end(); ++it) {
        UsbDev dev = {it->second->GetBusNum(), it->second->GetDevAddr()};
        uint8_t configIndex = 0;
        if (GetActiveConfig(dev.busNum, dev.devAddr, configIndex)) {
            USB_HILOGW(MODULE_USB_SERVICE, "get device active config failed.");
            continue;
        }
        USBConfig configs;
        if (it->second->GetConfig(static_cast<uint8_t>(configIndex) - 1, configs)) {
            USB_HILOGW(MODULE_USB_SERVICE, "get device config info failed.");
            continue;
        }
        std::vector<UsbInterface> interfaces = configs.GetInterfaces();

        for (uint32_t i = 0; i < interfaces.size(); i++) {
            // 0 indicate base class, 1 indicate subclass, 2 indicate protocal. -1 indicate any value.
            if ((interfaces[i].GetClass() == iterInterface->second[BASECLASS_INDEX]) && (interfaces[i].GetSubClass() ==
                iterInterface->second[SUBCLASS_INDEX] || iterInterface->second[SUBCLASS_INDEX] ==
                RANDOM_VALUE_INDICATE) && (interfaces[i].GetProtocol() == iterInterface->second[PROTOCAL_INDEX] ||
                iterInterface->second[PROTOCAL_INDEX] == RANDOM_VALUE_INDICATE)) {
                    ManageInterface(dev, interfaces[i].GetId(), disable);
                    USB_HILOGI(MODULE_USB_SERVICE, "size %{public}zu, interfaceType: %{public}d, disable: %{public}d",
                        devices_.size(), static_cast<int32_t>(interfaceType), disable);
                    std::this_thread::sleep_for(std::chrono::milliseconds(MANAGE_INTERFACE_INTERVAL));
            }
        }
    }
    return UEC_OK;
}

int32_t UsbHostManager::ManageDeviceTypeImpl(InterfaceType interfaceType, bool disable)
{
    auto iterInterface = g_typeMap.find(interfaceType);
    if (iterInterface == g_typeMap.end()) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbHostManager::not find interface type");
        return UEC_SERVICE_INVALID_VALUE;
    }
    for (auto it = devices_.begin(); it != devices_.end(); ++it) {
        if ((it->second->GetClass() == iterInterface->second[BASECLASS_INDEX]) && (it->second->GetSubclass() ==
            iterInterface->second[SUBCLASS_INDEX] || iterInterface->second[SUBCLASS_INDEX] ==
            RANDOM_VALUE_INDICATE) && (it->second->GetProtocol() == iterInterface->second[PROTOCAL_INDEX] ||
            iterInterface->second[PROTOCAL_INDEX] == RANDOM_VALUE_INDICATE)) {
                ManageDeviceImpl(it->second->GetVendorId(), it->second->GetProductId(), disable);
                USB_HILOGI(MODULE_USB_SERVICE, "list size %{public}zu, interfaceType: %{public}d, disable: %{public}d",
                    devices_.size(), static_cast<int32_t>(interfaceType), disable);
                std::this_thread::sleep_for(std::chrono::milliseconds(MANAGE_INTERFACE_INTERVAL));
        }
    }
    return UEC_OK;
}
} // namespace USB
} // namespace OHOS
