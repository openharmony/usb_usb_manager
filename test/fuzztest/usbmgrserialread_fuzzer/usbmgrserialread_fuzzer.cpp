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

#include "usbmgrserialread_fuzzer.h"

#include <memory>
#include "usb_srv_client.h"
#include "v1_0/serial_types.h"

namespace {
constexpr int32_t OK = 0;
constexpr int32_t MAX_MEMORY = 8192;
}
using OHOS::USB::UsbSrvClient;
namespace OHOS {
namespace SERIAL {
template<typename T>
std::shared_ptr<T> MakeSharedArray(size_t size)
{
    if (size == 0) {
        return NULL;
    }
    if (size > MAX_MEMORY) {
        return NULL;
    }
    T* buffer = new (std::nothrow)T[size];
    if (!buffer) {
        return NULL;
    }
    return std::shared_ptr<T>(buffer, [] (T* p) { delete[] p; });
}

bool UsbMgrSerialReadFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t) + sizeof(uint32_t) + sizeof(size_t)) {
        return false;
    }
    auto &usbSrvClient = UsbSrvClient::GetInstance();
    const int32_t portId = *reinterpret_cast<const int32_t *>(data);
    const uint32_t timeout = *reinterpret_cast<const uint32_t *>(data + sizeof(int32_t));
    const uint32_t readSize = *reinterpret_cast<const size_t *>(data + sizeof(int32_t) + sizeof(uint32_t));
    std::shared_ptr<uint8_t> buffer = MakeSharedArray<uint8_t>(readSize);
    if (usbSrvClient.SerialRead(portId, buffer.get(), readSize, timeout) != OK) {
        return false;
    }
    return true;
}
} // SERIAL
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::SERIAL::UsbMgrSerialReadFuzzTest(data, size);
    return 0;
}

