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
UsbSerialPort g_serialPort;
constexpr uint32_t TIMEOUT_MS = 1000;

constexpr int32_t ITERATION_FREQUENCY = 100;
constexpr int32_t REPETITION_FREQUENCY = 3;

// benchmark test for functions of usb port (EXCEPT callback)
class UsbmgrBenchmarkSerialTest : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State &state);
    void TearDown(const ::benchmark::State &state);
};

void UsbmgrBenchmarkSerialTest::SetUp(const ::benchmark::State &state)
{
    // initialization
    UsbCommonTest::GrantPermissionSysNative();
    std::vector<UsbSerialPort> ports;
    g_usbSrvClient.SerialGetPortList(ports);
    ASSERT_NE(0, ports.size());
    // request right
    bool hasRight;
    g_serialPort = ports.front();
    g_usbSrvClient.RequestSerialRight(g_serialPort.portId_, hasRight);
    (void)g_usbSrvClient.HasSerialRight(g_serialPort.portId_, hasRight);
    ASSERT_EQ(true, hasRight);
}

void UsbmgrBenchmarkSerialTest::TearDown(const ::benchmark::State &state)
{
    // end of the test
    ;
}

/**
 * @tc.name: SerialOpenClose01
 * @tc.desc: Test usbmgr functions: SerialClose, SerialOpen
 * @tc.desc: int32_t SerialClose(int32_t portId);
 * @tc.desc: int32_t SerialOpen(int32_t portId);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkSerialTest, SerialOpenClose01)(benchmark::State &state)
{
    int32_t ret;
    for (auto _ : state) {
        ret = g_usbSrvClient.SerialOpen(g_serialPort.portId_);
        ret = g_usbSrvClient.SerialClose(g_serialPort.portId_);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkSerialTest, SerialOpenClose01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: SerialWrite01
 * @tc.desc: Test usbmgr functions: SerialWrite
 * @tc.desc: int32_t SerialWrite(int32_t portId, const std::vector<uint8_t>& data,
 *      uint32_t bufferSize, uint32_t& actualSize, uint32_t timeout);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkSerialTest, SerialWrite01)(benchmark::State &state)
{
    int32_t ret;
    uint32_t actualSize;
    std::vector<uint8_t> data = {'\0'};
    ret = g_usbSrvClient.SerialOpen(g_serialPort.portId_);
    EXPECT_EQ(0, ret);
    for (auto _ : state) {
        ret = g_usbSrvClient.SerialWrite(g_serialPort.portId_, data, data.size(), actualSize, TIMEOUT_MS);
    }
    EXPECT_EQ(0, ret);
    EXPECT_EQ(0, g_usbSrvClient.SerialClose(g_serialPort.portId_));
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkSerialTest, SerialWrite01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: SerialWrite02
 * @tc.desc: Test usbmgr functions: SerialWrite
 * @tc.desc: int32_t SerialWrite(int32_t portId, const std::vector<uint8_t>& data,
 *      uint32_t bufferSize, uint32_t& actualSize, uint32_t timeout);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkSerialTest, SerialWrite02)(benchmark::State &state)
{
    int32_t ret;
    uint32_t actualSize;
    std::vector<uint8_t> data = {'t', 'e', 's', 't'};
    ret = g_usbSrvClient.SerialOpen(g_serialPort.portId_);
    EXPECT_EQ(0, ret);
    for (auto _ : state) {
        ret = g_usbSrvClient.SerialWrite(g_serialPort.portId_, data, data.size(), actualSize, TIMEOUT_MS);
    }
    EXPECT_EQ(0, ret);
    EXPECT_EQ(0, g_usbSrvClient.SerialClose(g_serialPort.portId_));
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkSerialTest, SerialWrite02)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: SerialGetAttribute01
 * @tc.desc: Test usbmgr functions: SerialGetAttribute
 * @tc.desc: int32_t SerialGetAttribute(int32_t portId, UsbSerialAttr& attribute);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkSerialTest, SerialGetAttribute01)(benchmark::State &state)
{
    int32_t ret;
    UsbSerialAttr attribute;
    ret = g_usbSrvClient.SerialOpen(g_serialPort.portId_);
    EXPECT_EQ(0, ret);
    for (auto _ : state) {
        ret = g_usbSrvClient.SerialGetAttribute(g_serialPort.portId_, attribute);
    }
    EXPECT_EQ(0, ret);
    EXPECT_EQ(0, g_usbSrvClient.SerialClose(g_serialPort.portId_));
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkSerialTest, SerialGetAttribute01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: SerialSetAttribute01
 * @tc.desc: Test usbmgr functions: SerialSetAttribute
 * @tc.desc: int32_t SerialSetAttribute(int32_t portId, const UsbSerialAttr& attribute);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkSerialTest, SerialSetAttribute01)(benchmark::State &state)
{
    int32_t ret;
    UsbSerialAttr attribute;
    ret = g_usbSrvClient.SerialOpen(g_serialPort.portId_);
    EXPECT_EQ(0, ret);
    ret = g_usbSrvClient.SerialGetAttribute(g_serialPort.portId_, attribute);
    EXPECT_EQ(0, ret);
    for (auto _ : state) {
        ret = g_usbSrvClient.SerialSetAttribute(g_serialPort.portId_, attribute);
    }
    EXPECT_EQ(0, ret);
    EXPECT_EQ(0, g_usbSrvClient.SerialClose(g_serialPort.portId_));
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkSerialTest, SerialSetAttribute01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: SerialGetPortList01
 * @tc.desc: Test usbmgr functions: SerialGetPortList
 * @tc.desc: int32_t SerialGetPortList(std::vector<UsbSerialPort>& serialPortList);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkSerialTest, SerialGetPortList01)(benchmark::State &state)
{
    int32_t ret;
    std::vector<UsbSerialPort> serialPortList;
    for (auto _ : state) {
        ret = g_usbSrvClient.SerialGetPortList(serialPortList);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkSerialTest, SerialGetPortList01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();

/**
 * @tc.name: HasSerialRight01
 * @tc.desc: Test usbmgr functions: HasSerialRight
 * @tc.desc: int32_t HasSerialRight(int32_t portId, bool &hasRight);
 * @tc.desc: Positive test: parameters correct
 * @tc.type: FUNC
 */
BENCHMARK_F(UsbmgrBenchmarkSerialTest, HasSerialRight01)(benchmark::State &state)
{
    int32_t ret;
    bool hasRight;
    for (auto _ : state) {
        ret = g_usbSrvClient.HasSerialRight(g_serialPort.portId_, hasRight);
    }
    EXPECT_EQ(0, ret);
}
BENCHMARK_REGISTER_F(UsbmgrBenchmarkSerialTest, HasSerialRight01)->
    Iterations(ITERATION_FREQUENCY)->Repetitions(REPETITION_FREQUENCY)->ReportAggregatesOnly();


} // namespace
BENCHMARK_MAIN();