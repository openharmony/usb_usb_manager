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

#include "usbd_accessory_test.h"

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>

#include "UsbSubscriberTest.h"
#include "hdf_log.h"
#include "v2_0/iusb_device_interface.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::USB;
using namespace std;
using namespace OHOS::HDI::Usb::V2_0;

sptr<UsbSubscriberTest> UsbdAccessoryTest::subscriber_ = nullptr;

sptr<OHOS::HDI::Usb::V2_0::IUsbDeviceInterface> g_usbDeviceInterface = nullptr;

void UsbdAccessoryTest::SetUpTestCase(void)
{
    g_usbDeviceInterface = OHOS::HDI::Usb::V2_0::IUsbDeviceInterface::Get();
    if (g_usbDeviceInterface == nullptr) {
        HDF_LOGE("%{public}s:g_usbDeviceInterface or is nullptr.", __func__);
        exit(0);
    }
    std::cout << "please connect accessory, press enter to continue" << std::endl;
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

void UsbdAccessoryTest::TearDownTestCase(void)
{
}

void UsbdAccessoryTest::SetUp(void)
{
}

void UsbdAccessoryTest::TearDown(void)
{
}

/**
 * @tc.number   : SUB_USB_DeviceManager_HDI_Accessory_0100
 * @tc.name     : GetAccessoryInfoTest001
 * @tc.desc     : int32_t GetAccessoryInfo
 * @tc.desc     : Positive test: parameters correctly
 * @tc.size     : MediumTest
 * @tc.type     : Function
 * @tc.level    : Level 3
 */
HWTEST_F(UsbdAccessoryTest, GetAccessoryInfoTest001, Function | MediumTest | Level1)
{
    HDF_LOGI("Case Start : GetAccessoryInfoTest001 : GetAccessoryInfo");
    vector<string> accessoryInfo;
    auto ret = g_usbDeviceInterface->GetAccessoryInfo(accessoryInfo);
    EXPECT_TRUE(ret == 0);
    HDF_LOGI("UsbdAccessoryTest::GetAccessoryInfoTest001 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!accessoryInfo.empty()) << "accessoryInfo NULL";
    HDF_LOGI("UsbdAccessoryTest::GetAccessoryInfoTest001 %{public}d size=%{public}zu", __LINE__,
               accessoryInfo.size());
}

/**
 * @tc.number   : SUB_USB_DeviceManager_HDI_Accessory_0200
 * @tc.name     : OpenAccessoryTest001
 * @tc.desc     : int32_t OpenAccessory
 * @tc.desc     : Positive test: parameters correctly
 * @tc.size     : MediumTest
 * @tc.type     : Function
 * @tc.level    : Level 3
 */
HWTEST_F(UsbdAccessoryTest, OpenAccessoryTest001, Function | MediumTest | Level1)
{
    HDF_LOGI("Case Start : OpenAccessoryTest001 : OpenAccessory");
    int32_t fd;
    auto ret = g_usbDeviceInterface->OpenAccessory(fd);
    EXPECT_TRUE(ret == 0);
    HDF_LOGI("UsbdAccessoryTest::OpenAccessoryTest001 %{public}d ret=%{public}d", __LINE__, ret);
    g_usbDeviceInterface->CloseAccessory(fd);
}

/**
 * @tc.number   : SUB_USB_DeviceManager_HDI_Accessory_0300
 * @tc.name     : OpenAccessoryTest002
 * @tc.desc     : int32_t OpenAccessory
 * @tc.desc     : Positive test: parameters correctly
 * @tc.size     : MediumTest
 * @tc.type     : Function
 * @tc.level    : Level 3
 */
HWTEST_F(UsbdAccessoryTest, OpenAccessoryTest002, Function | MediumTest | Level1)
{
    HDF_LOGI("Case Start : OpenAccessoryTest002 : OpenAccessory");
    int32_t fd;
    auto ret = g_usbDeviceInterface->OpenAccessory(fd);
    EXPECT_TRUE(ret == 0);
    HDF_LOGI("UsbdAccessoryTest::OpenAccessoryTest002 %{public}d ret=%{public}d", __LINE__, ret);
    ret = g_usbDeviceInterface->OpenAccessory(fd);
    EXPECT_TRUE(ret != 0);
    HDF_LOGI("UsbdAccessoryTest::OpenAccessoryTest002 %{public}d ret=%{public}d", __LINE__, ret);
    g_usbDeviceInterface->CloseAccessory(fd);
}

/**
 * @tc.number   : SUB_USB_DeviceManager_HDI_Accessory_0400
 * @tc.name     : CloseAccessoryTest001
 * @tc.desc     : int32_t CloseAccessory
 * @tc.desc     : Positive test: parameters correctly
 * @tc.size     : MediumTest
 * @tc.type     : Function
 * @tc.level    : Level 3
 */
HWTEST_F(UsbdAccessoryTest, CloseAccessoryTest001, Function | MediumTest | Level1)
{
    HDF_LOGI("Case Start : CloseAccessoryTest001 : CloseAccessory");
    int32_t fd;
    auto ret = g_usbDeviceInterface->OpenAccessory(fd);
    EXPECT_TRUE(ret == 0);
    HDF_LOGI("UsbdAccessoryTest::CloseAccessoryTest001 %{public}d ret=%{public}d", __LINE__, ret);
    ret = g_usbDeviceInterface->CloseAccessory(fd);
    EXPECT_TRUE(ret == 0);
    HDF_LOGI("UsbdAccessoryTest::CloseAccessoryTest001 %{public}d ret=%{public}d", __LINE__, ret);
}
