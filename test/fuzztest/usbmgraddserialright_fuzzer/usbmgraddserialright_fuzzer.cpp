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

#include "usbmgraddserialright_fuzzer.h"

#include "usb_srv_client.h"
#include "usb_errors.h"

namespace OHOS {
namespace USB {
    bool UsbMgrAddSerialRightFuzzTest(const uint8_t* data, size_t size)
    {
        auto &usbSrvClient = UsbSrvClient::GetInstance();
        if (data == nullptr || size < sizeof(int32_t) + sizeof(uint32_t)) {
            return false;
        }
        int32_t ret = usbSrvClient.AddSerialRight(*reinterpret_cast<const uint32_t *>(data),
            *reinterpret_cast<const int32_t *>(data + sizeof(uint32_t)));
        if (ret == UEC_OK) {
            return false;
        }
        return true;
    }
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::USB::UsbMgrAddSerialRightFuzzTest(data, size);
    return 0;
}

