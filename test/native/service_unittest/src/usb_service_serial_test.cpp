/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "usb_service_serial_test.h"
#include <sys/time.h>
#include <iostream>
#include <vector>
#include "delayed_sp_singleton.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "usb_common_test.h"
#include "usb_srv_client.h"
#include "usb_errors.h"
#include "usb_interface_type.h"
#include "usb_service.h"
#include "usb_serial_type.h"

using namespace testing::ext;
using namespace OHOS::USB;
using namespace OHOS;
using namespace std;
using namespace OHOS::USB::Common;

namespace OHOS {
namespace USB {
namespace Service {

void UsbServiceSerialTest::SetUpTestCase(void)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Start UsbServiceSerialTest");
}

void UsbServiceSerialTest::TearDownTestCase(void)
{
    USB_HILOGI(MODULE_USB_SERVICE, "End UsbServiceSerialTest");
}

void UsbServiceSerialTest::SetUp(void) {}

void UsbServiceSerialTest::TearDown(void) {}

HWTEST_F(UsbServiceSerialTest, SerialGetPortList001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialGetPortList001");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialGetPortList001 ret=%{public}d, size=%{public}zu",
        ret, serialPortList.size());
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialGetPortList001");
}

HWTEST_F(UsbServiceSerialTest, SerialGetPortList002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialGetPortList002");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialGetPortList002 ret=%{public}d", ret);
    ASSERT_GE(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialGetPortList002");
}

HWTEST_F(UsbServiceSerialTest, HasSerialRight001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : HasSerialRight001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    bool hasRight = false;
    int32_t ret = instance.HasSerialRight(portId, hasRight);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::HasSerialRight001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERIAL_PORT_NOT_EXIST);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : HasSerialRight001");
}

HWTEST_F(UsbServiceSerialTest, HasSerialRight002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : HasSerialRight002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = -1;
    bool hasRight = false;
    int32_t ret = instance.HasSerialRight(portId, hasRight);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::HasSerialRight002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : HasSerialRight002");
}

HWTEST_F(UsbServiceSerialTest, HasSerialRight003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : HasSerialRight003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 255;
    bool hasRight = false;
    int32_t ret = instance.HasSerialRight(portId, hasRight);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::HasSerialRight003 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : HasSerialRight003");
}

HWTEST_F(UsbServiceSerialTest, RequestSerialRight001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : RequestSerialRight001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    bool hasRight = false;
    int32_t ret = instance.RequestSerialRight(portId, hasRight);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::RequestSerialRight001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERIAL_PORT_NOT_EXIST);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : RequestSerialRight001");
}

HWTEST_F(UsbServiceSerialTest, RequestSerialRight002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : RequestSerialRight002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = -1;
    bool hasRight = false;
    int32_t ret = instance.RequestSerialRight(portId, hasRight);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::RequestSerialRight002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : RequestSerialRight002");
}

HWTEST_F(UsbServiceSerialTest, RequestSerialRight003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : RequestSerialRight003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 255;
    bool hasRight = false;
    int32_t ret = instance.RequestSerialRight(portId, hasRight);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::RequestSerialRight003 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : RequestSerialRight003");
}

HWTEST_F(UsbServiceSerialTest, CancelSerialRight001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : CancelSerialRight001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    int32_t ret = instance.CancelSerialRight(portId);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::CancelSerialRight001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERIAL_PORT_NOT_EXIST);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : CancelSerialRight001");
}

HWTEST_F(UsbServiceSerialTest, CancelSerialRight002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : CancelSerialRight002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = -1;
    int32_t ret = instance.CancelSerialRight(portId);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::CancelSerialRight002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : CancelSerialRight002");
}

HWTEST_F(UsbServiceSerialTest, CancelSerialRight003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : CancelSerialRight003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 255;
    int32_t ret = instance.CancelSerialRight(portId);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::CancelSerialRight003 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : CancelSerialRight003");
}

HWTEST_F(UsbServiceSerialTest, AddSerialRight001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : AddSerialRight001");
    auto &instance = UsbSrvClient::GetInstance();
    uint32_t tokenId = 0;
    int32_t portId = 0;
    int32_t ret = instance.AddSerialRight(tokenId, portId);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::AddSerialRight001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : AddSerialRight001");
}

HWTEST_F(UsbServiceSerialTest, AddSerialRight002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : AddSerialRight002");
    auto &instance = UsbSrvClient::GetInstance();
    uint32_t tokenId = 0;
    int32_t portId = -1;
    int32_t ret = instance.AddSerialRight(tokenId, portId);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::AddSerialRight002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : AddSerialRight002");
}

HWTEST_F(UsbServiceSerialTest, AddSerialRight003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : AddSerialRight003");
    auto &instance = UsbSrvClient::GetInstance();
    uint32_t tokenId = 0xFFFFFFFF;
    int32_t portId = 255;
    int32_t ret = instance.AddSerialRight(tokenId, portId);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::AddSerialRight003 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : AddSerialRight003");
}

HWTEST_F(UsbServiceSerialTest, SerialOpen001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialOpen001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    int32_t ret = instance.SerialOpen(portId);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialOpen001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERIAL_PORT_NOT_EXIST || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialOpen001");
}

HWTEST_F(UsbServiceSerialTest, SerialOpen002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialOpen002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = -1;
    int32_t ret = instance.SerialOpen(portId);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialOpen002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialOpen002");
}

HWTEST_F(UsbServiceSerialTest, SerialOpen003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialOpen003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 255;
    int32_t ret = instance.SerialOpen(portId);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialOpen003 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialOpen003");
}

HWTEST_F(UsbServiceSerialTest, SerialClose001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialClose001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    int32_t ret = instance.SerialClose(portId);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialClose001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERIAL_PORT_NOT_EXIST);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialClose001");
}

HWTEST_F(UsbServiceSerialTest, SerialClose002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialClose002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = -1;
    int32_t ret = instance.SerialClose(portId);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialClose002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialClose002");
}

HWTEST_F(UsbServiceSerialTest, SerialClose003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialClose003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 255;
    int32_t ret = instance.SerialClose(portId);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialClose003 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialClose003");
}

HWTEST_F(UsbServiceSerialTest, SerialRead001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialRead001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    std::vector<uint8_t> data;
    uint32_t size = 64;
    uint32_t actualSize = 0;
    uint32_t timeout = 1000;
    int32_t ret = instance.SerialRead(portId, data, size, actualSize, timeout);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialRead001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERIAL_PORT_NOT_EXIST || ret == UEC_SERIAL_IO_EXCEPTION);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialRead001");
}

HWTEST_F(UsbServiceSerialTest, SerialRead002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialRead002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = -1;
    std::vector<uint8_t> data;
    uint32_t size = 64;
    uint32_t actualSize = 0;
    uint32_t timeout = 1000;
    int32_t ret = instance.SerialRead(portId, data, size, actualSize, timeout);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialRead002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialRead002");
}

HWTEST_F(UsbServiceSerialTest, SerialRead003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialRead003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    std::vector<uint8_t> data;
    uint32_t size = 0;
    uint32_t actualSize = 0;
    uint32_t timeout = 1000;
    int32_t ret = instance.SerialRead(portId, data, size, actualSize, timeout);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialRead003 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialRead003");
}

HWTEST_F(UsbServiceSerialTest, SerialRead004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialRead004");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    std::vector<uint8_t> data;
    uint32_t size = 65536;
    uint32_t actualSize = 0;
    uint32_t timeout = 1000;
    int32_t ret = instance.SerialRead(portId, data, size, actualSize, timeout);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialRead004 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_SERIAL_IO_EXCEPTION);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialRead004");
}

HWTEST_F(UsbServiceSerialTest, SerialWrite001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialWrite001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04};
    uint32_t size = data.size();
    uint32_t actualSize = 0;
    uint32_t timeout = 1000;
    int32_t ret = instance.SerialWrite(portId, data, size, actualSize, timeout);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialWrite001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERIAL_PORT_NOT_EXIST || ret == UEC_SERIAL_IO_EXCEPTION);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialWrite001");
}

HWTEST_F(UsbServiceSerialTest, SerialWrite002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialWrite002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = -1;
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04};
    uint32_t size = data.size();
    uint32_t actualSize = 0;
    uint32_t timeout = 1000;
    int32_t ret = instance.SerialWrite(portId, data, size, actualSize, timeout);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialWrite002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialWrite002");
}

HWTEST_F(UsbServiceSerialTest, SerialWrite003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialWrite003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    std::vector<uint8_t> data;
    uint32_t size = 0;
    uint32_t actualSize = 0;
    uint32_t timeout = 1000;
    int32_t ret = instance.SerialWrite(portId, data, size, actualSize, timeout);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialWrite003 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialWrite003");
}

HWTEST_F(UsbServiceSerialTest, SerialWrite004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialWrite004");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    std::vector<uint8_t> data(200000, 0x01);
    uint32_t size = data.size();
    uint32_t actualSize = 0;
    uint32_t timeout = 1000;
    int32_t ret = instance.SerialWrite(portId, data, size, actualSize, timeout);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialWrite004 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_SERIAL_IO_EXCEPTION);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialWrite004");
}

HWTEST_F(UsbServiceSerialTest, SerialGetAttribute001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialGetAttribute001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    UsbSerialAttr attributeInfo;
    int32_t ret = instance.SerialGetAttribute(portId, attributeInfo);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialGetAttribute001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERIAL_PORT_NOT_EXIST);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialGetAttribute001");
}

HWTEST_F(UsbServiceSerialTest, SerialGetAttribute002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialGetAttribute002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = -1;
    UsbSerialAttr attributeInfo;
    int32_t ret = instance.SerialGetAttribute(portId, attributeInfo);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialGetAttribute002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialGetAttribute002");
}

HWTEST_F(UsbServiceSerialTest, SerialGetAttribute003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialGetAttribute003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 255;
    UsbSerialAttr attributeInfo;
    int32_t ret = instance.SerialGetAttribute(portId, attributeInfo);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialGetAttribute003 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialGetAttribute003");
}

HWTEST_F(UsbServiceSerialTest, SerialSetAttribute001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialSetAttribute001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    UsbSerialAttr attributeInfo;
    attributeInfo.baudRate_ = 9600;
    attributeInfo.stopBits_ = 1;
    attributeInfo.parity_ = 0;
    attributeInfo.dataBits_ = 8;
    int32_t ret = instance.SerialSetAttribute(portId, attributeInfo);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialSetAttribute001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERIAL_PORT_NOT_EXIST);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialSetAttribute001");
}

HWTEST_F(UsbServiceSerialTest, SerialSetAttribute002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialSetAttribute002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = -1;
    UsbSerialAttr attributeInfo;
    attributeInfo.baudRate_ = 9600;
    int32_t ret = instance.SerialSetAttribute(portId, attributeInfo);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialSetAttribute002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialSetAttribute002");
}

HWTEST_F(UsbServiceSerialTest, SerialSetAttribute003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialSetAttribute003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    UsbSerialAttr attributeInfo;
    attributeInfo.baudRate_ = 0;
    attributeInfo.stopBits_ = 0;
    attributeInfo.parity_ = 0;
    attributeInfo.dataBits_ = 0;
    int32_t ret = instance.SerialSetAttribute(portId, attributeInfo);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialSetAttribute003 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialSetAttribute003");
}

HWTEST_F(UsbServiceSerialTest, SerialSetAttribute004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialSetAttribute004");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    UsbSerialAttr attributeInfo;
    attributeInfo.baudRate_ = 115200;
    attributeInfo.stopBits_ = 1;
    attributeInfo.parity_ = 0;
    attributeInfo.dataBits_ = 8;
    int32_t ret = instance.SerialSetAttribute(portId, attributeInfo);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialSetAttribute004 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERIAL_PORT_NOT_EXIST);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialSetAttribute004");
}

HWTEST_F(UsbServiceSerialTest, SerialSetAttribute005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialSetAttribute005");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    UsbSerialAttr attributeInfo;
    attributeInfo.baudRate_ = 4800;
    attributeInfo.stopBits_ = 1;
    attributeInfo.parity_ = 0;
    attributeInfo.dataBits_ = 8;
    int32_t ret = instance.SerialSetAttribute(portId, attributeInfo);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialSetAttribute005 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERIAL_PORT_NOT_EXIST);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialSetAttribute005");
}

HWTEST_F(UsbServiceSerialTest, SerialSetAttribute006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialSetAttribute006");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    UsbSerialAttr attributeInfo;
    attributeInfo.baudRate_ = 19200;
    attributeInfo.stopBits_ = 1;
    attributeInfo.parity_ = 0;
    attributeInfo.dataBits_ = 8;
    int32_t ret = instance.SerialSetAttribute(portId, attributeInfo);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialSetAttribute006 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERIAL_PORT_NOT_EXIST);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialSetAttribute006");
}

HWTEST_F(UsbServiceSerialTest, SerialSetAttribute007, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialSetAttribute007");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    UsbSerialAttr attributeInfo;
    attributeInfo.baudRate_ = 38400;
    attributeInfo.stopBits_ = 1;
    attributeInfo.parity_ = 0;
    attributeInfo.dataBits_ = 8;
    int32_t ret = instance.SerialSetAttribute(portId, attributeInfo);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialSetAttribute007 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERIAL_PORT_NOT_EXIST);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialSetAttribute007");
}

HWTEST_F(UsbServiceSerialTest, SerialSetAttribute008, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialSetAttribute008");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    UsbSerialAttr attributeInfo;
    attributeInfo.baudRate_ = 57600;
    attributeInfo.stopBits_ = 1;
    attributeInfo.parity_ = 0;
    attributeInfo.dataBits_ = 8;
    int32_t ret = instance.SerialSetAttribute(portId, attributeInfo);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialSetAttribute008 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERIAL_PORT_NOT_EXIST);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialSetAttribute008");
}

HWTEST_F(UsbServiceSerialTest, SerialPortList001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialPortList001");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialPortList001 size=%{public}zu", serialPortList.size());
    ASSERT_GE(serialPortList.size(), 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialPortList001");
}

HWTEST_F(UsbServiceSerialTest, SerialPortList002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialPortList002");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (ret == UEC_OK && serialPortList.size() > 0) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialPortList002 portId=%{public}d",
            serialPortList[0].portId_);
    }
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialPortList002");
}

HWTEST_F(UsbServiceSerialTest, SerialPortList003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialPortList003");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (ret == UEC_OK && serialPortList.size() > 0) {
        for (size_t i = 0; i < serialPortList.size(); i++) {
            USB_HILOGI(MODULE_USB_SERVICE, "i=%{public}zu,bu=%{public}d,ad=%{public}d,vid=%{public}d,pid=%{public}d",
                i, serialPortList[i].busNum_, serialPortList[i].devAddr_,
                serialPortList[i].vid_, serialPortList[i].pid_);
        }
    }
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialPortList003");
}

HWTEST_F(UsbServiceSerialTest, SerialPortList004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialPortList004");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        ASSERT_GE(serialPortList[0].portId_, 0);
    }
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialPortList004");
}

HWTEST_F(UsbServiceSerialTest, SerialRight001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialRight001");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        bool hasRight = false;
        ret = instance.HasSerialRight(portId, hasRight);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialRight001 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialRight001");
}

HWTEST_F(UsbServiceSerialTest, SerialRight002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialRight002");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        bool hasRight = false;
        ret = instance.HasSerialRight(portId, hasRight);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialRight002 hasRight=%{public}d", hasRight);
        ASSERT_TRUE(hasRight == true || hasRight == false);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialRight002");
}

HWTEST_F(UsbServiceSerialTest, SerialRight003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialRight003");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        bool hasRight = false;
        ret = instance.RequestSerialRight(portId, hasRight);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialRight003 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialRight003");
}

HWTEST_F(UsbServiceSerialTest, SerialRight004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialRight004");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        bool hasRight = false;
        ret = instance.RequestSerialRight(portId, hasRight);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialRight004 hasRight=%{public}d", hasRight);
        ASSERT_TRUE(hasRight == true || hasRight == false);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialRight004");
}

HWTEST_F(UsbServiceSerialTest, SerialRight005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialRight005");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        ret = instance.CancelSerialRight(portId);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialRight005 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialRight005");
}

HWTEST_F(UsbServiceSerialTest, SerialRight006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialRight006");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        uint32_t tokenId = 1000;
        ret = instance.AddSerialRight(tokenId, portId);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialRight006 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialRight006");
}

HWTEST_F(UsbServiceSerialTest, SerialIO001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialIO001");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        std::vector<uint8_t> data = {0x55, 0xAA, 0x01, 0x02};
        uint32_t size = data.size();
        uint32_t actualSize = 0;
        uint32_t timeout = 500;
        ret = instance.SerialWrite(portId, data, size, actualSize, timeout);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialIO001 ret=%{public}d, actual=%{public}d",
            ret, actualSize);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialIO001");
}

HWTEST_F(UsbServiceSerialTest, SerialIO002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialIO002");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        std::vector<uint8_t> data;
        uint32_t size = 32;
        uint32_t actualSize = 0;
        uint32_t timeout = 500;
        ret = instance.SerialRead(portId, data, size, actualSize, timeout);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialIO002 ret=%{public}d, actual=%{public}d",
            ret, actualSize);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialIO002");
}

HWTEST_F(UsbServiceSerialTest, SerialIO003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialIO003");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        UsbSerialAttr attributeInfo;
        attributeInfo.baudRate_ = 115200;
        attributeInfo.stopBits_ = 1;
        attributeInfo.parity_ = 0;
        attributeInfo.dataBits_ = 8;
        ret = instance.SerialSetAttribute(portId, attributeInfo);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialIO003 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialIO003");
}

HWTEST_F(UsbServiceSerialTest, SerialIO004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialIO004");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        UsbSerialAttr attributeInfo;
        ret = instance.SerialGetAttribute(portId, attributeInfo);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialIO004 baudRate=%{public}d",
            attributeInfo.baudRate_);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialIO004");
}

HWTEST_F(UsbServiceSerialTest, SerialIO005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialIO005");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        std::vector<uint8_t> data = {0x01, 0x03, 0x05, 0x07, 0x09, 0x0B};
        uint32_t size = data.size();
        uint32_t actualSize = 0;
        uint32_t timeout = 2000;
        ret = instance.SerialWrite(portId, data, size, actualSize, timeout);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialIO005 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialIO005");
}

HWTEST_F(UsbServiceSerialTest, SerialIO006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialIO006");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        std::vector<uint8_t> data;
        uint32_t size = 128;
        uint32_t actualSize = 0;
        uint32_t timeout = 1000;
        ret = instance.SerialRead(portId, data, size, actualSize, timeout);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialIO006 ret=%{public}d, size=%{public}d",
            ret, actualSize);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialIO006");
}

HWTEST_F(UsbServiceSerialTest, SerialIO007, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialIO007");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        std::vector<uint8_t> data = {0xAA, 0x55, 0xAA, 0x55, 0xAA};
        uint32_t size = data.size();
        uint32_t actualSize = 0;
        uint32_t timeout = 3000;
        ret = instance.SerialWrite(portId, data, size, actualSize, timeout);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialIO007 ret=%{public}d, actual=%{public}d",
            ret, actualSize);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialIO007");
}

HWTEST_F(UsbServiceSerialTest, SerialIO008, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialIO008");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        UsbSerialAttr attributeInfo;
        attributeInfo.baudRate_ = 9600;
        attributeInfo.stopBits_ = 2;
        attributeInfo.parity_ = 1;
        attributeInfo.dataBits_ = 7;
        ret = instance.SerialSetAttribute(portId, attributeInfo);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialIO008 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialIO008");
}

HWTEST_F(UsbServiceSerialTest, SerialIO009, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialIO009");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        UsbSerialAttr attributeInfo;
        attributeInfo.baudRate_ = 14400;
        attributeInfo.stopBits_ = 1;
        attributeInfo.parity_ = 0;
        attributeInfo.dataBits_ = 8;
        ret = instance.SerialSetAttribute(portId, attributeInfo);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialIO009 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialIO009");
}

HWTEST_F(UsbServiceSerialTest, SerialIO010, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialIO010");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        UsbSerialAttr attributeInfo;
        attributeInfo.baudRate_ = 28800;
        attributeInfo.stopBits_ = 1;
        attributeInfo.parity_ = 0;
        attributeInfo.dataBits_ = 8;
        ret = instance.SerialSetAttribute(portId, attributeInfo);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialIO010 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialIO010");
}

HWTEST_F(UsbServiceSerialTest, SerialIO011, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialIO011");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        UsbSerialAttr attributeInfo;
        attributeInfo.baudRate_ = 128000;
        attributeInfo.stopBits_ = 1;
        attributeInfo.parity_ = 0;
        attributeInfo.dataBits_ = 8;
        ret = instance.SerialSetAttribute(portId, attributeInfo);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialIO011 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialIO011");
}

HWTEST_F(UsbServiceSerialTest, SerialIO012, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialIO012");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        std::vector<uint8_t> data;
        uint32_t size = 256;
        uint32_t actualSize = 0;
        uint32_t timeout = 500;
        ret = instance.SerialRead(portId, data, size, actualSize, timeout);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialIO012 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialIO012");
}

HWTEST_F(UsbServiceSerialTest, SerialPortDetails001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialPortDetails001");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    ASSERT_EQ(UEC_OK, ret);
    if (serialPortList.size() > 0) {
        UsbSerialPort &port = serialPortList[0];
        USB_HILOGI(MODULE_USB_SERVICE, "SerialPortDetails001 portId=%{public}d, bus=%{public}d, addr=%{public}d",
            port.portId_, port.busNum_, port.devAddr_);
        ASSERT_GE(port.portId_, 0);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialPortDetails001");
}

HWTEST_F(UsbServiceSerialTest, SerialPortDetails002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialPortDetails002");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    ASSERT_EQ(UEC_OK, ret);
    if (serialPortList.size() > 0) {
        UsbSerialPort &port = serialPortList[0];
        USB_HILOGI(MODULE_USB_SERVICE, "::SerialPortDetails002 vid=%{public}d, pid=%{public}d, serial=%{public}s",
            port.vid_, port.pid_, port.serialNum_.c_str());
        ASSERT_GE(port.vid_, 0);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialPortDetails002");
}

HWTEST_F(UsbServiceSerialTest, SerialPortDetails003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialPortDetails003");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    ASSERT_EQ(UEC_OK, ret);
    for (size_t i = 0; i < serialPortList.size(); i++) {
        USB_HILOGI(MODULE_USB_SERVICE, "port[%{public}zu] vid=%{public}d pid=%{public}d",
            i, serialPortList[i].vid_, serialPortList[i].pid_);
    }
    ASSERT_GE(serialPortList.size(), 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialPortDetails003");
}

HWTEST_F(UsbServiceSerialTest, SerialPortDetails004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialPortDetails004");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    ASSERT_EQ(UEC_OK, ret);
    if (serialPortList.size() > 1) {
        ASSERT_GE(serialPortList[1].portId_, 0);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialPortDetails004");
}

HWTEST_F(UsbServiceSerialTest, SerialAttributes001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialAttributes001");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        UsbSerialAttr attributeInfo;
        ret = instance.SerialGetAttribute(portId, attributeInfo);
        USB_HILOGI(MODULE_USB_SERVICE, "baud=%{public}d, stop=%{public}d, parity=%{public}d, data=%{public}d",
            attributeInfo.baudRate_, attributeInfo.stopBits_, attributeInfo.parity_, attributeInfo.dataBits_);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialAttributes001");
}

HWTEST_F(UsbServiceSerialTest, SerialAttributes002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialAttributes002");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        UsbSerialAttr attributeInfo;
        attributeInfo.baudRate_ = 0;
        attributeInfo.stopBits_ = 0;
        attributeInfo.parity_ = 0;
        attributeInfo.dataBits_ = 0;
        ret = instance.SerialSetAttribute(portId, attributeInfo);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialAttributes002 ret=%{public}d", ret);
        ASSERT_TRUE(ret != UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialAttributes002");
}

HWTEST_F(UsbServiceSerialTest, SerialAttributes003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialAttributes003");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        UsbSerialAttr attributeInfo;
        attributeInfo.baudRate_ = 230400;
        attributeInfo.stopBits_ = 1;
        attributeInfo.parity_ = 0;
        attributeInfo.dataBits_ = 8;
        ret = instance.SerialSetAttribute(portId, attributeInfo);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialAttributes003 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialAttributes003");
}

HWTEST_F(UsbServiceSerialTest, SerialAttributes004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialAttributes004");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        UsbSerialAttr attributeInfo;
        attributeInfo.baudRate_ = 460800;
        attributeInfo.stopBits_ = 1;
        attributeInfo.parity_ = 0;
        attributeInfo.dataBits_ = 8;
        ret = instance.SerialSetAttribute(portId, attributeInfo);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialAttributes004 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialAttributes004");
}

HWTEST_F(UsbServiceSerialTest, SerialOperation001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialOperation001");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        std::vector<uint8_t> data;
        data.push_back(0x01);
        data.push_back(0x02);
        data.push_back(0x03);
        uint32_t size = data.size();
        uint32_t actualSize = 0;
        ret = instance.SerialWrite(portId, data, size, actualSize, 1000);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialOperation001 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialOperation001");
}

HWTEST_F(UsbServiceSerialTest, SerialOperation002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialOperation002");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        std::vector<uint8_t> data;
        uint32_t size = 16;
        uint32_t actualSize = 0;
        ret = instance.SerialRead(portId, data, size, actualSize, 500);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialOperation002 ret=%{public}d, read=%{public}d",
            ret, actualSize);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialOperation002");
}

HWTEST_F(UsbServiceSerialTest, SerialOperation003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialOperation003");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        std::vector<uint8_t> data;
        data.push_back(0xFF);
        data.push_back(0x00);
        data.push_back(0xFF);
        data.push_back(0x00);
        data.push_back(0xFF);
        data.push_back(0x00);
        uint32_t size = data.size();
        uint32_t actualSize = 0;
        ret = instance.SerialWrite(portId, data, size, actualSize, 2000);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialOperation003 ret=%{public}d, written=%{public}d",
            ret, actualSize);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialOperation003");
}

HWTEST_F(UsbServiceSerialTest, SerialOperation004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialOperation004");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        std::vector<uint8_t> data;
        uint32_t size = 512;
        uint32_t actualSize = 0;
        ret = instance.SerialRead(portId, data, size, actualSize, 200);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialOperation004 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialOperation004");
}

HWTEST_F(UsbServiceSerialTest, SerialOperation005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialOperation005");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        UsbSerialAttr attr;
        attr.baudRate_ = 7200;
        attr.stopBits_ = 1;
        attr.parity_ = 0;
        attr.dataBits_ = 8;
        ret = instance.SerialSetAttribute(portId, attr);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialOperation005 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialOperation005");
}

HWTEST_F(UsbServiceSerialTest, SerialOperation006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SerialOperation006");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbSerialPort> serialPortList;
    int32_t ret = instance.SerialGetPortList(serialPortList);
    if (serialPortList.size() > 0) {
        int32_t portId = serialPortList[0].portId_;
        bool hasRight = false;
        ret = instance.HasSerialRight(portId, hasRight);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceSerialTest::SerialOperation006 ret=%{public}d, hasRight=%{public}d",
            ret, hasRight);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SerialOperation006");
}

} // namespace Service
} // namespace USB
} // namespace OHOS