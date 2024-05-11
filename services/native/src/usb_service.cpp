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

#include "usb_service.h"

#include <cstdio>
#include <iostream>
#include <ipc_skeleton.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <unordered_map>

#include "parameters.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "common_timer_errors.h"
#include "file_ex.h"
#include "if_system_ability_manager.h"
#include "iproxy_broker.h"
#include "iservice_registry.h"
#include "iusb_srv.h"
#include "securec.h"
#include "system_ability_definition.h"
#include "usb_common.h"
#include "usb_descriptor_parser.h"
#include "usb_errors.h"
#include "usb_port_manager.h"
#include "usb_right_manager.h"
#include "usbd_bulkcallback_impl.h"
#include "tokenid_kit.h"
#include "accesstoken_kit.h"
#include "usb_function_switch_window.h"
using OHOS::sptr;
using namespace OHOS::HDI::Usb::V1_1;

namespace OHOS {
namespace USB {
namespace {
constexpr const char *USB_SERVICE_NAME = "UsbService";
constexpr int32_t COMMEVENT_REGISTER_RETRY_TIMES = 10;
constexpr int32_t COMMEVENT_REGISTER_WAIT_DELAY_US = 20000;
constexpr uint32_t CURSOR_INIT = 18;
constexpr int32_t DESCRIPTOR_TYPE_STRING = 3;
constexpr int32_t DESCRIPTOR_VALUE_START_OFFSET = 2;
constexpr int32_t HALF = 2;
constexpr int32_t BIT_SHIFT_4 = 4;
constexpr int32_t BIT_HIGH_4 = 0xF0;
constexpr int32_t BIT_LOW_4 = 0x0F;
constexpr int32_t SERVICE_STARTUP_MAX_TIME = 30;
constexpr uint32_t UNLOAD_SA_TIMER_INTERVAL = 30 * 1000;
constexpr uint32_t MANAGE_INTERFACE_INTERVAL = 100;
constexpr uint32_t EDM_SA_MAX_TIME_OUT = 5000;
constexpr uint32_t EDM_SYSTEM_ABILITY_ID = 1601;
const std::u16string DESCRIPTOR = u"ohos.edm.IEnterpriseDeviceMgr";
constexpr uint32_t WITHOUT_USERID = 0;
constexpr uint32_t WITHOUT_ADMIN = 1;
constexpr uint32_t EMD_MASK_CODE = 20;
constexpr uint32_t DISABLE_USB = 1043;
constexpr uint32_t ALLOWED_USB_DEVICES = 1044;
constexpr uint32_t USB_STORAGE_DEVICE_ACCESS_POLICY = 1026;
constexpr int32_t WHITELIST_POLICY_MAX_DEVICES = 1000;
constexpr uint32_t EDM_SA_TIME_OUT_CODE = 9200007;
constexpr int32_t BASECLASS_INDEX = 0;
constexpr int32_t SUBCLASS_INDEX = 1;
constexpr int32_t PROTOCAL_INDEX = 2;
constexpr int32_t GET_EDM_STORAGE_DISABLE_TYPE = 2;
constexpr int32_t RANDOM_VALUE_INDICATE = -1;
constexpr int32_t USB_RIGHT_USERID_INVALID = -1;
constexpr const char *USB_DEFAULT_TOKEN = "UsbServiceTokenId";
std::unordered_map<InterfaceType, std::vector<int32_t>> g_typeMap  = {
    {InterfaceType::TYPE_STORAGE,   {8, -1, -1}},
    {InterfaceType::TYPE_AUDIO,     {1, -1, -1}},
    {InterfaceType::TYPE_HID,       {3, -1, -1}},
    {InterfaceType::TYPE_PHYSICAL,  {5, -1, -1}},
    {InterfaceType::TYPE_IMAGE,     {6, 1, 1}},
    {InterfaceType::TYPE_PRINTER,   {7, -1, -1}}
};

struct TupleCompare {
    bool operator()(const std::tuple<uint8_t, uint8_t, uint8_t>&lhs,
        const std::tuple<uint8_t, uint8_t, uint8_t>&rhs) const
    {
        return lhs < rhs;
    }
};
std::map<std::tuple<uint8_t, uint8_t, uint8_t>, bool, TupleCompare> claimed_interfaces;
} // namespace

auto g_serviceInstance = DelayedSpSingleton<UsbService>::GetInstance();
const bool G_REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSpSingleton<UsbService>::GetInstance().GetRefPtr());

UsbService::UsbService() : SystemAbility(USB_SYSTEM_ABILITY_ID, true)
{
    usbHostManager_ = std::make_shared<UsbHostManager>(nullptr);
    usbRightManager_ = std::make_shared<UsbRightManager>();
    usbPortManager_ = std::make_shared<UsbPortManager>();
    usbDeviceManager_ = std::make_shared<UsbDeviceManager>();
    usbd_ = OHOS::HDI::Usb::V1_1::IUsbInterface::Get();
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "IUsbInterface::Get inteface failed");
    }
}

UsbService::~UsbService() {}

int32_t UsbService::SetUsbd(const sptr<OHOS::HDI::Usb::V1_1::IUsbInterface> &usbd)
{
    if (usbd == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService usbd is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    usbd_ = usbd;

    if (usbPortManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbPortManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }
    usbPortManager_->SetUsbd(usbd);

    if (usbDeviceManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbDeviceManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }
    usbDeviceManager_->SetUsbd(usbd);
    return UEC_OK;
}

UsbService::SystemAbilityStatusChangeListener::SystemAbilityStatusChangeListener(
    sptr<UsbServiceSubscriber> usbdSubscriber)
    : usbdSubscriber_(usbdSubscriber)
{
}

void UsbService::SystemAbilityStatusChangeListener::OnAddSystemAbility(
    int32_t systemAbilityId, const std::string &deviceId)
{
    USB_HILOGI(MODULE_USB_SERVICE, "OnAddSystemAbility ID = %{public}d", systemAbilityId);
}

void UsbService::SystemAbilityStatusChangeListener::OnRemoveSystemAbility(
    int32_t systemAbilityId, const std::string &deviceId)
{
    USB_HILOGI(MODULE_USB_SERVICE, "OnRemoveSystemAbility ID = %{public}d", systemAbilityId);
    if (systemAbilityId == USB_SYSTEM_ABILITY_ID) {
        sptr<OHOS::HDI::Usb::V1_1::IUsbInterface> usbd_ = OHOS::HDI::Usb::V1_1::IUsbInterface::Get();
        if (usbd_ != nullptr) {
            usbd_->UnbindUsbdSubscriber(usbdSubscriber_);
        }
    }
}

void UsbService::OnStart()
{
    USB_HILOGI(MODULE_USB_SERVICE, "usb_service OnStart enter");
    if (ready_) {
        USB_HILOGE(MODULE_USB_SERVICE, "OnStart is ready, nothing to do");
        return;
    }

    if (!(Init())) {
        USB_HILOGE(MODULE_USB_SERVICE, "OnStart call init fail");
        return;
    }

    // wait for the usbd service to start and bind usb service and usbd service
    int32_t retryTimes = 0;
    while (retryTimes < SERVICE_STARTUP_MAX_TIME) {
        if (InitUsbd()) {
            break;
        }
        sleep(1);
        retryTimes++;

        if (retryTimes == SERVICE_STARTUP_MAX_TIME) {
            USB_HILOGE(MODULE_USB_SERVICE, "OnStart call initUsbd failed");
            return;
        }
    }

    if (usbPortManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbPortManager_");
        return;
    }

    usbPortManager_->Init();
    (void)usbDeviceManager_->Init();
    (void)InitUsbRight();
    ready_ = true;
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<ISystemAbilityStatusChange> status = new (std::nothrow) SystemAbilityStatusChangeListener(usbdSubscriber_);
    if (samgrProxy == nullptr || status == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "samgrProxy or SystemAbilityStatusChangeListener is nullptr");
        return;
    }
    int32_t ret = samgrProxy->SubscribeSystemAbility(USB_SYSTEM_ABILITY_ID, status);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "SubscribeSystemAbility failed. ret = %{public}d", ret);
        return;
    }
    USB_HILOGE(MODULE_USB_SERVICE, "OnStart and add system ability success");
}

bool UsbService::Init()
{
    USB_HILOGI(MODULE_USB_SERVICE, "usb_service Init enter");
    if (!eventRunner_) {
        eventRunner_ = AppExecFwk::EventRunner::Create(USB_SERVICE_NAME);
        if (eventRunner_ == nullptr) {
            USB_HILOGE(MODULE_USB_SERVICE, "Init failed due to create EventRunner");
            return false;
        }
    }
    if (handler_ == nullptr) {
        handler_ = std::make_shared<UsbServerEventHandler>(eventRunner_, g_serviceInstance);

        if (!Publish(g_serviceInstance)) {
            USB_HILOGE(MODULE_USB_SERVICE, "OnStart register to system ability manager failed.");
            return false;
        }
    }
    while (commEventRetryTimes_ <= COMMEVENT_REGISTER_RETRY_TIMES) {
        if (!IsCommonEventServiceAbilityExist()) {
            ++commEventRetryTimes_;
            usleep(COMMEVENT_REGISTER_WAIT_DELAY_US);
        } else {
            commEventRetryTimes_ = 0;
            break;
        }
    }
    USB_HILOGE(MODULE_USB_SERVICE, "Init success");
    return true;
}

bool UsbService::InitUsbd()
{
    usbd_ = OHOS::HDI::Usb::V1_1::IUsbInterface::Get();
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, " get usbd_ is nullptr");
        return false;
    }

    usbdSubscriber_ = new (std::nothrow) UsbServiceSubscriber();
    if (usbdSubscriber_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Init failed\n");
        return false;
    }
    recipient_ = new UsbdDeathRecipient();
    sptr<IRemoteObject> remote = OHOS::HDI::hdi_objcast<HDI::Usb::V1_0::IUsbInterface>(usbd_);
    if (!remote->AddDeathRecipient(recipient_)) {
        USB_HILOGE(MODULE_USB_SERVICE, "add DeathRecipient failed");
        return false;
    }

    ErrCode ret = usbd_->BindUsbdSubscriber(usbdSubscriber_);
    USB_HILOGI(MODULE_USB_SERVICE, "entry InitUsbd ret: %{public}d", ret);
    return SUCCEEDED(ret);
}

void UsbService::OnStop()
{
    USB_HILOGI(MODULE_USB_SERVICE, "entry stop service %{public}d", ready_);
    if (!ready_) {
        return;
    }
    eventRunner_.reset();
    handler_.reset();
    ready_ = false;

    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return;
    }
    sptr<IRemoteObject> remote = OHOS::HDI::hdi_objcast<HDI::Usb::V1_0::IUsbInterface>(usbd_);
    remote->RemoveDeathRecipient(recipient_);
    recipient_.clear();
    usbd_->UnbindUsbdSubscriber(usbdSubscriber_);
}

bool UsbService::IsCommonEventServiceAbilityExist()
{
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE,
            "IsCommonEventServiceAbilityExist Get ISystemAbilityManager "
            "failed, no SystemAbilityManager");
        return false;
    }
    sptr<IRemoteObject> remote = sm->CheckSystemAbility(COMMON_EVENT_SERVICE_ID);
    if (!remote) {
        USB_HILOGE(MODULE_USB_SERVICE, "No CesServiceAbility");
        return false;
    }
    return true;
}

int32_t UsbService::OpenDevice(uint8_t busNum, uint8_t devAddr)
{
    if (!UsbService::CheckDevicePermission(busNum, devAddr)) {
        return UEC_SERVICE_PERMISSION_DENIED;
    }

    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbd_->OpenDevice(dev);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "OpenDevice failed ret:%{public}d", ret);
    }

    return ret;
}

std::string UsbService::GetDeviceVidPidSerialNumber(std::string deviceName)
{
    std::string strDesc = "test";
    std::lock_guard<std::mutex> guard(mutex_);
    for (auto it = deviceVidPidMap_.begin(); it != deviceVidPidMap_.end(); ++it) {
        USB_HILOGI(MODULE_USB_SERVICE, " it->first = %{public}s", it->first.c_str());
        if (it->first == deviceName) {
            strDesc = it->second;
            break;
        }
    }
    return strDesc;
}

int32_t UsbService::GetDeviceVidPidSerialNumber(std::string deviceName, std::string& strDesc)
{
    int32_t isMatched = UEC_INTERFACE_INVALID_VALUE;
    std::lock_guard<std::mutex> guard(mutex_);
    for (auto it = deviceVidPidMap_.begin(); it != deviceVidPidMap_.end(); ++it) {
        USB_HILOGI(MODULE_USB_SERVICE, " it->first = %{public}s", it->first.c_str());
        if (it->first == deviceName) {
            strDesc = it->second;
            isMatched = UEC_OK;
            break;
        }
    }
    return isMatched;
}

bool UsbService::CheckDevicePermission(uint8_t busNum, uint8_t devAddr)
{
    std::string name = std::to_string(busNum) + "-" + std::to_string(devAddr);
    if (!UsbService::HasRight(name)) {
        USB_HILOGE(MODULE_USB_SERVICE, "No permission");
        return false;
    }
    return true;
}

bool UsbService::HasRight(std::string deviceName)
{
    USB_HILOGI(MODULE_USB_SERVICE, "calling usbRightManager HasRight");
    if (usbRightManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbRightManager_");
        return false;
    }

    std::string deviceVidPidSerialNum = "";
    if (GetDeviceVidPidSerialNumber(deviceName, deviceVidPidSerialNum) != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "can not find deviceName.");
        return false;
    }

    if (usbRightManager_->CheckPermission()) {
        USB_HILOGW(MODULE_USB_SERVICE, "system app, bypass: dev=%{public}s", deviceName.c_str());
        return true;
    }

    std::string bundleName;
    std::string tokenId;
    int32_t userId = USB_RIGHT_USERID_INVALID;
    if (!GetCallingInfo(bundleName, tokenId, userId)) {
        USB_HILOGE(MODULE_USB_SERVICE, "HasRight GetCallingInfo false");
        return false;
    }

    USB_HILOGI(MODULE_USB_SERVICE, "bundle=%{public}s, device=%{public}s", bundleName.c_str(), deviceName.c_str());
    if (usbRightManager_->HasRight(deviceVidPidSerialNum, bundleName, tokenId, userId)) {
        return true;
    }

    return usbRightManager_->HasRight(deviceVidPidSerialNum, bundleName, USB_DEFAULT_TOKEN, userId);
}

int32_t UsbService::RequestRight(std::string deviceName)
{
    USB_HILOGI(MODULE_USB_SERVICE, "calling usbRightManager RequestRight");
    if (usbRightManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbRightManager_");
        return UEC_SERVICE_INNER_ERR;
    }
    std::string deviceVidPidSerialNum = "";
    int32_t ret = GetDeviceVidPidSerialNumber(deviceName, deviceVidPidSerialNum);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "can not find deviceName.");
        return ret;
    }
    if (usbRightManager_->CheckPermission()) {
        USB_HILOGW(MODULE_USB_SERVICE, "system app, bypass: dev=%{public}s", deviceName.c_str());
        return UEC_OK;
    }
    std::string bundleName;
    std::string tokenId;
    int32_t userId = USB_RIGHT_USERID_INVALID;
    if (!GetCallingInfo(bundleName, tokenId, userId)) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetCallingInfo false");
        return false;
    }

    USB_HILOGI(MODULE_USB_SERVICE, "bundle=%{public}s, device=%{public}s", bundleName.c_str(), deviceName.c_str());
    return usbRightManager_->RequestRight(deviceName, deviceVidPidSerialNum, bundleName, tokenId, userId);
}

int32_t UsbService::RemoveRight(std::string deviceName)
{
    if (usbRightManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbRightManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }
    std::string deviceVidPidSerialNum = "";
    int32_t ret = GetDeviceVidPidSerialNumber(deviceName, deviceVidPidSerialNum);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "can not find deviceName.");
        return ret;
    }
    if (usbRightManager_->CheckPermission()) {
        USB_HILOGW(MODULE_USB_SERVICE, "system app, bypass: dev=%{public}s", deviceName.c_str());
        return UEC_OK;
    }
    std::string bundleName;
    std::string tokenId;
    int32_t userId = USB_RIGHT_USERID_INVALID;
    if (!GetCallingInfo(bundleName, tokenId, userId)) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetCallingInfo false");
        return false;
    }

    if (usbRightManager_->RemoveDeviceRight(deviceVidPidSerialNum, bundleName, tokenId, userId)) {
        USB_HILOGI(MODULE_USB_SERVICE, "RemoveDeviceRight done");
        return UEC_OK;
    }

    if (!usbRightManager_->RemoveDeviceRight(deviceVidPidSerialNum, bundleName, USB_DEFAULT_TOKEN, userId)) {
        USB_HILOGE(MODULE_USB_SERVICE, "RemoveDeviceRight failed");
        return UEC_SERVICE_INNER_ERR;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "RemoveRight done");
    return UEC_OK;
}

int32_t UsbService::GetDevices(std::vector<UsbDevice> &deviceList)
{
    std::map<std::string, UsbDevice *> devices;
    if (usbHostManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbHostManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }

    usbHostManager_->GetDevices(devices);
    USB_HILOGI(MODULE_USB_SERVICE, "list size %{public}zu", devices.size());
    for (auto it = devices.begin(); it != devices.end(); ++it) {
        if (!(usbRightManager_->CheckPermission())) {
            it->second->SetmSerial("");
        }
        deviceList.push_back(*it->second);
    }
    return UEC_OK;
}

int32_t UsbService::GetCurrentFunctions(int32_t &functions)
{
    std::lock_guard<std::mutex> guard(mutex_);
    if (usbRightManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbRightManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }
    if (!(usbRightManager_->CheckPermission())) {
        USB_HILOGW(MODULE_USB_SERVICE, "is not system app");
        return UEC_SERVICE_PERMISSION_DENIED_SYSAPI;
    }
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->GetCurrentFunctions(functions);
}

int32_t UsbService::SetCurrentFunctions(int32_t functions)
{
    std::lock_guard<std::mutex> guard(mutex_);
    USB_HILOGI(MODULE_USB_SERVICE, "func = %{public}d", functions);
    if (usbRightManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbRightManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }

    int32_t ret = usbRightManager_->HasSetFuncRight(functions);
    if (ret != 0) {
        USB_HILOGE(MODULE_USB_SERVICE, "HasSetFuncRight fail");
        return ret;
    }
    if (usbDeviceManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbDeviceManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }
    usbDeviceManager_->UpdateFunctions(functions);

    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    return usbd_->SetCurrentFunctions(functions);
}

int32_t UsbService::UsbFunctionsFromString(std::string_view funcs)
{
    if (usbRightManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbRightManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }
    if (!(usbRightManager_->CheckPermission())) {
        USB_HILOGW(MODULE_USB_SERVICE, "is not system app");
        return UEC_SERVICE_PERMISSION_DENIED_SYSAPI;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "calling UsbFunctionsFromString");
    return UsbDeviceManager::ConvertFromString(funcs);
}

std::string UsbService::UsbFunctionsToString(int32_t funcs)
{
    if (usbRightManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbRightManager_");
        return "";
    }
    if (!(usbRightManager_->CheckPermission())) {
        USB_HILOGW(MODULE_USB_SERVICE, "is not system app");
        return PERMISSION_DENIED_SYSAPI;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "calling UsbFunctionsToString");
    return UsbDeviceManager::ConvertToString(funcs);
}

int32_t UsbService::GetPorts(std::vector<UsbPort> &ports)
{
    USB_HILOGI(MODULE_USB_SERVICE, "calling usbPortManager getPorts");
    if (usbRightManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbRightManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }
    if (!(usbRightManager_->CheckPermission())) {
        USB_HILOGW(MODULE_USB_SERVICE, "is not system app");
        return UEC_SERVICE_PERMISSION_DENIED_SYSAPI;
    }
    if (usbPortManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbPortManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbPortManager_->GetPorts(ports);
}

int32_t UsbService::GetSupportedModes(int32_t portId, int32_t &supportedModes)
{
    USB_HILOGI(MODULE_USB_SERVICE, "calling usbPortManager getSupportedModes");
    if (usbRightManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbRightManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }
    if (!(usbRightManager_->CheckPermission())) {
        USB_HILOGW(MODULE_USB_SERVICE, "is not system app");
        return UEC_SERVICE_PERMISSION_DENIED_SYSAPI;
    }
    if (usbPortManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbPortManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbPortManager_->GetSupportedModes(portId, supportedModes);
}

int32_t UsbService::SetPortRole(int32_t portId, int32_t powerRole, int32_t dataRole)
{
    USB_HILOGI(MODULE_USB_SERVICE, "calling usbd getPorts");
    if (usbRightManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbRightManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }
    if (!(usbRightManager_->CheckPermission())) {
        USB_HILOGW(MODULE_USB_SERVICE, "is not system app");
        return UEC_SERVICE_PERMISSION_DENIED_SYSAPI;
    }
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    auto ret = usbd_->SetPortRole(portId, powerRole, dataRole);
    if (ret == HDF_ERR_NOT_SUPPORT) {
        USB_HILOGE(MODULE_USB_SERVICE, "SetPortRole not support");
        return UEC_SERVICE_NOT_SUPPORT_SWITCH_PORT;
    }
    return ret;
}

int32_t UsbService::ClaimInterface(uint8_t busNum, uint8_t devAddr, uint8_t interface, uint8_t force)
{
    if (!UsbService::CheckDevicePermission(busNum, devAddr)) {
        return UEC_SERVICE_PERMISSION_DENIED;
    }

    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    std::tuple<uint8_t, uint8_t, uint8_t>interface_tuple =
        std::make_tuple(busNum, devAddr, interface);
    if (claimed_interfaces.count(interface_tuple) > 0) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::interface already claimed");
        return UEC_OK;
    }

    int32_t ret = usbd_->ClaimInterface(dev, interface, force);
    if (ret == UEC_OK) {
        claimed_interfaces[interface_tuple] = true;
    }
    return ret;
}

int32_t UsbService::ReleaseInterface(uint8_t busNum, uint8_t devAddr, uint8_t interface)
{
    if (!UsbService::CheckDevicePermission(busNum, devAddr)) {
        return UEC_SERVICE_PERMISSION_DENIED;
    }

    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->ReleaseInterface(dev, interface);
}

int32_t UsbService::BulkTransferRead(
    const UsbDev &devInfo, const UsbPipe &pipe, std::vector<uint8_t> &bufferData, int32_t timeOut)
{
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbd_->BulkTransferRead(devInfo, pipe, timeOut, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BulkTransferRead error ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbService::BulkTransferWrite(
    const UsbDev &dev, const UsbPipe &pipe, const std::vector<uint8_t> &bufferData, int32_t timeOut)
{
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbd_->BulkTransferWrite(dev, pipe, timeOut, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BulkTransferWrite error ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbService::ControlTransfer(const UsbDev &dev, const UsbCtrlTransfer &ctrl, std::vector<uint8_t> &bufferData)
{
    std::lock_guard<std::mutex> guard(mutex_);
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    int32_t ret = UEC_SERVICE_INNER_ERR;
    if (((uint32_t)ctrl.requestType & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_DIR_OUT) {
        ret = usbd_->ControlTransferWrite(dev, ctrl, bufferData);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ControlTransferWrite error ret:%{public}d", ret);
        }
    } else {
        bufferData.clear();
        ret = usbd_->ControlTransferRead(dev, ctrl, bufferData);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ControlTransferRead error ret:%{public}d", ret);
        }
    }
    return ret;
}

int32_t UsbService::SetActiveConfig(uint8_t busNum, uint8_t devAddr, uint8_t configIndex)
{
    if (!UsbService::CheckDevicePermission(busNum, devAddr)) {
        return UEC_SERVICE_PERMISSION_DENIED;
    }

    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->SetConfig(dev, configIndex);
}

int32_t UsbService::GetActiveConfig(uint8_t busNum, uint8_t devAddr, uint8_t &configIndex)
{
    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    int32_t ret = usbd_->GetConfig(dev, configIndex);
    if (ret == UEC_OK) {
        claimed_interfaces.clear();
    }
    return ret;
}

int32_t UsbService::SetInterface(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid, uint8_t altIndex)
{
    if (!UsbService::CheckDevicePermission(busNum, devAddr)) {
        return UEC_SERVICE_PERMISSION_DENIED;
    }

    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->SetInterface(dev, interfaceid, altIndex);
}

int32_t UsbService::GetRawDescriptor(uint8_t busNum, uint8_t devAddr, std::vector<uint8_t> &bufferData)
{
    if (!UsbService::CheckDevicePermission(busNum, devAddr)) {
        return UEC_SERVICE_PERMISSION_DENIED;
    }

    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbd_->GetRawDescriptor(dev, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "error ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbService::GetFileDescriptor(uint8_t busNum, uint8_t devAddr, int32_t &fd)
{
    if (!UsbService::CheckDevicePermission(busNum, devAddr)) {
        return UEC_SERVICE_PERMISSION_DENIED;
    }

    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbd_->GetFileDescriptor(dev, fd);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "error ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbService::RequestQueue(const UsbDev &dev, const UsbPipe &pipe, const std::vector<uint8_t> &clientData,
    const std::vector<uint8_t> &bufferData)
{
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbd_->RequestQueue(dev, pipe, clientData, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "error ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbService::RequestWait(
    const UsbDev &dev, int32_t timeOut, std::vector<uint8_t> &clientData, std::vector<uint8_t> &bufferData)
{
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbd_->RequestWait(dev, clientData, bufferData, timeOut);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "error ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbService::RequestCancel(uint8_t busNum, uint8_t devAddr, uint8_t interfaceId, uint8_t endpointId)
{
    const UsbDev dev = {busNum, devAddr};
    const UsbPipe pipe = {interfaceId, endpointId};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->RequestCancel(dev, pipe);
}

int32_t UsbService::Close(uint8_t busNum, uint8_t devAddr)
{
    if (!UsbService::CheckDevicePermission(busNum, devAddr)) {
        return UEC_SERVICE_PERMISSION_DENIED;
    }

    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->CloseDevice(dev);
}

std::string UsbService::GetDevStringValFromIdx(uint8_t busNum, uint8_t devAddr, uint8_t idx)
{
    const UsbDev dev = {busNum, devAddr};
    std::vector<uint8_t> strV;
    std::string strDesc = " ";

    if (idx == 0) {
        return strDesc;
    }

    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return nullptr;
    }
    int32_t ret = usbd_->GetStringDescriptor(dev, idx, strV);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "get string[%{public}hhu] failed ret:%{public}d", idx, ret);
        return strDesc;
    }
    size_t length = strV.size();
    if ((length < DESCRIPTOR_VALUE_START_OFFSET) || (strV[1] != DESCRIPTOR_TYPE_STRING)) {
        USB_HILOGI(MODULE_USB_SERVICE, "type or length error, len:%{public}zu", length);
        return strDesc;
    }

    uint16_t *tbuf = new (std::nothrow) uint16_t[length + 1]();
    if (tbuf == nullptr) {
        USB_HILOGI(MODULE_USB_SERVICE, "new failed\n");
        return strDesc;
    }

    for (uint32_t i = 0; i < length - DESCRIPTOR_VALUE_START_OFFSET; ++i) {
        tbuf[i] = strV[i + DESCRIPTOR_VALUE_START_OFFSET];
    }
    size_t bufLen = (length - DESCRIPTOR_VALUE_START_OFFSET) / HALF;
    size_t wstrLen = wcslen((wchar_t*)tbuf) <= bufLen ? wcslen((wchar_t*)tbuf) : bufLen;
    std::wstring wstr(reinterpret_cast<wchar_t *>(tbuf), wstrLen);
    strDesc = std::string(wstr.begin(), wstr.end());
    USB_HILOGI(MODULE_USB_SERVICE, "getString idx:%{public}d length:%{public}zu, str: %{public}s",
        idx, strDesc.length(), strDesc.c_str());
    delete[] tbuf;
    return strDesc;
}

static std::string BcdToString(const std::vector<uint8_t> &bcd)
{
    std::string tstr;
    for (uint32_t i = 0; i < bcd.size(); ++i) {
        tstr += std::to_string((bcd[i] & BIT_HIGH_4) >> BIT_SHIFT_4);
        tstr += std::to_string((bcd[i] & BIT_LOW_4));
    }
    return tstr;
}

int32_t UsbService::FillDevStrings(UsbDevice &dev)
{
    uint8_t busNum;
    uint8_t devAddr;
    uint8_t offsetValue = 8;

    busNum = dev.GetBusNum();
    devAddr = dev.GetDevAddr();
    uint16_t bcdUsb = dev.GetbcdUSB();
    const std::vector<uint8_t> bcdData {(bcdUsb & 0xff), ((bcdUsb >> offsetValue) & 0xff)};
    dev.SetVersion(BcdToString(bcdData));
    dev.SetManufacturerName(GetDevStringValFromIdx(busNum, devAddr, dev.GetiManufacturer()));
    dev.SetProductName(GetDevStringValFromIdx(busNum, devAddr, dev.GetiProduct()));
    dev.SetmSerial(GetDevStringValFromIdx(busNum, devAddr, dev.GetiSerialNumber()));
    USB_HILOGI(MODULE_USB_SERVICE,
        "iSerial:%{public}d Manufactur:%{public}s product:%{public}s "
        "version:%{public}s",
        dev.GetiSerialNumber(), dev.GetManufacturerName().c_str(), dev.GetProductName().c_str(),
        dev.GetVersion().c_str());

    std::vector<USBConfig> configs;
    configs = dev.GetConfigs();
    for (auto it = configs.begin(); it != configs.end(); ++it) {
        it->SetName(GetDevStringValFromIdx(busNum, devAddr, it->GetiConfiguration()));
        USB_HILOGI(MODULE_USB_SERVICE, "Config:%{public}d %{public}s", it->GetiConfiguration(), it->GetName().c_str());
        std::vector<UsbInterface> interfaces = it->GetInterfaces();
        for (auto itIF = interfaces.begin(); itIF != interfaces.end(); ++itIF) {
            itIF->SetName(GetDevStringValFromIdx(busNum, devAddr, itIF->GetiInterface()));
            USB_HILOGI(MODULE_USB_SERVICE, "interface:%{public}hhu %{public}s", itIF->GetiInterface(),
                itIF->GetName().c_str());
        }
        it->SetInterfaces(interfaces);
    }
    dev.SetConfigs(configs);

    return UEC_OK;
}

int32_t UsbService::GetDeviceInfoDescriptor(const UsbDev &uDev, std::vector<uint8_t> &descriptor, UsbDevice &dev)
{
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbd_->GetRawDescriptor(uDev, descriptor);
    if (ret != UEC_OK) {
        usbd_->CloseDevice(uDev);
        USB_HILOGE(MODULE_USB_SERVICE, "GetRawDescriptor failed ret=%{public}d busNum:%{public}d devAddr:%{public}d",
            ret, uDev.busNum, uDev.devAddr);
        return ret;
    }
    uint8_t *buffer = descriptor.data();
    uint32_t length = descriptor.size();
    if ((!buffer) || (length == 0)) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetRawDescriptor failed len=%{public}d busNum:%{public}d devAddr:%{public}d",
            length, uDev.busNum, uDev.devAddr);
        return UEC_SERVICE_INVALID_VALUE;
    }
    dev.SetBusNum(uDev.busNum);
    dev.SetDevAddr(uDev.devAddr);
    dev.SetName(std::to_string(uDev.busNum) + "-" + std::to_string(uDev.devAddr));

    ret = UsbDescriptorParser::ParseDeviceDescriptor(buffer, length, dev);
    if (ret != UEC_OK) {
        usbd_->CloseDevice(uDev);
        USB_HILOGE(MODULE_USB_SERVICE, "ParseDeviceDescriptor failed ret=%{public}d", ret);
        return ret;
    }
    return ret;
}

int32_t UsbService::GetConfigDescriptor(UsbDevice &dev, std::vector<uint8_t> &descriptor)
{
    std::vector<USBConfig> configs;
    uint8_t *buffer = descriptor.data();
    uint32_t length = descriptor.size();
    uint32_t cursor = CURSOR_INIT;
    int32_t ret = UEC_OK;
    for (uint8_t i = 0; i < dev.GetDescConfigCount(); ++i) {
        if (length <= cursor) {
            USB_HILOGE(MODULE_USB_SERVICE, "GetConfigDescriptor[%{public}d] length=%{public}d", i, length);
            break;
        }
        USB_HILOGI(MODULE_USB_SERVICE, "GetConfigDescriptor length=%{public}d", length);
        uint32_t configCursor = 0;
        USBConfig config;
        ret = UsbDescriptorParser::ParseConfigDescriptor(buffer + cursor, length - cursor, configCursor, config);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ParseConfigDescriptor failed ret=%{public}d", ret);
            return ret;
        }
        cursor += configCursor;
        configs.push_back(config);
    }
    dev.SetConfigs(configs);
    ret = FillDevStrings(dev);
    USB_HILOGI(MODULE_USB_SERVICE, "FillDevStrings ret=%{public}d", ret);
    return ret;
}

int32_t UsbService::GetDeviceInfo(uint8_t busNum, uint8_t devAddr, UsbDevice &dev)
{
    USB_HILOGI(MODULE_USB_SERVICE, "busNum:%{public}d devAddr:%{public}d", busNum, devAddr);
    const UsbDev uDev = {busNum, devAddr};
    std::vector<uint8_t> descriptor;

    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbd_->OpenDevice(uDev);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "OpenDevice failed ret=%{public}d", ret);
        return ret;
    }

    ret = GetDeviceInfoDescriptor(uDev, descriptor, dev);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetDeviceInfoDescriptor ret=%{public}d", ret);
    }
    ret = GetConfigDescriptor(dev, descriptor);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetConfigDescriptor ret=%{public}d", ret);
        return ret;
    }

    usbd_->CloseDevice(uDev);
    USB_HILOGI(MODULE_USB_SERVICE, "CloseDevice=%{public}s", dev.ToString().c_str());

    return UEC_OK;
}

int32_t UsbService::GetEdmGlobalPolicy(sptr<IRemoteObject> remote, bool &IsGlobalDisabled)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(WITHOUT_USERID);
    data.WriteInt32(WITHOUT_ADMIN);

    uint32_t funcCode = (1 << EMD_MASK_CODE) | DISABLE_USB;
    int32_t ErrCode = remote->SendRequest(funcCode, data, reply, option);
    int32_t ret = ERR_INVALID_VALUE;
    bool isSuccess = reply.ReadInt32(ret) && (ret == UEC_OK);
    if (!isSuccess) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetGlobalPolicy failed. ErrCode =  %{public}d, ret = %{public}d",
            ErrCode, ret);
        return UEC_SERVICE_EDM_SEND_REQUEST_FAILED;
    }

    reply.ReadBool(IsGlobalDisabled);
    return UEC_OK;
}

int32_t UsbService::GetEdmTypePolicy(sptr<IRemoteObject> remote,
    std::unordered_map<InterfaceType, bool> &typeDisableMap)
{
    int32_t StorageDisableType = 0;
    bool IsStorageDisabled = false;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(WITHOUT_USERID);
    data.WriteInt32(WITHOUT_ADMIN);

    uint32_t funcCode = (1 << EMD_MASK_CODE) | USB_STORAGE_DEVICE_ACCESS_POLICY;
    int32_t ErrCode = remote->SendRequest(funcCode, data, reply, option);
    int32_t ret = ERR_INVALID_VALUE;
    bool isSuccess = reply.ReadInt32(ret) && (ret == ERR_OK);
    if (!isSuccess) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetEdmTypePolicy failed. ErrCode =  %{public}d, ret = %{public}d",
            ErrCode, ret);
        return UEC_SERVICE_EDM_SEND_REQUEST_FAILED;
    }

    reply.ReadInt32(StorageDisableType);
    if (StorageDisableType == GET_EDM_STORAGE_DISABLE_TYPE) {
        IsStorageDisabled = true;
    }
    typeDisableMap[InterfaceType::TYPE_STORAGE] = IsStorageDisabled;
    return UEC_OK;
}

int32_t UsbService::GetEdmWhiteListPolicy(sptr<IRemoteObject> remote, std::vector<UsbDeviceId> &trustUsbDeviceIds)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(WITHOUT_USERID);
    data.WriteInt32(WITHOUT_ADMIN);

    uint32_t funcCode = (1 << EMD_MASK_CODE) | ALLOWED_USB_DEVICES;
    int32_t ErrCode = remote->SendRequest(funcCode, data, reply, option);
    int32_t ret = ERR_INVALID_VALUE;
    bool IsSuccess = reply.ReadInt32(ret) && (ret == ERR_OK);
    if (!IsSuccess) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetEdmWhiteListPolicy failed. ErrCode =  %{public}d, ret = %{public}d",
            ErrCode, ret);
        return UEC_SERVICE_EDM_SEND_REQUEST_FAILED;
    }

    int32_t size = reply.ReadInt32();
    if (size > WHITELIST_POLICY_MAX_DEVICES) {
        USB_HILOGE(MODULE_USB_SERVICE, "EdmWhiteList size=[%{public}d] is too large", size);
        return UEC_SERVICE_EDM_DEVICE_SIZE_EXCEED;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "GetEdmWhiteListPolicy return size:%{public}d", size);
    for (int32_t i = 0; i < size; i++) {
        UsbDeviceId usbDeviceId;
        usbDeviceId.vendorId = reply.ReadInt32();
        usbDeviceId.productId = reply.ReadInt32();
        trustUsbDeviceIds.emplace_back(usbDeviceId);
    }
    return UEC_OK;
}

int32_t UsbService::GetEdmPolicy(bool &IsGlobalDisabled, std::unordered_map<InterfaceType, bool> &typeDisableMap,
    std::vector<UsbDeviceId> &trustUsbDeviceIds)
{
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "fail to get SystemAbilityManager");
        return UEC_SERVICE_GET_SYSTEM_ABILITY_MANAGER_FAILED;
    }
    sptr<IRemoteObject> remote = sm->CheckSystemAbility(EDM_SYSTEM_ABILITY_ID);
    if (remote == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "Get Edm SystemAbility failed.");
        return UEC_SERVICE_GET_EDM_SERVICE_FAILED;
    }

    int32_t ret = GetEdmGlobalPolicy(remote, IsGlobalDisabled);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetEdmGlobalPolicy failed.");
        return ret;
    }
    ret = GetEdmTypePolicy(remote, typeDisableMap);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetEdmTypePolicy failed.");
        return ret;
    }
    ret = GetEdmWhiteListPolicy(remote, trustUsbDeviceIds);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetEdmWhiteListPolicy failed.");
        return ret;
    }
    return UEC_OK;
}

int32_t UsbService::GetUsbPolicy(bool &IsGlobalDisabled, std::unordered_map<InterfaceType, bool> &typeDisableMap,
    std::vector<UsbDeviceId> &trustUsbDeviceIds)
{
    auto startTime = std::chrono::steady_clock::now();
    while (true) {
        int32_t ret = GetEdmPolicy(IsGlobalDisabled, typeDisableMap, trustUsbDeviceIds);
        if (ret == UEC_OK) {
            USB_HILOGI(MODULE_USB_SERVICE, "GetUsbPolicy succeed");
            break;
        } else if (ret == EDM_SA_TIME_OUT_CODE) {
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
            if (elapsedTime >= EDM_SA_MAX_TIME_OUT) {
                USB_HILOGE(MODULE_USB_SERVICE, "Time out, exit loop");
                return UEC_SERVICE_EDM_SA_TIME_OUT_FAILED;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(MANAGE_INTERFACE_INTERVAL));
        } else {
            USB_HILOGE(MODULE_USB_SERVICE, "EDM sa failed");
            return UEC_SERVICE_PREPARE_EDM_SA_FAILED;
        }
    }
    return UEC_OK;
}

int32_t UsbService::ExecuteManageDevicePolicy(std::vector<UsbDeviceId> &whiteList)
{
    std::map<std::string, UsbDevice *> devices;
    usbHostManager_->GetDevices(devices);
    int32_t ret = UEC_INTERFACE_NO_MEMORY;
    USB_HILOGI(MODULE_USB_SERVICE, "list size %{public}zu", devices.size());
    for (auto it = devices.begin(); it != devices.end(); ++it) {
        bool inWhiteList = false;
        for (auto dev : whiteList) {
            if (it->second->GetProductId() == dev.productId && it->second->GetVendorId() == dev.vendorId) {
                inWhiteList = true;
                break;
            }
        }
        if (inWhiteList) {
            ret = ManageDeviceImpl(it->second->GetVendorId(), it->second->GetProductId(), false);
        } else {
            ret = ManageDeviceImpl(it->second->GetVendorId(), it->second->GetProductId(), true);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(MANAGE_INTERFACE_INTERVAL));
    }
    if (ret != UEC_OK) {
        USB_HILOGI(MODULE_USB_SERVICE, "ManageDevice failed");
        return UEC_SERVICE_EXECUTE_POLICY_FAILED;
    }
    return UEC_OK;
}

bool UsbService::IsEdmEnabled()
{
    std::string edmParaValue = OHOS::system::GetParameter("persist.edm.edm_enable", "false");
    USB_HILOGI(MODULE_USB_SERVICE, "edmParaValue is %{public}s", edmParaValue.c_str());
    return edmParaValue == "true";
}

void UsbService::ExecuteStrategy(UsbDevice *devInfo)
{
    USB_HILOGI(MODULE_USB_SERVICE, "start");
    if (!IsEdmEnabled()) {
        USB_HILOGE(MODULE_USB_SERVICE, "edm is not activate, skip");
        return;
    }
    bool isGlobalDisabled = false;
    std::unordered_map<InterfaceType, bool> typeDisableMap{};
    std::vector<UsbDeviceId> trustUsbDeviceIds{};

    int32_t ret = GetUsbPolicy(isGlobalDisabled, typeDisableMap, trustUsbDeviceIds);
    if (ret == UEC_SERVICE_EDM_SA_TIME_OUT_FAILED || ret == UEC_SERVICE_PREPARE_EDM_SA_FAILED) {
        USB_HILOGE(MODULE_USB_SERVICE, "EDM sa time out or prepare failed, ret = %{public}d", ret);
        return;
    }

    if (isGlobalDisabled) {
        ret = ManageGlobalInterfaceImpl(isGlobalDisabled);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ManageGlobalInterface failed");
        }
        return;
    }
    bool flag = false;
    for (auto result : typeDisableMap) {
        flag = flag || result.second;
        if (result.second) {
            ret = ManageInterfaceTypeImpl(result.first, true);
        }
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ManageInterfaceType failed, type is %{public}d", (int32_t)result.first);
        }
    }
    if (flag) {
        USB_HILOGI(MODULE_USB_SERVICE, "Execute ManageInterfaceType finish");
        return;
    }
    if (trustUsbDeviceIds.empty()) {
        USB_HILOGI(MODULE_USB_SERVICE, "trustUsbDeviceIds is empty, no devices disable");
        return;
    }
    ret = ExecuteManageDevicePolicy(trustUsbDeviceIds);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "ExecuteManageDevicePolicy failed");
    }
    return;
}

bool UsbService::AddDevice(uint8_t busNum, uint8_t devAddr)
{
    UsbDevice *devInfo = new (std::nothrow) UsbDevice();
    if (devInfo == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "new failed");
        return false;
    }

    errno_t retSafe = memset_s(devInfo, sizeof(UsbDevice), 0, sizeof(UsbDevice));
    if (retSafe != EOK) {
        USB_HILOGI(MODULE_USB_SERVICE, "memset_s failed");
        return false;
    }

    int32_t ret = GetDeviceInfo(busNum, devAddr, *devInfo);
    USB_HILOGI(MODULE_USB_SERVICE, "GetDeviceInfo ret=%{public}d", ret);
    if (ret != UEC_OK) {
        delete devInfo;
        return false;
    }

    std::string name = std::to_string(busNum) + "-" + std::to_string(devAddr);
    std::string uniqueName = std::to_string(devInfo->GetVendorId()) + "-" + std::to_string(devInfo->GetProductId()) +
        "-" + devInfo->GetmSerial();
    USB_HILOGI(MODULE_USB_SERVICE, "map+: %{public}s", name.c_str());
    {
        std::lock_guard<std::mutex> guard(mutex_);
        deviceVidPidMap_.insert(std::pair<std::string, std::string>(name, uniqueName));
    }

    if (usbHostManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbHostManager_");
        return false;
    }

    usbHostManager_->AddDevice(devInfo);
    ExecuteStrategy(devInfo);
    return true;
}

bool UsbService::DelDevice(uint8_t busNum, uint8_t devAddr)
{
    USB_HILOGI(MODULE_USBD, "entry");
    int32_t ret = Close(busNum, devAddr);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "Close device failed width ret = %{public}d", ret);
    }

    if (usbHostManager_ == nullptr || usbRightManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbHostManager_ or usbRightManager_");
        return false;
    }

    std::string name = std::to_string(busNum) + "-" + std::to_string(devAddr);
    if (!usbRightManager_->RemoveDeviceAllRight(GetDeviceVidPidSerialNumber(name))) {
        USB_HILOGW(MODULE_USB_SERVICE, "remove right failed: %{public}s", name.c_str());
    }

    {
        std::lock_guard<std::mutex> guard(mutex_);
        for (auto it = deviceVidPidMap_.begin(); it != deviceVidPidMap_.end(); ++it) {
            if (it->first == name) {
                deviceVidPidMap_.erase(it);
                break;
            }
        }
    }

    return usbHostManager_->DelDevice(busNum, devAddr);
}

int32_t UsbService::InitUsbRight()
{
    int32_t ret = usbRightManager_->Init();
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "Init usb right manager failed: %{public}d", ret);
        return ret;
    }
    std::vector<std::string> devices;
    for (auto it = deviceVidPidMap_.begin(); it != deviceVidPidMap_.end(); ++it) {
        devices.push_back(it->second);
    }
    USB_HILOGI(MODULE_USBD, "clean: %{public}zu/%{public}zu", devices.size(), deviceVidPidMap_.size());
    ret = usbRightManager_->CleanUpRightExpired(devices);
    if (ret != USB_RIGHT_OK) {
        USB_HILOGE(MODULE_USBD, "clean expired usb right failed: %{public}d", ret);
    }
    return ret;
}

void UsbService::UpdateUsbPort(int32_t portId, int32_t powerRole, int32_t dataRole, int32_t mode)
{
    if (usbPortManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbPortManager_");
        return;
    }

    usbPortManager_->UpdatePort(portId, powerRole, dataRole, mode);
}

void UsbService::UpdateDeviceState(int32_t status)
{
    if (usbDeviceManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbDeviceManager_");
        return;
    }

    usbDeviceManager_->HandleEvent(status);
}

bool UsbService::GetBundleName(std::string &bundleName)
{
#ifdef USB_RIGHT_TEST
    bundleName = "com.usb.right";
    return true;
#endif
    pid_t uid = GetCallingUid();
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        return false;
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (remoteObject == nullptr) {
        return false;
    }

    sptr<AppExecFwk::IBundleMgr> bundleMgr(new AppExecFwk::BundleMgrProxy(remoteObject));
    if (bundleMgr == nullptr) {
        return false;
    }

    ErrCode ret = bundleMgr->GetNameForUid(uid, bundleName);
    if (ret != ERR_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "failed to obtain bundleName");
        return false;
    }
    return true;
}

bool UsbService::GetCallingInfo(std::string &bundleName, std::string &tokenId, int32_t &userId)
{
    OHOS::Security::AccessToken::AccessTokenID token = IPCSkeleton::GetCallingTokenID();
    OHOS::Security::AccessToken::HapTokenInfo hapTokenInfoRes;
    int32_t ret = OHOS::Security::AccessToken::AccessTokenKit::GetHapTokenInfo(token, hapTokenInfoRes);
    if (ret != ERR_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "failed, ret: %{public}d, app: %{public}s",
            ret, bundleName.c_str());
        return false;
    }
    bundleName = hapTokenInfoRes.bundleName;
    tokenId = std::to_string((uint32_t)token);
    userId = hapTokenInfoRes.userID;
    USB_HILOGD(MODULE_USB_SERVICE, "ret: %{public}d, app: %{public}s, user: %{public}d",
        ret, bundleName.c_str(), hapTokenInfoRes.userID);
    return true;
}

bool UsbService::GetBundleInfo(std::string &tokenId, int32_t &userId)
{
    OHOS::Security::AccessToken::AccessTokenID token = IPCSkeleton::GetCallingTokenID();
    OHOS::Security::AccessToken::HapTokenInfo hapTokenInfoRes;
    int32_t ret = OHOS::Security::AccessToken::AccessTokenKit::GetHapTokenInfo(token, hapTokenInfoRes);
    if (ret != ERR_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "failed, ret: %{public}d", ret);
        return false;
    }
    tokenId = USB_DEFAULT_TOKEN;
    userId = hapTokenInfoRes.userID;
    return true;
}

int32_t UsbService::RegBulkCallback(const UsbDev &devInfo, const UsbPipe &pipe, const sptr<IRemoteObject> &cb)
{
    if (cb == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "cb is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    if (hdiCb_ == nullptr) {
        hdiCb_ = new UsbdBulkCallbackImpl(cb);
    }
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    int32_t ret = usbd_->RegBulkCallback(devInfo, pipe, hdiCb_);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "RegBulkCallback error ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbService::UnRegBulkCallback(const UsbDev &devInfo, const UsbPipe &pipe)
{
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    hdiCb_ = nullptr;
    int32_t ret = usbd_->UnRegBulkCallback(devInfo, pipe);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "UnRegBulkCallback error ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbService::BulkRead(const UsbDev &devInfo, const UsbPipe &pipe, sptr<Ashmem> &ashmem)
{
    if (ashmem == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "BulkRead error ashmem");
        return UEC_SERVICE_INVALID_VALUE;
    }

    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbd_->BulkRead(devInfo, pipe, ashmem);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BulkRead error ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbService::BulkWrite(const UsbDev &devInfo, const UsbPipe &pipe, sptr<Ashmem> &ashmem)
{
    if (ashmem == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "BulkWrite error ashmem");
        return UEC_SERVICE_INVALID_VALUE;
    }

    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbd_->BulkWrite(devInfo, pipe, ashmem);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BulkWrite error ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbService::BulkCancel(const UsbDev &devInfo, const UsbPipe &pipe)
{
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbd_->BulkCancel(devInfo, pipe);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "BulkCancel error ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbService::AddRight(const std::string &bundleName, const std::string &deviceName)
{
    if (usbRightManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbRightManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }
    std::string deviceVidPidSerialNum = "";
    int32_t ret = GetDeviceVidPidSerialNumber(deviceName, deviceVidPidSerialNum);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "can not find deviceName.");
        return ret;
    }
    if (!(usbRightManager_->CheckPermission())) {
        USB_HILOGW(MODULE_USB_SERVICE, "is not system app");
        return UEC_SERVICE_PERMISSION_DENIED_SYSAPI;
    }
    std::string tokenId;
    int32_t userId = USB_RIGHT_USERID_INVALID;
    if (!GetBundleInfo(tokenId, userId)) {
        USB_HILOGE(MODULE_USB_SERVICE, "GetCallingInfo false");
        return false;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "AddRight bundleName = %{public}s, deviceName = %{public}s", bundleName.c_str(),
        deviceName.c_str());
    if (!usbRightManager_->AddDeviceRight(deviceVidPidSerialNum, bundleName, tokenId, userId)) {
        USB_HILOGE(MODULE_USB_SERVICE, "AddDeviceRight failed");
        return UEC_SERVICE_INNER_ERR;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "AddRight done");
    return UEC_OK;
}

int32_t UsbService::AddAccessRight(const std::string &tokenId, const std::string &deviceName)
{
    if (usbRightManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbRightManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }
    std::string deviceVidPidSerialNum = "";
    int32_t ret = GetDeviceVidPidSerialNumber(deviceName, deviceVidPidSerialNum);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "can not find deviceName.");
        return ret;
    }
    if (!(usbRightManager_->CheckPermission())) {
        USB_HILOGW(MODULE_USB_SERVICE, "is not system app");
        return UEC_SERVICE_PERMISSION_DENIED_SYSAPI;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "AddRight deviceName = %{public}s", deviceName.c_str());
    if (!usbRightManager_->AddDeviceRight(deviceVidPidSerialNum, tokenId)) {
        USB_HILOGE(MODULE_USB_SERVICE, "AddDeviceRight failed");
        return UEC_SERVICE_INNER_ERR;
    }
    USB_HILOGI(MODULE_USB_SERVICE, "AddRight done");
    return UEC_OK;
}

int UsbService::Dump(int fd, const std::vector<std::u16string> &args)
{
    if (fd < 0) {
        USB_HILOGE(MODULE_USB_SERVICE, "fd is invalid fd:%{public}d", fd);
        return UEC_SERVICE_INVALID_VALUE;
    }

    std::vector<std::string> argList;
    std::transform(args.begin(), args.end(), std::back_inserter(argList), [](const std::u16string &arg) {
        return Str16ToStr8(arg);
    });

    if (argList.empty()) {
        USB_HILOGE(MODULE_USB_SERVICE, "argList is empty");
        DumpHelp(fd);
        return UEC_SERVICE_INVALID_VALUE;
    }

    if (argList[0] == USB_HOST) {
        usbHostManager_->Dump(fd, argList[1]);
    } else if (argList[0] == USB_DEVICE) {
        usbDeviceManager_->Dump(fd, argList);
    } else if (argList[0] == USB_PORT) {
        usbPortManager_->Dump(fd, argList);
    } else if (argList[0] == USB_HELP) {
        DumpHelp(fd);
    } else {
        dprintf(fd, "Usb Dump service:invalid parameter.\n");
        DumpHelp(fd);
    }
    return UEC_OK;
}

void UsbService::DumpHelp(int32_t fd)
{
    dprintf(fd, "Refer to the following usage:\n");
    dprintf(fd, "-h: dump help\n");
    dprintf(fd, "============= dump the all device ==============\n");
    dprintf(fd, "usb_host -a: dump the all device list info\n");
    dprintf(fd, "------------------------------------------------\n");
    usbDeviceManager_->GetDumpHelp(fd);
    usbPortManager_->GetDumpHelp(fd);
}

void UsbService::UnLoadSelf(UnLoadSaType type)
{
    auto task = []() {
        auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgrProxy == nullptr) {
            USB_HILOGE(MODULE_USB_SERVICE, "get samgr failed");
            return;
        }

        auto ret = samgrProxy->UnloadSystemAbility(USB_SYSTEM_ABILITY_ID);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "unload failed");
        }
    };

    if (type == UNLOAD_SA_IMMEDIATELY) {
        task();
        return;
    }

    if (usbHostManager_ == nullptr || usbDeviceManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbHostManager_ or usbDeviceManager_");
        return;
    }

    unloadSelfTimer_.Unregister(unloadSelfTimerId_);
    unloadSelfTimer_.Shutdown();

    std::map<std::string, UsbDevice *> devices;
    usbHostManager_->GetDevices(devices);
    if (devices.size() != 0 || usbDeviceManager_->IsGadgetConnected()) { // delay unload conditions
        USB_HILOGW(MODULE_USB_SERVICE, "not need unload");
        return;
    }

    if (auto ret = unloadSelfTimer_.Setup(); ret != Utils::TIMER_ERR_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "set up timer failed %{public}u", ret);
        return;
    }
    unloadSelfTimerId_ = unloadSelfTimer_.Register(task, UNLOAD_SA_TIMER_INTERVAL, true);
}

void UsbService::UsbdDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "get samgr failed");
        return;
    }

    auto ret = samgrProxy->UnloadSystemAbility(USB_SYSTEM_ABILITY_ID);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "unload failed");
    }
}

int32_t UsbService::PreCallFunction()
{
    usbd_ = OHOS::HDI::Usb::V1_1::IUsbInterface::Get();
    if (usbRightManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbRightManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }
    if (!(usbRightManager_->CheckPermission())) {
        USB_HILOGW(MODULE_USB_SERVICE, "is not system app");
        return UEC_SERVICE_PERMISSION_DENIED_SYSAPI;
    }

    if (usbHostManager_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "invalid usbHostManager_");
        return UEC_SERVICE_INVALID_VALUE;
    }

    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return UEC_OK;
}

int32_t UsbService::ManageGlobalInterface(bool disable)
{
    if (PreCallFunction() != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "PreCallFunction failed");
        return UEC_SERVICE_PRE_MANAGE_INTERFACE_FAILED;
    }

    return ManageGlobalInterfaceImpl(disable);
}

int32_t UsbService::ManageDevice(int32_t vendorId, int32_t productId, bool disable)
{
    if (PreCallFunction() != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "PreCallFunction failed");
        return UEC_SERVICE_PRE_MANAGE_INTERFACE_FAILED;
    }

    return ManageDeviceImpl(vendorId, productId, disable);
}

int32_t UsbService::ManageInterfaceType(InterfaceType interfaceType, bool disable)
{
    if (PreCallFunction() != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "PreCallFunction failed");
        return UEC_SERVICE_PRE_MANAGE_INTERFACE_FAILED;
    }
    return ManageInterfaceTypeImpl(interfaceType, disable);
}

int32_t UsbService::ManageGlobalInterfaceImpl(bool disable)
{
    std::map<std::string, UsbDevice *> devices;
    usbHostManager_->GetDevices(devices);
    USB_HILOGI(MODULE_USB_SERVICE, "list size %{public}zu", devices.size());
    for (auto it = devices.begin(); it != devices.end(); ++it) {
        UsbDev dev = {it->second->GetBusNum(), it->second->GetDevAddr()};
        uint8_t configIndex = 0;
        if (usbd_->GetConfig(dev, configIndex)) {
            USB_HILOGW(MODULE_USB_SERVICE, "get device active config failed.");
            continue;
        }
        USBConfig configs;
        if (it->second->GetConfig(static_cast<uint8_t>(configIndex) - 1, configs)) {
            USB_HILOGW(MODULE_USB_SERVICE, "get device config info failed.");
            continue;
        }

        std::vector<UsbInterface> interfaces = configs.GetInterfaces();
        for (uint32_t i = 0; i < interfaces.size(); i++) {
            ManageInterface(dev, interfaces[i].GetId(), disable);
            std::this_thread::sleep_for(std::chrono::milliseconds(MANAGE_INTERFACE_INTERVAL));
        }
    }
    return UEC_OK;
}
int32_t UsbService::ManageDeviceImpl(int32_t vendorId, int32_t productId, bool disable)
{
    std::map<std::string, UsbDevice *> devices;
    usbHostManager_->GetDevices(devices);
    USB_HILOGI(MODULE_USB_SERVICE, "list size %{public}zu, vId: %{public}d, pId: %{public}d, b: %{public}d",
        devices.size(), vendorId, productId, disable);
    for (auto it = devices.begin(); it != devices.end(); ++it) {
        if (it->second->GetVendorId() == vendorId && it->second->GetProductId() == productId) {
            UsbDev dev = {it->second->GetBusNum(), it->second->GetDevAddr()};
            uint8_t configIndex = 0;
            if (usbd_->GetConfig(dev, configIndex)) {
                USB_HILOGW(MODULE_USB_SERVICE, "get device active config failed.");
                continue;
            }
            USBConfig configs;
            if (it->second->GetConfig(static_cast<uint8_t>(configIndex) - 1, configs)) {
                USB_HILOGW(MODULE_USB_SERVICE, "get device config info failed.");
                continue;
            }
            std::vector<UsbInterface> interfaces = configs.GetInterfaces();
            for (uint32_t i = 0; i < interfaces.size(); i++) {
                ManageInterface(dev, interfaces[i].GetId(), disable);
                std::this_thread::sleep_for(std::chrono::milliseconds(MANAGE_INTERFACE_INTERVAL));
            }
        }
    }
    return UEC_OK;
}
int32_t UsbService::ManageInterfaceTypeImpl(InterfaceType interfaceType, bool disable)
{
    auto iterInterface = g_typeMap .find(interfaceType);
    if (iterInterface == g_typeMap .end()) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::not find interface type");
        return UEC_SERVICE_INVALID_VALUE;
    }

    std::map<std::string, UsbDevice *> devices;
    usbHostManager_->GetDevices(devices);
    USB_HILOGI(MODULE_USB_SERVICE, "list size %{public}zu, interfaceType: %{public}d, disable: %{public}d",
        devices.size(), (int32_t)interfaceType, disable);
    for (auto it = devices.begin(); it != devices.end(); ++it) {
        UsbDev dev = {it->second->GetBusNum(), it->second->GetDevAddr()};
        uint8_t configIndex = 0;
        if (usbd_->GetConfig(dev, configIndex)) {
            USB_HILOGW(MODULE_USB_SERVICE, "get device active config failed.");
            continue;
        }
        USBConfig configs;
        if (it->second->GetConfig(static_cast<uint8_t>(configIndex) - 1, configs)) {
            USB_HILOGW(MODULE_USB_SERVICE, "get device config info failed.");
            continue;
        }
        std::vector<UsbInterface> interfaces = configs.GetInterfaces();

        for (uint32_t i = 0; i < interfaces.size(); i++) {
            // 0 indicate base class, 1 indicate subclass, 2 indicate protocal. -1 indicate any value.
            if (interfaces[i].GetClass() == iterInterface->second[BASECLASS_INDEX] && (interfaces[i].GetClass() ==
                iterInterface->second[SUBCLASS_INDEX] || iterInterface->second[SUBCLASS_INDEX] ==
                RANDOM_VALUE_INDICATE) && (interfaces[i].GetProtocol() == iterInterface->second[PROTOCAL_INDEX] ||
                iterInterface->second[PROTOCAL_INDEX] == RANDOM_VALUE_INDICATE)) {
                    ManageInterface(dev, interfaces[i].GetId(), disable);
                    std::this_thread::sleep_for(std::chrono::milliseconds(MANAGE_INTERFACE_INTERVAL));
            }
        }
    }
    return UEC_OK;
}

int32_t UsbService::ManageInterface(const HDI::Usb::V1_0::UsbDev &dev, uint8_t interfaceId, bool disable)
{
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    return usbd_->ManageInterface(dev, interfaceId, disable);
}

int32_t UsbService::GetInterfaceActiveStatus(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid, bool &unactivated)
{
    if (PreCallFunction() != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "PreCallFunction failed");
        return UEC_SERVICE_PRE_MANAGE_INTERFACE_FAILED;
    }

    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbd_->GetInterfaceActiveStatus(dev, interfaceid, unactivated);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "error ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbService::GetDeviceSpeed(uint8_t busNum, uint8_t devAddr, uint8_t &speed)
{
    if (PreCallFunction() != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "PreCallFunction failed");
        return UEC_SERVICE_PRE_MANAGE_INTERFACE_FAILED;
    }

    const UsbDev dev = {busNum, devAddr};
    if (usbd_ == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbService::usbd_ is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    int32_t ret = usbd_->GetDeviceSpeed(dev, speed);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "error ret:%{public}d", ret);
    }
    USB_HILOGE(MODULE_USB_SERVICE, "GetDeviceSpeedImpl:%{public}u", speed);
    return ret;
}

} // namespace USB
} // namespace OHOS
