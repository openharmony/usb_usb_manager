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

#include "usb_right_manager.h"
#include <semaphore.h>
#include <unistd.h>

#include "ability_manager_client.h"
#include "display_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "ui_service_mgr_client.h"
#include "usb_errors.h"

using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace USB {
namespace {
constexpr int32_t UI_DIALOG_USB_WIDTH_NARROW = 400;
constexpr int32_t UI_DIALOG_USB_HEIGHT_NARROW = 240;
} // namespace
void UsbRightManager::Init() {}

bool UsbRightManager::HasRight(const std::string &deviceName, const std::string &bundleName)
{
    auto itMap = rightMap.find(deviceName);
    if (itMap == rightMap.end()) {
        USB_HILOGE(MODULE_USB_SERVICE, "hasRight deviceName false");
        return false;
    } else {
        BundleNameList bundleNameList = itMap->second;
        auto itVevtor = std::find(bundleNameList.begin(), bundleNameList.end(), bundleName);
        if (itVevtor == bundleNameList.end()) {
            USB_HILOGE(MODULE_USB_SERVICE, "hasRight bundleName false");
            return false;
        }
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Request Right Success");
    return true;
}

int32_t UsbRightManager::RequestRight(const std::string &deviceName, const std::string &bundleName)
{
    if (HasRight(deviceName, bundleName)) {
        USB_HILOGE(MODULE_USB_SERVICE, "device has Right ");
        return UEC_OK;
    }

    if (!GetUserAgreementByDiag(deviceName, bundleName)) {
        USB_HILOGW(MODULE_USB_SERVICE, "user don't agree");
        return UEC_SERVICE_PERMISSION_DENIED;
    }
    return UEC_OK;
}

bool UsbRightManager::AddDeviceRight(const std::string &deviceName, const std::string &bundleName)
{
    auto itMap = rightMap.find(deviceName);
    if (itMap != rightMap.end()) {
        auto v = itMap->second;
        auto itVevtor = std::find(v.begin(), v.end(), bundleName);
        if (itVevtor != v.end()) {
            USB_HILOGE(MODULE_USB_SERVICE, "addDeviceRight false");
            return false;
        }
        itMap->second.push_back(bundleName);
        USB_HILOGI(MODULE_USB_SERVICE, "addDeviceRight success");
    }
    BundleNameList bundleNameList;
    bundleNameList.push_back(bundleName);
    rightMap.insert(RightMap::value_type(deviceName, bundleNameList));
    USB_HILOGI(MODULE_USB_SERVICE, "addDeviceRight success");
    return true;
}

bool UsbRightManager::RemoveDeviceRight(const std::string &deviceName)
{
    auto it = rightMap.find(deviceName);
    if (it != rightMap.end()) {
        rightMap.erase(it);
        USB_HILOGI(MODULE_USB_SERVICE, "removeDeviceRight success");
        return true;
    }
    return false;
}

std::pair<int32_t, int32_t> UsbRightManager::GetDisplayPosition()
{
    auto display = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    if (display == nullptr) {
        USB_HILOGI(MODULE_USB_SERVICE, "Dialog GetDefaultDisplay fail, try again.");
        display = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    }

    if (display != nullptr) {
        USB_HILOGI(
            MODULE_USB_SERVICE, "Display size: %{public}d x %{public}d", display->GetWidth(), display->GetHeight());
        return {display->GetWidth(), display->GetHeight()};
    }

    USB_HILOGI(MODULE_USB_SERVICE, "Dialog get display fail, use default wide.");
    return {UI_DIALOG_USB_WIDTH_NARROW, UI_DIALOG_USB_HEIGHT_NARROW};
}

bool UsbRightManager::ShowUsbDialog(const std::string &deviceName, const std::string &bundleName)
{
    auto abmc = AAFwk::AbilityManagerClient::GetInstance();
    if (abmc == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetInstance failed");
        return false;
    }

    AAFwk::Want want;
    want.SetElementName("com.usb.right", "UsbServiceExtAbility");
    want.SetParam("bundleName", bundleName);
    want.SetParam("deviceName", deviceName);
    int32_t result = abmc->StartAbility(want);
    if (result != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "StartRightService failed, result = %{public}d", result);
        return false;
    }
    return true;
}

bool UsbRightManager::GetUserAgreementByDiag(const std::string &deviceName, const std::string &bundleName)
{
#ifdef USB_RIGHT_TEST
    return true;
#endif
    if (dialogId_ >= 0) {
        USB_HILOGW(MODULE_USB_SERVICE, "dialog is already showing");
        return false;
    }

    if (!ShowUsbDialog(deviceName, bundleName)) {
        USB_HILOGE(MODULE_USB_SERVICE, "ShowUsbDialog failed");
        return false;
    }
    return true;
}

sptr<IBundleMgr> UsbRightManager::GetBundleMgr()
{
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        USB_HILOGW(MODULE_USB_SERVICE, "GetSystemAbilityManager return nullptr");
        return nullptr;
    }
    auto bundleMgrSa = sam->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleMgrSa == nullptr) {
        USB_HILOGW(MODULE_USB_SERVICE, "GetSystemAbility return nullptr");
        return nullptr;
    }
    auto bundleMgr = iface_cast<IBundleMgr>(bundleMgrSa);
    if (bundleMgr == nullptr) {
        USB_HILOGW(MODULE_USB_SERVICE, "iface_cast return nullptr");
    }
    return bundleMgr;
}

bool UsbRightManager::IsSystemHap()
{
    pid_t uid = IPCSkeleton::GetCallingUid();
    auto bundleMgr = GetBundleMgr();
    if (bundleMgr == nullptr) {
        USB_HILOGW(MODULE_USB_SERVICE, "BundleMgr is nullptr, return false");
        return false;
    }
    return bundleMgr->CheckIsSystemAppByUid(uid);
}
} // namespace USB
} // namespace OHOS
