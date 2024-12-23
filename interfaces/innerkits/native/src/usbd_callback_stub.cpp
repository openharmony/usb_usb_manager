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

#include "usbd_callback_stub.h"

#include "hilog_wrapper.h"
#include "usb_errors.h"
#include "struct_parcel.h"

namespace OHOS::USB {
int32_t UsbdStubCallBack::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    switch (code) {
        case CMD_USBD_TRANSFER_CALLBACK_WRITE: {
            TransferWriteCallback(code, data);
            break;
        }
        case CMD_USBD_TRANSFER_CALLBACK_READ: {
            TransferReadCallback(code, data);
            break;
        }
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return UEC_OK;
}

int32_t UsbdStubCallBack::TransferWriteCallback(uint32_t code, OHOS::MessageParcel &data)
{
    int32_t status;
    int32_t actLength;
    uint64_t userData;
    std::shared_ptr<UsbIsoVecParcel> usbIsoVecParcel(data.ReadParcelable<UsbIsoVecParcel>());
    if (usbIsoVecParcel == nullptr) {
        USB_HILOGE(MODULE_USB_INNERKIT, "get usbIsoVecParcel error");
        return UEC_SERVICE_WRITE_PARCEL_ERROR;
    }
    if (!data.ReadInt32(status)) {
        USB_HILOGE(MODULE_USB_INNERKIT, "get status error");
        return UEC_SERVICE_WRITE_PARCEL_ERROR;
    }
    if (!data.ReadInt32(actLength)) {
        USB_HILOGE(MODULE_USB_INNERKIT, "get actLength error");
        return UEC_SERVICE_WRITE_PARCEL_ERROR;
    }
    if (!data.ReadUint64(userData)) {
        USB_HILOGE(MODULE_USB_INNERKIT, "get userData error");
        return UEC_SERVICE_WRITE_PARCEL_ERROR;
    }
    USB_HILOGI(MODULE_USB_INNERKIT, "%{public}d TransferReadCallback status:%{public}d actLength:%{public}d",
        __LINE__, status, actLength);
    return OnTransferWriteCallback(status, actLength, usbIsoVecParcel->isoInfoVec, userData);
}

int32_t UsbdStubCallBack::TransferReadCallback(uint32_t code, OHOS::MessageParcel &data)
{
    int32_t status;
    int32_t actLength;
    uint64_t userData;
    std::shared_ptr<UsbIsoVecParcel> usbIsoVecParcel(data.ReadParcelable<UsbIsoVecParcel>());
    if (usbIsoVecParcel == nullptr) {
        USB_HILOGE(MODULE_USB_INNERKIT, "get usbIsoVecParcel error");
        return UEC_SERVICE_WRITE_PARCEL_ERROR;
    }
    if (!data.ReadInt32(status)) {
        USB_HILOGE(MODULE_USB_INNERKIT, "get status error");
        return UEC_SERVICE_WRITE_PARCEL_ERROR;
    }
    if (!data.ReadInt32(actLength)) {
        USB_HILOGE(MODULE_USB_INNERKIT, "get actLength error");
        return UEC_SERVICE_WRITE_PARCEL_ERROR;
    }
    if (!data.ReadUint64(userData)) {
        USB_HILOGE(MODULE_USB_INNERKIT, "get userData error");
        return UEC_SERVICE_WRITE_PARCEL_ERROR;
    }
    USB_HILOGI(MODULE_USB_INNERKIT, "%{public}d TransferReadCallback status:%{public}d actLength:%{public}d",
        __LINE__, status, actLength);
    return OnTransferReadCallback(status, actLength, usbIsoVecParcel->isoInfoVec, userData);
}
} // namespace OHOS::USB
