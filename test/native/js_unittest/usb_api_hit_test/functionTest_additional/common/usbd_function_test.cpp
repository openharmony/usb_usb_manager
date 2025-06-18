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
constexpr int32_t USB_PORT_ID_INVALID = 2;
constexpr int32_t USB_POWER_ROLE_INVALID = 4;
constexpr int32_t USB_DATA_ROLE_INVALID = 5;
using namespace testing::ext;
using namespace OHOS;
using namespace std;
using namespace OHOS::HDI::Usb::V2_0;

namespace {
sptr<IUsbPortInterface> g_usbPortInterface = nullptr;
sptr<IUsbDeviceInterface> g_usbDeviceInterface = nullptr;

void UsbdFunctionTestAdditional::SetUpTestCase(void)
{
    g_usbDeviceInterface = IUsbDeviceInterface::Get();
    g_usbPortInterface = IUsbPortInterface::Get();
    if (g_usbDeviceInterface == nullptr || g_usbPortInterface == nullptr) {
        HDF_LOGE("%{public}s:g_usbDeviceInterface or g_usbPortInterface is nullptr.", __func__);
        exit(0);
    }
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, POWER_ROLE_SINK, DATA_ROLE_DEVICE);
    sleep(SLEEP_TIME);
    HDF_LOGI("UsbdFunctionTestAdditional::[Device] %{public}d SetPortRole=%{public}d", __LINE__, ret);
    if (ret != 0) {
        ASSERT_EQ(HDF_ERR_NOT_SUPPORT, ret);
    } else {
        ASSERT_EQ(0, ret);
    }
}

void UsbdFunctionTestAdditional::TearDownTestCase(void)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_HDC);
    HDF_LOGI("UsbdFunctionTest::SUB_USB_DeviceManager_HDI_Func_0800 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

void UsbdFunctionTestAdditional::SetUp(void) {}

void UsbdFunctionTestAdditional::TearDown(void) {}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_1200
 * @tc.name: testHdiUsbFunctionTestSetCurrentFunctions013
 * @tc.desc: Sets the list of functions (represented by bit field) supported by the current device.
 * USB_FUNCTION_NONE,USB_FUNCTION_ACM.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestSetCurrentFunctions013, Function | MediumTest | Level1)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_NONE);
    ASSERT_EQ(0, ret);
    ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_ACM);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_1300
 * @tc.name: testHdiUsbFunctionTestSetCurrentFunctions014
 * @tc.desc: Sets the list of functions (represented by bit field) supported by the current device.
 * USB_FUNCTION_NONE,USB_FUNCTION_ECM.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestSetCurrentFunctions014, Function | MediumTest | Level1)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_NONE);
    ASSERT_EQ(0, ret);
    ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_ECM);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_1400
 * @tc.name: testHdiUsbFunctionTestSetCurrentFunctions015
 * @tc.desc: Sets the list of functions (represented by bit field) supported by the current device.
 * USB_FUNCTION_NONE,USB_FUNCTION_MTP.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestSetCurrentFunctions015, Function | MediumTest | Level1)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_NONE);
    ASSERT_EQ(0, ret);
    ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_MTP);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_1500
 * @tc.name: testHdiUsbFunctionTestSetCurrentFunctions016
 * @tc.desc: Sets the list of functions (represented by bit field) supported by the current device.
 * USB_FUNCTION_NONE,USB_FUNCTION_PTP.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestSetCurrentFunctions016, Function | MediumTest | Level1)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_NONE);
    ASSERT_EQ(0, ret);
    ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_PTP);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_1600
 * @tc.name: testHdiUsbFunctionTestSetCurrentFunctions017
 * @tc.desc: Sets the list of functions (represented by bit field) supported by the current device.
 * USB_FUNCTION_NONE,USB_FUNCTION_RNDIS.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestSetCurrentFunctions017, Function | MediumTest | Level1)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_NONE);
    ASSERT_EQ(0, ret);
    ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_RNDIS);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_1700
 * @tc.name: testHdiUsbFunctionTestSetCurrentFunctions018
 * @tc.desc: Sets the list of functions (represented by bit field) supported by the current device.
 * USB_FUNCTION_NONE,USB_FUNCTION_STORAGE.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestSetCurrentFunctions018, Function | MediumTest | Level1)
{
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_NONE);
    ASSERT_EQ(0, ret);
    ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_STORAGE);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_1800
 * @tc.name: testHdiUsbFunctionTestGetCurrentFunctions001
 * @tc.desc: Obtains the list of functions (represented by bit field) supported by the current device.
 * Set USB_FUNCTION_ACM Check if the Get is correct.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestGetCurrentFunctions001, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_NONE;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_ACM);
    ASSERT_EQ(0, ret);
    ret = g_usbDeviceInterface->GetCurrentFunctions(funcs);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(USB_FUNCTION_ACM, funcs);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_1900
 * @tc.name: testHdiUsbFunctionTestGetCurrentFunctions002
 * @tc.desc: Obtains the list of functions (represented by bit field) supported by the current device.
 * Set USB_FUNCTION_ECM Check if the Get is correct.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestGetCurrentFunctions002, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_NONE;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_ECM);
    ASSERT_EQ(0, ret);
    ret = g_usbDeviceInterface->GetCurrentFunctions(funcs);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(USB_FUNCTION_ECM, funcs);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_2000
 * @tc.name: testHdiUsbFunctionTestGetCurrentFunctions003
 * @tc.desc: Obtains the list of functions (represented by bit field) supported by the current device.
 * Set USB_FUNCTION_HDC Check if the Get is correct.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestGetCurrentFunctions003, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_NONE;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_HDC);
    ASSERT_EQ(0, ret);
    ret = g_usbDeviceInterface->GetCurrentFunctions(funcs);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(USB_FUNCTION_HDC, funcs);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_2100
 * @tc.name: testHdiUsbFunctionTestGetCurrentFunctions004
 * @tc.desc: Obtains the list of functions (represented by bit field) supported by the current device.
 * Set USB_FUNCTION_MTP Check if the Get is correct.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestGetCurrentFunctions004, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_NONE;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_MTP);
    ASSERT_EQ(0, ret);
    ret = g_usbDeviceInterface->GetCurrentFunctions(funcs);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(USB_FUNCTION_MTP, funcs);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_2200
 * @tc.name: testHdiUsbFunctionTestGetCurrentFunctions005
 * @tc.desc: Obtains the list of functions (represented by bit field) supported by the current device.
 * Set USB_FUNCTION_PTP Check if the Get is correct.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestGetCurrentFunctions005, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_NONE;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_PTP);
    ASSERT_EQ(0, ret);
    ret = g_usbDeviceInterface->GetCurrentFunctions(funcs);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(USB_FUNCTION_PTP, funcs);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_2300
 * @tc.name: testHdiUsbFunctionTestGetCurrentFunctions006
 * @tc.desc: Obtains the list of functions (represented by bit field) supported by the current device.
 * Set USB_FUNCTION_RNDIS Check if the Get is correct.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestGetCurrentFunctions006, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_NONE;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_RNDIS);
    ASSERT_EQ(0, ret);
    ret = g_usbDeviceInterface->GetCurrentFunctions(funcs);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(USB_FUNCTION_RNDIS, funcs);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_2400
 * @tc.name: testHdiUsbFunctionTestGetCurrentFunctions007
 * @tc.desc: Obtains the list of functions (represented by bit field) supported by the current device.
 * Set USB_FUNCTION_STORAGE Check if the Get is correct.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestGetCurrentFunctions007, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_NONE;
    auto ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_STORAGE);
    ASSERT_EQ(0, ret);
    ret = g_usbDeviceInterface->GetCurrentFunctions(funcs);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(USB_FUNCTION_STORAGE, funcs);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_2500
 * @tc.name: testHdiUsbFunctionTestSetPortRole001
 * @tc.desc: Closes a USB device to release all system resources related to the device.The entries are
 * (USB_PORT_ID_INVALID, POWER_ROLE_SINK, DATA_ROLE_DEVICE).
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestSetPortRole001, Function | MediumTest | Level2)
{
    auto ret = g_usbPortInterface->SetPortRole(USB_PORT_ID_INVALID, POWER_ROLE_SINK, DATA_ROLE_DEVICE);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_2600
 * @tc.name: testHdiUsbFunctionTestSetPortRole002
 * @tc.desc: Closes a USB device to release all system resources related to the device.The entries are (DEFAULT_PORT_ID,
 * USB_POWER_ROLE_INVALID, DATA_ROLE_NONE).
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestSetPortRole002, Function | MediumTest | Level2)
{
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, USB_POWER_ROLE_INVALID, DATA_ROLE_NONE);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_2700
 * @tc.name: testHdiUsbFunctionTestSetPortRole003
 * @tc.desc: Closes a USB device to release all system resources related to the device.The entries are (DEFAULT_PORT_ID,
 * POWER_ROLE_SINK, USB_DATA_ROLE_INVALID).
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestSetPortRole003, Function | MediumTest | Level2)
{
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, POWER_ROLE_SINK, USB_DATA_ROLE_INVALID);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_2800
 * @tc.desc: Closes a USB device to release all system resources related to the device.The entries are (DEFAULT_PORT_ID,
 * POWER_ROLE_NONE, USB_DATA_ROLE_INVALID).
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestSetPortRole004, Function | MediumTest | Level2)
{
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, POWER_ROLE_NONE, USB_DATA_ROLE_INVALID);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_2900
 * @tc.name: testHdiUsbFunctionTestSetPortRole005
 * @tc.desc: Closes a USB device to release all system resources related to the device.The entries are
 * (USB_PORT_ID_INVALID, POWER_ROLE_SINK, USB_DATA_ROLE_INVALID).
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestSetPortRole005, Function | MediumTest | Level2)
{
    auto ret = g_usbPortInterface->SetPortRole(USB_PORT_ID_INVALID, POWER_ROLE_SINK, USB_DATA_ROLE_INVALID);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_3000
 * @tc.name: testHdiUsbFunctionTestSetPortRole006
 * @tc.desc: Closes a USB device to release all system resources related to the device.The entries are
 * (USB_PORT_ID_INVALID, POWER_ROLE_NONE, USB_DATA_ROLE_INVALID).
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestSetPortRole006, Function | MediumTest | Level2)
{
    auto ret = g_usbPortInterface->SetPortRole(USB_PORT_ID_INVALID, POWER_ROLE_NONE, USB_DATA_ROLE_INVALID);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_3100
 * @tc.name: testHdiUsbFunctionTestSetPortRole007
 * @tc.desc: Closes a USB device to release all system resources related to the device.The entries are
 * (USB_PORT_ID_INVALID, USB_POWER_ROLE_INVALID, DATA_ROLE_NONE).
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestSetPortRole007, Function | MediumTest | Level2)
{
    auto ret = g_usbPortInterface->SetPortRole(USB_PORT_ID_INVALID, USB_POWER_ROLE_INVALID, DATA_ROLE_NONE);
    ASSERT_NE(ret, 0);
}
/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_3200
 * @tc.name: testHdiUsbFunctionTestSetPortRole008
 * @tc.desc: Closes a USB device to release all system resources related to the device.The entries are
 * (USB_PORT_ID_INVALID, USB_POWER_ROLE_INVALID, DATA_ROLE_HOST).
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestSetPortRole008, Function | MediumTest | Level2)
{
    auto ret = g_usbPortInterface->SetPortRole(USB_PORT_ID_INVALID, USB_POWER_ROLE_INVALID, DATA_ROLE_HOST);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_3300
 * @tc.name: testHdiUsbFunctionTestSetPortRole009
 * @tc.desc: Closes a USB device to release all system resources related to the device.The entries are
 * (USB_PORT_ID_INVALID, USB_POWER_ROLE_INVALID, DATA_ROLE_DEVICE).
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestSetPortRole009, Function | MediumTest | Level2)
{
    auto ret = g_usbPortInterface->SetPortRole(USB_PORT_ID_INVALID, USB_POWER_ROLE_INVALID, DATA_ROLE_DEVICE);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_3400
 * @tc.name: testHdiUsbFunctionTestQueryPort001
 * @tc.desc: Queries the current settings of a port.To see if portId is equal to DEFAULT_PORT_ID.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestQueryPort001, Function | MediumTest | Level1)
{
    int32_t portId = 0;
    int32_t powerRole = POWER_ROLE_NONE;
    int32_t dataRole = DATA_ROLE_NONE;
    int32_t mode = PORT_MODE_NONE;
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, POWER_ROLE_SINK, DATA_ROLE_DEVICE);
    if (ret != 0) {
        ASSERT_EQ(HDF_ERR_NOT_SUPPORT, ret);
    } else {
        ASSERT_EQ(0, ret);
    }
    ret = g_usbPortInterface->QueryPort(portId, powerRole, dataRole, mode);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(DEFAULT_PORT_ID, portId);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_3500
 * @tc.name: testHdiUsbFunctionTestQueryPort002
 * @tc.desc: Queries the current settings of a port.To see if powerRole is equal to POWER_ROLE_SINK.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestQueryPort002, Function | MediumTest | Level1)
{
    int32_t portId = DEFAULT_PORT_ID;
    int32_t powerRole = POWER_ROLE_NONE;
    int32_t dataRole = DATA_ROLE_NONE;
    int32_t mode = PORT_MODE_NONE;
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, POWER_ROLE_SINK, DATA_ROLE_DEVICE);
    if (ret != 0) {
        ASSERT_EQ(HDF_ERR_NOT_SUPPORT, ret);
    } else {
        ASSERT_EQ(0, ret);
    }
    ret = g_usbPortInterface->QueryPort(portId, powerRole, dataRole, mode);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(POWER_ROLE_SINK, powerRole);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_3600
 * @tc.name: testHdiUsbFunctionTestQueryPort003
 * @tc.desc: Queries the current settings of a port.To see if dataRole is equal to DATA_ROLE_DEVICE.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestQueryPort003, Function | MediumTest | Level1)
{
    int32_t portId = DEFAULT_PORT_ID;
    int32_t powerRole = POWER_ROLE_NONE;
    int32_t dataRole = DATA_ROLE_NONE;
    int32_t mode = PORT_MODE_NONE;
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, POWER_ROLE_SINK, DATA_ROLE_DEVICE);
    if (ret != 0) {
        ASSERT_EQ(HDF_ERR_NOT_SUPPORT, ret);
    } else {
        ASSERT_EQ(0, ret);
    }
    ret = g_usbPortInterface->QueryPort(portId, powerRole, dataRole, mode);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(DATA_ROLE_DEVICE, dataRole);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_3700
 * @tc.name: testHdiUsbFunctionTestQueryPort004
 * @tc.desc: Queries the current settings of a port.To see if portId is equal to DEFAULT_PORT_ID.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestQueryPort004, Function | MediumTest | Level1)
{
    int32_t portId = 0;
    int32_t powerRole = POWER_ROLE_NONE;
    int32_t dataRole = DATA_ROLE_NONE;
    int32_t mode = PORT_MODE_NONE;
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, POWER_ROLE_SOURCE, DATA_ROLE_HOST);
    if (ret != 0) {
        ASSERT_EQ(HDF_ERR_NOT_SUPPORT, ret);
    } else {
        ASSERT_EQ(0, ret);
    }
    ret = g_usbPortInterface->QueryPort(portId, powerRole, dataRole, mode);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(DEFAULT_PORT_ID, portId);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_3800
 * @tc.name: testHdiUsbFunctionTestQueryPort005
 * @tc.desc: Queries the current settings of a port.To see if powerRole is equal to DATA_ROLE_HOST.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestQueryPort005, Function | MediumTest | Level1)
{
    int32_t portId = DEFAULT_PORT_ID;
    int32_t powerRole = POWER_ROLE_NONE;
    int32_t dataRole = DATA_ROLE_NONE;
    int32_t mode = PORT_MODE_NONE;
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, POWER_ROLE_SOURCE, DATA_ROLE_HOST);
    if (ret == HDF_SUCCESS) {
        ret = g_usbPortInterface->QueryPort(portId, powerRole, dataRole, mode);
        ASSERT_EQ(0, ret);
        ASSERT_EQ(POWER_ROLE_SOURCE, powerRole);
    } else if (ret == HDF_ERR_NOT_SUPPORT) {
        ret = g_usbPortInterface->QueryPort(portId, powerRole, dataRole, mode);
        ASSERT_EQ(0, ret);
        ASSERT_EQ(POWER_ROLE_SINK, powerRole);
    }
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_FunctionTest_3900
 * @tc.name: testHdiUsbFunctionTestQueryPort006
 * @tc.desc: Queries the current settings of a port.To see if dataRole is equal to DATA_ROLE_HOST.
 */
HWTEST_F(UsbdFunctionTestAdditional, testHdiUsbFunctionTestQueryPort006, Function | MediumTest | Level1)
{
    int32_t portId = DEFAULT_PORT_ID;
    int32_t powerRole = POWER_ROLE_NONE;
    int32_t dataRole = DATA_ROLE_NONE;
    int32_t mode = PORT_MODE_NONE;
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, POWER_ROLE_SOURCE, DATA_ROLE_HOST);
    if (ret == HDF_SUCCESS) {
        ret = g_usbPortInterface->QueryPort(portId, powerRole, dataRole, mode);
        ASSERT_EQ(0, ret);
        ASSERT_EQ(DATA_ROLE_HOST, dataRole);
    } else if (ret == HDF_ERR_NOT_SUPPORT) {
        ret = g_usbPortInterface->QueryPort(portId, powerRole, dataRole, mode);
        ASSERT_EQ(0, ret);
        ASSERT_EQ(DATA_ROLE_DEVICE, dataRole);
    }
}

} // namespace
