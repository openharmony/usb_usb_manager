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

#include "usb_right_db_helper.h"

#include <string>

#include "bundle_installer_interface.h"
#include "hilog_wrapper.h"
#include "usb_errors.h"
#include "usb_right_database.h"

using namespace OHOS::NativeRdb;

namespace OHOS {
namespace USB {
std::shared_ptr<UsbRightDbHelper> UsbRightDbHelper::instance_;

UsbRightDbHelper::UsbRightDbHelper()
{
    rightDatabase_ = UsbRightDataBase::GetInstance();
}

std::shared_ptr<UsbRightDbHelper> UsbRightDbHelper::GetInstance()
{
    static std::mutex instanceMutex;
    std::lock_guard<std::mutex> guard(instanceMutex);
    if (instance_ == nullptr) {
        instance_.reset(new UsbRightDbHelper());
    }
    return instance_;
}

bool UsbRightDbHelper::IsRecordExpired(int32_t uid, const std::string &deviceName, const std::string &bundleName,
    const std::string &tokenId, uint64_t expiredTime)
{
    USB_HILOGI(MODULE_USB_SERVICE, "info: uid=%{public}d dev=%{public}s app=%{public}s",
        uid, deviceName.c_str(), bundleName.c_str());
    std::vector<struct UsbRightAppInfo> infos;
    int32_t ret = QueryRightRecord(uid, deviceName, bundleName, tokenId, infos);
    if (ret <= 0) {
        USB_HILOGI(MODULE_USB_SERVICE, "usb query no record/error: %{public}d", ret);
        return true;
    }
    size_t len = infos.size();
    for (size_t i = 0; i < len; i++) {
        if (!IsRecordExpired(infos.at(i), expiredTime)) {
            return false;
        }
    }
    return true;
}

bool UsbRightDbHelper::IsRecordExpired(const struct UsbRightAppInfo &info, uint64_t expiredTime)
{
    if (info.validPeriod == USB_RIGHT_VALID_PERIOD_MIN) {
        USB_HILOGD(MODULE_USB_SERVICE, "allow temporary");
        return false;
    } else if (info.validPeriod == USB_RIGHT_VALID_PERIOD_MAX) {
        USB_HILOGD(MODULE_USB_SERVICE, "allow forever");
        return false;
    } else if (info.requestTime + info.validPeriod > expiredTime) {
        USB_HILOGD(MODULE_USB_SERVICE, "allow based on request time");
        return false;
    } else if ((info.installTime > info.updateTime) || (info.installTime > info.requestTime)) {
        USB_HILOGW(MODULE_USB_SERVICE,
            "invalid: inst=%{public}" PRIu64 " updt=%{public}" PRIu64 " rqst=%{public}" PRIu64 "", info.installTime,
            info.updateTime, info.requestTime);
        /* ignore, return true to add right */
        return true;
    }
    /* unknown, return true to add right */
    USB_HILOGW(MODULE_USB_SERVICE,
        "info: inst=%{public}" PRIu64 " updt=%{public}" PRIu64 " rqst=%{public}" PRIu64 " expr=%{public}" PRIu64
        " valid=%{public}" PRIu64 "",
        info.installTime, info.updateTime, info.requestTime, expiredTime, info.validPeriod);
    return true;
}

int32_t UsbRightDbHelper::AddRightRecord(
    const std::string &deviceName, const std::string &bundleName, struct UsbRightAppInfo &info)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    ValuesBucket values;
    values.Clear();
    values.PutInt("uid", info.uid);
    values.PutLong("installTime", info.installTime);
    values.PutLong("updateTime", info.updateTime);
    values.PutLong("requestTime", info.requestTime);
    values.PutLong("validPeriod", info.validPeriod);
    values.PutString("deviceName", deviceName);
    values.PutString("bundleName", bundleName);
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error: %{public}d", ret);
        return ret;
    }
    ret = rightDatabase_->Insert(values);
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Insert error: %{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error: %{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    USB_HILOGI(MODULE_USB_SERVICE,
        "add success: uid=%{public}d inst=%{public}" PRIu64 " updt=%{public}" PRIu64 " rqst=%{public}" PRIu64
        " vald=%{public}" PRIu64 " dev=%{public}s app=%{public}s",
        info.uid, info.installTime, info.updateTime, info.requestTime, info.validPeriod, deviceName.c_str(),
        bundleName.c_str());
    return ret;
}

int32_t UsbRightDbHelper::QueryAndGetResult(const RdbPredicates &rdbPredicates, const std::vector<std::string> &columns,
    std::vector<struct UsbRightAppInfo> &infos)
{
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error: %{public}d", ret);
        return ret;
    }
    auto resultSet = rightDatabase_->Query(rdbPredicates, columns);
    if (resultSet == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Query error");
        (void)rightDatabase_->RollBack();
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error: %{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    return GetResultRightRecordEx(resultSet, infos);
}

int32_t UsbRightDbHelper::QueryRightRecord(int32_t uid, const std::string &deviceName, const std::string &bundleName,
    const std::string &tokenId, std::vector<struct UsbRightAppInfo> &infos)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    USB_HILOGI(MODULE_USB_SERVICE, "Query detail: uid=%{public}d dev=%{public}s app=%{public}s", uid,
        deviceName.c_str(), bundleName.c_str());
    std::vector<std::string> columns;
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    rdbPredicates.BeginWrap()
        ->EqualTo("uid", std::to_string(uid))
        ->And()
        ->EqualTo("deviceName", deviceName)
        ->And()
        ->EqualTo("bundleName", bundleName)
        ->And()
        ->EqualTo("tokenId", tokenId)
        ->EndWrap();
    return QueryAndGetResult(rdbPredicates, columns, infos);
}

int32_t UsbRightDbHelper::QueryUserRightRecord(int32_t uid, std::vector<struct UsbRightAppInfo> &infos)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    USB_HILOGD(MODULE_USB_SERVICE, "Query detail: uid=%{public}d", uid);
    std::vector<std::string> columns;
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    rdbPredicates.EqualTo("uid", std::to_string(uid));
    return QueryAndGetResult(rdbPredicates, columns, infos);
}

int32_t UsbRightDbHelper::QueryDeviceRightRecord(
    int32_t uid, const std::string &deviceName, std::vector<struct UsbRightAppInfo> &infos)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    USB_HILOGD(MODULE_USB_SERVICE, "Query detail: uid=%{public}d dev=%{public}s", uid, deviceName.c_str());
    std::vector<std::string> columns;
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    rdbPredicates.BeginWrap()->EqualTo("uid", std::to_string(uid))->And()->EqualTo("deviceName", deviceName)->EndWrap();
    return QueryAndGetResult(rdbPredicates, columns, infos);
}

int32_t UsbRightDbHelper::QueryAppRightRecord(
    int32_t uid, const std::string &bundleName, std::vector<struct UsbRightAppInfo> &infos)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    USB_HILOGD(MODULE_USB_SERVICE, "Query detail: uid=%{public}d dev=%{public}s", uid, bundleName.c_str());
    std::vector<std::string> columns;
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    rdbPredicates.BeginWrap()->EqualTo("uid", std::to_string(uid))->And()->EqualTo("bundleName", bundleName)->EndWrap();
    return QueryAndGetResult(rdbPredicates, columns, infos);
}

int32_t UsbRightDbHelper::QueryRightRecordUids(std::vector<std::string> &uids)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    std::vector<std::string> columns = {"uid"};
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    rdbPredicates.Distinct();
    return QueryAndGetResultColumnValues(rdbPredicates, columns, "uid", uids);
}

int32_t UsbRightDbHelper::QueryRightRecordApps(int32_t uid, std::vector<std::string> &apps)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    std::vector<std::string> columns = {"bundleName"};
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    rdbPredicates.EqualTo("uid", std::to_string(uid))->Distinct();
    return QueryAndGetResultColumnValues(rdbPredicates, columns, "bundleName", apps);
}

int32_t UsbRightDbHelper::QueryAndGetResultColumnValues(const RdbPredicates &rdbPredicates,
    const std::vector<std::string> &columns, const std::string &columnName, std::vector<std::string> &columnValues)
{
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error: %{public}d", ret);
        return ret;
    }
    auto resultSet = rightDatabase_->Query(rdbPredicates, columns);
    if (resultSet == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Query error");
        (void)rightDatabase_->RollBack();
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error: %{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    int32_t rowCount = 0;
    int32_t columnIndex = 0;
    if (resultSet->GetRowCount(rowCount) != E_OK || resultSet->GetColumnIndex(columnName, columnIndex) != E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "get table info failed");
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    bool endFlag = false;
    for (int32_t i = 0; (i < rowCount) && !endFlag; i++) {
        if (resultSet->GoToRow(i) != E_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "GoToRow %{public}d", i);
            return USB_RIGHT_RDB_EXECUTE_FAILTURE;
        }
        std::string tempStr;
        if (resultSet->GetString(columnIndex, tempStr) == E_OK) {
            columnValues.push_back(tempStr);
        }
        resultSet->IsEnded(endFlag);
    }
    int32_t position = 0;
    resultSet->GetRowIndex(position);
    resultSet->IsEnded(endFlag);
    USB_HILOGD(MODULE_USB_SERVICE, "idx=%{public}d rows=%{public}d pos=%{public}d ret=%{public}zu end=%{public}s",
        columnIndex, rowCount, position, columnValues.size(), (endFlag ? "yes" : "no"));
    return columnValues.size();
}

int32_t UsbRightDbHelper::UpdateRightRecord(
    int32_t uid, const std::string &deviceName, const std::string &bundleName, struct UsbRightAppInfo &info)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    ValuesBucket values;
    values.Clear();
    values.PutInt("uid", info.uid);
    values.PutLong("installTime", info.installTime);
    values.PutLong("updateTime", info.updateTime);
    values.PutLong("requestTime", info.requestTime);
    values.PutLong("validPeriod", info.validPeriod);
    values.PutString("deviceName", deviceName);
    values.PutString("bundleName", bundleName);
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error: %{public}d", ret);
        return ret;
    }
    int32_t changedRows = 0;
    ret = rightDatabase_->Update(changedRows, values, "uid = ? AND deviceName = ? AND bundleName = ?",
        std::vector<std::string> {std::to_string(info.uid), deviceName, bundleName});
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Update error: %{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error: %{public}d", ret);
        (void)rightDatabase_->RollBack();
    }
    return ret;
}

int32_t UsbRightDbHelper::DeleteAndNoOtherOperation(
    const std::string &whereClause, const std::vector<std::string> &whereArgs)
{
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error: %{public}d", ret);
        return ret;
    }
    int32_t changedRows = 0;
    ret = rightDatabase_->Delete(changedRows, whereClause, whereArgs);
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Delete error: %{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error: %{public}d", ret);
        (void)rightDatabase_->RollBack();
    }
    
    if (changedRows <= 0) {
        USB_HILOGI(MODULE_USB_SERVICE, "no row change: %{public}d", changedRows);
        return USB_RIGHT_RDB_EMPTY;
    }
    return ret;
}

int32_t UsbRightDbHelper::DeleteAndNoOtherOperation(const OHOS::NativeRdb::RdbPredicates &rdbPredicates)
{
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error: %{public}d", ret);
        return ret;
    }
    ret = rightDatabase_->Delete(rdbPredicates);
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Delete error: %{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error: %{public}d", ret);
        (void)rightDatabase_->RollBack();
    }
    return ret;
}

int32_t UsbRightDbHelper::DeleteRightRecord(int32_t uid, const std::string &deviceName,
    const std::string &bundleName, const std::string &tokenId)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    std::string whereClause = {"uid = ? AND deviceName = ? AND bundleName = ? AND tokenId = ?"};
    std::vector<std::string> whereArgs = {std::to_string(uid), deviceName, bundleName, tokenId};
    int32_t ret = DeleteAndNoOtherOperation(whereClause, whereArgs);
    if (ret != USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "failed: detale(uid, dev, app): %{public}d", ret);
    }
    return ret;
}

int32_t UsbRightDbHelper::DeleteDeviceRightRecord(int32_t uid, const std::string &deviceName)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    std::string whereClause = {"uid = ? AND deviceName = ?"};
    std::vector<std::string> whereArgs = {std::to_string(uid), deviceName};
    int32_t ret = DeleteAndNoOtherOperation(whereClause, whereArgs);
    if (ret != USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "failed: delete(uid, dev): %{public}d", ret);
    }
    return ret;
}

int32_t UsbRightDbHelper::DeleteAppRightRecord(int32_t uid, const std::string &bundleName)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    std::string whereClause = {"uid = ? AND bundleName = ?"};
    std::vector<std::string> whereArgs = {std::to_string(uid), bundleName};
    int32_t ret = DeleteAndNoOtherOperation(whereClause, whereArgs);
    if (ret != USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "failed: delete(uid, app): %{public}d", ret);
    }
    return ret;
}

int32_t UsbRightDbHelper::DeleteAppsRightRecord(int32_t uid, const std::vector<std::string> &bundleNames)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    rdbPredicates.BeginWrap()->EqualTo("uid", std::to_string(uid))->And()->In("bundleName", bundleNames)->EndWrap();
    int32_t ret = DeleteAndNoOtherOperation(rdbPredicates);
    if (ret != USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "failed: delete(uid, devs): %{public}d", ret);
    }
    return ret;
}

int32_t UsbRightDbHelper::DeleteUidRightRecord(int32_t uid)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    std::string whereClause = {"uid = ?"};
    std::vector<std::string> whereArgs = {std::to_string(uid)};
    int32_t ret = DeleteAndNoOtherOperation(whereClause, whereArgs);
    if (ret != USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "failed: delete(uid): %{public}d", ret);
    }
    return ret;
}

int32_t UsbRightDbHelper::DeleteNormalExpiredRightRecord(int32_t uid, uint64_t expiredTime)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    std::string whereClause = {"uid = ? AND requestTime < ? AND validPeriod NOT IN (?, ?)"};

    uint64_t relativeExpiredTime = (expiredTime <= USB_RIGHT_VALID_PERIOD_SET) ? 0 :
        (expiredTime - USB_RIGHT_VALID_PERIOD_SET);
    std::vector<std::string> whereArgs = {std::to_string(uid), std::to_string(relativeExpiredTime),
        std::to_string(USB_RIGHT_VALID_PERIOD_MIN), std::to_string(USB_RIGHT_VALID_PERIOD_MAX)};
    int32_t ret = DeleteAndNoOtherOperation(whereClause, whereArgs);
    if (ret != USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE,
        "failed: delete(uid=%{public}d, expr<%{public}" PRIu64 "): %{public}d", uid, expiredTime, ret);
    }
    return ret;
}

int32_t UsbRightDbHelper::DeleteValidPeriodRightRecord(long validPeriod, const std::string &deviceName)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    std::string whereClause = {"validPeriod = ? AND deviceName = ?"};
    std::vector<std::string> whereArgs = {std::to_string(validPeriod), deviceName};
    int32_t ret = DeleteAndNoOtherOperation(whereClause, whereArgs);
    if (ret != USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "failed: delete(dev, valid): %{public}d", ret);
    }
    return ret;
}

int32_t UsbRightDbHelper::GetResultSetTableInfo(
    const std::shared_ptr<OHOS::NativeRdb::ResultSet> &resultSet, struct UsbRightTableInfo &table)
{
    int32_t rowCount = 0;
    int32_t columnCount = 0;
    std::vector<std::string> columnNames;
    if (resultSet->GetRowCount(rowCount) != E_OK || resultSet->GetColumnCount(columnCount) != E_OK ||
        resultSet->GetAllColumnNames(columnNames) != E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "get table info failed");
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    int32_t columnNamesCount = static_cast<int32_t>(columnNames.size());
    for (int32_t i = 0; i < columnNamesCount; i++) {
        std::string &columnName = columnNames.at(i);
        if (columnName == "id") {
            table.primaryKeyIndex = i;
        }
        if (columnName == "uid") {
            table.uidIndex = i;
        }
        if (columnName == "installTime") {
            table.installTimeIndex = i;
        }
        if (columnName == "updateTime") {
            table.updateTimeIndex = i;
        }
        if (columnName == "requestTime") {
            table.requestTimeIndex = i;
        }
        if (columnName == "validPeriod") {
            table.validPeriodIndex = i;
        }
        if (columnName == "bundleName") {
            table.bundleNameIndex = i;
        }
        if (columnName == "deviceName") {
            table.deviceNameIndex = i;
        }
    }
    table.rowCount = rowCount;
    table.columnCount = columnCount;
    USB_HILOGD(MODULE_USB_SERVICE,
        "info[%{public}d/%{public}d]: "
        "%{public}d/%{public}d/%{public}d/%{public}d/%{public}d/%{public}d/%{public}d/%{public}d",
        rowCount, columnCount, table.primaryKeyIndex, table.uidIndex, table.installTimeIndex, table.updateTimeIndex,
        table.requestTimeIndex, table.validPeriodIndex, table.deviceNameIndex, table.bundleNameIndex);
    return USB_RIGHT_OK;
}

int32_t UsbRightDbHelper::GetResultRightRecordEx(
    const std::shared_ptr<OHOS::NativeRdb::ResultSet> &resultSet, std::vector<struct UsbRightAppInfo> &infos)
{
    struct UsbRightTableInfo table;
    int32_t ret = GetResultSetTableInfo(resultSet, table);
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetResultSetTableInfo failed");
        return ret;
    }
    bool endFlag = false;
    int32_t primaryKeyId = 0;
    int64_t installTime = 0;
    int64_t updateTime = 0;
    int64_t requestTime = 0;
    int64_t validPeriod = 0;
    for (int32_t i = 0; (i < table.rowCount) && !endFlag; i++) {
        if (resultSet->GoToRow(i) != E_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "GoToRow %{public}d", i);
            break;
        }
        struct UsbRightAppInfo info;
        if (resultSet->GetInt(table.primaryKeyIndex, primaryKeyId) == E_OK &&
            resultSet->GetInt(table.uidIndex, info.uid) == E_OK &&
            resultSet->GetLong(table.installTimeIndex, installTime) == E_OK &&
            resultSet->GetLong(table.updateTimeIndex, updateTime) == E_OK &&
            resultSet->GetLong(table.requestTimeIndex, requestTime) == E_OK &&
            resultSet->GetLong(table.validPeriodIndex, validPeriod) == E_OK) {
            info.primaryKeyId = static_cast<uint32_t>(primaryKeyId);
            info.installTime = static_cast<uint64_t>(installTime);
            info.updateTime = static_cast<uint64_t>(updateTime);
            info.requestTime = static_cast<uint64_t>(requestTime);
            info.validPeriod = static_cast<uint64_t>(validPeriod);
            infos.push_back(info);
        }
        resultSet->IsEnded(endFlag);
    }
    int32_t position = 0;
    resultSet->GetRowIndex(position);
    resultSet->IsEnded(endFlag);
    USB_HILOGD(MODULE_USB_SERVICE, "row=%{public}d col=%{public}d pos=%{public}d ret=%{public}zu end=%{public}s",
        table.rowCount, table.columnCount, position, infos.size(), (endFlag ? "yes" : "no"));
    return infos.size();
}

int32_t UsbRightDbHelper::AddOrUpdateRightRecord(int32_t uid, const std::string &deviceName,
    const std::string &bundleName, const std::string &tokenId, struct UsbRightAppInfo &info)
{
    std::lock_guard<std::mutex> guard(databaseMutex_);
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error: %{public}d", ret);
        return ret;
    }
    bool isUpdate = false;
    ret = CheckIfNeedUpdateEx(isUpdate, uid, deviceName, bundleName, tokenId);
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "check if need update error: %{public}d", ret);
        return ret;
    }
    ret = AddOrUpdateRightRecordEx(isUpdate, uid, deviceName, bundleName, tokenId, info);
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "add or update error: %{public}d", ret);
        return ret;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error: %{public}d", ret);
        (void)rightDatabase_->RollBack();
    }
    return ret;
}

int32_t UsbRightDbHelper::CheckIfNeedUpdateEx(bool &isUpdate, int32_t uid, const std::string &deviceName,
    const std::string &bundleName, const std::string &tokenId)
{
    std::vector<std::string> columns;
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    rdbPredicates.BeginWrap()
        ->EqualTo("uid", std::to_string(uid))
        ->And()
        ->EqualTo("deviceName", deviceName)
        ->And()
        ->EqualTo("bundleName", bundleName)
        ->And()
        ->EqualTo("tokenId", tokenId)
        ->EndWrap();
    auto resultSet = rightDatabase_->Query(rdbPredicates, columns);
    if (resultSet == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Query error");
        (void)rightDatabase_->RollBack();
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    int32_t rowCount = 0;
    if (resultSet->GetRowCount(rowCount) != E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetRowCount error");
        (void)rightDatabase_->RollBack();
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    isUpdate = (rowCount > 0 ? true : false);
    return USB_RIGHT_OK;
}

int32_t UsbRightDbHelper::AddOrUpdateRightRecordEx(bool isUpdate, int32_t uid, const std::string &deviceName,
    const std::string &bundleName, const std::string &tokenId, struct UsbRightAppInfo &info)
{
    int32_t ret = 0;
    ValuesBucket values;
    values.Clear();
    values.PutInt("uid", info.uid);
    values.PutLong("installTime", info.installTime);
    values.PutLong("updateTime", info.updateTime);
    values.PutLong("requestTime", info.requestTime);
    values.PutLong("validPeriod", info.validPeriod);
    values.PutString("deviceName", deviceName);
    values.PutString("bundleName", bundleName);
    values.PutString("tokenId", tokenId);

    if (isUpdate) {
        int32_t changedRows = 0;
        ret = rightDatabase_->Update(changedRows, values,
            "uid = ? AND deviceName = ? AND bundleName = ? AND tokenId = ?",
            std::vector<std::string> {std::to_string(info.uid), deviceName, bundleName, tokenId});
    } else {
        ret = rightDatabase_->Insert(values);
    }
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Insert or Update error: %{public}d", ret);
        (void)rightDatabase_->RollBack();
    }
    return ret;
}

int32_t UsbRightDbHelper::QueryRightRecordCount()
{
    int32_t ret = rightDatabase_->BeginTransaction();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BeginTransaction error :%{public}d", ret);
        return ret;
    }
    std::vector<std::string> columns;
    RdbPredicates rdbPredicates(USB_RIGHT_TABLE_NAME);
    auto resultSet = rightDatabase_->Query(rdbPredicates, columns);
    if (resultSet == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Query error");
        (void)rightDatabase_->RollBack();
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    ret = rightDatabase_->Commit();
    if (ret < USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "Commit error: %{public}d", ret);
        (void)rightDatabase_->RollBack();
        return ret;
    }
    int32_t rowCount = 0;
    if (resultSet->GetRowCount(rowCount) != E_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetRowCount error");
        return USB_RIGHT_RDB_EXECUTE_FAILTURE;
    }
    return rowCount;
}

} // namespace USB
} // namespace OHOS
