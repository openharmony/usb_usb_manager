/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef USBMGR_USB_SERVICE_SUBSCRIBER_H
#define USBMGR_USB_SERVICE_SUBSCRIBER_H

#include "v1_0/iusbd_subscriber.h"

namespace OHOS {
namespace USB {
class UsbServiceSubscriber : public OHOS::HDI::Usb::V1_0::IUsbdSubscriber {
public:
    UsbServiceSubscriber();
    ~UsbServiceSubscriber() = default;
    int32_t DeviceEvent(const HDI::Usb::V1_0::USBDeviceInfo &info) override;
    int32_t PortChangedEvent(const HDI::Usb::V1_0::PortInfo &info) override;
};
} // namespace USB
} // namespace OHOS
#endif // USBMGR_USB_SERVICE_SUBSCRIBER_H
