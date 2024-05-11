/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "usb_function_switch_window.h"

#include <semaphore.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include "cJSON.h"

#include "ability_manager_client.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "usb_errors.h"

using namespace OHOS::AppExecFwk;
using namespace OHOS::EventFwk;

namespace OHOS {
namespace USB {
constexpr int32_t INVALID_USERID = -1;
constexpr int32_t MESSAGE_PARCEL_KEY_SIZE = 3;

std::shared_ptr<UsbFunctionSwitchWindow> UsbFunctionSwitchWindow::instance_;

std::shared_ptr<UsbFunctionSwitchWindow> UsbFunctionSwitchWindow::GetInstance()
{
    if (instance_ == nullptr) {
        USB_HILOGI(MODULE_USB_SERVICE, "reset to new instance");
        instance_.reset(new UsbFunctionSwitchWindow());
    }
    return instance_;
}

UsbFunctionSwitchWindow::UsbFunctionSwitchWindow() {}

UsbFunctionSwitchWindow::~UsbFunctionSwitchWindow()
{
    if (windowAction_ == UsbFunctionSwitchWindowAction::FUNCTION_SWITCH_WINDOW_ACTION_SHOW) {
        (void)UnShowFunctionSwitchWindow();
    }
}

int32_t UsbFunctionSwitchWindow::Init()
{
    USB_HILOGI(MODULE_USB_SERVICE, "init: windown action=%{public}d", windowAction_);
    return UEC_OK;
}

bool UsbFunctionSwitchWindow::PopUpFunctionSwitchWindow()
{
    USB_HILOGI(MODULE_USB_SERVICE, "pop up function switch window");
    char paramValue[PARAM_BUF_LEN] = { 0 };
    const char defaultValue[PARAM_BUF_LEN] = { 0 };
    std::lock_guard<std::mutex> guard(opMutex_);
    int32_t ret = GetParameter("persist.usb.setting.gadget_conn_prompt", defaultValue, paramValue, sizeof(paramValue));
    if (ret < 0) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetParameter fail");
        return false;
    }
    ret = strcmp(paramValue, "true");
    if (ret != 0) {
        USB_HILOGE(MODULE_USB_SERVICE, "not allow open");
        return false;
    }
    if (windowAction_ == UsbFunctionSwitchWindowAction::FUNCTION_SWITCH_WINDOW_ACTION_FORBID) {
        USB_HILOGI(MODULE_USB_SERVICE, "forbid: pop up function switch window");
        return false;
    }
    windowAction_ = UsbFunctionSwitchWindowAction::FUNCTION_SWITCH_WINDOW_ACTION_SHOW;
    int32_t defaultChoose = 0;
    (void)GetDefaultChooseFunction(defaultChoose);
    return ShowFunctionSwitchWindow(defaultChoose);
}

bool UsbFunctionSwitchWindow::DismissFunctionSwitchWindow()
{
    USB_HILOGI(MODULE_USB_SERVICE, "dismiss function switch window");
    std::lock_guard<std::mutex> guard(opMutex_);
    if (windowAction_ == UsbFunctionSwitchWindowAction::FUNCTION_SWITCH_WINDOW_ACTION_FORBID) {
        USB_HILOGI(MODULE_USB_SERVICE, "forbid: dismiss function switch window");
        return false;
    }
    windowAction_ = UsbFunctionSwitchWindowAction::FUNCTION_SWITCH_WINDOW_ACTION_DISMISS;
    return UnShowFunctionSwitchWindow();
}

bool UsbFunctionSwitchWindow::GetDefaultChooseFunction(int32_t &defaultChoose)
{
    defaultChoose = UsbFunctionChoose::FUNCTION_CHOOSE_CHARGE_ONLY;
    return true;
}
void UsbFunctionSwitchWindow::UsbFuncAbilityConn::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    USB_HILOGI(MODULE_USB_SERVICE, "OnAbilityConnectDone");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInt32(MESSAGE_PARCEL_KEY_SIZE);
    data.WriteString16(u"bundleName");
    data.WriteString16(u"com.usb.right");
    data.WriteString16(u"abilityName");
    data.WriteString16(u"UsbFunctionSwitchExtAbility");
    data.WriteString16(u"parameters");
    cJSON* paramJson = cJSON_CreateObject();
    std::string uiExtensionTypeStr = "sysDialog/common";
    cJSON_AddStringToObject(paramJson, "ability.want.params.uiExtensionType", uiExtensionTypeStr.c_str());
    std::string paramStr(cJSON_PrintUnformatted(paramJson));
    data.WriteString16(Str8ToStr16(paramStr));
    cJSON_Delete(paramJson);
    paramJson = nullptr;
    
    const uint32_t cmdCode = 1;
    int32_t ret = remoteObject->SendRequest(cmdCode, data, reply, option);
    if (ret != ERR_OK) {
        USB_HILOGI(MODULE_USB_SERVICE, "show dialog is failed: %{public}d", ret);
        return;
    }

    return;
}

void UsbFunctionSwitchWindow::UsbFuncAbilityConn::OnAbilityDisconnectDone(
    const AppExecFwk::ElementName& element, int resultCode)
{
    USB_HILOGI(MODULE_USB_SERVICE, "OnAbilityDisconnectDone");
    return;
}

bool UsbFunctionSwitchWindow::ShowFunctionSwitchWindow(int32_t defaultChoose)
{
    USB_HILOGI(MODULE_USB_SERVICE, "show function switch window right now");
    if (usbFuncAbilityConn == nullptr) {
        usbFuncAbilityConn = sptr<UsbFuncAbilityConn>(new (std::nothrow) UsbFuncAbilityConn());
    }

    auto abilityManager = AAFwk::AbilityManagerClient::GetInstance();
    if (abilityManager == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "AbilityManagerClient is nullptr");
        return false;
    }

    AAFwk::Want want;
    want.SetElementName("com.ohos.sceneboard", "com.ohos.sceneboard.systemdialog");
    auto ret = abilityManager->ConnectAbility(want, usbFuncAbilityConn, INVALID_USERID);
    if (ret != ERR_OK) {
        want.SetElementName("com.ohos.systemui", "com.ohos.systemui.dialog");
        ret = abilityManager->ConnectAbility(want, usbFuncAbilityConn, INVALID_USERID);
        if (ret != ERR_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ConnectServiceExtensionAbility systemui failed, ret: %{public}d", ret);
            usbFuncAbilityConn = nullptr;
            return false;
        }
    }
    USB_HILOGI(MODULE_SERVICE, "StartAbility success, ret: %{public}d", ret);
    return true;
}

bool UsbFunctionSwitchWindow::UnShowFunctionSwitchWindow()
{
    if (usbFuncAbilityConn == nullptr) {
        return true;
    }

    auto abmc = AAFwk::AbilityManagerClient::GetInstance();
    if (abmc == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetInstance failed");
        return false;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "unshow function switch window");
    AAFwk::Want want;
    want.SetElementName(functionSwitchBundleName_, functionSwitchExtAbility_);
    auto ret = abmc->StopServiceAbility(want);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_SERVICE, "StopServiceAbility failed %{public}d", ret);
        if (abmc->KillProcess(functionSwitchBundleName_) != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "KillProcess failed");
        }
        return false;
    }
    USB_HILOGD(MODULE_USB_SERVICE, "unshow function switch window success");
    return true;
}

} // namespace USB
} // namespace OHOS
