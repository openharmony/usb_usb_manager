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

#include "usb_manage_device_policy_test.h"

#include <sys/time.h>
#include <iostream>
#include <vector>

#include "delayed_sp_singleton.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "usb_common_test.h"
#include "usb_srv_client.h"
#include "usb_errors.h"
#include "usb_interface_type.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::USB;
using namespace std;
using namespace OHOS::HDI::Usb::V1_0;
using namespace OHOS::USB::Common;

namespace OHOS {
namespace USB {
namespace ManageDevicePolicy {
constexpr int32_t SLEEP_TIME = 3;

void UsbManageDevicePolicyTest::SetUpTestCase(void)
{
    UsbCommonTest::GrantPermissionSysNative();
    auto &srvClient = UsbSrvClient::GetInstance();
    auto ret = srvClient.SetPortRole(1, 1, 1);
    sleep(SLEEP_TIME);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbManageDevicePolicyTest:: [Device] SetPortRole=%{public}d", ret);
    ret = UsbCommonTest::SwitchErrCode(ret);
    ASSERT_TRUE(ret == 0);
    if (ret != 0) {
        exit(0);
    }

    std::cout << "please connect device, press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (c == '\n' || c == EOF) {
            break;
        }
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Start UsbManageDevicePolicyTest");
}

void UsbManageDevicePolicyTest::TearDownTestCase(void)
{
    USB_HILOGI(MODULE_USB_SERVICE, "End UsbManageDevicePolicyTest");
}

void UsbManageDevicePolicyTest::SetUp(void) {}

void UsbManageDevicePolicyTest::TearDown(void) {}

/**
 * @tc.name: ManageDevicePolicy001
 * @tc.desc: Test functions to ManageDevicePolicy(std::vector<UsbDeviceId> &trustList);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageDevicePolicyTest, ManageGlobalInterface001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageDevicePolicy001 : ManageDevicePolicy");
    auto &client = UsbSrvClient::GetInstance();
    std::vector<UsbDeviceId> trustList{};
    auto ret = client.ManageDevicePolicy(trustList);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageDevicePolicy001 : ManageDevicePolicy");
}

/**
 * @tc.name: ManageDevicePolicy002
 * @tc.desc: Test functions to ManageDevicePolicy(std::vector<UsbDeviceId> &trustList);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageDevicePolicyTest, ManageDevicePolicy002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageDevicePolicy002 : ManageDevicePolicy");
    UsbCommonTest::GrantPermissionNormalNative();
    auto &client = UsbSrvClient::GetInstance();
    std::vector<UsbDeviceId> trustList{};
    auto ret = client.ManageDevicePolicy(trustList);
    ASSERT_NE(ret, 0);
    UsbCommonTest::GrantPermissionSysNative();
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageDevicePolicy002 : ManageDevicePolicy");
}

/**
 * @tc.name: ManageDevicePolicy003
 * @tc.desc: Test functions to ManageDevicePolicy(std::vector<UsbDeviceId> &trustList);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageDevicePolicyTest, ManageDevicePolicy003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageDevicePolicy003 : ManageDevicePolicy");
    auto &client = UsbSrvClient::GetInstance();
    std::vector<UsbDevice> devList;
    std::vector<UsbDeviceId> trustList;
    auto ret = client.GetDevices(devList);
    ASSERT_EQ(ret, 0);
    ASSERT_TRUE(!devList.empty());
    UsbDeviceId devId = {0, 0};
    trustList.emplace_back(devId);
    ret = client.ManageDevicePolicy(trustList);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageDevicePolicy003 : ManageDevicePolicy");
}

/**
 * @tc.name: ManageDevicePolicy004
 * @tc.desc: Test functions to ManageDevicePolicy(std::vector<UsbDeviceId> &trustList);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageDevicePolicyTest, ManageDevicePolicy004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageDevicePolicy004 : ManageDevicePolicy");
    auto &client = UsbSrvClient::GetInstance();
    std::vector<UsbDevice> devList;
    std::vector<UsbDeviceId> trustList;
    auto ret = client.GetDevices(devList);
    ASSERT_EQ(ret, 0);
    ASSERT_TRUE(!devList.empty());
    UsbDeviceId devId;
    devId.productId = devList.at(0).GetProductId();
    devId.vendorId = devList.at(0).GetVendorId();
    trustList.emplace_back(devId);
    ret = client.ManageDevicePolicy(trustList);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageDevicePolicy004 : ManageDevicePolicy");
}

/**
 * @tc.name: ManageDevicePolicy005
 * @tc.desc: Test functions to ManageDevicePolicy(std::vector<UsbDeviceId> &trustList);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageDevicePolicyTest, ManageDevicePolicy005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageDevicePolicy005 : ManageDevicePolicy");
    UsbCommonTest::GrantPermissionNormalNative();
    auto &client = UsbSrvClient::GetInstance();
    std::vector<UsbDevice> devList;
    std::vector<UsbDeviceId> trustList;
    auto ret = client.GetDevices(devList);
    ASSERT_EQ(ret, 0);
    ASSERT_TRUE(!devList.empty());
    UsbDeviceId devId = {0, 0};
    trustList.emplace_back(devId);
    ret = client.ManageDevicePolicy(trustList);
    ASSERT_NE(ret, 0);
    UsbCommonTest::GrantPermissionSysNative();
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageDevicePolicy005 : ManageDevicePolicy");
}

/**
 * @tc.name: ManageDevicePolicy006
 * @tc.desc: Test functions to ManageDevicePolicy(std::vector<UsbDeviceId> &trustList);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageDevicePolicyTest, ManageDevicePolicy006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageDevicePolicy006 : ManageDevicePolicy");
    UsbCommonTest::GrantPermissionNormalNative();
    auto &client = UsbSrvClient::GetInstance();
    std::vector<UsbDevice> devList;
    std::vector<UsbDeviceId> trustList;
    auto ret = client.GetDevices(devList);
    ASSERT_EQ(ret, 0);
    ASSERT_TRUE(!devList.empty());
    UsbDeviceId devId;
    devId.productId = devList.at(0).GetProductId();
    devId.vendorId = devList.at(0).GetVendorId();
    trustList.emplace_back(devId);
    ret = client.ManageDevicePolicy(trustList);
    ASSERT_NE(ret, 0);
    UsbCommonTest::GrantPermissionSysNative();
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageDevicePolicy006 : ManageDevicePolicy");
}
} // ManagerInterface
} // USB
} // OHOS
