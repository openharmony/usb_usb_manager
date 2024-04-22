/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef USB_CONFIG_H
#define USB_CONFIG_H

#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <vector>
#include "usb_common.h"
#include "usb_interface.h"
#include "cJSON.h"

namespace OHOS {
namespace USB {
class USBConfig {
public:
    USBConfig(uint32_t id, uint32_t attributes, std::string name, uint32_t maxPower,
        std::vector<UsbInterface> interfaces)
    {
        this->id_ = static_cast<int32_t>(id);
        this->attributes_ = attributes;
        this->maxPower_ = static_cast<int32_t>(maxPower);
        this->name_ = name;
        this->interfaces_ = interfaces;
    }

    explicit USBConfig(const cJSON *config)
    {
        if (config == nullptr) {
            USB_HILOGE(MODULE_USB_SERVICE, "config pointer is nullptr");
        }
        id_ = GetIntValue(config, "id");
        attributes_ = static_cast<uint32_t>(GetIntValue(config, "attributes"));
        maxPower_ = GetIntValue(config, "maxPower");
        name_ = GetStringValue(config, "name");
        cJSON *jsonInterfaces = cJSON_GetObjectItem(config, "interfaces");
        if (jsonInterfaces != nullptr) {
            for (int i = 0; i < cJSON_GetArraySize(jsonInterfaces); i++) {
                cJSON *jsonInterface = cJSON_GetArrayItem(jsonInterfaces, i);
                if (jsonInterface == nullptr) {
                    USB_HILOGE(MODULE_USB_SERVICE, "get item nullptr");
                    continue;
                }
                UsbInterface interface(jsonInterface);
                interfaces_.emplace_back(interface);
            }
        }
    }

    USBConfig() {}
    ~USBConfig() {}

    static int GetIntValue(const cJSON *jsonObject, const char *key)
    {
        cJSON *item = cJSON_GetObjectItem(jsonObject, key);
        if (item != nullptr && cJSON_IsNumber(item)) {
            return item->valueint;
        } else {
            USB_HILOGE(MODULE_USB_SERVICE, "Invalid or missing %s field", key);
            return 0;
        }
    }

    static std::string GetStringValue(const cJSON *jsonObject, const char *key)
    {
        cJSON *item = cJSON_GetObjectItem(jsonObject, key);
        if (item != nullptr && cJSON_IsString(item)) {
            return item->valuestring;
        } else {
            USB_HILOGE(MODULE_USB_SERVICE, "Invalid or missing %s field", key);
            return "";
        }
    }

    const int32_t &GetId() const
    {
        return id_;
    }

    const uint32_t &GetAttributes() const
    {
        return attributes_;
    }

    bool GetInterface(uint32_t index, UsbInterface &interface) const
    {
        if (index >= interfaces_.size()) {
            return false;
        }
        interface = interfaces_[index];
        return true;
    }

    uint32_t GetInterfaceCount() const
    {
        return interfaces_.size();
    }

    int32_t GetMaxPower() const
    {
        // 2 represent maxPower units
        return maxPower_ * 2;
    }

    const std::string &GetName() const
    {
        return name_;
    }

    bool IsRemoteWakeup() const
    {
        return (attributes_ & USB_CFG_REMOTE_WAKEUP) != 0;
    }

    bool IsSelfPowered() const
    {
        return (attributes_ & USB_CFG_SELF_POWERED) != 0;
    }

    void SetInterfaces(const std::vector<UsbInterface> &interfaces)
    {
        this->interfaces_ = interfaces;
    }

    std::vector<UsbInterface> &GetInterfaces()
    {
        return interfaces_;
    }

    void SetId(int32_t id)
    {
        this->id_ = id;
    }

    void SetAttribute(uint32_t attributes)
    {
        this->attributes_ = attributes;
    }

    void SetMaxPower(int32_t maxPower)
    {
        this->maxPower_ = maxPower;
    }

    std::string ToString() const
    {
        std::ostringstream ss;
        ss << "name=" << name_ << ","
           << "id=" << id_ << ","
           << "iConfiguration=" << (int32_t)iConfiguration_ << ","
           << "attributes=" << attributes_ << ","
           << "maxPower=" << maxPower_ << ";  ";
        std::string str = "USBConfig[" + ss.str() + "]";
        ss.str("");
        for (size_t i = 0; i < interfaces_.size(); ++i) {
            const UsbInterface &interface = interfaces_[i];
            str += interface.ToString();
        }
        return str;
    }

    void SetName(const std::string &name)
    {
        this->name_ = name;
    }

    void SetiConfiguration(uint8_t idx)
    {
        this->iConfiguration_ = idx;
    }

    uint8_t GetiConfiguration()
    {
        return this->iConfiguration_;
    }

    const std::string getJsonString() const
    {
        cJSON* config = cJSON_CreateObject();
        if (!config) {
            USB_HILOGE(MODULE_USB_SERVICE, "Create config error");
            return "";
        }
        cJSON_AddNumberToObject(config, "id", static_cast<double>(id_));
        cJSON_AddNumberToObject(config, "attributes", static_cast<double>(attributes_));
        cJSON_AddNumberToObject(config, "maxPower", static_cast<double>(maxPower_));
        cJSON_AddStringToObject(config, "name", name_.c_str());
        cJSON_AddBoolToObject(config, "isRemoteWakeup", IsRemoteWakeup());
        cJSON_AddBoolToObject(config, "isSelfPowered", IsSelfPowered());

        cJSON* interfaces = cJSON_CreateArray();
        if (!interfaces) {
            USB_HILOGE(MODULE_USB_SERVICE, "Create interfaces error");
            cJSON_Delete(config);
            return "";
        }
        for (auto &intf : interfaces_) {
            cJSON* pInterface =  cJSON_Parse(intf.getJsonString().c_str());
            cJSON_AddItemToArray(interfaces, pInterface);
        }
        cJSON_AddItemToObject(config, "interfaces", interfaces);
        char *pConfigStr = cJSON_PrintUnformatted(config);
        cJSON_Delete(config);
        if (!pConfigStr) {
            USB_HILOGE(MODULE_USB_SERVICE, "Print config error");
            return "";
        }
        std::string configStr(pConfigStr);
        free(pConfigStr);
        return configStr;
    }

private:
    int32_t id_ = INVALID_USB_INT_VALUE;
    uint32_t attributes_ = 0;
    std::vector<UsbInterface> interfaces_;
    int32_t maxPower_ = INVALID_USB_INT_VALUE;
    std::string name_;
    uint8_t iConfiguration_ = UINT8_MAX;
};
} // namespace USB
} // namespace OHOS

#endif // USB_CONFIG_H
