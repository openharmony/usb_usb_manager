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

#ifndef USB_BULK_RAWDATA_H
#define USB_BULK_RAWDATA_H
#include <vector>
#include "parcel.h"
#include "usb_common.h"
namespace OHOS {
namespace USB {

class UsbBulkTransData : public Parcelable {
public:
    UsbBulkTransData(std::vector<uint8_t> &buffer)
    {
        data_ = buffer;
    }

    UsbBulkTransData() {};

    bool Marshalling(Parcel &parcel) const override
    {
        uint32_t length = data_.size();
        const uint8_t *ptr = data_.data();
        if (!ptr) {
            length = 0;
        }

        if (!parcel.WriteUint32(length)) {
            USB_HILOGE(MODULE_USB_INNERKIT, "write bulktransfer length failed:%{public}u", length);
            return false;
        }
        if ((ptr) && (length > 0) && !parcel.WriteBuffer(reinterpret_cast<const void *>(ptr), length)) {
            USB_HILOGE(MODULE_USB_INNERKIT, "write bulktransfer buffer failed length:%{public}u", length);
            return false;
        }

        USB_HILOGI(MODULE_USB_INNERKIT, "success bulktransfer length:%{public}u", length);
        return true;
    }

    static UsbBulkTransData *Unmarshalling(Parcel &parcel)
    {
        UsbBulkTransData *pBufferData = new (std::nothrow) UsbBulkTransData;
        if (pBufferData == nullptr) {
            return nullptr;
        }

        uint32_t dataSize = 0;
        if (!parcel.ReadUint32(dataSize)) {
            USB_HILOGE(MODULE_USB_INNERKIT, "read bulktransfer dataSize failed");
            return pBufferData;
        }
        if (dataSize == 0) {
            USB_HILOGI(MODULE_USB_INNERKIT, "bulktransfer invalid size:%{public}u", dataSize);
            return pBufferData;
        }
        const uint8_t *readData = parcel.ReadUnpadBuffer(dataSize);
        if (readData == nullptr) {
            USB_HILOGE(MODULE_USB_INNERKIT, "bulktransfer failed size:%{public}u", dataSize);
            return pBufferData;
        }
        std::vector<uint8_t> tdata(readData, readData + dataSize);
        pBufferData->data_.swap(tdata);
        return pBufferData;
    }

    std::vector<uint8_t> data_;
};

} // namespace USB
} // namespace OHOS

#endif // USB_BULK_RAWDATA_H