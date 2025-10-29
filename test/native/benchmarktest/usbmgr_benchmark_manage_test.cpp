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
#include "usb_interface_type.h"

using namespace OHOS;
using namespace OHOS::EventFwk;
using namespace OHOS::USB;
using namespace OHOS::USB::Common;
using namespace OHOS::HDI::Usb::V1_0;
using namespace testing::ext;

namespace {
UsbSrvClient &g_usbSrvClient = UsbSrvClient::GetInstance();
std::vector<UsbDevice> g_devices;

constexpr int32_t ITERATION_FREQUENCY = 100;
constexpr int32_t REPETITION_FREQUENCY = 3;

// benchmark test for functions of usb management (i.e., enterprise device management)
class UsbmgrBenchmarkManageTest : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State &state);
    void TearDown(const ::benchmark::State &state);
};

void UsbmgrBenchmarkManageTest::SetUp(const ::benchmark::State &state)
{
    // initialization
    UsbCommonTest::GrantPermissionSysNative();
    (void)g_usbSrvClient.GetDevices(g_devices);
    ASSERT_NE(g_devices.size(), 0);
}

void UsbmgrBenchmarkManageTest::TearDown(const ::benchmark::State &state)
{
    // end of the test
    ;
}

/**
 * @tc.name: ManageGlobalInterface01
 * @tc.desc: Test usbmgr functions: ManageGlobalInterface
 * @tc.desc: int32_t ManageGlobalInterface(bool disable);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkManageTest, ManageGlobalInterface01)(benchmark::State &state)
{
    int32_t ret;
    bool disable = true;
    for (auto _ : state) {
        ret = g_usbSrvClient.ManageGlobalInterface(disable);
        disable = !disable;
        ret = g_usbSrvClient.ManageGlobalInterface(disable);
        disable = !disable;
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkManageTest, ManageGlobalInterface01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: ManageDevice01
 * @tc.desc: Test usbmgr functions: ManageDevice
 * @tc.desc: int32_t ManageDevice(int32_t vendorId, int32_t productId, bool disable);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkManageTest, ManageDevice01)(benchmark::State &state)
{
    int32_t ret;
    bool disable = true;
    auto dev = g_devices.front();
    for (auto _ : state) {
        ret = g_usbSrvClient.ManageDevice(dev.GetVendorId(), dev.GetProductId(), disable);
        disable = !disable;
        ret = g_usbSrvClient.ManageDevice(dev.GetVendorId(), dev.GetProductId(), disable);
        disable = !disable;
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkManageTest, ManageDevice01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: ManageDevice02
 * @tc.desc: Test usbmgr functions: ManageDevicePolicy
 * @tc.desc: int32_t ManageDevicePolicy(std::vector<UsbDeviceId> &trustList);
 * @tc.desc: Positive test: parameters correct, manage trust list
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkManageTest, ManageDevice02)(benchmark::State &state)
{
    int32_t ret;
    auto dev = g_devices.front();
    UsbDeviceId devId;
    devId.productId = dev.GetProductId();
    devId.vendorId = dev.GetVendorId();
    std::vector<UsbDeviceId> trustList {devId};
    std::vector<UsbDeviceId> emptyTrustList {};

    for (auto _ : state) {
        ret = g_usbSrvClient.ManageDevicePolicy(trustList);
        ret = g_usbSrvClient.ManageDevicePolicy(emptyTrustList);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkManageTest, ManageDevice02)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: ManageTypes01
 * @tc.desc: Test usbmgr functions: ManageInterfaceType
 * @tc.desc: int32_t ManageInterfaceType(const std::vector<UsbDeviceType> &disableType, bool disable);
 * @tc.desc: Positive test: parameters correct, device type
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkManageTest, ManageTypes01)(benchmark::State &state)
{
    auto dev = g_devices.front();
    UsbDeviceType type;
    type.baseClass = dev.GetClass();
    type.subClass = dev.GetSubclass();
    type.protocol = dev.GetProtocol();
    type.isDeviceType = true;
    std::vector<UsbDeviceType> disableTypes {type};
    int32_t ret;
    bool disable = true;
    for (auto _ : state) {
        ret = g_usbSrvClient.ManageInterfaceType(disableTypes, disable);
        disable = !disable;
        ret = g_usbSrvClient.ManageInterfaceType(disableTypes, disable);
        disable = !disable;
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkManageTest, ManageTypes01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: ManageTypes02
 * @tc.desc: Test usbmgr functions: ManageInterfaceType
 * @tc.desc: int32_t ManageInterfaceType(const std::vector<UsbDeviceType> &disableType, bool disable);
 * @tc.desc: Positive test: parameters correct, interface type
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkManageTest, ManageTypes02)(benchmark::State &state)
{
    auto dev = g_devices.front();
    EXPECT_NE(dev.GetConfigs().size(), 0);
    EXPECT_NE(dev.GetConfigs().front().GetInterfaces().size(), 0);
    auto interface = dev.GetConfigs().front().GetInterfaces().front();
    
    UsbDeviceType type;
    type.baseClass = interface.GetClass();
    type.subClass = interface.GetSubClass();
    type.protocol = interface.GetProtocol();
    type.isDeviceType = false;
    std::vector<UsbDeviceType> disableTypes {type};
    int32_t ret;
    bool disable = true;
    for (auto _ : state) {
        ret = g_usbSrvClient.ManageInterfaceType(disableTypes, disable);
        disable = !disable;
        ret = g_usbSrvClient.ManageInterfaceType(disableTypes, disable);
        disable = !disable;
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkManageTest, ManageTypes02)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

} // namespace
BENCHMARK_MAIN();