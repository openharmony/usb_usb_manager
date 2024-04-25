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

#ifndef USB_INTERFACE_H
#define USB_INTERFACE_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <optional>
#include "usb_endpoint.h"

namespace OHOS {
namespace USB {
class UsbInterface {
public:
    UsbInterface(int32_t id,
                 int32_t protocol,
                 int32_t interfaceClass,
                 int32_t subClass,
                 int32_t alternateSetting,
                 std::string name,
                 std::vector<USBEndpoint> endpoints)
    {
        this->id_ = id;
        this->protocol_ = protocol;
        this->klass_ = interfaceClass;
        this->subClass_ = subClass;
        this->alternateSetting_ = alternateSetting;
        this->endpoints_ = endpoints;
    }

    explicit UsbInterface(const cJSON *interface)
    {
        if (interface == nullptr) {
            USB_HILOGE(MODULE_USB_SERVICE, "interface pointer is nullptr");
        }
        id_ = GetIntValue(interface, "id");
        protocol_ = GetIntValue(interface, "protocol");
        klass_ = GetIntValue(interface, "clazz");
        subClass_ = GetIntValue(interface, "subClass");
        alternateSetting_ = GetIntValue(interface, "alternateSetting");
        name_ = GetStringValue(interface, "name");

        cJSON *endpoints = cJSON_GetObjectItem(interface, "endpoints");
        for (int i = 0; i < cJSON_GetArraySize(endpoints); i++) {
            cJSON *jsonEp = cJSON_GetArrayItem(endpoints, i);
            if (jsonEp == nullptr) {
                USB_HILOGE(MODULE_USB_SERVICE, "get item nullptr");
                continue;
            }
            USBEndpoint ep(jsonEp);
            endpoints_.emplace_back(ep);
        }
    }

    UsbInterface() {}

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

    const std::string &GetName() const
    {
        return name_;
    }

    int32_t GetId() const
    {
        return id_;
    }

    int32_t GetClass() const
    {
        return klass_;
    }

    int32_t GetSubClass() const
    {
        return subClass_;
    }

    int32_t GetAlternateSetting() const
    {
        return alternateSetting_;
    }

    int32_t GetProtocol() const
    {
        return protocol_;
    }

    int32_t GetEndpointCount() const
    {
        return endpoints_.size();
    }

    std::optional<USBEndpoint> GetEndpoint(uint32_t index) const
    {
        if (index >= endpoints_.size()) {
            USB_HILOGE(MODULE_USB_INNERKIT, "invalid index=%{public}u !", index);
            return std::nullopt;
        }

        return endpoints_[index];
    }

    std::vector<USBEndpoint> &GetEndpoints()
    {
        return endpoints_;
    }

    void SetEndpoints(const std::vector<USBEndpoint> &eps)
    {
        endpoints_ = eps;
    }

    void SetId(int32_t id)
    {
        id_ = id;
    }

    void SetProtocol(int32_t protocol)
    {
        protocol_ = protocol;
    }

    void SetClass(int32_t klass)
    {
        klass_ = klass;
    }

    void SetSubClass(int32_t subClass)
    {
        subClass_ = subClass;
    }

    void SetAlternateSetting(int32_t alternateSetting)
    {
        alternateSetting_ = alternateSetting;
    }

    void SetName(const std::string &name)
    {
        name_ = name;
    }

    ~UsbInterface() {}

    std::string ToString() const
    {
        std::ostringstream ss;
        ss << "id=" << id_ << ","
           << "name_=" << name_ << ","
           << "iInterface_=" << (int32_t)iInterface_ << ","
           << "klass_=" << klass_ << ","
           << "subClass_=" << subClass_ << ","
           << "protocol_=" << protocol_ << ","
           << "alternateSetting_=" << alternateSetting_ << "";
        std::string str = "UsbInterface[" + ss.str() + "];    ";
        ss.str("");
        for (size_t i = 0; i < endpoints_.size(); ++i) {
            const USBEndpoint &endpoint = endpoints_[i];
            str += endpoint.ToString();
        }
        return str;
    }

    void SetiInterface(uint8_t idx)
    {
        this->iInterface_ = idx;
    }

    uint8_t GetiInterface()
    {
        return this->iInterface_;
    }

    const std::string getJsonString() const
    {
        cJSON* interface = cJSON_CreateObject();
        if (!interface) {
            USB_HILOGE(MODULE_USB_SERVICE, "Create interface error");
            return "";
        }
        cJSON_AddNumberToObject(interface, "id", static_cast<double>(id_));
        cJSON_AddNumberToObject(interface, "protocol", static_cast<double>(protocol_));
        cJSON_AddNumberToObject(interface, "clazz", static_cast<double>(klass_));
        cJSON_AddNumberToObject(interface, "subClass", static_cast<double>(subClass_));
        cJSON_AddNumberToObject(interface, "alternateSetting", alternateSetting_);
        cJSON_AddStringToObject(interface, "name", name_.c_str());
        cJSON* endpoints = cJSON_CreateArray();
        if (!endpoints) {
            USB_HILOGE(MODULE_USB_SERVICE, "Create endpoints error");
        }
        for (size_t i = 0; i < endpoints_.size(); ++i) {
            const USBEndpoint &ep = endpoints_[i];
            cJSON* pEp =  cJSON_Parse(ep.getJsonString().c_str());
            cJSON_AddItemToArray(endpoints, pEp);
        }
        cJSON_AddItemToObject(interface, "endpoints", endpoints);
        char *pInterface = cJSON_PrintUnformatted(interface);
        cJSON_Delete(interface);
        if (!pInterface) {
            USB_HILOGE(MODULE_USB_SERVICE, "Print interface error");
            return "";
        }
        std::string interfaceJsonStr(pInterface);
        free(pInterface);
        return interfaceJsonStr;
    }

private:
    int32_t id_ = INT32_MAX;
    int32_t protocol_ = INT32_MAX;
    int32_t klass_ = INT32_MAX;
    int32_t subClass_ = INT32_MAX;
    int32_t alternateSetting_ = INT32_MAX;
    std::string name_;
    std::vector<USBEndpoint> endpoints_;
    uint8_t iInterface_ = UINT8_MAX;
};
} // namespace USB
} // namespace OHOS

#endif // USB_INTERFACE_H
