/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef USBMGR_INNERKITS_IUSB_SRV_H
#define USBMGR_INNERKITS_IUSB_SRV_H

#include "iremote_broker.h"
#include "iremote_object.h"
#include "usb_device.h"
#include "usb_port.h"
#include "v1_1/usb_types.h"
#include "v1_2/usb_types.h"
#include "serial/v1_0/serial_types.h"
#include "usb_interface_type.h"
#include "usb_accessory.h"

namespace OHOS {
namespace USB {

class TransferCallbackInfo {
public:
    int32_t status;
    int32_t actualLength;
};

using TransferCallback = std::function<void(const TransferCallbackInfo &,
    const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &, uint64_t)>;

} // namespace USB
} // namespace OHOS

#endif // USBMGR_INNERKITS_IUSB_SRV_H
