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

#include "usbd_device_status_test.h"

#include <iostream>
#include <vector>

#include "UsbSubscriberTest.h"
#include "hdf_log.h"
#include "v2_0/iusb_host_interface.h"
#include "v2_0/iusb_port_interface.h"

const int SLEEP_TIME = 3;
const uint8_t BUS_NUM_INVALID = 255;
const uint8_t DEV_ADDR_INVALID = 255;
const uint8_t INTERFACEID_OK = 1;
const uint8_t INTERFACEID_OK_NEW = 0;
const uint8_t INTERFACEID_INVALID = 255;
const uint8_t POINTID_DIR_IN = USB_ENDPOINT_DIR_IN | 2;

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::USB;
using namespace std;
using namespace OHOS::HDI::Usb::V2_0;
namespace OHOS {
namespace USB {
UsbDev UsbdDeviceStatusTest::dev_ = {0, 0};
sptr<UsbSubscriberTest> UsbdDeviceStatusTest::subscriber_ = nullptr;
sptr<OHOS::HDI::Usb::V2_0::IUsbHostInterface> g_usbHostInterface = nullptr;
sptr<OHOS::HDI::Usb::V2_0::IUsbPortInterface> g_usbPortInterface = nullptr;

void UsbdDeviceStatusTest::SetUpTestCase(void)
{
    g_usbPortInterface = OHOS::HDI::Usb::V2_0::IUsbPortInterface::Get();
    g_usbHostInterface = OHOS::HDI::Usb::V2_0::IUsbHostInterface::Get(true);
    if (g_usbPortInterface == nullptr || g_usbHostInterface == nullptr) {
        HDF_LOGE("%{public}s:g_usbPortInterface or g_usbHostInterface is nullptr.", __func__);
        exit(0);
    }
    auto ret = g_usbPortInterface->SetPortRole(1, 1, 1);
    sleep(SLEEP_TIME);
    HDF_LOGI("UsbdDeviceStatusTest::[Device] %{public}d SetPortRole=%{public}d", __LINE__, ret);
    if (ret != 0) {
        ASSERT_EQ(HDF_ERR_NOT_SUPPORT, ret);
    } else {
        ASSERT_EQ(0, ret);
    }

    subscriber_ = new UsbSubscriberTest();
    if (g_usbHostInterface->BindUsbdHostSubscriber(subscriber_) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: bind usbd subscriber_ failed", __func__);
        exit(0);
    }

    std::cout << "please connect device, press enter to continue" << std::endl;
    dev_ = {subscriber_->busNum_, subscriber_->devAddr_};

    ret = g_usbHostInterface->OpenDevice(dev_);
    HDF_LOGI("UsbdDeviceStatusTest::%{public}d OpenDevice=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

void UsbdDeviceStatusTest::TearDownTestCase(void)
{
    g_usbHostInterface->UnbindUsbdHostSubscriber(subscriber_);
    dev_ = {subscriber_->busNum_, subscriber_->devAddr_};
    auto ret = g_usbHostInterface->CloseDevice(dev_);
    HDF_LOGI("UsbdDeviceStatusTest::%{public}d Close=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

void UsbdDeviceStatusTest::SetUp(void) {}

void UsbdDeviceStatusTest::TearDown(void) {}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Func_1000
 * @tc.desc: Test functions to GetDeviceSpeed
 * @tc.desc: int32_t  GetDeviceSpeed(const UsbDev &dev, uint8_t interfaceId, uint8_t speed);
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdGetDeviceSpeed001, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    int32_t ret = -1;
    uint8_t speed = 0;
    ret = g_usbHostInterface->GetDeviceSpeed(dev, speed);
    HDF_LOGI("UsbdGetDeviceSpeed001 %{public}d GetDeviceSpeed=%{public}d, speed=%{public}d", __LINE__, ret, speed);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_8000
 * @tc.desc: Test functions to GetDeviceSpeed
 * @tc.desc: int32_t  GetDeviceSpeed(const UsbDev &dev, uint8_t interfaceId, uint8_t speed);
 * @tc.desc: Negative test: parameters exception, busNum error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdGetDeviceSpeed002, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    dev.busNum = BUS_NUM_INVALID;
    uint8_t speed = 0;
    auto ret = g_usbHostInterface->GetDeviceSpeed(dev, speed);
    HDF_LOGI("UsbdGetDeviceSpeed002 %{public}d ret=%{public}d, speed=%{public}d", __LINE__, ret, speed);
    ASSERT_NE(ret, 0);
}


/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_8100
 * @tc.desc: Test functions to GetDeviceSpeed
 * @tc.desc: int32_t  GetDeviceSpeed(const UsbDev &dev, uint8_t interfaceId, uint8_t speed);
 * @tc.desc: Negative test: parameters exception, devAddr error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdGetDeviceSpeed003, Function | MediumTest | Level1)
{
    struct UsbDev dev = {dev_.busNum, DEV_ADDR_INVALID};
    uint8_t speed = 0;
    auto ret = g_usbHostInterface->GetDeviceSpeed(dev, speed);
    HDF_LOGI("UsbdGetDeviceSpeed003 %{public}d, ret=%{public}d, speed=%{public}d", __LINE__, ret, speed);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_8200
 * @tc.desc: Test functions to GetDeviceSpeed
 * @tc.desc: int32_t  GetDeviceSpeed(const UsbDev &dev, uint8_t interfaceId, uint8_t speed);
 * @tc.desc: Negative test: parameters exception, busNum && devAddr error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdGetDeviceSpeed004, Function | MediumTest | Level1)
{
    uint8_t speed = 0;
    struct UsbDev dev = {BUS_NUM_INVALID, DEV_ADDR_INVALID};
    auto ret = g_usbHostInterface->GetDeviceSpeed(dev, speed);
    HDF_LOGI("UsbdGetDeviceSpeed004 %{public}d, ret=%{public}d, speed=%{public}d", __LINE__, ret, speed);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Func_1100
 * @tc.desc: Test functions to GetInterfaceActiveStatus
 * @tc.desc: int32_t  GetInterfaceActiveStatus(const UsbDev &dev, uint8_t interfaceId, bool unactived);
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdGetInterfaceActiveStatus001, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_OK_NEW;
    struct UsbDev dev = dev_;
    int32_t ret = -1;
    bool unactived = 1;
    ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdDeviceStatusTest::UsbdGetInterfaceActiveStatus %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    for (; interfaceId < INTERFACEID_INVALID; interfaceId++) {
        ret = g_usbHostInterface->GetInterfaceActiveStatus(dev, interfaceId, unactived);
        if (ret == 0) {
            break;
        }
    }
    HDF_LOGI("UsbdGetInterfaceActiveStatus001 %{public}d GetInterfaceActiveStatus=%{public}d, unactived=%{public}d",
        __LINE__, ret, unactived);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_8300
 * @tc.desc: Test functions to GetInterfaceActiveStatus
 * @tc.desc: int32_t  GetInterfaceActiveStatus(const UsbDev &dev, uint8_t interfaceId, bool unactived);
 * @tc.desc: Negative test: parameters exception, busNum error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdGetInterfaceActiveStatus002, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_OK;
    struct UsbDev dev = dev_;
    dev.busNum = BUS_NUM_INVALID;
    bool unactived = 1;
    auto ret = g_usbHostInterface->GetInterfaceActiveStatus(dev, interfaceId, unactived);
    HDF_LOGI("UsbdGetInterfaceActiveStatus002 %{public}d ret=%{public}d, unactived=%{public}d",
        __LINE__, ret, unactived);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_8400
 * @tc.desc: Test functions to GetInterfaceActiveStatus
 * @tc.desc: int32_t  GetInterfaceActiveStatus(const UsbDev &dev, uint8_t interfaceId, bool unactived);
 * @tc.desc: Negative test: parameters exception, devAddr error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdGetInterfaceActiveStatus003, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_OK;
    struct UsbDev dev = {dev_.busNum, DEV_ADDR_INVALID};
    bool unactived = 1;
    auto ret = g_usbHostInterface->GetInterfaceActiveStatus(dev, interfaceId, unactived);
    HDF_LOGI("UsbdGetInterfaceActiveStatus003 %{public}d, ret=%{public}d, unactived=%{public}d",
        __LINE__, ret, unactived);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_8500
 * @tc.desc: Test functions to GetInterfaceActiveStatus
 * @tc.desc: int32_t  GetInterfaceActiveStatus(const UsbDev &dev, uint8_t interfaceId, bool unactived);
 * @tc.desc: Negative test: parameters exception, interfaceid error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdGetInterfaceActiveStatus004, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_OK;
    struct UsbDev dev = dev_;
    interfaceId = INTERFACEID_INVALID;
    bool unactived = 1;
    auto ret = g_usbHostInterface->GetInterfaceActiveStatus(dev, interfaceId, unactived);
    HDF_LOGI("UsbdGetInterfaceActiveStatus004 %{public}d, ret=%{public}d, unactived=%{public}d",
        __LINE__, ret, unactived);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_8600
 * @tc.desc: Test functions to GetInterfaceActiveStatus
 * @tc.desc: int32_t  GetInterfaceActiveStatus(const UsbDev &dev, uint8_t interfaceId, bool unactived);
 * @tc.desc: Negative test: parameters exception, busNum && devAddr error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdGetInterfaceActiveStatus005, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_OK;
    bool unactived = 1;
    struct UsbDev dev = {BUS_NUM_INVALID, DEV_ADDR_INVALID};
    auto ret = g_usbHostInterface->GetInterfaceActiveStatus(dev, interfaceId, unactived);
    HDF_LOGI("UsbdGetInterfaceActiveStatus005 %{public}d, ret=%{public}d, unactived=%{public}d",
        __LINE__, ret, unactived);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_8700
 * @tc.desc: Test functions to GetInterfaceActiveStatus
 * @tc.desc: int32_t  GetInterfaceActiveStatus(const UsbDev &dev, uint8_t interfaceId, bool unactived);
 * @tc.desc: Negative test: parameters exception, busNum && interfaceid error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdGetInterfaceActiveStatus006, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_INVALID;
    bool unactived = 1;
    struct UsbDev dev = {BUS_NUM_INVALID, dev_.devAddr};
    auto ret = g_usbHostInterface->GetInterfaceActiveStatus(dev, interfaceId, unactived);
    HDF_LOGI("UsbdGetInterfaceActiveStatus006 %{public}d, ret=%{public}d, unactived=%{public}d",
        __LINE__, ret, unactived);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_8800
 * @tc.desc: Test functions to GetInterfaceActiveStatus
 * @tc.desc: int32_t  GetInterfaceActiveStatus(const UsbDev &dev, uint8_t interfaceId, bool unactived);
 * @tc.desc: Negative test: parameters exception, devAddr && interfaceid error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdGetInterfaceActiveStatus007, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_INVALID;
    bool unactived = 1;
    struct UsbDev dev = {dev_.busNum, DEV_ADDR_INVALID};
    auto ret = g_usbHostInterface->GetInterfaceActiveStatus(dev, interfaceId, unactived);
    HDF_LOGI("UsbdGetInterfaceActiveStatus007 %{public}d, ret=%{public}d, unactived=%{public}d",
        __LINE__, ret, unactived);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_Compatibility_8900
 * @tc.desc: Test functions to GetInterfaceActiveStatus
 * @tc.desc: int32_t  GetInterfaceActiveStatus(const UsbDev &dev, uint8_t interfaceId, bool unactived);
 * @tc.desc: Negative test: parameters exception, busNum && devAddr && interfaceid error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdGetInterfaceActiveStatus008, Function | MediumTest | Level1)
{
    uint8_t interfaceId = INTERFACEID_INVALID;
    bool unactived = 1;
    struct UsbDev dev = {BUS_NUM_INVALID, DEV_ADDR_INVALID};
    auto ret = g_usbHostInterface->GetInterfaceActiveStatus(dev, interfaceId, unactived);
    HDF_LOGI("UsbdGetInterfaceActiveStatus008 %{public}d, ret=%{public}d, unactived=%{public}d",
        __LINE__, ret, unactived);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number   : SUB_USB_HostManager_HDI_Func_2000
 * @tc.name     : UsbdClearHalt001
 * @tc.desc     : int32_t ClearHalt(const UsbDev &dev, const UsbPipe &pipe)
 * @tc.desc     : Positive test: parameters correctly
 * @tc.size     : MediumTest
 * @tc.type     : Function
 * @tc.level    : Level 3
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdClearHalt001, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    int32_t ret = -1;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_DIR_IN;
    ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdDeviceStatusTest::UsbdClearHalt001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    struct UsbPipe pipe = {interfaceId, pointId};
    ret = g_usbHostInterface->ClearHalt(dev, pipe);
    HDF_LOGI("UsbdClearHalt001 %{public}d ClearHalt=%{public}d", __LINE__, ret);
}

/**
 * @tc.number   : SUB_USB_HostManager_HDI_Compatibility_9300
 * @tc.name     : UsbdClearHalt002
 * @tc.desc     : int32_t ClearHalt(const UsbDev &dev, const UsbPipe &pipe)
 * @tc.desc     : Negative test: parameters exception, busNum error
 * @tc.size     : MediumTest
 * @tc.type     : Function
 * @tc.level    : Level 3
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdClearHalt002, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    int32_t ret = -1;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_DIR_IN;
    ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdDeviceStatusTest::UsbdClearHalt002 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    struct UsbPipe pipe = {interfaceId, pointId};
    dev.busNum = BUS_NUM_INVALID;
    ret = g_usbHostInterface->ClearHalt(dev, pipe);
    HDF_LOGI("UsbdClearHalt002 %{public}d ClearHalt=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number   : SUB_USB_HostManager_HDI_Compatibility_9400
 * @tc.name     : UsbdClearHalt003
 * @tc.desc     : int32_t ClearHalt(const UsbDev &dev, const UsbPipe &pipe)
 * @tc.desc     : Negative test: parameters exception, devAddr error
 * @tc.size     : MediumTest
 * @tc.type     : Function
 * @tc.level    : Level 3
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdClearHalt003, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    int32_t ret = -1;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_DIR_IN;
    ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdDeviceStatusTest::UsbdClearHalt003 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    struct UsbPipe pipe = {interfaceId, pointId};
    dev.devAddr = DEV_ADDR_INVALID;
    ret = g_usbHostInterface->ClearHalt(dev, pipe);
    HDF_LOGI("UsbdClearHalt003 %{public}d ClearHalt=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number   : SUB_USB_HostManager_HDI_Compatibility_9500
 * @tc.name     : UsbdClearHalt004
 * @tc.desc     : int32_t ClearHalt(const UsbDev &dev, const UsbPipe &pipe)
 * @tc.desc     : Negative test: parameters exception, interfaceid error
 * @tc.size     : MediumTest
 * @tc.type     : Function
 * @tc.level    : Level 3
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdClearHalt004, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    int32_t ret = -1;
    uint8_t interfaceId = INTERFACEID_INVALID;
    uint8_t pointId = POINTID_DIR_IN;
    ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdDeviceStatusTest::UsbdClearHalt004 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
    struct UsbPipe pipe = {interfaceId, pointId};
    ret = g_usbHostInterface->ClearHalt(dev, pipe);
    HDF_LOGI("UsbdClearHalt004 %{public}d ClearHalt=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number   : SUB_USB_HostManager_HDI_Compatibility_9600
 * @tc.name     : UsbdClearHalt005
 * @tc.desc     : int32_t ClearHalt(const UsbDev &dev, const UsbPipe &pipe)
 * @tc.desc     : Negative test: parameters exception, busNum && devAddr error
 * @tc.size     : MediumTest
 * @tc.type     : Function
 * @tc.level    : Level 3
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdClearHalt005, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    int32_t ret = -1;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_DIR_IN;
    ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdDeviceStatusTest::UsbdClearHalt005 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    struct UsbPipe pipe = {interfaceId, pointId};
    dev.busNum = BUS_NUM_INVALID;
    dev.devAddr = DEV_ADDR_INVALID;
    ret = g_usbHostInterface->ClearHalt(dev, pipe);
    HDF_LOGI("UsbdClearHalt005 %{public}d ClearHalt=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number   : SUB_USB_HostManager_HDI_Compatibility_9700
 * @tc.name     : UsbdClearHalt006
 * @tc.desc     : int32_t ClearHalt(const UsbDev &dev, const UsbPipe &pipe)
 * @tc.desc     : Negative test: parameters exception, busNum && interfaceid error
 * @tc.size     : MediumTest
 * @tc.type     : Function
 * @tc.level    : Level 3
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdClearHalt006, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    int32_t ret = -1;
    uint8_t interfaceId = INTERFACEID_INVALID;
    uint8_t pointId = POINTID_DIR_IN;
    ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdDeviceStatusTest::UsbdClearHalt006 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
    struct UsbPipe pipe = {interfaceId, pointId};
    dev.busNum = BUS_NUM_INVALID;
    ret = g_usbHostInterface->ClearHalt(dev, pipe);
    HDF_LOGI("UsbdClearHalt006 %{public}d ClearHalt=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number   : SUB_USB_HostManager_HDI_Compatibility_9800
 * @tc.name     : UsbdClearHalt007
 * @tc.desc     : int32_t ClearHalt(const UsbDev &dev, const UsbPipe &pipe)
 * @tc.desc     : Negative test: parameters exception, devAddr && interfaceid error
 * @tc.size     : MediumTest
 * @tc.type     : Function
 * @tc.level    : Level 3
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdClearHalt007, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    int32_t ret = -1;
    uint8_t interfaceId = INTERFACEID_INVALID;
    uint8_t pointId = POINTID_DIR_IN;
    ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdDeviceStatusTest::UsbdClearHalt007 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
    struct UsbPipe pipe = {interfaceId, pointId};
    dev.devAddr = DEV_ADDR_INVALID;
    ret = g_usbHostInterface->ClearHalt(dev, pipe);
    HDF_LOGI("UsbdClearHalt005 %{public}d ClearHalt=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number   : SUB_USB_HostManager_HDI_Compatibility_9900
 * @tc.name     : UsbdClearHalt008
 * @tc.desc     : int32_t ClearHalt(const UsbDev &dev, const UsbPipe &pipe)
 * @tc.desc     : Negative test: parameters exception, busNum && devAddr && interfaceid error
 * @tc.size     : MediumTest
 * @tc.type     : Function
 * @tc.level    : Level 3
 */
HWTEST_F(UsbdDeviceStatusTest, UsbdClearHalt008, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    int32_t ret = -1;
    uint8_t interfaceId = INTERFACEID_INVALID;
    uint8_t pointId = POINTID_DIR_IN;
    ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdDeviceStatusTest::UsbdClearHalt008 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
    struct UsbPipe pipe = {interfaceId, pointId};
    dev.busNum = BUS_NUM_INVALID;
    dev.devAddr = DEV_ADDR_INVALID;
    ret = g_usbHostInterface->ClearHalt(dev, pipe);
    HDF_LOGI("UsbdClearHalt008 %{public}d ClearHalt=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
}
} // USB
} // OHOS
