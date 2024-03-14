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

#ifndef USB_COMMON_TEST_H
#define USB_COMMON_TEST_H

#include "accesstoken_kit.h"
#include "hdf_log.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
namespace USB {
namespace Common {
class UsbCommonTest {
public:
    static void SetTestCaseNative(TokenInfoParams *infoInstance);
    static void GrantPermissionSysNative();
    static void GrantPermissionNormalNative();
    static void GrantNormalPermissionNative();
    static void GrantSysNoPermissionNative();
    static Security::AccessToken::AccessTokenID AllocHapTest();
    static Security::AccessToken::AccessTokenID AllocHapTestWithUserIdDiff();
    static Security::AccessToken::AccessTokenID AllocHapTestWithIndexDiff();
    static void SetSelfToken(Security::AccessToken::AccessTokenID tokenId);
    static void DeleteAllocHapToken(Security::AccessToken::AccessTokenID tokenId);
    static int32_t SwitchErrCode(int32_t ret);
};
} // Common
} // USB
} // OHOS
#endif
