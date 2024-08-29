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

#ifndef USB_HOST_MANAGER_H
#define USB_HOST_MANAGER_H

#include <map>
#include <string>
#include <vector>

#include "system_ability.h"
#include "usb_device.h"
#include "usb_right_manager.h"
#include "usb_interface_type.h"

namespace OHOS {
namespace USB {
struct DeviceClassUsage {
    uint8_t usage;
    std::string description;

    DeviceClassUsage(int8_t uage, std::string des)
        : usage(uage), description(des) {};
};

typedef std::map<std::string, UsbDevice *> MAP_STR_DEVICE;
class UsbHostManager {
public:
    explicit UsbHostManager(SystemAbility *systemAbility);
    ~UsbHostManager();
    void GetDevices(MAP_STR_DEVICE &devices);
    bool GetProductName(const std::string &deviceName, std::string &productName);
    bool DelDevice(uint8_t busNum, uint8_t devNum);
    bool AddDevice(UsbDevice *dev);
    bool Dump(int fd, const std::string &args);

private:
    bool PublishCommonEvent(const std::string &event, const UsbDevice &dev);
    void ReportHostPlugSysEvent(const std::string &event, const UsbDevice &dev);
    std::string ConcatenateToDescription(const UsbDeviceType &interfaceType, const std::string& str);
    int32_t GetDeviceDescription(int32_t baseClass, std::string &description, uint8_t &usage);
    int32_t GetInterfaceDescription(const UsbDevice &dev, std::string &description, int32_t &baseClass);
    std::string GetInterfaceUsageDescription(const UsbDeviceType &interfaceType);
    MAP_STR_DEVICE devices_;
    SystemAbility *systemAbility_;
};
} // namespace USB
} // namespace OHOS

#endif
