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

#include "usb_device_manager.h"
#include <hdf_base.h>
#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "hisysevent.h"
#include "usb_errors.h"
#include "usb_srv_support.h"
#include "usbd_type.h"

using namespace OHOS::AAFwk;
using namespace OHOS::EventFwk;
using namespace OHOS::HiviewDFX;
using namespace OHOS::HDI::Usb::V1_0;

namespace OHOS {
namespace USB {
constexpr int32_t PARAM_COUNT_TWO = 2;
constexpr int32_t PARAM_COUNT_THR = 3;
constexpr uint32_t CMD_INDEX = 1;
constexpr uint32_t PARAM_INDEX = 2;
const std::map<std::string_view, uint32_t> UsbDeviceManager::FUNCTION_MAPPING_N2C = {
    {UsbSrvSupport::FUNCTION_NAME_NONE, UsbSrvSupport::FUNCTION_NONE},
    {UsbSrvSupport::FUNCTION_NAME_ACM, UsbSrvSupport::FUNCTION_ACM},
    {UsbSrvSupport::FUNCTION_NAME_ECM, UsbSrvSupport::FUNCTION_ECM},
    {UsbSrvSupport::FUNCTION_NAME_HDC, UsbSrvSupport::FUNCTION_HDC},
    {UsbSrvSupport::FUNCTION_NAME_MTP, UsbSrvSupport::FUNCTION_MTP},
    {UsbSrvSupport::FUNCTION_NAME_PTP, UsbSrvSupport::FUNCTION_PTP},
    {UsbSrvSupport::FUNCTION_NAME_RNDIS, UsbSrvSupport::FUNCTION_RNDIS},
    {UsbSrvSupport::FUNCTION_NAME_STORAGE, UsbSrvSupport::FUNCTION_STORAGE},
};

UsbDeviceManager::UsbDeviceManager()
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceManager::Init start");
    usbd_ = IUsbInterface::Get();
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbDeviceManager::Get inteface failed");
    }
}

bool UsbDeviceManager::AreSettableFunctions(int32_t funcs)
{
    return static_cast<uint32_t>(funcs) == UsbSrvSupport::FUNCTION_NONE ||
        ((~functionSettable_ & static_cast<uint32_t>(funcs)) == 0);
}

uint32_t UsbDeviceManager::ConvertFromString(std::string_view strFun)
{
    if (strFun == UsbSrvSupport::FUNCTION_NAME_NONE) {
        return UsbSrvSupport::FUNCTION_NONE;
    }

    size_t len = strFun.length();
    if (len == 0) {
        return UEC_SERVICE_INVALID_VALUE;
    }

    std::vector<std::string_view> vModeStr;
    size_t pos = 0;
    while (pos < len) {
        size_t findPos = strFun.find(",", pos);
        if (findPos == strFun.npos) {
            vModeStr.push_back(strFun.substr(pos, len - pos));
            break;
        }
        vModeStr.push_back(strFun.substr(pos, findPos - pos));
        pos = findPos + 1;
    }

    uint32_t ret = 0;
    for (auto &&item : vModeStr) {
        auto it = FUNCTION_MAPPING_N2C.find(item);
        if (it != FUNCTION_MAPPING_N2C.end()) {
            ret |= it->second;
        } else {
            USB_HILOGE(MODULE_USB_SERVICE, "UsbDeviceManager::ConvertFromString Invalid argument of usb function");
            return UEC_SERVICE_INVALID_VALUE;
        }
    }

    return ret;
}

std::string UsbDeviceManager::ConvertToString(uint32_t function)
{
    std::string stream;
    if (function <= UsbSrvSupport::FUNCTION_NONE || function > functionSettable_) {
        stream = std::string {UsbSrvSupport::FUNCTION_NAME_NONE};
        return stream;
    }
    bool flag = false;
    for (auto it = FUNCTION_MAPPING_N2C.begin(); it != FUNCTION_MAPPING_N2C.end(); ++it) {
        if ((function & it->second) != 0) {
            if (flag) {
                stream += ",";
            }
            stream += std::string {it->first};
            flag = true;
        }
    }
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceManager::ConvertToString success");
    return stream;
}

void UsbDeviceManager::UpdateFunctions(int32_t func)
{
    ReportFuncChangeSysEvent(currentFunctions_, func);
    currentFunctions_ = func;
}

int32_t UsbDeviceManager::GetCurrentFunctions()
{
    return currentFunctions_;
}

void UsbDeviceManager::HandleEvent(int32_t status)
{
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbDeviceManager::usbd_ is nullptr");
        return;
    }
    switch (status) {
        case ACT_UPDEVICE: {
            connected_ = true;
            usbd_->GetCurrentFunctions(currentFunctions_);
            break;
        }
        case ACT_DOWNDEVICE: {
            connected_ = false;
            break;
        }
        default:
            USB_HILOGE(MODULE_USB_SERVICE, "invalid status %{public}d", status);
            return;
    }

    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_USB_STATE);

    want.SetParam(std::string {UsbSrvSupport::CONNECTED}, connected_);
    uint32_t remainderFunc = static_cast<uint32_t>(currentFunctions_);
    // start from bit 1
    uint32_t bit = 1;
    while (remainderFunc != 0) {
        if (remainderFunc & bit) {
            want.SetParam(ConvertToString(bit), true);
            // set current bit to zero
            remainderFunc &= ~bit;
        }
        // 1 means to next bit
        bit = bit << 1;
    }

    CommonEventData data(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    USB_HILOGI(MODULE_SERVICE,
        "send COMMON_EVENT_USB_STATE broadcast connected:%{public}d, currentFunctions:%{public}u", connected_,
        currentFunctions_);
    CommonEventManager::PublishCommonEvent(data, publishInfo);
    ReportDevicePlugSysEvent(currentFunctions_, connected_);
}

void UsbDeviceManager::GetDumpHelp(int32_t fd)
{
    dprintf(fd, "========= dump the all device function =========\n");
    dprintf(fd, "usb_device -a:    Query all function\n");
    dprintf(fd, "usb_device -f Q:  Query Current function\n");
    dprintf(fd, "usb_device -f 0:  Switch to function:none\n");
    dprintf(fd, "usb_device -f 1:  Switch to function:acm\n");
    dprintf(fd, "usb_device -f 2:  Switch to function:ecm\n");
    dprintf(fd, "usb_device -f 3:  Switch to function:acm&ecm\n");
    dprintf(fd, "usb_device -f 4:  Switch to function:hdc\n");
    dprintf(fd, "usb_device -f 5:  Switch to function:acm&hdc\n");
    dprintf(fd, "usb_device -f 6:  Switch to function:ecm&hdc\n");
    dprintf(fd, "usb_device -f 32: Switch to function:rndis\n");
    dprintf(fd, "usb_device -f 512:Switch to function:storage\n");
    dprintf(fd, "usb_device -f 36: Switch to function:rndis&hdc\n");
    dprintf(fd, "usb_device -f 516:Switch to function:storage&hdc\n");
    dprintf(fd, "------------------------------------------------\n");
}

void UsbDeviceManager::DumpGetSupportFunc(int32_t fd)
{
    dprintf(fd, "Usb Device function list info:\n");
    dprintf(fd, "current function: %s\n", ConvertToString(currentFunctions_).c_str());
    dprintf(fd, "supported functions list: %s\n", ConvertToString(functionSettable_).c_str());
}

void UsbDeviceManager::DumpSetFunc(int32_t fd, const std::string &args)
{
    int32_t currentFunction;
    int32_t ret;
    if (args.compare("Q") == 0) {
        ret = usbd_->GetCurrentFunctions(currentFunction);
        if (ret != UEC_OK) {
            dprintf(fd, "GetCurrentFunctions failed: %d\n", __LINE__);
            return;
        }
        dprintf(fd, "current function: %s\n", ConvertToString(currentFunction).c_str());
        return;
    }

    int32_t mode = stoi(args);
    ret = usbd_->SetCurrentFunctions(mode);
    if (ret != UEC_OK) {
        dprintf(fd, "SetCurrentFunctions failed");
        return;
    }
    ret = usbd_->GetCurrentFunctions(currentFunction);
    if (ret != UEC_OK) {
        dprintf(fd, "GetCurrentFunctions failed: %d\n", __LINE__);
        return;
    }

    dprintf(fd, "current function: %s\n", ConvertToString(currentFunction).c_str());
}

void UsbDeviceManager::Dump(int32_t fd, const std::vector<std::string> &args)
{
    if (args.size() < PARAM_COUNT_TWO || args.size() > PARAM_COUNT_THR) {
        GetDumpHelp(fd);
        return;
    }

    if (args[CMD_INDEX] == "-a" && args.size() == PARAM_COUNT_TWO) {
        DumpGetSupportFunc(fd);
    } else if (args[CMD_INDEX] == "-f" && args.size() == PARAM_COUNT_THR) {
        DumpSetFunc(fd, args[PARAM_INDEX]);
    } else {
        dprintf(fd, "func param error, please enter again\n");
        GetDumpHelp(fd);
    }
}

void UsbDeviceManager::ReportFuncChangeSysEvent(int32_t currentFunctions, int32_t updateFunctions)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Device function Indicates the switch point information:");
    HiSysEventWrite(HiSysEvent::Domain::USB, "USB_FUNCTION_CHANGED",
        HiSysEvent::EventType::BEHAVIOR, "CURRENT_FUNCTION",
        currentFunctions_, "UPDATE_FUNCTION", updateFunctions);
}

void UsbDeviceManager::ReportDevicePlugSysEvent(int32_t currentFunctions, bool connected)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Device mode Indicates the insertion and removal information:");
    HiSysEventWrite(HiSysEvent::Domain::USB, "USB_PLUG_IN_OUT_DEVICE_MODE",
        HiSysEvent::EventType::BEHAVIOR, "CURRENT_FUNCTIONS",
        currentFunctions, "CONNECTED", connected);
}
} // namespace USB
} // namespace OHOS
