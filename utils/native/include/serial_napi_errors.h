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

#ifndef SERIAL_NAPI_ERRORS_H
#define SERIAL_NAPI_ERRORS_H

#include <map>
#include <string>
#include <string_view>
#include "napi/native_api.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace USB {
void ThrowBusinessError(const napi_env &env, int32_t errCode, const std::string &errMsg);
napi_value CreateBusinessError(const napi_env &env, int32_t errCode, const std::string &errMsg);

enum SerialJsErrCode : int32_t {
    SYSPARAM_INVALID_INPUT = 401,
    SERIAL_SYSAPI_NOPERMISSION_CALL = 201,
    SERIAL_SYSAPI_PERMISSION_DENIED = 202,
    SERIAL_SERVICE_ABNORMAL = 31400001,
    SERIAL_INTERFACE_PERMISSION_DENIED = 31400002,
    SERIAL_PORT_NOT_EXIST = 31400003,
    SERIAL_PORT_OCCUPIED = 31400004,
    SERIAL_PORT_NOT_OPEN = 31400005,
    SERIAL_TIMED_OUT = 31400006,
    SERIAL_IO_EXCEPTION = 31400007,
    UEC_COMMON_RIGHT_DATABASE_ERROR = 14400005,
};

#ifndef USB_NAPI_ERRORS_H
const std::map<int32_t, std::string_view> ERRCODE_MSG_MAP = {
    {SYSPARAM_INVALID_INPUT, "BusinessError 401:Parameter error."},
    {SERIAL_SYSAPI_NOPERMISSION_CALL,
        "BusinessError 201:Permission denied. The application does not have the permission to call the API."},
    {SERIAL_SYSAPI_PERMISSION_DENIED, "BusinessError 202:Permission denied. Normal application uses system api."},
    {SERIAL_SERVICE_ABNORMAL, "BusinessError 31400001:The serial port service is abnormal."},
    {SERIAL_INTERFACE_PERMISSION_DENIED,
        "BusinessError 31400002:Permission denied. no access right to serial device, call requestSerialRight first."},
    {SERIAL_PORT_NOT_EXIST, "BusinessError 31400003:port not exist."},
    {SERIAL_PORT_OCCUPIED, "BusinessError 31400004:device is using by other application."},
    {SERIAL_PORT_NOT_OPEN, "BusinessError 31400005:device is not open, call open first."},
    {SERIAL_TIMED_OUT, "BusinessError 31400006:transfer timeout."},
    {SERIAL_IO_EXCEPTION, "BusinessError 31400007: IO exception. Possible causes: "
                          "1. The transfer was canceled. "
                          "2. The device offered more data."},
};
#else
#error
#endif

inline bool CheckAndThrowOnError(napi_env env, bool assertion, int errCode, const std::string& errMsg)
{
    if (!assertion) {
        USB_HILOGE(MODULE_USB_UTILS, "%{public}s", errMsg.c_str());
        OHOS::USB::ThrowBusinessError(env, errCode, errMsg.c_str());
        return false;
    }
    return true;
}

inline bool CheckNapiResult(napi_env env, napi_status theCall, const std::string& loginfo)
{
    if (theCall != napi_ok) {
        USB_HILOGE(MODULE_USB_UTILS, "%{public}s:  %{public}s", __func__, loginfo.c_str());
        return false;
    }
    return true;
}

} // namespace SERIAL
} // namespace OHOS
#endif // SERIAL_NAPI_ERRORS_H
