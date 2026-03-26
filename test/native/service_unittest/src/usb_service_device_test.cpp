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

#include "usb_service_device_test.h"
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

using namespace testing::ext;
using namespace OHOS::USB;
using namespace OHOS;
using namespace std;
using namespace OHOS::USB::Common;

namespace OHOS {
namespace USB {
namespace Service {

void UsbServiceDeviceTest::SetUpTestCase(void)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Start UsbServiceDeviceTest");
}

void UsbServiceDeviceTest::TearDownTestCase(void)
{
    USB_HILOGI(MODULE_USB_SERVICE, "End UsbServiceDeviceTest");
}

void UsbServiceDeviceTest::SetUp(void) {}

void UsbServiceDeviceTest::TearDown(void) {}

#ifdef USB_MANAGER_FEATURE_DEVICE

HWTEST_F(UsbServiceDeviceTest, GetCurrentFunctions001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetCurrentFunctions001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = 0;
    int32_t ret = instance.GetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::GetCurrentFunctions001 ret=%{public}d, funcs=%{public}d",
        ret, funcs);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetCurrentFunctions001");
}

HWTEST_F(UsbServiceDeviceTest, GetCurrentFunctions002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetCurrentFunctions002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = 0;
    int32_t ret = instance.GetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::GetCurrentFunctions002 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetCurrentFunctions002");
}

HWTEST_F(UsbServiceDeviceTest, GetCurrentFunctions003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetCurrentFunctions003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = -1;
    int32_t ret = instance.GetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::GetCurrentFunctions003 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetCurrentFunctions003");
}

HWTEST_F(UsbServiceDeviceTest, SetCurrentFunctions001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = 0;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::SetCurrentFunctions001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions001");
}

HWTEST_F(UsbServiceDeviceTest, SetCurrentFunctions002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = 1;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::SetCurrentFunctions002 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions002");
}

HWTEST_F(UsbServiceDeviceTest, SetCurrentFunctions003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = -1;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::SetCurrentFunctions003 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions003");
}

HWTEST_F(UsbServiceDeviceTest, SetCurrentFunctions004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetCurrentFunctions004");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = 0xFFFFFFFF;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::SetCurrentFunctions004 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetCurrentFunctions004");
}

HWTEST_F(UsbServiceDeviceTest, UsbFunctionsFromString001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsFromString001");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "mtp";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::UsbFunctionsFromString001 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsFromString001");
}

HWTEST_F(UsbServiceDeviceTest, UsbFunctionsFromString002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsFromString002");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::UsbFunctionsFromString002 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsFromString002");
}

HWTEST_F(UsbServiceDeviceTest, UsbFunctionsFromString003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsFromString003");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "invalid_function";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::UsbFunctionsFromString003 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsFromString003");
}

HWTEST_F(UsbServiceDeviceTest, UsbFunctionsFromString004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsFromString004");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "ptp";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::UsbFunctionsFromString004 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsFromString004");
}

HWTEST_F(UsbServiceDeviceTest, UsbFunctionsFromString005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsFromString005");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "rndis";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::UsbFunctionsFromString005 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsFromString005");
}

HWTEST_F(UsbServiceDeviceTest, UsbFunctionsToString001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsToString001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = 0;
    std::string funcResult;
    int32_t ret = instance.UsbFunctionsToString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::UsbFunctionsToString001 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsToString001");
}

HWTEST_F(UsbServiceDeviceTest, UsbFunctionsToString002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsToString002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = 1;
    std::string funcResult;
    int32_t ret = instance.UsbFunctionsToString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::UsbFunctionsToString002 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsToString002");
}

HWTEST_F(UsbServiceDeviceTest, UsbFunctionsToString003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsToString003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = -1;
    std::string funcResult;
    int32_t ret = instance.UsbFunctionsToString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::UsbFunctionsToString003 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsToString003");
}

HWTEST_F(UsbServiceDeviceTest, UsbFunctionsToString004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : UsbFunctionsToString004");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = 0xFFFFFFFF;
    std::string funcResult;
    int32_t ret = instance.UsbFunctionsToString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::UsbFunctionsToString004 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : UsbFunctionsToString004");
}

HWTEST_F(UsbServiceDeviceTest, GetAccessoryList001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetAccessoryList001");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<USBAccessory> accessList;
    int32_t ret = instance.GetAccessoryList(accessList);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::GetAccessoryList001 ret=%{public}d, size=%{public}zu",
        ret, accessList.size());
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetAccessoryList001");
}

HWTEST_F(UsbServiceDeviceTest, GetAccessoryList002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetAccessoryList002");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<USBAccessory> accessList;
    int32_t ret = instance.GetAccessoryList(accessList);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::GetAccessoryList002 ret=%{public}d", ret);
    ASSERT_GE(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetAccessoryList002");
}

HWTEST_F(UsbServiceDeviceTest, OpenAccessory001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : OpenAccessory001");
    auto &instance = UsbSrvClient::GetInstance();
    USBAccessory access;
    int32_t fd = -1;
    int32_t ret = instance.OpenAccessory(access, fd);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::OpenAccessory001 ret=%{public}d, fd=%{public}d", ret, fd);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : OpenAccessory001");
}

HWTEST_F(UsbServiceDeviceTest, OpenAccessory002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : OpenAccessory002");
    auto &instance = UsbSrvClient::GetInstance();
    USBAccessory access;
    access.SetManufacturer("test_manufacturer");
    access.SetModel("test_model");
    access.SetVersion("1.0");
    int32_t fd = -1;
    int32_t ret = instance.OpenAccessory(access, fd);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::OpenAccessory002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : OpenAccessory002");
}

HWTEST_F(UsbServiceDeviceTest, CloseAccessory001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : CloseAccessory001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t fd = -1;
    int32_t ret = instance.CloseAccessory(fd);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::CloseAccessory001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : CloseAccessory001");
}

HWTEST_F(UsbServiceDeviceTest, CloseAccessory002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : CloseAccessory002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t fd = 0;
    int32_t ret = instance.CloseAccessory(fd);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::CloseAccessory002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : CloseAccessory002");
}

HWTEST_F(UsbServiceDeviceTest, HasAccessoryRight001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : HasAccessoryRight001");
    auto &instance = UsbSrvClient::GetInstance();
    USBAccessory access;
    bool result = false;
    int32_t ret = instance.HasAccessoryRight(access, result);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::HasAccessoryRight001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : HasAccessoryRight001");
}

HWTEST_F(UsbServiceDeviceTest, HasAccessoryRight002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : HasAccessoryRight002");
    auto &instance = UsbSrvClient::GetInstance();
    USBAccessory access;
    access.SetManufacturer("test");
    bool result = false;
    int32_t ret = instance.HasAccessoryRight(access, result);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::HasAccessoryRight002 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : HasAccessoryRight002");
}

HWTEST_F(UsbServiceDeviceTest, RequestAccessoryRight001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : RequestAccessoryRight001");
    auto &instance = UsbSrvClient::GetInstance();
    USBAccessory access;
    bool result = false;
    int32_t ret = instance.RequestAccessoryRight(access, result);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::RequestAccessoryRight001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : RequestAccessoryRight001");
}

HWTEST_F(UsbServiceDeviceTest, RequestAccessoryRight002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : RequestAccessoryRight002");
    auto &instance = UsbSrvClient::GetInstance();
    USBAccessory access;
    access.SetManufacturer("test");
    bool result = false;
    int32_t ret = instance.RequestAccessoryRight(access, result);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::RequestAccessoryRight002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : RequestAccessoryRight002");
}

HWTEST_F(UsbServiceDeviceTest, CancelAccessoryRight001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : CancelAccessoryRight001");
    auto &instance = UsbSrvClient::GetInstance();
    USBAccessory access;
    int32_t ret = instance.CancelAccessoryRight(access);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::CancelAccessoryRight001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : CancelAccessoryRight001");
}

HWTEST_F(UsbServiceDeviceTest, CancelAccessoryRight002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : CancelAccessoryRight002");
    auto &instance = UsbSrvClient::GetInstance();
    USBAccessory access;
    access.SetManufacturer("test");
    int32_t ret = instance.CancelAccessoryRight(access);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::CancelAccessoryRight002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : CancelAccessoryRight002");
}

#endif // USB_MANAGER_FEATURE_DEVICE

#ifdef USB_MANAGER_FEATURE_PORT

HWTEST_F(UsbServiceDeviceTest, GetPorts001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetPorts001");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbPort> ports;
    int32_t ret = instance.GetPorts(ports);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::GetPorts001 ret=%{public}d, size=%{public}zu",
        ret, ports.size());
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetPorts001");
}

HWTEST_F(UsbServiceDeviceTest, GetPorts002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetPorts002");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbPort> ports;
    int32_t ret = instance.GetPorts(ports);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::GetPorts002 ret=%{public}d", ret);
    ASSERT_GE(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetPorts002");
}

HWTEST_F(UsbServiceDeviceTest, GetSupportedModes001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetSupportedModes001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    int32_t supportedModes = 0;
    int32_t ret = instance.GetSupportedModes(portId, supportedModes);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::GetSupportedModes001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetSupportedModes001");
}

HWTEST_F(UsbServiceDeviceTest, GetSupportedModes002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetSupportedModes002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = -1;
    int32_t supportedModes = 0;
    int32_t ret = instance.GetSupportedModes(portId, supportedModes);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::GetSupportedModes002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetSupportedModes002");
}

HWTEST_F(UsbServiceDeviceTest, GetSupportedModes003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : GetSupportedModes003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 255;
    int32_t supportedModes = 0;
    int32_t ret = instance.GetSupportedModes(portId, supportedModes);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::GetSupportedModes003 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : GetSupportedModes003");
}

HWTEST_F(UsbServiceDeviceTest, SetPortRole001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetPortRole001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    int32_t powerRole = 0;
    int32_t dataRole = 0;
    int32_t ret = instance.SetPortRole(portId, powerRole, dataRole);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::SetPortRole001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetPortRole001");
}

HWTEST_F(UsbServiceDeviceTest, SetPortRole002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetPortRole002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = -1;
    int32_t powerRole = 0;
    int32_t dataRole = 0;
    int32_t ret = instance.SetPortRole(portId, powerRole, dataRole);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::SetPortRole002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetPortRole002");
}

HWTEST_F(UsbServiceDeviceTest, SetPortRole003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetPortRole003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 255;
    int32_t powerRole = 255;
    int32_t dataRole = 255;
    int32_t ret = instance.SetPortRole(portId, powerRole, dataRole);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::SetPortRole003 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetPortRole003");
}

#endif // USB_MANAGER_FEATURE_PORT

HWTEST_F(UsbServiceDeviceTest, SetPortRole004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetPortRole004");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 1;
    int32_t powerRole = 1;
    int32_t dataRole = 1;
    int32_t ret = instance.SetPortRole(portId, powerRole, dataRole);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::SetPortRole004 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetPortRole004");
}

HWTEST_F(UsbServiceDeviceTest, SetPortRole005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetPortRole005");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    int32_t powerRole = 0;
    int32_t dataRole = 1;
    int32_t ret = instance.SetPortRole(portId, powerRole, dataRole);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::SetPortRole005 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetPortRole005");
}

HWTEST_F(UsbServiceDeviceTest, SetPortRole006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SetPortRole006");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    int32_t powerRole = 1;
    int32_t dataRole = 0;
    int32_t ret = instance.SetPortRole(portId, powerRole, dataRole);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::SetPortRole006 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : SetPortRole006");
}

HWTEST_F(UsbServiceDeviceTest, DeviceFunction001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : DeviceFunction001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = USB_FUNCTION_NONE;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceFunction001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : DeviceFunction001");
}

HWTEST_F(UsbServiceDeviceTest, DeviceFunction002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : DeviceFunction002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = USB_FUNCTION_MTP;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceFunction002 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : DeviceFunction002");
}

HWTEST_F(UsbServiceDeviceTest, DeviceFunction003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : DeviceFunction003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = USB_FUNCTION_PTP;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceFunction003 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : DeviceFunction003");
}

HWTEST_F(UsbServiceDeviceTest, DeviceFunction004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : DeviceFunction004");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = USB_FUNCTION_RNDIS;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceFunction004 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : DeviceFunction004");
}

HWTEST_F(UsbServiceDeviceTest, DeviceFunction005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : DeviceFunction005");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = USB_FUNCTION_MIDI;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceFunction005 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : DeviceFunction005");
}

HWTEST_F(UsbServiceDeviceTest, DeviceFunction006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : DeviceFunction006");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = USB_FUNCTION_AUDIO_SOURCE;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceFunction006 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : DeviceFunction006");
}

HWTEST_F(UsbServiceDeviceTest, DeviceFunction007, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : DeviceFunction007");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = USB_FUNCTION_VIDEO_SINK;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceFunction007 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : DeviceFunction007");
}

HWTEST_F(UsbServiceDeviceTest, DeviceFunction008, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : DeviceFunction008");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = USB_FUNCTION_VIDEO_SOURCE;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceFunction008 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : DeviceFunction008");
}

HWTEST_F(UsbServiceDeviceTest, DeviceFunction009, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : DeviceFunction009");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = USB_FUNCTION_ECM;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceFunction009 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : DeviceFunction009");
}

HWTEST_F(UsbServiceDeviceTest, DeviceFunction010, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : DeviceFunction010");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = USB_FUNCTION_HDC;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceFunction010 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : DeviceFunction010");
}

HWTEST_F(UsbServiceDeviceTest, FunctionString001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : FunctionString001");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "none";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::FunctionString001 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : FunctionString001");
}

HWTEST_F(UsbServiceDeviceTest, FunctionString002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : FunctionString002");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "none1";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::FunctionString002 ret=%{public}d", ret);
    ASSERT_NE(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : FunctionString002");
}

HWTEST_F(UsbServiceDeviceTest, FunctionString003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : FunctionString003");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "acm";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::FunctionString003 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : FunctionString003");
}

HWTEST_F(UsbServiceDeviceTest, FunctionString004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : FunctionString004");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "ncm";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::FunctionString004 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : FunctionString004");
}

HWTEST_F(UsbServiceDeviceTest, FunctionString005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : FunctionString005");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "hdc";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::FunctionString005 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : FunctionString005");
}

HWTEST_F(UsbServiceDeviceTest, FunctionString006, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : FunctionString006");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "uac1";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::FunctionString006 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : FunctionString006");
}

HWTEST_F(UsbServiceDeviceTest, FunctionString007, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : FunctionString007");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "uvc";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::FunctionString007 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : FunctionString007");
}

HWTEST_F(UsbServiceDeviceTest, FunctionString008, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : FunctionString008");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "wifi";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::FunctionString008 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : FunctionString008");
}

HWTEST_F(UsbServiceDeviceTest, FunctionString009, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : FunctionString009");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "ethernet";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::FunctionString009 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : FunctionString009");
}

HWTEST_F(UsbServiceDeviceTest, FunctionString010, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : FunctionString010");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcs = "smart";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::FunctionString010 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : FunctionString010");
}

HWTEST_F(UsbServiceDeviceTest, Accessory001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Accessory001");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<USBAccessory> accessList;
    int32_t ret = instance.GetAccessoryList(accessList);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::Accessory001 size=%{public}zu", accessList.size());
    ASSERT_GE(accessList.size(), 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Accessory001");
}

HWTEST_F(UsbServiceDeviceTest, Accessory002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Accessory002");
    auto &instance = UsbSrvClient::GetInstance();
    USBAccessory access;
    access.SetManufacturer("manufacturer");
    access.SetModel("model");
    access.SetSerialNumber("serial123");
    access.SetVersion("1.0.0");
    int32_t fd = -1;
    int32_t ret = instance.OpenAccessory(access, fd);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::Accessory002 ret=%{public}d", ret);
    ASSERT_TRUE(ret != UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Accessory002");
}

HWTEST_F(UsbServiceDeviceTest, Accessory003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Accessory003");
    auto &instance = UsbSrvClient::GetInstance();
    USBAccessory access;
    access.SetManufacturer("test123");
    access.SetModel("TestAccessory");
    access.SetVersion("2.0");
    bool result = false;
    int32_t ret = instance.HasAccessoryRight(access, result);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::Accessory003 ret=%{public}d, result=%{public}d", ret, result);
    ASSERT_GE(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Accessory003");
}

HWTEST_F(UsbServiceDeviceTest, Port001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Port001");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbPort> ports;
    int32_t ret = instance.GetPorts(ports);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::Port001 size=%{public}zu", ports.size());
    ASSERT_GE(ports.size(), 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Port001");
}

HWTEST_F(UsbServiceDeviceTest, Port002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : Port002");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbPort> ports;
    int32_t ret = instance.GetPorts(ports);
    if (ret == UEC_OK && ports.size() > 0) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::Port002 portId=%{public}d", ports[0].GetPortId());
    }
    ASSERT_GE(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : Port002");
}

HWTEST_F(UsbServiceDeviceTest, PortModes001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : PortModes001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    int32_t supportedModes = 0;
    int32_t ret = instance.GetSupportedModes(portId, supportedModes);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::PortModes001 modes=%{public}d", supportedModes);
    ASSERT_GE(supportedModes, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : PortModes001");
}

HWTEST_F(UsbServiceDeviceTest, PortModes002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : PortModes002");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbPort> ports;
    int32_t ret = instance.GetPorts(ports);
    if (ret == UEC_OK && ports.size() > 0) {
        int32_t portId = ports[0].GetPortId();
        int32_t supportedModes = 0;
        ret = instance.GetSupportedModes(portId, supportedModes);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::PortModes002 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : PortModes002");
}

HWTEST_F(UsbServiceDeviceTest, PortModes003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : PortModes003");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbPort> ports;
    int32_t ret = instance.GetPorts(ports);
    if (ret == UEC_OK && ports.size() > 1) {
        int32_t portId = ports[1].GetPortId();
        int32_t supportedModes = 0;
        ret = instance.GetSupportedModes(portId, supportedModes);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::PortModes003 ret=%{public}d, modes=%{public}d",
            ret, supportedModes);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : PortModes003");
}

HWTEST_F(UsbServiceDeviceTest, PortModes004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : PortModes004");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 2;
    int32_t supportedModes = 0;
    int32_t ret = instance.GetSupportedModes(portId, supportedModes);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::PortModes004 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : PortModes004");
}

HWTEST_F(UsbServiceDeviceTest, PortRole001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : PortRole001");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbPort> ports;
    int32_t ret = instance.GetPorts(ports);
    if (ret == UEC_OK && ports.size() > 0) {
        int32_t portId = ports[0].GetPortId();
        int32_t powerRole = 0;
        int32_t dataRole = 0;
        ret = instance.SetPortRole(portId, powerRole, dataRole);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::PortRole001 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : PortRole001");
}

HWTEST_F(UsbServiceDeviceTest, PortRole002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : PortRole002");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbPort> ports;
    int32_t ret = instance.GetPorts(ports);
    if (ret == UEC_OK && ports.size() > 0) {
        int32_t portId = ports[0].GetPortId();
        int32_t powerRole = 1;
        int32_t dataRole = 1;
        ret = instance.SetPortRole(portId, powerRole, dataRole);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::PortRole002 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : PortRole002");
}

HWTEST_F(UsbServiceDeviceTest, PortRole003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : PortRole003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t portId = 0;
    int32_t powerRole = 2;
    int32_t dataRole = 2;
    int32_t ret = instance.SetPortRole(portId, powerRole, dataRole);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::PortRole003 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED || ret == UEC_SERVICE_NOT_SUPPORT_SWITCH_PORT);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : PortRole003");
}

HWTEST_F(UsbServiceDeviceTest, DeviceMode001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : DeviceMode001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = 0;
    int32_t ret = instance.GetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceMode001 current=%{public}d", funcs);
    ASSERT_GE(funcs, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : DeviceMode001");
}

HWTEST_F(UsbServiceDeviceTest, DeviceMode002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : DeviceMode002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = 0;
    int32_t ret = instance.GetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceMode002 ret=%{public}d", ret);
    ASSERT_GE(ret, UEC_OK);
    if (ret == UEC_OK) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceMode002 has_functions=%{public}d", funcs > 0);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : DeviceMode002");
}

HWTEST_F(UsbServiceDeviceTest, DeviceMode003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : DeviceMode003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = 0;
    int32_t ret = instance.GetCurrentFunctions(funcs);
    if (ret == UEC_OK && funcs > 0) {
        ret = instance.SetCurrentFunctions(0);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceMode003 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : DeviceMode003");
}

HWTEST_F(UsbServiceDeviceTest, FunctionConversion001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : FunctionConversion001");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcsStr = "mtp,mtp";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcsStr, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::FunctionConversion001 ret=%{public}d, result=%{public}d",
        ret, funcResult);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : FunctionConversion001");
}

HWTEST_F(UsbServiceDeviceTest, FunctionConversion002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : FunctionConversion002");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcsStr = "mtp,ptp";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcsStr, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::FunctionConversion002 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : FunctionConversion002");
}

HWTEST_F(UsbServiceDeviceTest, FunctionConversion003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : FunctionConversion003");
    auto &instance = UsbSrvClient::GetInstance();
    std::string funcsStr = "";
    int32_t funcResult = 0;
    int32_t ret = instance.UsbFunctionsFromString(funcsStr, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::FunctionConversion003 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : FunctionConversion003");
}

HWTEST_F(UsbServiceDeviceTest, FunctionConversion004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : FunctionConversion004");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = 0x12345678;
    std::string funcResult;
    int32_t ret = instance.UsbFunctionsToString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::FunctionConversion004 ret=%{public}d", ret);
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : FunctionConversion004");
}

HWTEST_F(UsbServiceDeviceTest, FunctionConversion005, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : FunctionConversion005");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = 0x00000001;
    std::string funcResult;
    int32_t ret = instance.UsbFunctionsToString(funcs, funcResult);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::FunctionConversion005 ret=%{public}d, result=%{public}s",
        ret, funcResult.c_str());
    ASSERT_EQ(UEC_OK, ret);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : FunctionConversion005");
}

HWTEST_F(UsbServiceDeviceTest, PortInfo001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : PortInfo001");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbPort> ports;
    int32_t ret = instance.GetPorts(ports);
    ASSERT_EQ(UEC_OK, ret);
    if (ports.size() > 0) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::PortInfo001 portId=%{public}d, mode=%{public}d",
            ports[0].GetPortId(), ports[0].GetUsbMode());
    }
    ASSERT_GE(ports.size(), 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : PortInfo001");
}

HWTEST_F(UsbServiceDeviceTest, PortInfo002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : PortInfo002");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbPort> ports;
    int32_t ret = instance.GetPorts(ports);
    ASSERT_EQ(UEC_OK, ret);
    for (size_t i = 0; i < ports.size(); i++) {
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::PortInfo002 port[%{public}zu] id=%{public}d",
            i, ports[i].GetPortId());
    }
    ASSERT_GE(ports.size(), 0);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : PortInfo002");
}

HWTEST_F(UsbServiceDeviceTest, PortInfo003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : PortInfo003");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbPort> ports;
    int32_t ret = instance.GetPorts(ports);
    ASSERT_EQ(UEC_OK, ret);
    if (ports.size() > 0) {
        int32_t portId = ports[0].GetPortId();
        int32_t supportedModes = 0;
        ret = instance.GetSupportedModes(portId, supportedModes);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::PortInfo003 supportedModes=%{public}d", supportedModes);
        ASSERT_GE(supportedModes, 0);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : PortInfo003");
}

HWTEST_F(UsbServiceDeviceTest, PortInfo004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : PortInfo004");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbPort> ports;
    int32_t ret = instance.GetPorts(ports);
    ASSERT_EQ(UEC_OK, ret);
    for (size_t i = 0; i < ports.size(); i++) {
        int32_t portId = ports[i].GetPortId();
        int32_t supportedModes = 0;
        instance.GetSupportedModes(portId, supportedModes);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::PortInfo004 port[%{public}zu] modes=%{public}d",
            i, supportedModes);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : PortInfo004");
}

HWTEST_F(UsbServiceDeviceTest, DeviceFunctionTests001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : DeviceFunctionTests001");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = USB_FUNCTION_ACM;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceFunctionTests001 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : DeviceFunctionTests001");
}

HWTEST_F(UsbServiceDeviceTest, DeviceFunctionTests002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : DeviceFunctionTests002");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = USB_FUNCTION_OBEX;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceFunctionTests002 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : DeviceFunctionTests002");
}

HWTEST_F(UsbServiceDeviceTest, DeviceFunctionTests003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : DeviceFunctionTests003");
    auto &instance = UsbSrvClient::GetInstance();
    int32_t funcs = USB_FUNCTION_RNDIS + USB_FUNCTION_MTP;
    int32_t ret = instance.SetCurrentFunctions(funcs);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::DeviceFunctionTests003 ret=%{public}d", ret);
    ASSERT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_PERMISSION_DENIED);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : DeviceFunctionTests003");
}

HWTEST_F(UsbServiceDeviceTest, AccessoryTests001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : AccessoryTests001");
    auto &instance = UsbSrvClient::GetInstance();
    USBAccessory access;
    access.SetManufacturer("TestManufacturer");
    access.SetModel("TestModel");
    access.SetSerialNumber("TestSerial123");
    access.SetVersion("1.0");
    bool result = false;
    int32_t ret = instance.HasAccessoryRight(access, result);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::AccessoryTests001 ret=%{public}d, result=%{public}d",
        ret, result);
    ASSERT_GE(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : AccessoryTests001");
}

HWTEST_F(UsbServiceDeviceTest, AccessoryTests002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : AccessoryTests002");
    auto &instance = UsbSrvClient::GetInstance();
    USBAccessory access;
    access.SetManufacturer("Another");
    access.SetModel("Model2");
    access.SetVersion("2.0");
    bool result = false;
    int32_t ret = instance.RequestAccessoryRight(access, result);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::AccessoryTests002 ret=%{public}d", ret);
    ASSERT_GE(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : AccessoryTests002");
}

HWTEST_F(UsbServiceDeviceTest, AccessoryTests003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : AccessoryTests003");
    auto &instance = UsbSrvClient::GetInstance();
    USBAccessory access;
    access.SetManufacturer("Test3");
    int32_t ret = instance.CancelAccessoryRight(access);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::AccessoryTests003 ret=%{public}d", ret);
    ASSERT_GE(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : AccessoryTests003");
}

HWTEST_F(UsbServiceDeviceTest, PortAndRoleTests001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : PortAndRoleTests001");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbPort> ports;
    int32_t ret = instance.GetPorts(ports);
    ASSERT_EQ(UEC_OK, ret);
    if (ports.size() > 0) {
        int32_t portId = ports[0].GetPortId();
        int32_t supportedModes = 0;
        ret = instance.GetSupportedModes(portId, supportedModes);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::PortAndRoleTests001 modes=%{public}d", supportedModes);
        ASSERT_GE(supportedModes, 0);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : PortAndRoleTests001");
}

HWTEST_F(UsbServiceDeviceTest, PortAndRoleTests002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : PortAndRoleTests002");
    auto &instance = UsbSrvClient::GetInstance();
    std::vector<UsbPort> ports;
    int32_t ret = instance.GetPorts(ports);
    if (ret == UEC_OK && ports.size() > 0) {
        int32_t portId = ports[0].GetPortId();
        int32_t powerRole = 0;
        int32_t dataRole = 1;
        ret = instance.SetPortRole(portId, powerRole, dataRole);
        USB_HILOGI(MODULE_USB_SERVICE, "UsbServiceDeviceTest::PortAndRoleTests002 ret=%{public}d", ret);
        ASSERT_GE(ret, UEC_OK);
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : PortAndRoleTests002");
}

} // namespace Service
} // namespace USB
} // namespace OHOS