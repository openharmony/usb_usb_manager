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

#ifndef USB_FUNCTION_SWITCH_WINDOW_H
#define USB_FUNCTION_SWITCH_WINDOW_H

#include <mutex>

#include "ability_connect_callback_stub.h"
#include "bundle_mgr_interface.h"
#include "parameter.h"
#include "usb_common.h"
#include "usb_srv_support.h"

#define PARAM_BUF_LEN 128
namespace OHOS {
namespace USB {

enum UsbFunctionChoose : int32_t {
    FUNCTION_CHOOSE_CHARGE_ONLY = -1,
    FUNCTION_CHOOSE_TRANSFER_FILE = UsbSrvSupport::FUNCTION_MTP,
    FUNCTION_CHOOSE_TRANSFER_PIC = UsbSrvSupport::FUNCTION_PTP,
};

enum UsbFunctionSwitchWindowAction : int32_t {
    FUNCTION_SWITCH_WINDOW_ACTION_DEFAULT = 0,
    FUNCTION_SWITCH_WINDOW_ACTION_SHOW,
    FUNCTION_SWITCH_WINDOW_ACTION_DISMISS,
    FUNCTION_SWITCH_WINDOW_ACTION_FORBID,
};

class UsbFunctionSwitchWindow {
public:
    static std::shared_ptr<UsbFunctionSwitchWindow> GetInstance();
    ~UsbFunctionSwitchWindow();
    int32_t Init();
    bool PopUpFunctionSwitchWindow();
    bool DismissFunctionSwitchWindow();
    int32_t SetCurrentFunctionLabel(int32_t func);
    int32_t RemoveCurrentFunctionLabel();
private:
    UsbFunctionSwitchWindow();
    DISALLOW_COPY_AND_MOVE(UsbFunctionSwitchWindow);
    class UsbFuncAbilityConn : public OHOS::AAFwk::AbilityConnectionStub {
        void OnAbilityConnectDone(const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject,
            int32_t resultCode) override;
        void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode) override;
        public:
    };
    bool GetDefaultChooseFunction(int32_t &defaultChoose);
    bool ShowFunctionSwitchWindow(int32_t defaultChoose);
    bool UnShowFunctionSwitchWindow();
    static std::shared_ptr<UsbFunctionSwitchWindow> instance_;
    sptr<UsbFuncAbilityConn> usbFuncAbilityConn = nullptr;
    int32_t windowAction_ = UsbFunctionSwitchWindowAction::FUNCTION_SWITCH_WINDOW_ACTION_DEFAULT;
    std::mutex opMutex_;
    const std::string functionSwitchBundleName_ = "com.usb.right";
    const std::string functionSwitchExtAbility_ = "UsbFunctionSwitchExtAbility";
};
} // namespace USB
} // namespace OHOS

#endif
