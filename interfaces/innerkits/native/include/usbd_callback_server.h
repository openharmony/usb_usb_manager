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

#ifndef USBD_CALLBACK_SERVER_H
#define USBD_CALLBACK_SERVER_H

#include "iusb_srv.h"
#include "usbd_callback_stub.h"
#include "v1_2/usb_types.h"

namespace OHOS::USB {
class UsbdCallBackServer : public UsbdStubCallBack {
public:
    explicit UsbdCallBackServer(const TransferCallback &callback) : callback_(callback) {}
    UsbdCallBackServer() = default;
    ~UsbdCallBackServer() = default;
    
    int32_t OnTransferWriteCallback(int32_t status, int32_t actLength,
        std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &isoInfo, uint64_t userData) override;
    int32_t OnTransferReadCallback(int32_t status, int32_t actLength,
        std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &isoInfo, uint64_t userData) override;
private:
    std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> isoInfo;
    TransferCallbackInfo info;
    TransferCallback callback_;
};
} // namespace OHOS::USB
#endif
