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

#include "usb_hub_device_test.h"

#include <sys/time.h>
#include <sys/ioctl.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

#include "delayed_sp_singleton.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "usb_common_test.h"
#include "usb_srv_client.h"
#include "usb_errors.h"
#include "cJSON.h"
#include "common_event_manager.h"
#include "common_event_support.h"
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::USB;
using namespace std;
using namespace OHOS::HDI::Usb::V1_0;
using namespace OHOS::HDI::Usb::V1_2;
using namespace OHOS::USB::Common;
using namespace OHOS::EventFwk;
using namespace testing::ext;
namespace OHOS {
namespace USB {
namespace HubDevice {
sem_t UsbHubDeviceTest::testSem_ {};
void UsbHubDeviceTest::SetUpTestCase(void)
{
    std::cout << "please connect hub device, press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
        ;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "Start UsbHubDeviceTest");
}

void UsbHubDeviceTest::TearDownTestCase(void)
{
    USB_HILOGI(MODULE_USB_SERVICE, "End UsbHubDeviceTest");
}

void UsbHubDeviceTest::SetUp(void)
{
sem_init(&UsbHubDeviceTest::testSem_, 1, 0);
}

void UsbHubDeviceTest::TearDown(void)
{
sem_destroy(&UsbHubDeviceTest::testSem_);
}

/**
 * @tc.name: getDevices001
 * @tc.desc: Test system sa to getDevices(std::vector<UsbDevice> &deviceList);
 * @tc.type: FUNC
 */
HWTEST_F(UsbHubDeviceTest, getDevices001, TestSize.Level1)
{
UsbCommonTest::GrantPermissionNormalNative();
std::this_thread::sleep_for(std::chrono::seconds(2));
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : getDevices001 : getDevices");
    vector<UsbDevice> devi;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(devi);
    ASSERT_EQ(ret, 0);
    USB_HILOGI(MODULE_USB_SERVICE, "UsbHubDeviceTest::getDevices001 %{public}d ret=%{public}d", __LINE__, ret);
    EXPECT_TRUE(!(devi.empty())) << "delist NULL";
    USB_HILOGI(MODULE_USB_SERVICE, "UsbHubDeviceTest::getDevices001 %{public}d size=%{public}zu", __LINE__,
            devi.size());
    bool hasHub = false;
    for (size_t i = 0; i < devi.size(); i++) {
        if (devi[i].GetClass() == 9) {
            hasHub = true;
            break;
        } else {
            USB_HILOGI(MODULE_USB_SERVICE, "UsbHubDeviceTest::getDevices001 class %{public}d ret=%{public}d", __LINE__, devi[i].GetClass());
        }
    }
    ASSERT_TRUE(hasHub);
    USB_HILOGI(MODULE_USB_SERVICE, "Case End : getDevices001 : getDevices");
}
 
class UsbSubscriberTest : public CommonEventSubscriber {
public:
    explicit UsbSubscriberTest(const CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp) {}

    void OnReceiveEvent(const CommonEventData &data) override
    {
        std::string deviceStr = data.GetData();
        eventDatas_.push_back(data);
        USB_HILOGI(MODULE_USB_SERVICE, "recv event ok datainfo: %{public}s", deviceStr.c_str());
        std::cout << deviceStr << std::endl;
        sem_post(&UsbHubDeviceTest::testSem_);
    }

    static std::vector<CommonEventData> eventDatas_;
};
std::vector<CommonEventData> UsbSubscriberTest::eventDatas_ {};

/**
 * @tc.name: SUB_USB_Broadcast_001
 * @tc.desc: usb device attached event
 * @tc.type: FUNC
 */
HWTEST_F(UsbHubDeviceTest, SUB_USB_Broadcast_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SUB_USB_Broadcast_001");
    UsbCommonTest::GrantPermissionSysNative();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USB_DEVICE_ATTACHED);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<UsbSubscriberTest> subscriber = std::make_shared<UsbSubscriberTest>(subscriberInfo);
    CommonEventManager::SubscribeCommonEvent(subscriber);

    std::cout << "please connect a hub to the host" << std::endl;

    // block until UsbSubscriberTest post
    sem_wait(&UsbHubDeviceTest::testSem_);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
    USB_HILOGI(MODULE_USB_SERVICE, "UnSubscribeCommonEvent");
    bool hasHub = false;
    for (size_t i = 0; i < UsbSubscriberTest::eventDatas_.size(); i++) {
        // parse string to Json::Value
        std::string deviceStr = UsbSubscriberTest::eventDatas_[i].GetData();
        cJSON* deviceJson = cJSON_Parse(deviceStr.c_str());
        if (!deviceJson) {
            USB_HILOGI(MODULE_USB_SERVICE, "SUB_USB_Broadcast_001 error, parse json string error");
        }
        UsbDevice device(deviceJson);
        if (device.GetClass() == 9) {
            hasHub = true;
        }
    }
    UsbSubscriberTest::eventDatas_.clear();
    EXPECT_TRUE(hasHub);
}

/**
 * @tc.name: SUB_USB_Broadcast_002
 * @tc.desc: usb device attached event
 * @tc.type: FUNC
 */
HWTEST_F(UsbHubDeviceTest, SUB_USB_Broadcast_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : SUB_USB_Broadcast_002");
    UsbCommonTest::GrantPermissionSysNative();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USB_DEVICE_DETACHED);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<UsbSubscriberTest> subscriber = std::make_shared<UsbSubscriberTest>(subscriberInfo);
    CommonEventManager::SubscribeCommonEvent(subscriber);

    std::cout << "please disconnect a hub to the host" << std::endl;

    // block until UsbSubscriberTest post
    sem_wait(&UsbHubDeviceTest::testSem_);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
    USB_HILOGI(MODULE_USB_SERVICE, "UnSubscribeCommonEvent");
    bool hasHub = false;
    for (size_t i = 0; i < UsbSubscriberTest::eventDatas_.size(); i++) {
        // parse string to Json::Value
        std::string deviceStr = UsbSubscriberTest::eventDatas_[i].GetData();
        cJSON* deviceJson = cJSON_Parse(deviceStr.c_str());
        if (!deviceJson) {
            USB_HILOGI(MODULE_USB_SERVICE, "SUB_USB_Broadcast_002 error, parse json string error");
        }
        UsbDevice device(deviceJson);
        if (device.GetClass() == 9) {
            hasHub = true;
        }
    }
    UsbSubscriberTest::eventDatas_.clear();
    EXPECT_TRUE(hasHub);
}

} // HubDevice
} // USB
} // OHOS
 