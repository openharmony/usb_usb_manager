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

#include "usb_mass_storage_notification.h"
#include <parameters.h>
#include "hilog_wrapper.h"
#include "want_agent_info.h"
#include "want_agent_helper.h"
#include "notification_normal_content.h"
#include "notification_helper.h"
#include "notification_content.h"
#include "notification_request.h"
#include "resource_manager.h"
#include "locale_config.h"
#include "locale_info.h"
#include "string_wrapper.h"
#include "iservice_registry.h"
#include "bundle_mgr_interface.h"
#include "system_ability_definition.h"
#include "image_source.h"


using namespace OHOS::AAFwk;

namespace OHOS {
namespace USB {
namespace {
    constexpr int16_t USB_DEVICE_CLASS_MASS_STORAGE = 8;
    constexpr int16_t MASS_STORAGE_NOTIFICATION_ID = 100;
    constexpr int32_t REQUEST_CODE = 10;
    const std::string FILEMANAGER_BUNDLE_NAME = "com.ohos.settings";
    const std::string FILEMANAGER_ABILITY_NAME = "MainAbility";
    const std::string CREATOR_BUNDLE_NAME = "com.ohos.settings";
    const std::string SETTINGS_HAP_PATH = "/system/app/Settings/Settings.hap";
    const std::string ICON_NAME = "notification_icon";
    const std::string DEVICE_TYPE = "2in1";
} // namespace

std::shared_ptr<UsbMassStorageNotification> UsbMassStorageNotification::instance_;

std::shared_ptr<UsbMassStorageNotification> UsbMassStorageNotification::GetInstance()
{
    if (instance_ == nullptr) {
        USB_HILOGI(MODULE_USB_SERVICE, "reset to new instance");
        instance_.reset(new UsbMassStorageNotification());
    }
    return instance_;
}

UsbMassStorageNotification::UsbMassStorageNotification()
{
    GetHapString();
    GetHapIcon();
    isNeedNotify = system::GetParameter("const.product.devicetype", "unknown") == DEVICE_TYPE;
}

UsbMassStorageNotification::~UsbMassStorageNotification() {}

void UsbMassStorageNotification::GetHapString()
{
    std::shared_ptr<Global::Resource::ResourceManager> resourceManager(Global::Resource::CreateResourceManager());
    if (resourceManager == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "resourceManager is null");
        return;
    }
    std::unique_ptr<Global::Resource::ResConfig> resConfig(Global::Resource::CreateResConfig());
    if (resConfig == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "resConfig is null");
        return;
    }
    std::map<std::string, std::string> configs;
    OHOS::Global::I18n::LocaleInfo locale(Global::I18n::LocaleConfig::GetSystemLocale(), configs);
    resConfig->SetLocaleInfo(locale.GetLanguage().c_str(), locale.GetScript().c_str(), locale.GetRegion().c_str());
    resourceManager->UpdateResConfig(*resConfig);
    if (!resourceManager->AddResource(SETTINGS_HAP_PATH.c_str())) {
        USB_HILOGE(MODULE_USB_SERVICE, "AddResource failed");
        return;
    }
    for (auto it : notificationMap) {
        std::string outValue;
        resourceManager->GetStringByName(it.first.c_str(), outValue);
        notificationMap[it.first] = outValue;
    }
    return;
}

void UsbMassStorageNotification::GetHapIcon()
{
    std::shared_ptr<Global::Resource::ResourceManager> resourceManager(Global::Resource::CreateResourceManager());
    if (resourceManager == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "resourceManager is null");
        return;
    }
    if (!resourceManager->AddResource(SETTINGS_HAP_PATH.c_str())) {
        USB_HILOGE(MODULE_USB_SERVICE, "AddResource failed");
        return;
    }
    size_t len = 0;
    std::unique_ptr<uint8_t[]> data;
    resourceMgr_->GetMediaDataByName(ICON_NAME.c_str(), len, data);
    Media::SourceOptions opts;
    uint32_t errorCode = 0;
    std::unique_ptr<Media::ImageSource> imageSource =
        Media::ImageSource::CreateImageSource(data.get(), len, opts, errorCode);
    Media::DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = Media::PixelFormat::BGRA_8888;
    if (imageSource) {
        auto pixelMapPtr = imageSource->CreatePixelMap(decodeOpts, errorCode);
        icon = std::shared_ptr<Media::PixelMap>(pixelMapPtr.release());
    }
    if (errorCode != 0 || !icon) {
        USB_HILOGE(MODULE_USB_SERVICE, "Get icon failed");
    }
    return;
}

bool UsbMassStorageNotification::IsMassStorage(const UsbDevice &dev)
{
    for (int32_t i = 0; i < dev.GetConfigCount(); ++i) {
        USBConfig config;
        dev.GetConfig(i, config);
        for (uint32_t j = 0; j < config.GetInterfaceCount(); ++j) {
            UsbInterface interface;
            config.GetInterface(j, interface);
            if (interface.GetClass() == USB_DEVICE_CLASS_MASS_STORAGE) {
                return true;
            }
        }
    }
    return false;
}

void UsbMassStorageNotification::SendNotification(const UsbDevice &dev)
{
    USB_HILOGD(MODULE_USB_SERVICE, "enter SendNotification");
    if (!IsMassStorage(dev)) {
        USB_HILOGD(MODULE_USB_SERVICE, "Send Notification, not Mass Storage, return!");
        return;
    }
    if (!isNeedNotify) {
        USB_HILOGD(MODULE_USB_SERVICE, "no need send Notification, return!");
        return;
    }
    if (notificationMap[MASS_STORAGE_NOTIFICATION_TITLE_KEY].empty() ||
        notificationMap[MASS_STORAGE_NOTIFICATION_TEXT_KEY].empty() ||
        notificationMap[MASS_STORAGE_NOTIFICATION_BUTTON_KEY].empty()) {
        USB_HILOGE(MODULE_USB_SERVICE, "notificationMap is empty， return!");
        return;
    }
    PublishUsbNotification();
}

void UsbMassStorageNotification::PublishUsbNotification()
{
    std::shared_ptr<OHOS::Notification::NotificationNormalContent> normalContent =
    std::make_shared<OHOS::Notification::NotificationNormalContent>();
    if (normalContent == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "notification normal content nullptr");
        return;
    }
    normalContent->SetTitle(notificationMap[MASS_STORAGE_NOTIFICATION_TITLE_KEY]);
    normalContent->SetText(notificationMap[MASS_STORAGE_NOTIFICATION_TEXT_KEY]);
    std::shared_ptr<OHOS::Notification::NotificationContent> content =
        std::make_shared<OHOS::Notification::NotificationContent>(normalContent);
    if (content == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "notification content nullptr");
        return;
    }
    auto want = std::make_shared<OHOS::AAFwk::Want>;
    want.SetElementName(FILEMANAGER_BUNDLE_NAME, FILEMANAGER_ABILITY_NAME);
    std::vector<std::shared_ptr<OHOS::AAFwk::Want>> wants;
    wants.push(want);
    std::vector<OHOS::AbilityRuntime::WantAgent::WantAgentConstant::Flags> flags;
    flags.push_back(OHOS::AbilityRuntime::WantAgent::WantAgentConstant::Flags::UPDATE_PRESENT_FLAG);
    OHOS::AbilityRuntime::WantAgent::WantAgentInfo wantAgentInfo(
        REQUEST_CODE,
        AbilityRuntime::WantAgent::WantAgentConstant::OperationType::START_ABILITY,
        flags, wants, nullptr
    );
    auto wantAgent = OHOS::AbilityRuntime::WantAgent::WantAgentHelper::GetWantAgent(wantAgentInfo);
    std::string buttonTitle = notificationMap[MASS_STORAGE_NOTIFICATION_BUTTON_KEY];
    std::shared_ptr<OHOS::Notification::NotificationActionButton> actionButton =
        OHOS::Notification::NotificationActionButton::Create(nullptr, buttonTitle, wantAgent);
    if (actionButton == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "notification actionButton nullptr");
        return;
    }
    OHOS::Notification::NotificationRequest request;
    request.SetNotificationId(MASS_STORAGE_NOTIFICATION_ID);
    request.SetContent(content);
    request.AddActionButton(actionButton);
    request.SetCreatorUid(USB_SYSTEM_ABILITY_ID);
    request.SetCreatorBundleName(CREATOR_BUNDLE_NAME);
    if (icon.has_value()) {
        request.SetLittleIcon(icon.value());
    }
    request.SetLabel(notificationMap[MASS_STORAGE_NOTIFICATION_LABAL_KEY]);
    request.SetSlotType(OHOS::Notification::NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
    int32_t result = OHOS::Notification::NotificationHelper::PublishNotification(request);
    USB_HILOGI(MODULE_USB_SERVICE, "publish mass storage notification result : %{public}d", result);
}

void UsbMassStorageNotification::CancelNotification(const std::map<std::string, UsbDevice *> &devices,
    const UsbDevice &dev, const std::string &name)
{
    if (!IsMassStorage(dev)) {
        USB_HILOGD(MODULE_USB_SERVICE, "Cancel Notification, not Mass Storage, return!");
        return;
    }
    if (!isNeedNotify) {
        USB_HILOGD(MODULE_USB_SERVICE, "no need Cancel Notification, return!");
        return;
    }
    for (auto it : devices) {
        UsbDevice *device = it.second;
        if (device == nullptr) {
            continue;
        }
        if ((it.first != name) && IsMassStorage(*device)) {
            USB_HILOGD(MODULE_USB_SERVICE, "Cancel Notification, still has other Mass Storage, return!");
            return;
        }
    }
    int32_t result = OHOS::Notification::NotificationHelper::CancelNotification(MASS_STORAGE_NOTIFICATION_ID);
    USB_HILOGI(MODULE_USB_SERVICE, "Cancel mass storage notification result : %{public}d", result);
}
} // namespace USB
} // namespace OHOS
