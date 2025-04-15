/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef USB_REPORT_SYS_EVENT_H
#define USB_REPORT_SYS_EVENT_H

#include <string>
#include "v1_0/usb_types.h"
#include "usb_interface.h"
#include "usb_device.h"
#include "usb_host_manager.h"
#include "usb_endpoint.h"

namespace OHOS {
namespace USB {
typedef std::map<std::string, UsbDevice *> MAP_STR_DEVICE;
class UsbReportSysEvent {
public:
    static void ReportTransferFaultSysEvent(const std::string transferType,
        const HDI::Usb::V1_0::UsbDev &tmpDev, const HDI::Usb::V1_0::UsbPipe &tmpPipe,
        int32_t ret, const std::string description, MAP_STR_DEVICE &devices);
    static void CheckAttributeReportTransferFaultSysEvent(const std::string interfaceName,
        const HDI::Usb::V1_0::UsbDev &tmpDev, const HDI::Usb::V1_0::UsbPipe &tmpPipe, const USBEndpoint &ep,
        int32_t ret, const std::string description, MAP_STR_DEVICE &devices);
    static bool GetUsbInterfaceId(const HDI::Usb::V1_0::UsbDev &tmpDev, const HDI::Usb::V1_0::UsbPipe &tmpPipe,
        int32_t interfaceId, MAP_STR_DEVICE &devices, UsbInterface &itIF, UsbDevice &dev);
};

} // namespace USB
} // namespace OHOS

#endif //USB_REPORT_SYS_EVENT_H