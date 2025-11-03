/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "usb_report_sys_event.h"

#include "hilog_wrapper.h"
#include "usb_config.h"
#include "usb_interface.h"
#include "usb_device.h"
#include "usb_host_manager.h"
#include "usb_endpoint.h"
#include "usb_errors.h"
#include "hisysevent.h"

#ifdef USB_PERIPHERAL_FAULT_NOTIFY
#include "driver_ext_mgr_client.h"
#endif

using namespace OHOS::HiviewDFX;

namespace OHOS {
namespace USB {
constexpr int32_t ERR_CODE_TIMEOUT = -7;

void UsbReportSysEvent::ReportTransferFaultSysEvent(const std::string transferType,
    const HDI::Usb::V1_0::UsbDev &tmpDev, const HDI::Usb::V1_0::UsbPipe &tmpPipe,
    int32_t ret, const std::string description, MAP_STR_DEVICE &devices)
{
    UsbInterface itIF;
    UsbDevice dev;
    USB_HILOGI(MODULE_USBD, "report transfor fault sys event");
    if (!GetUsbInterfaceId(tmpDev, tmpPipe, tmpPipe.intfId, devices, itIF, dev)) {
        USB_HILOGE(MODULE_SERVICE, "GetUsbConfigs failed");
        return;
    }
    int32_t hiRet = HiSysEventWrite(HiSysEvent::Domain::USB, "TRANSFER_FAULT",
        HiSysEvent::EventType::FAULT, "TRANSFER_TYPE", transferType,
        "VENDOR_ID", dev.GetVendorId(), "PRODUCT_ID", dev.GetProductId(),
        "INTERFACE_CLASS", itIF.GetClass(), "INTERFACE_SUBCLASS", itIF.GetSubClass(),
        "INTERFACE_PROTOCOL", itIF.GetProtocol(),
        "INFT_ID", tmpPipe.intfId, "ENDPOINT_ID", tmpPipe.endpointId,
        "FAIL_REASON", ret, "FAIL_DESCRIPTION", description);
    if (hiRet != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "HiSysEventWrite ret: %{public}d", hiRet);
    }

#ifdef USB_PERIPHERAL_FAULT_NOTIFY
    ExternalDeviceManager::DriverExtMgrClient::GetInstance().NotifyUsbPeripheralFault(
        HiSysEvent::Domain::USB, "TRANSFOR_FAULT"); // "TRANSFOR" in external_device code
#endif
}

void UsbReportSysEvent::CheckAttributeReportTransferFaultSysEvent(const std::string transferType,
    const HDI::Usb::V1_0::UsbDev &tmpDev, const HDI::Usb::V1_0::UsbPipe &tmpPipe, const USBEndpoint &ep,
    int32_t ret, const std::string description, MAP_STR_DEVICE &devices)
{
    UsbInterface itIF;
    UsbDevice dev;
    USB_HILOGI(MODULE_USBD, "report transfor fault sys event");
    if (ep.GetAttributes() == 0x03 && ret == ERR_CODE_TIMEOUT) {
        USB_HILOGE(MODULE_SERVICE, "submitTransfer is timeout");
        return;
    }
    if (!GetUsbInterfaceId(tmpDev, tmpPipe, tmpPipe.intfId, devices, itIF, dev)) {
        USB_HILOGE(MODULE_SERVICE, "GetUsbConfigs failed");
        return;
    }
    int32_t hiRet = HiSysEventWrite(HiSysEvent::Domain::USB, "TRANSFER_FAULT",
        HiSysEvent::EventType::FAULT, "TRANSFER_TYPE", transferType,
        "VENDOR_ID", dev.GetVendorId(), "PRODUCT_ID", dev.GetProductId(),
        "INTERFACE_CLASS", itIF.GetClass(), "INTERFACE_SUBCLASS", itIF.GetSubClass(),
        "INTERFACE_PROTOCOL", itIF.GetProtocol(),
        "INFT_ID", tmpPipe.intfId, "ENDPOINT_ID", tmpPipe.endpointId,
        "FAIL_REASON", ret, "FAIL_DESCRIPTION", description);
    if (hiRet != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "HiSysEventWrite ret: %{public}d", hiRet);
    }

#ifdef USB_PERIPHERAL_FAULT_NOTIFY
    ExternalDeviceManager::DriverExtMgrClient::GetInstance().NotifyUsbPeripheralFault(
        HiSysEvent::Domain::USB, "TRANSFOR_FAULT"); // "TRANSFOR" in external_device code
#endif
}

bool UsbReportSysEvent::GetUsbInterfaceId(const HDI::Usb::V1_0::UsbDev &tmpDev, const HDI::Usb::V1_0::UsbPipe &tmpPipe,
    int32_t interfaceId, MAP_STR_DEVICE &devices, UsbInterface &itIF, UsbDevice &dev)
{
    std::string name = std::to_string(tmpDev.busNum) + "-" + std::to_string(tmpDev.devAddr);
    MAP_STR_DEVICE::iterator iter = devices.find(name);
    if (iter == devices.end()) {
        USB_HILOGE(MODULE_SERVICE, "name:%{public}s bus:%{public}hhu dev:%{public}hhu not exist", name.c_str(),
            tmpDev.busNum, tmpDev.devAddr);
        return false;
    }
    if (iter->second == nullptr) {
        USB_HILOGE(MODULE_SERVICE, "%{public}s: %{public}s device is nullptr.", __func__, name.c_str());
        return false;
    }
    dev = *(iter->second);

    if (tmpPipe.intfId == 0 && tmpPipe.endpointId == 0) {
        itIF.SetClass(dev.GetClass());
        itIF.SetSubClass(dev.GetSubclass());
        itIF.SetProtocol(dev.GetProtocol());
        return true;
    }

    auto configs = dev.GetConfigs();
    for (auto &config : configs) {
        std::vector<UsbInterface> interfaces = config.GetInterfaces();
        for (auto &interface : interfaces) {
            if (interface.GetId() == interfaceId) {
                itIF = interface;
                return true;
            }
        }
    }
    return false;
}
} // OHOS
} // USB