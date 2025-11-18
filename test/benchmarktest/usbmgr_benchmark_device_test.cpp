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

constexpr int32_t ITERATION_FREQUENCY = 100;
constexpr int32_t REPETITION_FREQUENCY = 3;

// benchmark test for functions as a usb device (EXCEPT accessory, and callback)
class UsbmgrBenchmarkDeviceTest : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State &state);
    void TearDown(const ::benchmark::State &state);
};

void UsbmgrBenchmarkDeviceTest::SetUp(const ::benchmark::State &state)
{
    // initialization
    UsbCommonTest::GrantPermissionSysNative();
}

void UsbmgrBenchmarkDeviceTest::TearDown(const ::benchmark::State &state)
{
    // end of the test
    ;
}

/**
 * @tc.name: GetCurrentFunctions01
 * @tc.desc: Test usbmgr functions: GetCurrentFunctions
 * @tc.desc: int32_t GetCurrentFunctions(int32_t &funcs);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkDeviceTest, GetCurrentFunctions01)(benchmark::State &state)
{
    int32_t ret;
    int32_t func;
    for (auto _ : state) {
        ret = g_usbSrvClient.GetCurrentFunctions(func);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkDeviceTest, GetCurrentFunctions01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: SetCurrentFunctions01
 * @tc.desc: Test usbmgr functions: SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkDeviceTest, SetCurrentFunctions01)(benchmark::State &state)
{
    int32_t ret;
    std::vector<int32_t> funcs {
        0, (1 << 0), (1 << 1), (1 << 3), (1 << 4), (1 << 5), (1 << 8), (1 << 9), (1 << 2),
    };
    for (auto _ : state) {
        for (auto func : funcs) {
            ret = g_usbSrvClient.SetCurrentFunctions(func);
        }
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkDeviceTest, SetCurrentFunctions01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();


} // namespace
BENCHMARK_MAIN();