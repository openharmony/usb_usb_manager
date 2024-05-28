/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "usbmgrex_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "usb_service.h"

using namespace OHOS::HDI::Usb::V1_0;
using namespace OHOS::USB;

namespace OHOS {
constexpr size_t THRESHOLD = 10;
constexpr int32_t OFFSET = 4;
constexpr int32_t ZERO_BIT = 0;
constexpr int32_t FIRST_BIT = 1;
constexpr int32_t SECOND_BIT = 2;
constexpr int32_t THIRD_BIT = 3;
constexpr int32_t ZERO_MOVE_LEN = 24;
constexpr int32_t FIRST_MOVE_LEN = 16;
constexpr int32_t SECOND_MOVE_LEN = 8;
const std::u16string USB_INTERFACE_TOKEN = u"ohos.usb.IUsbSrv";

void SetTestCaseNative(TokenInfoParams *infoInstance)
{
    uint64_t tokenId = GetAccessTokenId(infoInstance);
    int ret = SetSelfTokenID(tokenId);
    if (ret == 0) {
        USB_HILOGI(MODULE_USB_SERVICE, "SetSelfTokenID success %{public}d", __LINE__);
    } else {
        USB_HILOGE(MODULE_USB_SERVICE, "SetSelfTokenID fail %{public}d", ret);
    }
    ret = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    if (ret == 0) {
        USB_HILOGI(MODULE_USB_SERVICE, "ReloadNativeTokenInfo success %{public}d", __LINE__);
    } else {
        USB_HILOGE(MODULE_USB_SERVICE, "ReloadNativeTokenInfo fail %{public}d", ret);
    }
}

void GrantPermissionSysNative()
{
    const char **permsInfo = new(std::nothrow)const char* [1];
    permsInfo[0] = "ohos.permission.MANAGE_USB_CONFIG";
    TokenInfoParams g_sysInfoInstance = {
    .dcapsNum = 0,
    .permsNum = 1,
    .aclsNum = 0,
    .dcaps = nullptr,
    .perms = permsInfo,
    .acls = nullptr,
    .processName = "usb_manager_test_sys_with_perms",
    .aplStr = "system_basic",
    };
    SetTestCaseNative(&g_sysInfoInstance);
}

void GrantPermissionNormalNative()
{
    TokenInfoParams g_normalInfoInstance = {
    .dcapsNum = 0,
    .permsNum = 0,
    .aclsNum = 0,
    .dcaps = nullptr,
    .perms = nullptr,
    .acls = nullptr,
    .processName = "usb_manager_test_normal",
    .aplStr = "normal",
    };
    SetTestCaseNative(&g_normalInfoInstance);
}

uint32_t Convert2Uint32(const uint8_t *ptr)
{
    if (ptr == nullptr) {
        return 0;
    }
    /*
     * Move the 0th digit 24 to the left, the first digit 16 to the left, the second digit 8 to the left,
     * and the third digit no left
     */
    return (ptr[ZERO_BIT] << ZERO_MOVE_LEN) | (ptr[FIRST_BIT] << FIRST_MOVE_LEN) | (ptr[SECOND_BIT] <<
        SECOND_MOVE_LEN) | (ptr[THIRD_BIT]);
}

bool DoSomethingInterestingWithMyAPI(const uint8_t *rawData, size_t size)
{
    if (rawData == nullptr) {
        return false;
    }
    uint32_t code = Convert2Uint32(rawData);
    rawData = rawData + OFFSET;
    size = size - OFFSET;

    MessageParcel data;
    data.WriteInterfaceToken(USB_INTERFACE_TOKEN);
    data.WriteBuffer(rawData, size);
    data.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    GrantPermissionSysNative();
    DelayedSpSingleton<UsbService>::GetInstance()->OnRemoteRequest(code, data, reply, option);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < OHOS::THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}