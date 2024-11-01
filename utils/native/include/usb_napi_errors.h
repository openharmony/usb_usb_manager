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

#ifndef USB_NAPI_ERRORS_H
#define USB_NAPI_ERRORS_H

#include <map>
#include <string_view>
#include "napi/native_api.h"

namespace OHOS {
namespace USB {
enum UsbJsErrCode : int32_t {
    SYSPARAM_INVALID_INPUT = 401,
    USB_DEVICE_PERMISSION_DENIED = 14400001,
    USB_SYSAPI_PERMISSION_DENIED = 202,
    USB_HDC_PERMISSION_DENIED = 14400002,
    USB_NOT_SUPPORT_SWITCH_PORT = 14400003,
    USB_DATABASE_REQUESTEXCEPTION = 14400004,
    USB_SERVICE_EXCEPTION = 14400005,
    USB_ACCESSORY_NOT_MATCH = 14400006,
    USB_WITHOUT_USER_AUTHORIZATION = 14400007,
    USB_ACCESSORY_NOT_SUPPORT = 14400008,
    USB_ACCESSORY_CAN_NOT_REOPEN = 14400009,
};

const std::map<int32_t, std::string_view> ERRCODE_MSG_MAP = {
    {SYSPARAM_INVALID_INPUT,       "BusinessError 401:Parameter error."                                         },
    {USB_DEVICE_PERMISSION_DENIED, "BusinessError 14400001:Permission denied."                                  },
    {USB_SYSAPI_PERMISSION_DENIED, "BusinessError 202:Permission denied. Normal application uses system api."   },
    {USB_HDC_PERMISSION_DENIED,    "BusinessError 14400002:Permission denied. The HDC is disabled by the system."},
    {USB_NOT_SUPPORT_SWITCH_PORT,
     "BusinessError 14400003:Unsupported operation.The current device does not support port role switching."    },
    {USB_DATABASE_REQUESTEXCEPTION,    "BusinessError 14400004:Database request operation exception."},
    {USB_SERVICE_EXCEPTION,    "BusinessError 14400005:Service exception."},
    {USB_ACCESSORY_NOT_MATCH,    "BusinessError 14400006:USBAccessory not match."},
    {USB_WITHOUT_USER_AUTHORIZATION,    "BusinessError 14400007:Without user authorization."},
    {USB_ACCESSORY_NOT_SUPPORT,    "BusinessError 14400008:Accessory mode is not supported."},
    {USB_ACCESSORY_CAN_NOT_REOPEN,    "BusinessError 14400009:Cannot repeatedly open accessory."},
};

void ThrowBusinessError(const napi_env &env, int32_t errCode, const std::string &errMsg);
napi_value CreateBusinessError(const napi_env &env, int32_t errCode, const std::string &errMsg);

#define USB_ASSERT_BASE(env, assertion, errCode, errMsg, retVal) \
    do {                                                         \
        if (!(assertion)) {                                      \
            USB_HILOGE(MODULE_JS_NAPI, #errMsg);                 \
            ThrowBusinessError((env), errCode, errMsg);          \
            return retVal;                                       \
        }                                                        \
    } while (0)

#define NOTHING
#define USB_ASSERT(env, assertion, errCode, errMsg) USB_ASSERT_BASE(env, assertion, errCode, errMsg, nullptr)
#define USB_ASSERT_RETURN_VOID(env, assertion, errCode, errMsg) \
    USB_ASSERT_BASE(env, assertion, errCode, errMsg, NOTHING)
#define USB_ASSERT_RETURN_FALSE(env, assertion, errCode, errMsg) USB_ASSERT_BASE(env, assertion, errCode, errMsg, false)
#define USB_ASSERT_RETURN_UNDEF(env, assertion, errCode, errMsg) \
    do {                                                         \
        napi_value obj = nullptr;                                \
        napi_get_undefined(env, &obj);                           \
        USB_ASSERT_BASE(env, assertion, errCode, errMsg, obj);   \
    } while (0)

#define NAPI_CHECK(env, theCall, loginfo)                                     \
    do {                                                                      \
        if ((theCall) != napi_ok) {                                           \
            USB_HILOGE(MODULE_JS_NAPI, "%{public}s " #loginfo " ", __func__); \
            napi_value obj = nullptr;                                         \
            napi_get_undefined(env, &obj);                                    \
            return obj;                                                       \
        }                                                                     \
    } while (0)

#define NAPI_CHECK_BASE(theCall, loginfo, retVal)                             \
    do {                                                                      \
        if ((theCall) != napi_ok) {                                           \
            USB_HILOGE(MODULE_JS_NAPI, "%{public}s " #loginfo " ", __func__); \
            return retVal;                                                    \
        }                                                                     \
    } while (0)

#define NAPI_CHECK_RETURN_VOID(theCall, loginfo)  NAPI_CHECK_BASE(theCall, loginfo, NOTHING)
#define NAPI_CHECK_RETURN_FALSE(theCall, loginfo) NAPI_CHECK_BASE(theCall, loginfo, false)
} // namespace USB
} // namespace OHOS
#endif // USB_NAPI_ERRORS_H
