/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef SERIAL_MANAGER_H
#define SERIAL_MANAGER_H

#include <algorithm>
#include <map>
#include <mutex>
#include <string>
#include <vector>
#include "v1_0/iserial_interface.h"
#include <ipc_skeleton.h>
#include "usb_right_manager.h"

namespace OHOS {
namespace SERIAL {
class SerialManager {
public:
    SerialManager();
    ~SerialManager();

    int32_t SerialOpen(int32_t portId);
    int32_t SerialClose(int32_t portId);
    int32_t SerialRead(int32_t portId, uint8_t *buffData, uint32_t size, uint32_t timeout);
    int32_t SerialWrite(int32_t portId, const std::vector<uint8_t>& data, uint32_t size, uint32_t timeout);
    int32_t SerialGetAttribute(int32_t portId, OHOS::HDI::Usb::Serial::V1_0::SerialAttribute& attribute);
    int32_t SerialSetAttribute(int32_t portId, const OHOS::HDI::Usb::Serial::V1_0::SerialAttribute& attribute);
    int32_t SerialGetPortList(std::vector<OHOS::HDI::Usb::Serial::V1_0::SerialPort>& serialPortList);
    void SerialPortListDump(int32_t fd, const std::vector<std::string>& args);
    void ListGetDumpHelp(int32_t fd);
    void SerialGetAttributeDump(int32_t fd, const std::vector<std::string>& args);
    bool IsPortIdExist(int32_t portId);
    void FreeTokenId(int32_t portId, uint32_t tokenId);
    int32_t GetTokenId(uint32_t &tokenId);
    bool GetSerialPort(int32_t portId, OHOS::HDI::Usb::Serial::V1_0::SerialPort& serialPort);
private:
    bool IsPortStatus(int32_t portId);
    bool CheckTokenIdValidity(int32_t portId);
    void UpdateSerialPortMap(std::vector<OHOS::HDI::Usb::Serial::V1_0::SerialPort>& serialPortList);
    int32_t CheckPortAndTokenId(int32_t portId);
    void ReportSerialOperateSysEvent(std::string interfaceName, int32_t portId, uint32_t tokenId);
    void ReportSerialOperateSetAttributeSysEvent(int32_t portId, uint32_t tokenId,
        const OHOS::HDI::Usb::Serial::V1_0::SerialAttribute& attribute);
    std::map<int32_t, uint32_t> portTokenMap_;
    std::map<int32_t, OHOS::HDI::Usb::Serial::V1_0::SerialPort> serialPortMap_;
    sptr<OHOS::HDI::Usb::Serial::V1_0::ISerialInterface> serial_ = nullptr;
    std::shared_ptr<USB::UsbRightManager> usbRightManager_;
    std::mutex serialPortMapMutex_;
};
} // namespace SERIAL
} // namespace OHOS

#endif //SERIAL_MANAGER_H
