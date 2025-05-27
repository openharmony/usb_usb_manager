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
#include "usbd_auto_function_test.h"
#include "v2_0/iusb_device_interface.h"
#include "v2_0/iusb_port_interface.h"
#include "v2_0/usb_types.h"

using namespace testing::ext;
using namespace OHOS;
using namespace std;
using namespace OHOS::HDI::Usb::V2_0;

namespace {
sptr<IUsbPortInterface> g_usbPortInterface = nullptr;
sptr<IUsbDeviceInterface> g_usbDeviceInterface = nullptr;

void UsbdAutoFunctionTest::SetUpTestCase(void)
{
    g_usbDeviceInterface = IUsbDeviceInterface::Get();
    g_usbPortInterface = IUsbPortInterface::Get();
    if (g_usbDeviceInterface == nullptr || g_usbPortInterface == nullptr) {
        HDF_LOGE("%{public}s:g_usbPortInterface or g_usbPortInterface is nullptr.", __func__);
    }
}

void UsbdAutoFunctionTest::TearDownTestCase(void) {}

void UsbdAutoFunctionTest::SetUp(void) {}

void UsbdAutoFunctionTest::TearDown(void) {}

/**
 * @tc.name: SUB_USB_DeviceManager_HDI_Func_0300
 * @tc.desc: Test functions to GetCurrentFunctions
 * @tc.desc: int32_t GetCurrentFunctions(int32_t &funcs);
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdAutoFunctionTest, SUB_USB_DeviceManager_HDI_Func_0300, Function | MediumTest | Level1)
{
    int32_t funcs = USB_FUNCTION_NONE;
    auto ret = g_usbDeviceInterface->GetCurrentFunctions(funcs);
    HDF_LOGI("UsbdAutoFunctionTest::SUB_USB_DeviceManager_HDI_Func_0300 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_PortManager_HDI_Func_0200
 * @tc.desc: Test functions to QueryPort
 * @tc.desc: int32_t QueryPort(int32_t &portId, int32_t &powerRole, int32_t &dataRole, int32_t &mode);
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdAutoFunctionTest, SUB_USB_PortManager_HDI_Func_0200, Function | MediumTest | Level1)
{
    int32_t portId = DEFAULT_PORT_ID;
    int32_t powerRole = POWER_ROLE_NONE;
    int32_t dataRole = DATA_ROLE_NONE;
    int32_t mode = PORT_MODE_NONE;
    auto ret = g_usbPortInterface->QueryPort(portId, powerRole, dataRole, mode);
    HDF_LOGI("UsbdAutoFunctionTest::SUB_USB_PortManager_HDI_Func_0200 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}
} // namespace
