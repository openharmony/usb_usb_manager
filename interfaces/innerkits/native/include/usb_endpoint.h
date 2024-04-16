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

#ifndef USB_ENDPOINT_H
#define USB_ENDPOINT_H

#include <iostream>
#include <sstream>
#include <string>
#include "usb_common.h"
#include "cJSON.h"

namespace OHOS {
namespace USB {
class USBEndpoint {
public:
    USBEndpoint(uint32_t address, uint32_t attributes, uint32_t interval, uint32_t maxPacketSize)
    {
        this->address_ = address;
        this->attributes_ = attributes;
        this->interval_ = static_cast<int32_t>(interval);
        this->maxPacketSize_ = static_cast<int32_t>(maxPacketSize);
    }

    explicit USBEndpoint(const cJSON *endpoint)
    {
        if (endpoint == nullptr) {
            USB_HILOGE(MODULE_USB_SERVICE, "endpoint pointer is nullptr");
        }
        address_ = static_cast<uint32_t>(GetIntValue(endpoint, "address"));
        attributes_ = static_cast<uint32_t>(GetIntValue(endpoint, "attributes"));
        interval_ = GetIntValue(endpoint, "interval");
        maxPacketSize_ = GetIntValue(endpoint, "maxPacketSize");
        interfaceId_ = GetIntValue(endpoint, "interfaceId");
    }

    USBEndpoint() {}
    ~USBEndpoint() {}

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

    uint8_t GetNumber() const
    {
        return address_ & USB_ENDPOINT_NUMBER_MASK;
    }

    const uint32_t &GetAddress() const
    {
        return address_;
    }

    uint32_t GetDirection() const
    {
        return address_ & USB_ENDPOINT_DIR_MASK;
    }

    const uint32_t &GetAttributes() const
    {
        return attributes_;
    }

    uint32_t GetEndpointNumber() const
    {
        return address_ & USB_ENDPOINT_NUMBER_MASK;
    }

    const int32_t &GetInterval() const
    {
        return interval_;
    }

    const int32_t &GetMaxPacketSize() const
    {
        return maxPacketSize_;
    }

    uint32_t GetType() const
    {
        return (attributes_ & USB_ENDPOINT_XFERTYPE_MASK);
    }

    std::string ToString() const
    {
        std::string ret = "USBEndpoint:[Address:";
        ret.append(std::to_string(address_))
            .append(", Direction:")
            .append(std::to_string(GetDirection()))
            .append(", Attributes:")
            .append(std::to_string(attributes_))
            .append(", EndpointNumber:")
            .append(std::to_string(GetEndpointNumber()))
            .append(", Interval:")
            .append(std::to_string(interval_))
            .append(", MaxPacketSize:")
            .append(std::to_string(maxPacketSize_))
            .append(", Type:")
            .append(std::to_string(GetType()))
            .append("]");
        return ret;
    }

    void SetAddr(uint32_t val)
    {
        address_ = val;
    }

    void SetAttr(uint32_t val)
    {
        attributes_ = val;
    }

    void SetInterval(int32_t val)
    {
        interval_ = val;
    }

    void SetMaxPacketSize(int32_t val)
    {
        maxPacketSize_ = val;
    }

    void SetInterfaceId(uint8_t interfaceId)
    {
        this->interfaceId_ = interfaceId;
    }

    int8_t GetInterfaceId() const
    {
        return interfaceId_;
    }

    const std::string getJsonString() const
    {
        cJSON* endPointJson = cJSON_CreateObject();
        if (!endPointJson) {
            USB_HILOGE(MODULE_USB_SERVICE, "Create endPointJson error");
        }
        cJSON_AddNumberToObject(endPointJson, "address", static_cast<double>(address_));
        cJSON_AddNumberToObject(endPointJson, "attributes", static_cast<double>(attributes_));
        cJSON_AddNumberToObject(endPointJson, "interval", static_cast<double>(interval_));
        cJSON_AddNumberToObject(endPointJson, "maxPacketSize", static_cast<double>(maxPacketSize_));
        cJSON_AddNumberToObject(endPointJson, "direction", static_cast<double>(GetDirection()));
        cJSON_AddNumberToObject(endPointJson, "number", static_cast<double>(GetEndpointNumber()));
        cJSON_AddNumberToObject(endPointJson, "type", static_cast<double>(GetType()));
        cJSON_AddNumberToObject(endPointJson, "interfaceId", static_cast<double>(interfaceId_));
        std::string endPointJsonStr(cJSON_PrintUnformatted(endPointJson));
        cJSON_Delete(endPointJson);
        return endPointJsonStr;
    }

private:
    uint32_t address_ = 0;
    uint32_t attributes_ = 0;
    int32_t interval_ = INVALID_USB_INT_VALUE;
    int32_t maxPacketSize_ = INVALID_USB_INT_VALUE;
    uint8_t interfaceId_ = UINT8_MAX;
};
} // namespace USB
} // namespace OHOS

#endif // USB_ENDPOINT_H
