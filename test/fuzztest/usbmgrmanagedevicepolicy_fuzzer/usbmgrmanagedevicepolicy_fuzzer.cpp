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

#include "usbmgrmanagedevicepolicy_fuzzer.h"
#include "usb_srv_client.h"
#include "usb_errors.h"
#include "usb_common_test.h"

namespace OHOS {
constexpr int32_t OFFSET = 4;
constexpr size_t THRESHOLD = 10;
namespace USB {
bool UsbMgrManageDevicePolicyFuzzTest(const uint8_t* data, size_t size)
{
    Common::UsbCommonTest::GrantPermissionSysNative();
    if (data == nullptr || size < OFFSET + sizeof(int32_t)) {
        USB_HILOGE(MODULE_USB_SERVICE, "data size is insufficient!");
        return false;
    }
    std::vector<UsbDevice> devList;
    std::vector<UsbDeviceId> trustList;
    UsbDeviceId devId{*reinterpret_cast<const int32_t *>(data), *reinterpret_cast<const int32_t *>(data + OFFSET)};
    auto &usbSrvClient = UsbSrvClient::GetInstance();
    auto ret = usbSrvClient.GetDevices(devList);
    if (ret != UEC_OK || devList.empty()) {
        USB_HILOGE(MODULE_USB_SERVICE, "get devices failed ret=%{public}d", ret);
        return false;
    }
    ret = usbSrvClient.ManageDevicePolicy(trustList);
    if (ret != UEC_OK) {
        return false;
    }
    trustList.emplace_back(devId);
    ret = usbSrvClient.ManageDevicePolicy(trustList);
    if (ret != UEC_OK) {
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
    OHOS::USB::UsbMgrManageDevicePolicyFuzzTest(data, size);
    return 0;
}

