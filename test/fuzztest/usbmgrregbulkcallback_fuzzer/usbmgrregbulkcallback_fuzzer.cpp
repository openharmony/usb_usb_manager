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

#include "usbmgrregbulkcallback_fuzzer.h"

#include "iremote_object.h"
#include "usb_srv_client.h"
#include "usb_errors.h"
#include "usb_common_fuzz.h"

namespace OHOS {
const uint32_t OFFSET = 4;
const uint32_t OFFSET_BYTE = 8;
constexpr size_t THRESHOLD = 10;
namespace USB {
    bool UsbMgrRegBulkCallbackFuzzTest(const uint8_t* data, size_t /* size */)
    {
        auto[res, pipe, interface] = UsbMgrPrepareFuzzEnv();
        (void)pipe;
        (void)interface;
        if (!res) {
            USB_HILOGE(MODULE_USB_SERVICE, "prepare error");
            return false;
        }

        int32_t ret = UsbSrvClient::GetInstance().RegBulkCallback(reinterpret_cast<USBDevicePipe &>(data),
            reinterpret_cast<const USBEndpoint &>(std::move(data + OFFSET)),
+           reinterpret_cast<const sptr<IRemoteObject> &>(std::move(data + OFFSET_BYTE)));
        if (ret == UEC_OK) {
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
    OHOS::USB::UsbMgrRegBulkCallbackFuzzTest(data, size);
    return 0;
}

