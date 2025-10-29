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

class UsbmgrBenchmarkPortManagerTest : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State &state);
    void TearDown(const ::benchmark::State &state);
};

void UsbmgrBenchmarkPortManagerTest::SetUp(const ::benchmark::State &state)
{
    // initialization
    UsbCommonTest::GrantPermissionSysNative();
    std::vector<UsbDevice> devices;
    (void)g_usbSrvClient.GetDevices(devices);
    ASSERT_NE(devices.size(), 0);
    g_device = devices.back();
}

void UsbmgrBenchmarkPortManagerTest::TearDown(const ::benchmark::State &state)
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
BENCHMARK_F(UsbmgrBenchmarkPortManagerTest, GetPorts01)(benchmark::State &state)
{
    int32_t ret;
    std::vector<UsbPort> usbPorts;
    for (auto _ : state) {
        ret = g_usbSrvClient.GetPorts(usbPorts);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkPortManagerTest, GetPorts01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();


} // namespace
BENCHMARK_MAIN();