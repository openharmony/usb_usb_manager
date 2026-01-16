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

#include "usb_service.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "usbmgrprivate_fuzzer.h"
#include "usb_errors.h"

namespace OHOS {
const uint32_t OFFSET = 2;
namespace USB {
    void GetDeviceVidPidSerialNumberFuzzTest(const uint8_t* rawData, size_t size)
    {
        if (rawData == nullptr || size < OFFSET) {
            return;
        }

        auto serviceInstance = UsbService::GetGlobalInstance();
        uint8_t busNum = reinterpret_cast<const uint8_t &>(rawData);
        uint8_t devAddr = reinterpret_cast<const uint8_t &>(std::move(rawData + sizeof(uint8_t)));
        std::string name = std::to_string(busNum) + "-" + std::to_string(devAddr);
        std::string uniqueName = std::to_string(busNum) + "+" + std::to_string(devAddr);
        std::string deviceName = name;
        std::string strDesc;
        serviceInstance->deviceVidPidMap_.insert(std::pair<std::string, std::string>(name, uniqueName));
        if (serviceInstance->GetDeviceVidPidSerialNumber(deviceName, strDesc) != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "can not find deviceName.");
        }
    }

    void UsbDeviceTypeChangeFuzzTest(const uint8_t* rawData, size_t size)
    {
        if (rawData == nullptr || size < sizeof(UsbDeviceType) + sizeof(UsbDeviceTypeInfo)) {
            return;
        }
        auto serviceInstance = UsbService::GetGlobalInstance();
        UsbDeviceType info = *reinterpret_cast<const UsbDeviceType *>(rawData);
        std::vector<UsbDeviceType> disableType;
        std::vector<UsbDeviceTypeInfo> deviceTypes;
        disableType.push_back(info);
        serviceInstance->UsbDeviceTypeChange(disableType, deviceTypes);
    }

    bool UsbMgrPrivateFuzzTest(const uint8_t* rawData, size_t size)
    {
        if (rawData == nullptr) {
            return false;
        }

        GetDeviceVidPidSerialNumberFuzzTest(rawData, size);
        UsbDeviceTypeChangeFuzzTest(rawData, size);

        return true;
    }
} // USB
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::USB::UsbMgrPrivateFuzzTest(data, size);
    return 0;
}
