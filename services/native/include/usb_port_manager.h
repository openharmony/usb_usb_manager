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

#ifndef USB_PORT_MANAGER_H
#define USB_PORT_MANAGER_H

#include <algorithm>
#include <map>
#include <mutex>
#include <string>
#include <vector>
#include "usb_common.h"
#include "usb_port.h"
#include "v1_0/iusb_interface.h"

namespace OHOS {
namespace USB {
class UsbPortManager {
public:
    UsbPortManager();
    ~UsbPortManager();

    void Init();
    void Init(int32_t test);
    int32_t GetPorts(std::vector<UsbPort> &ports);
    int32_t GetSupportedModes(int32_t portId, int32_t &supportedModes);
    void SetPortRoles(int32_t portId, int32_t powerRole, int32_t dataRole);
    int32_t QueryPort();
    void UpdatePort(int32_t portId, int32_t powerRole, int32_t dataRole, int32_t mode);
    void AddPort(UsbPort &port);
    void RemovePort(int32_t portId);
    bool Dump(int fd, const std::string &args);

private:
    void ReportPortRoleChangeSysEvent(
        int32_t currentPowerRole, int32_t updatePowerRole, int32_t currentDataRole, int32_t updateDataRole);
    std::mutex mutex_;
    std::map<int32_t, UsbPort> portMap_;
    sptr<HDI::Usb::V1_0::IUsbInterface> usbd_ = nullptr;
};
} // namespace USB
} // namespace OHOS

#endif
