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

#include "serial_test.h"

#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "v1_0/serial_types.h"
#include "usb_srv_client.h"
#include "usb_errors.h"
#include "napi_util.h"
#include "hilog_wrapper.h"
#include "grant_sys_permission.h"

using namespace testing::ext;
using namespace OHOS::HDI::Usb::Serial::V1_0;
using namespace OHOS::Security::AccessToken;

using OHOS::USB::UsbSrvClient;
using OHOS::USB::MODULE_USB_SERVICE;
using OHOS::USB::USB_MGR_LABEL;
using OHOS::USB::UEC_INTERFACE_TIMED_OUT;
using OHOS::USB::UEC_SERIAL_PORT_NOT_EXIST;

constexpr int32_t VALID_PORTID = 0;
constexpr int32_t INVALID_PORTID = -1;
constexpr int32_t OK = 0;
constexpr int32_t ONE_KBYTE = 1024;
constexpr int32_t ONE_SECOND = 1000;
constexpr int32_t MAX_MEMORY = 8192;
static std::vector<OHOS::HDI::Usb::Serial::V1_0::SerialPort> g_portList;

template<typename T>
std::shared_ptr<T> make_shared_array(size_t size)
{
    if (size == 0) {
        return NULL;
    }
    if (size > MAX_MEMORY) {
        return NULL;
    }
    T* buffer = new (std::nothrow)T[size];
    if (!buffer) {
        return NULL;
    }
    return std::shared_ptr<T>(buffer, [] (T* p) { delete[] p; });
}

namespace OHOS {
namespace SERIAL {
void SerialTest::SetUpTestCase(void)
{
    GrantPermissionSysNative();
    UsbSrvClient::GetInstance().SerialGetPortList(g_portList);
    UsbSrvClient::GetInstance().RequestSerialRight(VALID_PORTID);
    if (!UsbSrvClient::GetInstance().HasSerialRight(VALID_PORTID)) {
        return;
    }
}

void SerialTest::TearDownTestCase(void) {}

void SerialTest::SetUp(void) {}

void SerialTest::TearDown(void) {}

/**
 * @tc.name: SerialGetPortList_001
 * @tc.desc: Test functions to int32_t SerialGetPortList(std::vector<OHOS::HDI::Usb::Serial::V1_0::SerialPort>&
 *           portList)
 * @tc.type: FUNC
 */
HWTEST_F(SerialTest, SerialGetPortList_001, TestSize.Level1)
{
    EXPECT_NE(g_portList.size(), 0);
    EXPECT_EQ(g_portList[0].portId, VALID_PORTID);
}

/**
 * @tc.name: SerialOpen_001
 * @tc.desc: Test functions to int32_t SerialOpen(int32_t portId)
 * @tc.type: FUNC
 */
HWTEST_F(SerialTest, SerialOpen_001, TestSize.Level1)
{
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
    EXPECT_EQ(UsbSrvClient::GetInstance().SerialOpen(VALID_PORTID), OK);
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
}

/**
 * @tc.name: SerialOpen_002
 * @tc.desc: Test functions to int32_t SerialOpen(int32_t portId)
 * @tc.type: FUNC
 */
HWTEST_F(SerialTest, SerialOpen_002, TestSize.Level1)
{
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
    UsbSrvClient::GetInstance().SerialOpen(VALID_PORTID);
    EXPECT_NE(UsbSrvClient::GetInstance().SerialOpen(VALID_PORTID), OK);
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
}

/**
 * @tc.name: SerialOpen_003
 * @tc.desc: Test functions to int32_t SerialOpen(int32_t portId)
 * @tc.type: FUNC
 */
HWTEST_F(SerialTest, SerialOpen_003, TestSize.Level1)
{
    UsbSrvClient::GetInstance().SerialClose(INVALID_PORTID);
    UsbSrvClient::GetInstance().SerialOpen(INVALID_PORTID);
    EXPECT_NE(UsbSrvClient::GetInstance().SerialOpen(INVALID_PORTID), OK);
    UsbSrvClient::GetInstance().SerialClose(INVALID_PORTID);
}

/**
 * @tc.name: SerialWrite_001
 * @tc.desc: Test functions to int32_t SerialWrite(int32_t portId, const std::vector<uint8_t> &data,
 *           uint32_t size, uint32_t timeout)
 * @tc.type: FUNC
 */
HWTEST_F(SerialTest, SerialWrite_001, TestSize.Level1)
{
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
    UsbSrvClient::GetInstance().SerialOpen(VALID_PORTID);
    std::vector<uint8_t> data = { 't', 'e', 's', 't' };
    uint32_t actualSize = 0;
    int32_t ret = UsbSrvClient::GetInstance().SerialWrite(VALID_PORTID, data, data.size(), actualSize, 0);
    EXPECT_EQ(ret, OK);
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
}

/**
 * @tc.name: SerialWrite_002
 * @tc.desc: Test functions to int32_t SerialWrite(int32_t portId, const std::vector<uint8_t> &data,
 *           uint32_t size, uint32_t timeout)
 * @tc.type: FUNC
 */
HWTEST_F(SerialTest, SerialWrite_002, TestSize.Level1)
{
    UsbSrvClient::GetInstance().SerialClose(INVALID_PORTID);
    UsbSrvClient::GetInstance().SerialOpen(INVALID_PORTID);
    std::vector<uint8_t> data = { 't', 'e', 's', 't' };
    uint32_t actualSize = 0;
    int32_t ret = UsbSrvClient::GetInstance().SerialWrite(INVALID_PORTID, data, data.size(), actualSize, 0);
    EXPECT_EQ(ret, UEC_SERIAL_PORT_NOT_EXIST);
    UsbSrvClient::GetInstance().SerialClose(INVALID_PORTID);
}

/**
 * @tc.name: SerialRead_001
 * @tc.desc: Test functions to int32_t SerialRead(int32_t portId, uint8_t *data,
 *           uint32_t size, uint32_t timeout)
 * @tc.type: FUNC
 */
HWTEST_F(SerialTest, SerialRead_001, TestSize.Level1)
{
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
    UsbSrvClient::GetInstance().SerialOpen(VALID_PORTID);
    std::shared_ptr<uint8_t> data = make_shared_array<uint8_t>(ONE_KBYTE);
    uint32_t actualSize = 0;
    int32_t ret = UsbSrvClient::GetInstance().SerialRead(VALID_PORTID, data.get(), ONE_KBYTE, actualSize, ONE_SECOND);
    EXPECT_EQ(ret, UEC_INTERFACE_TIMED_OUT);
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
}

/**
 * @tc.name: SerialRead_002
 * @tc.desc: Test functions to int32_t SerialRead(int32_t portId, uint8_t *data,
 *           uint32_t size, uint32_t timeout)
 * @tc.type: FUNC
 */
HWTEST_F(SerialTest, SerialRead_002, TestSize.Level1)
{
    UsbSrvClient::GetInstance().SerialClose(INVALID_PORTID);
    UsbSrvClient::GetInstance().SerialOpen(INVALID_PORTID);
    std::shared_ptr<uint8_t> data = make_shared_array<uint8_t>(ONE_KBYTE);
    uint32_t actualSize = 0;
    int32_t ret = UsbSrvClient::GetInstance().SerialRead(INVALID_PORTID, data.get(), ONE_KBYTE, actualSize, 0);
    EXPECT_EQ(ret, UEC_SERIAL_PORT_NOT_EXIST);
    UsbSrvClient::GetInstance().SerialClose(INVALID_PORTID);
}

/**
 * @tc.name: SerialRead_003
 * @tc.desc: Test functions to int32_t SerialRead(int32_t portId, uint8_t *data,
 *           uint32_t size, uint32_t timeout)
 * @tc.type: FUNC
 */
HWTEST_F(SerialTest, SerialRead_003, TestSize.Level1)
{
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
    UsbSrvClient::GetInstance().SerialOpen(VALID_PORTID);
    std::cout << "Please open the serial port tool to send data in ";
    std::cout << "a single transmission, and press Enter to continue" << std::endl;
    getchar();
    std::shared_ptr<uint8_t> data = make_shared_array<uint8_t>(ONE_KBYTE);
    uint32_t actualSize = 0;
    int32_t ret = UsbSrvClient::GetInstance().SerialRead(VALID_PORTID, data.get(), ONE_KBYTE, actualSize, 0);
    EXPECT_EQ(ret, 0);
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
}

/**
 * @tc.name: SerialGetAttribute_001
 * @tc.desc: Test functions to int32_t SerialGetAttribute(int32_t portId,
 *           OHOS::HDI::Usb::Serial::V1_0::SerialAttribute& attributeInfo)
 * @tc.type: FUNC
 */
HWTEST_F(SerialTest, SerialGetAttribute_001, TestSize.Level1)
{
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
    UsbSrvClient::GetInstance().SerialOpen(VALID_PORTID);
    OHOS::HDI::Usb::Serial::V1_0::SerialAttribute attributeInfo;
    int32_t ret = UsbSrvClient::GetInstance().SerialGetAttribute(VALID_PORTID, attributeInfo);
    EXPECT_EQ(ret, OK);
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
}

/**
 * @tc.name: SerialGetAttribute_002
 * @tc.desc: Test functions to int32_t SerialGetAttribute(int32_t portId,
 *           OHOS::HDI::Usb::Serial::V1_0::SerialAttribute& attributeInfo)
 * @tc.type: FUNC
 */
HWTEST_F(SerialTest, SerialGetAttribute_002, TestSize.Level1)
{
    UsbSrvClient::GetInstance().SerialClose(INVALID_PORTID);
    UsbSrvClient::GetInstance().SerialOpen(INVALID_PORTID);
    OHOS::HDI::Usb::Serial::V1_0::SerialAttribute attributeInfo;
    int32_t ret = UsbSrvClient::GetInstance().SerialGetAttribute(INVALID_PORTID, attributeInfo);
    EXPECT_NE(ret, OK);
    UsbSrvClient::GetInstance().SerialClose(INVALID_PORTID);
}

/**
 * @tc.name: SerialSetAttribute_001
 * @tc.desc: Test functions to int32_t SerialSetAttribute(int32_t portId,
 *           const OHOS::HDI::Usb::Serial::V1_0::SerialAttribute& attributeInfo)
 * @tc.type: FUNC
 */
HWTEST_F(SerialTest, SerialSetAttribute_001, TestSize.Level1)
{
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
    UsbSrvClient::GetInstance().SerialOpen(VALID_PORTID);
    OHOS::HDI::Usb::Serial::V1_0::SerialAttribute attributeInfo;
    attributeInfo.baudrate = OHOS::HDI::Usb::Serial::V1_0::BAUDRATE_576000;
    attributeInfo.dataBits = OHOS::HDI::Usb::Serial::V1_0::USB_ATTR_DATABIT_6;
    attributeInfo.parity = OHOS::HDI::Usb::Serial::V1_0::USB_ATTR_PARITY_ODD;
    attributeInfo.stopBits = OHOS::HDI::Usb::Serial::V1_0::USB_ATTR_STOPBIT_2;
    int32_t ret = UsbSrvClient::GetInstance().SerialSetAttribute(VALID_PORTID, attributeInfo);
    EXPECT_EQ(ret, OK);
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
}

/**
 * @tc.name: SerialSetAttribute_002
 * @tc.desc: Test functions to int32_t SerialSetAttribute(int32_t portId,
 *           const OHOS::HDI::Usb::Serial::V1_0::SerialAttribute& attributeInfo)
 * @tc.type: FUNC
 */
HWTEST_F(SerialTest, SerialSetAttribute_002, TestSize.Level1)
{
    UsbSrvClient::GetInstance().SerialClose(INVALID_PORTID);
    UsbSrvClient::GetInstance().SerialOpen(INVALID_PORTID);
    OHOS::HDI::Usb::Serial::V1_0::SerialAttribute attributeInfo;
    attributeInfo.baudrate = OHOS::HDI::Usb::Serial::V1_0::BAUDRATE_576000;
    attributeInfo.dataBits = OHOS::HDI::Usb::Serial::V1_0::USB_ATTR_DATABIT_6;
    attributeInfo.parity = OHOS::HDI::Usb::Serial::V1_0::USB_ATTR_PARITY_ODD;
    attributeInfo.stopBits = OHOS::HDI::Usb::Serial::V1_0::USB_ATTR_STOPBIT_2;
    int32_t ret = UsbSrvClient::GetInstance().SerialSetAttribute(INVALID_PORTID, attributeInfo);
    EXPECT_NE(ret, OK);
    UsbSrvClient::GetInstance().SerialClose(INVALID_PORTID);
}

/**
 * @tc.name: SerialClose_001
 * @tc.desc: Test functions to int32_t SerialClose(int32_t portId)
 * @tc.type: FUNC
 */
HWTEST_F(SerialTest, SerialClose_001, TestSize.Level1)
{
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
    UsbSrvClient::GetInstance().SerialOpen(VALID_PORTID);
    EXPECT_EQ(UsbSrvClient::GetInstance().SerialClose(VALID_PORTID), OK);
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
}

/**
 * @tc.name: SerialClose_002
 * @tc.desc: Test functions to int32_t SerialClose(int32_t portId)
 * @tc.type: FUNC
 */
HWTEST_F(SerialTest, SerialClose_002, TestSize.Level1)
{
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
    UsbSrvClient::GetInstance().SerialOpen(VALID_PORTID);
    UsbSrvClient::GetInstance().SerialClose(VALID_PORTID);
    EXPECT_NE(UsbSrvClient::GetInstance().SerialClose(VALID_PORTID), OK);
}
}
}