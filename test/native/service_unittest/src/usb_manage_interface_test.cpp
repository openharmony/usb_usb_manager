/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "usb_manage_interface_test.h"

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

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::USB;
using namespace std;
using namespace OHOS::HDI::Usb::V1_0;
using namespace OHOS::USB::Common;

namespace OHOS {
namespace USB {
namespace ManageInterface {
constexpr int32_t SLEEP_TIME = 3;

std::unordered_map<InterfaceType, std::vector<int32_t>> typeMapInfo = {
    {InterfaceType::TYPE_STORAGE,   {8, -1, -1}},
    {InterfaceType::TYPE_AUDIO,     {1, -1, -1}},
    {InterfaceType::TYPE_HID,       {3, -1, -1}},
    {InterfaceType::TYPE_PRINTER,   {7, -1, -1}}
};

void UsbManageInterfaceTest::SetUpTestCase(void)
{
    UsbCommonTest::GrantPermissionSysNative();
    auto &srvClient = UsbSrvClient::GetInstance();
    auto ret = srvClient.SetPortRole(1, 1, 1);
    sleep(SLEEP_TIME);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbManageInterfaceTest:: [Device] SetPortRole=%{public}d", ret);
    ret = UsbCommonTest::SwitchErrCode(ret);
    ASSERT_TRUE(ret == 0);
    if (ret != 0) {
        exit(0);
    }

    std::cout << "please connect device, press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
        ;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Start UsbManageInterfaceTest");
}

void UsbManageInterfaceTest::TearDownTestCase(void)
{
    USB_HILOGI(MODULE_USB_SERVICE, "End UsbManageInterfaceTest");
}

void UsbManageInterfaceTest::SetUp(void) {}

void UsbManageInterfaceTest::TearDown(void) {}

/**
 * @tc.name: ManageGlobalInterface001
 * @tc.desc: Test functions to ManageGlobalInterface(bool disable);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageInterfaceTest, ManageGlobalInterface001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageGlobalInterface001 : ManageGlobalInterface");
    auto &client = UsbSrvClient::GetInstance();
    auto ret = client.ManageGlobalInterface(true);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageGlobalInterface001 : ManageGlobalInterface");
}

/**
 * @tc.name: ManageGlobalInterface002
 * @tc.desc: Test functions to ManageGlobalInterface(bool disable);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageInterfaceTest, ManageGlobalInterface002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageGlobalInterface002 : ManageGlobalInterface");
    UsbCommonTest::GrantPermissionNormalNative();
    auto &client = UsbSrvClient::GetInstance();
    auto ret = client.ManageGlobalInterface(true);
    ASSERT_NE(ret, 0);
    UsbCommonTest::GrantPermissionSysNative();
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageGlobalInterface002 : ManageGlobalInterface");
}

/**
 * @tc.name: ManageGlobalInterface003
 * @tc.desc: Test functions to ManageGlobalInterface(bool disable);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageInterfaceTest, ManageGlobalInterface003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageGlobalInterface003 : ManageGlobalInterface");
    auto &client = UsbSrvClient::GetInstance();
    auto ret = client.ManageGlobalInterface(false);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageGlobalInterface003 : ManageGlobalInterface");
}

/**
 * @tc.name: ManageDevice001
 * @tc.desc: Test functions to ManageDevice(int32_t vendorId, int32_t productId, bool disable);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageInterfaceTest, ManageDevice001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageDevice001 : ManageDevice");
    auto &client = UsbSrvClient::GetInstance();
    vector<UsbDevice> devi;
    auto ret = client.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    int32_t vendorId = 0;
    int32_t productId = 0;
    if (devi.size() > 0) {
        vendorId = devi.at(0).GetVendorId();
        productId = devi.at(0).GetProductId();
    }
    ret = client.ManageDevice(vendorId, productId, true);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageDevice001 : ManageDevice");
}

/**
 * @tc.name: ManageDevice002
 * @tc.desc: Test functions to ManageDevice(int32_t vendorId, int32_t productId, bool disable);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageInterfaceTest, ManageDevice002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageDevice002 : ManageDevice");
    UsbCommonTest::GrantPermissionNormalNative();
    auto &client = UsbSrvClient::GetInstance();
    vector<UsbDevice> devi;
    auto ret = client.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    int32_t vendorId = 0;
    int32_t productId = 0;
    if (devi.size() > 0) {
        vendorId = devi.at(0).GetVendorId();
        productId = devi.at(0).GetProductId();
    }
    ret = client.ManageDevice(vendorId, productId, true);
    ASSERT_NE(ret, 0);
    UsbCommonTest::GrantPermissionSysNative();
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageDevice002 : ManageDevice");
}

/**
 * @tc.name: ManageDevice003
 * @tc.desc: Test functions to ManageDevice(int32_t vendorId, int32_t productId, bool disable);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageInterfaceTest, ManageDevice003, TestSize.Level1)
{
    auto &client = UsbSrvClient::GetInstance();
    vector<UsbDevice> devi;
    auto ret = client.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    int32_t vendorId = 0;
    int32_t productId = 0;
    if (devi.size() > 0) {
        vendorId = devi.at(0).GetVendorId();
        productId = devi.at(0).GetProductId();
    }
    ret = client.ManageDevice(vendorId, productId, false);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ManageInterfaceType001
 * @tc.desc: Test functions toManageInterfaceType(InterfaceType interfaceType, bool disable);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageInterfaceTest, ManageInterfaceType001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageInterfaceType001 : ManageInterfaceType");
    auto &client = UsbSrvClient::GetInstance();
    vector<UsbDevice> devi;
    auto ret = client.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    InterfaceType interfaceType = InterfaceType::TYPE_HID;
    if (devi.size() > 0) {
        for (auto iter = typeMapInfo.begin(); iter != typeMapInfo.end(); iter ++) {
            if (devi.at(0).GetClass() == iter->second[0] &&
            (devi.at(0).GetSubclass() == iter->second[1] || iter->second[1] == -1) &&
            (devi.at(0).GetProtocol() == iter->second[2] || iter->second[2] == -1)) {
                interfaceType = iter->first;
            }
        }
    }
    ret = client.ManageInterfaceType(interfaceType, true);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageInterfaceType001 : ManageInterfaceType");
}

/**
 * @tc.name: ManageInterfaceType002
 * @tc.desc: Test functions to ManageInterfaceType(InterfaceType interfaceType, bool disable);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageInterfaceTest, ManageInterfaceType002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageInterfaceType002 : ManageInterfaceType");
    UsbCommonTest::GrantPermissionNormalNative();
    auto &client = UsbSrvClient::GetInstance();
    vector<UsbDevice> devi;
    auto ret = client.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    InterfaceType interfaceType = InterfaceType::TYPE_HID;
    if (devi.size() > 0) {
        for (auto iter = typeMapInfo.begin(); iter != typeMapInfo.end(); iter ++) {
            if (devi.at(0).GetClass() == iter->second[0] &&
            (devi.at(0).GetSubclass() == iter->second[1] || iter->second[1] == -1) &&
            (devi.at(0).GetProtocol() == iter->second[2] || iter->second[2] == -1)) {
                interfaceType = iter->first;
            }
        }
    }
    ret = client.ManageInterfaceType(interfaceType, true);
    ASSERT_NE(ret, 0);
    UsbCommonTest::GrantPermissionSysNative();
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageInterfaceType002 : ManageInterfaceType");
}

/**
 * @tc.name: ManageInterfaceType003
 * @tc.desc: Test functions to ManageInterfaceType(InterfaceType interfaceType, bool disable);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageInterfaceTest, ManageInterfaceType003, TestSize.Level1)
{
    auto &client = UsbSrvClient::GetInstance();
    vector<UsbDevice> devi;
    auto ret = client.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    InterfaceType interfaceType = InterfaceType::TYPE_HID;
    if (devi.size() > 0) {
        for (auto iter = typeMapInfo.begin(); iter != typeMapInfo.end(); iter ++) {
            if (devi.at(0).GetClass() == iter->second[0] &&
            (devi.at(0).GetSubclass() == iter->second[1] || iter->second[1] == -1) &&
            (devi.at(0).GetProtocol() == iter->second[2] || iter->second[2] == -1)) {
                interfaceType = iter->first;
            }
        }
    }
    ret = client.ManageInterfaceType(interfaceType, false);
    ASSERT_EQ(ret, 0);
}

} // ManagerInterface
} // USB
} // OHOS
