/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

namespace OHOS {
namespace SERIAL {
class SerialManager {
public:
    SerialManager();
    ~SerialManager();

    int32_t SerialOpen(int32_t portId);
    int32_t SerialClose(int32_t portId);
    int32_t SerialRead(int32_t portId, std::vector<uint8_t>& data, uint32_t size);
    int32_t SerialWrite(int32_t portId, const std::vector<uint8_t>& data, uint32_t size);
    int32_t SerialGetAttribute(int32_t portId, OHOS::HDI::Usb::Serial::V1_0::SerialAttribute& attribute);
    int32_t SerialSetAttribute(int32_t portId, const OHOS::HDI::Usb::Serial::V1_0::SerialAttribute& attribute);
    int32_t SerialGetPortList(std::vector<OHOS::HDI::Usb::Serial::V1_0::SerialPort>& serialPortList);
    void SerialPortListDump(int32_t fd, const std::vector<std::string>& args);
    void ListGetDumpHelp(int32_t fd);
    void SerialGetDump(int32_t fd, const std::vector<std::string>& args);
private:
    bool IsPortStatus(int32_t portId);
    std::mutex mutex_;
    std::map<int32_t, bool> portManageMap_;
    sptr<OHOS::HDI::Usb::Serial::V1_0::ISerialInterface> serial_ = nullptr;
};
} // namespace SERIAL
} // namespace OHOS

#endif //SERIAL_MANAGER_H
