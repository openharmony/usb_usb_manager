/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef USB_RIGHT_DB_HELPER_H
#define USB_RIGHT_DB_HELPER_H

#include "data_ability_predicates.h"
#include "rdb_errno.h"
#include "rdb_helper.h"
#include "rdb_open_callback.h"
#include "rdb_predicates.h"
#include "rdb_store.h"
#include "result_set.h"
#include "usb_right_database.h"
#include "value_object.h"

namespace OHOS {
namespace USB {

/* unit: second */
constexpr int64_t USB_RIGHT_VALID_PERIOD_MIN = 0;
constexpr int64_t USB_RIGHT_VALID_PERIOD_MAX = 0xFFFFFFFFL;
constexpr uint64_t USB_RIGHT_VALID_PERIOD_SET = 300;

struct UsbRightAppInfo {
    uint32_t primaryKeyId; /* table primary key */
    int32_t uid;           /* app install user id */
    uint64_t installTime;  /* app install time */
    uint64_t updateTime;   /* app update time */
    uint64_t requestTime;  /* app request permission time */
    uint64_t validPeriod;  /* app permission valid period */
};

struct UsbRightTableInfo {
    int32_t rowCount;
    int32_t columnCount;
    int32_t primaryKeyIndex;
    int32_t uidIndex;
    int32_t installTimeIndex;
    int32_t updateTimeIndex;
    int32_t requestTimeIndex;
    int32_t validPeriodIndex;
    int32_t deviceNameIndex;
    int32_t bundleNameIndex;
};

class UsbRightDbHelper {
public:
    static std::shared_ptr<UsbRightDbHelper> GetInstance();

    /* query if permission record is expired by query database */
    bool IsRecordExpired(int32_t uid, const std::string &deviceName, const std::string &bundleName,
        const std::string &tokenId, uint64_t expiredTime);
    /* query if permission record is expired by time info */
    bool IsRecordExpired(const struct UsbRightAppInfo &info, uint64_t expiredTime);
    /* add (user, device, app) record */
    int32_t AddRightRecord(const std::string &deviceName, const std::string &bundleName, struct UsbRightAppInfo &info);
    /* query (user) record */
    int32_t QueryUserRightRecord(int32_t uid, std::vector<struct UsbRightAppInfo> &infos);
    /* query (user, device) record */
    int32_t QueryDeviceRightRecord(
        int32_t uid, const std::string &deviceName, std::vector<struct UsbRightAppInfo> &infos);
    /* query (user, app) record */
    int32_t QueryAppRightRecord(int32_t uid, const std::string &bundleName, std::vector<struct UsbRightAppInfo> &infos);
    /* query (user, device, app, tokenId) record */
    int32_t QueryRightRecord(int32_t uid, const std::string &deviceName, const std::string &bundleName,
        const std::string &tokenId, std::vector<struct UsbRightAppInfo> &infos);
    /* query users */
    int32_t QueryRightRecordUids(std::vector<std::string> &uids);
    /* query apps */
    int32_t QueryRightRecordApps(int32_t uid, std::vector<std::string> &apps);

    /* update (user, device, app) record */
    int32_t UpdateRightRecord(
        int32_t uid, const std::string &deviceName, const std::string &bundleName, struct UsbRightAppInfo &info);

    /* add or update (user, device, app, tokenId) record */
    int32_t AddOrUpdateRightRecord(int32_t uid, const std::string &deviceName, const std::string &bundleName,
        const std::string &tokenId, struct UsbRightAppInfo &info);

    /* delete (user, device, app, tokenId) record */
    int32_t DeleteRightRecord(int32_t uid, const std::string &deviceName,
        const std::string &bundleName, const std::string &tokenId);
    /* delete (user, device) record */
    int32_t DeleteDeviceRightRecord(int32_t uid, const std::string &deviceName);
    /* delete (user, app) record */
    int32_t DeleteAppRightRecord(int32_t uid, const std::string &bundleName);
    /* delete (user, apps) record */
    int32_t DeleteAppsRightRecord(int32_t uid, const std::vector<std::string> &bundleNames);
    /* delete (user) record */
    int32_t DeleteUidRightRecord(int32_t uid);
    /* delete (user, time) expired record */
    int32_t DeleteNormalExpiredRightRecord(int32_t uid, uint64_t expiredTime);
    /* delete (validTime, device) record */
    int32_t DeleteValidPeriodRightRecord(long validPeriod, const std::string &deviceName);

private:
    UsbRightDbHelper();
    DISALLOW_COPY_AND_MOVE(UsbRightDbHelper);

    int32_t CheckIfNeedUpdateEx(bool &isUpdate, int32_t uid, const std::string &deviceName,
        const std::string &bundleName, const std::string &tokenId);
    int32_t AddOrUpdateRightRecordEx(bool isUpdate, int32_t uid, const std::string &deviceName,
        const std::string &bundleName, const std::string &tokenId, struct UsbRightAppInfo &info);
    int32_t QueryRightRecordCount(void);
    int32_t GetResultSetTableInfo(
        const std::shared_ptr<OHOS::NativeRdb::ResultSet> &resultSet, struct UsbRightTableInfo &table);
    int32_t GetResultRightRecordEx(const std::shared_ptr<OHOS::NativeRdb::ResultSet> &resultSet,
        std::vector<struct UsbRightAppInfo> &infos);
    int32_t QueryAndGetResult(const OHOS::NativeRdb::RdbPredicates &rdbPredicates,
        const std::vector<std::string> &columns, std::vector<struct UsbRightAppInfo> &infos);
    int32_t QueryAndGetResultColumnValues(const OHOS::NativeRdb::RdbPredicates &rdbPredicates,
        const std::vector<std::string> &columns, const std::string &columnName, std::vector<std::string> &columnValues);
    int32_t DeleteAndNoOtherOperation(const std::string &whereClause, const std::vector<std::string> &whereArgs);
    int32_t DeleteAndNoOtherOperation(const OHOS::NativeRdb::RdbPredicates &rdbPredicates);

    static std::shared_ptr<UsbRightDbHelper> instance_;
    std::mutex databaseMutex_;
    std::shared_ptr<UsbRightDataBase> rightDatabase_;
};

} // namespace USB
} // namespace OHOS

#endif // USB_RIGHT_DB_HELPER_H
