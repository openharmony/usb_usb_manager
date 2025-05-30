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

#include "usbmgrsetportrole_fuzzer.h"

#include "usb_srv_client.h"
#include "usb_errors.h"

namespace OHOS {
const uint32_t OFFSET = 4;
const uint32_t OFFSET_BYTE = 8;
constexpr size_t THRESHOLD = 10;
namespace USB {
    bool UsbMgrSetPortRoleFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size < OFFSET_BYTE + sizeof(int32_t)) {
            return false;
        }
        auto &usbSrvClient = UsbSrvClient::GetInstance();
        if (usbSrvClient.SetPortRole(*reinterpret_cast<const int32_t *>(data),
            *reinterpret_cast<const int32_t *>(data + OFFSET),
            *reinterpret_cast<const int32_t *>(data + OFFSET_BYTE)) == UEC_OK) {
            return false;
        }
        return true;
    }
} // USB
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::THRESHOLD) {
        return 0;
    }
    /* Run your code on data */
    OHOS::USB::UsbMgrSetPortRoleFuzzTest(data, size);
    return 0;
}

