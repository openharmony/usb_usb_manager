/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "usbmgrdetachkerneldriver_fuzzer.h"

#include "usb_errors.h"
#include "usb_srv_client.h"
#include <cstddef>
#include <cstdint>
#include "usb_service.h"

using namespace OHOS::USB;

namespace OHOS {
const uint32_t OFFSET = 4;
constexpr size_t THRESHOLD = 10;
enum class UsbInterfaceCode {
    USB_FUN_HAS_RIGHT = 0,
    USB_FUN_REQUEST_RIGHT,
    USB_FUN_REMOVE_RIGHT,
    USB_FUN_OPEN_DEVICE,
    USB_FUN_RESET_DEVICE,
    USB_FUN_GET_DEVICE,
    USB_FUN_GET_DEVICES,
    USB_FUN_GET_CURRENT_FUNCTIONS,
    USB_FUN_SET_CURRENT_FUNCTIONS,
    USB_FUN_USB_FUNCTIONS_FROM_STRING,
    USB_FUN_USB_FUNCTIONS_TO_STRING,
    USB_FUN_CLAIM_INTERFACE,
    USB_FUN_RELEASE_INTERFACE,
    USB_FUN_BULK_TRANSFER_READ,
    USB_FUN_BULK_TRANSFER_WRITE,
    USB_FUN_CONTROL_TRANSFER,
    USB_FUN_USB_CONTROL_TRANSFER,
    USB_FUN_SET_ACTIVE_CONFIG,
    USB_FUN_GET_ACTIVE_CONFIG,
    USB_FUN_SET_INTERFACE,
    USB_FUN_GET_PORTS,
    USB_FUN_GET_SUPPORTED_MODES,
    USB_FUN_SET_PORT_ROLE,
    USB_FUN_REQUEST_QUEUE,
    USB_FUN_REQUEST_WAIT,
    USB_FUN_REQUEST_CANCEL,
    USB_FUN_GET_DESCRIPTOR,
    USB_FUN_GET_FILEDESCRIPTOR,
    USB_FUN_CLOSE_DEVICE,
    USB_FUN_BULK_AYSNC_READ,
    USB_FUN_BULK_AYSNC_WRITE,
    USB_FUN_BULK_AYSNC_CANCEL,
    USB_FUN_REG_BULK_CALLBACK,
    USB_FUN_UNREG_BULK_CALLBACK,
    USB_FUN_ADD_RIGHT,
    USB_FUN_DISABLE_GLOBAL_INTERFACE,
    USB_FUN_DISABLE_DEVICE,
    USB_FUN_DISABLE_INTERFACE_TYPE,
    USB_FUN_CLEAR_HALT,
    USB_FUN_GET_DEVICE_SPEED,
    USB_FUN_GET_DRIVER_ACTIVE_STATUS,
    USB_FUN_ADD_ACCESS_RIGHT,
    USB_FUN_BULK_TRANSFER_READ_WITH_LENGTH,
    USB_FUN_ATTACH_KERNEL_DRIVER,
    USB_FUN_DETACH_KERNEL_DRIVER,
};
const std::u16string USB_INTERFACE_TOKEN = u"ohos.usb.IUsbSrv";

bool DoSomethingInterestingWithMyAPI(const uint8_t *rawData, size_t size)
    {
        if (rawData == nullptr) {
            return false;
        }
        rawData = rawData + OFFSET;
        size = size - OFFSET;

        uint32_t code = static_cast<uint32_t>(UsbInterfaceCode::USB_FUN_DETACH_KERNEL_DRIVER);
        MessageParcel data;
        data.WriteInterfaceToken(USB_INTERFACE_TOKEN);
        data.WriteBuffer(rawData, size);
        data.RewindRead(0);
        MessageParcel reply;
        MessageOption option;
        DelayedSpSingleton<UsbService>::GetInstance()->OnRemoteRequest(code, data, reply, option);

        return true;
    }
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::THRESHOLD) {
        return 0;
    }
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
