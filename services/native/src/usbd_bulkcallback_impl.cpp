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
#include <hdf_log.h>
#include <message_option.h>
#include <message_parcel.h>
#include "usbd_bulk_callback.h"

int32_t UsbdBulkCallbackImpl::OnBulkWriteCallback(int32_t status, int32_t actLength)
{
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    if (!data.WriteInt32(status)) {
        HDF_LOGE("%{public}s: write status failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!data.WriteInt32(actLength)) {
        HDF_LOGE("%{public}s: write actLength failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = remote_->SendRequest(OHOS::USB::UsbdBulkCallBack::CMD_USBD_BULK_CALLBACK_WRITE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, ret);
        return ret;
    }
    return HDF_SUCCESS;
}

int32_t UsbdBulkCallbackImpl::OnBulkReadCallback(int32_t status, int32_t actLength)
{
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    if (!data.WriteInt32(status)) {
        HDF_LOGE("%{public}s: write status failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!data.WriteInt32(actLength)) {
        HDF_LOGE("%{public}s: write actLength failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = remote_->SendRequest(OHOS::USB::UsbdBulkCallBack::CMD_USBD_BULK_CALLBACK_READ, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, ret);
        return ret;
    }
    return HDF_SUCCESS;
}
