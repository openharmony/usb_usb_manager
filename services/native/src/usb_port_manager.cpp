/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "usb_port_manager.h"
#include "hisysevent.h"
#include "usb_errors.h"

using namespace OHOS::HiviewDFX;
using namespace OHOS::HDI::Usb::V1_0;

namespace OHOS {
namespace USB {
const int32_t SUPPORTED_MODES = 3;

UsbPortManager::UsbPortManager()
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbPortManager::Init start");
    usbd_ = IUsbInterface::Get();
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbPortManager::Get inteface failed");
    }
}

UsbPortManager::~UsbPortManager()
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbPortManager::unInit start");
}

void UsbPortManager::Init()
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbPortManager::QueryPort start");
    int32_t ret = QueryPort();
    if (ret) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbPortManager::QueryPort false");
    }
}

int32_t UsbPortManager::GetPorts(std::vector<UsbPort> &ports)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (portMap_.size() > 0) {
        for (auto it = portMap_.begin(); it != portMap_.end(); ++it) {
            ports.push_back(it->second);
        }
        USB_HILOGI(MODULE_USB_SERVICE, "UsbPortManager::GetPorts success");
        return UEC_OK;
    } else {
        int32_t ret = QueryPort();
        if (ret == UEC_OK) {
            for (auto it = portMap_.begin(); it != portMap_.end(); ++it) {
                ports.push_back(it->second);
            }
            USB_HILOGI(MODULE_USB_SERVICE, "UsbPortManager::QueryPort and GetPorts success");
            return ret;
        }
    }
    USB_HILOGE(MODULE_USB_SERVICE, "UsbPortManager::GetPorts false");
    return UEC_SERVICE_INVALID_VALUE;
}

int32_t UsbPortManager::GetSupportedModes(int32_t portId, int32_t &supportedModes)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = portMap_.find(portId);
    if (it != portMap_.end()) {
        supportedModes = it->second.supportedModes;
        USB_HILOGI(MODULE_USB_SERVICE, "UsbPortManager::GetSupportedModes success");
        return UEC_OK;
    }
    USB_HILOGE(MODULE_USB_SERVICE, "UsbPortManager::GetSupportedModes false");
    return UEC_SERVICE_INVALID_VALUE;
}

int32_t UsbPortManager::QueryPort()
{
    int32_t portId = 0;
    int32_t powerRole = 0;
    int32_t dataRole = 0;
    int32_t mode = 0;

    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbPortManager::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbd_->QueryPort(portId, powerRole, dataRole, mode);
    USB_HILOGI(MODULE_USB_SERVICE, "portId:%{public}d powerRole:%{public}d dataRole:%{public}d mode:%{public}d ",
        portId, powerRole, dataRole, mode);
    if (ret) {
        USB_HILOGE(MODULE_USB_SERVICE, "Get().queryPorts failed");
        return ret;
    }
    UsbPortStatus usbPortStatus;
    UsbPort usbPort;
    usbPortStatus.currentMode = mode;
    usbPortStatus.currentDataRole = dataRole;
    usbPortStatus.currentPowerRole = powerRole;
    usbPort.id = portId;
    usbPort.supportedModes = SUPPORTED_MODES;
    usbPort.usbPortStatus = usbPortStatus;
    AddPort(usbPort);
    return ret;
}

void UsbPortManager::UpdatePort(int32_t portId, int32_t powerRole, int32_t dataRole, int32_t mode)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbPortManager::updatePort run");
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = portMap_.find(portId);
    if (it != portMap_.end()) {
        if (it->second.id == portId) {
            ReportPortRoleChangeSysEvent(it->second.usbPortStatus.currentPowerRole, powerRole,
                it->second.usbPortStatus.currentDataRole, dataRole);
            it->second.usbPortStatus.currentPowerRole = powerRole;
            it->second.usbPortStatus.currentDataRole = dataRole;
            it->second.usbPortStatus.currentMode = mode;
            USB_HILOGI(MODULE_USB_SERVICE, "UsbPortManager::updatePort seccess");
            return;
        }
    }
    USB_HILOGE(MODULE_USB_SERVICE, "updatePort false");
}

void UsbPortManager::AddPort(UsbPort &port)
{
    USB_HILOGI(MODULE_USB_SERVICE, "addPort run");
    auto res = portMap_.insert(std::map<int32_t, UsbPort>::value_type(port.id, port));
    if (!res.second) {
        USB_HILOGW(MODULE_USB_SERVICE, "addPort port id duplicated");
        return;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "addPort successed");
}

void UsbPortManager::RemovePort(int32_t portId)
{
    USB_HILOGI(MODULE_USB_SERVICE, "removePort run");
    std::lock_guard<std::mutex> lock(mutex_);
    size_t num = portMap_.erase(portId);
    if (num == 0) {
        USB_HILOGW(MODULE_USB_SERVICE, "removePort false");
        return;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "removePort seccess");
}

bool UsbPortManager::Dump(int fd, const std::string &args)
{
    if (args.compare("-a") != 0) {
        dprintf(fd, "args is not -a\n");
        return false;
    }

    dprintf(fd, "Usb Port device status:\n");
    std::vector<UsbPort> ports;
    if (GetPorts(ports) != UEC_OK) {
        dprintf(fd, "UsbPortManager::GetPorts failed, port is empty\n");
        return UEC_SERVICE_INVALID_VALUE;
    }

    for (size_t i = 0; i < ports.size(); ++i) {
        dprintf(fd, "id: %d | supportedModes: %d | currentMode: %d | currentPowerRole: %d | currentDataRole: %d\n",
            ports[i].id, ports[i].supportedModes, ports[i].usbPortStatus.currentMode,
            ports[i].usbPortStatus.currentPowerRole, ports[i].usbPortStatus.currentDataRole);
    }
    return true;
}

void UsbPortManager::ReportPortRoleChangeSysEvent(
    int32_t currentPowerRole, int32_t updatePowerRole, int32_t currentDataRole, int32_t updateDataRole)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Port switch points are as follows:");
    HiSysEvent::Write("USB", "USB_PORT_ROLE_CHANGED", HiSysEvent::EventType::BEHAVIOR, "CURRENT_POWERROLE",
        currentPowerRole, "UPDATE_POWERROLE", updatePowerRole, "CURRENT_DATAROLE", currentDataRole, "UPDATE_DATAROLE",
        updateDataRole);
}
} // namespace USB
} // namespace OHOS
