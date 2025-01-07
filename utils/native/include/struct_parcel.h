/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef USB_STRUCT_PARCEL_H
#define USB_STRUCT_PARCEL_H

#include "parcel.h"
#include "v1_2/usb_types.h"

namespace OHOS {
namespace USB {

struct UsbIsoParcel final : public Parcelable {
    UsbIsoParcel() = default;
    ~UsbIsoParcel() = default;
    bool Marshalling(Parcel &out) const override;
    static UsbIsoParcel *Unmarshalling(Parcel &in);
    HDI::Usb::V1_2::UsbIsoPacketDescriptor isoInfo;
};

struct UsbIsoVecParcel final : public Parcelable {
    UsbIsoVecParcel() = default;
    ~UsbIsoVecParcel() = default;
    bool Marshalling(Parcel &out) const override;
    static UsbIsoVecParcel *Unmarshalling(Parcel &in);
    std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> isoInfoVec;
};

} // namespace USB
} // namespace OHOS
#endif // USB_STRUCT_PARCEL_H
