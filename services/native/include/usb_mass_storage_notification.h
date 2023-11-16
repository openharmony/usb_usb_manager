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

#ifndef USB_MASS_STORAGE_NOTIFICATION_H
#define USB_MASS_STORAGE_NOTIFICATION_H

#include <optional>
#include <string>
#include "usb_device.h"
#include "pixel_map.h"

namespace OHOS {
namespace USB {

using OptionalPixelMap = std::optional<std::shared_ptr<Media::PixelMap>>;

const std::string MASS_STORAGE_NOTIFICATION_TITLE_KEY = "notification_title";
const std::string MASS_STORAGE_NOTIFICATION_TEXT_KEY = "notification_text";
const std::string MASS_STORAGE_NOTIFICATION_BUTTON_KEY = "notification_button";
const std::string MASS_STORAGE_NOTIFICATION_LABAL_KEY = "notification_label";

class UsbMassStorageNotification {
public:
    static std::shared_ptr<UsbMassStorageNotification> GetInstance();
    ~UsbMassStorageNotification();
    void SendNotification(const UsbDevice &dev);
    void CancelNotification(const std::map<std::string, UsbDevice *> &devices,
        const UsbDevice &dev, const std::string &name);

private:
    UsbMassStorageNotification();
    void PublishUsbNotification();
    bool IsMassStorage(const UsbDevice &dev);
    void GetHapString();
    void GetHapIcon();
    void GetFilemanagerBundleName();

    static std::shared_ptr<UsbMassStorageNotification> instance_;
    OptionalPixelMap icon;
    std::map<std::string, std::string> notificationMap = {
        {MASS_STORAGE_NOTIFICATION_TITLE_KEY, ""},
        {MASS_STORAGE_NOTIFICATION_TEXT_KEY, ""},
        {MASS_STORAGE_NOTIFICATION_BUTTON_KEY, ""},
        {MASS_STORAGE_NOTIFICATION_LABAL_KEY, ""}
    };
    std::string filemanagerBundleName;
};
} // namespace USB
} // namespace OHOS

#endif
