/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#ifdef USB_MANAGER_V2_0
#include "v2_0/iusb_port_interface.h"
#include "usb_manager_subscriber.h"
#include "mem_mgr_proxy.h"
#include "mem_mgr_client.h"
#include "system_ability_definition.h"
#endif // USB_MANAGER_V2_0

namespace OHOS {
namespace USB {
class UsbPortManager {
public:
    UsbPortManager();
    ~UsbPortManager();

    void Init();
    void Init(int32_t test);
#ifdef USB_MANAGER_V2_0
    bool InitUsbPortInterface();
    void Stop();
    int32_t BindUsbdSubscriber(const sptr<HDI::Usb::V2_0::IUsbdSubscriber> &subscriber);
    int32_t UnbindUsbdSubscriber(const sptr<HDI::Usb::V2_0::IUsbdSubscriber> &subscriber);
#endif // USB_MANAGER_V2_0
    int32_t SetUsbd(const sptr<HDI::Usb::V1_0::IUsbInterface> &usbd);
    int32_t GetPorts(std::vector<UsbPort> &ports);
    int32_t GetSupportedModes(int32_t portId, int32_t &supportedModes);
    int32_t QueryPort();
    void UpdatePort(int32_t portId, int32_t powerRole, int32_t dataRole, int32_t mode);
    void UpdatePort(int32_t portId, int32_t powerRole, int32_t dataRole, int32_t mode, int32_t supportedModes);
    void AddPort(UsbPort &port);
    void RemovePort(int32_t portId);
    void GetDumpHelp(int32_t fd);
    void Dump(int32_t fd, const std::vector<std::string> &args);

    int32_t SetPortRole(int32_t portId, int32_t powerRole, int32_t dataRole);
private:
    void GetIUsbInterface();
    void GetPortsInfo(int32_t fd);
    void DumpGetSupportPort(int32_t fd);
    void DumpSetPortRoles(int32_t fd, const std::string &args);
    void ReportPortRoleChangeSysEvent(
        int32_t currentPowerRole, int32_t updatePowerRole, int32_t currentDataRole, int32_t updateDataRole);
    void AddPortInfo(int32_t portId, int32_t supportedModes,
        int32_t currentMode, int32_t currentDataRole, int32_t currentPowerRole);
    void AddSupportedMode();
    std::mutex mutex_;
    std::map<int32_t, UsbPort> portMap_;
    std::map<int32_t, int32_t> supportedModeMap_;
    sptr<HDI::Usb::V1_0::IUsbInterface> usbd_ = nullptr;
#ifdef USB_MANAGER_V2_0
    sptr<HDI::Usb::V2_0::IUsbPortInterface> usbPortInterface_ = nullptr;
    sptr<UsbManagerSubscriber> usbManagerSubscriber_;
    sptr<IRemoteObject::DeathRecipient> recipient_ = nullptr;

    class UsbdPortDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;
    };
#endif // USB_MANAGER_V2_0
};
} // namespace USB
} // namespace OHOS

#endif
