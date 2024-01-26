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

#include "usb_callback_test.h"

#include <sys/time.h>

#include <unistd.h>
#include <iostream>
#include <vector>

#include "ashmem.h"
#include "delayed_sp_singleton.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "securec.h"
#include "system_ability_definition.h"
#include "usb_callback_test.h"
#include "usb_common_test.h"
#include "usb_errors.h"
#include "usb_srv_client.h"

using namespace OHOS;
using namespace OHOS::USB;
using namespace std;
using namespace OHOS::USB::Common;

int32_t UsbCallbackTest::OnBulkWriteCallback(int32_t status, int32_t actLength)
{
    USB_HILOGE(MODULE_USB_SERVICE, "yu_test_, status %{public}d, actLength:%{public}d", status, actLength);
    return 0;
}

int32_t UsbCallbackTest::OnBulkReadCallback(int32_t status, int32_t actLength)
{
    USB_HILOGE(MODULE_USB_SERVICE, "yu_test_, status %{public}d, actLength:%{public}d", status, actLength);
    return 0;
}