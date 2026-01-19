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

#include "usb_control_transfer_test.h"

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
#include "v1_2/iusb_interface.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::USB;
using namespace std;
using namespace OHOS::HDI::Usb::V1_2;
using namespace OHOS::USB::Common;

namespace OHOS {
namespace USB {
namespace UsbControlTransfer {
constexpr uint32_t USB_DEVICE_CLASS_HUB = 0x09;
std::vector<UsbDevice> g_devices;

void UsbControlTransferTest::SetUpTestCase(void)
{
    UsbCommonTest::GrantPermissionSysNative();
    auto &srvClient = UsbSrvClient::GetInstance();
    std::cout << "please connect device, press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (c == '\n' || c == EOF) {
            break;
        }
    }
    
    int32_t ret = srvClient.GetDevices(g_devices);
    ASSERT_TRUE(ret == 0);
    if (ret != 0) {
        exit(0);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Start UsbControlTransferTest");
}

void UsbControlTransferTest::TearDownTestCase(void)
{}

void UsbControlTransferTest::SetUp(void) {}

void UsbControlTransferTest::TearDown(void) {}

/**
 * @tc.name: UsbControlTransfer001
 * @tc.desc: Test functions to UsbControlTransfer(
    USBDevicePipe &pipe, const HDI::Usb::V1_2::UsbCtrlTransferParams &ctrlParams, std::vector<uint8_t> &bufferData);
 * @tc.type: FUNC
 */
HWTEST_F(UsbControlTransferTest, UsbControlTransfer001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbControlTransfer001 : GET_STATUS");
    auto &client = UsbSrvClient::GetInstance();

    // open device except hub
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < g_devices.size(); i++) {
        if (g_devices[i].GetClass() != USB_DEVICE_CLASS_HUB) {
            device = g_devices[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    USBDevicePipe pipe;
    client.OpenDevice(device, pipe);
    // claim interface and get endpoint
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    auto ret = client.ClaimInterface(pipe, interface, true);
    ASSERT_EQ(ret, 0);

    UsbCtrlTransferParams ctrlParams;
    ctrlParams.requestType = 0x80;
    ctrlParams.requestCmd = 0x00;
    ctrlParams.value = 0x0000;
    ctrlParams.index = 0;
    ctrlParams.length = 2;
    ctrlParams.timeout = 0;
    std::vector<uint8_t> bufferData(ctrlParams.length);
    ret = client.UsbControlTransfer(pipe, ctrlParams, bufferData);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbControlTransfer001 : GET_STATUS");
}

/**
 * @tc.name: UsbControlTransfer002
 * @tc.desc: Test functions to UsbControlTransfer(
    USBDevicePipe &pipe, const HDI::Usb::V1_2::UsbCtrlTransferParams &ctrlParams, std::vector<uint8_t> &bufferData);
 * @tc.type: FUNC
 */
HWTEST_F(UsbControlTransferTest, UsbControlTransfer002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbControlTransfer002 : CLEAR_FEATURE");
    auto &client = UsbSrvClient::GetInstance();
    // open device except hub
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < g_devices.size(); i++) {
        if (g_devices[i].GetClass() != USB_DEVICE_CLASS_HUB) {
            device = g_devices[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    USBDevicePipe pipe;
    client.OpenDevice(device, pipe);
    // claim interface and get endpoint
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    auto ret = client.ClaimInterface(pipe, interface, true);
    ASSERT_EQ(ret, 0);
    // 1. first set feature
    UsbCtrlTransferParams ctrlParams;
    ctrlParams.requestType = 0x02;
    ctrlParams.requestCmd = 0x03;
    ctrlParams.value = 0x0000;
    ctrlParams.index = point.GetAddress();
    ctrlParams.length = 1;
    ctrlParams.timeout = 0;
    std::vector<uint8_t> bufferData(ctrlParams.length);
    ret = client.UsbControlTransfer(pipe, ctrlParams, bufferData);
    ASSERT_EQ(ret, 0);
    // 2. then clear feature
    ctrlParams.requestType = 0x02;
    ctrlParams.requestCmd = 0x0001;
    ctrlParams.value = 0x0000;
    ctrlParams.index = point.GetAddress();
    ctrlParams.length = 1;
    ctrlParams.timeout = 0;
    std::vector<uint8_t> bufferData2(ctrlParams.length);
    ret = client.UsbControlTransfer(pipe, ctrlParams, bufferData2);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbControlTransfer002 : CLEAR_FEATURE");
}

/**
 * @tc.name: UsbControlTransfer003
 * @tc.desc: Test functions to UsbControlTransfer(
    USBDevicePipe &pipe, const HDI::Usb::V1_2::UsbCtrlTransferParams &ctrlParams, std::vector<uint8_t> &bufferData);
 * @tc.type: FUNC
 */
HWTEST_F(UsbControlTransferTest, UsbControlTransfer003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbControlTransfer003 : SET_FEATURE");
    auto &client = UsbSrvClient::GetInstance();
    // open device except hub
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < g_devices.size(); i++) {
        if (g_devices[i].GetClass() != USB_DEVICE_CLASS_HUB) {
            device = g_devices[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    USBDevicePipe pipe;
    client.OpenDevice(device, pipe);
    // claim interface and get endpoint
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    auto ret = client.ClaimInterface(pipe, interface, true);
    ASSERT_EQ(ret, 0);
    // first set feature
    UsbCtrlTransferParams ctrlParams;
    ctrlParams.requestType = 0x02;
    ctrlParams.requestCmd = 0x0003;
    ctrlParams.value = 0x0000;
    ctrlParams.index = point.GetAddress();
    ctrlParams.length = 1;
    ctrlParams.timeout = 0;
    std::vector<uint8_t> bufferData(ctrlParams.length);
    ret = client.UsbControlTransfer(pipe, ctrlParams, bufferData);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbControlTransfer003 : SET_FEATURE");
}

/**
 * @tc.name: UsbControlTransfer004
 * @tc.desc: Test functions to UsbControlTransfer(
    USBDevicePipe &pipe, const HDI::Usb::V1_2::UsbCtrlTransferParams &ctrlParams, std::vector<uint8_t> &bufferData);
 * @tc.type: FUNC
 */
HWTEST_F(UsbControlTransferTest, UsbControlTransfer004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbControlTransfer004 : GET_DESCRIPTOR");
    const std::vector<uint8_t> descTypes = {
        0x01,   // type=0x01: device
        0x02,   // type=0x02: configuration
        0x03,   // type=0x03: string
        0x04,   // type=0x04: interface
        0x05,   // type=0x05: endpoint
        0x06,   // type=0x06: device qualifier
        0x07,   // type=0x07: other speed configuration
        0x08,   // type=0x08: interface power
    };
    auto &client = UsbSrvClient::GetInstance();
    // open device except hub
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < g_devices.size(); i++) {
        if (g_devices[i].GetClass() != USB_DEVICE_CLASS_HUB) {
            device = g_devices[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    USBDevicePipe pipe;
    client.OpenDevice(device, pipe);
    // claim interface and get endpoint
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    auto ret = client.ClaimInterface(pipe, interface, true);
    ASSERT_EQ(ret, 0);
    UsbCtrlTransferParams ctrlParams;
    ctrlParams.requestType = 0x80;
    ctrlParams.requestCmd = 0x0006;
    ctrlParams.value = 0x0000;
    ctrlParams.index = 0x0000;
    ctrlParams.length = 18;
    ctrlParams.timeout = 0;
    std::vector<uint8_t> bufferData(ctrlParams.length);
    for (auto type : descTypes) {
        ctrlParams.value = type;
        ret = client.UsbControlTransfer(pipe, ctrlParams, bufferData);
        ASSERT_EQ(ret, 0);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbControlTransfer004 : GET_DESCRIPTOR");
}

/**
 * @tc.name: UsbControlTransfer005
 * @tc.desc: Test functions to UsbControlTransfer(
    USBDevicePipe &pipe, const HDI::Usb::V1_2::UsbCtrlTransferParams &ctrlParams, std::vector<uint8_t> &bufferData);
 * @tc.type: FUNC
 */
HWTEST_F(UsbControlTransferTest, UsbControlTransfer005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbControlTransfer005 : SYNCH_FRAME");
    auto &client = UsbSrvClient::GetInstance();
    // open device except hub
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < g_devices.size(); i++) {
        if (g_devices[i].GetClass() != USB_DEVICE_CLASS_HUB) {
            device = g_devices[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    USBDevicePipe pipe;
    client.OpenDevice(device, pipe);
    // claim interface and get endpoint
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    auto ret = client.ClaimInterface(pipe, interface, true);
    ASSERT_EQ(ret, 0);
    UsbCtrlTransferParams ctrlParams;
    ctrlParams.requestType = 0x82;
    ctrlParams.requestCmd = 0x000c;
    ctrlParams.value = 0x0000;
    ctrlParams.index = 0x0000;
    ctrlParams.length = 2;
    ctrlParams.timeout = 0;
    std::vector<uint8_t> bufferData(ctrlParams.length);
    ret = client.UsbControlTransfer(pipe, ctrlParams, bufferData);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbControlTransfer005 : SYNCH_FRAME");
}

/**
 * @tc.name: UsbControlTransfer006
 * @tc.desc: Test functions to UsbControlTransfer(
    USBDevicePipe &pipe, const HDI::Usb::V1_2::UsbCtrlTransferParams &ctrlParams, std::vector<uint8_t> &bufferData);
 * @tc.type: FUNC
 */
HWTEST_F(UsbControlTransferTest, UsbControlTransfer006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbControlTransfer006 : GET_CONFIGURATION");
    auto &client = UsbSrvClient::GetInstance();
    // open device except hub
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < g_devices.size(); i++) {
        if (g_devices[i].GetClass() != USB_DEVICE_CLASS_HUB) {
            device = g_devices[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    USBDevicePipe pipe;
    client.OpenDevice(device, pipe);
    // claim interface and get endpoint
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    auto ret = client.ClaimInterface(pipe, interface, true);
    ASSERT_EQ(ret, 0);
    // first set feature
    UsbCtrlTransferParams ctrlParams;
    ctrlParams.requestType = 0b10000000;
    ctrlParams.requestCmd = 0x08;
    ctrlParams.value = 0x00;
    ctrlParams.index = 0;
    ctrlParams.length = 1;
    ctrlParams.timeout = 0;
    std::vector<uint8_t> bufferData(ctrlParams.length);
    ret = client.UsbControlTransfer(pipe, ctrlParams, bufferData);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbControlTransfer006 : GET_CONFIGURATION");
}

/**
 * @tc.name: UsbControlTransfer007
 * @tc.desc: Test functions to UsbControlTransfer(
    USBDevicePipe &pipe, const HDI::Usb::V1_2::UsbCtrlTransferParams &ctrlParams, std::vector<uint8_t> &bufferData);
 * @tc.type: FUNC
 */
HWTEST_F(UsbControlTransferTest, UsbControlTransfer007, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbControlTransfer007 : GET_INTERFACE");
    auto &client = UsbSrvClient::GetInstance();
    // open device except hub
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < g_devices.size(); i++) {
        if (g_devices[i].GetClass() != USB_DEVICE_CLASS_HUB) {
            device = g_devices[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    USBDevicePipe pipe;
    client.OpenDevice(device, pipe);
    // claim interface and get endpoint
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    auto ret = client.ClaimInterface(pipe, interface, true);
    ASSERT_EQ(ret, 0);
    // first set feature
    UsbCtrlTransferParams ctrlParams;
    ctrlParams.requestType = 0b10000001;
    ctrlParams.requestCmd = 0x0a;
    ctrlParams.value = 0x00;
    ctrlParams.index = 0;
    ctrlParams.length = 1;
    ctrlParams.timeout = 0;
    std::vector<uint8_t> bufferData(ctrlParams.length);
    ret = client.UsbControlTransfer(pipe, ctrlParams, bufferData);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbControlTransfer007 : GET_INTERFACE");
}

/**
 * @tc.name: UsbControlTransfer008
 * @tc.desc: Test functions to UsbControlTransfer(
    USBDevicePipe &pipe, const HDI::Usb::V1_2::UsbCtrlTransferParams &ctrlParams, std::vector<uint8_t> &bufferData);
 * @tc.type: FUNC
 */
HWTEST_F(UsbControlTransferTest, UsbControlTransfer008, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbControlTransfer008 : SET_CONFIGURATION");
    auto &client = UsbSrvClient::GetInstance();
    // open device except hub
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < g_devices.size(); i++) {
        if (g_devices[i].GetClass() != USB_DEVICE_CLASS_HUB) {
            device = g_devices[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    USBDevicePipe pipe;
    client.OpenDevice(device, pipe);
    // claim interface and get endpoint
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    auto ret = client.ClaimInterface(pipe, interface, true);
    ASSERT_EQ(ret, 0);
    // first set feature
    UsbCtrlTransferParams ctrlParams;
    ctrlParams.requestType = 0b00000000;
    ctrlParams.requestCmd = 0x09;
    ctrlParams.value = 0x00;
    ctrlParams.index = 0;
    ctrlParams.length = 1;
    ctrlParams.timeout = 0;
    std::vector<uint8_t> bufferData(ctrlParams.length);
    ret = client.UsbControlTransfer(pipe, ctrlParams, bufferData);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbControlTransfer008 : SET_CONFIGURATION");
}

/**
 * @tc.name: UsbControlTransfer009
 * @tc.desc: Test functions to UsbControlTransfer(
    USBDevicePipe &pipe, const HDI::Usb::V1_2::UsbCtrlTransferParams &ctrlParams, std::vector<uint8_t> &bufferData);
 * @tc.type: FUNC
 */
HWTEST_F(UsbControlTransferTest, UsbControlTransfer009, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbControlTransfer009 : SET_INTERFACE");
    auto &client = UsbSrvClient::GetInstance();
    // open device except hub
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < g_devices.size(); i++) {
        if (g_devices[i].GetClass() != USB_DEVICE_CLASS_HUB) {
            device = g_devices[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    USBDevicePipe pipe;
    client.OpenDevice(device, pipe);
    // claim interface and get endpoint
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    auto ret = client.ClaimInterface(pipe, interface, true);
    ASSERT_EQ(ret, 0);
    // first set feature
    UsbCtrlTransferParams ctrlParams;
    ctrlParams.requestType = 0b00000001;
    ctrlParams.requestCmd = 0x0b;
    ctrlParams.value = 0x00;
    ctrlParams.index = 0;
    ctrlParams.length = 1;
    ctrlParams.timeout = 0;
    std::vector<uint8_t> bufferData(ctrlParams.length);
    ret = client.UsbControlTransfer(pipe, ctrlParams, bufferData);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbControlTransfer009 : SET_INTERFACE");
}

/**
 * @tc.name: UsbControlTransfer010
 * @tc.desc: Test functions to UsbControlTransfer(
    USBDevicePipe &pipe, const HDI::Usb::V1_2::UsbCtrlTransferParams &ctrlParams, std::vector<uint8_t> &bufferData);
 * @tc.type: FUNC
 */
HWTEST_F(UsbControlTransferTest, UsbControlTransfer010, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbControlTransfer010 : SET_DESCRIPTOR");
    const std::vector<uint8_t> descTypes = {
        0x01,   // type=0x01: device
        0x02,   // type=0x02: configuration
        0x03,   // type=0x03: string
        0x04,   // type=0x04: interface
        0x05,   // type=0x05: endpoint
        0x06,   // type=0x06: device qualifier
        0x07,   // type=0x07: other speed configuration
        0x08,   // type=0x08: interface power
    };
    auto &client = UsbSrvClient::GetInstance();
    // open device except hub
    UsbDevice device;
    bool hasDevice = false;
    for (int32_t i = 0; i < g_devices.size(); i++) {
        if (g_devices[i].GetClass() != USB_DEVICE_CLASS_HUB) {
            device = g_devices[i];
            hasDevice = true;
        }
    }
    EXPECT_TRUE(hasDevice);
    USBDevicePipe pipe;
    client.OpenDevice(device, pipe);
    // claim interface and get endpoint
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    auto ret = client.ClaimInterface(pipe, interface, true);
    ASSERT_EQ(ret, 0);
    // first set feature
    UsbCtrlTransferParams ctrlParams;
    ctrlParams.requestType = 0b00000000;
    ctrlParams.requestCmd = 0x07;
    ctrlParams.value = 0x00;
    ctrlParams.index = 0;
    ctrlParams.length = 18;
    ctrlParams.timeout = 0;
    std::vector<uint8_t> bufferData(ctrlParams.length);
    for (auto type : descTypes) {
        ctrlParams.value = type;
        ret = client.UsbControlTransfer(pipe, ctrlParams, bufferData);
        ASSERT_EQ(ret, 0);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbControlTransfer010 : SET_DESCRIPTOR");
}

} // UsbControlTransfer
} // USB
} // OHOS