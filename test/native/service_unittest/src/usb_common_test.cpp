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

#include "usb_common_test.h"

static constexpr int32_t DEFAULT_API_VERSION = 8;
using namespace OHOS::Security::AccessToken;

namespace OHOS {
namespace USB {
namespace Common {
static HapInfoParams g_InfoParams = {
    .userID = 1,
    .bundleName = "usb",
    .instIndex = 0,
    .appIDDesc = "usb_test",
    .apiVersion = DEFAULT_API_VERSION,
    .isSystemApp = true
};

HapPolicyParams policy = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "domain"
};

void UsbCommonTest::SetTestCaseHapApply (void)
{
    AccessTokenKit::AllocHapToken(g_InfoParams, policy);
    AccessTokenIDEx tokenID = AccessTokenKit::GetHapTokenIDEx(g_InfoParams.userID, g_InfoParams.bundleName,
        g_InfoParams.instIndex);
    SetSelfTokenID(tokenID.tokenIDEx);
}
} // Common
} // USB
} // OHOS
