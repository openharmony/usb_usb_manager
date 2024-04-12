/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "cstdio"
#include "usb_common_test.h"
#include "usb_srv_client.h"
#include "cJSON.h"
#include "common_event_manager.h"
#include "common_event_support.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::USB;
using namespace OHOS::USB::Common;
using namespace OHOS::EventFwk;
static constexpr int32_t DEFAULT_PORT_ID = 1;
static constexpr int32_t DEFAULT_ROLE_HOST = 1;
static constexpr int32_t DEFAULT_ROLE_DEVICE = 2;
static constexpr int32_t MIN_ARG_NUM = 3;
static constexpr uint32_t CMD_INDEX = 1;
static constexpr uint32_t PARAM_INDEX = 2;

static constexpr int32_t HOST_MODE = 2;

static UsbSrvClient &g_usbClient = UsbSrvClient::GetInstance();

static void PrintHelp()
{
    printf("2 args\n");
    printf("-p 0: Query Port\n");
    printf("-p 1: Switch to host\n");
    printf("-p 2: Switch to device:\n");
    printf("-f 0: Query function\n");
    printf("-f 1: Switch to function:acm\n");
    printf("-f 2: Switch to function:ecm\n");
    printf("-f 3: Switch to function:acm&ecm\n");
    printf("-f 4: Switch to function:hdc\n");
    printf("-f 5: Switch to function:acm&hdc\n");
    printf("-f 6: Switch to function:ecm&hdc\n");
    printf("-f 8: Switch to function:mtp\n");
    printf("-f 16: Switch to function:ptp\n");
    printf("-f 32: Switch to function:rndis\n");
    printf("-f 512: Switch to function:storage\n");
    printf("-f 36: Switch to function:rndis&hdc\n");
    printf("-f 516: Switch to function:storage&hdc\n");
    printf("-c 1: Switch to recv braodcast\n");
    printf("-s 0: Get devicespeed\n");
    printf("-s 1: Get interface actived\n");
    printf("-r 0: Reset proxy\n");
}

class UsbSubscriberTest : public CommonEventSubscriber {
public:
    explicit UsbSubscriberTest(const CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp) {}

    void OnReceiveEvent(const CommonEventData &data) override
    {
        USB_HILOGI(MODULE_USB_SERVICE, "recv event ok");
        eventData_ = data;
        std::string deviceStr = eventData_.GetData();
        USB_HILOGI(MODULE_USB_SERVICE, "recv broadcast: %{public}s", deviceStr.c_str());

        cJSON* pDevice =  cJSON_Parse(deviceStr.c_str());
        UsbDevice device(pDevice);
        std::string strConfig = "null";
        if (device.GetConfigCount() > 0) {
            USBConfig config;
            device.GetConfig(0, config);
            strConfig = config.ToString();
        }
        USB_HILOGI(MODULE_USB_SERVICE, "recv broadcast:Name: %{public}s, config size: %{public}d, config0: %{public}s",
            device.GetName().c_str(), device.GetConfigCount(), strConfig.c_str());
    }

    static CommonEventData eventData_;
};

CommonEventData UsbSubscriberTest::eventData_ {};
std::shared_ptr<UsbSubscriberTest> subscriber = nullptr;
static void AddCommonEvent()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USB_DEVICE_DETACHED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USB_DEVICE_ATTACHED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USB_STATE);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    subscriber = std::make_shared<UsbSubscriberTest>(subscriberInfo);
    bool ret = CommonEventManager::SubscribeCommonEvent(subscriber);
    if (!ret) {
        USB_HILOGW(MODULE_USB_SERVICE, "subscriber event for failed: %{public}d", ret);
    }
}

static void StopSubscriberCommonEvent(int32_t signo)
{
    (void) signo;
    if (subscriber != nullptr) {
        CommonEventManager::UnSubscribeCommonEvent(subscriber);
    }
    std::cout << "stop recv broadcast."<< std::endl;
    USB_HILOGI(MODULE_USB_SERVICE, "stop recv broadcast.");
}

static void GetCurrentFunctionInfo()
{
    int32_t funcs = 0;
    string strFun = "";
    int32_t ret = g_usbClient.GetCurrentFunctions(funcs);
    if (ret) {
        printf("%s:%d error exit\n", __func__, __LINE__);
        return;
    }
    strFun = g_usbClient.UsbFunctionsToString(funcs);
    printf("%s:%d get current function: %s\n", __func__, __LINE__, strFun.c_str());
}

static void FunctionSwitch(UsbSrvClient &g_usbClient, int32_t mode)
{
    switch (mode) {
        case 0:
            GetCurrentFunctionInfo();
            break;
        default:
            int32_t ret = g_usbClient.SetCurrentFunctions(mode);
            if (ret) {
                printf("%s:%d error exit\n", __func__, __LINE__);
                break;
            }
            GetCurrentFunctionInfo();
            break;
    }
}

static void GetPortsInfo()
{
    std::vector<UsbPort> usbports;
    int32_t ret = g_usbClient.GetPorts(usbports);
    if (ret) {
        printf("%s:%d error exit\n", __func__, __LINE__);
        return;
    }

    if (usbports[0].usbPortStatus.currentMode == HOST_MODE) {
        printf("get current port %d: host\n", usbports[0].usbPortStatus.currentMode);
    } else {
        printf("get current port %d: device\n", usbports[0].usbPortStatus.currentMode);
    }
}

static void PortSwitch(UsbSrvClient &g_usbClient, int32_t mode)
{
    switch (mode) {
        case 0:
            GetPortsInfo();
            break;
        case DEFAULT_ROLE_HOST:
            g_usbClient.SetPortRole(DEFAULT_PORT_ID, DEFAULT_ROLE_HOST, DEFAULT_ROLE_HOST);
            GetPortsInfo();
            break;
        case DEFAULT_ROLE_DEVICE:
            g_usbClient.SetPortRole(DEFAULT_PORT_ID, DEFAULT_ROLE_DEVICE, DEFAULT_ROLE_DEVICE);
            GetPortsInfo();
            break;
        default:
            printf("%s:%d port param error\n", __func__, __LINE__);
            break;
    }
}

static void DeviceSpeed(UsbSrvClient &g_usbClient, int32_t &sp)
{
    vector<UsbDevice> devi;
    g_usbClient.GetDevices(devi);
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    g_usbClient.OpenDevice(device, pipe);
    uint8_t speed = -1;
    g_usbClient.GetDeviceSpeed(pipe, speed);
    sp = speed;
    return;
}

static void InterfaceStatus(UsbSrvClient &g_usbClient, int32_t &ds)
{
    vector<UsbDevice> devi;
    g_usbClient.GetDevices(devi);
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    g_usbClient.OpenDevice(device, pipe);
    UsbInterface interface = device.GetConfigs().front().GetInterfaces().at(0);
    bool unactivated = false;
    g_usbClient.GetInterfaceActiveStatus(pipe, interface, unactivated);
    unactivated ? ds = 1 : ds = 0;
    return;
}

static void ResetProxy(UsbSrvClient &g_usbClient, int32_t &sp)
{
    vector<UsbDevice> devi;
    g_usbClient.GetDevices(devi);
    USBDevicePipe pipe;
    UsbDevice device = devi.front();
    g_usbClient.OpenDevice(device, pipe);
    std::cout << "please kill service, press enter to continue" << std::endl;
    int32_t c;
    while (c != EOF) {
        if ((c = getchar()) == '\n') {
            break;
        }
    }
    uint8_t speed = -1;
    g_usbClient.GetDeviceSpeed(pipe, speed);
    sp = speed;
    return;
}

static void DeviceStatus(UsbSrvClient &g_usbClient, int32_t mode)
{
    switch (mode) {
        case 0:
            int32_t sp;
            DeviceSpeed(g_usbClient, sp);
            printf("%s:%d device speed=%d\n", __func__, __LINE__, sp);
            break;
        case 1:
            int32_t ds;
            InterfaceStatus(g_usbClient, ds);
            printf("%s:%d interface status=%d\n", __func__, __LINE__, ds);
            break;
        default:
            printf("%s:%d port param error\n", __func__, __LINE__);
            break;
    }
}

static void SetProxy(UsbSrvClient &g_usbClient, int32_t mode)
{
    switch (mode) {
        case 0:
            int32_t sp;
            ResetProxy(g_usbClient, sp);
            if (sp > 0) {
                printf("%s:%d ResetProxy Okay\n", __func__, __LINE__);
            } else {
                printf("%s:%d ResetProxy failed\n", __func__, __LINE__);
            }
            break;
        default:
            printf("%s:%d port param error\n", __func__, __LINE__);
            break;
    }
}

static inline bool isNumber(string_view strv)
{
    return (strv.find_first_not_of("0123456789") == strv.npos);
}

int32_t main(int32_t argc, char *argv[])
{
    UsbCommonTest::GrantPermissionSysNative();

    if (argc < MIN_ARG_NUM) {
        PrintHelp();
        return 0;
    }

    if (!isNumber(argv[PARAM_INDEX])) {
        PrintHelp();
        return 0;
    }

    uint32_t mode;
    if ((!strcmp(argv[CMD_INDEX], "-f"))) {
        mode = stoi(argv[PARAM_INDEX]);
        FunctionSwitch(g_usbClient, mode);
    } else if (!strcmp(argv[CMD_INDEX], "-p")) {
        mode = stoi(argv[PARAM_INDEX]);
        PortSwitch(g_usbClient, mode);
    } else if (!strcmp(argv[CMD_INDEX], "-s")) {
        mode = stoi(argv[PARAM_INDEX]);
        DeviceStatus(g_usbClient, mode);
    } else if (!strcmp(argv[CMD_INDEX], "-r")) {
        mode = stoi(argv[PARAM_INDEX]);
        SetProxy(g_usbClient, mode);
    } else if (!strcmp(argv[CMD_INDEX], "-c")) {
        AddCommonEvent();
        printf("Press input c to exit.\n");
        char ch = getchar();
        while (ch != 'c') {
            ch = getchar();
            if (ch == 'c') {
                StopSubscriberCommonEvent(0);
                break;
            }
            sleep(1);
        }
        printf("show boac exit.\n");
    } else {
        printf("param incorrect: please input -h for help\n");
    }
    return 0;
}
