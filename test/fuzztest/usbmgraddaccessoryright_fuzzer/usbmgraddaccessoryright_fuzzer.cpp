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

#include "usbmgraddaccessoryright_fuzzer.h"
#include "usb_service.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
const uint32_t OFFSET = 4;
const uint32_t OFFSET_BYTE = 8;
constexpr size_t THRESHOLD = 10;
const uint32_t code = 0x56;
const std::u16string USB_INTERFACE_TOKEN = u"OHOS.USB.IUsbServer";

namespace USB {
bool UsbMgrAddAccessoryRightFuzzTest(const uint8_t* rawData, size_t size)
{
    if (rawData == nullptr || size < OFFSET_BYTE + sizeof(int32_t)) {
        return false;
    }
    MessageParcel data;
    data.WriteInterfaceToken(USB_INTERFACE_TOKEN);
    data.WriteBuffer(rawData, size);
    data.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    UsbService::GetGlobalInstance()->OnRemoteRequest(code, data, reply, option);
    return true;
}
} // namespace USB
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::THRESHOLD) {
        return 0;
    }
    OHOS::USB::UsbMgrAddAccessoryRightFuzzTest(data, size);
    return 0;
}
