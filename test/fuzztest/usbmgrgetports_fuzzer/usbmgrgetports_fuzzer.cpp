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

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "usbmgrgetports_fuzzer.h"
#include "usb_srv_client.h"
#include "usb_errors.h"
namespace OHOS {
    namespace USB {
        bool UsbMgrGetPortsFuzzTest(const uint8_t* data, size_t size)
        {
            unsigned seed = 0;
            if (size >= sizeof(unsigned)) {
                errno_t ret = memcpy_s(&seed, sizeof(unsigned), data, sizeof(unsigned));
                if (ret != UEC_OK) {
                    return false;
                }
                srand(seed);
            }
            auto &usbSrvClient = UsbSrvClient::GetInstance();
            std::vector<UsbPort> portlist;
            portlist.clear();
            int32_t ret = usbSrvClient.GetPorts(portlist);
            if (ret == UEC_OK) {
                if (portlist.empty()) {
                    USB_HILOGW(MODULE_USB_SERVICE, "GetPorts returned empty list");
                }
                return true;
            }
            if (!portlist.empty()) {
                USB_HILOGE(MODULE_USB_SERVICE, "GetPorts failed but portlist not empty");
            }
            return false;
        }
    }
}
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::USB::UsbMgrGetPortsFuzzTest(data, size);
    return 0;
}

static const uint64_t SYSTEM_APP_MASK = (static_cast<uint64_t>(1) << 32);

extern "C" int LLVMFuzzerInitialize(int *argc, char **argv)
{
    constexpr int permissionNum = 1;
    const char *perms[permissionNum] = {"ohos.permission.MANAGE_USB_CONFIG"};
    NativeTokenInfoParams info = {
        .dcapsNum = 0,
        .permsNum = permissionNum,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "usb_manager_fuzztest",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&info);
    SetSelfTokenID(tokenId | SYSTEM_APP_MASK);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    return 0;
}
