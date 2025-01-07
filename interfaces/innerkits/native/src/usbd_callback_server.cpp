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

#include "usbd_callback_server.h"

#include "hilog_wrapper.h"
#include "usb_errors.h"

namespace OHOS::USB {

int32_t UsbdCallBackServer::OnTransferWriteCallback(int32_t status, int32_t actualLength,
    std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &isoInfo, uint64_t userData)
{
    info.status = status;
    info.actualLength = actualLength;
    if (!isoInfo.empty()) {
        this->isoInfo = isoInfo;
    }
    callback_(info, isoInfo, userData);
    return UEC_OK;
}

int32_t UsbdCallBackServer::OnTransferReadCallback(int32_t status, int32_t actualLength,
    std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &isoInfo, uint64_t userData)
{
    info.status = status;
    info.actualLength = actualLength;
    if (!isoInfo.empty()) {
        this->isoInfo = isoInfo;
    }
    callback_(info, isoInfo, userData);
    return UEC_OK;
}

} // namespace OHOS::USB
