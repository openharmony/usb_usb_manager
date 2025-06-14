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

#include "usb_device_pipe_test.h"

#include <sys/time.h>
#include <sys/ioctl.h>
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
using namespace OHOS::HDI::Usb::V1_2;
using namespace OHOS::USB::Common;

namespace OHOS {
namespace USB {
namespace DevicePipe {
constexpr int32_t USB_BUS_NUM_INVALID = -1;
constexpr int32_t USB_DEV_ADDR_INVALID = -1;
constexpr int32_t SLEEP_TIME = 3;
constexpr int32_t BUFFER_SIZE = 255;
#define USBDEVFS_GET_SPEED          _IO('U', 31)
void UsbDevicePipeTest::SetUpTestCase(void)
{
    UsbCommonTest::GrantPermissionSysNative();
    auto &srvClient = UsbSrvClient::GetInstance();
    auto ret = srvClient.SetPortRole(1, 1, 1);
    sleep(SLEEP_TIME);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest:: [Device] SetPortRole=%{public}d", ret);
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
    USB_HILOGI(MODULE_USB_SERVICE, "Start UsbDevicePipeTest");
}

void UsbDevicePipeTest::TearDownTestCase(void)
{
    USB_HILOGI(MODULE_USB_SERVICE, "End UsbDevicePipeTest");
}

void UsbDevicePipeTest::SetUp(void) {}

void UsbDevicePipeTest::TearDown(void) {}

/**
 * @tc.name: getDevices001
 * @tc.desc: Test functions to getDevices(std::vector<UsbDevice> &delist);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, getDevices001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : getDevices001 : getDevices");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::getDevices001 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::getDevices001 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::getDevices001 %{public}d OpenDevice=%{public}d", __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::getDevices001 %{public}d Close=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : getDevices001 : getDevices");
}

/**
 * @tc.name: UsbOpenDevice001
 * @tc.desc: Test functions of OpenDevice
 * @tc.desc: int32_t OpenDevice(const UsbDevice &device, USBDevicePipe &pip)
 * @tc.desc: 正向测试：代码正常运行，返回结果为0
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbOpenDevice001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbOpenDevice001: OpenDevice");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    std::vector<UsbDevice> delist;
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbOpenDevice001 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbOpenDevice001 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    USBDevicePipe pipe;
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbOpenDevice001 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbOpenDevice001: OpenDevice");
}

/**
 * @tc.name: UsbResetDevice001
 * @tc.desc: Test functions of ResetDevice
 * @tc.desc: int32_t ResetDevice(const UsbDevice &device)
 * @tc.desc: 正向测试：代码正常运行，返回结果为0
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbResetDevice001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbResetDevice001: ResetDevice");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    std::vector<UsbDevice> delist;
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbResetDevice001 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbResetDevice001 %{public}d size=%{public}zu", __LINE__,
        delist.size());
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    USBDevicePipe pipe;
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbResetDevice001 %{public}d OpenDevice=%{public}d", __LINE__,
        ret);
    ret = UsbSrvClient.ResetDevice(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbResetDevice001 %{public}d ResetDevice=%{public}d", __LINE__,
        ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbResetDevice001: ResetDevice");
}

/**
 * @tc.name: UsbResetDevice002
 * @tc.desc: Test functions of ResetDevice
 * @tc.desc: int32_t ResetDevice(const UsbDevice &device)=
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbResetDevice002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbResetDevice002: ResetDevice");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    std::vector<UsbDevice> delist;
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbResetDevice002 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbResetDevice002 %{public}d size=%{public}zu", __LINE__,
        delist.size());
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    USBDevicePipe pipe;
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbResetDevice002 %{public}d OpenDevice=%{public}d", __LINE__,
        ret);
    USBDevicePipe testPipe;
    testPipe.SetBusNum(pipe.GetBusNum());
    testPipe.SetDevAddr(USB_DEV_ADDR_INVALID);
    ret = UsbSrvClient.ResetDevice(testPipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbResetDevice002 %{public}d ResetDevice=%{public}d", __LINE__,
        ret);
    ASSERT_NE(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbResetDevice002: ResetDevice");
}

/**
 * @tc.name: UsbResetDevice003
 * @tc.desc: Test functions of ResetDevice
 * @tc.desc: int32_t ResetDevice(const UsbDevice &device)=
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbResetDevice003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbResetDevice003: ResetDevice");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    std::vector<UsbDevice> delist;
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbResetDevice003 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbResetDevice003 %{public}d size=%{public}zu", __LINE__,
        delist.size());
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    USBDevicePipe pipe;
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbResetDevice003 %{public}d OpenDevice=%{public}d", __LINE__,
        ret);
    USBDevicePipe testPipe;
    testPipe.SetBusNum(USB_BUS_NUM_INVALID);
    testPipe.SetDevAddr(pipe.GetDevAddr());
    ret = UsbSrvClient.ResetDevice(testPipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbResetDevice003 %{public}d ResetDevice=%{public}d", __LINE__,
        ret);
    ASSERT_NE(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbResetDevice003: ResetDevice");
}

/**
 * @tc.name: UsbResetDevice004
 * @tc.desc: Test functions of ResetDevice
 * @tc.desc: int32_t ResetDevice(const UsbDevice &device)=
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbResetDevice004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbResetDevice004: ResetDevice");
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    std::vector<UsbDevice> delist;
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbResetDevice004 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbResetDevice004 %{public}d size=%{public}zu", __LINE__,
        delist.size());
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    USBDevicePipe pipe;
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbResetDevice004 %{public}d OpenDevice=%{public}d", __LINE__,
        ret);
    USBDevicePipe testPipe;
    testPipe.SetBusNum(USB_BUS_NUM_INVALID);
    testPipe.SetDevAddr(USB_DEV_ADDR_INVALID);
    ret = UsbSrvClient.ResetDevice(testPipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbResetDevice004 %{public}d ResetDevice=%{public}d", __LINE__,
        ret);
    ASSERT_NE(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbResetDevice004: ResetDevice");
}

/**
 * @tc.name: Usbcontrolstansfer001
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer001 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer001 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer001 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer001 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint32_t len = 8;
    uint8_t buffer[BUFFER_SIZE] = {0};
    struct UsbCtrlTransfer ctrldata = {0b10000000, 8, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer001 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer001 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer002
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer002 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer002 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer002 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer002 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint32_t len = 8;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetBusNum(BUFFER_SIZE);
    struct UsbCtrlTransfer ctrldata = {0b10000000, 8, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer002 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer002 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer003
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer003 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer003 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer003 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer003 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint32_t len = 8;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetDevAddr(BUFFER_SIZE);
    struct UsbCtrlTransfer ctrldata = {0b10000000, 8, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer003 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer003 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer004
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer004 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer004 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer004 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer004 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = 8;
    uint8_t buffer[BUFFER_SIZE] = {0};
    struct UsbCtrlTransfer ctrldata = {0b10000000, 6, 0x100, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer004 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer004 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer005
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer005 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer005 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer005 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer005 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = 8;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetBusNum(BUFFER_SIZE);
    struct UsbCtrlTransfer ctrldata = {0b10000000, 6, 0x100, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer005 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer005 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer006
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer006 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer006 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer006 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer006 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = 8;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetDevAddr(BUFFER_SIZE);
    struct UsbCtrlTransfer ctrldata = {0b10000000, 6, 0x100, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer006 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer006 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer007
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer007, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer007 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer007 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer007 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().front();
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer007 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    struct UsbCtrlTransfer ctrldata = {0b10000001, 0X0A, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer007 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer007 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer007 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer008
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer008, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer008 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer008 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer008 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().front();
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer008 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetBusNum(BUFFER_SIZE);
    struct UsbCtrlTransfer ctrldata = {0b10000001, 0X0A, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer008 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer008 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer008 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer009
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer009, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer009 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer009 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer009 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().front();
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer009 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetDevAddr(BUFFER_SIZE);
    struct UsbCtrlTransfer ctrldata = {0b10000001, 0X0A, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer009 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer009 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer009 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer010
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer010, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer010 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer010 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer010 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer010 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    struct UsbCtrlTransfer ctrldata = {0b10000000, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer010 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer010 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer010 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer011
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer011, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer011 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer011 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer011 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer011 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetBusNum(BUFFER_SIZE);
    struct UsbCtrlTransfer ctrldata = {0b10000000, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer011 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer011 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer011 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer012
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer012, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer012 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer012 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer012 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer012 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetDevAddr(BUFFER_SIZE);
    struct UsbCtrlTransfer ctrldata = {0b10000000, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer012 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer012 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer012 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer0013
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer0013, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer0013 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer0013 %{public}d ret=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer0013 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer0013 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    struct UsbCtrlTransfer ctrldata = {0b10000001, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer0013 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer0013 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer0013 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer0014
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer0014, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer0014 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer0014 %{public}d ret=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer0014 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer0014 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetBusNum(BUFFER_SIZE);
    struct UsbCtrlTransfer ctrldata = {0b10000001, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer0014 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer0014 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer0014 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer0015
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer0015, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer0015 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer0015 %{public}d ret=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer0015 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer0015 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetDevAddr(BUFFER_SIZE);
    struct UsbCtrlTransfer ctrldata = {0b10000001, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer0015 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer0015 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer0015 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer016
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer016, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer016 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer016 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer016 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer016 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = 16;
    uint8_t buffer[BUFFER_SIZE] = {0};
    struct UsbCtrlTransfer ctrldata = {0b10000010, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer016 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer016 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer016 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer017
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer017, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer017 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer017 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer017 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer017 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = 16;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetBusNum(BUFFER_SIZE);
    struct UsbCtrlTransfer ctrldata = {0b10000010, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer017 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer017 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer017 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer018
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer018, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer018 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer018 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer018 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer018 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = 16;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetDevAddr(BUFFER_SIZE);
    struct UsbCtrlTransfer ctrldata = {0b10000010, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer018 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer018 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer018 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer019
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer019, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer019 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer019 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer019 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer019 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    struct UsbCtrlTransfer ctrldata = {0b10000010, 0X0C, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer019 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer019 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer020
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer020, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer020 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer020 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer020 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer020 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetBusNum(BUFFER_SIZE);
    struct UsbCtrlTransfer ctrldata = {0b10000010, 0X0C, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer020 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer020 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer021
 * @tc.desc: Test functions to ControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer021, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer021 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer021 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer021 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer021 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetDevAddr(BUFFER_SIZE);
    struct UsbCtrlTransfer ctrldata = {0b10000010, 0X0C, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.ControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer021 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer021 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer022
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer022, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer022 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer022 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer022 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer022 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint32_t len = 8;
    uint8_t buffer[BUFFER_SIZE] = {0};
    struct UsbCtrlTransferParams ctrldata = {0b10000000, 8, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer022 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer022 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer023
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer023, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer023 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer023 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer023 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer023 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint32_t len = 8;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetBusNum(BUFFER_SIZE);
    struct UsbCtrlTransferParams ctrldata = {0b10000000, 8, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer023 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer023 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer024
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer024, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer024 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer024 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer024 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer024 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint32_t len = 8;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetDevAddr(BUFFER_SIZE);
    struct UsbCtrlTransferParams ctrldata = {0b10000000, 8, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer024 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer024 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer025
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer025, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer025 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer025 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer025 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer025 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = 8;
    uint8_t buffer[BUFFER_SIZE] = {0};
    struct UsbCtrlTransferParams ctrldata = {0b10000000, 6, 0x100, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer025 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer025 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer026
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer026, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer026 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer026 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer026 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer026 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = 8;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetBusNum(BUFFER_SIZE);
    struct UsbCtrlTransferParams ctrldata = {0b10000000, 6, 0x100, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer026 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer026 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer027
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer027, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer027 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer027 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer027 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer027 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = 8;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetDevAddr(BUFFER_SIZE);
    struct UsbCtrlTransferParams ctrldata = {0b10000000, 6, 0x100, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer027 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer027 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer028
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer028, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer028 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer028 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer028 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().front();
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer028 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    struct UsbCtrlTransferParams ctrldata = {0b10000001, 0X0A, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer028 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer028 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer028 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer029
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer029, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer029 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer029 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer029 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().front();
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer029 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetBusNum(BUFFER_SIZE);
    struct UsbCtrlTransferParams ctrldata = {0b10000001, 0X0A, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer029 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer029 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer029 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer030
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer030, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer030 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer030 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer030 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().front();
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer030 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetDevAddr(BUFFER_SIZE);
    struct UsbCtrlTransferParams ctrldata = {0b10000001, 0X0A, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer030 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer030 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer030 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer031
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer031, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer031 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer031 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer031 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer031 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    struct UsbCtrlTransferParams ctrldata = {0b10000000, 0, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer031 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer031 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer031 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer032
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer032, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer032 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer032 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer032 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer032 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetBusNum(BUFFER_SIZE);
    struct UsbCtrlTransferParams ctrldata = {0b10000000, 0, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer032 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer032 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer032 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer033
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer033, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer033 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer033 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer033 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer033 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetDevAddr(BUFFER_SIZE);
    struct UsbCtrlTransferParams ctrldata = {0b10000000, 0, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer033 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer033 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer033 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer034
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer034, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer034 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer034 %{public}d ret=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer034 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer034 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    struct UsbCtrlTransferParams ctrldata = {0b10000001, 0, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer034 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer034 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer034 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer035
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer035, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer035 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer035 %{public}d ret=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer035 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer035 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetBusNum(BUFFER_SIZE);
    struct UsbCtrlTransferParams ctrldata = {0b10000001, 0, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer035 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer035 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer035 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer036
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer036, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer036 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer036 %{public}d ret=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer036 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer036 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetDevAddr(BUFFER_SIZE);
    struct UsbCtrlTransferParams ctrldata = {0b10000001, 0, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer036 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer036 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer036 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer037
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer037, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer037 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer037 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer037 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer037 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = 16;
    uint8_t buffer[BUFFER_SIZE] = {0};
    struct UsbCtrlTransferParams ctrldata = {0b10000010, 0, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer037 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer037 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer037 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer038
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer038, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer038 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer038 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer038 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer038 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = 16;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetBusNum(BUFFER_SIZE);
    struct UsbCtrlTransferParams ctrldata = {0b10000010, 0, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer038 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer038 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer038 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer039
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer039, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer039 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer039 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer039 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer039 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    uint32_t len = 16;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetDevAddr(BUFFER_SIZE);
    struct UsbCtrlTransferParams ctrldata = {0b10000010, 0, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer039 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer039 %{public}d Close=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer039 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer040
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer040, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer040 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer040 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer040 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer040 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    struct UsbCtrlTransferParams ctrldata = {0b10000010, 0X0C, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer040 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer040 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer041
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer041, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer041 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer041 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer041 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer041 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetBusNum(BUFFER_SIZE);
    struct UsbCtrlTransferParams ctrldata = {0b10000010, 0X0C, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer041 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer041 : UsbControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer042
 * @tc.desc: Test functions to UsbControlTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Usbcontrolstansfer042, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer042 : UsbControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer042 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer042 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer042 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint32_t len = BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE] = {0};
    pipe.SetDevAddr(BUFFER_SIZE);
    struct UsbCtrlTransferParams ctrldata = {0b10000010, 0X0C, 0, 0, 0, 500};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Usbcontrolstansfer042 %{public}d UsbControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCoreTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer042 : UsbControlTransfer");
}

/**
 * @tc.name: UsbClaimInterface001
 * @tc.desc: Test functions to ClaimInterface(const UsbInterface &interface, bool force);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbClaimInterface001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbClaimInterface001 : ClaimInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface001 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface001 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface001 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().front();
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface001 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface001 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbClaimInterface001 : ClaimInterface");
}

/**
 * @tc.name: UsbClaimInterface002
 * @tc.desc: Test functions to ClaimInterface(const UsbInterface &interface, bool force);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbClaimInterface002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbClaimInterface002 : ClaimInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface002 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface002 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface002 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().front();
    ret = UsbSrvClient.ClaimInterface(pipe, interface, false);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface002 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface002 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbClaimInterface002 : ClaimInterface");
}

/**
 * @tc.name: UsbClaimInterface003
 * @tc.desc: Test functions to ClaimInterface(const UsbInterface &interface, bool force);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbClaimInterface003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbClaimInterface003 : ClaimInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface003 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface003 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface003 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface003 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface003 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbClaimInterface003 : ClaimInterface");
}

/**
 * @tc.name: UsbClaimInterface004
 * @tc.desc: Test functions to ClaimInterface(const UsbInterface &interface, bool force);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbClaimInterface004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbClaimInterface004 : ClaimInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface004 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface004 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface004 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, false);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface004 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface004 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbClaimInterface004 : ClaimInterface");
}

/**
 * @tc.name: UsbClaimInterface005
 * @tc.desc: Test functions to ClaimInterface(const UsbInterface &interface, bool force);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbClaimInterface005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbClaimInterface005 : ClaimInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface005 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface005 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface005 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().front();
    pipe.SetBusNum(BUFFER_SIZE);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface005 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface005 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbClaimInterface005 : ClaimInterface");
}

/**
 * @tc.name: UsbClaimInterface006
 * @tc.desc: Test functions to ClaimInterface(const UsbInterface &interface, bool force);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbClaimInterface006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbClaimInterface006 : ClaimInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface006 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface006 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface006 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().front();
    pipe.SetDevAddr(BUFFER_SIZE);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface006 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface006 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbClaimInterface006 : ClaimInterface");
}

/**
 * @tc.name: UsbClaimInterface007
 * @tc.desc: Test functions to ClaimInterface(const UsbInterface &interface, bool force);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbClaimInterface007, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbClaimInterface007 : ClaimInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface007 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface007 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface007 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    pipe.SetBusNum(BUFFER_SIZE);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface007 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface007 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbClaimInterface007 : ClaimInterface");
}

/**
 * @tc.name: UsbClaimInterface008
 * @tc.desc: Test functions to ClaimInterface(const UsbInterface &interface, bool force);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbClaimInterface008, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbClaimInterface008 : ClaimInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface008 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface008 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface008 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    pipe.SetDevAddr(BUFFER_SIZE);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface008 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbClaimInterface008 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbClaimInterface008 : ClaimInterface");
}

/**
 * @tc.name: UsbReleaseInterface001
 * @tc.desc: Test functions to ReleaseInterface(const UsbInterface &interface);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbReleaseInterface001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbReleaseInterface001 : ReleaseInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface001 %{public}d ret=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface001 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface001 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface001 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface001 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbReleaseInterface001 : ReleaseInterface");
}

/**
 * @tc.name: UsbReleaseInterface002
 * @tc.desc: Test functions to ReleaseInterface(const UsbInterface &interface);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbReleaseInterface002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbReleaseInterface002 : ReleaseInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface002 %{public}d ret=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface002 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface002 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    pipe.SetBusNum(BUFFER_SIZE);
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface002 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface002 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbReleaseInterface002 : ReleaseInterface");
}

/**
 * @tc.name: UsbReleaseInterface003
 * @tc.desc: Test functions to ReleaseInterface(const UsbInterface &interface);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbReleaseInterface003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbReleaseInterface003 : ReleaseInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface003 %{public}d ret=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface003 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface003 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    pipe.SetDevAddr(BUFFER_SIZE);
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface003 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface003 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbReleaseInterface003 : ReleaseInterface");
}

/**
 * @tc.name: UsbReleaseInterface004
 * @tc.desc: Test functions to ReleaseInterface(const UsbInterface &interface);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbReleaseInterface004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbReleaseInterface004 : ReleaseInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface004 %{public}d ret=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface004 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface004 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface004 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface004 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface004 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbReleaseInterface004 : ReleaseInterface");
}

/**
 * @tc.name: UsbReleaseInterface005
 * @tc.desc: Test functions to ReleaseInterface(const UsbInterface &interface);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbReleaseInterface005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbReleaseInterface005 : ReleaseInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface005 %{public}d ret=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface005 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface005 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    pipe.SetBusNum(BUFFER_SIZE);
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface005 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface005 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbReleaseInterface005 : ReleaseInterface");
}

/**
 * @tc.name: UsbReleaseInterface006
 * @tc.desc: Test functions to ReleaseInterface(const UsbInterface &interface);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbReleaseInterface006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbReleaseInterface006 : ReleaseInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface006 %{public}d ret=%{public}d", __LINE__,
               ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface006 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface006 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    pipe.SetDevAddr(BUFFER_SIZE);
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface006 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbReleaseInterface006 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbReleaseInterface006 : ReleaseInterface");
}

/**
 * @tc.name: UsbBulkTransfer001
 * @tc.desc: Test functions to BulkTransfer(const USBEndpoint &endpoint, uint8_t *buffer, uint32_t &length, int32_t
 * timeout);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbBulkTransfer001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer001 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer001 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer001 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer001 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer001 %{public}d point=%{public}d", __LINE__,
               point.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer001 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint8_t buffer[BUFFER_SIZE] = "bulk read";
    uint32_t len = BUFFER_SIZE;
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, point, bulkbuffer, 3000);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer001 %{public}d BulkTransfer=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer001 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer001 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer002
 * @tc.desc: Test functions to BulkTransfer(const USBEndpoint &endpoint, uint8_t *buffer, uint32_t &length, int32_t
 * timeout);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbBulkTransfer002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer002 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer002 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer002 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer002 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer002 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint8_t buffer[BUFFER_SIZE] = "bulk read";
    uint32_t len = BUFFER_SIZE;
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, point, bulkbuffer, -5);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer002 %{public}d BulkTransfer=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer002 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer002 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer003
 * @tc.desc: Test functions to BulkTransfer(const USBEndpoint &endpoint, uint8_t *buffer, uint32_t &length, int32_t
 * timeout);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbBulkTransfer003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer003 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer003 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer003 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer003 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer003 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer003 %{public}d point=%{public}d", __LINE__,
               point.GetInterfaceId());
    uint8_t buffer[BUFFER_SIZE] = "bulk read";
    uint32_t len = BUFFER_SIZE;
    pipe.SetBusNum(BUFFER_SIZE);
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, point, bulkbuffer, 500);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer003 %{public}d BulkTransfer=%{public}d", __LINE__,
               ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer003 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer003 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer004
 * @tc.desc: Test functions to BulkTransfer(const USBEndpoint &endpoint, uint8_t *buffer, uint32_t &length, int32_t
 * timeout);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbBulkTransfer004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer004 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer004 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer004 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer004 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer004 %{public}d point=%{public}d", __LINE__,
               point.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer004 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint8_t buffer[BUFFER_SIZE] = "bulk read";
    uint32_t len = BUFFER_SIZE;
    pipe.SetDevAddr(BUFFER_SIZE);
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, point, bulkbuffer, 500);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer004 %{public}d BulkTransfer=%{public}d", __LINE__,
               ret);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer004 %{public}d len=%{public}d", __LINE__, len);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer004 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer004 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer005
 * @tc.desc: Test functions to BulkTransfer(const USBEndpoint &endpoint, uint8_t *buffer, uint32_t &length, int32_t
 * timeout);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbBulkTransfer005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer005 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer005 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer005 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer005 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer005 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer005 %{public}d point=%{public}d", __LINE__,
               point.GetInterfaceId());
    uint8_t buffer[BUFFER_SIZE] = "bulk read";
    uint32_t len = BUFFER_SIZE;
    point.SetInterfaceId(BUFFER_SIZE);
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, point, bulkbuffer, 500);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer005 %{public}d BulkTransfer=%{public}d", __LINE__,
               ret);
    ASSERT_NE(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer005 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer005 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer006
 * @tc.desc: Test functions to BulkTransfer(const USBEndpoint &endpoint, uint8_t *buffer, uint32_t &length, int32_t
 * timeout);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbBulkTransfer006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer006 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer006 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer006 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer006 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer006 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer006 %{public}d point=%{public}d", __LINE__,
               point.GetInterfaceId());
    uint8_t buffer[BUFFER_SIZE] = "bulk read";
    uint32_t len = BUFFER_SIZE;
    point.SetAddr(BUFFER_SIZE);
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, point, bulkbuffer, 500);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer006 %{public}d BulkTransfer=%{public}d", __LINE__,
               ret);
    ASSERT_NE(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer006 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer006 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer007
 * @tc.desc: Test functions to BulkTransfer(const USBEndpoint &endpoint, uint8_t *buffer, uint32_t &length, int32_t
 * timeout);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbBulkTransfer007, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer007 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer007 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer007 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().at(1);
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer007 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer007 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer007 %{public}d point=%{public}d", __LINE__,
               point.GetInterfaceId());
    uint8_t buffer[BUFFER_SIZE] = "hello world Bulk transfer007";
    uint32_t len = BUFFER_SIZE;
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, point, bulkbuffer, 500);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer007 %{public}d BulkTransfer=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer007 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer007 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer008
 * @tc.desc: Test functions to BulkTransfer(const USBEndpoint &endpoint, uint8_t *buffer, uint32_t &length, int32_t
 * timeout);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbBulkTransfer008, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer008 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer008 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer008 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().at(1);
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer008 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer008 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer008 %{public}d point=%{public}d", __LINE__,
               point.GetInterfaceId());
    uint8_t buffer[BUFFER_SIZE] = "hello world Bulk transfer008";
    uint32_t len = BUFFER_SIZE;
    pipe.SetDevAddr(BUFFER_SIZE);
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, point, bulkbuffer, 500);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer008 %{public}d BulkTransfer=%{public}d", __LINE__,
               ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer008 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer008 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer009
 * @tc.desc: Test functions to BulkTransfer(const USBEndpoint &endpoint, uint8_t *buffer, uint32_t &length, int32_t
 * timeout);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbBulkTransfer009, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer009 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer009 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer009 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().at(1);
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer009 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer009 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer009 %{public}d point=%{public}d", __LINE__,
               point.GetInterfaceId());
    uint8_t buffer[BUFFER_SIZE] = "hello world Bulk transfer009";
    uint32_t len = BUFFER_SIZE;
    pipe.SetBusNum(BUFFER_SIZE);
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, point, bulkbuffer, 500);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer009 %{public}d BulkTransfer=%{public}d", __LINE__,
               ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer009 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer009 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer010
 * @tc.desc: Test functions to BulkTransfer(const USBEndpoint &endpoint, uint8_t *buffer, uint32_t &length, int32_t
 * timeout);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbBulkTransfer010, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer010 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer010 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer010 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().at(1);
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer010 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer010 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer010 %{public}d point=%{public}d", __LINE__,
               point.GetInterfaceId());
    uint8_t buffer[BUFFER_SIZE] = "hello world Bulk transfer010";
    uint32_t len = BUFFER_SIZE;
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, point, bulkbuffer, -5);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer010 %{public}d BulkTransfer=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer010 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer010 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer011
 * @tc.desc: Test functions to BulkTransfer(const USBEndpoint &endpoint, uint8_t *buffer, uint32_t &length, int32_t
 * timeout);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, UsbBulkTransfer011, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer011 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer011 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer011 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer011 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer011 %{public}d OpenDevice again=%{public}d",
    __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer011 %{public}d point=%{public}d", __LINE__,
               point.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer011 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer011 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    uint8_t buffer[BUFFER_SIZE] = "bulk read";
    uint32_t len = BUFFER_SIZE;
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, point, bulkbuffer, 500);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer011 %{public}d BulkTransfer=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::UsbBulkTransfer011 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer011 : BulkTransfer");
}

/**
 * @tc.name: SetConfiguration001
 * @tc.desc: Test functions to  SetConfiguration(const USBConfig &config);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, SetConfiguration001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetConfiguration001 : SetConfiguration");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetConfiguration001 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetConfiguration001 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    USBConfig config = device.GetConfigs().front();
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetConfiguration001 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.SetConfiguration(pipe, config);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetConfiguration001 %{public}d SetConfiguration=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetConfiguration001 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetConfiguration001 : SetConfiguration");
}

/**
 * @tc.name: SetConfiguration002
 * @tc.desc: Test functions to  SetConfiguration(const USBConfig &config);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, SetConfiguration002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetConfiguration002 : SetConfiguration");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetConfiguration002 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetConfiguration002 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    USBConfig config = device.GetConfigs().front();
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetConfiguration002 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    pipe.SetBusNum(BUFFER_SIZE);
    ret = UsbSrvClient.SetConfiguration(pipe, config);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetConfiguration002 %{public}d SetConfiguration=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetConfiguration002 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetConfiguration002 : SetConfiguration");
}

/**
 * @tc.name: SetConfiguration003
 * @tc.desc: Test functions to  SetConfiguration(const USBConfig &config);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, SetConfiguration003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetConfiguration003 : SetConfiguration");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetConfiguration003 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetConfiguration003 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    USBConfig config = device.GetConfigs().front();
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetConfiguration003 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    pipe.SetDevAddr(BUFFER_SIZE);
    ret = UsbSrvClient.SetConfiguration(pipe, config);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetConfiguration003 %{public}d SetConfiguration=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetConfiguration003 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);

    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetConfiguration003 : SetConfiguration");
}

/**
 * @tc.name: Close001
 * @tc.desc: Test functions to  Close();
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Close001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Close001 : Close");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close001 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close001 %{public}d size=%{public}zu", __LINE__, delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close001 %{public}d OpenDevice=%{public}d", __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close001 %{public}d close=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Close001 : Close");
}

/**
 * @tc.name: Close002
 * @tc.desc: Test functions to  Close();
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Close002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Close002 : Close");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close002 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close002 %{public}d size=%{public}zu", __LINE__, delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close002 %{public}d OpenDevice=%{public}d", __LINE__, ret);
    ASSERT_EQ(ret, 0);
    pipe.SetBusNum(BUFFER_SIZE);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close002 %{public}d close=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!ret);
    pipe.SetBusNum(device.GetBusNum());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close002 %{public}d close=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Close002 : Close");
}

/**
 * @tc.name: Close003
 * @tc.desc: Test functions to  Close();
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, Close003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Close003 : Close");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close003 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close003 %{public}d size=%{public}zu", __LINE__, delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close003 %{public}d OpenDevice=%{public}d", __LINE__, ret);
    ASSERT_EQ(ret, 0);
    pipe.SetDevAddr(BUFFER_SIZE);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close003 %{public}d close=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!ret);
    pipe.SetDevAddr(device.GetDevAddr());
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close003 %{public}d close=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Close003 : Close");
}

/**
 * @tc.name: SetInterface001
 * @tc.desc: Test functions to SetInterface(const UsbInterface &interface);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, SetInterface001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetInterface001 : SetInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface001 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface001 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface001 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface001 %{public}d ClaimInterface=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.SetInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface001 %{public}d SetInterface=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface001 %{public}d close=%{public}d", __LINE__, close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetInterface001 : SetInterface");
}

/**
 * @tc.name: SetInterface002
 * @tc.desc: Test functions to SetInterface(const UsbInterface &interface);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, SetInterface002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetInterface002 : SetInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface002 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface002 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface002 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface002 %{public}d ClaimInterface=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    pipe.SetBusNum(BUFFER_SIZE);
    ret = UsbSrvClient.SetInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface002 %{public}d SetInterface=%{public}d", __LINE__,
               ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface002 %{public}d close=%{public}d", __LINE__, close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetInterface002 : SetInterface");
}

/**
 * @tc.name: SetInterface003
 * @tc.desc: Test functions to SetInterface(const UsbInterface &interface);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, SetInterface003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetInterface003 : SetInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface003 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface003 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface003 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface003 %{public}d ClaimInterface=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    pipe.SetDevAddr(BUFFER_SIZE);
    ret = UsbSrvClient.SetInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface003 %{public}d SetInterface=%{public}d", __LINE__,
               ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface003 %{public}d close=%{public}d", __LINE__, close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetInterface003 : SetInterface");
}

/**
 * @tc.name: SetInterface004
 * @tc.desc: Test functions to  SetInterface(const UsbInterface &interface);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, SetInterface004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetInterface004 : SetInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface004 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface004 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface004 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().at(0).GetInterfaces().at(1);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface004 %{public}d ClaimInterface=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.SetInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface004 %{public}d SetInterface=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface004 %{public}d close=%{public}d", __LINE__, close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetInterface004 : SetInterface");
}

/**
 * @tc.name: SetInterface005
 * @tc.desc: Test functions to  SetInterface(const UsbInterface &interface);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, SetInterface005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetInterface005 : SetInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface005 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface005 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface005 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().at(0).GetInterfaces().at(1);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface005 %{public}d ClaimInterface=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    pipe.SetBusNum(BUFFER_SIZE);
    ret = UsbSrvClient.SetInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface005 %{public}d SetInterface=%{public}d", __LINE__,
               ret);
    ASSERT_NE(ret, 0);
    pipe.SetBusNum(device.GetBusNum());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface005 %{public}d close=%{public}d", __LINE__, close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetInterface005 : SetInterface");
}

/**
 * @tc.name: SetInterface006
 * @tc.desc: Test functions to  SetInterface(const UsbInterface &interface);
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, SetInterface006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetInterface006 : SetInterface");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface006 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface006 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface006 %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().at(0).GetInterfaces().at(1);
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface006 %{public}d ClaimInterface=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    pipe.SetDevAddr(BUFFER_SIZE);
    ret = UsbSrvClient.SetInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface006 %{public}d SetInterface=%{public}d", __LINE__,
               ret);
    ASSERT_NE(ret, 0);
    pipe.SetDevAddr(device.GetDevAddr());
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::SetInterface006 %{public}d close=%{public}d", __LINE__, close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetInterface006 : SetInterface");
}

/**
 * @tc.name: GetRawDescriptors001
 * @tc.desc: Test functions to GetRawDescriptors
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, GetRawDescriptors001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetRawDescriptors001 : GetRawDescriptors");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetRawDescriptors001 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetRawDescriptors001 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetRawDescriptors001 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    std::vector<uint8_t> vData;
    ret = UsbSrvClient.GetRawDescriptors(pipe, vData);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetRawDescriptors001 %{public}d GetRawDescriptors=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetRawDescriptors001 : ControlTransfer");
}

/**
 * @tc.name: GetRawDescriptors002
 * @tc.desc: Test functions to GetRawDescriptors
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, GetRawDescriptors002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetRawDescriptors002 : GetRawDescriptors");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetRawDescriptors002 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetRawDescriptors002 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetRawDescriptors002 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    std::vector<uint8_t> vData;
    USBDevicePipe pipeTmp = pipe;
    pipeTmp.SetBusNum(BUFFER_SIZE);
    ret = UsbSrvClient.GetRawDescriptors(pipeTmp, vData);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetRawDescriptors002 %{public}d GetRawDescriptors=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetRawDescriptors002 : ControlTransfer");
}

/**
 * @tc.name: GetRawDescriptors003
 * @tc.desc: Test functions to GetRawDescriptors
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, GetRawDescriptors003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetRawDescriptors003 : GetRawDescriptors");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetRawDescriptors003 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetRawDescriptors003 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetRawDescriptors003 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    std::vector<uint8_t> vData;
    USBDevicePipe pipeTmp = pipe;
    pipeTmp.SetDevAddr(BUFFER_SIZE);
    ret = UsbSrvClient.GetRawDescriptors(pipeTmp, vData);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetRawDescriptors003 %{public}d GetRawDescriptors=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetRawDescriptors003 : ControlTransfer");
}

/**
 * @tc.name: GetFileDescriptors001
 * @tc.desc: Test functions to GetRawDescriptors
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, GetFileDescriptors001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetFileDescriptors001 : GetRawDescriptors");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors001 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors001 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors001 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    std::vector<uint8_t> vData;
    ret = UsbSrvClient.GetRawDescriptors(pipe, vData);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors001 %{public}d GetRawDescriptors=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    int32_t fd = 0;
    ret = UsbSrvClient.GetFileDescriptor(pipe, fd);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors001 %{public}d GetFileDescriptor=%{public}d",
               __LINE__, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors001 %{public}d fd=%{public}d",
               __LINE__, fd);
    ASSERT_EQ(ret, 0);
    ret = ioctl(fd, USBDEVFS_GET_SPEED, NULL);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptor001 %{public}d fd=%{public}d ret=%{public}d",
               __LINE__, fd, ret);
    EXPECT_GE(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetFileDescriptors001 : ControlTransfer");
}

/**
 * @tc.name: GetFileDescriptors002
 * @tc.desc: Test functions to GetRawDescriptors
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, GetFileDescriptors002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetFileDescriptors002 : GetRawDescriptors");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors002 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors002 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors002 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    std::vector<uint8_t> vData;
    ret = UsbSrvClient.GetRawDescriptors(pipe, vData);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors002 %{public}d GetRawDescriptors=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    USBDevicePipe pipeTmp = pipe;
    pipeTmp.SetBusNum(BUFFER_SIZE);
    int32_t fd = 0;
    ret = UsbSrvClient.GetFileDescriptor(pipeTmp, fd);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors002 %{public}d GetFileDescriptor=%{public}d",
               __LINE__, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors001 %{public}d fd=%{public}d",
               __LINE__, fd);
    ASSERT_NE(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetFileDescriptors002 : ControlTransfer");
}

/**
 * @tc.name: GetFileDescriptors003
 * @tc.desc: Test functions to GetRawDescriptors
 * @tc.type: FUNC
 */
HWTEST_F(UsbDevicePipeTest, GetFileDescriptors003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetFileDescriptors003 : GetRawDescriptors");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors003 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors003 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() != 9) {
            device = delist[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors003 %{public}d OpenDevice=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    std::vector<uint8_t> vData;
    ret = UsbSrvClient.GetRawDescriptors(pipe, vData);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors003 %{public}d GetRawDescriptors=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    USBDevicePipe pipeTmp = pipe;
    pipeTmp.SetDevAddr(BUFFER_SIZE);
    int32_t fd = 0;
    ret = UsbSrvClient.GetFileDescriptor(pipeTmp, fd);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors003 %{public}d GetFileDescriptor=%{public}d",
               __LINE__, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::GetFileDescriptors001 %{public}d fd=%{public}d",
               __LINE__, fd);
    ASSERT_NE(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDevicePipeTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetFileDescriptors003 : ControlTransfer");
}
} // DevicePipe
} // USB
} // OHOS
