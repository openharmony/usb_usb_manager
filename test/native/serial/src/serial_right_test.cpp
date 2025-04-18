/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "serial_right_test.h"

#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "usb_srv_client.h"
#include "usb_right_db_helper.h"
#include "hilog_wrapper.h"
#include "grant_sys_permission.h"

using namespace testing::ext;
using namespace OHOS::HDI::Usb::Serial::V1_0;
using namespace OHOS::Security::AccessToken;

using OHOS::USB::UsbSrvClient;
using OHOS::USB::MODULE_USB_SERVICE;
using OHOS::USB::USB_MGR_LABEL;

constexpr int32_t VALID_PORTID = 0;
constexpr int32_t INVALID_PORTID = -1;
constexpr int32_t OK = 0;

static std::vector<OHOS::USB::UsbSerialPort> g_portList;

namespace OHOS {
namespace SERIAL {
void SerialRightTest::SetUpTestCase(void)
{
    UsbSrvClient::GetInstance().SerialGetPortList(g_portList);
    for (auto it : g_portList) {
        UsbSrvClient::GetInstance().CancelSerialRight(it.portId_);
    }

    UsbSrvClient::GetInstance().SerialGetPortList(g_portList);
    GrantPermissionSysNative();
}

void SerialRightTest::TearDownTestCase(void) {}

void SerialRightTest::SetUp(void) {}

void SerialRightTest::TearDown(void) {}

/**
 * @tc.name: HasSerialRight001
 * @tc.desc: Test functions to int32_t HasSerialRight(int32_t portId)
 * @tc.type: FUNC
 */
HWTEST_F(SerialRightTest, HasSerialRight001, TestSize.Level1)
{
    bool hasRight = false;
    UsbSrvClient::GetInstance().CancelSerialRight(VALID_PORTID);
    UsbSrvClient::GetInstance().HasSerialRight(VALID_PORTID, hasRight);
    EXPECT_EQ(hasRight, true);
    UsbSrvClient::GetInstance().RequestSerialRight(VALID_PORTID, hasRight);
    UsbSrvClient::GetInstance().HasSerialRight(VALID_PORTID, hasRight);
    EXPECT_EQ(hasRight, true);
}

/**
 * @tc.name: HasSerialRight002
 * @tc.desc: Test functions to int32_t HasSerialRight(int32_t portId)
 * @tc.type: FUNC
 */
HWTEST_F(SerialRightTest, HasSerialRight002, TestSize.Level1)
{
    bool hasRight = false;
    int ret = UsbSrvClient::GetInstance().HasSerialRight(INVALID_PORTID, hasRight);
    EXPECT_NE(ret, OK);
    UsbSrvClient::GetInstance().RequestSerialRight(INVALID_PORTID, hasRight);
    ret = UsbSrvClient::GetInstance().HasSerialRight(INVALID_PORTID, hasRight);
    EXPECT_NE(ret, OK);
}

/**
 * @tc.name: CancelSerialRight001
 * @tc.desc: Test functions to int32_t CancelSerialRight(int32_t portId)
 * @tc.type: FUNC
 */
HWTEST_F(SerialRightTest, CancelSerialRight001, TestSize.Level1)
{
    UsbSrvClient::GetInstance().CancelSerialRight(VALID_PORTID);
    bool hasRight = false;
    UsbSrvClient::GetInstance().RequestSerialRight(VALID_PORTID, hasRight);
    EXPECT_EQ(UsbSrvClient::GetInstance().CancelSerialRight(VALID_PORTID), OK);
}

/**
 * @tc.name: CancelSerialRight002
 * @tc.desc: Test functions to int32_t CancelSerialRight(int32_t portId)
 * @tc.type: FUNC
 */
HWTEST_F(SerialRightTest, CancelSerialRight002, TestSize.Level1)
{
    UsbSrvClient::GetInstance().CancelSerialRight(VALID_PORTID);
    bool hasRight = false;
    UsbSrvClient::GetInstance().RequestSerialRight(VALID_PORTID, hasRight);
    UsbSrvClient::GetInstance().CancelSerialRight(VALID_PORTID);
    EXPECT_EQ(UsbSrvClient::GetInstance().CancelSerialRight(VALID_PORTID), OK);
}

/**
 * @tc.name: CancelSerialRight003
 * @tc.desc: Test functions to int32_t CancelSerialRight(int32_t portId)
 * @tc.type: FUNC
 */
HWTEST_F(SerialRightTest, CancelSerialRight003, TestSize.Level1)
{
    EXPECT_NE(UsbSrvClient::GetInstance().CancelSerialRight(INVALID_PORTID), OK);
}

/**
 * @tc.name: RequestSerialRight001
 * @tc.desc: Test functions to int32_t RequestSerialRight(int32_t portId)
 * @tc.type: FUNC
 */
HWTEST_F(SerialRightTest, RequestSerialRight001, TestSize.Level1)
{
    UsbSrvClient::GetInstance().CancelSerialRight(VALID_PORTID);
    bool hasRight = false;
    EXPECT_EQ(UsbSrvClient::GetInstance().RequestSerialRight(VALID_PORTID, hasRight), OK);
}

/**
 * @tc.name: RequestSerialRight002
 * @tc.desc: Test functions to int32_t RequestSerialRight(int32_t portId)
 * @tc.type: FUNC
 */
HWTEST_F(SerialRightTest, RequestSerialRight002, TestSize.Level1)
{
    bool hasRight = false;
    EXPECT_NE(UsbSrvClient::GetInstance().RequestSerialRight(INVALID_PORTID, hasRight), OK);
}
}
}