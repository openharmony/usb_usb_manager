/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef USB_PORT_H
#define USB_PORT_H

#include <string>
#include <vector>
#include "parcel.h"
#include "usb_common.h"

namespace OHOS {
namespace USB {
struct UsbPortStatus {
    int32_t currentMode;
    int32_t currentPowerRole;
    int32_t currentDataRole;
};
struct UsbPort : public Parcelable {
    int32_t id;
    int32_t supportedModes;
    UsbPortStatus usbPortStatus;

    bool Marshalling(Parcel &parcel) const override
    {
        WRITE_PARCEL_AND_RETURN_FALSE_WHEN_FAIL(Int32, parcel, this->id);
        WRITE_PARCEL_AND_RETURN_FALSE_WHEN_FAIL(Int32, parcel, this->supportedModes);
        WRITE_PARCEL_AND_RETURN_FALSE_WHEN_FAIL(Int32, parcel, this->usbPortStatus.currentMode);
        WRITE_PARCEL_AND_RETURN_FALSE_WHEN_FAIL(Int32, parcel, this->usbPortStatus.currentPowerRole);
        WRITE_PARCEL_AND_RETURN_FALSE_WHEN_FAIL(Int32, parcel, this->usbPortStatus.currentDataRole);
        return true;
    }

    static UsbPort *Unmarshalling(Parcel &data)
    {
        UsbPort *usbPort = new (std::nothrow) UsbPort;
        if (usbPort == nullptr) {
            return nullptr;
        }
        usbPort->id = data.ReadInt32();
        usbPort->supportedModes = data.ReadInt32();
        usbPort->usbPortStatus.currentMode = data.ReadInt32();
        usbPort->usbPortStatus.currentPowerRole = data.ReadInt32();
        usbPort->usbPortStatus.currentDataRole = data.ReadInt32();
        return usbPort;
    }
};
} // namespace USB
} // namespace OHOS
#endif // USB_PORT_H
