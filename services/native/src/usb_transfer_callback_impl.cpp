/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "usb_transfer_callback_impl.h"
#include "usbd_callback_stub.h"
#include "message_option.h"
#include "message_parcel.h"
#include "usb_errors.h"
#include "hilog_wrapper.h"
#include "struct_parcel.h"

namespace OHOS {
namespace USB {
int32_t UsbTransferCallbackImpl::OnTransferWriteCallback(int32_t status, int32_t actLength,
    const std::vector<HDI::Usb::V2_0::UsbIsoPacketDescriptor> &isoInfo, const uint64_t userData)
{
    if (remote_ == nullptr) {
        USB_HILOGE(MODULE_USB_HOST, "%{public}s: remote_ is nullptr", __func__);
        return UEC_SERVICE_INVALID_VALUE;
    }
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    if (!data.WriteInterfaceToken(remote_->GetInterfaceDescriptor())) {
        USB_HILOGE(MODULE_USB_HOST, "%{public}s: write token failed", __func__);
        return UEC_SERVICE_INVALID_VALUE;
    }

    UsbPassIsoVecParcel usbIsoVecParcel;
    usbIsoVecParcel.isoInfoVec = isoInfo;
    if (!data.WriteParcelable(&usbIsoVecParcel)) {
        USB_HILOGE(MODULE_USB_HOST, "%{public}s:write usbIsoVecParcel failed", __func__);
        return UEC_SERVICE_INVALID_VALUE;
    }
    if (!data.WriteInt32(status)) {
        USB_HILOGE(MODULE_USB_HOST, "%{public}s: write status failed", __func__);
        return UEC_SERVICE_INVALID_VALUE;
    }
    if (!data.WriteInt32(actLength)) {
        USB_HILOGE(MODULE_USB_HOST, "%{public}s: write actLength failed", __func__);
        return UEC_SERVICE_INVALID_VALUE;
    }
    if (!data.WriteUint64(userData)) {
        USB_HILOGE(MODULE_USB_HOST, "%{public}s: write userData failed", __func__);
        return UEC_SERVICE_INVALID_VALUE;
    }

    int32_t ret = remote_->SendRequest(UsbdStubCallBack::CMD_USBD_TRANSFER_CALLBACK_WRITE, data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_HOST, "%{public}s UsbdStubCallBack failed, error code is %{public}d", __func__, ret);
        return ret;
    }
    return UEC_OK;
}

int32_t UsbTransferCallbackImpl::OnTransferReadCallback(int32_t status, int32_t actLength,
    const std::vector<HDI::Usb::V2_0::UsbIsoPacketDescriptor> &isoInfo, const uint64_t userData)
{
    USB_HILOGI(MODULE_USB_HOST, "%{public}s: UsbdTransferCallbackImpl OnTransferReadCallback enter", __func__);
    if (remote_ == nullptr) {
        USB_HILOGE(MODULE_USB_HOST, "%{public}s: remote_ is nullptr", __func__);
        return UEC_SERVICE_INVALID_VALUE;
    }
    OHOS::MessageParcel data;
    if (!data.WriteInterfaceToken(remote_->GetInterfaceDescriptor())) {
        USB_HILOGE(MODULE_USB_HOST, "%{public}s: write token failed", __func__);
        return UEC_SERVICE_INVALID_VALUE;
    }
    UsbPassIsoVecParcel usbIsoVecParcel;
    usbIsoVecParcel.isoInfoVec = isoInfo;
    if (!data.WriteParcelable(&usbIsoVecParcel)) {
        USB_HILOGE(MODULE_USB_HOST, "%{public}s:write usbIsoVecParcel failed", __func__);
        return UEC_SERVICE_INVALID_VALUE;
    }
    if (!data.WriteInt32(status)) {
        USB_HILOGE(MODULE_USB_HOST, "%{public}s: write status failed", __func__);
        return UEC_SERVICE_INVALID_VALUE;
    }
    if (!data.WriteInt32(actLength)) {
        USB_HILOGE(MODULE_USB_HOST, "%{public}s: write actLength failed", __func__);
        return UEC_SERVICE_INVALID_VALUE;
    }
    if (!data.WriteUint64(userData)) {
        USB_HILOGE(MODULE_USB_HOST, "%{public}s: write userData failed", __func__);
        return UEC_SERVICE_INVALID_VALUE;
    }
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    int32_t ret = remote_->SendRequest(UsbdStubCallBack::CMD_USBD_TRANSFER_CALLBACK_READ, data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_HOST, "%{public}s UsbdStubCallBack failed, error code is %{public}d", __func__, ret);
        return ret;
    }
    return UEC_OK;
}
} // namespace USB
} // namespace OHOS
