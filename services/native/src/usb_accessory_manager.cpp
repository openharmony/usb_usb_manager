/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024. All rights reserved.
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
#include "usb_accessory_manager.h"

#include <iostream>
#include <functional>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <hdf_base.h>

#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "hisysevent.h"
#include "usb_errors.h"
#include "usb_srv_support.h"
#include "usbd_type.h"
#include "cJSON.h"
using namespace OHOS::AAFwk;
using namespace OHOS::EventFwk;
using namespace OHOS::HiviewDFX;
using namespace OHOS::HDI::Usb::V1_0;
using namespace OHOS::HDI::Usb::V1_1;

namespace OHOS {
namespace USB {
constexpr int32_t ACCESSORY_INFO_SIZE = 5;
constexpr int32_t ACCESSORY_EXTRA_INDEX = 5;
constexpr int32_t FUN_ACCESSORY = 1 << 11;
constexpr int32_t NUM_OF_SERAIL_BIT = 16;
constexpr uint32_t DELAY_ACC_INTERVAL = 10 * 1000;
constexpr uint32_t ANTI_SHAKE_INTERVAL = 1 * 1000;
constexpr int32_t ACCESSORY_IS_BUSY = -16;
constexpr int32_t BASE64_CHAR_INDEX0 = 0;
constexpr int32_t BASE64_CHAR_INDEX1 = 1;
constexpr int32_t BASE64_CHAR_INDEX2 = 2;
constexpr int32_t BASE64_CHAR_INDEX3 = 3;
constexpr int32_t BASE64_CHAR_INDEX4 = 4;
constexpr int32_t BASE64_CHAR_INDEX6 = 6;
constexpr uint8_t BASE64_CHAR_MASK30 = 0x30;
constexpr uint8_t BASE64_CHAR_MASKF  = 0xf;
constexpr uint8_t BASE64_CHAR_MASK3C = 0x3c;
const std::string BASE64_CHARS =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

UsbAccessoryManager::UsbAccessoryManager()
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbAccessoryManager::Init start");
    usbdImpl_ = OHOS::HDI::Usb::V1_1::IUsbInterface::Get();
    if (usbdImpl_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbDeviceManager::Get inteface failed");
    }
}

int32_t UsbAccessoryManager::SetUsbd(const sptr<OHOS::HDI::Usb::V1_1::IUsbInterface> usbd)
{
    if (usbd == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbAccessoryManager usbd is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    usbdImpl_ = usbd;
    return UEC_OK;
}

void UsbAccessoryManager::GetAccessoryList(const std::string &bundleName,
    std::vector<USBAccessory> &accessoryList)
{
    if (accStatus_ == ACC_START) {
        USBAccessory access = this->accessory;
        access.SetSerialNumber(bundleName + this->accessory.GetSerialNumber());
        accessoryList.push_back(access);
    }
    return;
}

int32_t UsbAccessoryManager::OpenAccessory(int32_t &fd)
{
    if (usbdImpl_ == nullptr) {
        USB_HILOGE(MODULE_USB_INNERKIT, "UsbAccessoryManager usbdImpl_ is nullptr.");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbdImpl_->OpenAccessory(fd);
    if (ret == UEC_OK) {
        accFd_ = fd;
        return ret;
    } else if (ret == ACCESSORY_IS_BUSY) {
        return UEC_SERVICE_ACCESSORY_REOPEN;
    }
    return UEC_SERVICE_ACCESSORY_OPEN_NATIVE_NODE_FAILED;
}

int32_t UsbAccessoryManager::CloseAccessory(int32_t fd)
{
    if (usbdImpl_ == nullptr) {
        USB_HILOGE(MODULE_USB_INNERKIT, "UsbAccessoryManager usbdImpl_ is nullptr.");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbdImpl_->CloseAccessory(accFd_);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "%{public}s, close ret: %{public}d, fd: %{public}d.", __func__, ret, fd);
        return ret;
    }
    accFd_ = 0;
    return ret;
}

int32_t UsbAccessoryManager::ProcessAccessoryStart(int32_t curFunc, int32_t curAccStatus)
{
    if ((curFunc & FUN_ACCESSORY) != 0 && accStatus_ != ACC_START) {
        this->accStatus_ = ACC_START;
        std::vector<std::string> accessorys;
        usbdImpl_->GetAccessoryInfo(accessorys);
        this->accessory.SetAccessory(accessorys);
        std::string hashSerial = SerialValueHash(this->accessory.GetSerialNumber());
        this->accessory.SetSerialNumber(hashSerial);
        Want want;
        want.SetAction(CommonEventSupport::COMMON_EVENT_USB_ACCESSORY_ATTACHED);
        CommonEventData data(want);
        data.SetData(this->accessory.GetJsonString().c_str());
        CommonEventPublishInfo publishInfo;
        USB_HILOGI(MODULE_SERVICE, "send accessory attached broadcast device:%{public}s",
            this->accessory.GetJsonString().c_str());
        return CommonEventManager::PublishCommonEvent(data, publishInfo);
    } else if ((curFunc & FUN_ACCESSORY) == 0 && curAccStatus == ACC_CONFIGURING) {
        int32_t ret = usbdImpl_->SetCurrentFunctions(FUN_ACCESSORY);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_SERVICE, "curFunc %{public}d curAccStatus:%{public}d, set func ret: %{public}d",
                curFunc, curAccStatus, ret);
            return ret;
        }
        lastDeviceFunc_ = curFunc;
        auto task = [&]() {
            this->accStatus_ = ACC_STOP;
            int32_t ret = usbdImpl_ ->SetCurrentFunctions(this->lastDeviceFunc_);
            if (ret != UEC_OK) {
                USB_HILOGW(MODULE_SERVICE, "set old func: %{public}d ret: %{public}d", this->lastDeviceFunc_, ret);
            }
            return;
        };
        ret = accDelayTimer_.Setup();
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "set up accDelayTimer_ failed %{public}u", ret);
            return ret;
        }
        accDelayTimerId_ = accDelayTimer_.Register(task, DELAY_ACC_INTERVAL, true);
        this->accStatus_ = ACC_CONFIGURING;
    } else {
        USB_HILOGD(MODULE_SERVICE, "curFunc %{public}d curAccStatus:%{public}d not necessary", curFunc, curAccStatus);
    }
    return UEC_OK;
}

int32_t UsbAccessoryManager::ProcessAccessoryStop(int32_t curFunc, int32_t curAccStatus)
{
    if ((curFunc & FUN_ACCESSORY) != 0 && accStatus_ == ACC_START) {
        accStatus_ = ACC_STOP;
        int32_t ret = usbdImpl_ ->SetCurrentFunctions(lastDeviceFunc_);
        if (ret != UEC_OK) {
            USB_HILOGW(MODULE_SERVICE, "setFunc %{public}d curAccStatus:%{public}d, set func ret: %{public}d",
                lastDeviceFunc_, curAccStatus, ret);
        }
        curDeviceFunc_ = lastDeviceFunc_;
        Want want;
        want.SetAction(CommonEventSupport::COMMON_EVENT_USB_ACCESSORY_DETACHED);
        CommonEventData data(want);
        data.SetData(this->accessory.GetJsonString().c_str());
        CommonEventPublishInfo publishInfo;
        USB_HILOGI(MODULE_SERVICE, "send accessory detached broadcast device:%{public}s",
            this->accessory.GetJsonString().c_str());
        return CommonEventManager::PublishCommonEvent(data, publishInfo);
    } else {
        USB_HILOGD(MODULE_SERVICE, "curFunc %{public}d curAccStatus:%{public}d not necessary",
            curFunc, curAccStatus);
    }
    return UEC_OK;
}

int32_t UsbAccessoryManager::ProcessAccessoryStop()
{
    this->accStatus_ = ACC_SEND;
    std::vector<std::string> accessorys;
    usbdImpl_->GetAccessoryInfo(accessorys);
    this->accessory.SetAccessory(accessorys);
    std::string extraInfo;
    if (accessorys.size() >= ACCESSORY_INFO_SIZE && !accessorys[ACCESSORY_EXTRA_INDEX].empty()) {
        if (base64Map_.empty()) {
            InitBase64Map();
        }
        std::string extraEcode = accessorys[ACCESSORY_EXTRA_INDEX];
        USB_HILOGE(MODULE_USB_SERVICE, "extraEcode, length: %{public}zu, extraData: %{public}s",
            extraEcode.length(), extraEcode.c_str());
        std::vector<uint8_t> extraData = Base64Decode(extraEcode);
        cJSON *root = cJSON_CreateArray();
        for (uint8_t value : extraData) {
            cJSON_AddItemToArray(root, cJSON_CreateNumber(value));
        }
        char *pExtraJson = cJSON_PrintUnformatted(root);
        cJSON_Delete(root);
        if (!pExtraJson) {
            USB_HILOGE(MODULE_USB_SERVICE, "print json error.");
            return UEC_SERVICE_INVALID_VALUE;
        }
        extraInfo = pExtraJson;
        cJSON_free(pExtraJson);
        pExtraJson = nullptr;
    }

    USBAccessory extraAcces;
    extraAcces.SetDescription(extraInfo);
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_USB_ACCESSORY_ATTACHED);
    CommonEventData data(want);
    data.SetData(extraAcces.GetJsonString().c_str());
    CommonEventPublishInfo publishInfo;
    USB_HILOGI(MODULE_SERVICE, "send accessory attached broadcast device:%{public}s",
        extraAcces.GetJsonString().c_str());
    CommonEventManager::PublishCommonEvent(data, publishInfo);
    return UEC_OK;
}

void UsbAccessoryManager::HandleEvent(int32_t status, bool delayProcess)
{
    if (usbdImpl_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbAccessoryManager::usbd_ is nullptr");
        return;
    }
    std::lock_guard<std::mutex> guard(mutexHandleEvent_);
    eventStatus_ = status;
    if (delayProcess) {
        antiShakeDelayTimer_.Unregister(antiShakeDelayTimerId_);
        antiShakeDelayTimer_.Shutdown();
    }
    if ((status == ACT_UPDEVICE || status == ACT_DOWNDEVICE) && delayProcess) {
        auto task = [&]() {
            this->HandleEvent(this->eventStatus_, false);
            return;
        };
        int32_t ret = antiShakeDelayTimer_.Setup();
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "set up antiShakeDelayTimer_ failed %{public}u", ret);
            return;
        }
        antiShakeDelayTimerId_ = antiShakeDelayTimer_.Register(task, ANTI_SHAKE_INTERVAL, true);
        return;
    }
    int32_t curAccStatus = ACC_NONE;
    switch (status) {
        case ACT_UPDEVICE: {
            if (accStatus_ == ACC_CONFIGURING) {
                curAccStatus = ACC_START;
            }
            break;
        }
        case ACT_ACCESSORYUP: {
            curAccStatus = ACC_CONFIGURING;
            break;
        }
        case ACT_DOWNDEVICE:
        case ACT_ACCESSORYDOWN: {
            curAccStatus = ACC_STOP;
            break;
        }
        case ACT_ACCESSORYSEND: {
            curAccStatus = ACC_SEND;
            break;
        }
        default:
            USB_HILOGE(MODULE_USB_SERVICE, "invalid status %{public}d", status);
    }
    ProcessHandle(curAccStatus);
}

void UsbAccessoryManager::ProcessHandle(int32_t curAccStatus)
{
    int32_t ret = UEC_INTERFACE_INVALID_VALUE;
    if ((curAccStatus == ACC_CONFIGURING || curAccStatus == ACC_START)) {
        accDelayTimer_.Unregister(accDelayTimerId_);
        accDelayTimer_.Shutdown();

        int32_t curFunc = 0;
        ret = usbdImpl_->GetCurrentFunctions(curFunc);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "GetCurrentFunctions ret: %{public}d", ret);
            return;
        }
        this->curDeviceFunc_ = curFunc;
        ret = ProcessAccessoryStart(curFunc, curAccStatus);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ProcessAccessoryStart ret: %{public}d", ret);
            return;
        }
    } else if (curAccStatus == ACC_STOP && accStatus_ != ACC_CONFIGURING) {
        ret = ProcessAccessoryStop(curDeviceFunc_, curAccStatus);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ProcessAccessoryStop ret: %{public}d", ret);
            return;
        }
    } else if (curAccStatus == ACC_SEND) {
        ProcessAccessoryStop();
    }
    return;
}

std::string UsbAccessoryManager::SerialValueHash(const std::string&serialValue)
{
    uint64_t timestamp = 0;
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    timestamp = static_cast<uint64_t>(millis);

    std::hash<std::string> string_hash;
    std::hash<uint64_t> int_hash;

    uint32_t hashValue = (string_hash(serialValue) ^ (int_hash(timestamp) << 1));

    std::stringstream ss;
    ss << std::hex << std::setw(NUM_OF_SERAIL_BIT) << std::setfill('0') << hashValue;
    return ss.str();
}

void UsbAccessoryManager::InitBase64Map()
{
    for (size_t i = 0; i < BASE64_CHARS.size(); ++i) {
        base64Map_[BASE64_CHARS[i]] = i;
    }
}

std::vector<uint8_t> UsbAccessoryManager::Base64Decode(const std::string& encoded_string)
{
    std::vector<uint8_t> decoded_data;
    long in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    uint8_t char_array_4[BASE64_CHAR_INDEX4];
    uint8_t char_array_3[BASE64_CHAR_INDEX3];
    while (in_len-- && (encoded_string[in_] != '=') && base64Map_.find(encoded_string[in_]) != base64Map_.end()) {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == BASE64_CHAR_INDEX4) {
            for (i = 0; i < BASE64_CHAR_INDEX4; i++)
                char_array_4[i] = base64Map_[char_array_4[i]];
            char_array_3[BASE64_CHAR_INDEX0] = (char_array_4[BASE64_CHAR_INDEX0] << BASE64_CHAR_INDEX2) +
                ((char_array_4[BASE64_CHAR_INDEX1] & BASE64_CHAR_MASK30) >> BASE64_CHAR_INDEX4);
            char_array_3[BASE64_CHAR_INDEX1] = ((char_array_4[BASE64_CHAR_INDEX1] & BASE64_CHAR_MASKF) <<
                BASE64_CHAR_INDEX4) + ((char_array_4[BASE64_CHAR_INDEX2] & BASE64_CHAR_MASK3C) >> BASE64_CHAR_INDEX2);
            char_array_3[BASE64_CHAR_INDEX2] = ((char_array_4[BASE64_CHAR_INDEX2] & BASE64_CHAR_INDEX3) <<
                BASE64_CHAR_INDEX6) + char_array_4[BASE64_CHAR_INDEX3];

            for (i = 0; (i < BASE64_CHAR_INDEX3); i++) {
                decoded_data.push_back(char_array_3[i]);
            }
            i = BASE64_CHAR_INDEX0;
        }
    }

    if (i) {
        for (j = i; j < BASE64_CHAR_INDEX4; j++)
            char_array_4[j] = BASE64_CHAR_INDEX0;

        for (j = 0; j < BASE64_CHAR_INDEX4; j++)
            char_array_4[j] = base64Map_[char_array_4[j]];

        char_array_3[BASE64_CHAR_INDEX0] = (char_array_4[BASE64_CHAR_INDEX0] << BASE64_CHAR_INDEX2) +
            ((char_array_4[BASE64_CHAR_INDEX1] & BASE64_CHAR_MASK30) >> BASE64_CHAR_INDEX4);
        char_array_3[BASE64_CHAR_INDEX1] = ((char_array_4[BASE64_CHAR_INDEX1] & BASE64_CHAR_MASKF) <<
            BASE64_CHAR_INDEX4) + ((char_array_4[BASE64_CHAR_INDEX2] & BASE64_CHAR_MASK3C) >> BASE64_CHAR_INDEX2);
        char_array_3[BASE64_CHAR_INDEX2] = ((char_array_4[BASE64_CHAR_INDEX2] & BASE64_CHAR_INDEX3) <<
            BASE64_CHAR_INDEX6) + char_array_4[BASE64_CHAR_INDEX3];

        for (j = 0; (j < i - 1); j++) {
            decoded_data.push_back(char_array_3[j]);
        }
    }
    return decoded_data;
}

bool UsbAccessoryManager::compare(const std::string &s1, const std::string &s2)
{
    if (s1.empty())
        return (s2.empty());
    return s1 == s2;
}

int32_t UsbAccessoryManager::GetAccessorySerialNumber(const USBAccessory &access,
    const std::string &bundleName, std::string &serialValue)
{
    USB_HILOGD(MODULE_USB_SERVICE, "%{public}s, bundleName: %{public}s, serial: %{public}s",
        __func__, bundleName.c_str(), this->accessory.GetSerialNumber().c_str());
    if (accStatus_ != ACC_START) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid status %{public}d", accStatus_);
        return UEC_SERVICE_ACCESSORY_NOT_MATCH;
    } else if (compare(this->accessory.GetManufacturer(), access.GetManufacturer()) &&
        (compare(this->accessory.GetProduct(), access.GetProduct())) &&
        compare(this->accessory.GetDescription(), access.GetDescription()) &&
        compare(this->accessory.GetManufacturer(), access.GetManufacturer()) &&
        compare(this->accessory.GetVersion(), access.GetVersion()) &&
        (compare(this->accessory.GetSerialNumber(), access.GetSerialNumber()) ||
        compare(bundleName + this->accessory.GetSerialNumber(), access.GetSerialNumber()))) {
        serialValue = access.GetManufacturer() + access.GetProduct() + access.GetVersion() + access.GetSerialNumber();
        return UEC_OK;
    }
    return UEC_SERVICE_ACCESSORY_NOT_MATCH;
}

} // USB
} // OHOS