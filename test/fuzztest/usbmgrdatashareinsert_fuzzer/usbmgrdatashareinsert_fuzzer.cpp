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

#include "usb_service.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "usb_errors.h"
#include "usbmgrdatashareinsert_fuzzer.h"
#include "usb_settings_datashare.h"

namespace OHOS {
namespace USB {
    bool UsbMgrDataShareInsertFuzzTest(const uint8_t* rawData, size_t size)
    {
        if (rawData == nullptr) {
            return false;
        }
        unsigned seed = 0;
        if (size >= sizeof(unsigned)) {
            errno_t ret = memcpy_s(&seed, sizeof(unsigned), rawData, sizeof(unsigned));
            if (ret != UEC_OK) {
                return false;
            }
            srand(seed);
        }
        auto datashareHelper = std::make_shared<UsbSettingDataShare>();
        std::string status {"false"};
        OHOS::Uri uri(
            "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=USBTEST_FUZZ_STATUS");
        datashareHelper->Query(uri, "USBTEST_FUZZ_STATUS", status)
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
    OHOS::USB::UsbMgrControlTransferFuzzTest(data, size);
    return 0;
}

