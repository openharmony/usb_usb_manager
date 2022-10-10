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

#include "usb_napi_errors.h"
#include <optional>

#include "hilog_wrapper.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace USB {
std::optional<std::string_view> GetErrMsgByErrCode(int32_t errCode)
{
    auto obj = ERRCODE_MSG_MAP.find(errCode);
    if (obj == ERRCODE_MSG_MAP.end()) {
        USB_HILOGE(MODULE_JS_NAPI, "invalid errCode %{public}d", errCode);
        return std::nullopt;
    }
    return obj->second;
}

void ThrowBusinessError(const napi_env &env, int32_t errCode, const std::string &errMsg)
{
    auto commMsg = GetErrMsgByErrCode(errCode);
    if (!commMsg.has_value()) {
        USB_HILOGE(MODULE_JS_NAPI, "get error code failed");
        return;
    }
    std::string cMsg(std::string(commMsg.value()) + errMsg);
    napi_value message = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, cMsg.c_str(), NAPI_AUTO_LENGTH, &message));

    napi_value code = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, errCode, &code));

    napi_value businessError = nullptr;
    napi_create_error(env, nullptr, message, &businessError);
    napi_set_named_property(env, businessError, "code", code);
    napi_throw(env, businessError);
}
} // namespace USB
} // namespace OHOS