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

#include "ability_manager_client.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "usb_errors.h"

using namespace OHOS::AppExecFwk;
using namespace OHOS::EventFwk;

namespace OHOS {
namespace USB {

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

bool UsbFunctionSwitchWindow::ShowFunctionSwitchWindow(int32_t defaultChoose)
{
    USB_HILOGI(MODULE_USB_SERVICE, "show function switch window right now");
    auto abmc = AAFwk::AbilityManagerClient::GetInstance();
    if (abmc == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetInstance failed");
        return false;
    }
    AAFwk::Want want;
    want.SetElementName(functionSwitchBundleName_, functionSwitchExtAbility_);
    want.SetParam("defaultChoose", defaultChoose);

    auto ret = abmc->StartAbility(want);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_SERVICE, "StartAbility failed %{public}d", ret);
        return false;
    }
    USB_HILOGD(MODULE_SERVICE, "StartAbility success");
    return true;
}

bool UsbFunctionSwitchWindow::UnShowFunctionSwitchWindow()
{
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
