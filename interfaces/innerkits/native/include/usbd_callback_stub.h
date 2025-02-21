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

#ifndef USBD_STUB_CALLBACK_H
#define USBD_STUB_CALLBACK_H

#include "ipc_object_stub.h"
#include "v1_2/usb_types.h"

namespace OHOS::USB {
class UsbdStubCallBack : public OHOS::IPCObjectStub {
public:
    enum {
        CMD_USBD_TRANSFER_CALLBACK_READ,
        CMD_USBD_TRANSFER_CALLBACK_WRITE,
    };

    explicit UsbdStubCallBack() : OHOS::IPCObjectStub(u"UsbdStubCallback.V1_2") {}
    ~UsbdStubCallBack() override = default;
    int32_t OnRemoteRequest(uint32_t code, OHOS::MessageParcel &data, OHOS::MessageParcel &reply,
        OHOS::MessageOption &option) override;

    virtual int32_t OnTransferWriteCallback(int32_t status, int32_t actLength,
        std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &isoInfo, uint64_t userData) = 0;
    virtual int32_t OnTransferReadCallback(int32_t status, int32_t actLength,
        std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &isoInfo, uint64_t userData) = 0;

    int32_t TransferWriteCallback(uint32_t code, OHOS::MessageParcel &data);
    int32_t TransferReadCallback(uint32_t code, OHOS::MessageParcel &data);
};
} // namespace OHOS::USB
#endif // USBD_STUB_CALLBACK_H
