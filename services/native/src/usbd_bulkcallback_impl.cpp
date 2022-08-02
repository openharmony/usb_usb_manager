/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "usbd_bulkcallback_impl.h"
#include "message_option.h"
#include "message_parcel.h"
#include "usbd_bulk_callback.h"
#include "usb_errors.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace USB {
int32_t UsbdBulkCallbackImpl::OnBulkWriteCallback(int32_t status, int32_t actLength)
{
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    if (!data.WriteInt32(status)) {
        USB_HILOGE(MODULE_USB_SERVICE, "%{public}s: write status failed", __func__);
        return UEC_SERVICE_INVALID_VALUE;
    }

    if (!data.WriteInt32(actLength)) {
        USB_HILOGE(MODULE_USB_SERVICE, "%{public}s: write actLength failed", __func__);
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = remote_->SendRequest(UsbdBulkCallBack::CMD_USBD_BULK_CALLBACK_WRITE, data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "%{public}s failed, error code is %{public}d", __func__, ret);
        return ret;
    }
    return UEC_OK;
}

int32_t UsbdBulkCallbackImpl::OnBulkReadCallback(int32_t status, int32_t actLength)
{
    OHOS::MessageParcel data;
    if (!data.WriteInt32(status)) {
        USB_HILOGE(MODULE_USB_SERVICE, "%{public}s: write status failed", __func__);
        return UEC_SERVICE_INVALID_VALUE;
    }

    if (!data.WriteInt32(actLength)) {
        USB_HILOGE(MODULE_USB_SERVICE, "%{public}s: write actLength failed", __func__);
        return UEC_SERVICE_INVALID_VALUE;
    }

    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    int32_t ret = remote_->SendRequest(UsbdBulkCallBack::CMD_USBD_BULK_CALLBACK_READ, data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "%{public}s failed, error code is %{public}d", __func__, ret);
        return ret;
    }
    return UEC_OK;
}
} // namespace USB
} // namespace OHOS