/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http:www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <regex>
#include <unistd.h>
#include "hisysevent.h"
#include "serial_errors.h"
#include "securec.h"
#include "serial_manager.h"
#include "hilog_wrapper.h"

#define TWOARGES 2
#ifdef SERIAL_MOCK
static std::vector<uint8_t> g_mockBuffer = {'s', 'e', 'r', 'i', 'a', 'l', '\0'};
static const size_t BUFFER_CAPACITY = 30;
#endif

using namespace OHOS::HiviewDFX;
using OHOS::USB::USB_MGR_LABEL;
using OHOS::USB::MODULE_USB_SERVICE;
using OHOS::USB::UEC_MANAGER_PORT_REPEAT_OPEN;
using OHOS::USB::UEC_MANAGER_PORT_REPEAT_CLOSE;
using OHOS::USB::UEC_MANAGER_PORT_NOT_OPEN;
using OHOS::USB::UEC_OK;

namespace OHOS {
namespace SERIAL {
constexpr int32_t PARAM_COUNT_ONE = 1;

SerialManager::SerialManager()
{
    serial_ = OHOS::HDI::Usb::Serial::V1_0::ISerialInterface::Get("serial_interface_service", true);
    if (serial_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "SerialManager::SerialManager serial_ is null");
    }
}

SerialManager::~SerialManager()
{
    USB_HILOGI(MODULE_USB_SERVICE, "SerialManager::~SerialManager start");
}

int32_t SerialManager::SerialOpen(int32_t portId)
{
    USB_HILOGI(MODULE_USB_SERVICE, "serialManager::SerialOpen Execute");
    std::lock_guard<std::mutex> guard(mutex_);
    if (serial_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "SerialManager::SerialOpen serial_ is nullptr");
        return OHOS::USB::UEC_SERVICE_INVALID_VALUE;
    }

    if (portManageMap_.find(portId) != portManageMap_.end()) {
        if (portManageMap_[portId]) {
            USB_HILOGE(MODULE_USB_SERVICE, "SerialManager::SerialOpen The port has been opened");
            return UEC_MANAGER_PORT_REPEAT_OPEN;
        }
    }

    int32_t ret = serial_->SerialOpen(portId);
    if (ret == UEC_OK) {
        portManageMap_[portId] = true;
    }
    return ret;
}

int32_t SerialManager::SerialClose(int32_t portId)
{
    USB_HILOGI(MODULE_USB_SERVICE, "serialManager::SerialClose start");
    std::lock_guard<std::mutex> guard(mutex_);
    if (serial_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "SerialManager::SerialClose serial_ is nullptr");
        return OHOS::USB::UEC_SERVICE_INVALID_VALUE;
    }

    if (portManageMap_.find(portId) != portManageMap_.end()) {
        if (!portManageMap_.find(portId)->second) {
            USB_HILOGE(MODULE_USB_SERVICE, "SerialManager::SerialClose The port repeat close");
            return UEC_MANAGER_PORT_REPEAT_CLOSE;
        }
    }

    int32_t ret = serial_->SerialClose(portId);
    if (ret == UEC_OK) {
        portManageMap_[portId] = false;
    }
    return ret;
}

int32_t SerialManager::SerialRead(int32_t portId, std::vector<uint8_t>& data, uint32_t size)
{
    USB_HILOGI(MODULE_USB_SERVICE, "serialManager::SerialRead start");
    if (serial_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "SerialManager::SerialRead serial_ is nullptr");
        return OHOS::USB::UEC_SERVICE_INVALID_VALUE;
    }

    if (!IsPortStatus(portId)) {
        USB_HILOGE(MODULE_USB_SERVICE, "SerialManager::SerialRead The port is not open");
        return UEC_MANAGER_PORT_NOT_OPEN;
    }
    #ifdef SERIAL_MOCK
    data.clear();
    if (size > g_mockBuffer.size()) {
        return UEC_OK;
    }
    for (size_t i = 0; i < size; ++i) {
        data.push_back(g_mockBuffer[i]);
    }
    auto it = g_mockBuffer.cbegin();
    g_mockBuffer.erase(it, it + size);
    return UEC_OK;
    #else
    return serial_->SerialRead(portId, data, size);
    #endif
}

int32_t SerialManager::SerialWrite(int32_t portId, const std::vector<uint8_t>& data, uint32_t size)
{
    USB_HILOGI(MODULE_USB_SERVICE, "serialManager::SerialWrite start");
    if (serial_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "SerialManager::SerialWrite serial_ is nullptr");
        return OHOS::USB::UEC_SERVICE_INVALID_VALUE;
    }

    if (!IsPortStatus(portId)) {
        USB_HILOGE(MODULE_USB_SERVICE, "SerialManager::SerialWrite The port is not open");
        return UEC_MANAGER_PORT_NOT_OPEN;
    }
    #ifdef SERIAL_MOCK
    for (size_t i = 0; i < size <= data.size() ? size : data.size(); ++i) {
        if (g_mockBuffer.size() == BUFFER_CAPACITY) {
            return UEC_OK;
        }
        g_mockBuffer.push_back(data[i]);
    }
    return UEC_OK;
    #else
    return serial_->SerialWrite(portId, data, size);
    #endif
}

int32_t SerialManager::SerialGetAttribute(int32_t portId, OHOS::HDI::Usb::Serial::V1_0::SerialAttribute& attribute)
{
    USB_HILOGI(MODULE_USB_SERVICE, "serialManager::SerialGetAttribute start");
    if (serial_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "SerialManager::SerialGetAttribute serial_ is nullptr");
        return OHOS::USB::UEC_SERVICE_INVALID_VALUE;
    }
    if (!IsPortStatus(portId)) {
        USB_HILOGE(MODULE_USB_SERVICE, "SerialManager::SerialGetAttribute The port is not open");
        return UEC_MANAGER_PORT_NOT_OPEN;
    }
    return serial_->SerialGetAttribute(portId, attribute);
}

int32_t SerialManager::SerialSetAttribute(int32_t portId,
    const OHOS::HDI::Usb::Serial::V1_0::SerialAttribute& attribute)
{
    USB_HILOGI(MODULE_USB_SERVICE, "serialManager::SerialSetAttribute start");

    if (serial_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "SerialManager::SerialSetAttribute serial_ is nullptr");
        return OHOS::USB::UEC_SERVICE_INVALID_VALUE;
    }

    if (!IsPortStatus(portId)) {
        USB_HILOGE(MODULE_USB_SERVICE, "SerialManager::SerialSetAttribute The port is not open");
        return UEC_MANAGER_PORT_NOT_OPEN;
    }
    return serial_->SerialSetAttribute(portId, attribute);
}

int32_t SerialManager::SerialGetPortList(std::vector<OHOS::HDI::Usb::Serial::V1_0::SerialPort>& serialPortList)
{
    USB_HILOGI(MODULE_USB_SERVICE, "serialManager::SerialGetPortList start");

    if (serial_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "serialManager::SerialGetPortList serial_ is nullptr");
        return OHOS::USB::UEC_SERVICE_INVALID_VALUE;
    }

    return serial_->SerialGetPortList(serialPortList);
}

bool SerialManager::IsPortStatus(int32_t portId)
{
    if (portManageMap_.find(portId) == portManageMap_.end()) {
        return false;
    }

    return portManageMap_.find(portId)->second;
}

void SerialManager::SerialPortListDump(int32_t fd, const std::vector<std::string>& args)
{
    if (args.size() != PARAM_COUNT_ONE) {
        dprintf(fd, "port param error: exactly %d parameters are required\n", PARAM_COUNT_ONE);
        ListGetDumpHelp(fd);
        return;
    }
    
    if (args[0] =="-l") {
        std::vector<OHOS::HDI::Usb::Serial::V1_0::SerialPort> serialPortList;
        SerialGetPortList(serialPortList);
        dprintf(fd, "=========== dump the serial port list ===========\n");
        for (size_t i = 0; i < serialPortList.size(); ++i) {
            char portName[20];
            snprintf_s(portName, sizeof(portName), sizeof(portName)-1, "ttyUSB%zu", i);
            dprintf(fd, "%s\n", portName);
        }
        dprintf(fd, "------------------------------------------------\n");
    } else {
        dprintf(fd, "port param error: invalid command '%s'\n", args[0].c_str());
        ListGetDumpHelp(fd);
    }
}

void SerialManager::SerialGetAttributeDump(int32_t fd, const std::vector<std::string>& args)
{
    size_t size = args.size();
    OHOS::HDI::Usb::Serial::V1_0::SerialAttribute attribute;
    if (size == 1) {
        std::vector<OHOS::HDI::Usb::Serial::V1_0::SerialPort> serialPortList;
        int32_t ret = SerialGetPortList(serialPortList);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "SerialGetPortList failed");
            return;
        }
        size_t portCount = serialPortList.size();
        for (size_t i = 0; i <portCount; ++i) {
            int32_t portId = i;
            ret = SerialGetAttribute(portId, attribute);
            if (ret != UEC_OK) {
                SerialOpen(portId);
                SerialGetAttribute(portId, attribute);
                SerialClose(portId);
            }
            dprintf(fd, "Port %zu: baudrate: %zu stopBits:%zu parity:%zu dataBits:%zu\n",
                i, attribute.baudrate, attribute.stopBits, attribute.parity, attribute.dataBits);
        }
    } else if (size == TWOARGES) {
        int32_t portId = std::stoi(args[1]);
        int32_t ret = SerialGetAttribute(portId, attribute);
        if (ret != UEC_OK) {
            ret = SerialOpen(portId);
            if (ret != UEC_OK) {
                USB_HILOGE(MODULE_USB_SERVICE, "SerialOpen failed");
                dprintf(fd, "Port %zu: port does not exist\n", portId);
                return;
            }
            ret = SerialGetAttribute(portId, attribute);
            if (ret != UEC_OK) {
                USB_HILOGE(MODULE_USB_SERVICE, "SerialGetAttribute failed");
            }
            SerialClose(portId);
        }
        dprintf(fd, "Port %zu: baudrate: %zu stopBits:%zu parity:%zu dataBits:%zu\n",
            portId, attribute.baudrate, attribute.stopBits, attribute.parity, attribute.dataBits);
    } else {
        ListGetDumpHelp(fd);
    }
}

void SerialManager::ListGetDumpHelp(int32_t fd)
{
    dprintf(fd, "=========== dump the serial port information ===========\n");
    dprintf(fd, "serail -l: Query All Serial Port List\n");
    dprintf(fd, "serial -h: Serial port help\n");
    dprintf(fd, "serial -g : Gets the properties of all ports\n");
    dprintf(fd, "serial \"-g [port number]\": Gets the properties of the specified port\n");
    dprintf(fd, "------------------------------------------------\n");
}
}  //namespace SERIAL
}  //namespace OHOS
