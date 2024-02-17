/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "usb_event_mock_test.h"

#include <gtest/gtest.h>
#include <iostream>
#include "cJSON.h"
#include <semaphore.h>

#include "common_event_manager.h"
#include "common_event_support.h"
#include "usb_common_test.h"
#include "usb_device.h"
#include "usb_device_pipe.h"
#include "usb_srv_client.h"
#include "usb_srv_support.h"

using namespace OHOS;
using namespace OHOS::HDI::Usb::V1_0;
using namespace OHOS::USB;
using namespace OHOS::EventFwk;
using namespace OHOS::USB::Common;
using namespace std;
using namespace testing::ext;

using ::testing::Eq;
using ::testing::Exactly;
using ::testing::Ge;
using ::testing::Le;
using ::testing::Ne;
using ::testing::Return;

namespace OHOS {
namespace USB {
sem_t UsbEventMockTest::testSem_ {};
sptr<MockUsbImpl> UsbEventMockTest::mockUsbImpl_ = nullptr;
sptr<UsbService> UsbEventMockTest::usbSrv_ = nullptr;

class UsbSubscriberMockTest : public CommonEventSubscriber {
public:
    explicit UsbSubscriberMockTest(const CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp) {}

    void OnReceiveEvent(const CommonEventData &data) override
    {
        USB_HILOGI(MODULE_USB_SERVICE, "recv event ok");
        eventData_ = data;
        sem_post(&UsbEventMockTest::testSem_);
    }

    static CommonEventData eventData_;
};

CommonEventData UsbSubscriberMockTest::eventData_ {};

void UsbEventMockTest::SetUpTestCase(void)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbEventMockTest SetUpTestCase Start");
    UsbCommonTest::SetTestCaseHapApply();

    usbSrv_ = DelayedSpSingleton<UsbService>::GetInstance();
    EXPECT_NE(usbSrv_, nullptr);
    mockUsbImpl_ = DelayedSpSingleton<MockUsbImpl>::GetInstance();
    EXPECT_NE(mockUsbImpl_, nullptr);

    usbSrv_->SetUsbd(mockUsbImpl_);

    sptr<UsbServiceSubscriber> iSubscriber = new UsbServiceSubscriber();
    EXPECT_NE(iSubscriber, nullptr);
    mockUsbImpl_->BindUsbdSubscriber(iSubscriber);
}

void UsbEventMockTest::TearDownTestCase(void)
{
    USB_HILOGI(MODULE_USB_SERVICE, "UsbEventMockTest TearDownTestCase enter");
    sptr<IUsbInterface> usbd = IUsbInterface::Get();
    mockUsbImpl_->UnbindUsbdSubscriber(nullptr);
    usbSrv_->SetUsbd(usbd);

    mockUsbImpl_ = nullptr;
    usbSrv_ = nullptr;
    DelayedSpSingleton<UsbService>::DestroyInstance();
    DelayedSpSingleton<MockUsbImpl>::DestroyInstance();
}

void UsbEventMockTest::SetUp()
{
    sem_init(&UsbEventMockTest::testSem_, 1, 0);
}

void UsbEventMockTest::TearDown()
{
    sem_destroy(&UsbEventMockTest::testSem_);
}

/**
 * @tc.name: SUB_USB_Broadcast_0100
 * @tc.desc: usb gadget connected event
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbEventMockTest, SUB_USB_Broadcast_0100, TestSize.Level1)
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USB_STATE);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<UsbSubscriberMockTest> subscriber = std::make_shared<UsbSubscriberMockTest>(subscriberInfo);
    CommonEventManager::SubscribeCommonEvent(subscriber);

    USBDeviceInfo info;
    info.status = ACT_UPDEVICE;
    EXPECT_CALL(*mockUsbImpl_, GetCurrentFunctions(testing::_)).WillRepeatedly(Return(0));
    auto ret = mockUsbImpl_->SubscriberDeviceEvent(info);
    EXPECT_EQ(0, ret);

    sem_wait(&UsbEventMockTest::testSem_);
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
    auto &want = UsbSubscriberMockTest::eventData_.GetWant();
    EXPECT_EQ(want.GetAction(), CommonEventSupport::COMMON_EVENT_USB_STATE);
    EXPECT_TRUE(want.GetBoolParam(std::string {UsbSrvSupport::CONNECTED}, false));
    EXPECT_TRUE(want.GetBoolParam(std::string {UsbSrvSupport::FUNCTION_NAME_HDC}, false));
}

/**
 * @tc.name: SUB_USB_Broadcast_0200
 * @tc.desc: usb gadget disconnected event
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbEventMockTest, SUB_USB_Broadcast_0200, TestSize.Level1)
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USB_STATE);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<UsbSubscriberMockTest> subscriber = std::make_shared<UsbSubscriberMockTest>(subscriberInfo);
    CommonEventManager::SubscribeCommonEvent(subscriber);

    USBDeviceInfo info;
    info.status = ACT_DOWNDEVICE;
    EXPECT_CALL(*mockUsbImpl_, GetCurrentFunctions(testing::_)).WillRepeatedly(Return(0));
    auto ret = mockUsbImpl_->SubscriberDeviceEvent(info);
    EXPECT_EQ(0, ret);

    sem_wait(&UsbEventMockTest::testSem_);
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
    auto &want = UsbSubscriberMockTest::eventData_.GetWant();
    EXPECT_EQ(want.GetAction(), CommonEventSupport::COMMON_EVENT_USB_STATE);
    EXPECT_FALSE(want.GetBoolParam(std::string {UsbSrvSupport::CONNECTED}, true));
    EXPECT_TRUE(want.GetBoolParam(std::string {UsbSrvSupport::FUNCTION_NAME_HDC}, false));
}

/**
 * @tc.name: SUB_USB_Broadcast_0300
 * @tc.desc: usb device attached event
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbEventMockTest, SUB_USB_Broadcast_0300, TestSize.Level1)
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USB_DEVICE_ATTACHED);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<UsbSubscriberMockTest> subscriber = std::make_shared<UsbSubscriberMockTest>(subscriberInfo);
    CommonEventManager::SubscribeCommonEvent(subscriber);

    USBDeviceInfo info = {ACT_DEVUP, BUS_NUM_OK, DEV_ADDR_OK};
    EXPECT_CALL(*mockUsbImpl_, OpenDevice(testing::_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockUsbImpl_, CloseDevice(testing::_)).WillRepeatedly(Return(0));
    auto ret = mockUsbImpl_->SubscriberDeviceEvent(info);
    EXPECT_EQ(0, ret);

    sem_wait(&UsbEventMockTest::testSem_);
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
    auto &want = UsbSubscriberMockTest::eventData_.GetWant();
    EXPECT_EQ(want.GetAction(), CommonEventSupport::COMMON_EVENT_USB_DEVICE_ATTACHED);
    std::string deviceStr = UsbSubscriberMockTest::eventData_.GetData();
    cJSON* deviceJson = cJSON_Parse(deviceStr.c_str());
    if (!deviceJson) {
        USB_HILOGI(MODULE_USB_SERVICE, "SUB_USB_Broadcast_0300 error, parse json string error");
    }
    EXPECT_TRUE(deviceJson);
    UsbDevice device(deviceJson);
    EXPECT_NE(device.GetiProduct(), 0);
    EXPECT_NE(device.GetiManufacturer(), 0);
    EXPECT_GT(device.GetConfigCount(), 0);
    USBConfig config;
    device.GetConfig(0, config);
    EXPECT_NE(config.GetAttributes(), 0);
    EXPECT_GT(config.GetInterfaceCount(), 0);
    UsbInterface interface;
    config.GetInterface(0, interface);
    EXPECT_NE(interface.GetClass(), 0);
    EXPECT_NE(interface.GetEndpointCount(), 0);
    auto endpoint = interface.GetEndpoint(0);
    EXPECT_NE(endpoint.value().GetAttributes(), 0);

    info.status = ACT_DOWNDEVICE;
    EXPECT_CALL(*mockUsbImpl_, CloseDevice(testing::_)).WillRepeatedly(Return(0));
    ret = mockUsbImpl_->SubscriberDeviceEvent(info);
    EXPECT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_Broadcast_0400
 * @tc.desc: usb device detached event
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbEventMockTest, SUB_USB_Broadcast_0400, TestSize.Level1)
{
    USBDeviceInfo info = {ACT_DEVUP, BUS_NUM_OK, DEV_ADDR_OK};
    EXPECT_CALL(*mockUsbImpl_, OpenDevice(testing::_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockUsbImpl_, CloseDevice(testing::_)).WillRepeatedly(Return(0));
    auto ret = mockUsbImpl_->SubscriberDeviceEvent(info);
    EXPECT_EQ(0, ret);

    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USB_DEVICE_DETACHED);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<UsbSubscriberMockTest> subscriber = std::make_shared<UsbSubscriberMockTest>(subscriberInfo);
    CommonEventManager::SubscribeCommonEvent(subscriber);

    info.status = ACT_DEVDOWN;
    EXPECT_CALL(*mockUsbImpl_, CloseDevice(testing::_)).WillRepeatedly(Return(0));
    ret = mockUsbImpl_->SubscriberDeviceEvent(info);
    EXPECT_EQ(0, ret);

    sem_wait(&UsbEventMockTest::testSem_);
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
    auto &want = UsbSubscriberMockTest::eventData_.GetWant();
    EXPECT_EQ(want.GetAction(), CommonEventSupport::COMMON_EVENT_USB_DEVICE_DETACHED);
    std::string deviceStr = UsbSubscriberMockTest::eventData_.GetData();
    cJSON* deviceJson = cJSON_Parse(deviceStr.c_str());
    if (!deviceJson) {
        USB_HILOGI(MODULE_USB_SERVICE, "SUB_USB_Broadcast_0400 error, parse json string error");
    }
    EXPECT_TRUE(deviceJson);
    UsbDevice device(deviceJson);
    EXPECT_NE(device.GetiProduct(), 0);
    EXPECT_NE(device.GetiManufacturer(), 0);
}

/**
 * @tc.name: SUB_USB_Broadcast_0500
 * @tc.desc: usb port change to host event
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbEventMockTest, SUB_USB_Broadcast_0500, TestSize.Level1)
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USB_PORT_CHANGED);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<UsbSubscriberMockTest> subscriber = std::make_shared<UsbSubscriberMockTest>(subscriberInfo);
    CommonEventManager::SubscribeCommonEvent(subscriber);

    int32_t portId = DEFAULT_PORT_ID;
    int32_t powerRole = UsbSrvSupport::POWER_ROLE_SOURCE;
    int32_t dataRole = UsbSrvSupport::DATA_ROLE_HOST;
    auto ret = mockUsbImpl_->SetPortRole(portId, powerRole, dataRole);
    EXPECT_EQ(0, ret);

    sem_wait(&UsbEventMockTest::testSem_);
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
    auto &want = UsbSubscriberMockTest::eventData_.GetWant();
    EXPECT_EQ(want.GetAction(), CommonEventSupport::COMMON_EVENT_USB_PORT_CHANGED);
    std::string portStr = UsbSubscriberMockTest::eventData_.GetData();
    cJSON* portJson = cJSON_Parse(portStr.c_str());
    if (!portJson) {
        USB_HILOGI(MODULE_USB_SERVICE, "SUB_USB_Broadcast_0500 error, parse json string error");
    }
    EXPECT_TRUE(portJson);
    cJSON* jsonMode = cJSON_GetObjectItem(portJson, "mode");
    EXPECT_EQ(jsonMode->valueint, UsbSrvSupport::PORT_MODE_HOST);
}

/**
 * @tc.name: SUB_USB_Broadcast_0600
 * @tc.desc: usb port change to device event
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbEventMockTest, SUB_USB_Broadcast_0600, TestSize.Level1)
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USB_PORT_CHANGED);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<UsbSubscriberMockTest> subscriber = std::make_shared<UsbSubscriberMockTest>(subscriberInfo);
    CommonEventManager::SubscribeCommonEvent(subscriber);

    int32_t portId = DEFAULT_PORT_ID;
    int32_t powerRole = UsbSrvSupport::POWER_ROLE_SINK;
    int32_t dataRole = UsbSrvSupport::DATA_ROLE_DEVICE;
    auto ret = mockUsbImpl_->SetPortRole(portId, powerRole, dataRole);
    EXPECT_EQ(0, ret);

    sem_wait(&UsbEventMockTest::testSem_);
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
    auto &want = UsbSubscriberMockTest::eventData_.GetWant();
    EXPECT_EQ(want.GetAction(), CommonEventSupport::COMMON_EVENT_USB_PORT_CHANGED);
    std::string portStr = UsbSubscriberMockTest::eventData_.GetData();
    cJSON* portJson = cJSON_Parse(portStr.c_str());
    if (!portJson) {
        USB_HILOGI(MODULE_USB_SERVICE, "SUB_USB_Broadcast_0600 error, parse json string error");
    }
    EXPECT_TRUE(portJson);
    cJSON* jsonMode = cJSON_GetObjectItem(portJson, "mode");
    EXPECT_EQ(jsonMode->valueint, UsbSrvSupport::PORT_MODE_DEVICE);
}
} // namespace USB
} // namespace OHOS
