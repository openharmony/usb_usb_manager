/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef USB_FUNCTION_MANAGER_H
#define USB_FUNCTION_MANAGER_H

#include <map>
#include <string>
#include <vector>

#include "usb_common.h"
#include "usb_srv_support.h"
#include "v1_0/iusb_interface.h"
#include "v1_0/iusbd_subscriber.h"

namespace OHOS {
namespace USB {
class UsbDeviceManager {
public:
    UsbDeviceManager();
    static bool AreSettableFunctions(int32_t funcs);

    static uint32_t ConvertFromString(std::string_view funcs);
    static std::string ConvertToString(uint32_t func);
    void UpdateFunctions(int32_t func);
    int32_t GetCurrentFunctions();
    void HandleEvent(int32_t status);
    bool Dump(int fd, const std::string &args);

private:
    void ReportFuncChangeSysEvent(int32_t currentFunctions, int32_t updateFunctions);
    void ReportDevicePlugSysEvent(int32_t currentFunctions, bool connected);
    static constexpr uint32_t functionSettable_ =
        UsbSrvSupport::FUNCTION_HDC | UsbSrvSupport::FUNCTION_ACM | UsbSrvSupport::FUNCTION_ECM |
        UsbSrvSupport::FUNCTION_RNDIS | UsbSrvSupport::FUNCTION_STORAGE;
    static const std::map<std::string_view, uint32_t> FUNCTION_MAPPING_N2C;
    int32_t currentFunctions_ {UsbSrvSupport::FUNCTION_HDC};
    bool connected_ {false};
    sptr<HDI::Usb::V1_0::IUsbInterface> usbd_ = nullptr;
};
} // namespace USB
} // namespace OHOS

#endif // USB_FUNCTION_MANAGER_H
