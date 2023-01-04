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
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "tokenid_kit.h"
#include "usb_errors.h"

using namespace OHOS::AppExecFwk;
using namespace OHOS::Security::AccessToken;

namespace OHOS {
namespace USB {

sem_t UsbRightManager::waitDialogDisappear_ {0};

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

bool UsbRightManager::RemoveDeviceRight(const std::string &deviceName, const std::string &bundleName)
{
    auto it = rightMap.find(deviceName);
    if (it != rightMap.end()) {
        auto &v = it->second;
        auto itVevtor = std::find(v.begin(), v.end(), bundleName);
        if (itVevtor != v.end()) {
            it->second.erase(itVevtor);
            return true;
        }
    }
    USB_HILOGI(MODULE_USB_SERVICE, "RemoveDeviceRight failed");
    return false;
}

bool UsbRightManager::RemoveDeviceAllRight(const std::string &deviceName)
{
    auto it = rightMap.find(deviceName);
    if (it != rightMap.end()) {
        rightMap.erase(it);
        USB_HILOGI(MODULE_USB_SERVICE, "removeDeviceAllRight success");
        return true;
    }
    return false;
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

    sptr<UsbAbilityConn> usbAbilityConn_ = new (std::nothrow) UsbAbilityConn();
    sem_init(&waitDialogDisappear_, 1, 0);
    auto ret = abmc->ConnectAbility(want, usbAbilityConn_, -1);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_SERVICE, "connectAbility failed %{public}d", ret);
        return false;
    }

    // Waiting for the user to click
    sem_wait(&waitDialogDisappear_);
    return true;
}

bool UsbRightManager::GetUserAgreementByDiag(const std::string &deviceName, const std::string &bundleName)
{
#ifdef USB_RIGHT_TEST
    return true;
#endif
    // There can only be one dialog at a time
    std::lock_guard<std::mutex> guard(dialogRunning_);
    if (!ShowUsbDialog(deviceName, bundleName)) {
        USB_HILOGE(MODULE_USB_SERVICE, "ShowUsbDialog failed");
        return false;
    }

    return HasRight(deviceName, bundleName);
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

static bool IsTokenAplMatch(ATokenAplEnum apl)
{
    uint64_t tokenId = IPCSkeleton::GetCallingFullTokenID();
    NativeTokenInfo info;
    AccessTokenKit::GetNativeTokenInfo(tokenId, info);
    if (info.apl == apl) {
        return true;
    }
    return false;
}

bool UsbRightManager::IsSystemCore()
{
    bool isMatch = IsTokenAplMatch(ATokenAplEnum::APL_SYSTEM_CORE);
    if (!isMatch) {
        USB_HILOGW(MODULE_USB_SERVICE, "access token denied");
    }
    return isMatch;
}

bool UsbRightManager::IsSystemBasic()
{
    bool isMatch = IsTokenAplMatch(ATokenAplEnum::APL_SYSTEM_BASIC);
    if (!isMatch) {
        USB_HILOGW(MODULE_USB_SERVICE, "access token denied");
    }
    return isMatch;
}

bool UsbRightManager::IsSystemApl()
{
    return IsSystemBasic() || IsSystemCore();
}

bool UsbRightManager::IsSystemApp()
{
    uint64_t tokenid = IPCSkeleton::GetCallingFullTokenID();
    bool isSystemApp = TokenIdKit::IsSystemAppByFullTokenID(tokenid);
    if (!isSystemApp) {
        USB_HILOGW(MODULE_USB_SERVICE, "not is sysapp, return false");
        return false;
    }
    return true;
}

bool UsbRightManager::IsSystemHap()
{
    if (IsSystemApl() || IsSystemApp()) {
        return true;
    }
    USB_HILOGW(MODULE_USB_SERVICE, "not system apl or system app, return false");
    return false;
}
} // namespace USB
} // namespace OHOS
