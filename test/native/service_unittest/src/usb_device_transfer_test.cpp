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

#include "usb_device_transfer_test.h"

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
namespace DeviceTransfer {
constexpr int32_t USB_BUS_NUM_INVALID = -1;
constexpr int32_t USB_DEV_ADDR_INVALID = -1;
constexpr int32_t USB_DEVICE_VENDOR_ID = 0x12D1;
constexpr int32_t USB_DEVICE_PRODUCT_ID = 0xDCBA;
constexpr int32_t BUFFER_SIZE = 255;
constexpr int32_t USB_HUB_DEVICE = 255;
constexpr int32_t TEN = 10;
constexpr int32_t TRANSFER_TIMEOUT = 2;
constexpr int32_t BUFFER_SIZE_16M_LEN = 16 * 1024 * 1024;
constexpr int32_t BUFFER_SIZE_190K_LEN = 190 * 1024;
constexpr int32_t BUFFER_SIZE_200K_LEN = 200 * 1024;
bool GetSpecifiedDevice(vector<UsbDevice> &delist, UsbDevice &device) 
{
    bool hasDevice = false;
    for (int32_t i = 0; i < delist.size(); i++) {
        if (delist[i].GetClass() == USB_HUB_DEVICE) {
            continue;
        }
        
        if (delist[i].GetVendorId() == USB_DEVICE_VENDOR_ID &&
            delist[i].GetVendorId() == USB_DEVICE_PRODUCT_ID) {
            device = delist[i];
            hasDevice = true;
        }
    }
    return hasDevice;
}

bool GetBulkEp(std::vector<USBEndpoint> &eps, USBEndpoint &epOut, USBEndpoint &epIn)
{
    bool hasEpOut = false;
    bool hasEpIn = false;
    for (int32_t i = 0; i < eps.size(); i++) {
        if (eps[i].GetDirection() == USB_ENDPOINT_DIR_OUT &&
            eps[i].GetType() == USB_ENDPOINT_XFER_BULK) {
            hasEpOut = true;
            epOut = eps[i];
            continue;
        } else if (eps[i].GetDirection() == USB_ENDPOINT_DIR_IN &&
            eps[i].GetType() == USB_ENDPOINT_XFER_BULK) {
            hasEpOut = true;
            epIn = eps[i];
            continue;
        }
        
    }
    return hasEpOut && hasEpIn;
}

void UsbDeviceTransferTest::SetUpTestCase(void)
{
    UsbCommonTest::GrantPermissionSysNative();
    std::cout << "please connect device, press enter to continue" << std::endl;
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
    USB_HILOGI(MODULE_USB_SERVICE, "Start UsbDeviceTransferTest");
}

void UsbDeviceTransferTest::TearDownTestCase(void)
{
    USB_HILOGI(MODULE_USB_SERVICE, "End UsbDeviceTransferTest");
}

void UsbDeviceTransferTest::SetUp(void) {}

void UsbDeviceTransferTest::TearDown(void) {}

/**
 * @tc.name: Usbcontrolstansfer001
 * @tc.desc: Test functions to ControlTransfer read buffer size 0
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, Usbcontrolstansfer001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer001 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer001 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer001 %{public}d size=%{public}zu",
        __LINE__, delist.size());
    USBDevicePipe pipe;
    UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer001 %{public}d Open=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    struct UsbCtrlTransferParams ctrldata = {0b11000000, 0xA1, 0, 0x02, 0, 1000};
    std::vector<uint8_t> ctrlbuffer;;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer001 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(ctrlbuffer.size(), 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer001 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer002
 * @tc.desc: Test functions to ControlTransfer write buffer size 0
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, Usbcontrolstansfer002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer002 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer002 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer002 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
    UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer002 %{public}d Open=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA1, 0, 0x03, 0, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer002 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(ctrlbuffer.size(), 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer002 : ControlTransfer");
}


/**
 * @tc.name: Usbcontrolstansfer003
 * @tc.desc: Test functions to ControlTransfer read buffer
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, Usbcontrolstansfer003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer003 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer003 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer003 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
    UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer003 %{public}d Open=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    uint32_t len = 8;
    uint8_t buffer[BUFFER_SIZE] = {0};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    struct UsbCtrlTransferParams ctrldata = {0b11000000, 0xA1, 0, 0x02, len, 1000};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer003 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(ctrlbuffer.size(), len);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer003 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer004
 * @tc.desc: Test functions to ControlTransfer write buffer size 0
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, Usbcontrolstansfer004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer004 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer004 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer004 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
    UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer004 %{public}d Open=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint32_t len = 8;
    uint8_t buffer[BUFFER_SIZE] = {0};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA1, 0, 0x03, len, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer004 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(ctrlbuffer.size(), len);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer004 : ControlTransfer");
}


/**
 * @tc.name: Usbcontrolstansfer005
 * @tc.desc: Test functions to ControlTransfer read timeout
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, Usbcontrolstansfer005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer005 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer005 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer005 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
    UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer005 %{public}d Open=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    uint32_t len = 8;
    uint8_t buffer[BUFFER_SIZE] = {0};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    struct UsbCtrlTransferParams ctrldata = {0b11000000, 0xA1, 1000, 0x02, len, 1000};
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer005 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_NE(ret, 0);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer005 : ControlTransfer");
}

/**
 * @tc.name: Usbcontrolstansfer006
 * @tc.desc: Test functions to ControlTransfer write timeout
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, Usbcontrolstansfer006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Usbcontrolstansfer006 : ControlTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer006 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer006 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
    UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer006 %{public}d Open=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    uint32_t len = 8;
    uint8_t buffer[BUFFER_SIZE] = {0};
    std::vector<uint8_t> ctrlbuffer = {buffer, buffer + len};
    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA1, 100, 0x03, len, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Usbcontrolstansfer006 %{public}d ControlTransfer=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(ctrlbuffer.size(), len);
    ret = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbDeviceTransferTest::Close=%{public}d", ret);
    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Usbcontrolstansfer006 : ControlTransfer");
}

/**
 * @tc.name: UsbBulkTransfer001
 * @tc.desc: Test functions to BulkTransfer read buffer size 0
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbBulkTransfer001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer001 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer001 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer001 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer001 %{public}d Open=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest %{public}d  ep out point=%{public}d, ep in point=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer001 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x02, 0, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer001 %{public}d control%{public}d", __LINE__,
            ret);
    ASSERT_EQ(ret, 0);

    std::vector<uint8_t> bulkbuffer;
    ret = UsbSrvClient.BulkTransfer(pipe, epIn, bulkbuffer, 3000);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer001 %{public}d BulkTransfer=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(bulkbuffer.size(), 0);
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer001 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer001 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer001 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer002
 * @tc.desc: Test functions to BulkTransfer write buffer size 0
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbBulkTransfer002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer002 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer002 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer002 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer002 %{public}d Open=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest %{public}d  ep out point=%{public}d, ep in point=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer002 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x03, 0, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer002 %{public}d control%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);

    std::vector<uint8_t> bulkbuffer;
    ret = UsbSrvClient.BulkTransfer(pipe, epOut, bulkbuffer, 3000);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer002 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(bulkbuffer.size(), 0);
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer002 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer002 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer002 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer003
 * @tc.desc: Test functions to BulkTransfer read buffer size BUFFER_SIZE
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbBulkTransfer003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer003 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer003 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer003 %{public}d size=%{public}zu",
        __LINE__, delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer003 %{public}d Open=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "UsbBulkTransfer003 %{public}d  ep out=%{public}d, ep in=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer003 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x02, 10, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer003 %{public}d control%{public}d", __LINE__,
            ret);
    ASSERT_EQ(ret, 0);

    uint8_t buffer[BUFFER_SIZE] = "bulk read";
    uint32_t len = BUFFER_SIZE;
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, epIn, bulkbuffer, 3000);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer003 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(bulkbuffer.size(), 10);

    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer003 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer003 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer003 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer004
 * @tc.desc: Test functions to BulkTransfer write buffer size BUFFER_SIZE
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbBulkTransfer004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer004 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer004 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer004 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer004 %{public}d Open=%{public}d", __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "UsbBulkTransfer004 %{public}d  ep out=%{public}d, ep in=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer004 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x03, 0, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer004 %{public}d control%{public}d", __LINE__,
            ret);
    ASSERT_EQ(ret, 0);

    uint8_t buffer[BUFFER_SIZE] = "bulk_wriet.";
    uint32_t len = BUFFER_SIZE;
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, epOut, bulkbuffer, 3000);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer004 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(bulkbuffer.size(), BUFFER_SIZE);
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer004 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer004 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer004 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer005
 * @tc.desc: Test functions to BulkTransfer read buffer timeout
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbBulkTransfer005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer005 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer005 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer005 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer005 %{public}d Open=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "UsbBulkTransfer005 %{public}d  ep out=%{public}d, ep in=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer005 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 1000, 0x04, 0, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer005 %{public}d control=%{public}d", __LINE__,
            ret);
    ASSERT_EQ(ret, 0);

    uint8_t buffer[BUFFER_SIZE] = "bulk read";
    uint32_t len = BUFFER_SIZE;
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, epIn, bulkbuffer, 3000);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer005 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_NE(ret, 0);

    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer005 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer005 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer005 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer006
 * @tc.desc: Test functions to BulkTransfer write buffer timeout
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbBulkTransfer006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer006 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer006 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer006 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer006 %{public}d Open=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "UsbBulkTransfer006 %{public}d  ep out=%{public}d, ep in=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer006 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    
    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 1000, 0x03, 0, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer006 %{public}d control%{public}d", __LINE__,
            ret);
    ASSERT_EQ(ret, 0);

    uint8_t buffer[BUFFER_SIZE] = "bulk_wriet.";
    uint32_t len = BUFFER_SIZE;
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, epOut, bulkbuffer, 3000);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer006 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_NE(ret, 0);

    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer006 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer006 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer006 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer007
 * @tc.desc: Test functions to BulkTransfer read buffer size BUFFER_SIZE_190K_LEN
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbBulkTransfer007, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer007 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer007 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer007 %{public}d size=%{public}zu",
        __LINE__, delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer007 %{public}d Open=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "UsbBulkTransfer007 %{public}d  ep out=%{public}d, ep in=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer007 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x02, 10, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer007 %{public}d control%{public}d", __LINE__,
            ret);
    ASSERT_EQ(ret, 0);

    uint8_t buffer[BUFFER_SIZE_190K_LEN] = "bulk read";
    uint32_t len = BUFFER_SIZE_190K_LEN;
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, epIn, bulkbuffer, 3000);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer007 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(bulkbuffer.size(), 10);

    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer007 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer007 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer007 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer008
 * @tc.desc: Test functions to BulkTransfer write buffer size BUFFER_SIZE_190K_LEN
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbBulkTransfer008, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer008 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer008 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer008 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer008 %{public}d Open=%{public}d", __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "UsbBulkTransfer008 %{public}d  ep out=%{public}d, ep in=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer008 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x03, 0, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer008 %{public}d control%{public}d", __LINE__,
            ret);
    ASSERT_EQ(ret, 0);

    uint8_t buffer[BUFFER_SIZE_190K_LEN] = "bulk_wriet.";
    uint32_t len = BUFFER_SIZE_190K_LEN;
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, epOut, bulkbuffer, 3000);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer008 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(bulkbuffer.size(), BUFFER_SIZE_190K_LEN);
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer008 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer008 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer008 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer009
 * @tc.desc: Test functions to BulkTransfer read buffer size BUFFER_SIZE_200K_LEN
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbBulkTransfer009, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer009 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer009 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer009 %{public}d size=%{public}zu",
        __LINE__, delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer009 %{public}d Open=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "UsbBulkTransfer009 %{public}d  ep out=%{public}d, ep in=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer009 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x02, 10, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer009 %{public}d control%{public}d", __LINE__,
            ret);
    ASSERT_EQ(ret, 0);

    uint8_t buffer[BUFFER_SIZE_200K_LEN] = "bulk read";
    uint32_t len = BUFFER_SIZE_200K_LEN;
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, epIn, bulkbuffer, 3000);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer009 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(bulkbuffer.size(), 10);

    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer009 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer009 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer009 : BulkTransfer");
}

/**
 * @tc.name: UsbBulkTransfer010
 * @tc.desc: Test functions to BulkTransfer write buffer size BUFFER_SIZE_200K_LEN
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbBulkTransfer010, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbBulkTransfer010 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer010 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer010 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer010 %{public}d Open=%{public}d", __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "UsbBulkTransfer010 %{public}d  ep out=%{public}d, ep in=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer010 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x03, 0, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer010 %{public}d control%{public}d", __LINE__,
            ret);
    ASSERT_EQ(ret, 0);

    uint8_t buffer[BUFFER_SIZE_200K_LEN] = "bulk_wriet.";
    uint32_t len = BUFFER_SIZE_200K_LEN;
    std::vector<uint8_t> bulkbuffer = {buffer, buffer + len};
    ret = UsbSrvClient.BulkTransfer(pipe, epOut, bulkbuffer, 3000);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer010 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(bulkbuffer.size(), BUFFER_SIZE_200K_LEN);
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer010 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbBulkTransfer010 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbBulkTransfer010 : BulkTransfer");
}

/**
 * @tc.name: UsbSubmitTransfer001
 * @tc.desc: Test functions to SubmitTransfer Bulk read buffer size 0
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbSubmitBulkTransfer001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbSubmitBulkTransfer001 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer001 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer001 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer001 %{public}d Open=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest %{public}d  ep out point=%{public}d, ep in point=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer001 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x02, 0, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer001 %{public}d control%{public}d", __LINE__,
            ret);
    ASSERT_EQ(ret, 0);

    std::vector<uint8_t> bulkbuffer;
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", BUFFER_SIZE);
    ASSERT_NE(ashmem, nullptr);
    ashmem->MapReadAndWriteAshmem();

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = epIn.GetAddress();    // 0x01写 0x81读
    transferInfo.flags = 0;
    transferInfo.type = TYPE_BULK;
    transferInfo.timeOut = 2000;
    transferInfo.length = 0;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192

    std::promise<std::pair<int, int>> promise;
    std::futrue<std::pair<int, int>> future = promise.get_future();
    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitBulkTransfer001 cb status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
        promise.set_value({info.status, info.actualLength});
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pipe, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer001 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);

    auto result = future.get();
    ASSERT_EQ(result.first, 0);
    ASSERT_EQ(result.second, 0);
    ashmem->UnmapAshmem();
    ashmem->CloseAshmem();
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer001 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer001 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbSubmitBulkTransfer001 : BulkTransfer");
}

/**
 * @tc.name: UsbSubmitTransfer002
 * @tc.desc: Test functions to SubmitTransfer Bulk write buffer size 0
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbSubmitBulkTransfer002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbSubmitBulkTransfer002 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer002 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer002 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
    UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer002 %{public}d Open=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest %{public}d  ep out point=%{public}d, ep in point=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer002 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x03, 0, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer002 %{public}d control%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);

    std::vector<uint8_t> bulkbuffer;
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", BUFFER_SIZE);
    ASSERT_NE(ashmem, nullptr);
    ashmem->MapReadAndWriteAshmem();

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = epOut.GetAddress();    // 0x01写 0x81读
    transferInfo.flags = 0;
    transferInfo.type = TYPE_BULK;
    transferInfo.timeOut = 2000;
    transferInfo.length = 0;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192

    std::promise<std::pair<int, int>> promise;
    std::futrue<std::pair<int, int>> future = promise.get_future();
    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitBulkTransfer002 cb status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
        promise.set_value({info.status, info.actualLength});
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pipe, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer002 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);

    auto result = future.get();
    ASSERT_EQ(result.first, 0);
    ASSERT_EQ(result.second, 0);

    ashmem->UnmapAshmem();
    ashmem->CloseAshmem();
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer002 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer002 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbSubmitBulkTransfer002 : BulkTransfer");
}

/**
 * @tc.name: UsbSubmitTransfer001
 * @tc.desc: Test functions to SubmitTransfer Bulk read buffer size 10
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbSubmitBulkTransfer003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbSubmitBulkTransfer003 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer003 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer003 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer003 %{public}d Open=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest %{public}d  ep out point=%{public}d, ep in point=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer003 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x02, TEN, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer003 %{public}d control%{public}d", __LINE__,
            ret);
    ASSERT_EQ(ret, 0);

    const uint8_t dataToRead[TEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", BUFFER_SIZE);
    ASSERT_NE(ashmem, nullptr);
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToRead, sizeof(dataToRead), 0);
    ASSERT_TRUE(writeSuccess);

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = epIn.GetAddress();    // 0x01写 0x81读
    transferInfo.flags = 0;
    transferInfo.type = TYPE_BULK;
    transferInfo.timeOut = 2000;
    transferInfo.length = TEN;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192

    std::promise<std::pair<int, int>> promise;
    std::futrue<std::pair<int, int>> future = promise.get_future();
    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitBulkTransfer003 cb status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
        promise.set_value({info.status, info.actualLength});
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pipe, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer003 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);

    auto result = future.get();
    ASSERT_EQ(result.first, 0);
    ASSERT_EQ(result.second, TEN);
    
    ashmem->UnmapAshmem();
    ashmem->CloseAshmem();
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer003 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer003 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbSubmitBulkTransfer003 : BulkTransfer");
}

/**
 * @tc.name: UsbSubmitTransfer002
 * @tc.desc: Test functions to SubmitTransfer Bulk write buffer size 10
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbSubmitBulkTransfer002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbSubmitBulkTransfer002 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer002 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer002 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
    UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer002 %{public}d Open=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest %{public}d  ep out point=%{public}d, ep in point=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer002 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x03, TEN, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer002 %{public}d control%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);

    const uint8_t dataToWrite[TEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", BUFFER_SIZE);
    ASSERT_NE(ashmem, nullptr);
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, sizeof(dataToWrite), 0);
    ASSERT_TRUE(writeSuccess);

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = epOut.GetAddress();    // 0x01写 0x81读
    transferInfo.flags = 0;
    transferInfo.type = TYPE_BULK;
    transferInfo.timeOut = 2000;
    transferInfo.length = TEN;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192

    std::promise<std::pair<int, int>> promise;
    std::futrue<std::pair<int, int>> future = promise.get_future();
    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitBulkTransfer002 cb status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
        promise.set_value({info.status, info.actualLength});
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pipe, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer002 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);

    auto result = future.get();
    ASSERT_EQ(result.first, 0);
    ASSERT_EQ(result.second, TEN);

    ashmem->UnmapAshmem();
    ashmem->CloseAshmem();
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer002 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer002 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbSubmitBulkTransfer002 : BulkTransfer");
}

/**
 * @tc.name: UsbSubmitTransfer001
 * @tc.desc: Test functions to SubmitTransfer Bulk read buffer size 10 timeout
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbSubmitBulkTransfer005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbSubmitBulkTransfer005 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer005 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer005 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
        UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer005 %{public}d Open=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest %{public}d  ep out point=%{public}d, ep in point=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer005 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    
    // 0x04, bulk read, 3000 process times
    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x03, 3000, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer005 %{public}d control%{public}d", __LINE__,
            ret);
    ASSERT_EQ(ret, 0);

    const uint8_t dataToRead[TEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", BUFFER_SIZE);
    ASSERT_NE(ashmem, nullptr);
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToRead, sizeof(dataToRead), 0);
    ASSERT_TRUE(writeSuccess);

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = epIn.GetAddress();    // 0x01写 0x81读
    transferInfo.flags = 0;
    transferInfo.type = TYPE_BULK;
    transferInfo.timeOut = 2000;
    transferInfo.length = TEN;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192

    std::promise<std::pair<int, int>> promise;
    std::futrue<std::pair<int, int>> future = promise.get_future();
    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitBulkTransfer005 cb status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
        promise.set_value({info.status, info.actualLength});
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pipe, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer005 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);

    auto result = future.get();
    // status timeout
    ASSERT_EQ(result.first, TRANSFER_TIMEOUT);
    ASSERT_EQ(result.second, 0);
    
    ashmem->UnmapAshmem();
    ashmem->CloseAshmem();
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer005 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer005 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbSubmitBulkTransfer005 : BulkTransfer");
}

/**
 * @tc.name: UsbSubmitTransfer002
 * @tc.desc: Test functions to SubmitTransfer Bulk write buffer size 10 timeout
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbSubmitBulkTransfer006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbSubmitBulkTransfer006 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer006 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer006 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
    UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer006 %{public}d Open=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest %{public}d  ep out point=%{public}d, ep in point=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer006 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    // 0x03, bulk write, 3000 process times
    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x03, 3000, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer006 %{public}d control%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);

    const uint8_t dataToWrite[TEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", BUFFER_SIZE);
    ASSERT_NE(ashmem, nullptr);
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, sizeof(dataToWrite), 0);
    ASSERT_TRUE(writeSuccess);

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = epOut.GetAddress();    // 0x01写 0x81读
    transferInfo.flags = 0;
    transferInfo.type = TYPE_BULK;
    transferInfo.timeOut = 2000;
    transferInfo.length = TEN;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192

    std::promise<std::pair<int, int>> promise;
    std::futrue<std::pair<int, int>> future = promise.get_future();
    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitBulkTransfer006 cb status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
        promise.set_value({info.status, info.actualLength});
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pipe, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer006 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);

    auto result = future.get();
    ASSERT_EQ(result.first, TRANSFER_TIMEOUT);
    ASSERT_EQ(result.second, 0);

    ashmem->UnmapAshmem();
    ashmem->CloseAshmem();
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer006 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer006 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbSubmitBulkTransfer006 : BulkTransfer");
}

/**
 * @tc.name: UsbSubmitTransfer007
 * @tc.desc: Test functions to SubmitTransfer Bulk read buffer size 16M
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbSubmitTransfer007, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbSubmitTransfer007 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitTransfer007 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitTransfer007 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
    UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitTransfer007 %{public}d Open=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest %{public}d ep out point=%{public}d, ep in point=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitTransfer007 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x02, 0, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitTransfer007 %{public}d control%{public}d", __LINE__,
            ret);
    ASSERT_EQ(ret, 0);

    std::vector<uint8_t> bulkbuffer(BUFFER_SIZE_16M_LEN, 'a');
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", BUFFER_SIZE_16M_LEN);
    ASSERT_NE(ashmem, nullptr);
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(bulkbuffer.data(), BUFFER_SIZE_16M_LEN, 0);

    ASSERT_TRUE(writeSuccess);
    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = epIn.GetAddress();    // 0x01写 0x81读
    transferInfo.flags = 0;
    transferInfo.type = TYPE_BULK;
    transferInfo.timeOut = 2000;
    transferInfo.length = BUFFER_SIZE_16M_LEN;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192

    std::promise<std::pair<int, int>> promise;
    std::futrue<std::pair<int, int>> future = promise.get_future();
    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransfer007 cb status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
        promise.set_value({info.status, info.actualLength});
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pipe, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitTransfer007 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    //报错，mem out
    ASSERT_NE(ret, 0);

    ashmem->UnmapAshmem();
    ashmem->CloseAshmem();
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitTransfer007 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitTransfer007 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbSubmitTransfer007 : BulkTransfer");
}

/**
 * @tc.name: UsbSubmitTransfer002
 * @tc.desc: Test functions to SubmitTransfer Bulk write buffer size 0
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbSubmitBulkTransfer008, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbSubmitBulkTransfer008 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer008 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer008 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
    UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer008 %{public}d Open=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest %{public}d ep out point=%{public}d, ep in point=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer008 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x03, 0, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer008 %{public}d control%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);

    std::vector<uint8_t> bulkbuffer(BUFFER_SIZE_16M_LEN, 'a');
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", BUFFER_SIZE_16M_LEN);
    ASSERT_NE(ashmem, nullptr);
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(bulkbuffer.data(), sizeof(BUFFER_SIZE_16M_LEN), 0);
    ASSERT_TRUE(writeSuccess);
    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = epOut.GetAddress();    // 0x01写 0x81读
    transferInfo.flags = 0;
    transferInfo.type = TYPE_BULK;
    transferInfo.timeOut = 2000;
    transferInfo.length = 0;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192

    std::promise<std::pair<int, int>> promise;
    std::futrue<std::pair<int, int>> future = promise.get_future();
    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitBulkTransfer008 cb status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
        promise.set_value({info.status, info.actualLength});
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pipe, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer008 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_NE(ret, 0);

    ashmem->UnmapAshmem();
    ashmem->CloseAshmem();
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer008 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer008 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbSubmitBulkTransfer008 : BulkTransfer");
}

/**
 * @tc.name: UsbSubmitBulkTransfer009
 * @tc.desc: Test functions to SubmitTransfer Bulk read buffer size 16M -1
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbSubmitBulkTransfer009, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbSubmitBulkTransfer009 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer009 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer009 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
    UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer009 %{public}d Open=%{public}d", __LINE__,
               ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest %{public}d  ep out point=%{public}d, ep in point=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer009 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x02, 0, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer009 %{public}d control%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);

    std::vector<uint8_t> bulkbuffer(BUFFER_SIZE_16M_LEN, 'a');
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", BUFFER_SIZE_16M_LEN - 1);
    ASSERT_NE(ashmem, nullptr);
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(bulkbuffer.data(), BUFFER_SIZE_16M_LEN - 1, 0);

    ASSERT_TRUE(writeSuccess);
    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = epIn.GetAddress();    // 0x01写 0x81读
    transferInfo.flags = 0;
    transferInfo.type = TYPE_BULK;
    transferInfo.timeOut = 2000;
    transferInfo.length = BUFFER_SIZE_16M_LEN - 1;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192

    std::promise<std::pair<int, int>> promise;
    std::futrue<std::pair<int, int>> future = promise.get_future();
    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitBulkTransfer009 cb status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
        promise.set_value({info.status, info.actualLength});
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pipe, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer009 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    //报错，mem out
    ASSERT_NE(ret, 0);

    ashmem->UnmapAshmem();
    ashmem->CloseAshmem();
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer009 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer009 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbSubmitBulkTransfer009 : BulkTransfer");
}

/**
 * @tc.name: UsbSubmitTransfer002
 * @tc.desc: Test functions to SubmitTransfer Bulk write buffer size 16M -1
 * @tc.type: FUNC
 */
HWTEST_F(UsbDeviceTransferTest, UsbSubmitBulkTransfer010, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbSubmitBulkTransfer010 : BulkTransfer");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer010 %{public}d ret=%{public}d",
        __LINE__, ret);
    EXPECT_TRUE(!(delist.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer010 %{public}d size=%{public}zu", __LINE__,
               delist.size());
    USBDevicePipe pipe;
    UsbDevice device;
    bool hasDevice = false;
    hasDevice = GetSpecifiedDevice(delist, device);
    EXPECT_TRUE(hasDevice);
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer010 %{public}d Open=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint epOut;
    USBEndpoint epIn;
    bool hasEp = GetBulkEp(interface.GetEndpoints(), epOut, epIn);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest %{public}d  ep out point=%{public}d, ep in point=%{public}d",
        __LINE__, epOut.GetInterfaceId(), epIn.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer010 %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);

    struct UsbCtrlTransferParams ctrldata = {0b01000000, 0xA2, 0, 0x03, 0, 1000};
    std::vector<uint8_t> ctrlbuffer;
    ret = UsbSrvClient.UsbControlTransfer(pipe, ctrldata, ctrlbuffer);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer010 %{public}d control%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);

    std::vector<uint8_t> bulkbuffer(BUFFER_SIZE_16M_LEN -1, 'a');
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", BUFFER_SIZE_16M_LEN -1);
    ASSERT_NE(ashmem, nullptr);
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(bulkbuffer.data(), sizeof(BUFFER_SIZE_16M_LEN - 1), 0);
    ASSERT_TRUE(writeSuccess);
    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = epOut.GetAddress();    // 0x01写 0x81读
    transferInfo.flags = 0;
    transferInfo.type = TYPE_BULK;
    transferInfo.timeOut = 2000;
    transferInfo.length = BUFFER_SIZE_16M_LEN -1;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192

    std::promise<std::pair<int, int>> promise;
    std::futrue<std::pair<int, int>> future = promise.get_future();
    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitBulkTransfer010 cb status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
        promise.set_value({info.status, info.actualLength});
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pipe, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer010 %{public}d BulkTransfer=%{public}d",
        __LINE__, ret);
    ASSERT_NE(ret, 0);

    ashmem->UnmapAshmem();
    ashmem->CloseAshmem();
    ret = UsbSrvClient.ReleaseInterface(pipe, interface);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer010 %{public}d ReleaseInterface=%{public}d",
               __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "TransferTest::UsbSubmitBulkTransfer010 %{public}d close=%{public}d", __LINE__,
               close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbSubmitBulkTransfer010 : BulkTransfer");
}

} // UsbDeviceTransferTest
} // USB
} // OHOS
