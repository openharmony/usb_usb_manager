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

#include "usb_submit_transfer_test.h"

#include <csignal>
#include <iostream>
#include <vector>

#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "iusb_srv.h"
#include "system_ability_definition.h"
#include "usb_common.h"
#include "usb_common_test.h"
#include "usb_device_pipe.h"
#include "usb_request.h"
#include "usb_srv_client.h"
#include "v1_2/usb_types.h"
#include "usb_errors.h"
#include "ashmem.h"

using namespace testing::ext;
using namespace OHOS::USB;
using namespace OHOS;
using namespace std;
using namespace OHOS::USB::Common;

namespace OHOS {
namespace USB {
namespace SubmitTransfer {
constexpr int32_t SLEEP_TIME = 3;

void UsbSubmitTransferTest::SetUpTestCase(void)
{
    UsbCommonTest::GrantPermissionSysNative();
    auto &srvClient = UsbSrvClient::GetInstance();
    auto ret = srvClient.SetPortRole(1, 1, 1);
    sleep(SLEEP_TIME);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferTest:: [Device] SetPortRole:%{public}d", ret);
    ret = UsbCommonTest::SwitchErrCode(ret);
    ASSERT_TRUE(ret == 0);
    if (ret != 0) {
        exit(0);
    }
    std::cout << "please connect device, press enter to continue" << std::endl;
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
    USB_HILOGI(MODULE_USB_SERVICE, "Start UsbSubmitTransferTest");
}

void UsbSubmitTransferTest::TearDownTestCase(void)
{
    USB_HILOGI(MODULE_USB_SERVICE, "End UsbSubmitTransferTest");
}

void UsbSubmitTransferTest::SetUp(void) {}

void UsbSubmitTransferTest::TearDown(void) {}
 
/**
 * @tc.name: UsbSubmitTransferBulkWrite
 * @tc.desc: Test the USB data write functionality of UsbSubmitTransfer OK.
 * @tc.type: FUNC
 */
HWTEST_F(UsbSubmitTransferTest, UsbSubmitTransferBulkWrite, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWrite enter.");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    EXPECT_TRUE(!(delist.empty())) << "Device list is empty";
    UsbDevice device = delist.front();
    ret = UsbSrvClient.RequestRight(device.GetName());
    ASSERT_EQ(ret, 0);
    USBDevicePipe pip;
    ret = UsbSrvClient.OpenDevice(device, pip);
    ASSERT_EQ(ret, 0);

    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.ClaimInterface(pip, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferBulkWrite ret:%{public}d", __LINE__, ret);
    ASSERT_EQ(ret, 0);

    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", 10);
    ASSERT_NE(ashmem, nullptr);
    const uint8_t dataToWrite[10] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, sizeof(dataToWrite), 0);
    ASSERT_TRUE(writeSuccess);

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0x01;    // 0x01写 0x81读
    transferInfo.flags = 0;
    transferInfo.type = 2;           // 开发板仅支持bulk
    transferInfo.timeOut = 2000;
    transferInfo.length = 10;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192

    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        ASSERT_EQ(info.status, UEC_OK);
        USB_HILOGI(MODULE_USB_SERVICE,
            "UsbSubmitTransferBulkWrite Callback status: %{public}d, actualLength: %{public}d",
            info.status, info.actualLength);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWrite Callback packet size: %{public}d", packets.size());
    };

    ret = UsbSrvClient.UsbSubmitTransfer(pip, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferBulkWrite ret:%{public}d", __LINE__, ret);
    ASSERT_EQ(ret, UEC_OK);
    bool close = UsbSrvClient.Close(pip);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWrite end.");
}

/**
 * @tc.name: UsbSubmitTransferBulkReadTimeOut
 * @tc.desc: Test the USB data read functionality of UsbSubmitTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbSubmitTransferTest, UsbSubmitTransferBulkReadTimeOut, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkReadTimeOut enter.");

    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    EXPECT_TRUE(!(delist.empty())) << "Device list is empty";

    UsbDevice device = delist.front();
    ret = UsbSrvClient.RequestRight(device.GetName());
    ASSERT_EQ(ret, 0);

    USBDevicePipe pip;
    ret = UsbSrvClient.OpenDevice(device, pip);
    ASSERT_EQ(ret, 0);

    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.ClaimInterface(pip, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkReadTimeOut ClaimInterface ret%{public}d", ret);
    ASSERT_EQ(ret, 0);

    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", 10);
    ASSERT_NE(ashmem, nullptr);
    ashmem->MapReadAndWriteAshmem();

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0x81;    // 0x01写 0x81读
    transferInfo.flags = 0;
    transferInfo.type = 2;           // 开发板仅支持bulk
    transferInfo.timeOut = 2000;
    transferInfo.length = 10;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192

    auto callback = [](const TransferCallbackInfo &info,
                            const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE,
            "UsbSubmitTransferBulkReadTimeOut Callback status: %{public}d, actualLength: %{public}d",
            info.status, info.actualLength);

        USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkReadTimeOut Callback packet: %{public}d", packets.size());
        int32_t errCode;
        if (info.status == 2) {
            errCode = TIMEOUT_ERROR;
        }
        ASSERT_EQ(errCode, USB_SUBMIT_TRANSFER_TIMEOUT_ERROR);
    };

    ret = UsbSrvClient.UsbSubmitTransfer(pip, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferBulkReadTimeOut ret:%{public}d", __LINE__, ret);
    ASSERT_EQ(ret, UEC_OK);

    bool close = UsbSrvClient.Close(pip);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkReadTimeOut end.");
}


/**
 * @tc.name: UsbSubmitTransferBulkWriteInvalidEndpoint
 * @tc.desc: Test the USB data write functionality of UsbSubmitTransfer with invalid endpoint type
 * @tc.type: FUNC
 */
HWTEST_F(UsbSubmitTransferTest, UsbSubmitTransferBulkWriteInvalidEndpoint, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWriteInvalidEndpoint begin.");

    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    EXPECT_TRUE(!(delist.empty())) << "Device list is empty";
    UsbDevice device = delist.front();
    ret = UsbSrvClient.RequestRight(device.GetName());
    ASSERT_EQ(ret, 0);
    USBDevicePipe pip;
    ret = UsbSrvClient.OpenDevice(device, pip);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.ClaimInterface(pip, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE,
        "%{public}d line. UsbSubmitTransferBulkWriteInvalidEndpoint ret:%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);

    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", 10);
    ASSERT_NE(ashmem, nullptr);
    const uint8_t dataToWrite[10] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, sizeof(dataToWrite), 0);
    ASSERT_TRUE(writeSuccess);
    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0xFF;    //无效参数
    transferInfo.flags = 0;
    transferInfo.type = 2;           // 开发板仅支持bulk
    transferInfo.timeOut = 2000;
    transferInfo.length = 10;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192

    auto callback = [](const TransferCallbackInfo &info,
                            const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE,
            "UsbSubmitTransferBulkWriteInvalidEndpoint cb status: %{public}d, actualLength: %{public}d",
            info.status, info.actualLength);
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pip, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferBulkWriteInvalidEndpoint ret=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, USB_SUBMIT_TRANSFER_INVALID_PARAM);
    bool close = UsbSrvClient.Close(pip);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWriteInvalidEndpoint end.");
}

/**
 * @tc.name:  UsbSubmitTransferBulkWriteIoError
 * @tc.desc: Test USB I/O error when interface is not claimed.
 * @tc.type: FUNC
 */
HWTEST_F(UsbSubmitTransferTest, UsbSubmitTransferBulkWriteIoError, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWriteIoError begin.");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    EXPECT_TRUE(!(delist.empty())) << "Device list is empty";
    UsbDevice device = delist.front();
    ret = UsbSrvClient.RequestRight(device.GetName());
    ASSERT_EQ(ret, 0);
    USBDevicePipe pip;
    ret = UsbSrvClient.OpenDevice(device, pip);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().front();

    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", 10);
    ASSERT_NE(ashmem, nullptr);
    const uint8_t dataToWrite[10] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, sizeof(dataToWrite), 0);
    ASSERT_TRUE(writeSuccess);

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0x01;    // 写操作（对于开发板，0x81是读操作）
    transferInfo.flags = 0;
    transferInfo.type = 2;           // 开发板仅支持bulk
    transferInfo.timeOut = 2000;
    transferInfo.length = 10;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192
    auto callback = [](const TransferCallbackInfo &info,
                            const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWriteIoError status: %{public}d, actualLength: %{public}d",
            info.status, info.actualLength);
        ASSERT_EQ(info.status, UEC_OK);
    };

    ret = UsbSrvClient.UsbSubmitTransfer(pip, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferBulkWriteIoError ret:%{public}d", __LINE__, ret);
    ASSERT_EQ(ret,  USB_SUBMIT_TRANSFER_IO_ERROR);
    bool close = UsbSrvClient.Close(pip);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWriteIoError end.");
}

/**
 * @tc.name: UsbCancelTransferBulkWrite
 * @tc.desc: Test the functionality of UsbCancelTransfer for write operation
 * @tc.type: FUNC
 */
HWTEST_F(UsbSubmitTransferTest, UsbCancelTransferBulkWrite, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCancelTransferBulkWrite enter.");

    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    EXPECT_TRUE(!(delist.empty())) << "Device list is empty";

    UsbDevice device = delist.front();
    ret = UsbSrvClient.RequestRight(device.GetName());
    ASSERT_EQ(ret, 0);

    USBDevicePipe pip;
    ret = UsbSrvClient.OpenDevice(device, pip);
    ASSERT_EQ(ret, 0);

    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.ClaimInterface(pip, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE,
        "UsbCancelTransferBulkWrite ClaimInterface %{public}d ret:%{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);

    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", 10);
    ASSERT_NE(ashmem, nullptr);
    const uint8_t dataToWrite[10] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, 10, 0);
    ASSERT_TRUE(writeSuccess);

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0x01;    // 写操作
    transferInfo.flags = 0;
    transferInfo.type = 2;           // 开发板不支持ISO传输类型
    transferInfo.timeOut = 0;        // 设置超时时间
    transferInfo.length = 10;        // 设置传输数据的长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // 只有type为1有iso

    auto callback = [](const TransferCallbackInfo &info,
                                const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE,
            "UsbCancelTransferBulkWrite Callback status: %{public}d, actualLength: %{public}d",
            info.status, info.actualLength);
        ASSERT_EQ(info.status, UEC_OK);
    };

    ret = UsbSrvClient.UsbSubmitTransfer(pip, transferInfo, callback, ashmem);
    ASSERT_EQ(ret, UEC_OK);

    // 取消写操作
    ret = UsbSrvClient.UsbCancelTransfer(pip, transferInfo.endpoint);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbCancelTransferBulkWrite ret:%{public}d", __LINE__, ret);
    ASSERT_EQ(ret, UEC_OK);

    bool close = UsbSrvClient.Close(pip);
    EXPECT_TRUE(close);

    USB_HILOGI(MODULE_USB_SERVICE, "UsbCancelTransferBulkWrite end.");
}


/**
 * @tc.name: UsbCancelTransferBulkRead
 * @tc.desc: Test the functionality of UsbCancelTransfer for read operation
 * @tc.type: FUNC
 */
HWTEST_F(UsbSubmitTransferTest, UsbCancelTransferBulkRead, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCancelTransferBulkRead enter.");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    EXPECT_TRUE(!(delist.empty())) << "Device list is empty";
    UsbDevice device = delist.front();
    ret = UsbSrvClient.RequestRight(device.GetName());
    ASSERT_EQ(ret, 0);
    USBDevicePipe pip;
    ret = UsbSrvClient.OpenDevice(device, pip);
    ASSERT_EQ(ret, 0);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.ClaimInterface(pip, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbCancelTransferBulkRead ClaimInterface ret: %{public}d",
        __LINE__, ret);
    ASSERT_EQ(ret, 0);

    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", 10);
    ASSERT_NE(ashmem, nullptr);
    ashmem->MapReadAndWriteAshmem();

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0x81;    // 读操作
    transferInfo.flags = 0;
    transferInfo.type = 2;           // 开发板不支持ISO传输类型
    transferInfo.timeOut = 0;        // 设置超时时间
    transferInfo.length = 10;        // 设置传输数据的长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // 只有type为1有iso

    auto callback = [](const TransferCallbackInfo &info,
                               const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE,
            "UsbCancelTransferBulkRead Callback status: %{public}d, actualLength: %{public}d",
            info.status, info.actualLength);
        ASSERT_EQ(info.status, UEC_OK);
    };

    ret = UsbSrvClient.UsbSubmitTransfer(pip, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbCancelTransferBulkRead ret: %{public}d", __LINE__, ret);
    ASSERT_EQ(ret, UEC_OK);

    // 取消读操作
    ret = UsbSrvClient.UsbCancelTransfer(pip, transferInfo.endpoint);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbCancelTransferBulkRead ret: %{public}d", __LINE__, ret);
    ASSERT_EQ(ret, UEC_OK);

    bool close = UsbSrvClient.Close(pip);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCancelTransferBulkRead end.");
}

} // namespace SubmitTransfer
} // namespace USB
} // namespace OHOS