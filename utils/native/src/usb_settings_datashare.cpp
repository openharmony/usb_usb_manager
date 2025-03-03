/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "usb_settings_datashare.h"

#include "hilog_wrapper.h"
#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "iservice_registry.h"
#include "uri.h"
#include "singleton.h"

namespace OHOS {
namespace USB {
namespace {
constexpr const char *SETTINGS_DATASHARE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
const std::string SETTINGS_DATASHARE_EXT_URI = "datashare:///com.ohos.settingsdata.DataAbility";
constexpr const char *SETTINGS_DATA_COLUMN_KEYWORD = "KEYWORD";
constexpr const char *SETTINGS_DATA_COLUMN_VALUE = "VALUE";
}

UsbSettingDataShare::UsbSettingDataShare()
{
    datashareHelper_ = CreateDataShareHelper(USB_SYSTEM_ABILITY_ID);
}

UsbSettingDataShare::~UsbSettingDataShare()
{
    if (datashareHelper_ != nullptr) {
        USB_HILOGI(MODULE_USB_SERVICE, "datashareHelper_ is here Release!");
        datashareHelper_->Release();
    }
}

std::shared_ptr<DataShare::DataShareHelper> UsbSettingDataShare::CreateDataShareHelper(int systemAbilityId)
{
    USB_HILOGI(MODULE_USB_SERVICE, "CreateDataShareHelper start!");
    sptr<ISystemAbilityManager> saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetSystemAbilityManager failed!");
        return nullptr;
    }
    sptr<IRemoteObject> remote = saManager->GetSystemAbility(systemAbilityId);
    if (remote == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetSystemAbility Service Failed!");
        return nullptr;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "systemAbilityId = %{public}d", systemAbilityId);
    return DataShare::DataShareHelper::Creator(remote, SETTINGS_DATASHARE_URI, SETTINGS_DATASHARE_EXT_URI);
}

bool UsbSettingDataShare::Query(Uri& uri, const std::string& key, std::string& value)
{
    USB_HILOGI(MODULE_USB_SERVICE, "start Query key = %{public}s", key.c_str());
    if (datashareHelper_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "query error, datashareHelper_ is nullptr");
        return false;
    }

    std::vector<std::string> columns;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTINGS_DATA_COLUMN_KEYWORD, key);
    auto result = datashareHelper_->Query(uri, predicates, columns);
    if (result == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "query error, result is nullptr");
        return false;
    }

    int rowCount = 0;
    result->GetRowCount(rowCount);
    if (rowCount == 0) {
        USB_HILOGE(MODULE_USB_SERVICE, "query success, but rowCount is 0");
        return true;
    }

    if (result->GoToFirstRow() != DataShare::E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "query error, go to first row error");
        result->Close();
        return false;
    }

    int columnIndex = 0;
    result->GetColumnIndex(SETTINGS_DATA_COLUMN_VALUE, columnIndex);
    result->GetString(columnIndex, value);
    result->Close();
    USB_HILOGI(MODULE_USB_SERVICE, "SettingUtils: query success");
    return true;
}
} // namespace USB
} // namespace OHOS
 