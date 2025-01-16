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

#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <atomic>
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
constexpr int32_t TIMEOUT = 2000;
std::mutex g_mtx;                                     // 全局互斥锁，用于保护共享资源
std::condition_variable g_cv;                         // 条件变量，用于线程间的同步
std::vector<bool> g_threadSuccessFlags;               // 用于标记每个线程是否成功完成操作
std::atomic<int32_t> g_completedThreads(0);           // 用于标记线程完成数量
std::atomic<int32_t> g_successCount(0);               // 用于统计 ASSERT_EQ(ret, UEC_OK) 成功的次数

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

void UsbSubmitTransferBulkWriteThreadFunc(int32_t threadId)
{
    std::unique_lock<std::mutex> lock(g_mtx);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWrite enter.");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWrite %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWrite %{public}d point=%{public}d", __LINE__,
               point.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", TEN);
    ASSERT_NE(ashmem, nullptr);
    const uint8_t dataToWrite[TEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, sizeof(dataToWrite), 0);
    ASSERT_TRUE(writeSuccess);
    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0x01;    // 0x01写 0x81读
    transferInfo.flags = 0;
    transferInfo.type = TYPE_BULK; // 开发板仅支持bulk
    transferInfo.timeOut = TIMEOUT;
    transferInfo.length = TEN;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192
    auto callback = [](const TransferCallbackInfo &info,
        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {};
    ret = UsbSrvClient.UsbSubmitTransfer(pipe, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferBulkWrite ret:%{public}d", __LINE__, ret);
    if (ret == UEC_OK) {
        g_successCount++;
    }
    ASSERT_EQ(ret, UEC_OK);
    bool close = UsbSrvClient.Close(pipe);
    EXPECT_TRUE(close);
    lock.unlock();
    g_threadSuccessFlags[threadId] = true;
    g_completedThreads++;
    g_cv.notify_one();
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWrite end.");
}

HWTEST_F(UsbSubmitTransferTest, UsbSubmitTransferBulkWriteConcurrent, TestSize.Level1)
{
    int32_t numThreads = 5;  // 并发线程数量
    g_threadSuccessFlags.resize(numThreads);
    std::vector<std::thread> threads;
    // 启动多个线程
    for (int32_t i = 0; i < numThreads; ++i) {
        threads.emplace_back([i]() { UsbSubmitTransferBulkWriteThreadFunc(i); });
    }
    // 等待所有线程完成
    std::unique_lock<std::mutex> lock(g_mtx);
    g_cv.wait(lock, [numThreads]{ return g_completedThreads == numThreads; });
    // 清理资源和验证结果
    for (auto& t : threads) {
        t.join();
    }
    // 检查是否所有线程都成功完成操作
    for (int32_t i = 0; i < numThreads; ++i) {
        EXPECT_TRUE(g_threadSuccessFlags[i]) << "Thread " << i << " did not complete successfully.";
    }
    // 检查是否所有的 ASSERT_EQ(ret, UEC_OK) 都成功
    EXPECT_EQ(g_successCount, numThreads) << "Not all UsbSubmitTransfer calls returned UEC_OK.";
}

/**
 * @tc.name: UsbSubmitTransferBulkWrite
 * @tc.desc: Test the USB data write functionality of UsbSubmitTransfer OK.
 * @tc.type: FUNC
 */
HWTEST_F(UsbSubmitTransferTest, UsbSubmitTransferBulkWrite, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWrite enter.");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWrite %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWrite %{public}d size=%{public}zu", __LINE__,
               devi.size());
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWrite %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWrite %{public}d point=%{public}d", __LINE__,
               point.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", TEN);
    ASSERT_NE(ashmem, nullptr);
    const uint8_t dataToWrite[TEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, sizeof(dataToWrite), 0);
    ASSERT_TRUE(writeSuccess);
    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0x01;    // 0x01写 0x81读
    transferInfo.flags = 0;
    transferInfo.type = TYPE_BULK; // 开发板仅支持bulk
    transferInfo.timeOut = 2000;
    transferInfo.length = TEN;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192
    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWrite cb status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pipe, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferBulkWrite ret:%{public}d", __LINE__, ret);
    ASSERT_EQ(ret, 0);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWrite %{public}d close=%{public}d", __LINE__, close);
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
    EXPECT_TRUE(!(delist.empty())) << "Device list is empty";
    UsbDevice device = delist.front();
    ret = UsbSrvClient.RequestRight(device.GetName());
    USBDevicePipe pip;
    ret = UsbSrvClient.OpenDevice(device, pip);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.ClaimInterface(pip, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkReadTimeOut ClaimInterface ret%{public}d", ret);

    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", TEN);
    ASSERT_NE(ashmem, nullptr);
    ashmem->MapReadAndWriteAshmem();

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0x81;    // 0x01写 0x81读
    transferInfo.flags = 0;
    transferInfo.type = TYPE_BULK; // 开发板仅支持bulk
    transferInfo.timeOut = 2000;
    transferInfo.length = TEN;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192

    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE,
            "UsbSubmitTransferBulkReadTimeOut Callback status: %{public}d, actualLength: %{public}d",
            info.status, info.actualLength);
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
    EXPECT_TRUE(!(delist.empty())) << "Device list is empty";
    UsbDevice device = delist.front();
    ret = UsbSrvClient.RequestRight(device.GetName());
    USBDevicePipe pip;
    ret = UsbSrvClient.OpenDevice(device, pip);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.ClaimInterface(pip, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferBulkWriteInvalidEndpoint ret:%{public}d",
        __LINE__, ret);

    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", TEN);
    ASSERT_NE(ashmem, nullptr);
    const uint8_t dataToWrite[TEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, sizeof(dataToWrite), 0);
    ASSERT_TRUE(writeSuccess);
    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0xFF;    //无效参数
    transferInfo.flags = 0;
    transferInfo.type = TYPE_BULK; // 开发板仅支持bulk
    transferInfo.timeOut = 2000;
    transferInfo.length = TEN;        // 期望长度
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
    if (ret == -1) {
        ret = IO_ERROR;
    }
    ASSERT_EQ(ret, USB_SUBMIT_TRANSFER_IO_ERROR);
    bool close = UsbSrvClient.Close(pip);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferBulkWriteInvalidEndpoint end.");
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
    USBDevicePipe pip;
    ret = UsbSrvClient.OpenDevice(device, pip);

    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.ClaimInterface(pip, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCancelTransferBulkWrite ClaimInterface %{public}d ret:%{public}d",
        __LINE__, ret);

    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", TEN);
    ASSERT_NE(ashmem, nullptr);
    const uint8_t dataToWrite[TEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, TEN, 0);
    ASSERT_TRUE(writeSuccess);

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0x01;    // 写操作
    transferInfo.flags = 0;
    transferInfo.type = TYPE_BULK; // 开发板不支持ISO传输类型
    transferInfo.timeOut = 0;        // 设置超时时间
    transferInfo.length = TEN;        // 设置传输数据的长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // 只有type为1有iso

    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbCancelTransferBulkWrite cb status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
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
    EXPECT_TRUE(!(delist.empty())) << "Device list is empty";
    UsbDevice device = delist.front();
    ret = UsbSrvClient.RequestRight(device.GetName());
    USBDevicePipe pip;
    ret = UsbSrvClient.OpenDevice(device, pip);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.ClaimInterface(pip, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbCancelTransferBulkRead ClaimInterface ret: %{public}d",
        __LINE__, ret);

    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", TEN);
    ASSERT_NE(ashmem, nullptr);
    ashmem->MapReadAndWriteAshmem();

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0x81;    // 读操作
    transferInfo.flags = 0;
    transferInfo.type = TYPE_BULK; // 开发板不支持ISO传输类型
    transferInfo.timeOut = 0;        // 设置超时时间
    transferInfo.length = TEN;        // 设置传输数据的长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // 只有type为1有iso

    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbCancelTransferBulkRead cb status:%{public}d,actualLength: %{public}d",
            info.status, info.actualLength);
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

/**
 * @tc.name: UsbSubmitTransferInterruptWrite
 * @tc.desc: Test the USB data write functionality of UsbSubmitTransfer OK.
 * @tc.type: FUNC
 */
HWTEST_F(UsbSubmitTransferTest, UsbSubmitTransferInterruptWrite, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferInterruptWrite enter.");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferInterruptWrite %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferInterruptWrite %{public}d size=%{public}zu", __LINE__,
               devi.size());
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferInterruptWrite %{public}d point=%{public}d", __LINE__,
               point.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", TEN);
    ASSERT_NE(ashmem, nullptr);
    const uint8_t dataToWrite[TEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, sizeof(dataToWrite), 0);
    ASSERT_TRUE(writeSuccess);
    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0x01;    // 0x01写 0x81读,中断写不支持，只能读
    transferInfo.flags = 0;
    transferInfo.type = TYPE_INTERRUPT; // 开发板仅支持bulk
    transferInfo.timeOut = 2000;
    transferInfo.length = TEN;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192
    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferInterruptWrite cb status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pipe, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferInterruptWrite ret:%{public}d", __LINE__, ret);
    if (ret == -1) {
        ret = IO_ERROR;
    }
    ASSERT_EQ(ret, USB_SUBMIT_TRANSFER_IO_ERROR);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferInterruptWrite %{public}d close=%{public}d", __LINE__, close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferInterruptWrite end.");
}

/**
 * @tc.name: UsbSubmitTransferInterruptReadInvalidEndpoint
 * @tc.desc: Test the USB data write functionality of UsbSubmitTransfer with invalid endpoint type
 * @tc.type: FUNC
 */
HWTEST_F(UsbSubmitTransferTest, UsbSubmitTransferInterruptReadInvalidEndpoint, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferInterruptReadInvalidEndpoint begin.");

    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    EXPECT_TRUE(!(delist.empty())) << "Device list is empty";
    UsbDevice device = delist.front();
    ret = UsbSrvClient.RequestRight(device.GetName());
    USBDevicePipe pip;
    ret = UsbSrvClient.OpenDevice(device, pip);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.ClaimInterface(pip, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferInterruptReadInvalidEndpoint ret:%{public}d",
        __LINE__, ret);

    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", TEN);
    ASSERT_NE(ashmem, nullptr);
    const uint8_t dataToWrite[TEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, sizeof(dataToWrite), 0);
    ASSERT_TRUE(writeSuccess);
    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0xFF;    //无效参数
    transferInfo.flags = 0;
    transferInfo.type = TYPE_INTERRUPT; // 开发板仅支持bulk
    transferInfo.timeOut = 2000;
    transferInfo.length = TEN;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192

    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE,
            "UsbSubmitTransferInterruptReadInvalidEndpoint cb status: %{public}d, actualLength: %{public}d",
            info.status, info.actualLength);
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pip, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferInterruptReadInvalidEndpoint ret=%{public}d",
        __LINE__, ret);
    if (ret == -1) {
        ret = IO_ERROR;
    }
    ASSERT_EQ(ret, USB_SUBMIT_TRANSFER_IO_ERROR);
    bool close = UsbSrvClient.Close(pip);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferInterruptReadInvalidEndpoint end.");
}

/**
 * @tc.name:  UsbSubmitTransferInterruptWriteIoError
 * @tc.desc: Test USB I/O error when interface is not claimed.
 * @tc.type: FUNC
 */
HWTEST_F(UsbSubmitTransferTest, UsbSubmitTransferInterruptWriteIoError, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferInterruptWriteIoError begin.");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    EXPECT_TRUE(!(delist.empty())) << "Device list is empty";
    UsbDevice device = delist.front();
    ret = UsbSrvClient.RequestRight(device.GetName());
    USBDevicePipe pip;
    ret = UsbSrvClient.OpenDevice(device, pip);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    UsbSrvClient.ReleaseInterface(pip, interface);
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", TEN);
    ASSERT_NE(ashmem, nullptr);
    const uint8_t dataToWrite[TEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, sizeof(dataToWrite), 0);
    ASSERT_TRUE(writeSuccess);

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0x01;    // 写操作（对于开发板，0x81是读操作）
    transferInfo.flags = 0;
    transferInfo.type = TYPE_INTERRUPT; // 开发板仅支持bulk
    transferInfo.timeOut = 2000;
    transferInfo.length = TEN;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 0;  // iso传输包数量 iso单包传输最大长度192
    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "InterruptWriteIoError status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
    };

    ret = UsbSrvClient.UsbSubmitTransfer(pip, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferInterruptWriteIoError ret:%{public}d",
        __LINE__, ret);
    if (ret == -1) {
        ret = IO_ERROR;
    }
    ASSERT_EQ(ret,  USB_SUBMIT_TRANSFER_IO_ERROR);
    bool close = UsbSrvClient.Close(pip);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferInterruptWriteIoError end.");
}

/**
 * @tc.name: UsbSubmitTransferIsochronousWrite
 * @tc.desc: Test the USB data write functionality of UsbSubmitTransfer OK.
 * @tc.type: FUNC
 */
HWTEST_F(UsbSubmitTransferTest, UsbSubmitTransferIsochronousWrite, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferIsochronousWrite enter.");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferIsochronousWrite %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferIsochronousWrite %{public}d size=%{public}zu", __LINE__,
               devi.size());
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    UsbSrvClient.RequestRight(device.GetName());
    ret = UsbSrvClient.OpenDevice(device, pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferIsochronousWrite %{public}d OpenDevice=%{public}d", __LINE__,
               ret);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().front();
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferIsochronousWrite %{public}d point=%{public}d", __LINE__,
               point.GetInterfaceId());
    ret = UsbSrvClient.ClaimInterface(pipe, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferIsochronousWrite %{public}d ClaimInterface=%{public}d",
               __LINE__, ret);
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", TEN);
    ASSERT_NE(ashmem, nullptr);
    const uint8_t dataToWrite[TEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, sizeof(dataToWrite), 0);
    ASSERT_TRUE(writeSuccess);
    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0x05;    // 0x01写 0x81读 测试设备：usb耳机，端点0x05
    transferInfo.flags = 0;
    transferInfo.type = TYPE_ISOCHRONOUS; // 开发板仅支持bulk
    transferInfo.timeOut = 2000;
    transferInfo.length = TEN;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 1;  // iso传输包数量 iso单包传输最大长度192
    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferIsochronousWrite cb status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pipe, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferIsochronousWrite ret:%{public}d", __LINE__, ret);
    ASSERT_EQ(ret, UEC_OK);
    bool close = UsbSrvClient.Close(pipe);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferIsochronousWrite %{public}d close=%{public}d", __LINE__, close);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferIsochronousWrite end.");
}

/**
 * @tc.name: UsbSubmitTransferIsochronousRead
 * @tc.desc: Test the USB data read functionality of UsbSubmitTransfer
 * @tc.type: FUNC
 */
HWTEST_F(UsbSubmitTransferTest, UsbSubmitTransferIsochronousRead, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferIsochronousRead enter.");

    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    EXPECT_TRUE(!(delist.empty())) << "Device list is empty";
    UsbDevice device = delist.front();
    ret = UsbSrvClient.RequestRight(device.GetName());
    USBDevicePipe pip;
    ret = UsbSrvClient.OpenDevice(device, pip);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.ClaimInterface(pip, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferIsochronousRead ClaimInterface ret%{public}d", ret);

    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", TEN);
    ASSERT_NE(ashmem, nullptr);
    ashmem->MapReadAndWriteAshmem();

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0x84;    // 0x01写 0x81读
    transferInfo.flags = 0;
    transferInfo.type = TYPE_ISOCHRONOUS; // 开发板仅支持bulk
    transferInfo.timeOut = 2000;
    transferInfo.length = TEN;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 1;  // iso传输包数量 iso单包传输最大长度192

    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE,
            "UsbSubmitTransferIsochronousRead Callback status: %{public}d, actualLength: %{public}d",
            info.status, info.actualLength);
    };

    ret = UsbSrvClient.UsbSubmitTransfer(pip, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferIsochronousRead ret:%{public}d", __LINE__, ret);
    ASSERT_EQ(ret, UEC_OK);

    bool close = UsbSrvClient.Close(pip);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferIsochronousRead end.");
}

/**
 * @tc.name: UsbSubmitTransferIsochronousWriteInvalidEndpoint
 * @tc.desc: Test the USB data write functionality of UsbSubmitTransfer with invalid endpoint type
 * @tc.type: FUNC
 */
HWTEST_F(UsbSubmitTransferTest, UsbSubmitTransferIsochronousWriteInvalidEndpoint, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferIsochronousWriteInvalidEndpoint begin.");

    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    EXPECT_TRUE(!(delist.empty())) << "Device list is empty";
    UsbDevice device = delist.front();
    ret = UsbSrvClient.RequestRight(device.GetName());
    USBDevicePipe pip;
    ret = UsbSrvClient.OpenDevice(device, pip);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.ClaimInterface(pip, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferIsochronousWriteInvalidEndpoint ret:%{public}d",
        __LINE__, ret);

    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", TEN);
    ASSERT_NE(ashmem, nullptr);
    const uint8_t dataToWrite[TEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, sizeof(dataToWrite), 0);
    ASSERT_TRUE(writeSuccess);
    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0xFF;    //无效参数
    transferInfo.flags = 0;
    transferInfo.type = TYPE_ISOCHRONOUS; // 开发板仅支持bulk
    transferInfo.timeOut = 2000;
    transferInfo.length = TEN;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 1;  // iso传输包数量 iso单包传输最大长度192

    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE,
            "UsbSubmitTransferIsochronousWriteInvalidEndpoint cb status: %{public}d, actualLength: %{public}d",
            info.status, info.actualLength);
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pip, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferIsochronousWriteInvalidEndpoint ret=%{public}d",
        __LINE__, ret);
    if (ret == -2) {
        ret = INVALID_PARAM;
    }
    ASSERT_EQ(ret, USB_SUBMIT_TRANSFER_INVALID_PARAM);
    bool close = UsbSrvClient.Close(pip);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferIsochronousWriteInvalidEndpoint end.");
}

/**
 * @tc.name:  UsbSubmitTransferIsochronousWriteIoError
 * @tc.desc: Test USB I/O error when interface is not claimed.
 * @tc.type: FUNC
 */
HWTEST_F(UsbSubmitTransferTest, UsbSubmitTransferIsochronousWriteIoError, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferIsochronousWriteIoError begin.");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    EXPECT_TRUE(!(delist.empty())) << "Device list is empty";
    UsbDevice device = delist.front();
    ret = UsbSrvClient.RequestRight(device.GetName());
    USBDevicePipe pip;
    ret = UsbSrvClient.OpenDevice(device, pip);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    USBEndpoint point = interface.GetEndpoints().front();
    UsbSrvClient.ReleaseInterface(pip, interface);
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", TEN);
    ASSERT_NE(ashmem, nullptr);
    const uint8_t dataToWrite[TEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, sizeof(dataToWrite), 0);
    ASSERT_TRUE(writeSuccess);
    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0x01;    // 写操作（对于开发板，0x81是读操作）
    transferInfo.flags = 0;
    transferInfo.type = TYPE_ISOCHRONOUS; // 开发板仅支持bulk
    transferInfo.timeOut = 2000;
    transferInfo.length = TEN;        // 期望长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 1;  // iso传输包数量 iso单包传输最大长度192
    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "IsochronousWriteIoError status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
    };
    ret = UsbSrvClient.UsbSubmitTransfer(pip, transferInfo, callback, ashmem);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbSubmitTransferIsochronousWriteIoError ret:%{public}d",
        __LINE__, ret);
    if (ret == -1) {
        ret = IO_ERROR;
    }
    ASSERT_EQ(ret,  USB_SUBMIT_TRANSFER_IO_ERROR);
    bool close = UsbSrvClient.Close(pip);
    EXPECT_TRUE(close);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbSubmitTransferIsochronousWriteIoError end.");
}

/**
 * @tc.name: UsbCancelTransferIsochronousWrite
 * @tc.desc: Test the functionality of UsbCancelTransfer for write operation
 * @tc.type: FUNC
 */
HWTEST_F(UsbSubmitTransferTest, UsbCancelTransferIsochronousWrite, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCancelTransferIsochronousWrite enter.");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    EXPECT_TRUE(!(delist.empty())) << "Device list is empty";
    UsbDevice device = delist.front();
    ret = UsbSrvClient.RequestRight(device.GetName());
    USBDevicePipe pip;
    ret = UsbSrvClient.OpenDevice(device, pip);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(1);
    USBEndpoint point = interface.GetEndpoints().front();
    ret = UsbSrvClient.ClaimInterface(pip, interface, true);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbCancelTransferIsochronousWrite ClaimInterface %{public}d ret:%{public}d",
        __LINE__, ret);

    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("usb_shared_memory", TEN);
    ASSERT_NE(ashmem, nullptr);
    const uint8_t dataToWrite[TEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    ashmem->MapReadAndWriteAshmem();
    bool writeSuccess = ashmem->WriteToAshmem(dataToWrite, TEN, 0);
    ASSERT_TRUE(writeSuccess);

    HDI::Usb::V1_2::USBTransferInfo transferInfo;
    transferInfo.endpoint = 0x05;    // 写操作
    transferInfo.flags = 0;
    transferInfo.type = TYPE_ISOCHRONOUS; // 开发板不支持ISO传输类型
    transferInfo.timeOut = 0;        // 设置超时时间
    transferInfo.length = TEN;        // 设置传输数据的长度
    transferInfo.userData = 0;
    transferInfo.numIsoPackets = 1;  // 只有type为1有iso

    auto callback = [](const TransferCallbackInfo &info,
                        const std::vector<HDI::Usb::V1_2::UsbIsoPacketDescriptor> &packets, uint64_t userData) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbCancelTransferIsochronousWrite cb status:%{public}d,actualLength:%{public}d",
            info.status, info.actualLength);
    };
    UsbSrvClient.UsbSubmitTransfer(pip, transferInfo, callback, ashmem);
    // 取消写操作
    ret = UsbSrvClient.UsbCancelTransfer(pip, transferInfo.endpoint);
    USB_HILOGI(MODULE_USB_SERVICE, "%{public}d line. UsbCancelTransferIsochronousWrite ret:%{public}d", __LINE__, ret);
    ASSERT_EQ(ret, FIVE); // 传输已完成或者已被取消

    bool close = UsbSrvClient.Close(pip);
    EXPECT_TRUE(close);

    USB_HILOGI(MODULE_USB_SERVICE, "UsbCancelTransferIsochronousWrite end.");
}
} // namespace SubmitTransfer
} // namespace USB
} // namespace OHOS