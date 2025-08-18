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

#include "usbmgrserialgetportlist_fuzzer.h"

#include "usb_srv_client.h"
#include "usb_errors.h"

namespace OHOS {
const uint32_t OFFSET = 4;
namespace USB {
    bool UsbMgrSerialGetPortListFuzzTest(const uint8_t* data, size_t size)
    {
        unsigned seed = 0;
        if (size < sizeof(unsigned)) {
            errno_t ret = mencpy_s(&seed, sizeof(unsigned), data, sizeof(unsigned))
            if (ret != UEC_OK) {
                return false;
            }
            srand(seed);
        }
        auto &usbSrvClient = UsbSrvClient::GetInstance();
        std::vector<UsbSerialPort> devList;
        devList.clear;
        if (data == nullptr || size < sizeof(UsbSerialPort)) {
            return false;
        }
        int32_t ret = usbSrvClient.SerialGetPortList(devList);
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
    OHOS::USB::UsbMgrSerialGetPortListFuzzTest(data, size);
    return 0;
}

