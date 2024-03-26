/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "usb_device_status_test.h"

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
#include "v1_1/iusb_interface.h"
#include "v1_0/usb_types.h"

constexpr int SLEEP_TIME = 3;
constexpr int32_t BUFFER_SIZE = 255;

using namespace std;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::USB;
using namespace OHOS::USB::Common;
using namespace OHOS::HDI::Usb::V1_1;


namespace OHOS {
namespace USB {

void UsbDeviceStatusTest::SetUpTestCase(void)
{
    UsbCommonTest::GrantPermissionSysNative();
    auto &srvClient = UsbSrvClient::GetInstance();
    auto ret = srvClient.SetPortRole(1, 1, 1);
    sleep(SLEEP_TIME);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceStatusTest:: [Device] SetPortRole=%{public}d", ret);
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
    USB_HILOGI(MODULE_USB_SERVICE, "Start UsbDeviceStatusTest");
}

void UsbDeviceStatusTest::TearDownTestCase(void)
{
    USB_HILOGI(MODULE_USB_SERVICE, "End UsbDeviceStatusTest");
}

void UsbDeviceStatusTest::SetUp(void) {}

void UsbDeviceStatusTest::TearDown(void) {}


/**
 * @tc.name: GetDeviceSpeed001
 * @tc.desc: Test functions to GetDeviceSpeed
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceStatusTest, GetDeviceSpeed001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetDeviceSpeed001");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed001 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed001 %{public}d size=%{public}zu", __LINE__,
        devi.size());
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed001 %{public}d OpenDevice=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    uint8_t speed = 0;
    ret = UsbSrvClient.GetDeviceSpeed(pipe, speed);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed001 %{public}d GetFileDescriptor=%{public}d",
        __LINE__, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed001 %{public}d speed=%{public}u",
        __LINE__, speed);
    EXPECT_TRUE(ret == 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceStatusTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetDeviceSpeed001");
}

/**
 * @tc.name: GetDeviceSpeed002
 * @tc.desc: Test functions to GetDeviceSpeed
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceStatusTest, GetDeviceSpeed002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetDeviceSpeed002");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed002 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed002 %{public}d size=%{public}zu", __LINE__,
        devi.size());
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed002 %{public}d OpenDevice=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    USBDevicePipe pipeTmp = pipe;
    pipeTmp.SetBusNum(BUFFER_SIZE);
    uint8_t speed = 0;
    ret = UsbSrvClient.GetDeviceSpeed(pipeTmp, speed);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed002 %{public}d GetFileDescriptor=%{public}d",
        __LINE__, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed002 %{public}d speed=%{public}u",
        __LINE__, speed);
    EXPECT_TRUE(ret != 0);
    pipe.SetBusNum(device.GetBusNum());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceStatusTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetDeviceSpeed002");
}

/**
 * @tc.name: GetDeviceSpeed003
 * @tc.desc: Test functions to GetDeviceSpeed
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceStatusTest, GetDeviceSpeed003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetDeviceSpeed003");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed003 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed003 %{public}d size=%{public}zu", __LINE__,
        devi.size());
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed003 %{public}d OpenDevice=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    USBDevicePipe pipeTmp = pipe;
    pipeTmp.SetDevAddr(BUFFER_SIZE);
    uint8_t speed = 0;
    ret = UsbSrvClient.GetDeviceSpeed(pipeTmp, speed);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed003 %{public}d GetFileDescriptor=%{public}d",
        __LINE__, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed003 %{public}d speed=%{public}u",
        __LINE__, speed);
    EXPECT_TRUE(ret != 0);
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceStatusTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetDeviceSpeed003");
}

/**
 * @tc.name: GetDeviceSpeed004
 * @tc.desc: Test functions to GetDeviceSpeed
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceStatusTest, GetDeviceSpeed004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetDeviceSpeed004");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed004 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed004 %{public}d size=%{public}zu", __LINE__,
        devi.size());
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed004 %{public}d OpenDevice=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    USBDevicePipe pipeTmp = pipe;
    pipeTmp.SetBusNum(BUFFER_SIZE);
    pipeTmp.SetDevAddr(BUFFER_SIZE);
    uint8_t speed = 0;
    ret = UsbSrvClient.GetDeviceSpeed(pipeTmp, speed);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed004 %{public}d GetFileDescriptor=%{public}d",
        __LINE__, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceSpeed004 %{public}d speed=%{public}u",
        __LINE__, speed);
    EXPECT_TRUE(ret != 0);
    pipe.SetBusNum(device.GetBusNum());
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceStatusTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetDeviceSpeed004");
}

/**
 * @tc.name: GetInterfaceStatus001
 * @tc.desc: Test functions to GetInterfaceStatus
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceStatusTest, GetInterfaceStatus001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetInterfaceStatus001");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus001 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus001 %{public}d size=%{public}zu", __LINE__,
        devi.size());
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus001 %{public}d OpenDevice=%{public}d", __LINE__,
        ret);
    EXPECT_TRUE(ret == 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface006 %{public}d ClaimInterface=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret == 0);
    bool unactived = 1;
    ret = UsbSrvClient.GetInterfaceActiveStatus(pipe, interface, unactived);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus001 %{public}d GetInterfaceStatus=%{public}d",
    __LINE__, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus001 %{public}d unactived=%{public}d",
        __LINE__, unactived);
    EXPECT_TRUE(ret == 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus001 %{public}d close=%{public}d", __LINE__, close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetInterfaceStatus001");
}

/**
 * @tc.name: GetInterfaceStatus002
 * @tc.desc: Test functions to GetInterfaceStatus
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceStatusTest, GetInterfaceStatus002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetInterfaceStatus002");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus002 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus002 %{public}d size=%{public}zu", __LINE__,
        devi.size());
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus002 %{public}d OpenDevice=%{public}d", __LINE__,
        ret);
    EXPECT_TRUE(ret == 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    pipe.SetBusNum(BUFFER_SIZE);
    bool unactived = 1;
    ret = UsbSrvClient.GetInterfaceActiveStatus(pipe, interface, unactived);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus002 %{public}d GetInterfaceStatus=%{public}d",
    __LINE__, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus002 %{public}d unactived=%{public}d",
        __LINE__, unactived);
    EXPECT_TRUE(ret != 0);
    pipe.SetBusNum(device.GetBusNum());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus002 %{public}d close=%{public}d", __LINE__, close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetInterfaceStatus002");
}

/**
 * @tc.name: GetInterfaceStatus003
 * @tc.desc: Test functions to GetInterfaceStatus
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceStatusTest, GetInterfaceStatus003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetInterfaceStatus003");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus003 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus003 %{public}d size=%{public}zu", __LINE__,
        devi.size());
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus003 %{public}d OpenDevice=%{public}d", __LINE__,
        ret);
    EXPECT_TRUE(ret == 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    pipe.SetDevAddr(BUFFER_SIZE);
    bool unactived = 1;
    ret = UsbSrvClient.GetInterfaceActiveStatus(pipe, interface, unactived);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus003 %{public}d GetInterfaceStatus=%{public}d",
    __LINE__, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus003 %{public}d unactived=%{public}d",
        __LINE__, unactived);
    EXPECT_TRUE(ret != 0);
    pipe.SetDevAddr(device.GetDevAddr());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus003 %{public}d close=%{public}d", __LINE__, close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetInterfaceStatus003");
}

/**
 * @tc.name: GetInterfaceStatus004
 * @tc.desc: Test functions to  GetInterfaceStatus
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceStatusTest, GetInterfaceStatus004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetInterfaceStatus004");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus004 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus004 %{public}d size=%{public}zu", __LINE__,
        devi.size());
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus004 %{public}d OpenDevice=%{public}d", __LINE__,
        ret);
    EXPECT_TRUE(ret == 0);
    UsbInterface interface = device.GetConfigs().at(0).GetInterfaces().at(1);
    bool unactived = 1;
    ret = UsbSrvClient.GetInterfaceActiveStatus(pipe, interface, unactived);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus004 %{public}d GetInterfaceStatus=%{public}d",
    __LINE__, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus004 %{public}d unactived=%{public}d",
        __LINE__, unactived);
    EXPECT_TRUE(ret == 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus004 %{public}d close=%{public}d", __LINE__, close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetInterfaceStatus004");
}

/**
 * @tc.name: GetInterfaceStatus005
 * @tc.desc: Test functions to  GetInterfaceStatus
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceStatusTest, GetInterfaceStatus005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetInterfaceStatus005");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus005 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus005 %{public}d size=%{public}zu", __LINE__,
        devi.size());
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus005 %{public}d OpenDevice=%{public}d", __LINE__,
        ret);
    EXPECT_TRUE(ret == 0);
    UsbInterface interface = device.GetConfigs().at(0).GetInterfaces().at(1);
    pipe.SetBusNum(BUFFER_SIZE);
    bool unactived = 1;
    ret = UsbSrvClient.GetInterfaceActiveStatus(pipe, interface, unactived);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus005 %{public}d GetInterfaceStatus=%{public}d",
        __LINE__, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus005 %{public}d unactived=%{public}d",
        __LINE__, unactived);
    EXPECT_TRUE(ret != 0);
    pipe.SetBusNum(device.GetBusNum());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus005 %{public}d close=%{public}d", __LINE__, close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetInterfaceStatus005");
}

/**
 * @tc.name: GetInterfaceStatus006
 * @tc.desc: Test functions to  GetInterfaceStatus
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceStatusTest, GetInterfaceStatus006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetInterfaceStatus007");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus006 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus006 %{public}d size=%{public}zu", __LINE__,
        devi.size());
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus006 %{public}d OpenDevice=%{public}d", __LINE__,
        ret);
    EXPECT_TRUE(ret == 0);
    UsbInterface interface = device.GetConfigs().at(0).GetInterfaces().at(1);
    pipe.SetDevAddr(BUFFER_SIZE);
    bool unactived = 1;
    ret = UsbSrvClient.GetInterfaceActiveStatus(pipe, interface, unactived);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus006 %{public}d GetInterfaceStatus=%{public}d",
    __LINE__, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus006 %{public}d unactived=%{public}d",
        __LINE__, unactived);
    EXPECT_TRUE(ret != 0);
    pipe.SetDevAddr(device.GetDevAddr());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus006 %{public}d close=%{public}d", __LINE__, close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetInterfaceStatus006");
}

/**
 * @tc.name: GetInterfaceStatus007
 * @tc.desc: Test functions to  GetInterfaceStatus
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceStatusTest, GetInterfaceStatus007, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetInterfaceStatus007");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus007 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus007 %{public}d size=%{public}zu", __LINE__,
        devi.size());
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus007 %{public}d OpenDevice=%{public}d", __LINE__,
        ret);
    EXPECT_TRUE(ret == 0);
    UsbInterface interface = device.GetConfigs().at(0).GetInterfaces().at(0);
    pipe.SetDevAddr(BUFFER_SIZE);
    pipe.SetBusNum(BUFFER_SIZE);
    bool unactived = 1;
    ret = UsbSrvClient.GetInterfaceActiveStatus(pipe, interface, unactived);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus007 %{public}d GetInterfaceStatus=%{public}d",
    __LINE__, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus007 %{public}d unactived=%{public}d",
        __LINE__, unactived);
    EXPECT_TRUE(ret != 0);
    pipe.SetDevAddr(device.GetDevAddr());
    pipe.SetBusNum(device.GetBusNum());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus007 %{public}d close=%{public}d", __LINE__, close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetInterfaceStatus007");
}

/**
 * @tc.name: GetInterfaceStatus008
 * @tc.desc: Test functions to  GetInterfaceStatus
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceStatusTest, GetInterfaceStatus008, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetInterfaceStatus008");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    EXPECT_TRUE(ret == 0);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus008 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus008 %{public}d size=%{public}zu", __LINE__,
        devi.size());
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus008 %{public}d OpenDevice=%{public}d", __LINE__,
        ret);
    EXPECT_TRUE(ret == 0);
    UsbInterface interface = device.GetConfigs().at(0).GetInterfaces().at(1);
    pipe.SetDevAddr(BUFFER_SIZE);
    pipe.SetBusNum(BUFFER_SIZE);
    bool unactived = 1;
    ret = UsbSrvClient.GetInterfaceActiveStatus(pipe, interface, unactived);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus008 %{public}d GetInterfaceStatus=%{public}d",
    __LINE__, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus008 %{public}d unactived=%{public}d",
        __LINE__, unactived);
    EXPECT_TRUE(ret != 0);
    pipe.SetDevAddr(device.GetDevAddr());
    pipe.SetBusNum(device.GetBusNum());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "GetInterfaceStatus008 %{public}d close=%{public}d", __LINE__, close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetInterfaceStatus008");
}
} // USB
} // OHOS