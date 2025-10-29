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

#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <map>

#include "usb_service.h"
#include "usb_srv_client.h"
#include "usb_common_test.h"
#include "usb_errors.h"

using namespace OHOS;
using namespace OHOS::EventFwk;
using namespace OHOS::USB;
using namespace OHOS::USB::Common;
using namespace OHOS::HDI::Usb::V1_0;
using namespace testing::ext;

namespace {
UsbSrvClient &g_usbSrvClient = UsbSrvClient::GetInstance();
OHOS::USB::UsbDevice g_device;

constexpr int32_t ITERATION_FREQUENCY = 100;
constexpr int32_t REPETITION_FREQUENCY = 3;

class UsbmgrBenchmarkHostManagerTest : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State &state);
    void TearDown(const ::benchmark::State &state);
};

void UsbmgrBenchmarkHostManagerTest::SetUp(const ::benchmark::State &state)
{
    // initialization
    UsbCommonTest::GrantPermissionSysNative();
    std::vector<UsbDevice> devices;
    (void)g_usbSrvClient.GetDevices(devices);
    ASSERT_NE(devices.size(), 0);
    g_device = devices.back();
}

void UsbmgrBenchmarkHostManagerTest::TearDown(const ::benchmark::State &state)
{
    // end of the test
    ;
}

/**
 * @tc.name: HasRight01
 * @tc.desc: Test usbmgr functions: HasRight
 * @tc.desc: bool HasRight(std::string deviceName);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostManagerTest, HasRight01)(benchmark::State &state)
{
    bool hasRight;
    std::string deviceName = std::to_string(g_device.GetBusNum()) + "-" + std::to_string(g_device.GetDevAddr());
    for (auto _ : state) {
        hasRight = g_usbSrvClient.HasRight(deviceName);
    }
    EXPECT_EQ(true, hasRight);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, HasRight01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: GetDevices01
 * @tc.desc: Test usbmgr functions: getDevices
 * @tc.desc: int32_t GetDevices(std::vector<UsbDevice> &deviceList);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostManagerTest, GetDevices01)(benchmark::State &state)
{
    int32_t ret;
    std::vector<UsbDevice> devices;
    for (auto _ : state) {
        ret = g_usbSrvClient.GetDevices(devices);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, GetDevices01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: OpenDevice01
 * @tc.desc: Test usbmgr functions: OpenDevice
 * @tc.desc: int32_t OpenDevice(const UsbDevice &device, USBDevicePipe &pipe);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostManagerTest, OpenDevice01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    for (auto _ : state) {
        ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, OpenDevice01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: Close01
 * @tc.desc: Test usbmgr functions: Close
 * @tc.desc: bool Close(const USBDevicePipe &pip);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostManagerTest, Close01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    for (auto _ : state) {
        (void)g_usbSrvClient.OpenDevice(g_device, pipe);
        ret = g_usbSrvClient.Close(pipe);
    }
    EXPECT_EQ(1, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, Close01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: ResetDevice01
 * @tc.desc: Test usbmgr functions: ResetDevice
 * @tc.desc: int32_t ResetDevice(USBDevicePipe &pipe);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostManagerTest, ResetDevice01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    EXPECT_EQ(0, ret);
    for (auto _ : state) {
        ret = g_usbSrvClient.ResetDevice(pipe);
    }
    ret = g_usbSrvClient.Close(pipe);
    EXPECT_EQ(1, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, ResetDevice01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: ClaimInterface01
 * @tc.desc: Test usbmgr functions: ClaimInterface
 * @tc.desc: int32_t ClaimInterface(USBDevicePipe &pipe, const UsbInterface &interface, bool force);
 * @tc.desc: Positive test: parameters correct, force = true
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostManagerTest, ClaimInterface01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    EXPECT_EQ(0, ret);
    auto configs = g_device.GetConfigs();
    EXPECT_GE(configs.size(), 0);
    auto interfaces = configs.front().GetInterfaces();
    EXPECT_GE(interfaces.size(), 0);
    for (auto _ : state) {
        ret = g_usbSrvClient.ClaimInterface(pipe, interfaces.front(), true);
    }
    EXPECT_EQ(0, ret);
    ret = g_usbSrvClient.Close(pipe);
    EXPECT_EQ(1, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, ClaimInterface01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: UsbAttachKernelDriver01
 * @tc.desc: Test usbmgr functions: UsbAttachKernelDriver
 * @tc.desc: int32_t UsbAttachKernelDriver(USBDevicePipe &pipe, const UsbInterface &interface);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostManagerTest, UsbAttachKernelDriver01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    EXPECT_EQ(0, ret);
    auto configs = g_device.GetConfigs();
    EXPECT_GE(configs.size(), 0);
    auto interfaces = configs.front().GetInterfaces();
    EXPECT_GE(interfaces.size(), 0);
    ret = g_usbSrvClient.ClaimInterface(pipe, interfaces.front(), true);
    EXPECT_EQ(0, ret);
    for (auto _ : state) {
        ret = UsbAttachKernelDriver(pipe, interfaces.front());
    }
    EXPECT_EQ(0, ret);
    ret = g_usbSrvClient.Close(pipe);
    EXPECT_EQ(1, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, UsbAttachKernelDriver01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: UsbDetachKernelDriver01
 * @tc.desc: Test usbmgr functions: UsbDetachKernelDriver
 * @tc.desc: int32_t UsbDetachKernelDriver(USBDevicePipe &pipe, const UsbInterface &interface);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostManagerTest, UsbDetachKernelDriver01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    EXPECT_EQ(0, ret);
    auto configs = g_device.GetConfigs();
    EXPECT_GE(configs.size(), 0);
    auto interfaces = configs.front().GetInterfaces();
    EXPECT_GE(interfaces.size(), 0);
    ret = g_usbSrvClient.ClaimInterface(pipe, interfaces.front(), true);
    EXPECT_EQ(0, ret);
    for (auto _ : state) {
        (void)UsbAttachKernelDriver(pipe, interfaces.front());
        ret = UsbDetachKernelDriver(pipe, interfaces.front());
    }
    EXPECT_EQ(0, ret);
    (void)UsbAttachKernelDriver(pipe, interfaces.front());
    ret = g_usbSrvClient.Close(pipe);
    EXPECT_EQ(1, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, UsbDetachKernelDriver01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: ReleaseInterface01
 * @tc.desc: Test usbmgr functions: ReleaseInterface
 * @tc.desc: int32_t ReleaseInterface(USBDevicePipe &pipe, const UsbInterface &interface);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostManagerTest, ReleaseInterface01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    EXPECT_EQ(0, ret);
    auto configs = g_device.GetConfigs();
    EXPECT_GE(configs.size(), 0);
    auto interfaces = configs.front().GetInterfaces();
    EXPECT_GE(interfaces.size(), 0);
    for (auto _ : state) {
        (void)g_usbSrvClient.ClaimInterface(pipe, interfaces.front(), true);
        g_usbSrvClient.ReleaseInterface(pipe, interfaces.front());
    }
    EXPECT_EQ(0, ret);
    ret = g_usbSrvClient.Close(pipe);
    EXPECT_EQ(1, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, ReleaseInterface01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

} // namespace
BENCHMARK_MAIN();