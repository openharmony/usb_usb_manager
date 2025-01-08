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

#include "usbmgrsubmittransfer_fuzzer.h"

#include "securec.h"

#include "ashmem.h"
#include "usb_errors.h"
#include "usb_common_fuzz.h"

using namespace OHOS::HDI::Usb::V1_2;

namespace OHOS {
const int32_t ASHMEM_MAX_SIZE = 1024;
const int32_t BITS_PER_BYTE = 8;
const int32_t NUM_THREE = 3;
const int32_t NUM_TWO = 2;
const int32_t NUM_ONE = 1;
constexpr size_t THRESHOLD = 10;
const int32_t ASYNC_TRANSFER_TIME_OUT = 1000;
const int32_t LIBUSB_TRANSFER_TYPE_BULK = 2;
namespace USB {
    uint32_t Convert2Uint32(const uint8_t *ptr)
    {
        if (ptr == nullptr) {
            return 0;
        }
        /*
        * Move the 0th digit 24 to the left, the first digit 16 to the left, the second digit 8 to the left,
        * and the third digit no left
        */
        return (ptr[0] << BITS_PER_BYTE * NUM_THREE) | (ptr[NUM_ONE] << BITS_PER_BYTE * NUM_TWO) |
            (ptr[NUM_TWO] << BITS_PER_BYTE) | (ptr[NUM_THREE]);
    }

    bool InitAshmem(sptr<Ashmem> &asmptr, int32_t asmSize, uint8_t rflg)
    {
        asmptr = Ashmem::CreateAshmem("ttashmem000", asmSize);
        if (asmptr == nullptr) {
            USB_HILOGE(MODULE_USB_SERVICE, "InitAshmem CreateAshmem failed");
            return false;
        }

        asmptr->MapReadAndWriteAshmem();

        if (rflg == 0) {
            uint8_t tdata[ASHMEM_MAX_SIZE];
            int32_t offset = 0;
            int32_t tlen = 0;

            // std::fill(std::begin(tdata), std::end(tdata), 'Y');
            int32_t retSafe = memset_s(tdata, sizeof(tdata), 'Y', ASHMEM_MAX_SIZE);
            if (retSafe != EOK) {
                USB_HILOGI(MODULE_USB_SERVICE, "InitAshmemOne memset_s failed");
                return false;
            }

            while (offset < asmSize) {
                tlen = (asmSize - offset) < ASHMEM_MAX_SIZE ? (asmSize - offset) : ASHMEM_MAX_SIZE;
                asmptr->WriteToAshmem(tdata, tlen, offset);
                offset += tlen;
            }
        }
        asmptr->UnmapAshmem();
        return true;
    }

    bool UsbMgrSubmitTransferFuzzTest(const uint8_t *data, size_t /* size */)
    {
        if (data == nullptr) {
            USB_HILOGE(MODULE_USB_SERVICE, "data is null");
            return false;
        }
        uint32_t ashmemSize = Convert2Uint32(data);

        sptr<Ashmem> ashmPtr = nullptr;
        InitAshmem(ashmPtr, ashmemSize, 0);
        auto[res, pipe, interface] = UsbMgrPrepareFuzzEnv();
        if (!res) {
            USB_HILOGE(MODULE_USB_SERVICE, "prepare error");
            return false;
        }

        USBTransferInfo usbInfo;
        usbInfo.endpoint = 0x1;
        usbInfo.flags = 0;
        usbInfo.type = LIBUSB_TRANSFER_TYPE_BULK;
        usbInfo.timeOut = ASYNC_TRANSFER_TIME_OUT;
        usbInfo.userData = 0;
        usbInfo.numIsoPackets = 0;
        usbInfo.length = 0;

        static auto func = [] () -> void {
            USB_HILOGI(MODULE_USB_SERVICE, "enter fuzz func callback");
            return;
        };

        auto &usbSrvClient = UsbSrvClient::GetInstance();

        if (usbSrvClient.UsbSubmitTransfer(const_cast<USBDevicePipe &>(pipe), usbInfo, nullptr, ashmPtr)) {
            return false;
        }
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
    OHOS::USB::UsbMgrSubmitTransferFuzzTest(data, size);
    return 0;
}
