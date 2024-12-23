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

#ifndef USBMGR_USBD_TRANSFER_CALLBACK_IMPL_H
#define USBMGR_USBD_TRANSFER_CALLBACK_IMPL_H

#include <refbase.h>
#include "iremote_object.h"
#include "v1_2/iusbd_transfer_callback.h"
#include "v1_2/usb_types.h"

namespace OHOS {
namespace USB {
class UsbdTransferCallbackImpl : public HDI::Usb::V1_2::IUsbdTransferCallback {
public:
    explicit UsbdTransferCallbackImpl(const OHOS::sptr<OHOS::IRemoteObject> &cb) : remote_(cb) {}
    UsbdTransferCallbackImpl() = default;

    int32_t OnTransferWriteCallback(int32_t status, int32_t actLength,
        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &isoInfo, const uint64_t userData) override;
    int32_t OnTransferReadCallback(int32_t status, int32_t actLength,
        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &isoInfo, const uint64_t userData) override;

    void SetTransferCallback(const OHOS::sptr<OHOS::IRemoteObject> &cb)
    {
        remote_ = cb;
    }
private:
    sptr<IRemoteObject> remote_ = nullptr;
};
} // namespace USB
} // namespace OHOS
#endif // USBMGR_USBD_TRANSFER_CALLBACK_IMPL_H
