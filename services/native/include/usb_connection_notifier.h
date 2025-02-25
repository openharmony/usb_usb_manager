/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
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

#ifndef USB_CONNECTION_NOTIFIER_H
#define USB_CONNECTION_NOTIFIER_H
#include <optional>
#include <string>
#include "usb_device.h"
#include "pixel_map.h"
#include "notification_helper.h"
#include "notification_content.h"
#include "notification_request.h"

namespace OHOS {
namespace USB {

using OptionalPixelMap = std::optional<std::shared_ptr<Media::PixelMap>>;

const std::string USB_FUNC_CHARGE = "USB_func_charge_notifier";
const std::string USB_FUNC_MTP = "USB_func_mtp_notifier";
const std::string USB_FUNC_PTP = "USB_func_ptp_notifier";
const std::string USB_FUNC_MORE = "USB_func_more_notifier";
const std::string USB_HDC_NOTIFIER_TITLE = "USB_hdc_notifier_title";
const std::string USB_HDC_NOTIFIER_CONTENT = "USB_hdc_notifier_content";

class UsbConnectionNotifier {
public:
    static std::shared_ptr<UsbConnectionNotifier> GetInstance();
    ~UsbConnectionNotifier();
    void SendNotification(std::string func);
    void CancelNotification();
    void SendHdcNotification();
    void CancelHdcNotification();

    std::unordered_map<std::string, std::string> notifierMap = {
        {USB_FUNC_CHARGE, ""},
        {USB_FUNC_MTP, ""},
        {USB_FUNC_PTP, ""},
        {USB_FUNC_MORE, ""},
        {USB_HDC_NOTIFIER_TITLE, ""},
        {USB_HDC_NOTIFIER_CONTENT, ""}
    };

private:
    UsbConnectionNotifier();
    void SetWantAgent(OHOS::Notification::NotificationRequest &request);
    void SetWantAgentHdc(OHOS::Notification::NotificationRequest &request);
    static std::mutex insMutex_;
    void GetHapString();
    void GetHapIcon();
    void GetSetUid();
    int32_t GetSupportedFunctions();
    int32_t usbSupportedFunctions_ = 0;
    sptr<AppExecFwk::IBundleMgr> GetBundleMgr(void);

    static std::shared_ptr<UsbConnectionNotifier> instance_;
    OHOS::Notification::NotificationRequest request_;
    OHOS::Notification::NotificationRequest requestHdc_;
    OptionalPixelMap icon_;
};
} // namespace USB
} // namespace OHOS
#endif // USB_CONNECTION_NOTIFIER_H
