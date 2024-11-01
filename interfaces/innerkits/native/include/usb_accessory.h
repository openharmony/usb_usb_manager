/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024. All rights reserved.
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
#ifndef USB_ACCESSORY_H
#define USB_ACCESSORY_H
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <vector>
#include <string>
#include "usb_common.h"
#include "cJSON.h"

const int32_t ACC_SIZE = 5;
const int32_t ACC_MANUFACTURER_INDEX = 0;
const int32_t ACC_MODEL_INDEX = 1;
const int32_t ACC_DESCRIPTION_INDEX = 2;
const int32_t ACC_VERSION_INDEX = 3;
const int32_t ACC_SERIAL_INDEX = 4;
namespace OHOS {
namespace USB {
class USBAccessory {
public:
    USBAccessory(const std::string &manufacturer, const std::string &model,
        const std::string &description, const std::string &version, const std::string &serial)
    {
        this->manufacturer_ = manufacturer;
        this->model_ = model;
        this->description_ = description;
        this->version_ = version;
        this->serial_ = serial;
    }

    explicit USBAccessory(const cJSON *accesory)
    {
        if (accesory == nullptr) {
            USB_HILOGE(MODULE_USB_SERVICE, "accesory pointer is nullptr");
            return;
        }
        manufacturer_ = GetStringValue(accesory, "manufacturer");
        model_ = GetStringValue(accesory, "model");
        description_ = GetStringValue(accesory, "description");
        version_ = GetStringValue(accesory, "version");
        serial_ = GetStringValue(accesory, "serial");
    }

    USBAccessory() {}

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

    const std::string GetJsonString() const
    {
        cJSON *accJson = cJSON_CreateObject();
        if (!accJson) {
            USB_HILOGE(MODULE_USB_SERVICE, "Create accessory error");
            return "";
        }
        cJSON_AddStringToObject(accJson, "manufacturer", manufacturer_.c_str());
        cJSON_AddStringToObject(accJson, "model", model_.c_str());
        cJSON_AddStringToObject(accJson, "description", description_.c_str());
        cJSON_AddStringToObject(accJson, "version", version_.c_str());
        cJSON_AddStringToObject(accJson, "serial", serial_.c_str());
        char *pAccJson = cJSON_PrintUnformatted(accJson);
        cJSON_Delete(accJson);
        if (!pAccJson) {
            USB_HILOGE(MODULE_USB_SERVICE, "Print accJson error");
            return "";
        }
        std::string accJsonStr(pAccJson);
        cJSON_free(pAccJson);
        pAccJson = NULL;
        return accJsonStr;
    }

    std::string ToString() const
    {
        std::ostringstream ss;
        ss << "manufacturer=" << manufacturer_ << ","
           << "model=" << model_ << ","
           << "description=" << description_ << ","
           << "version=" << version_ << ","
           << "serial=" << serial_ << ";  ";
        std::string str = "USBAccessory[" + ss.str() + "]";
        return str;
    }

    void SetAccessory(std::vector<std::string> &accessorys)
    {
        if (accessorys.size() < ACC_SIZE) {
            USB_HILOGE(MODULE_USB_SERVICE, "accessorys param invalid");
            return;
        }

        this->manufacturer_ = accessorys.at(ACC_MANUFACTURER_INDEX);
        this->model_ = accessorys.at(ACC_MODEL_INDEX);
        this->description_ = accessorys.at(ACC_DESCRIPTION_INDEX);
        this->version_ = accessorys.at(ACC_VERSION_INDEX);
        this->serial_ = accessorys.at(ACC_SERIAL_INDEX);
    }

    std::string GetManufacturer() const
    {
        return manufacturer_;
    }

    std::string GetModel() const
    {
        return model_;
    }

    std::string GetDescription() const
    {
        return description_;
    }

    std::string GetVersion() const
    {
        return version_;
    }

    std::string GetSerial() const
    {
        return serial_;
    }

    void SetManufacturer(const std::string &manufacturer)
    {
        this->manufacturer_ = manufacturer;
    }

    void SetModel(const std::string &model)
    {
        this->model_ = model;
    }

    void SetDescription(const std::string &description)
    {
        this->description_ = description;
    }

    void SetVersion(const std::string &version)
    {
        this->version_ = version;
    }

    void SetSerial(const std::string &serial)
    {
        this->serial_ = serial;
    }

    bool operator==(USBAccessory& obj) const
    {
        return (compare(manufacturer_, obj.GetManufacturer()) &&
                compare(model_, obj.GetModel()) &&
                compare(description_, obj.GetDescription()) &&
                compare(version_, obj.GetVersion()) &&
                compare(serial_, obj.GetSerial()));
    }
private:
    static bool compare(const std::string &s1, const std::string &s2)
    {
        return s1 == s2;
    }

private:
   std::string manufacturer_;
   std::string model_;
   std::string description_;
   std::string version_;
   std::string serial_;
};

} // USB
} // OHOS
#endif // USB_ACCESSORY_H