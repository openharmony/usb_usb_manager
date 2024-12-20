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
#include "usb_interface_type.h"

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
 * @tc.name: ManageInterfaceStorage001
 * @tc.desc: Test functions toManageInterfaceStorage(InterfaceType interfaceType, bool disable);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageInterfaceTest, ManageInterfaceType001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageInterfaceStorage001 : ManageInterfaceStorage");
    auto &client = UsbSrvClient::GetInstance();
    vector<UsbDevice> devi;
    auto ret = client.GetDevices(devi);
    ASSERT_EQ(ret, 0);
    vector<UsbDeviceType> disableType;
    UsbDeviceType usbDeviceType;
    usbDeviceType.baseClass = 3;
    usbDeviceType.subClass = 1;
    usbDeviceType.protocol = 2;
    usbDeviceType.isDeviceType = 0;
    disableType.emplace_back(usbDeviceType);
    ret = client.ManageInterfaceType(disableType, true);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageInterfaceStorage001 : ManageInterfaceStorage");
}

/**
 * @tc.name: ManageInterfaceStorage002
 * @tc.desc: Test functions to ManageInterfaceStorage(InterfaceType interfaceType, bool disable);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageInterfaceTest, ManageInterfaceType002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageInterfaceStorage002 : ManageInterfaceStorage");
    UsbCommonTest::GrantPermissionNormalNative();
    auto &client = UsbSrvClient::GetInstance();
    vector<UsbDevice> devi;
    auto ret = client.GetDevices(devi);
    ASSERT_EQ(ret, 0);
    vector<UsbDeviceType> disableType;
    UsbDeviceType usbDeviceType;
    usbDeviceType.baseClass = 8;
    usbDeviceType.subClass = 6;
    usbDeviceType.protocol = 80;
    usbDeviceType.isDeviceType = 0;
    disableType.emplace_back(usbDeviceType);
    ret = client.ManageInterfaceType(disableType, true);
    ASSERT_NE(ret, 0);
    UsbCommonTest::GrantPermissionSysNative();
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageInterfaceStorage002 : ManageInterfaceStorage");
}

/**
 * @tc.name: ManageInterfaceStorage003
 * @tc.desc: Test functions to ManageInterfaceStorage(InterfaceType interfaceType, bool disable);
 * @tc.type: FUNC
 */
HWTEST_F(UsbManageInterfaceTest, ManageInterfaceType003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageInterfaceType003 : ManageInterfaceType");
    auto &client = UsbSrvClient::GetInstance();
    vector<UsbDevice> devi;
    auto ret = client.GetDevices(devi);
    ASSERT_EQ(ret, 0);
    vector<UsbDeviceType> disableType;
    UsbDeviceType usbDeviceType;
    usbDeviceType.baseClass = 3;
    usbDeviceType.subClass = 1;
    usbDeviceType.protocol = 2;
    usbDeviceType.isDeviceType = 0;
    disableType.emplace_back(usbDeviceType);
    ret = client.ManageInterfaceType(disableType, false);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageInterfaceType003 : ManageInterfaceType");
}

HWTEST_F(UsbManageInterfaceTest, ManageInterfaceType004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageInterfaceType004 : ManageInterfaceType");
    auto &client = UsbSrvClient::GetInstance();
    vector<UsbDevice> devi;
    auto ret = client.GetDevices(devi);
    ASSERT_EQ(ret, 0);
    vector<UsbDeviceType> disableType;
    UsbDeviceType usbDeviceType;
    usbDeviceType.baseClass = 8;
    usbDeviceType.subClass = 6;
    usbDeviceType.protocol = 80;
    usbDeviceType.isDeviceType = 0;
    disableType.emplace_back(usbDeviceType);
    ret = client.ManageInterfaceType(disableType, false);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageInterfaceType004 : ManageInterfaceType");
}


HWTEST_F(UsbManageInterfaceTest, ManageInterfaceType005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : ManageInterfaceType005 : ManageDevice");
    UsbCommonTest::GrantPermissionNormalNative();
    auto &client = UsbSrvClient::GetInstance();
    vector<UsbDevice> devi;
    auto ret = client.GetDevices(devi);
    ASSERT_EQ(ret, 0);
    vector<UsbDeviceType> disableType;
    UsbDeviceType usbDeviceType;
    usbDeviceType.baseClass = 8;
    usbDeviceType.subClass = 6;
    usbDeviceType.protocol = 80;
    usbDeviceType.isDeviceType = 0;
    disableType.emplace_back(usbDeviceType);
    ret = client.ManageInterfaceType(disableType, false);
    ASSERT_NE(ret, 0);
    UsbCommonTest::GrantPermissionSysNative();
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : ManageInterfaceType005 : ManageDevice");
}
} // ManagerInterface
} // USB
} // OHOS
