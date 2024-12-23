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

#include "struct_parcel.h"

namespace OHOS {
namespace USB {

bool UsbIsoParcel::Marshalling(Parcel &out) const
{
    if (!out.WriteInt32(isoInfo.isoLength)) {
        return false;
    }
    if (!out.WriteInt32(isoInfo.isoActualLength)) {
        return false;
    }
    if (!out.WriteInt32(isoInfo.isoStatus)) {
        return false;
    }

    return true;
}

UsbIsoParcel *UsbIsoParcel::Unmarshalling(Parcel &in)
{
    auto *usbIsoParcel = new (std::nothrow) UsbIsoParcel();
    if (usbIsoParcel == nullptr) {
        return nullptr;
    }

    usbIsoParcel->isoInfo.isoLength = in.ReadInt32();
    usbIsoParcel->isoInfo.isoActualLength = in.ReadInt32();
    usbIsoParcel->isoInfo.isoStatus = in.ReadInt32();
    return usbIsoParcel;
}

bool UsbIsoVecParcel::Marshalling(Parcel &out) const
{
    const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> isoInfoVec = this->isoInfoVec;
    uint32_t vecSize = isoInfoVec.size();
    if (!out.WriteUint32(vecSize)) {
        return false;
    }

    for (uint32_t index = 0; index < vecSize; index++) {
        sptr<UsbIsoParcel> usbIsoParcel = new (std::nothrow) UsbIsoParcel();
        if (usbIsoParcel == nullptr) {
            return false;
        }
        
        usbIsoParcel->isoInfo = isoInfoVec.at(index);
        if (!out.WriteParcelable(usbIsoParcel)) {
            delete (usbIsoParcel);
            usbIsoParcel = nullptr;
            return false;
        }
    }
    return true;
}

UsbIsoVecParcel *UsbIsoVecParcel::Unmarshalling(Parcel &in)
{
    UsbIsoVecParcel *usbIsoVecParcel = new (std::nothrow) UsbIsoVecParcel();
    if (usbIsoVecParcel == nullptr) {
        return nullptr;
    }

    uint32_t vecSize;
    if (!in.ReadUint32(vecSize)) {
        delete (usbIsoVecParcel);
        usbIsoVecParcel = nullptr;
        return nullptr;
    }

    for (uint32_t index = 0; index < vecSize; index++) {
        sptr<UsbIsoParcel> usbIsoParcel = in.ReadParcelable<UsbIsoParcel>();
        if (usbIsoParcel == nullptr) {
            return nullptr;
        }
        usbIsoVecParcel->isoInfoVec.emplace_back(usbIsoParcel->isoInfo);
    }
    return usbIsoVecParcel;
}

} // namespace USB
} // namespace OHOS
