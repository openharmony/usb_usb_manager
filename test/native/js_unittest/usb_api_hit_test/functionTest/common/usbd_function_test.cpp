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

#include <iostream>

#include "hdf_log.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "usbd_function_test.h"
#include "v2_0/iusb_device_interface.h"
#include "v2_0/iusb_port_interface.h"
#include "v2_0/usb_types.h"

constexpr int32_t SLEEP_TIME = 3;
constexpr int32_t USB_FUNCTION_INVALID = -1;
constexpr int32_t USB_PORT_ID_INVALID = 2;
constexpr int32_t USB_POWER_ROLE_INVALID = 4;
constexpr int32_t USB_DATA_ROLE_INVALID = 5;
constexpr int32_t USB_FUNCTION_UNSUPPORTED = 128;

using namespace testing::ext;
using namespace OHOS;
using namespace std;
using namespace OHOS::HDI::Usb::V2_0;

namespace {
sptr<IUsbPortInterface> g_usbPortInterface = nullptr;
sptr<IUsbDeviceInterface> g_usbDeviceInterface = nullptr;

void UsbdFunctionTest::SetUpTestCase(void)
{
    g_usbDeviceInterface = IUsbDeviceInterface::Get();
    g_usbPortInterface = IUsbPortInterface::Get();
    if (g_usbDeviceInterface == nullptr || g_usbPortInterface == nullptr) {
        HDF_LOGE("%{public}s:g_usbDeviceInterface or g_usbPortInterface is nullptr.", __func__);
        exit(0);
    }
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, POWER_ROLE_SINK, DATA_ROLE_DEVICE);
    sleep(SLEEP_TIME);
    HDF_LOGI("UsbdFunctionTest::[Device] %{public}d SetPortRole=%{public}d", __LINE__, ret);
    if (ret != 0) {
        ASSERT_EQ(HDF_ERR_NOT_SUPPORT, ret);
    } else {
        ASSERT_EQ(0, ret);
    }
}

void UsbdFunctionTest::TearDownTestCase(void)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_HDC);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_0800 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

void UsbdFunctionTest::SetUp(void) {}

void UsbdFunctionTest::TearDown(void) {}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_0300
 * @tc.desc: Test functions to GetCurrentFunctions
 * @tc.desc: int32_t GetCurrentFunctions(int32_t &funcs);
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_0300, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_NONE;
    auto ret = g_usbDeviceInterface->GetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_0300 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_0400
 * @tc.desc: Test functions to GetCurrentFunctions
 * @tc.desc: int32_t GetCurrentFunctions(int32_t &funcs);
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_0400, Function | MediumTest | Level1)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_ACM);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_0400 %{public}d SetCurrentFunctions=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(0, ret);
    int32_t funcs = USB_FUNCTION_NONE;
    ret = g_usbDeviceInterface->GetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_0400 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**********************************************************************************************************/

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_0500
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_0500, Function | MediumTest | Level1)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_ACM);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_0500 %{public}d SetCurrentFunctions=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Compatibility_1500
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Negative test: parameters exception, Funcs error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Compatibility_1500, Function | MediumTest | Level1)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_INVALID);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Compatibility_1500 %{public}d, ret=%{public}d",
        __LINE__, ret);
    ASSERT_NE(ret, 0);
}
/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_0600
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_0600, Function | MediumTest | Level1)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_ECM);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_0600 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_0700
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_0700, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_ACM | USB_FUNCTION_ECM;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_0700 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_0800
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_0800, Function | MediumTest | Level1)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_HDC);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_0800 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_0900
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_0900, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_ACM | USB_FUNCTION_HDC;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_0900 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_1000
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_1000, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_ECM | USB_FUNCTION_HDC;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_1000 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_1100
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_1100, Function | MediumTest | Level1)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_RNDIS);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_1100 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_1400
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_1400, Function | MediumTest | Level1)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_STORAGE);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_1400 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_1500
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_1500, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_RNDIS | USB_FUNCTION_HDC;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_1500 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_1600
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_1600, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_STORAGE | USB_FUNCTION_HDC;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_1600 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_1900
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_1900, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_MTP;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_1900 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_2000
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_2000, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_PTP;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_2000 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_2100
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_2100, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_MTP | USB_FUNCTION_HDC;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_2100 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_2200
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_2200, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_PTP | USB_FUNCTION_HDC;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_2200 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_2300
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_2300, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_MTP | USB_FUNCTION_RNDIS;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_2300 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_2400
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_2400, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_PTP | USB_FUNCTION_RNDIS;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(funcs);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_2400 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_1700
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Negative test: parameters exception, funcs error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_1700, Function | MediumTest | Level1)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_UNSUPPORTED);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_1700 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_1800
 * @tc.desc: Test functions to SetCurrentFunctions
 * @tc.desc: int32_t SetCurrentFunctions(int32_t funcs)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_DeviceManager_HDI_Func_1800, Function | MediumTest | Level1)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_NONE);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_1800 ret=%{public}d", ret);
    ASSERT_EQ(0, ret);
    HDF_LOGI("UsbdFunctionTest::the function was set to none successfully");
    ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_HDC);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_PortManager_HDI_Func_0100
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_PortManager_HDI_Func_0100, Function | MediumTest | Level1)
{
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, POWER_ROLE_SOURCE, DATA_ROLE_HOST);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_PortManager_HDI_Func_0100 %{public}d ret=%{public}d", __LINE__, ret);
    if (ret != 0) {
        ASSERT_EQ(HDF_ERR_NOT_SUPPORT, ret);
    } else {
        ASSERT_EQ(0, ret);
    }
}

/**
 * @tc.name: SUB_USB_PortManager_HDI_Compatibility_0100
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole)
 * @tc.desc: Negative test: parameters exception, portId error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_PortManager_HDI_Compatibility_0100, Function | MediumTest | Level1)
{
    auto ret = g_usbPortInterface->SetPortRole(USB_PORT_ID_INVALID, POWER_ROLE_SOURCE, DATA_ROLE_HOST);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_PortManager_HDI_Compatibility_0100 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_PortManager_HDI_Compatibility_0200
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole)
 * @tc.desc: Negative test: parameters exception, powerRole error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_PortManager_HDI_Compatibility_0200, Function | MediumTest | Level1)
{
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, USB_POWER_ROLE_INVALID, DATA_ROLE_DEVICE);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_PortManager_HDI_Compatibility_0200 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_PortManager_HDI_Compatibility_0300
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole)
 * @tc.desc: Negative test: parameters exception, dataRole error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_PortManager_HDI_Compatibility_0300, Function | MediumTest | Level1)
{
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, POWER_ROLE_SOURCE, USB_DATA_ROLE_INVALID);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_PortManager_HDI_Compatibility_0300 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_PortManager_HDI_Compatibility_0400
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole)
 * @tc.desc: Negative test: parameters exception, powerRole error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_PortManager_HDI_Compatibility_0400, Function | MediumTest | Level1)
{
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, USB_POWER_ROLE_INVALID, DATA_ROLE_HOST);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_PortManager_HDI_Compatibility_0400 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_PortManager_HDI_Compatibility_0500
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole)
 * @tc.desc: Negative test: parameters exception, portId && dataRole error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_PortManager_HDI_Compatibility_0500, Function | MediumTest | Level1)
{
    auto ret = g_usbPortInterface->SetPortRole(USB_PORT_ID_INVALID, POWER_ROLE_SOURCE, USB_DATA_ROLE_INVALID);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_PortManager_HDI_Compatibility_0500 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_PortManager_HDI_Compatibility_0600
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole)
 * @tc.desc: Negative test: parameters exception, powerRole && dataRole error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_PortManager_HDI_Compatibility_0600, Function | MediumTest | Level1)
{
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, USB_POWER_ROLE_INVALID, USB_DATA_ROLE_INVALID);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_PortManager_HDI_Compatibility_0600 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_PortManager_HDI_Compatibility_0700
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole)
 * @tc.desc: Negative test: parameters exception, portId && powerRole && dataRole error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_PortManager_HDI_Compatibility_0700, Function | MediumTest | Level1)
{
    auto ret = g_usbPortInterface->SetPortRole(USB_PORT_ID_INVALID, USB_POWER_ROLE_INVALID, USB_DATA_ROLE_INVALID);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_PortManager_HDI_Compatibility_0700 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_PortManager_HDI_Compatibility_0800
 * @tc.desc: Test functions to SetPortRole
 * @tc.desc: int32_t SetPortRole(int32_t portId,int32_t powerRole,int32_t dataRole)
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_PortManager_HDI_Compatibility_0800, Function | MediumTest | Level1)
{
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, POWER_ROLE_SINK, DATA_ROLE_DEVICE);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_PortManager_HDI_Compatibility_0800 %{public}d ret=%{public}d", __LINE__, ret);
    if (ret != 0) {
        ASSERT_EQ(HDF_ERR_NOT_SUPPORT, ret);
    } else {
        ASSERT_EQ(0, ret);
    }
}

/**
 * @tc.name: SUB_USB_PortManager_HDI_Func_0200
 * @tc.desc: Test functions to QueryPort
 * @tc.desc: int32_t QueryPort(int32_t &portId, int32_t &powerRole, int32_t &dataRole, int32_t &mode);
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdFunctionTest, SUB_USB_PortManager_HDI_Func_0200, Function | MediumTest | Level1)
{
    int32_t portId = DEFAULT_PORT_ID;
    int32_t powerRole = POWER_ROLE_NONE;
    int32_t dataRole = DATA_ROLE_NONE;
    int32_t mode = PORT_MODE_NONE;
    auto ret = g_usbPortInterface->QueryPort(portId, powerRole, dataRole, mode);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_PortManager_HDI_Func_0200 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}
} // namespace
