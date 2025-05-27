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
#include <vector>

#include "hdf_log.h"
#include "usbd_device_test.h"
#include "v2_0/iusb_host_interface.h"
#include "v2_0/iusb_port_interface.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::USB;
using namespace std;
using namespace OHOS::HDI::Usb::V2_0;

const int SLEEP_TIME = 3;
const uint8_t BUS_NUM_INVALID = 255;
const uint8_t DEV_ADDR_INVALID = 255;
sptr<UsbSubscriberTest> UsbdDeviceTest::subscriber_ = nullptr;

namespace {
sptr<OHOS::HDI::Usb::V2_0::IUsbHostInterface> g_usbHostInterface = nullptr;
sptr<OHOS::HDI::Usb::V2_0::IUsbPortInterface> g_usbPortInterface = nullptr;

struct UsbDev UsbdDeviceTest::dev_ = { 0, 0 };

void UsbdDeviceTest::SetUpTestCase(void)
{
    g_usbHostInterface = OHOS::HDI::Usb::V2_0::IUsbHostInterface::Get(true);
    g_usbPortInterface = OHOS::HDI::Usb::V2_0::IUsbPortInterface::Get();
    if (g_usbPortInterface == nullptr || g_usbHostInterface == nullptr) {
        HDF_LOGE("%{public}s:g_usbPortInterface or g_usbHostInterface is nullptr.", __func__);
        exit(0);
    }
    auto ret = g_usbPortInterface->SetPortRole(1, 1, 1);
    sleep(SLEEP_TIME);
    HDF_LOGI("UsbdDeviceTest::[Device] %{public}d SetPortRole=%{public}d", __LINE__, ret);
    if (ret != 0) {
        ASSERT_EQ(HDF_ERR_NOT_SUPPORT, ret);
    } else {
        ASSERT_EQ(0, ret);
    }

    subscriber_ = new UsbSubscriberTest();
    if (g_usbHostInterface->BindUsbdHostSubscriber(subscriber_) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: bind usbd subscriber_ failed\n", __func__);
        exit(0);
    }
    dev_ = { subscriber_->busNum_, subscriber_->devAddr_ };

    std::cout << "please connect device, press enter to continue" << std::endl;
}

void UsbdDeviceTest::TearDownTestCase(void)
{
    g_usbHostInterface->UnbindUsbdHostSubscriber(subscriber_);
}

void UsbdDeviceTest::SetUp(void) {}

void UsbdDeviceTest::TearDown(void) {}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Func_0100
 * @tc.desc: Test functions to OpenDevice
 * @tc.desc: int32_t OpenDevice(const UsbDev &dev);
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceTest, SUB_USB_HostManager_HDI_Func_0100, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    auto ret = g_usbHostInterface->OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result =%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_0100
 * @tc.desc: Test functions to OpenDevice
 * @tc.desc: int32_t OpenDevice(const UsbDev &dev);
 * @tc.desc: Negative test: parameters exception, busNum error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceTest, SUB_USB_HostManager_HDI_Compatibility_0100, Function | MediumTest | Level1)
{
    struct UsbDev dev = { BUS_NUM_INVALID, dev_.devAddr };
    auto ret = g_usbHostInterface->OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_0200
 * @tc.desc: Test functions to OpenDevice
 * @tc.desc: int32_t OpenDevice(const UsbDev &dev);
 * @tc.desc: Negative test: parameters exception, devAddr error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceTest, SUB_USB_HostManager_HDI_Compatibility_0200, Function | MediumTest | Level1)
{
    struct UsbDev dev = { dev_.busNum, DEV_ADDR_INVALID };
    auto ret = g_usbHostInterface->OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_0300
 * @tc.desc: Test functions to OpenDevice
 * @tc.desc: int32_t OpenDevice(const UsbDev &dev);
 * @tc.desc: Negative test: parameters exception, busNum && devAddr error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceTest, SUB_USB_HostManager_HDI_Compatibility_0300, Function | MediumTest | Level1)
{
    struct UsbDev dev = { BUS_NUM_INVALID, DEV_ADDR_INVALID };
    auto ret = g_usbHostInterface->OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**********************************************************************************************************/

/**
 * @tc.name: SUB_USB_HostManager_HDI_Func_1400
 * @tc.desc: Test functions to CloseDevice
 * @tc.desc: int32_t CloseDevice(const UsbDev &dev);
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceTest, SUB_USB_HostManager_HDI_Func_1400, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    auto ret = g_usbHostInterface->OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    ret = g_usbHostInterface->CloseDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d Close result=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_7000
 * @tc.desc: Test functions to CloseDevice
 * @tc.desc: int32_t CloseDevice(const UsbDev &dev);
 * @tc.desc: Negative test: parameters exception, busNum error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceTest, SUB_USB_HostManager_HDI_Compatibility_7000, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    auto ret = g_usbHostInterface->OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    dev.busNum = BUS_NUM_INVALID;
    ret = g_usbHostInterface->CloseDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d Close result=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
    dev = dev_;
    g_usbHostInterface->CloseDevice(dev);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_7100
 * @tc.desc: Test functions to CloseDevice
 * @tc.desc: int32_t CloseDevice(const UsbDev &dev);
 * @tc.desc: Negative test: parameters exception, devAddr error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceTest, SUB_USB_HostManager_HDI_Compatibility_7100, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    auto ret = g_usbHostInterface->OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    dev.devAddr = DEV_ADDR_INVALID;
    ret = g_usbHostInterface->CloseDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d Close result=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
    dev = dev_;
    g_usbHostInterface->CloseDevice(dev);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_7200
 * @tc.desc: Test functions to CloseDevice
 * @tc.desc: int32_t CloseDevice(const UsbDev &dev);
 * @tc.desc: Negative test: parameters exception, busNum && devAddr error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceTest, SUB_USB_HostManager_HDI_Compatibility_7200, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    auto ret = g_usbHostInterface->OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    dev.busNum = BUS_NUM_INVALID;
    dev.devAddr = DEV_ADDR_INVALID;
    ret = g_usbHostInterface->CloseDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d Close result=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
    dev = dev_;
    g_usbHostInterface->CloseDevice(dev);
}

/**
 * @tc.number   : SUB_USB_HostManager_HDI_Func_1900
 * @tc.name     : UsbdResetDevice001
 * @tc.desc     : int32_t ResetDevice(const UsbDev &dev)
 * @tc.desc     : Positive test: parameters correctly
 * @tc.size     : MediumTest
 * @tc.type     : Function
 * @tc.level    : Level 3
 */
HWTEST_F(UsbdDeviceTest, UsbdResetDevice001, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    auto ret = g_usbHostInterface->OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    ret = g_usbHostInterface->ResetDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d UsbdResetDevice001 result=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    ret = g_usbHostInterface->CloseDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d Close result=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number   : SUB_USB_HostManager_HDI_Compatibility_9000
 * @tc.name     : UsbdResetDevice002
 * @tc.desc     : int32_t ResetDevice(const UsbDev &dev)
 * @tc.desc     : Negative test
 * @tc.size     : MediumTest
 * @tc.type     : Function
 * @tc.level    : Level 3
 */
HWTEST_F(UsbdDeviceTest, UsbdResetDevice002, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    auto ret = g_usbHostInterface->OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    dev.busNum = BUS_NUM_INVALID;
    ret = g_usbHostInterface->ResetDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d UsbdResetDevice002 result=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
    ret = g_usbHostInterface->CloseDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d Close result=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
    dev = dev_;
    g_usbHostInterface->CloseDevice(dev);
}

/**
 * @tc.number   : SUB_USB_HostManager_HDI_Compatibility_9100
 * @tc.name     : UsbdResetDevice003
 * @tc.desc     : int32_t ResetDevice(const UsbDev &dev)
 * @tc.desc     : Negative test
 * @tc.size     : MediumTest
 * @tc.type     : Function
 * @tc.level    : Level 3
 */
HWTEST_F(UsbdDeviceTest, UsbdResetDevice003, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    auto ret = g_usbHostInterface->OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    dev.devAddr = DEV_ADDR_INVALID;
    ret = g_usbHostInterface->ResetDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d UsbdResetDevice003 result=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
    ret = g_usbHostInterface->CloseDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d Close result=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
    dev = dev_;
    g_usbHostInterface->CloseDevice(dev);
}

/**
 * @tc.number   : SUB_USB_HostManager_HDI_Compatibility_9200
 * @tc.name     : UsbdResetDevice004
 * @tc.desc     : int32_t ResetDevice(const UsbDev &dev)
 * @tc.desc     : Negative test
 * @tc.size     : MediumTest
 * @tc.type     : Function
 * @tc.level    : Level 3
 */
HWTEST_F(UsbdDeviceTest, UsbdResetDevice004, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    auto ret = g_usbHostInterface->OpenDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d OpenDevice result=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    dev.busNum = BUS_NUM_INVALID;
    dev.devAddr = DEV_ADDR_INVALID;
    ret = g_usbHostInterface->ResetDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d UsbdResetDevice004 result=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
    ret = g_usbHostInterface->CloseDevice(dev);
    HDF_LOGI("UsbdDeviceTest:: Line:%{public}d Close result=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
    dev = dev_;
    g_usbHostInterface->CloseDevice(dev);
}
} // namespace