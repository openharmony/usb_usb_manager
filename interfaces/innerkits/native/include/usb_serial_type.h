/*
 * Copyright 2025 Huawei Device Co., Ltd.
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

#ifndef USB_SERIAL_TYPE_H
#define USB_SERIAL_TYPE_H
#include <string>
#include "parcel.h"
#include "usb_common.h"

namespace OHOS {
namespace USB {
struct UsbSerialPort : public Parcelable {
    int portId_;
    uint8_t busNum_;
    uint8_t devAddr_;
    int vid_;
    int pid_;
    std::string serialNum_;

    bool Marshalling(Parcel &parcel) const override
    {
        WRITE_PARCEL_AND_RETURN_FALSE_WHEN_FAIL(Int32, parcel, this->portId_);
        WRITE_PARCEL_AND_RETURN_FALSE_WHEN_FAIL(Uint8, parcel, this->busNum_);
        WRITE_PARCEL_AND_RETURN_FALSE_WHEN_FAIL(Uint8, parcel, this->devAddr_);
        WRITE_PARCEL_AND_RETURN_FALSE_WHEN_FAIL(Int32, parcel, this->vid_);
        WRITE_PARCEL_AND_RETURN_FALSE_WHEN_FAIL(Int32, parcel, this->pid_);
        WRITE_PARCEL_AND_RETURN_FALSE_WHEN_FAIL(String16, parcel, Str8ToStr16(this->serialNum_));
        return true;
    }

    static UsbSerialPort *Unmarshalling(Parcel &data)
    {
        UsbSerialPort *serialPort = new (std::nothrow) UsbSerialPort;
        if (serialPort == nullptr) {
            return nullptr;
        }

        serialPort->portId_ = data.ReadInt32();
        serialPort->busNum_ = data.ReadUint8();
        serialPort->devAddr_ = data.ReadUint8();
        serialPort->vid_ = data.ReadInt32();
        serialPort->pid_ = data.ReadInt32();
        serialPort->serialNum_ = Str16ToStr8(data.ReadString16());
        return serialPort;
    }
};

struct UsbSerialAttr : public Parcelable {
    uint32_t baudRate_;
    uint8_t stopBits_;
    uint8_t parity_;
    uint8_t dataBits_;

    bool Marshalling(Parcel &parcel) const override
    {
        WRITE_PARCEL_AND_RETURN_FALSE_WHEN_FAIL(Uint32, parcel, this->baudRate_);
        WRITE_PARCEL_AND_RETURN_FALSE_WHEN_FAIL(Uint8, parcel, this->stopBits_);
        WRITE_PARCEL_AND_RETURN_FALSE_WHEN_FAIL(Uint8, parcel, this->parity_);
        WRITE_PARCEL_AND_RETURN_FALSE_WHEN_FAIL(Uint8, parcel, this->dataBits_);
        return true;
    }

    static UsbSerialAttr *Unmarshalling(Parcel &data)
    {
        UsbSerialAttr *usbSerialAttr = new (std::nothrow) UsbSerialAttr;
        if (usbSerialAttr == nullptr) {
            return nullptr;
        }

        usbSerialAttr->baudRate_ = data.ReadUint32();
        usbSerialAttr->stopBits_ = data.ReadUint8();
        usbSerialAttr->parity_ = data.ReadUint8();
        usbSerialAttr->dataBits_ = data.ReadUint8();
        return usbSerialAttr;
    }
};

}
}
#endif //USB_SERIAL_TYPE_H