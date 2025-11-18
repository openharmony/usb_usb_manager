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
USBAccessory g_access;

constexpr int32_t ITERATION_FREQUENCY = 100;
constexpr int32_t REPETITION_FREQUENCY = 3;

// benchmark test for functions of usb port (EXCEPT callback)
class UsbmgrBenchmarkAccessoryTest : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State &state);
    void TearDown(const ::benchmark::State &state);
};

void UsbmgrBenchmarkAccessoryTest::SetUp(const ::benchmark::State &state)
{
    // initialization
    UsbCommonTest::GrantPermissionSysNative();
    std::vector<USBAccessory> accessList;
    g_usbSrvClient.GetAccessoryList(accessList);
    ASSERT_NE(0, accessList.size());
    g_access = accessList.front();
}

void UsbmgrBenchmarkAccessoryTest::TearDown(const ::benchmark::State &state)
{
    // end of the test
    ;
}

/**
 * @tc.name: GetAccessoryList01
 * @tc.desc: Test usbmgr functions: GetAccessoryList
 * @tc.desc: int32_t GetAccessoryList(std::vector<USBAccessory> &accessList);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkAccessoryTest, GetAccessoryList01)(benchmark::State &state)
{
    int32_t ret;
    std::vector<USBAccessory> accessList;
    for (auto _ : state) {
        ret = g_usbSrvClient.GetAccessoryList(saccessList);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkAccessoryTest, GetAccessoryList01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: HasAccessoryRight01
 * @tc.desc: Test usbmgr functions: HasAccessoryRight
 * @tc.desc: int32_t HasAccessoryRight(const USBAccessory &access, bool &result);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkAccessoryTest, HasAccessoryRight01)(benchmark::State &state)
{
    int32_t ret;
    bool result;
    for (auto _ : state) {
        ret = g_usbSrvClient.HasAccessoryRight(g_access, result);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkAccessoryTest, HasAccessoryRight01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();


} // namespace
BENCHMARK_MAIN();