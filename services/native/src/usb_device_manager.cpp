/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include <regex>
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
constexpr int32_t DECIMAL_BASE = 10;
constexpr uint32_t CMD_INDEX = 1;
constexpr uint32_t PARAM_INDEX = 2;
constexpr uint32_t DELAY_CONNECT_INTERVAL = 300;
constexpr uint32_t DELAY_DISCONN_INTERVAL = 1400;
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
    auto ret = delayDisconn_.Setup();
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "set up timer failed %{public}u", ret);
    }
}

UsbDeviceManager::~UsbDeviceManager()
{
    delayDisconn_.Shutdown();
}

int32_t UsbDeviceManager::Init()
{
    std::shared_ptr<UsbFunctionSwitchWindow> window_ = UsbFunctionSwitchWindow::GetInstance();
    if (window_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "get usb function switch window failed");
        return UEC_SERVICE_INNER_ERR;
    }

    int32_t ret = window_->Init();
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Init usb function switch window failed");
    }
    return ret;
}

int32_t UsbDeviceManager::SetUsbd(const sptr<IUsbInterface> &usbd)
{
    if (usbd == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbDeviceManager usbd is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    usbd_ = usbd;
    return UEC_OK;
}

bool UsbDeviceManager::IsSettableFunctions(int32_t funcs)
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
        return std::string {UsbSrvSupport::FUNCTION_NAME_NONE};
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
    if (func == currentFunctions_) {
        return;
    }
    ReportFuncChangeSysEvent(currentFunctions_, func);
    currentFunctions_ = func;
    BroadcastFuncChange(connected_, currentFunctions_);
}

int32_t UsbDeviceManager::GetCurrentFunctions()
{
    return currentFunctions_;
}

void UsbDeviceManager::HandleEvent(int32_t status)
{
    if (usbd_ == nullptr) {
        return;
    }
    bool curConnect = false;
    switch (status) {
        case ACT_UPDEVICE:
            curConnect = true;
            gadgetConnected_ = true;
            break;
        case ACT_DOWNDEVICE:
            curConnect = false;
            gadgetConnected_ = false;
            break;
        case ACT_ACCESSORYUP:
        case ACT_ACCESSORYDOWN:
        case ACT_ACCESSORYSEND:
            ProcessFunctionSwitchWindow(false);
            return;
        default:
            USB_HILOGE(MODULE_USB_SERVICE, "invalid status %{public}d", status);
            return;
    }
    delayDisconn_.Unregister(delayDisconnTimerId_);
    if (curConnect && (connected_ != curConnect)) {
        auto task = [&]() {
            connected_ = true;
            usbd_->GetCurrentFunctions(currentFunctions_);
            ProcessFuncChange(connected_, currentFunctions_);
        };
        delayDisconnTimerId_ = delayDisconn_.Register(task, DELAY_CONNECT_INTERVAL, true);
    } else if (!curConnect && (connected_ != curConnect)) {
        auto task = [&]() {
            connected_ = false;
            uint32_t functions = static_cast<uint32_t>(currentFunctions_);
            if ((functions & USB_FUNCTION_MTP) != 0 || (functions & USB_FUNCTION_PTP) != 0) {
                functions = functions & (~USB_FUNCTION_MTP) & (~USB_FUNCTION_PTP);
                USB_HILOGI(MODULE_USB_SERVICE, "usb function reset %{public}u", functions);
                functions = functions == 0 ? USB_FUNCTION_STORAGE : functions;
                usbd_->SetCurrentFunctions(functions);
            }
            ProcessFuncChange(connected_, functions);
            currentFunctions_ = static_cast<int32_t>(functions);
            return;
        };
        delayDisconnTimerId_ = delayDisconn_.Register(task, DELAY_DISCONN_INTERVAL, true);
    } else {
        USB_HILOGI(MODULE_USB_SERVICE, "else info cur status %{public}d, bconnected: %{public}d", status, connected_);
    }
}

int32_t UsbDeviceManager::UserChangeProcess()
{
    if ((currentFunctions_ & USB_FUNCTION_MTP) != 0 || (currentFunctions_ & USB_FUNCTION_PTP) != 0) {
        currentFunctions_ = currentFunctions_ & (~USB_FUNCTION_MTP) & (~USB_FUNCTION_PTP);
        currentFunctions_ = currentFunctions_ == 0 ? USB_FUNCTION_STORAGE : currentFunctions_;
        USB_HILOGI(MODULE_USB_SERVICE, "usb function reset %{public}d", currentFunctions_);
        return usbd_->SetCurrentFunctions(currentFunctions_);
    }
    return UEC_OK;
}

void UsbDeviceManager::BroadcastFuncChange(bool connected, int32_t currentFunc)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Current Connect %{public}d,bconnected: %{public}d", connected, currentFunc);
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_USB_STATE);
 
    want.SetParam(std::string {UsbSrvSupport::CONNECTED}, connected);
    uint32_t remainderFunc = static_cast<uint32_t>(currentFunc);
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
    USB_HILOGI(MODULE_SERVICE, "send COMMON_EVENT_USB_STATE broadcast connected:%{public}d, "
        "currentFunctions:%{public}d", connected, currentFunc);
    CommonEventManager::PublishCommonEvent(data, publishInfo);
    ReportDevicePlugSysEvent(currentFunc, connected);
}

void UsbDeviceManager::ProcessFuncChange(bool connected, int32_t currentFunc)
{
    BroadcastFuncChange(connected, currentFunc);
    ProcessFunctionSwitchWindow(connected);
}

void UsbDeviceManager::ProcessFunctionSwitchWindow(bool connected)
{
    std::shared_ptr<UsbFunctionSwitchWindow> window_ = UsbFunctionSwitchWindow::GetInstance();
    if (window_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "show window: get usb function switch window failed");
        return;
    }

    if (connected) {
        USB_HILOGD(MODULE_USB_SERVICE, "start pop up usb service switch window");
        if (!window_->PopUpFunctionSwitchWindow()) {
            USB_HILOGE(MODULE_USB_SERVICE, "start pop up usb service switch window failed");
        }
    } else {
        USB_HILOGD(MODULE_USB_SERVICE, "start dismiss usb service switch window");
        if (!window_->DismissFunctionSwitchWindow()) {
            USB_HILOGE(MODULE_USB_SERVICE, "start dismiss usb service switch window failed");
        }
    }
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

bool StringToInteger(const std::string &str, int32_t &result)
{
    char *endptr = nullptr;
    errno = 0;
    int64_t number = std::strtol(str.c_str(), &endptr, DECIMAL_BASE);
    if (errno != 0 || number < INT32_MIN || number > INT32_MAX) {
        USB_HILOGE(MODULE_USB_SERVICE, "number is out of range");
        return false;
    }
    if (endptr == nullptr || endptr == str.c_str() || *endptr != '\0') {
        USB_HILOGE(MODULE_USB_SERVICE, "conversion failed");
        return false;
    }
    result = static_cast<int32_t>(number);
    return true;
}

void UsbDeviceManager::DumpSetFunc(int32_t fd, const std::string &args)
{
    int32_t currentFunction;
    int32_t ret;
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbDeviceManager::DumpSetFunc usbd_ is nullptr");
        return;
    }
    if (args.compare("Q") == 0) {
        ret = usbd_->GetCurrentFunctions(currentFunction);
        if (ret != UEC_OK) {
            dprintf(fd, "GetCurrentFunctions failed: %d\n", ret);
            return;
        }
        dprintf(fd, "current function: %s\n", ConvertToString(currentFunction).c_str());
        return;
    }
    if (!std::regex_match(args, std::regex("^[0-9]+$"))) {
        dprintf(fd, "Invalid input, please enter a valid integer\n");
        GetDumpHelp(fd);
        return;
    }
    int32_t mode;
    if (!StringToInteger(args, mode)) {
        dprintf(fd, "Invalid input, the number is out of range\n");
        GetDumpHelp(fd);
        return;
    }
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
    HiSysEventWrite(HiSysEvent::Domain::USB, "FUNCTION_CHANGED",
        HiSysEvent::EventType::BEHAVIOR, "CURRENT_FUNCTION",
        currentFunctions_, "UPDATE_FUNCTION", updateFunctions);
}

void UsbDeviceManager::ReportDevicePlugSysEvent(int32_t currentFunctions, bool connected)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Device mode Indicates the insertion and removal information:");
    HiSysEventWrite(HiSysEvent::Domain::USB, "PLUG_IN_OUT_DEVICE_MODE",
        HiSysEvent::EventType::BEHAVIOR, "CURRENT_FUNCTIONS",
        currentFunctions, "CONNECTED", connected);
}
bool UsbDeviceManager::IsGadgetConnected(void)
{
    return gadgetConnected_;
}
} // namespace USB
} // namespace OHOS
