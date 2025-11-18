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

// benchmark test for functions as a usb host (EXCEPT transfer, serial, request, EDM, and callback)
class UsbmgrBenchmarkHostTest : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State &state);
    void TearDown(const ::benchmark::State &state);
};

void UsbmgrBenchmarkHostTest::SetUp(const ::benchmark::State &state)
{
    // initialization
    UsbCommonTest::GrantPermissionSysNative();
    std::vector<UsbDevice> devices;
    (void)g_usbSrvClient.GetDevices(devices);
    ASSERT_NE(devices.size(), 0);
    g_device = devices.back();
}

void UsbmgrBenchmarkHostTest::TearDown(const ::benchmark::State &state)
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
BENCHMARK_F(UsbmgrBenchmarkHostTest, HasRight01)(benchmark::State &state)
{
    bool hasRight;
    std::string deviceName = std::to_string(g_device.GetBusNum()) + "-" + std::to_string(g_device.GetDevAddr());
    for (auto _ : state) {
        hasRight = g_usbSrvClient.HasRight(deviceName);
    }
    EXPECT_EQ(true, hasRight);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostTest, HasRight01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: GetDevices01
 * @tc.desc: Test usbmgr functions: getDevices
 * @tc.desc: int32_t GetDevices(std::vector<UsbDevice> &deviceList);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostTest, GetDevices01)(benchmark::State &state)
{
    int32_t ret;
    std::vector<UsbDevice> devices;
    for (auto _ : state) {
        ret = g_usbSrvClient.GetDevices(devices);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostTest, GetDevices01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: OpenDevice01
 * @tc.desc: Test usbmgr functions: OpenDevice
 * @tc.desc: int32_t OpenDevice(const UsbDevice &device, USBDevicePipe &pipe);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostTest, OpenDevice01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    for (auto _ : state) {
        ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostTest, OpenDevice01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: Close01
 * @tc.desc: Test usbmgr functions: Close
 * @tc.desc: bool Close(const USBDevicePipe &pip);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostTest, Close01)(benchmark::State &state)
{
    USBDevicePipe pipe;
    for (auto _ : state) {
        (void)g_usbSrvClient.OpenDevice(g_device, pipe);
        EXPECT_EQ(g_usbSrvClient.Close(pipe), true);
    }
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostTest, Close01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: ResetDevice01
 * @tc.desc: Test usbmgr functions: ResetDevice
 * @tc.desc: int32_t ResetDevice(USBDevicePipe &pipe);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostTest, ResetDevice01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    EXPECT_EQ(0, ret);
    for (auto _ : state) {
        ret = g_usbSrvClient.ResetDevice(pipe);
    }
    EXPECT_EQ(g_usbSrvClient.Close(pipe), true);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostTest, ResetDevice01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: ClaimInterface01
 * @tc.desc: Test usbmgr functions: ClaimInterface
 * @tc.desc: int32_t ClaimInterface(USBDevicePipe &pipe, const UsbInterface &interface, bool force);
 * @tc.desc: Positive test: parameters correct, force = true
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostTest, ClaimInterface01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    EXPECT_EQ(0, ret);
    auto configs = g_device.GetConfigs();
    EXPECT_NE(configs.size(), 0);
    auto interfaces = configs.front().GetInterfaces();
    EXPECT_NE(interfaces.size(), 0);
    for (auto _ : state) {
        ret = g_usbSrvClient.ClaimInterface(pipe, interfaces.front(), true);
    }
    EXPECT_EQ(0, ret);
    EXPECT_EQ(g_usbSrvClient.Close(pipe), true);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostTest, ClaimInterface01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: UsbAttachKernelDriver01
 * @tc.desc: Test usbmgr functions: UsbAttachKernelDriver
 * @tc.desc: int32_t UsbAttachKernelDriver(USBDevicePipe &pipe, const UsbInterface &interface);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostTest, UsbAttachKernelDriver01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    EXPECT_EQ(0, ret);
    auto configs = g_device.GetConfigs();
    EXPECT_NE(configs.size(), 0);
    auto interfaces = configs.front().GetInterfaces();
    EXPECT_NE(interfaces.size(), 0);
    ret = g_usbSrvClient.ClaimInterface(pipe, interfaces.front(), true);
    EXPECT_EQ(0, ret);
    for (auto _ : state) {
        ret = g_usbSrvClient.UsbAttachKernelDriver(pipe, interfaces.front());
    }
    EXPECT_EQ(0, ret);
    EXPECT_EQ(g_usbSrvClient.Close(pipe), true);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostTest, UsbAttachKernelDriver01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: UsbDetachKernelDriver01
 * @tc.desc: Test usbmgr functions: UsbDetachKernelDriver
 * @tc.desc: int32_t UsbDetachKernelDriver(USBDevicePipe &pipe, const UsbInterface &interface);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostTest, UsbDetachKernelDriver01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    EXPECT_EQ(0, ret);
    auto configs = g_device.GetConfigs();
    EXPECT_NE(configs.size(), 0);
    auto interfaces = configs.front().GetInterfaces();
    EXPECT_NE(interfaces.size(), 0);
    ret = g_usbSrvClient.ClaimInterface(pipe, interfaces.front(), true);
    EXPECT_EQ(0, ret);
    for (auto _ : state) {
        (void)g_usbSrvClient.UsbAttachKernelDriver(pipe, interfaces.front());
        ret = g_usbSrvClient.UsbDetachKernelDriver(pipe, interfaces.front());
    }
    EXPECT_EQ(0, ret);
    (void)g_usbSrvClient.UsbAttachKernelDriver(pipe, interfaces.front());
    EXPECT_EQ(g_usbSrvClient.Close(pipe), true);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostTest, UsbDetachKernelDriver01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: SetConfiguration01
 * @tc.desc: Test usbmgr functions: SetConfiguration
 * @tc.desc: int32_t SetConfiguration(USBDevicePipe &pipe, const USBConfig &config);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostTest, SetConfiguration01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    EXPECT_EQ(0, ret);
    auto configs = g_device.GetConfigs();
    EXPECT_NE(configs.size(), 0);
    for (auto _ : state) {
        for (auto &config : configs) {
            ret = g_usbSrvClient.SetConfiguration(pipe, config);
        }
    }
    EXPECT_EQ(0, ret);
    EXPECT_EQ(g_usbSrvClient.Close(pipe), true);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostTest, SetConfiguration01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: SetInterface01
 * @tc.desc: Test usbmgr functions: SetInterface
 * @tc.desc: int32_t SetInterface(USBDevicePipe &pipe, const UsbInterface &interface);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostTest, SetInterface01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    EXPECT_EQ(0, ret);
    auto configs = g_device.GetConfigs();
    EXPECT_NE(configs.size(), 0);
    auto interfaces = configs.front().GetInterfaces();
    EXPECT_NE(interfaces.size(), 0);
    for (auto _ : state) {
        for (auto &interface : interfaces) {
            ret = g_usbSrvClient.ClaimInterface(pipe, interface, true);
            EXPECT_EQ(0, ret);
            ret = g_usbSrvClient.SetInterface(pipe, interface);
            EXPECT_EQ(0, ret);
            ret = g_usbSrvClient.ReleaseInterface(pipe, interface);
            EXPECT_EQ(0, ret);
        }
    }
    EXPECT_EQ(0, ret);
    EXPECT_EQ(g_usbSrvClient.Close(pipe), true);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostTest, SetInterface01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: ReleaseInterface01
 * @tc.desc: Test usbmgr functions: ReleaseInterface
 * @tc.desc: int32_t ReleaseInterface(USBDevicePipe &pipe, const UsbInterface &interface);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostTest, ReleaseInterface01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    EXPECT_EQ(0, ret);
    auto configs = g_device.GetConfigs();
    EXPECT_NE(configs.size(), 0);
    auto interfaces = configs.front().GetInterfaces();
    EXPECT_NE(interfaces.size(), 0);
    for (auto _ : state) {
        (void)g_usbSrvClient.ClaimInterface(pipe, interfaces.front(), true);
        g_usbSrvClient.ReleaseInterface(pipe, interfaces.front());
    }
    EXPECT_EQ(0, ret);
    EXPECT_EQ(g_usbSrvClient.Close(pipe), true);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostTest, ReleaseInterface01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: GetRawDescriptors01
 * @tc.desc: Test usbmgr functions: GetRawDescriptors
 * @tc.desc: int32_t GetRawDescriptors(USBDevicePipe &pipe, std::vector<uint8_t> &bufferData);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostTest, GetRawDescriptors01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    std::vector<uint8_t> bufferData;
    ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    EXPECT_EQ(0, ret);
    for (auto _ : state) {
        ret = g_usbSrvClient.GetRawDescriptors(pipe, bufferData);
    }
    EXPECT_EQ(0, ret);
    EXPECT_EQ(g_usbSrvClient.Close(pipe), true);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostTest, GetRawDescriptors01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: GetFileDescriptor01
 * @tc.desc: Test usbmgr functions: GetFileDescriptor
 * @tc.desc: int32_t GetFileDescriptor(USBDevicePipe &pipe, int32_t &fd);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostTest, GetFileDescriptor01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    int32_t fd;
    ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    EXPECT_EQ(0, ret);
    for (auto _ : state) {
        ret = g_usbSrvClient.GetFileDescriptor(pipe, fd);
    }
    EXPECT_EQ(0, ret);
    EXPECT_EQ(g_usbSrvClient.Close(pipe), true);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostTest, GetFileDescriptor01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: RequestRight01
 * @tc.desc: Test usbmgr functions: RequestRight
 * @tc.desc: int32_t RequestRight(std::string deviceName);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostTest, RequestRight01)(benchmark::State &state)
{
    int32_t ret;
    std::string deviceName = std::to_string(g_device.GetBusNum()) + "-" + std::to_string(g_device.GetDevAddr());
    for (auto _ : state) {
        ret = g_usbSrvClient.RequestRight(deviceName);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostTest, RequestRight01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: RemoveRight01
 * @tc.desc: Test usbmgr functions: RemoveRight
 * @tc.desc: int32_t RemoveRight(std::string deviceName);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostTest, RemoveRight01)(benchmark::State &state)
{
    int32_t ret;
    std::string deviceName = std::to_string(g_device.GetBusNum()) + "-" + std::to_string(g_device.GetDevAddr());
    for (auto _ : state) {
        (void)g_usbSrvClient.RequestRight(deviceName);
        ret = g_usbSrvClient.RemoveRight(deviceName);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostTest, RemoveRight01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: ClearHalt01
 * @tc.desc: Test usbmgr functions: ClearHalt
 * @tc.desc: int32_t ClearHalt(USBDevicePipe &pipe, const USBEndpoint &endpoint);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkHostTest, ClearHalt01)(benchmark::State &state)
{
    int32_t ret;
    USBDevicePipe pipe;
    ret = g_usbSrvClient.OpenDevice(g_device, pipe);
    EXPECT_EQ(0, ret);
    auto configs = g_device.GetConfigs();
    EXPECT_NE(configs.size(), 0);
    auto interfaces = configs.front().GetInterfaces();
    EXPECT_NE(interfaces.size(), 0);
    auto interface = interfaces.front();
    for (auto _ : state) {
        for (auto ep : interface.GetEndpoints()) {
            ret = g_usbSrvClient.ClearHalt(pipe, ep);
        }
    }
    EXPECT_EQ(0, ret);
    EXPECT_EQ(g_usbSrvClient.Close(pipe), true);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkHostTest, ClearHalt01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

} // namespace
BENCHMARK_MAIN();