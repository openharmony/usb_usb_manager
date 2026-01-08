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

#include "usb_service.h"
#include "usbmgrcontroltransfer_fuzzer.h"
#include "usb_srv_client.h"

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "usb_errors.h"

using namespace OHOS::HDI::Usb::V1_0;
using namespace OHOS::HDI::Usb::V1_2;
namespace OHOS {
const uint32_t OFFSET = 4;
constexpr size_t THRESHOLD = 10;
const uint32_t code = 0x18;
const std::u16string USB_INTERFACE_TOKEN = u"OHOS.USB.IUsbServer";
namespace USB {
    bool UsbMgrControlTransferFuzzTest(const uint8_t* rawData, size_t size)
    {
        if (rawData == nullptr || size < sizeof(USBDevicePipe) || size < OFFSET + sizeof(UsbCtrlTransfer)) {
            USB_HILOGE(MODULE_USB_SERVICE, "rawData size is insufficient!");
            return false;
        }

        MessageParcel data;
        data.WriteInterfaceToken(USB_INTERFACE_TOKEN);
        data.WriteBuffer(rawData, size);
        data.RewindRead(0);
        MessageParcel reply;
        MessageOption option;
        UsbService::GetGlobalInstance()->OnRemoteRequest(code, data, reply, option);
        return true;
    }
} // USB
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::THRESHOLD) {
        return 0;
    }
    /* Run your code on data */
    OHOS::USB::UsbMgrControlTransferFuzzTest(data, size);
    return 0;
}

