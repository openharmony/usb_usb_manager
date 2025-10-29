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
 * @tc.name: HasRight02
 * @tc.desc: Test usbmgr functions: HasRight
 * @tc.desc: bool HasRight(std::string deviceName);
 * @tc.desc: Negative test: parameters error, invalid deviceName
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostManagerTest, HasRight02)(benchmark::State &state)
{
    bool hasRight;
    std::string deviceName = std::to_string(0) + "-" + std::to_string(0);
    for (auto _ : state) {
        hasRight = g_usbSrvClient.HasRight(deviceName);
    }
    EXPECT_EQ(false, hasRight);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, HasRight02)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: HasRight03
 * @tc.desc: Test usbmgr functions: HasRight
 * @tc.desc: bool HasRight(std::string deviceName);
 * @tc.desc: Negative test: parameters error, not name of a device
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostManagerTest, HasRight03)(benchmark::State &state)
{
    bool hasRight;
    std::string deviceName = "*#123";
    for (auto _ : state) {
        hasRight = g_usbSrvClient.HasRight(deviceName);
    }
    EXPECT_EQ(false, hasRight);
    UsbCommonTest::GrantPermissionSysNative();
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, HasRight03)->
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
 * @tc.name: OpenDevice02
 * @tc.desc: Test usbmgr functions: OpenDevice
 * @tc.desc: int32_t OpenDevice(const UsbDevice &device, USBDevicePipe &pipe);
 * @tc.desc: Negative test: parameters error, invalid device
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostManagerTest, OpenDevice02)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    UsbDevice device;
    device.SetBusNum(0);
    device.SetDevAddr(0);
    for (auto _ : state) {
        ret = g_usbSrvClient.OpenDevice(device, pipe);
    }
    EXPECT_NE(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, OpenDevice02)->
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
        (void)g_usbSrvClient.OpenDevice(device, pipe);
        ret = g_usbSrvClient.Close(pipe);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, Close01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: Close02
 * @tc.desc: Test usbmgr functions: Close
 * @tc.desc: bool Close(const USBDevicePipe &pip);
 * @tc.desc: Negative test: parameters error, invalid pipe
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostManagerTest, Close02)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    for (auto _ : state) {
        ret = g_usbSrvClient.Close(pipe);
    }
    EXPECT_NE(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, Close02)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: Close03
 * @tc.desc: Test usbmgr functions: Close
 * @tc.desc: bool Close(const USBDevicePipe &pip);
 * @tc.desc: Negative test: parameters error, pipe already closed
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostManagerTest, Close03)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    (void)g_usbSrvClient.OpenDevice(device, pipe);
    ret = g_usbSrvClient.Close(pipe);
    EXPECT_EQ(0, ret);
    for (auto _ : state) {
        ret = g_usbSrvClient.Close(pipe);
    }
    EXPECT_NE(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, Close03)->
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
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, ResetDevice01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: ResetDevice02
 * @tc.desc: Test usbmgr functions: ResetDevice
 * @tc.desc: int32_t ResetDevice(USBDevicePipe &pipe);
 * @tc.desc: Negative test: parameters error, invalid pipe
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostManagerTest, ResetDevice02)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    for (auto _ : state) {
        ret = g_usbSrvClient.ResetDevice(pipe);
    }
    EXPECT_NE(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, ResetDevice02)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: ClaimInterface01
 * @tc.desc: Test usbmgr functions: ClaimInterface
 * @tc.desc: int32_t ClaimInterface(USBDevicePipe &pipe, const UsbInterface &interface, bool force);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostManagerTest, ClaimInterface01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    EXPECT_EQ(0, ret);
    for (auto _ : state) {
        ret = g_usbSrvClient.ResetDevice(pipe);
    }
    ret = g_usbSrvClient.Close(pipe);
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostManagerTest, ClaimInterface01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

} // namespace
BENCHMARK_MAIN();