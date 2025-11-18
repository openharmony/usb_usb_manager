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
OHOS::USB::UsbPort g_port;

constexpr int32_t ITERATION_FREQUENCY = 100;
constexpr int32_t REPETITION_FREQUENCY = 3;

// benchmark test for functions of usb port (EXCEPT callback)
class UsbmgrBenchmarkPortTest : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State &state);
    void TearDown(const ::benchmark::State &state);
};

void UsbmgrBenchmarkPortTest::SetUp(const ::benchmark::State &state)
{
    // initialization
    UsbCommonTest::GrantPermissionSysNative();
    std::vector<UsbPort> ports;
    g_usbSrvClient.GetPorts(ports);
    ASSERT_NE(0, ports.size());
    g_port = ports.front();
}

void UsbmgrBenchmarkPortTest::TearDown(const ::benchmark::State &state)
{
    // end of the test
    ;
}

/**
 * @tc.name: GetPorts01
 * @tc.desc: Test usbmgr functions: GetPorts
 * @tc.desc: int32_t GetPorts(std::vector<UsbPort> &usbPorts);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkPortTest, GetPorts01)(benchmark::State &state)
{
    int32_t ret;
    std::vector<UsbPort> usbPorts;
    for (auto _ : state) {
        ret = g_usbSrvClient.GetPorts(usbPorts);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkPortTest, GetPorts01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: GetSupportedModes01
 * @tc.desc: Test usbmgr functions: GetSupportedModes
 * @tc.desc: int32_t GetSupportedModes(int32_t portId, int32_t &supportedModes);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkPortTest, GetSupportedModes01)(benchmark::State &state)
{
    int32_t ret;
    int32_t modes;
    for (auto _ : state) {
        ret = g_usbSrvClient.GetSupportedModes(g_port.id, modes);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkPortTest, GetSupportedModes01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: SetPortRole01
 * @tc.desc: Test usbmgr functions: SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId, int32_t powerRole, int32_t dataRole);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkPortTest, SetPortRole01)(benchmark::State &state)
{
    int32_t ret;
    std::vector<int32_t> roles {1, 2};
    for (auto _ : state) {
        for (auto pr : roles) {
            for (auto dr : roles) {
                ret = g_usbSrvClient.SetPortRole(g_port.id, pr, dr);
            }
        }
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkPortTest, SetPortRole01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();


} // namespace
BENCHMARK_MAIN();