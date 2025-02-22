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

#include "grant_sys_permission.h"

using namespace OHOS::Security::AccessToken;

using OHOS::USB::MODULE_USB_SERVICE;
using OHOS::USB::USB_MGR_LABEL;
 
static void SetTestCaseNative(TokenInfoParams *infoInstance)
{
    uint64_t tokenId = GetAccessTokenId(infoInstance);
    int ret = SetSelfTokenID(tokenId);
    if (ret == 0) {
        USB_HILOGI(MODULE_USB_SERVICE, "SetSelfTokenID success %{public}d", __LINE__);
    } else {
        USB_HILOGE(MODULE_USB_SERVICE, "SetSelfTokenID fail %{public}d", ret);
    }
    ret = AccessTokenKit::ReloadNativeTokenInfo();
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