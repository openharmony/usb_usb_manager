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
#include "usbmgrsetcurrentfunctions_fuzzer.h"

#include "usb_srv_client.h"
#include "usb_errors.h"

namespace {
    const int32_t MAX_FUNC_NUM = 6;
}

namespace OHOS {
const uint32_t code = 0X2A;
const std::u16string USB_INTERFACE_TOKEN = u"ohos.usb.IUsbServer";
namespace USB {
    bool UsbMgrSetCurrentFunctionsFuzzTest(const uint8_t* rawData, size_t size)
    {
        if (rawData == nullptr || size < sizeof(int32_t)) {
            return false;
        }
        int32_t func = *reinterpret_cast<const int32_t *>(rawData);
        if (func <= MAX_FUNC_NUM) {
            func += MAX_FUNC_NUM;
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
} // USB
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::USB::UsbMgrSetCurrentFunctionsFuzzTest(data, size);
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
