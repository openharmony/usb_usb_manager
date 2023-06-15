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

#include "usb_right_database.h"

#include "hilog_wrapper.h"
#include "usb_errors.h"

namespace OHOS {
namespace USB {

std::shared_ptr<UsbRightDataBase> UsbRightDataBase::instance_ = nullptr;

UsbRightDataBase::UsbRightDataBase()
{
    std::string rightDatabaseName = USB_RIGHT_DB_PATH + USB_RIGHT_DB_NAME;
    int32_t errCode = OHOS::NativeRdb::E_OK;
    OHOS::NativeRdb::RdbStoreConfig config(rightDatabaseName);
    config.SetSecurityLevel(NativeRdb::SecurityLevel::S1);
    UsbRightDataBaseCallBack sqliteOpenHelperCallback;
    store_ = OHOS::NativeRdb::RdbHelper::GetRdbStore(config, DATABASE_OPEN_VERSION, sqliteOpenHelperCallback, errCode);
    if (errCode != OHOS::NativeRdb::E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetRdbStore errCode :%{public}d", errCode);
    } else {
        USB_HILOGI(MODULE_USB_SERVICE, "GetRdbStore success :%{public}d", errCode);
    }
}

std::shared_ptr<UsbRightDataBase> UsbRightDataBase::GetInstance()
{
    if (instance_ == nullptr) {
        USB_HILOGI(MODULE_USB_SERVICE, "reset to new instance");
        instance_.reset(new UsbRightDataBase());
        return instance_;
    }
    return instance_;
}

int32_t UsbRightDataBase::BeginTransaction()
{
    if (store_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction store_ is nullptr");
        return USB_RIGHT_RDB_NO_INIT;
    }
    int32_t ret = store_->BeginTransaction();
    if (ret != OHOS::NativeRdb::E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction fail :%{public}d", ret);
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    return USB_RIGHT_OK;
}

int32_t UsbRightDataBase::Commit()
{
    if (store_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit store_ is nullptr");
        return USB_RIGHT_RDB_NO_INIT;
    }
    int32_t ret = store_->Commit();
    if (ret != OHOS::NativeRdb::E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit fail :%{public}d", ret);
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    return USB_RIGHT_OK;
}

int32_t UsbRightDataBase::RollBack()
{
    if (store_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "RollBack store_ is nullptr");
        return USB_RIGHT_RDB_NO_INIT;
    }
    int32_t ret = store_->RollBack();
    if (ret != OHOS::NativeRdb::E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "RollBack fail :%{public}d", ret);
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    return USB_RIGHT_OK;
}

int64_t UsbRightDataBase::Insert(const OHOS::NativeRdb::ValuesBucket &insertValues)
{
    if (store_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Insert store_ is  nullptr");
        return USB_RIGHT_RDB_NO_INIT;
    }
    int64_t outRowId = 0;
    int32_t ret = store_->Insert(outRowId, USB_RIGHT_TABLE_NAME, insertValues);
    USB_HILOGI(MODULE_USB_SERVICE, "Insert id=%{public}" PRIu64 "", outRowId);
    if (ret != OHOS::NativeRdb::E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Insert ret :%{public}d", ret);
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    return outRowId;
}

int32_t UsbRightDataBase::Update(
    int32_t &changedRows, const OHOS::NativeRdb::ValuesBucket &values, const OHOS::NativeRdb::RdbPredicates &predicates)
{
    if (store_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Update(RdbPredicates) store_ is nullptr");
        return USB_RIGHT_RDB_NO_INIT;
    }
    int32_t ret = store_->Update(changedRows, values, predicates);
    if (ret != OHOS::NativeRdb::E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Update(RdbPredicates) ret :%{public}d", ret);
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    return USB_RIGHT_OK;
}

int32_t UsbRightDataBase::Update(int32_t &changedRows, const OHOS::NativeRdb::ValuesBucket &values,
    const std::string &whereClause, const std::vector<std::string> &whereArgs)
{
    if (store_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Update(whereClause) store_ is nullptr");
        return USB_RIGHT_RDB_NO_INIT;
    }
    int32_t ret = store_->Update(changedRows, USB_RIGHT_TABLE_NAME, values, whereClause, whereArgs);
    if (ret != OHOS::NativeRdb::E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Update(whereClause) ret :%{public}d", ret);
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    return USB_RIGHT_OK;
}

int32_t UsbRightDataBase::Delete(const OHOS::NativeRdb::RdbPredicates &predicates)
{
    if (store_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Delete(RdbPredicates) store_ is  nullptr");
        return USB_RIGHT_RDB_NO_INIT;
    }
    int32_t deleteRow = 0;
    int32_t ret = store_->Delete(deleteRow, predicates);
    if (ret != OHOS::NativeRdb::E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Delete(RdbPredicates) ret :%{public}d", ret);
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    return USB_RIGHT_OK;
}

int32_t UsbRightDataBase::Delete(
    int32_t &changedRows, const std::string &whereClause, const std::vector<std::string> &whereArgs)
{
    if (store_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Delete store_ is nullptr");
        return USB_RIGHT_RDB_NO_INIT;
    }
    int32_t ret = store_->Delete(changedRows, USB_RIGHT_TABLE_NAME, whereClause, whereArgs);
    if (ret != OHOS::NativeRdb::E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Delete(whereClause) ret :%{public}d", ret);
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    return USB_RIGHT_OK;
}

int32_t UsbRightDataBase::ExecuteSql(const std::string &sql, const std::vector<OHOS::NativeRdb::ValueObject> &bindArgs)
{
    if (store_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "ExecuteSql store_ is nullptr");
        return USB_RIGHT_RDB_NO_INIT;
    }
    int32_t ret = store_->ExecuteSql(sql, bindArgs);
    if (ret != OHOS::NativeRdb::E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "ExecuteSql ret :%{public}d", ret);
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    return USB_RIGHT_OK;
}

std::shared_ptr<OHOS::NativeRdb::ResultSet> UsbRightDataBase::QuerySql(
    const std::string &sql, const std::vector<std::string> &selectionArgs)
{
    if (store_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "QuerySql(sql) store_ is nullptr");
        return nullptr;
    }
    return store_->QuerySql(sql);
}

std::shared_ptr<OHOS::NativeRdb::ResultSet> UsbRightDataBase::Query(
    const OHOS::NativeRdb::AbsRdbPredicates &predicates, const std::vector<std::string> &columns)
{
    if (store_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Query(AbsRdbPredicates) store_ is nullptr");
        return nullptr;
    }
    return store_->Query(predicates, columns);
}

int32_t UsbRightDataBaseCallBack::OnCreate(OHOS::NativeRdb::RdbStore &store)
{
    std::string sql = CREATE_USB_RIGHT_TABLE;
    int32_t ret = store.ExecuteSql(sql);
    if (ret != OHOS::NativeRdb::E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "OnCreate failed: %{public}d", ret);
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "DB OnCreate Done: %{public}d", ret);
    return USB_RIGHT_OK;
}

int32_t UsbRightDataBaseCallBack::OnUpgrade(OHOS::NativeRdb::RdbStore &store, int32_t oldVersion, int32_t newVersion)
{
    USB_HILOGI(MODULE_USB_SERVICE, "DB OnUpgrade Enter");
    (void)store;
    (void)oldVersion;
    (void)newVersion;
    return USB_RIGHT_OK;
}

int32_t UsbRightDataBaseCallBack::OnDowngrade(OHOS::NativeRdb::RdbStore &store, int32_t oldVersion, int32_t newVersion)
{
    USB_HILOGI(MODULE_USB_SERVICE, "DB OnDowngrade Enter");
    (void)store;
    (void)oldVersion;
    (void)newVersion;
    return USB_RIGHT_OK;
}

} // namespace USB
} // namespace OHOS
