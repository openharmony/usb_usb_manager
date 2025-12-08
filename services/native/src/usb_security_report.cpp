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

#include <chrono>
#include "usb_errors.h"
#include "usb_security_report.h"
#include "hilog_wrapper.h"
#ifdef SECURITY_GUARD_ENABLE
#include "event_info.h"
#include "sg_collect_client.h"
#endif // SECURITY_GUARD_ENABLE

namespace OHOS {
namespace USB {
    
uint64_t UsbSecurityReport::GetCurrentTime()
{
    // get ms value since unix timestamp
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
    );
}

void UsbSecurityReport::ReportSecurityInfo(int64_t eventId, std::string version, nlohmann::json &json, bool isAsync)
{
#ifdef SECURITY_GUARD_ENABLE
    auto eventInfo = std::make_shared<Security::SecurityGuard::EventInfo>(eventId, version, json.dump());
    if (isAsync) {
        (void)Security::SecurityGuard::NativeDataCollectKit::ReportSecurityInfoAsync(eventInfo);
    } else {
        int32_t ret = Security::SecurityGuard::NativeDataCollectKit::ReportSecurityInfo(eventInfo);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "%{public}s: failed to report security with ret = %{public}d",
                __func__, ret);
        }
    }
#endif // SECURITY_GUARD_ENABLE
}

} // USB
} // namespace OHOS