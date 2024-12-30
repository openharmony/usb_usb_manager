/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <unistd.h>

#include <sys/time.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <future>
#include <functional>
#include <mutex>

#include "hilog_wrapper.h"
#include "securec.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_common.h"
#include "napi_util.h"
#include "serial_async_context.h"
#include "serial_napi_errors.h"
#include "usb_errors.h"

#include "usb_srv_client.h"
#include "v1_0/iserial_interface.h"

using namespace OHOS;
using namespace OHOS::USB;

const int32_t ARGC_0 = 0;
const int32_t ARGC_1 = 1;
const int32_t ARGC_2 = 2;
const int32_t ARGC_3 = 3;
const int32_t USLEEP_TIME = 500;

std::mutex g_mutex;
static std::vector<OHOS::HDI::Usb::Serial::V1_0::SerialPort> g_portIds;
static UsbSrvClient &g_usbClient = UsbSrvClient::GetInstance();

static napi_value ToInt32Value(napi_env env, int32_t value)
{
    napi_value staticValue = nullptr;
    napi_create_int32(env, value, &staticValue);
    return staticValue;
}

int32_t ErrorCodeConversion(int32_t value)
{
    if (value == UEC_SERVICE_PERMISSION_DENIED_SYSAPI) {
        return SERIAL_SYSAPI_PERMISSION_DENIED;
    } else if (value == UEC_SERVICE_PERMISSION_DENIED) {
        return SERIAL_INTERFACE_PERMISSION_DENIED;
    } else if (value == UEC_MANAGER_PORT_REPEAT_OPEN) {
        return SERIAL_PORT_OCCUPIED;
    } else if (value == UEC_MANAGER_DEVICENOTOPEN || value == UEC_MANAGER_PORT_NOT_OPEN) {
        return SERIAL_PORT_NOT_OPEN;
    } else if (value == UEC_INTERFACE_TIMED_OUT) {
        return SERIAL_TIMED_OUT;
    } else if (value == UEC_MANAGER_IO_EXCEPTION) {
        return SERIAL_IO_EXCEPTION;
    } else {
        return SERIAL_SERVICE_ABNORMAL;
    }
}

static napi_value SerialGetPortListNapi(napi_env env, napi_callback_info info)
{
    USB_HILOGI(MODULE_JS_NAPI, "SerialGetPortListNapi start");
    size_t argc = ARGC_0;
    if (!NapiCheck(env, napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr), "Get call back info failed")) {
        return nullptr;
    }
    if (!SerialAssert(env, (argc == ARGC_0), SYSPARAM_INVALID_INPUT, "The function takes no arguments.")) {
        return nullptr;
    }
    int32_t ret = g_usbClient.SerialGetPortList(g_portIds);
    if (!SerialAssert(env, (ret == 0), ErrorCodeConversion(ret), "get portlist failed")) {
        return nullptr;
    }
    napi_value result = nullptr;
    napi_create_array(env, &result);
    for (uint32_t i = 0; i < g_portIds.size(); ++i) {
        napi_value portObj;
        napi_create_object(env, &portObj);
        NapiUtil::SetValueInt32(env, "portId", g_portIds[i].portId, portObj);
        std::string deviceName = std::to_string(g_portIds[i].deviceInfo.busNum) + "-" +
            std::to_string(g_portIds[i].deviceInfo.devAddr);
        NapiUtil::SetValueUtf8String(env, "deviceName", deviceName, portObj);
        USB_HILOGE(MODULE_JS_NAPI, "portId: %{public}d", g_portIds[i].portId);
        USB_HILOGE(MODULE_JS_NAPI, "deviceName: %{public}s", deviceName.c_str());
        napi_set_element(env, result, i, portObj);
    }
    return result;
}

bool IsPortIdExist(int32_t portId)
{
    for (auto it : g_portIds) {
        if (it.portId == portId) {
            return true;
        }
    }
    return false;
}

static napi_value SerialGetAttributeNapi(napi_env env, napi_callback_info info)
{
    USB_HILOGI(MODULE_JS_NAPI, "SerialGetAttributeNapi start");
    size_t argc = ARGC_1;
    napi_value argv[ARGC_1] = {nullptr};
    if (!NapiCheck(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), "Get call back info failed")) {
        return nullptr;
    }
    if (!SerialAssert(env, (argc == ARGC_1), SYSPARAM_INVALID_INPUT, "The function takes 1 arguments.")) {
        return nullptr;
    }
    napi_value portId = argv[0];
    napi_valuetype type;
    napi_typeof(env, portId, &type);
    if (!SerialAssert(env, type == napi_number, SYSPARAM_INVALID_INPUT, "The type of arg0 must be int32_t.")) {
        return nullptr;
    }
    int32_t portIdValue = -1;
    napi_get_value_int32(env, portId, &portIdValue);
    if (!SerialAssert(env, (portIdValue != -1), SYSPARAM_INVALID_INPUT, "Failed to get portId.")) {
        return nullptr;
    }
    if (!IsPortIdExist(portIdValue)) {
        if (!SerialAssert(env, false, SERIAL_PORT_NOT_EXIST, "portid does not exist.")) {
            return nullptr;
        }
    }
    OHOS::HDI::Usb::Serial::V1_0::SerialAttribute serialAttribute;
    int32_t ret = g_usbClient.SerialGetAttribute(portIdValue, serialAttribute);
    if (!SerialAssert(env, (ret == 0), ErrorCodeConversion(ret), "Failed to get attribute.")) {
        return nullptr;
    }
    napi_value result = nullptr;
    napi_create_object(env, &result);
    NapiUtil::SetValueUint32(env, "baudrate", serialAttribute.baudrate, result);
    NapiUtil::SetValueUint32(env, "dataBits", serialAttribute.dataBits, result);
    NapiUtil::SetValueUint32(env, "parity", serialAttribute.parity, result);
    NapiUtil::SetValueUint32(env, "stopBits", serialAttribute.stopBits, result);
    return result;
}

bool SetAttributeTransferParams(napi_env env, napi_callback_info info,
    int32_t& portIdValue, OHOS::HDI::Usb::Serial::V1_0::SerialAttribute& serialAttribute)
{
    USB_HILOGI(MODULE_JS_NAPI, "SetAttributeTransferParams start");
    size_t argc = ARGC_2;
    napi_value argv[ARGC_2] = { nullptr };
    if (!NapiCheck(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), "Get call back info failed")) {
        return false;
    }
    if (!SerialAssert(env, (argc == ARGC_2), SYSPARAM_INVALID_INPUT, "The function takes 2 arguments.")) {
        return false;
    }
    napi_value portId = argv[0];
    napi_valuetype type;
    napi_typeof(env, portId, &type);
    if (!SerialAssert(env, type == napi_number, SYSPARAM_INVALID_INPUT, "The type of arg0 must be int32_t.")) {
        return false;
    }
    napi_get_value_int32(env, portId, &portIdValue);
    if (!SerialAssert(env, (portIdValue != -1), SYSPARAM_INVALID_INPUT, "Failed to get portId.")) {
        return false;
    }
    if (!IsPortIdExist(portIdValue)) {
        if (!SerialAssert(env, false, SERIAL_PORT_NOT_EXIST, "portid does not exist.")) {
            return false;
        }
    }
    napi_value obj = argv[1];
    napi_typeof(env, obj, &type);
    if (!SerialAssert(env, type == napi_object, SYSPARAM_INVALID_INPUT, "The type of arg1 must be SerialAttribute.")) {
        return false;
    }
    NapiUtil::JsObjectToUint(env, obj, "baudrate", serialAttribute.baudrate);
    NapiUtil::JsObjectToUint(env, obj, "dataBits", serialAttribute.dataBits);
    NapiUtil::JsObjectToUint(env, obj, "parity", serialAttribute.parity);
    NapiUtil::JsObjectToUint(env, obj, "stopBits", serialAttribute.stopBits);
    return true;
}

static napi_value SerialSetAttributeNapi(napi_env env, napi_callback_info info)
{
    USB_HILOGI(MODULE_JS_NAPI, "SerialSetAttributeNapi start");
    OHOS::HDI::Usb::Serial::V1_0::SerialAttribute serialAttribute;
    int32_t portIdValue = -1;
    if (!SetAttributeTransferParams(env, info, portIdValue, serialAttribute)) {
        return nullptr;
    }
    USB_HILOGI(MODULE_JS_NAPI, "SetAttributeNapi portIdValue: %{public}d", portIdValue);
    int ret = g_usbClient.SerialSetAttribute(portIdValue, serialAttribute);
    if (!SerialAssert(env, (ret == 0), ErrorCodeConversion(ret), "Failed to set attribute.")) {
        return nullptr;
    }
    return nullptr;
}

bool SerialWriteTransferParams(napi_env env, napi_callback_info info,
    int32_t& portIdValue, napi_value* buffer, uint32_t& timeoutValue)
{
    USB_HILOGI(MODULE_JS_NAPI, "SerialWriteTransferParams start");
    size_t argc = ARGC_3;
    napi_value argv[ARGC_3] = {nullptr};
    if (!NapiCheck(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), "Get call back info failed")) {
        return false;
    }
    if (!SerialAssert(env, (argc == ARGC_2 || argc == ARGC_3), SYSPARAM_INVALID_INPUT,
        "The function takes 2 or 3 arguments.")) {
        return false;
    }
    napi_value portId = argv[0];
    napi_valuetype type;
    napi_typeof(env, portId, &type);
    if (!SerialAssert(env, type == napi_number, SYSPARAM_INVALID_INPUT, "The type of portId must be int32_t.")) {
        return false;
    }
    napi_get_value_int32(env, portId, &portIdValue);
    if (!SerialAssert(env, (portIdValue != -1), SYSPARAM_INVALID_INPUT, "Failed to get portId.")) {
        return false;
    }
    if (!IsPortIdExist(portIdValue)) {
        if (!SerialAssert(env, false, SERIAL_PORT_NOT_EXIST, "portid does not exist.")) {
            return false;
        }
    }
    *buffer = argv[1];
    if (argc == ARGC_3) {
        napi_value timeout = argv[2];
        napi_typeof(env, timeout, &type);
        if (!SerialAssert(env, type == napi_number, SYSPARAM_INVALID_INPUT, "The type of timeout must be uint32_t.")) {
            return false;
        }
        napi_get_value_uint32(env, timeout, &timeoutValue);
    }
    return true;
}

static napi_value SerialWriteSyncNapi(napi_env env, napi_callback_info info)
{
    USB_HILOGI(MODULE_JS_NAPI, "SerialWriteSyncNapi start");
    int32_t portIdValue = -1;
    napi_value buffer;
    uint32_t timeoutValue = 0;
    if (!SerialWriteTransferParams(env, info, portIdValue, &buffer, timeoutValue)) {
        return nullptr;
    }
    napi_typedarray_type arrayType;
    size_t bufferLength;
    void* bufferValue = nullptr;
    NAPI_CALL(env, napi_get_typedarray_info(env, buffer, &arrayType, &bufferLength, &bufferValue, nullptr, nullptr));
    if (!SerialAssert(env, arrayType == napi_uint8_array, SYSPARAM_INVALID_INPUT,
        "The type of buffer must be an array of uint8_t.")) {
        return nullptr;
    }
    std::vector<uint8_t> bufferVector(static_cast<uint8_t*>(bufferValue),
        static_cast<uint8_t*>(bufferValue) + bufferLength);
    std::future<int32_t> resp = std::async(&UsbSrvClient::SerialWrite, &g_usbClient, portIdValue,
        std::cref(bufferVector), static_cast<uint32_t>(bufferLength));
    std::future_status status = std::future_status::deferred;
    if (timeoutValue == 0) {
        resp.wait();
    } else {
        status = resp.wait_for(std::chrono::milliseconds(timeoutValue));
        if (!SerialAssert(env, status == std::future_status::ready, SERIAL_TIMED_OUT, "write timed out.")) {
            return nullptr;
        }
    }
    napi_value result = nullptr;
    
    if (!SerialAssert(env, (resp.get() == 0), ErrorCodeConversion(resp.get()), "SerialWrite Failed.")) {
        return nullptr;
    }

    napi_create_int32(env, resp.get(), &result);
    return result;
}

static auto g_serialWriteExecute = [](napi_env env, void* data) {
    SerialWriteAsyncContext *context = static_cast<SerialWriteAsyncContext *>(data);
    if (context->contextErrno) {
        USB_HILOGE(MODULE_JS_NAPI, "ExecuteCallback failed, reason: napi_get_reference_value");
        return;
    }
    size_t bufferLength = context->size;
    void* bufferValue = context->pData;
    if (context->contextErrno) {
        USB_HILOGE(MODULE_JS_NAPI, "ExecuteCallback failed, reason: napi_get_typedarray_info");
        return;
    }
    if (bufferLength == 0) {
        USB_HILOGE(MODULE_JS_NAPI, "no valid data to write");
        return;
    }
    context->size = bufferLength;
    std::vector<uint8_t> bufferVector(static_cast<uint8_t*>(bufferValue),
        static_cast<uint8_t*>(bufferValue) + context->size);
    std::future<int32_t> resp = std::async(&UsbSrvClient::SerialWrite, &g_usbClient,
        context->portId, std::cref(bufferVector), context->size);
    std::future_status status = std::future_status::deferred;
    if (context->timeout == 0) {
        resp.wait();
    } else {
        status = resp.wait_for(std::chrono::milliseconds(context->timeout));
        context->contextErrno = (status != std::future_status::ready?SERIAL_TIMED_OUT:0);
        if (context->contextErrno) {
            USB_HILOGE(MODULE_JS_NAPI, "Write timeout!");
            return;
        }
    }
    context->contextErrno = ErrorCodeConversion(resp.get());
    context->ret = resp.get();
};

static auto g_serialWriteComplete = [](napi_env env, napi_status status, void *data) {
    SerialWriteAsyncContext *context = static_cast<SerialWriteAsyncContext *>(data);
    napi_value result = nullptr;
    if (context->contextErrno) {
        napi_create_int32(env, context->contextErrno, &result);
        napi_reject_deferred(env, context->deferred, result);
    } else {
        napi_create_int32(env, context->ret, &result);
        napi_resolve_deferred(env, context->deferred, result);
    }
    napi_delete_async_work(env, context->work);
    delete context;
};

static napi_value SerialWriteNapi(napi_env env, napi_callback_info info)
{
    USB_HILOGE(MODULE_JS_NAPI, "start write Napi");
    int32_t portIdValue = -1;
    napi_value buffer;
    uint32_t timeoutValue = 0;
    if (!SerialWriteTransferParams(env, info, portIdValue, &buffer, timeoutValue)) {
        return nullptr;
    }
    napi_typedarray_type arrayType;
    size_t bufferLength;
    void* bufferValue = nullptr;
    NAPI_CALL(env, napi_get_typedarray_info(env, buffer, &arrayType, &bufferLength, &bufferValue, nullptr, nullptr));
    if (!SerialAssert(env, arrayType == napi_uint8_array, SYSPARAM_INVALID_INPUT,
        "The type of buffer must be an array of uint8_t.")) {
        return nullptr;
    }
    SerialWriteAsyncContext *asyncContext = new (std::nothrow) SerialWriteAsyncContext;
    napi_value promise;
    NAPI_CALL(env, napi_create_promise(env, &asyncContext->deferred, &promise));
    asyncContext->portId = portIdValue;
    asyncContext->timeout = timeoutValue;
    asyncContext->contextErrno = 0;
    asyncContext->size = bufferLength;
    asyncContext->pData = bufferValue;
    napi_value resourceName;
    napi_create_string_utf8(env, "SerialWrite", NAPI_AUTO_LENGTH, &resourceName);
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, g_serialWriteExecute, g_serialWriteComplete,
        asyncContext, &asyncContext->work));
    napi_queue_async_work(env, asyncContext->work);
    return promise;
}

int32_t SerialReadHelper(int32_t portId, uint8_t *data, uint32_t size)
{
    std::vector<uint8_t> bufferVector;
    int ret = g_usbClient.SerialRead(portId, bufferVector, size);
    if (ret != 0) {
        USB_HILOGE(MODULE_JS_NAPI, "read error, ret = %{public}d", ret);
        return ret;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    size_t num = 0;
    for (size_t i = 0; i < bufferVector.size(); ++i) {
        if (num <= size - 1) {
            data[num] = bufferVector.at(i);
            num++;
            continue;
        }
        usleep(USLEEP_TIME);
        num = 0;
    }
    USB_HILOGI(MODULE_JS_NAPI, "data = %{public}s", data);
    return ret;
}

bool ReadTransferParams(napi_env env, napi_callback_info info, int32_t& portIdValue,
    napi_value* buffer, uint32_t& timeoutValue)
{
    size_t argc = ARGC_3;
    napi_value argv[ARGC_3] = {nullptr};
    if (!NapiCheck(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), "Get call back info failed")) {
        return false;
    }
    if (!SerialAssert(env, (argc == ARGC_2 || argc == ARGC_3), SYSPARAM_INVALID_INPUT,
        "The function takes 2 or 3 arguments.")) {
        return false;
    }
    napi_value portId = argv[0];
    napi_valuetype type;
    napi_typeof(env, portId, &type);
    if (!SerialAssert(env, type == napi_number, SYSPARAM_INVALID_INPUT, "The type of portId must be int32_t.")) {
        return false;
    }
    napi_get_value_int32(env, portId, &portIdValue);
    if (!SerialAssert(env, (portIdValue != -1), SYSPARAM_INVALID_INPUT, "Failed to get portId.")) {
        return false;
    }
    if (!IsPortIdExist(portIdValue)) {
        if (!SerialAssert(env, false, SERIAL_PORT_NOT_EXIST, "portid does not exist.")) {
            return false;
        }
    }
    *buffer = argv[1];
    if (argc == ARGC_3) {
        napi_value timeout = argv[2];
        napi_typeof(env, timeout, &type);
        if (!SerialAssert(env, type == napi_number, SYSPARAM_INVALID_INPUT, "The type of timeout must be uint32_t.")) {
            return false;
        }
        napi_get_value_uint32(env, timeout, &timeoutValue);
    }
    return true;
}

static napi_value SerialReadSyncNapi(napi_env env, napi_callback_info info)
{
    USB_HILOGI(MODULE_JS_NAPI, "SerialReadSyncNapi start");
    int32_t portIdValue = -1;
    napi_value buffer;
    uint32_t timeoutValue = 0;
    if (!ReadTransferParams(env, info, portIdValue, &buffer, timeoutValue)) {
        return nullptr;
    }
    napi_typedarray_type arrayType;
    size_t bufferLength;
    void* bufferValue = nullptr;
    NAPI_CALL(env, napi_get_typedarray_info(env, buffer, &arrayType, &bufferLength, &bufferValue, nullptr, nullptr));
    if (!SerialAssert(env, arrayType == napi_uint8_array, SYSPARAM_INVALID_INPUT,
        "The type of buffer must be an array of uint8_t.")) {
        return nullptr;
    }
    std::future<int32_t> resp = std::async(SerialReadHelper, portIdValue, static_cast<uint8_t*>(bufferValue),
        static_cast<uint32_t>(bufferLength));
    std::future_status status = std::future_status::deferred;
    if (timeoutValue == 0) {
        resp.wait();
    } else {
        status = resp.wait_for(std::chrono::milliseconds(timeoutValue));
        if (!SerialAssert(env, status == std::future_status::ready, SERIAL_TIMED_OUT, "read timed out.")) {
            return nullptr;
        }
    }
    napi_value result = nullptr;
    if (!SerialAssert(env, (resp.get() == 0), ErrorCodeConversion(resp.get()), "SerialReadSync Failed.")) {
        return nullptr;
    }

    napi_create_int32(env, resp.get(), &result);
    return result;
}

static auto g_serialReadExecute = [](napi_env env, void* data) {
    SerialReadAsyncContext *context = static_cast<SerialReadAsyncContext *>(data);
    std::future<int32_t> resp = std::async(std::launch::async, SerialReadHelper,
        context->portId, static_cast<uint8_t*>(context->pData), context->size);
    std::future_status status = std::future_status::deferred;
    if (context->timeout == 0) {
        resp.wait();
    } else {
        status = resp.wait_for(std::chrono::milliseconds(context->timeout));
        context->contextErrno = (status != std::future_status::ready?SERIAL_TIMED_OUT:0);
        if (context->contextErrno) {
            USB_HILOGE(MODULE_JS_NAPI, "Read timeout!");
            return;
        }
    }
    context->contextErrno = ErrorCodeConversion(resp.get());
    context->ret = resp.get();
};

static auto g_serialReadComplete = [](napi_env env, napi_status status, void* data) {
    SerialReadAsyncContext *context = static_cast<SerialReadAsyncContext *>(data);
    napi_value result = nullptr;
    if (context->contextErrno) {
        napi_create_int32(env, context->contextErrno, &result);
        napi_reject_deferred(env, context->deferred, result);
    } else {
        napi_create_int32(env, context->ret, &result);
        napi_resolve_deferred(env, context->deferred, result);
    }
    napi_delete_async_work(env, context->work);
    delete context;
};

static napi_value SerialReadNapi(napi_env env, napi_callback_info info)
{
    USB_HILOGI(MODULE_JS_NAPI, "SerialReadNapi start");
    int32_t portIdValue = -1;
    napi_value buffer;
    uint32_t timeoutValue = 0;
    if (!ReadTransferParams(env, info, portIdValue, &buffer, timeoutValue)) {
        return nullptr;
    }
    napi_typedarray_type arrayType;
    size_t bufferLength;
    void* bufferValue = nullptr;
    NAPI_CALL(env, napi_get_typedarray_info(env, buffer, &arrayType, &bufferLength, &bufferValue, nullptr, nullptr));
    if (!SerialAssert(env, arrayType == napi_uint8_array, SYSPARAM_INVALID_INPUT,
        "The type of buffer must be an array of uint8_t.")) {
        return nullptr;
    }
    SerialReadAsyncContext *asyncContext = new (std::nothrow) SerialReadAsyncContext;
    napi_value promise;
    NAPI_CALL(env, napi_create_promise(env, &asyncContext->deferred, &promise));
    asyncContext->portId = portIdValue;
    asyncContext->timeout = timeoutValue;
    asyncContext->contextErrno = 0;
    asyncContext->size = bufferLength;
    asyncContext->pData = bufferValue;
    napi_value resourceName;
    napi_create_string_utf8(env, "SerialRead", NAPI_AUTO_LENGTH, &resourceName);
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, g_serialReadExecute, g_serialReadComplete,
        asyncContext, &asyncContext->work));
    napi_queue_async_work(env, asyncContext->work);
    return promise;
}

static napi_value SerialOpenNapi(napi_env env, napi_callback_info info)
{
    USB_HILOGI(MODULE_JS_NAPI, "serialOpenNapi start");
    size_t argc = ARGC_1;
    napi_value argv[ARGC_1] = {nullptr};
    if (!NapiCheck(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), "Get call back info failed")) {
        return nullptr;
    }
    if (!SerialAssert(env, (argc == ARGC_1), SYSPARAM_INVALID_INPUT, "The function takes 1 arguments.")) {
        return nullptr;
    }
    napi_value portId = argv[0];
    napi_valuetype type;
    napi_typeof(env, portId, &type);
    if (!SerialAssert(env, type == napi_number, SYSPARAM_INVALID_INPUT, "The type of portId must be int32_t.")) {
        return nullptr;
    }
    int32_t portIdValue = -1;
    napi_get_value_int32(env, portId, &portIdValue);
    if (!SerialAssert(env, (portIdValue != -1), SYSPARAM_INVALID_INPUT, "Failed to get portId.")) {
        return nullptr;
    }
    if (!IsPortIdExist(portIdValue)) {
        if (!SerialAssert(env, false, SERIAL_PORT_NOT_EXIST, "portid does not exist.")) {
            return nullptr;
        }
    }
    USB_HILOGE(MODULE_JS_NAPI, "portIdValue: %{public}d", portIdValue);
    int ret = g_usbClient.SerialOpen(portIdValue);
    if (!SerialAssert(env, ret == 0, ErrorCodeConversion(ret), "SerialOpen failed.")) {
        return nullptr;
    }

    return nullptr;
}

static napi_value SerialCloseNapi(napi_env env, napi_callback_info info)
{
    USB_HILOGI(MODULE_JS_NAPI, "SerialCloseNapi start");
    size_t argc = ARGC_1;
    napi_value argv[ARGC_1] = {nullptr};
    if (!NapiCheck(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), "Get call back info failed")) {
        return nullptr;
    }
    if (!SerialAssert(env, (argc == ARGC_1), SYSPARAM_INVALID_INPUT, "The function takes 1 arguments.")) {
        return nullptr;
    }
    napi_value portId = argv[0];
    napi_valuetype type;
    napi_typeof(env, portId, &type);
    if (!SerialAssert(env, type == napi_number, SYSPARAM_INVALID_INPUT, "The type of portId must be int32_t.")) {
        return nullptr;
    }
    int32_t portIdValue = -1;
    napi_get_value_int32(env, portId, &portIdValue);
    if (!SerialAssert(env, (portIdValue != -1), SYSPARAM_INVALID_INPUT, "Failed to get portId.")) {
        return nullptr;
    }
    if (!IsPortIdExist(portIdValue)) {
        if (!SerialAssert(env, false, SERIAL_PORT_NOT_EXIST, "portid does not exist.")) {
            return nullptr;
        }
    }
    
    int ret = g_usbClient.SerialClose(portIdValue);
    if (!SerialAssert(env, ret == 0, ErrorCodeConversion(ret), "SerialClose failed.")) {
        return nullptr;
    }
    return nullptr;
}

static napi_value SerialHasRightNapi(napi_env env, napi_callback_info info)
{
    USB_HILOGI(MODULE_JS_NAPI, "SerialHasRightNapi start");
    size_t argc = ARGC_1;
    napi_value argv[ARGC_1] = {nullptr};
    if (!NapiCheck(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), "Get call back info failed")) {
        return nullptr;
    }
    if (!SerialAssert(env, (argc == ARGC_1), SYSPARAM_INVALID_INPUT, "The function takes 1 arguments.")) {
        return nullptr;
    }
    napi_value portId = argv[0];
    napi_valuetype type;
    napi_typeof(env, portId, &type);
    if (!SerialAssert(env, type == napi_number, SYSPARAM_INVALID_INPUT, "The type of portId must be int32_t.")) {
        return nullptr;
    }
    int32_t portIdValue = -1;
    napi_get_value_int32(env, portId, &portIdValue);
    if (!SerialAssert(env, (portIdValue != -1), SYSPARAM_INVALID_INPUT, "Failed to get portId.")) {
        return nullptr;
    }
    if (!IsPortIdExist(portIdValue)) {
        if (!SerialAssert(env, false, SERIAL_PORT_NOT_EXIST, "portid does not exist.")) {
            return nullptr;
        }
    }
    napi_value result = nullptr;
    bool ret = g_usbClient.HasSerialRight(portIdValue);
    napi_get_boolean(env, ret, &result);
    return result;
}

static napi_value CancelSerialRightNapi(napi_env env, napi_callback_info info)
{
    USB_HILOGI(MODULE_JS_NAPI, "CancelSerialRightNapi start");
    size_t argc = ARGC_1;
    napi_value argv[ARGC_1] = {nullptr};
    if (!NapiCheck(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), "Get call back info failed")) {
        return nullptr;
    }
    if (!SerialAssert(env, (argc == ARGC_1), SYSPARAM_INVALID_INPUT, "The function takes 1 arguments.")) {
        return nullptr;
    }
    napi_value portId = argv[0];
    napi_valuetype type;
    napi_typeof(env, portId, &type);
    if (!SerialAssert(env, type == napi_number, SYSPARAM_INVALID_INPUT, "The type of portId must be int32_t.")) {
        return nullptr;
    }
    int32_t portIdValue = -1;
    napi_get_value_int32(env, portId, &portIdValue);
    if (!SerialAssert(env, (portIdValue != -1), SYSPARAM_INVALID_INPUT, "Failed to get portId.")) {
        return nullptr;
    }
    if (!IsPortIdExist(portIdValue)) {
        if (!SerialAssert(env, false, SERIAL_PORT_NOT_EXIST, "portid does not exist.")) {
            return nullptr;
        }
    }
    int32_t ret = g_usbClient.CancelSerialRight(portIdValue);
    if (!SerialAssert(env, ret == 0, ErrorCodeConversion(ret), "SerialRemoveRight failed.")) {
        return nullptr;
    }
    return nullptr;
}

static napi_value SerialAddRightNapi(napi_env env, napi_callback_info info)
{
    USB_HILOGI(MODULE_JS_NAPI, "SerialAddRightNapi start");
    size_t argc = ARGC_2;
    napi_value argv[ARGC_2] = {nullptr};
    if (!NapiCheck(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), "Get call back info failed")) {
        return nullptr;
    }
    if (!SerialAssert(env, (argc == ARGC_2), SYSPARAM_INVALID_INPUT, "The function takes 2 arguments.")) {
        return nullptr;
    }
    napi_value tokenId = argv[0];
    napi_valuetype type;
    napi_typeof(env, tokenId, &type);
    if (!SerialAssert(env, type == napi_number, SYSPARAM_INVALID_INPUT, "The type of tokenId must be uint32_t.")) {
        return nullptr;
    }
    uint32_t tokenIdValue = -1;
    napi_get_value_uint32(env, tokenId, &tokenIdValue);
    if (!SerialAssert(env, (tokenIdValue != -1), SYSPARAM_INVALID_INPUT, "Failed to get tokenId.")) {
        return nullptr;
    }
    napi_value portId = argv[1];
    napi_typeof(env, portId, &type);
    if (!SerialAssert(env, type == napi_number, SYSPARAM_INVALID_INPUT, "The type of portId must be int32_t.")) {
        return nullptr;
    }
    int32_t portIdValue = -1;
    napi_get_value_int32(env, portId, &portIdValue);
    if (!SerialAssert(env, (portIdValue != -1), SYSPARAM_INVALID_INPUT, "Failed to get portId.")) {
        return nullptr;
    }
    int32_t ret = g_usbClient.AddSerialRight(tokenIdValue, portIdValue);
    if (!SerialAssert(env, ret == 0, ErrorCodeConversion(ret), "SerialAddRight failed.")) {
        return nullptr;
    }
    return nullptr;
}

static auto g_serialRequestRightExecute = [](napi_env env, void* data) {
    SerialRequestRightAsyncContext *asyncContext = static_cast<SerialRequestRightAsyncContext *>(data);
    int32_t ret = g_usbClient.RequestSerialRight(asyncContext->portIdValue);
    asyncContext->ret = ErrorCodeConversion(ret);
};

static auto g_serialRequestRightComplete = [](napi_env env, napi_status status, void* data) {
    SerialRequestRightAsyncContext *asyncContext = static_cast<SerialRequestRightAsyncContext *>(data);
    napi_value result = nullptr;
    if (asyncContext->contextErrno) {
        USB_HILOGE(MODULE_JS_NAPI, "Failed to request serial right");
        napi_get_boolean(env, false, &result);
        napi_reject_deferred(env, asyncContext->deferred, result);
    } else {
        napi_get_boolean(env, !asyncContext->ret, &result);
        napi_resolve_deferred(env, asyncContext->deferred, result);
    }
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
};

static napi_value SerialRequestRightNapi(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_1;
    napi_value argv[ARGC_1] = {nullptr};
    if (!NapiCheck(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), "Get call back info failed")) {
        return nullptr;
    }
    if (!SerialAssert(env, (argc == ARGC_1), SYSPARAM_INVALID_INPUT, "The function takes 1 arguments.")) {
        return nullptr;
    }
    napi_value portId = argv[0];
    napi_valuetype type;
    napi_typeof(env, portId, &type);
    if (!SerialAssert(env, type == napi_number, SYSPARAM_INVALID_INPUT, "The type of portId must be int32_t.")) {
        return nullptr;
    }
    int32_t portIdValue = -1;
    napi_get_value_int32(env, portId, &portIdValue);
    if (!SerialAssert(env, (portIdValue != -1), SYSPARAM_INVALID_INPUT, "Failed to get portId.")) {
        return nullptr;
    }
    if (!IsPortIdExist(portIdValue)) {
        if (!SerialAssert(env, false, SERIAL_PORT_NOT_EXIST, "portid does not exist.")) {
            return nullptr;
        }
    }
    SerialRequestRightAsyncContext* asyncContext = new (std::nothrow) SerialRequestRightAsyncContext;
    asyncContext->portIdValue = portIdValue;
    asyncContext->contextErrno = 0;
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &asyncContext->deferred, &result));
    napi_value resourceName;
    napi_create_string_utf8(env, "SerialRequestRight", NAPI_AUTO_LENGTH, &resourceName);
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, g_serialRequestRightExecute,
            g_serialRequestRightComplete, static_cast<void*>(asyncContext), &asyncContext->work));
    napi_queue_async_work(env, asyncContext->work);
    return result;
}

static napi_value DeclareEnum(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_50", ToInt32Value(env, BAUDRATE_50)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_75", ToInt32Value(env, BAUDRATE_75)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_110", ToInt32Value(env, BAUDRATE_110)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_134", ToInt32Value(env, BAUDRATE_134)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_150", ToInt32Value(env, BAUDRATE_150)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_200", ToInt32Value(env, BAUDRATE_200)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_300", ToInt32Value(env, BAUDRATE_300)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_600", ToInt32Value(env, BAUDRATE_600)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_1200", ToInt32Value(env, BAUDRATE_1200)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_1800", ToInt32Value(env, BAUDRATE_1800)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_2400", ToInt32Value(env, BAUDRATE_2400)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_4800", ToInt32Value(env, BAUDRATE_4800)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_9600", ToInt32Value(env, BAUDRATE_9600)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_19200", ToInt32Value(env, BAUDRATE_19200)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_38400", ToInt32Value(env, BAUDRATE_38400)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_57600", ToInt32Value(env, BAUDRATE_57600)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_115200", ToInt32Value(env, BAUDRATE_115200)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_230400", ToInt32Value(env, BAUDRATE_230400)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_460800", ToInt32Value(env, BAUDRATE_460800)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_500000", ToInt32Value(env, BAUDRATE_500000)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_576000", ToInt32Value(env, BAUDRATE_576000)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_921600", ToInt32Value(env, BAUDRATE_921600)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_1000000", ToInt32Value(env, BAUDRATE_1000000)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_1152000", ToInt32Value(env, BAUDRATE_1152000)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_1500000", ToInt32Value(env, BAUDRATE_1500000)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_2000000", ToInt32Value(env, BAUDRATE_2000000)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_2500000", ToInt32Value(env, BAUDRATE_2500000)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_3000000", ToInt32Value(env, BAUDRATE_3000000)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_3500000", ToInt32Value(env, BAUDRATE_3500000)),
        DECLARE_NAPI_STATIC_PROPERTY("BAUDRATE_4000000", ToInt32Value(env, BAUDRATE_4000000)),

        DECLARE_NAPI_STATIC_PROPERTY("USB_ATTR_DATABIT_8", ToInt32Value(env, USB_ATTR_DATABIT_8)),
        DECLARE_NAPI_STATIC_PROPERTY("USB_ATTR_DATABIT_7", ToInt32Value(env, USB_ATTR_DATABIT_7)),
        DECLARE_NAPI_STATIC_PROPERTY("USB_ATTR_DATABIT_6", ToInt32Value(env, USB_ATTR_DATABIT_6)),
        DECLARE_NAPI_STATIC_PROPERTY("USB_ATTR_DATABIT_5", ToInt32Value(env, USB_ATTR_DATABIT_5)),
        DECLARE_NAPI_STATIC_PROPERTY("USB_ATTR_DATABIT_4", ToInt32Value(env, USB_ATTR_DATABIT_4)),

        DECLARE_NAPI_STATIC_PROPERTY("USB_ATTR_PARITY_NONE", ToInt32Value(env, USB_ATTR_PARITY_NONE)),
        DECLARE_NAPI_STATIC_PROPERTY("USB_ATTR_PARITY_ODD", ToInt32Value(env, USB_ATTR_PARITY_ODD)),
        DECLARE_NAPI_STATIC_PROPERTY("USB_ATTR_PARITY_EVEN", ToInt32Value(env, USB_ATTR_PARITY_EVEN)),
        DECLARE_NAPI_STATIC_PROPERTY("USB_ATTR_PARITY_MARK", ToInt32Value(env, USB_ATTR_PARITY_MARK)),
        DECLARE_NAPI_STATIC_PROPERTY("USB_ATTR_PARITY_SPACE", ToInt32Value(env, USB_ATTR_PARITY_SPACE)),

        DECLARE_NAPI_STATIC_PROPERTY("USB_ATTR_STOPBIT_1", ToInt32Value(env, USB_ATTR_STOPBIT_1)),
        DECLARE_NAPI_STATIC_PROPERTY("USB_ATTR_STOPBIT_1P5", ToInt32Value(env, USB_ATTR_STOPBIT_1P5)),
        DECLARE_NAPI_STATIC_PROPERTY("USB_ATTR_STOPBIT_2", ToInt32Value(env, USB_ATTR_STOPBIT_2)),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

EXTERN_C_START
/*
 * function for module exports
 */
napi_value SerialInit(napi_env env, napi_value exports)
{
    USB_HILOGD(MODULE_JS_NAPI, "enter");
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getPortList", SerialGetPortListNapi),
        DECLARE_NAPI_FUNCTION("open", SerialOpenNapi),
        DECLARE_NAPI_FUNCTION("close", SerialCloseNapi),
        DECLARE_NAPI_FUNCTION("read", SerialReadNapi),
        DECLARE_NAPI_FUNCTION("readSync", SerialReadSyncNapi),
        DECLARE_NAPI_FUNCTION("write", SerialWriteNapi),
        DECLARE_NAPI_FUNCTION("writeSync", SerialWriteSyncNapi),
        DECLARE_NAPI_FUNCTION("getAttribute", SerialGetAttributeNapi),
        DECLARE_NAPI_FUNCTION("setAttribute", SerialSetAttributeNapi),
        DECLARE_NAPI_FUNCTION("hasSerialRight", SerialHasRightNapi),
        DECLARE_NAPI_FUNCTION("requestSerialRight", SerialRequestRightNapi),
        DECLARE_NAPI_FUNCTION("addSerialRight", SerialAddRightNapi),
        DECLARE_NAPI_FUNCTION("cancelSerialRight", CancelSerialRightNapi),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    DeclareEnum(env, exports);
    USB_HILOGD(MODULE_JS_NAPI, "return");
    return exports;
}
EXTERN_C_END
