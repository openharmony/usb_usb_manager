/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

#include "napi_util.h"
#include <codecvt>
#include <cstdio>
#include <climits>
#include <locale>
#include <string>
#include <cstdint>

#include "hilog_wrapper.h"
#include "securec.h"
#include "usb_common.h"
#include "usb_errors.h"
#include "usb_napi_errors.h"
namespace OHOS {
namespace USB {
const int32_t MAX_STR_LENGTH = 1024;
void NapiUtil::JsValueToString(const napi_env &env, const napi_value &value, const int32_t bufLen, std::string &target)
{
    if (bufLen <= 0 || bufLen > MAX_STR_LENGTH) {
        USB_HILOGE(MODULE_JS_NAPI, "string too long malloc failed");
        return;
    }
    // 1 represent '\0'
    int32_t actBufLen = bufLen + 1;
    std::unique_ptr<char[]> buf = std::make_unique<char[]>(actBufLen);

    errno_t ret = memset_s(buf.get(), actBufLen, 0, actBufLen);
    RETURN_IF_WITH_LOG(ret != EOK, "JsValueToString memset_s failed.");

    size_t result = 0;
    napi_get_value_string_utf8(env, value, buf.get(), actBufLen, &result);
    target = buf.get();
}

void NapiUtil::JsObjectToString(
    const napi_env &env, const napi_value &object, std::string fieldStr, const int32_t bufLen, std::string &fieldRef)
{
    if (bufLen <= 0) {
        USB_HILOGE(MODULE_JS_NAPI, "invalid bufLen=%{public}d", bufLen);
        return;
    }

    bool hasProperty = false;
    napi_has_named_property(env, object, fieldStr.c_str(), &hasProperty);
    if (hasProperty) {
        napi_value field;
        napi_valuetype valueType;

        napi_get_named_property(env, object, fieldStr.c_str(), &field);
        napi_typeof(env, field, &valueType);
        USB_ASSERT_RETURN_VOID(
            env, valueType == napi_string, OHEC_COMMON_PARAM_ERROR, "The type of " + fieldStr + " must be string.");
        // 1 represent '\0'
        int32_t actBufLen = bufLen + 1;
        std::unique_ptr<char[]> buf = std::make_unique<char[]>(actBufLen);

        errno_t ret = memset_s(buf.get(), actBufLen, 0, actBufLen);
        RETURN_IF_WITH_LOG(ret != EOK, "JsObjectToString memset_s failed.");

        size_t result = 0;
        napi_get_value_string_utf8(env, field, buf.get(), actBufLen, &result);
        fieldRef = buf.get();
    } else {
        USB_HILOGW(MODULE_JS_NAPI, "js to str no property: %{public}s", fieldStr.c_str());
    }
}

bool NapiUtil::JsObjectGetProperty(
    const napi_env &env, const napi_value &object, std::string fieldStr, napi_value &value)
{
    bool hasProperty = false;
    napi_has_named_property(env, object, fieldStr.c_str(), &hasProperty);
    if (hasProperty) {
        napi_get_named_property(env, object, fieldStr.c_str(), &value);
    } else {
        USB_HILOGW(MODULE_JS_NAPI, "js object has no property: %{public}s", fieldStr.c_str());
    }
    return hasProperty;
}

void NapiUtil::JsObjectToBool(const napi_env &env, const napi_value &object, std::string fieldStr, bool &fieldRef)
{
    bool hasProperty = false;
    napi_has_named_property(env, object, fieldStr.c_str(), &hasProperty);
    if (hasProperty) {
        napi_value field;
        napi_valuetype valueType;

        napi_get_named_property(env, object, fieldStr.c_str(), &field);
        napi_typeof(env, field, &valueType);
        USB_ASSERT_RETURN_VOID(
            env, valueType == napi_boolean, OHEC_COMMON_PARAM_ERROR, "The type of " + fieldStr + " must be boolean.");
        napi_get_value_bool(env, field, &fieldRef);
    } else {
        USB_HILOGW(MODULE_JS_NAPI, "js to boolean no property: %{public}s", fieldStr.c_str());
    }
}

void NapiUtil::JsObjectToInt(const napi_env &env, const napi_value &object, std::string fieldStr, int32_t &fieldRef)
{
    bool hasProperty = false;
    napi_has_named_property(env, object, fieldStr.c_str(), &hasProperty);
    if (hasProperty) {
        napi_value field;
        napi_valuetype valueType;

        napi_get_named_property(env, object, fieldStr.c_str(), &field);
        napi_typeof(env, field, &valueType);
        USB_ASSERT_RETURN_VOID(
            env, valueType == napi_number, OHEC_COMMON_PARAM_ERROR, "The type of " + fieldStr + " must be number.");
        napi_get_value_int32(env, field, &fieldRef);
    } else {
        USB_HILOGW(MODULE_JS_NAPI, "js to int32_t no property: %{public}s", fieldStr.c_str());
    }
}

void NapiUtil::JsObjectToUint(
    const napi_env &env, const napi_value &object, const std::string &fieldStr, uint32_t &fieldRef)
{
    bool hasProperty = false;
    napi_status status = napi_has_named_property(env, object, fieldStr.c_str(), &hasProperty);
    if (status != napi_ok || !hasProperty) {
        USB_HILOGE(MODULE_JS_NAPI, "js to uint32_t no property: %{public}s", fieldStr.c_str());
        return;
    }

    napi_value field = nullptr;
    napi_valuetype valueType;

    status = napi_get_named_property(env, object, fieldStr.c_str(), &field);
    if (status != napi_ok) {
        USB_HILOGE(MODULE_JS_NAPI, "get property failed: %{public}s", fieldStr.c_str());
        return;
    }

    status = napi_typeof(env, field, &valueType);
    if (status != napi_ok) {
        USB_HILOGE(MODULE_JS_NAPI, "type error failed: %{public}s", fieldStr.c_str());
        return;
    }

    USB_ASSERT_RETURN_VOID(
        env, valueType == napi_number, OHEC_COMMON_PARAM_ERROR, "The type of " + fieldStr + " must be number.");
    status = napi_get_value_uint32(env, field, &fieldRef);
    if (status != napi_ok) {
        USB_HILOGE(MODULE_JS_NAPI, "get value failed: %{public}s", fieldStr.c_str());
    }
}

bool NapiUtil::JsObjectToUint32(
    const napi_env &env, const napi_value &object, const std::string &fieldStr, uint32_t &fieldRef)
{
    bool hasProperty = false;
    napi_status status = napi_has_named_property(env, object, fieldStr.c_str(), &hasProperty);
    if (status != napi_ok || !hasProperty) {
        USB_HILOGE(MODULE_JS_NAPI, "js to uint32_t no property: %{public}s", fieldStr.c_str());
        return false;
    }

    napi_value field = nullptr;
    napi_valuetype valueType;

    status = napi_get_named_property(env, object, fieldStr.c_str(), &field);
    if (status != napi_ok) {
        USB_HILOGE(MODULE_JS_NAPI, "get property failed: %{public}s", fieldStr.c_str());
        return false;
    }

    status = napi_typeof(env, field, &valueType);
    if (status != napi_ok) {
        USB_HILOGE(MODULE_JS_NAPI, "type error failed: %{public}s", fieldStr.c_str());
        return false;
    }

    USB_ASSERT_RETURN_FALSE(
        env, valueType == napi_number, OHEC_COMMON_PARAM_ERROR, "The type of " + fieldStr + " must be number.");
    status = napi_get_value_uint32(env, field, &fieldRef);
    if (status != napi_ok) {
        USB_HILOGE(MODULE_JS_NAPI, "get value failed: %{public}s", fieldStr.c_str());
        return false;
    }
    return true;
}

bool NapiUtil::JsObjectToUint8(
    const napi_env &env, const napi_value &object, const std::string &fieldStr, uint8_t &fieldRef)
{
    uint32_t tmpValue = UINT_MAX;
    JsObjectToUint(env, object, fieldStr, tmpValue);
    if (tmpValue > SCHAR_MAX) {
        USB_HILOGE(MODULE_JS_NAPI, "type error failed: uint32_t to uint8_t");
        return false;
    } else {
        fieldRef = tmpValue;
        return true;
    }
}

bool NapiUtil::JsObjectToUint8WithDefaultValue(
    const napi_env &env, const napi_value &object, const std::string &fieldStr, uint8_t &fieldRef, uint8_t defaultValue)
{
    bool hasProperty = false;
    napi_status status = napi_has_named_property(env, object, fieldStr.c_str(), &hasProperty);
    if (!hasProperty) {
        fieldRef = defaultValue;
        USB_HILOGI(MODULE_JS_NAPI, "Property '%{public}s' not found, using default: %{public}u",
            fieldStr.c_str(), defaultValue);
        return true;
    }
    if (status != napi_ok) {
        USB_HILOGE(MODULE_JS_NAPI, "js to uint32_t no property: %{public}s", fieldStr.c_str());
        return false;
    }

    napi_value field = nullptr;
    napi_valuetype valueType;

    status = napi_get_named_property(env, object, fieldStr.c_str(), &field);
    if (status != napi_ok) {
        USB_HILOGE(MODULE_JS_NAPI, "get property failed: %{public}s", fieldStr.c_str());
        return false;
    }

    status = napi_typeof(env, field, &valueType);
    if (status != napi_ok) {
        USB_HILOGE(MODULE_JS_NAPI, "type error failed: %{public}s", fieldStr.c_str());
        return false;
    }

    USB_ASSERT_RETURN_FALSE(
        env, valueType == napi_number, OHEC_COMMON_PARAM_ERROR, "The type of " + fieldStr + " must be number.");
    uint32_t tempFieldRef;
    status = napi_get_value_uint32(env, field, &tempFieldRef);
    fieldRef = static_cast<uint8_t>(tempFieldRef);
    if (status != napi_ok) {
        USB_HILOGE(MODULE_JS_NAPI, "get value failed: %{public}s", fieldStr.c_str());
        return false;
    }
    if (fieldRef > SCHAR_MAX) {
        USB_HILOGE(MODULE_JS_NAPI, "type error failed: uint32_t to uint8_t");
        return false;
    }
    return true;
}

bool NapiUtil::JsUint8ArrayParse(
    const napi_env &env, const napi_value &object, uint8_t **uint8Buffer, size_t &bufferSize, size_t &offset)
{
    bool isTypedArray = false;
    if (napi_is_typedarray(env, object, &isTypedArray) != napi_ok || !isTypedArray) {
        USB_ASSERT_RETURN_FALSE(env, isTypedArray, OHEC_COMMON_PARAM_ERROR, "The type of buffer must be TypedArray.");
        USB_HILOGW(MODULE_JS_NAPI, "invalid type");
        return false;
    }

    napi_typedarray_type type;
    napi_value buffer;

    napi_status infoStatus = napi_get_typedarray_info(
        env, object, &type, &bufferSize, reinterpret_cast<void **>(uint8Buffer), &buffer, &offset);
    if (infoStatus != napi_ok) {
        USB_HILOGW(MODULE_JS_NAPI, "get typedarray info failed, status: %{public}d", infoStatus);
        return false;
    }
    USB_ASSERT_RETURN_FALSE(
        env, type == napi_uint8_array, OHEC_COMMON_PARAM_ERROR, "The type of buffer must be Uint8Array.");
    USB_ASSERT_RETURN_FALSE(
        env, bufferSize != 0, OHEC_COMMON_PARAM_ERROR, "The size of buffer must be a positive number.");
    return true;
}

void NapiUtil::Uint8ArrayToJsValue(
    const napi_env &env, std::vector<uint8_t> &uint8Buffer, size_t bufferSize, napi_value &result)
{
    uint8_t *nativeArraybuffer = nullptr;
    napi_value nativeValue = nullptr;
    napi_create_arraybuffer(env, bufferSize, reinterpret_cast<void **>(&nativeArraybuffer), &nativeValue);

    errno_t ret = memcpy_s(nativeArraybuffer, bufferSize, uint8Buffer.data(), bufferSize);
    if (ret != EOK) {
        USB_HILOGE(MODULE_JS_NAPI, "memcpy_s failed");
        return;
    }

    napi_create_typedarray(env, napi_uint8_array, bufferSize, nativeValue, 0, &result);
}

void NapiUtil::SetValueUtf8String(const napi_env &env, std::string fieldStr, std::string str, napi_value &result)
{
    napi_value value;
    napi_create_string_utf8(env, str.c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, fieldStr.c_str(), value);
}

void NapiUtil::SetValueInt32(const napi_env &env, std::string fieldStr, const int32_t intValue, napi_value &result)
{
    napi_value value;
    napi_create_int32(env, intValue, &value);
    napi_set_named_property(env, result, fieldStr.c_str(), value);
}

void NapiUtil::SetValueUint32(
    const napi_env &env, const std::string &fieldStr, const uint32_t uintValue, napi_value &result)
{
    napi_value value = nullptr;
    napi_status status = napi_create_uint32(env, uintValue, &value);
    if (status != napi_ok) {
        USB_HILOGE(MODULE_JS_NAPI, "create uint32 failed:%{public}s", fieldStr.c_str());
        return;
    }

    status = napi_set_named_property(env, result, fieldStr.c_str(), value);
    if (status != napi_ok) {
        USB_HILOGE(MODULE_JS_NAPI, "set property failed:%{public}s", fieldStr.c_str());
    }
}

void NapiUtil::SetValueBool(const napi_env &env, std::string fieldStr, const bool boolValue, napi_value &result)
{
    napi_value value;
    napi_get_boolean(env, boolValue, &value);
    napi_set_named_property(env, result, fieldStr.c_str(), value);
}

} // namespace USB
} // namespace OHOS
