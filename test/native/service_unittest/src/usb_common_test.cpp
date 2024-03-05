/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "usb_common_test.h"
#include "usb_errors.h"
#include "hilog_wrapper.h"

using namespace OHOS::Security::AccessToken;

namespace OHOS {
namespace USB {
namespace Common {

PermissionDef g_infoManagerTestPermDef = {
    .permissionName = "",
    .bundleName = "accesstoken_test",
    .grantMode = 1,
    .label = "label",
    .labelId = 1,
    .description = "test usb service hap",
    .descriptionId = 1,
    .availableLevel = APL_NORMAL
};

PermissionStateFull g_infoManagerTestState = {
    .grantFlags = {1},
    .grantStatus = {PermissionState::PERMISSION_GRANTED},
    .isGeneral = true,
    .permissionName = "",
    .resDeviceID = {"local"}
};

HapPolicyParams g_infoManagerTestPolicyPrams = {
    .apl = APL_NORMAL,
    .domain = "test.domain",
    .permList = {g_infoManagerTestPermDef},
    .permStateList = {g_infoManagerTestState}
};

HapInfoParams g_infoManagerTestInfoParms = {
    .bundleName = "usb_service_test_hap",
    .userID = 1,
    .instIndex = 0,
    .appIDDesc = "usb service test hap",
    .isSystemApp = false
};

HapInfoParams g_infoManagerTestInfoParms1 = {
    .bundleName = "usb_service_test_hap",
    .userID = 2,
    .instIndex = 0,
    .appIDDesc = "usb service test hap",
    .isSystemApp = false
};

HapInfoParams g_infoManagerTestInfoParms2 = {
    .bundleName = "usb_service_test_hap",
    .userID = 1,
    .instIndex = 1,
    .appIDDesc = "usb service test hap",
    .isSystemApp = false
};

void UsbCommonTest::SetTestCaseNative(TokenInfoParams *infoInstance)
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

void UsbCommonTest::GrantPermissionSysNative()
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

void UsbCommonTest::GrantPermissionNormalNative()
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

void UsbCommonTest::GrantNormalPermissionNative()
{
    const char **permsInfo = new(std::nothrow)const char* [1];
    permsInfo[0] = "ohos.permission.MANAGE_USB_CONFIG";
    TokenInfoParams g_normalInfoInstance = {
    .dcapsNum = 0,
    .permsNum = 1,
    .aclsNum = 0,
    .dcaps = nullptr,
    .perms = permsInfo,
    .acls = nullptr,
    .processName = "usb_manager_test_normal_with_perms",
    .aplStr = "normal",
    };
    SetTestCaseNative(&g_normalInfoInstance);
}

void UsbCommonTest::GrantSysNoPermissionNative()
{
    TokenInfoParams g_sysInfoInstance = {
    .dcapsNum = 0,
    .permsNum = 0,
    .aclsNum = 0,
    .dcaps = nullptr,
    .perms = nullptr,
    .acls = nullptr,
    .processName = "usb_manager_test_sys",
    .aplStr = "system_basic",
    };
    SetTestCaseNative(&g_sysInfoInstance);
}

AccessTokenID UsbCommonTest::AllocHapTest()
{
    AccessTokenID tokenId = 0;
    AccessTokenIDEx tokenIdEx = {0};
    tokenIdEx = AccessTokenKit::AllocHapToken(g_infoManagerTestInfoParms, g_infoManagerTestPolicyPrams);
    tokenId = tokenIdEx.tokenIdExStruct.tokenID;
    if (tokenId != 0) {
        USB_HILOGI(MODULE_USB_SERVICE, "AllocHapToken success, tokenId: %{public}d", tokenId);
    } else {
        USB_HILOGE(MODULE_USB_SERVICE, "AllocHapToken fail,tokenId: %{public}d", tokenId);
    }
    return tokenId;
}
AccessTokenID UsbCommonTest::AllocHapTestWithUserIdDiff()
{
    AccessTokenID tokenId = 0;
    AccessTokenIDEx tokenIdEx = {0};
    tokenIdEx = AccessTokenKit::AllocHapToken(g_infoManagerTestInfoParms1, g_infoManagerTestPolicyPrams);
    tokenId = tokenIdEx.tokenIdExStruct.tokenID;
    if (tokenId != 0) {
        USB_HILOGI(MODULE_USB_SERVICE, "AllocHapToken success, tokenId: %{public}d", tokenId);
    } else {
        USB_HILOGE(MODULE_USB_SERVICE, "AllocHapToken fail,tokenId: %{public}d", tokenId);
    }
    return tokenId;
}
AccessTokenID UsbCommonTest::AllocHapTestWithIndexDiff()
{
    AccessTokenID tokenId = 0;
    AccessTokenIDEx tokenIdEx = {0};
    tokenIdEx = AccessTokenKit::AllocHapToken(g_infoManagerTestInfoParms2, g_infoManagerTestPolicyPrams);
    tokenId = tokenIdEx.tokenIdExStruct.tokenID;
    if (tokenId != 0) {
        USB_HILOGI(MODULE_USB_SERVICE, "AllocHapToken success, tokenId: %{public}d", tokenId);
    } else {
        USB_HILOGE(MODULE_USB_SERVICE, "AllocHapToken fail,tokenId: %{public}d", tokenId);
    }
    return tokenId;
}

void UsbCommonTest::SetSelfToken(AccessTokenID tokenId)
{
    auto ret = SetSelfTokenID(tokenId);
    if (ret == 0) {
        USB_HILOGI(MODULE_USB_SERVICE, "SetSelfTokenID success, tokenId: %{public}d", tokenId);
    } else {
        USB_HILOGE(MODULE_USB_SERVICE, "SetSelfTokenID fail %{public}d, tokenId: %{public}d", ret, tokenId);
    }
    ret = AccessTokenKit::ReloadNativeTokenInfo();
    if (ret == 0) {
        USB_HILOGI(MODULE_USB_SERVICE, "ReloadNativeTokenInfo success %{public}d", __LINE__);
    } else {
        USB_HILOGE(MODULE_USB_SERVICE, "ReloadNativeTokenInfo fail %{public}d", ret);
    }
}

void UsbCommonTest::DeleteAllocHapToken(AccessTokenID tokenId)
{
    auto ret = AccessTokenKit::DeleteToken(tokenId);
    if (ret == 0) {
        USB_HILOGI(MODULE_USB_SERVICE, "DeleteToken success tokenId: %{public}d", tokenId);
    } else {
        USB_HILOGE(MODULE_USB_SERVICE, "DeleteToken fail %{public}d, tokenId: %{public}d", ret, tokenId);
    }
}

int32_t UsbCommonTest::SwitchErrCode(int32_t ret)
{
    return ret == UEC_SERVICE_NOT_SUPPORT_SWITCH_PORT ? UEC_OK : ret;
}

} // namespace Common
} // namespace USB
} // namespace OHOS
