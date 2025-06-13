/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "usb_timer_wraper.h"
#include "usb_errors.h"

namespace OHOS {
namespace USB {

std::shared_ptr<UsbTimerWrapper> UsbTimerWrapper::instance_;
std::mutex UsbTimerWrapper::insMutex_;

std::shared_ptr<UsbTimerWrapper> UsbTimerWrapper::GetInstance()
{
    std::lock_guard<std::mutex> guard(insMutex_);
    if (instance_ == nullptr) {
        USB_HILOGI(MODULE_USB_SERVICE, "reset to new instance");
        instance_.reset(new UsbTimerWrapper());
    }
    return instance_;
}

UsbTimerWrapper::UsbTimerWrapper()
{
    int32_t ret = static_cast<int32_t>(usbDelayTimer_.Setup());
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "set up usbDelayTimer_ failed %{public}u", ret);
        return;
    }
}

UsbTimerWrapper::~UsbTimerWrapper()
{
    usbDelayTimer_.Shutdown();
}

void UsbTimerWrapper::Unregister(uint32_t timerId)
{
    usbDelayTimer_.Unregister(timerId);
}

uint32_t UsbTimerWrapper::Register(const TimerCallback& callback, uint32_t interval, bool once)
{
    return usbDelayTimer_.Register(callback, interval, once);
}

}
}