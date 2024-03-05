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

#ifndef USB_RIGHT_DATABASE_H
#define USB_RIGHT_DATABASE_H

#include <pthread.h>

#include "data_ability_predicates.h"
#include "rdb_errno.h"
#include "rdb_helper.h"
#include "rdb_open_callback.h"
#include "rdb_predicates.h"
#include "rdb_store.h"
#include "result_set.h"
#include "value_object.h"

namespace OHOS {
namespace USB {

static std::string USB_RIGHT_DB_PATH = "/data/service/el1/public/usb_service/";

constexpr const char *USB_RIGHT_DB_NAME = "usbRight.db";
constexpr const char *USB_RIGHT_TABLE_NAME = "usbRightInfoTable";
constexpr int32_t DATABASE_OPEN_VERSION = 1;
constexpr int32_t DATABASE_NEW_VERSION = 2;

constexpr const char *CREATE_USB_RIGHT_TABLE = "CREATE TABLE IF NOT EXISTS [usbRightInfoTable]("
                                               "[id] INTEGER PRIMARY KEY AUTOINCREMENT, "
                                               "[uid] INTEGER, "
                                               "[installTime] INTEGER, "
                                               "[updateTime] INTEGER, "
                                               "[requestTime] INTEGER, "
                                               "[validPeriod] INTEGER, "
                                               "[deviceName] TEXT,"
                                               "[bundleName] TEXT,"
                                               "[tokenId] TEXT);";

class UsbRightDataBase {
public:
    static std::shared_ptr<UsbRightDataBase> GetInstance();
    int64_t Insert(const OHOS::NativeRdb::ValuesBucket &insertValues);
    int32_t Update(int32_t &changedRows, const OHOS::NativeRdb::ValuesBucket &values,
        const OHOS::NativeRdb::RdbPredicates &predicates);
    int32_t Update(int32_t &changedRows, const OHOS::NativeRdb::ValuesBucket &values, const std::string &whereClause,
        const std::vector<std::string> &whereArgs);
    int32_t Delete(const OHOS::NativeRdb::RdbPredicates &rdbPredicates);
    int32_t Delete(int32_t &changedRows, const std::string &whereClause, const std::vector<std::string> &whereArgs);
    int32_t ExecuteSql(const std::string &sql,
        const std::vector<OHOS::NativeRdb::ValueObject> &bindArgs = std::vector<OHOS::NativeRdb::ValueObject>());
    std::shared_ptr<OHOS::NativeRdb::ResultSet> QuerySql(
        const std::string &sql, const std::vector<std::string> &selectionArgs);
    std::shared_ptr<OHOS::NativeRdb::ResultSet> Query(
        const OHOS::NativeRdb::AbsRdbPredicates &predicates, const std::vector<std::string> &columns);
    int32_t BeginTransaction();
    int32_t Commit();
    int32_t RollBack();

private:
    UsbRightDataBase();
    DISALLOW_COPY_AND_MOVE(UsbRightDataBase);

    static std::shared_ptr<UsbRightDataBase> instance_;
    std::shared_ptr<OHOS::NativeRdb::RdbStore> store_;
};

class UsbRightDataBaseCallBack : public OHOS::NativeRdb::RdbOpenCallback {
public:
    int32_t OnCreate(OHOS::NativeRdb::RdbStore &rdbStore) override;
    int32_t OnUpgrade(OHOS::NativeRdb::RdbStore &rdbStore, int32_t oldVersion, int32_t newVersion) override;
    int32_t OnDowngrade(OHOS::NativeRdb::RdbStore &rdbStore, int32_t currentVersion, int32_t targetVersion) override;
};

} // namespace USB
} // namespace OHOS

#endif // USB_RIGHT_DATABASE_H
