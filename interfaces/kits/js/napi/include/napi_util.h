/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef NAPI_UTIL_H
#define NAPI_UTIL_H
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include <string>
namespace OHOS {
namespace USB {
class NapiUtil {
public:
    static void JsValueToString(
        const napi_env &env, const napi_value &value, const int32_t bufLen, std::string &target);
    static void JsObjectToString(const napi_env &env, const napi_value &object, std::string fieldStr,
        const int32_t bufLen, std::string &fieldRef);
    static bool JsObjectGetProperty(
        const napi_env &env, const napi_value &object, std::string fieldStr, napi_value &value);
    static void JsObjectToBool(const napi_env &env, const napi_value &object, std::string fieldStr, bool &fieldRef);
    static void JsObjectToInt(const napi_env &env, const napi_value &object, std::string fieldStr, int32_t &fieldRef);
    static void JsObjectToUint(
        const napi_env &env, const napi_value &object, const std::string &fieldStr, uint32_t &fieldRef);
    static void JsObjectToUint(
        const napi_env &env, const napi_value &object, const std::string &fieldStr, uint8_t &fieldRef);
    static bool JsUint8ArrayParse(
        const napi_env &env, const napi_value &object, uint8_t **uint8Buffer, size_t &bufferSize, size_t &offset);
    static void Uint8ArrayToJsValue(
        const napi_env &env, std::vector<uint8_t> &uint8Buffer, size_t bufferSize, napi_value &result);
    static void SetValueUtf8String(const napi_env &env, std::string fieldStr, std::string str, napi_value &result);
    static void SetValueInt32(const napi_env &env, std::string fieldStr, const int32_t intValue, napi_value &result);
    static void SetValueUint32(
        const napi_env &env, const std::string &fieldStr, const uint32_t uintValue, napi_value &result);
    static void SetValueBool(const napi_env &env, std::string fieldStr, const bool boolValue, napi_value &result);
};

enum class TransferFlagsJs {
    /** Report short frames as errors */
    TRANSFER_SHORT_NOT_OK = 0,
    
    /** Automatically free transfer buffer */
    TRANSFER_FREE_BUFFER = 1,

    /** Automatically free transfer after callback returns */
    TRANSFER_FREE_TRANSFER = 2,

    /** Transmissions that are multiples of wMaxPacketSize will add an additional zero packet. */
    TRANSFER_ADD_ZERO_PACKET = 3,

    TRANSFER_FLAGS_UNKNOWN = 4
};

enum class TransferStatusJs {
    /** Transfer completed */
    TRANSFER_COMPLETED = 0,

    /** Transfer failed */
    TRANSFER_ERROR = 1,

    /** Transfer timed out */
    TRANSFER_TIMED_OUT = 2,

    /** Transfer was cancelled */
    TRANSFER_CANCELLED = 3,

    /** For bulk/interrupt endpoints: halt condition detected (endpoint
     * stalled). For control endpoints: control request not supported.
     */
    TRANSFER_STALL = 4,

    /** Device was disconnected */
    TRANSFER_NO_DEVICE = 5,

    /** Device sent more data than requested */
    TRANSFER_OVERFLOW = 6,

    TRANSFER_STATUS_UNKNOWN = 7
};

enum class EndpointTransferTypeJs {
    /** Control endpoint */
    TRANSFER_TYPE_CONTROL = 0x0,

    /** Isochronous endpoint */
    TRANSFER_TYPE_ISOCHRONOUS = 0x1,

    /** Bulk endpoint */
    TRANSFER_TYPE_BULK = 0x2,

    /** Interrupt endpoint */
    TRANSFER_TYPE_INTERRUPT = 0x3,

    TRANSFER_TYPE_UNKNOWN = 0x4
};

} // namespace USB
} // namespace OHOS
#endif // NAPI_UTIL_H
