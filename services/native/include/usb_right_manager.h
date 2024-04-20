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

#ifndef USB_RIGHT_MANAGER_H
#define USB_RIGHT_MANAGER_H

#include <algorithm>
#include <map>
#include <mutex>
#include <semaphore.h>
#include <string>
#include <vector>

#include "ability_connect_callback_stub.h"
#include "bundle_mgr_interface.h"
#include "usb_common.h"
#include "parameter.h"

namespace OHOS {
namespace USB {

class UsbRightManager {
public:
    int32_t Init();
    /* deviceName is in VID-PID format */
    bool HasRight(const std::string &deviceName, const std::string &bundleName,
        const std::string &tokenId, const int32_t &userId);
    /* busDev is in busNum-devAddr format */
    int32_t RequestRight(const std::string &busDev, const std::string &deviceName, const std::string &bundleName,
        const std::string &tokenId, const int32_t &userId);
    bool AddDeviceRight(const std::string &deviceName, const std::string &tokenIdStr);
    bool AddDeviceRight(const std::string &deviceName, const std::string &bundleName,
        const std::string &tokenId, const int32_t &userId);
    bool RemoveDeviceRight(const std::string &deviceName, const std::string &bundleName,
        const std::string &tokenId, const int32_t &userId);
    bool RemoveDeviceAllRight(const std::string &deviceName);
    bool CheckPermission();
    bool IsSystemApp();
    int32_t CleanUpRightExpired(std::vector<std::string> &devices);
    static int32_t CleanUpRightUserDeleted(int32_t &totalUsers, int32_t &deleteUsers);
    static int32_t IsOsAccountExists(int32_t id, bool &isAccountExists);
    static int32_t CleanUpRightAppUninstalled(int32_t uid, const std::string &bundleName);
    int32_t HasSetFuncRight(int32_t functions);

private:
    bool GetUserAgreementByDiag(const std::string &busDev, const std::string &deviceName, const std::string &bundleName,
        const std::string &tokenId, const int32_t &userId);
    bool ShowUsbDialog(const std::string &busDev, const std::string &deviceName,
        const std::string &bundleName, const std::string &tokenId);
    bool CheckSaPermission();
    bool IsAllDigits(const std::string &bundleName);
    sptr<AppExecFwk::IBundleMgr> GetBundleMgr();

    static sem_t waitDialogDisappear_;
    class UsbAbilityConn : public AAFwk::AbilityConnectionStub {
        void OnAbilityConnectDone(const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject,
            int32_t resultCode) override
        {
            USB_HILOGI(MODULE_USB_SERVICE, "connect done");
        }
        void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode) override
        {
            USB_HILOGI(MODULE_USB_SERVICE, "disconnect done");
            sem_post(&waitDialogDisappear_);
        }
    };

    std::mutex dialogRunning_;

    bool IsAppInstalled(int32_t uid, const std::string &bundleName);
    void GetCurrentUserId(int32_t &uid);
    bool GetBundleInstallAndUpdateTime(
        int32_t uid, const std::string &bundleName, uint64_t &installTime, uint64_t &updateTime);
    uint64_t GetCurrentTimestamp();
    void StringVectorSortAndUniq(std::vector<std::string> &strings);
    static bool StringVectorFound(const std::vector<std::string> &strings, const std::string &value, int32_t &index);

    int32_t CleanUpRightAppUninstalled(int32_t uid, int32_t &totalApps, int32_t &deleteApps);
    int32_t CleanUpRightTemporaryExpired(const std::string &deviceName);
    int32_t CleanUpRightNormalExpired(int32_t uid);
    int32_t CleanUpRightAppReinstalled(int32_t uid, uint32_t &totalApps, uint32_t &deleteApps);
    int32_t TidyUpRight(uint32_t choose);
};

} // namespace USB
} // namespace OHOS

#endif
