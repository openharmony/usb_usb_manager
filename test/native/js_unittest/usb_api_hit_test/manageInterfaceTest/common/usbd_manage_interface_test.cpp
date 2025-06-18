/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "usbd_manage_interface_test.h"

#include <iostream>
#include <vector>

#include "hdf_log.h"
#include "v1_0/iusb_interface.h"
#include "v1_0/usb_types.h"

const int SLEEP_TIME = 3;
const uint8_t BUS_NUM_INVALID = 255;
const uint8_t DEV_ADDR_INVALID = 255;
const uint8_t INTERFACEID_OK = 1;
const uint8_t INTERFACEID_OK_NEW = 0;
const uint8_t INTERFACEID_INVALID = 255;

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::USB;
using namespace std;
using namespace OHOS::HDI::Usb::V1_0;
namespace OHOS {
namespace USB {
namespace ManageInterface {
UsbDev UsbdManageInterfaceTest::dev_ = {0, 0};
sptr<UsbSubscriberTest> UsbdManageInterfaceTest::subscriber_ = nullptr;
sptr<IUsbInterface> g_usbInterface = nullptr;

void UsbdManageInterfaceTest::SetUpTestCase(void)
{
    g_usbInterface = IUsbInterface::Get();
    if (g_usbInterface == nullptr) {
        HDF_LOGE("%{public}s:IUsbInterface::Get() failed.", __func__);
        exit(0);
    }
    auto ret = g_usbInterface->SetPortRole(1, 1, 1);
    sleep(SLEEP_TIME);
    HDF_LOGI("UsbdManageInterfaceTest::[Device] %{public}d SetPortRole=%{public}d", __LINE__, ret);
    if (ret != 0) {
        ASSERT_EQ(HDF_ERR_NOT_SUPPORT, ret);
    } else {
        ASSERT_EQ(0, ret);
    }

    subscriber_ = new UsbSubscriberTest();
    if (g_usbInterface->BindUsbdSubscriber(subscriber_) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: bind usbd subscriber_ failed", __func__);
        exit(0);
    }

    std::cout << "please connect device, press enter to continue" << std::endl;
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
    dev_ = {subscriber_->busNum_, subscriber_->devAddr_};

    ret = g_usbInterface->OpenDevice(dev_);
    HDF_LOGI("UsbdManageInterfaceTest:: %{public}d OpenDevice=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

void UsbdManageInterfaceTest::TearDownTestCase(void)
{
    g_usbInterface->UnbindUsbdSubscriber(subscriber_);
    dev_ = {subscriber_->busNum_, subscriber_->devAddr_};
    auto ret = g_usbInterface->CloseDevice(dev_);
    HDF_LOGI("UsbdManageInterfaceTest:: %{public}d Close=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

void UsbdManageInterfaceTest::SetUp(void) {}

void UsbdManageInterfaceTest::TearDown(void) {}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Func_1500
 * @tc.desc: Test functions to ManageInterface
 * @tc.desc: int32_t  ManageInterface(const UsbDev &dev, uint8_t interfaceId, bool disable);
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdManageInterfaceTest, SUB_USB_HostManager_HDI_Func_1500, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_OK_NEW;
    struct UsbDev dev = dev_;
    int32_t ret = -1;
    for (; interfaceId < INTERFACEID_INVALID; interfaceId++) {
        ret = g_usbInterface->ManageInterface(dev, interfaceId, true);
        if (ret == 0) {
            break;
        }
    }
    HDF_LOGI("UsbdManageInterfaceTest::SUB_USB_HostManager_HDI_Func_1500 %{public}d ManageInterface=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_7300
 * @tc.desc: Test functions to ManageInterface
 * @tc.desc: int32_t  ManageInterface(const UsbDev &dev, uint8_t interfaceId, bool disable);
 * @tc.desc: Negative test: parameters exception, busNum error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdManageInterfaceTest, SUB_USB_HostManager_HDI_Compatibility_7300, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_OK;
    struct UsbDev dev = dev_;
    dev.busNum = BUS_NUM_INVALID;
    auto ret = g_usbInterface->ManageInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdManageInterfaceTest::SUB_USB_HostManager_HDI_Compatibility_7300 %{public}d ret=%{public}d",
        __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_7400
 * @tc.desc: Test functions to ManageInterface
 * @tc.desc: int32_t  ManageInterface(const UsbDev &dev, uint8_t interfaceId, bool disable);
 * @tc.desc: Negative test: parameters exception, devAddr error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdManageInterfaceTest, SUB_USB_HostManager_HDI_Compatibility_7400, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_OK;
    struct UsbDev dev = {dev_.busNum, DEV_ADDR_INVALID};
    auto ret = g_usbInterface->ManageInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdManageInterfaceTest::SUB_USB_HostManager_HDI_Compatibility_7400 %{public}d ret=%{public}d",
        __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_7500
 * @tc.desc: Test functions to ManageInterface
 * @tc.desc: int32_t  ManageInterface(const UsbDev &dev, uint8_t interfaceId, bool disable);
 * @tc.desc: Negative test: parameters exception, interfaceid error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdManageInterfaceTest, SUB_USB_HostManager_HDI_Compatibility_7500, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_OK;
    struct UsbDev dev = dev_;
    interfaceId = INTERFACEID_INVALID;
    auto ret = g_usbInterface->ManageInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdManageInterfaceTest::SUB_USB_HostManager_HDI_Compatibility_7500 %{public}d ret=%{public}d",
        __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_7600
 * @tc.desc: Test functions to ManageInterface
 * @tc.desc: int32_t  ManageInterface(const UsbDev &dev, uint8_t interfaceId, bool disable);
 * @tc.desc: Negative test: parameters exception, busNum && devAddr error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdManageInterfaceTest, SUB_USB_HostManager_HDI_Compatibility_7600, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_OK;
    struct UsbDev dev = {BUS_NUM_INVALID, DEV_ADDR_INVALID};
    auto ret = g_usbInterface->ManageInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdManageInterfaceTest::SUB_USB_HostManager_HDI_Compatibility_7600 %{public}d ret=%{public}d",
        __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_7700
 * @tc.desc: Test functions to ManageInterface
 * @tc.desc: int32_t  ManageInterface(const UsbDev &dev, uint8_t interfaceId, bool disable);
 * @tc.desc: Negative test: parameters exception, busNum && interfaceid error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdManageInterfaceTest, SUB_USB_HostManager_HDI_Compatibility_7700, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_INVALID;
    struct UsbDev dev = {BUS_NUM_INVALID, dev_.devAddr};
    auto ret = g_usbInterface->ManageInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdManageInterfaceTest::SUB_USB_HostManager_HDI_Compatibility_7700 %{public}d ret=%{public}d",
        __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_7800
 * @tc.desc: Test functions to ManageInterface
 * @tc.desc: int32_t  ManageInterface(const UsbDev &dev, uint8_t interfaceId, bool disable);
 * @tc.desc: Negative test: parameters exception, devAddr && interfaceid error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdManageInterfaceTest, SUB_USB_HostManager_HDI_Compatibility_7800, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_INVALID;
    struct UsbDev dev = {dev_.busNum, DEV_ADDR_INVALID};
    auto ret = g_usbInterface->ManageInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdManageInterfaceTest::SUB_USB_HostManager_HDI_Compatibility_7800 %{public}d ret=%{public}d",
        __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_7900
 * @tc.desc: Test functions to ManageInterface
 * @tc.desc: int32_t  ManageInterface(const UsbDev &dev, uint8_t interfaceId, bool disable);
 * @tc.desc: Negative test: parameters exception, busNum && devAddr && interfaceid error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdManageInterfaceTest, SUB_USB_HostManager_HDI_Compatibility_7900, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_INVALID;
    struct UsbDev dev = {BUS_NUM_INVALID, DEV_ADDR_INVALID};
    auto ret = g_usbInterface->ManageInterface(dev, interfaceId, true);
    HDF_LOGI("UsbdManageInterfaceTest::SUB_USB_HostManager_HDI_Compatibility_7900 %{public}d ret=%{public}d",
        __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Func_1600
 * @tc.desc: Test functions to ManageInterface
 * @tc.desc: int32_t  ManageInterface(const UsbDev &dev, uint8_t interfaceId, bool disable);
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdManageInterfaceTest, SUB_USB_HostManager_HDI_Func_1600, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_OK_NEW;
    struct UsbDev dev = dev_;
        int32_t ret = -1;
    for (; interfaceId < INTERFACEID_INVALID; interfaceId++) {
        ret = g_usbInterface->ManageInterface(dev, interfaceId, false);
        if (ret == 0) {
            break;
        }
    }
    HDF_LOGI("UsbdManageInterfaceTest::SUB_USB_HostManager_HDI_Func_1600 %{public}d ManageInterface=%{public}d",
        __LINE__, ret);
    ASSERT_EQ(0, ret);
}
} // ManageInterface
} // USB
} // OHOS
