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

#include "usbmgrserialsetattribute_fuzzer.h"

#include "usb_srv_client.h"
#include "v1_0/serial_types.h"
#include "usb_serial_type.h"
namespace {
constexpr int32_t OK = 0;
}
using OHOS::USB::UsbSrvClient;
namespace OHOS {
const uint32_t OFFSET = 4;
namespace SERIAL {
bool UsbMgrSerialSetAttributeFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < sizeof(UsbSerialAttr) + OFFSET) {
        return false;
    }
    auto &usbSrvClient = UsbSrvClient::GetInstance();
    int32_t portId = *reinterpret_cast<const int32_t *>(data);

    if (usbSrvClient.SerialSetAttribute(portId, reinterpret_cast<UsbSerialAttr &>(std::move(data + OFFSET))) != OK) {
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
    OHOS::SERIAL::UsbMgrSerialSetAttributeFuzzTest(data, size);
    return 0;
}

