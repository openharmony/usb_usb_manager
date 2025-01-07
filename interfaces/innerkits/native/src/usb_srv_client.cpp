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

#include "usb_srv_client.h"
#include "datetime_ex.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "usb_common.h"
#include "usb_device.h"
#include "usb_errors.h"
#include "timer.h"
#include "v1_2/iusb_interface.h"

using namespace OHOS::HDI::Usb::V1_2;
namespace OHOS {
namespace USB {
constexpr uint32_t WAIT_SERVICE_LOAD = 500;
constexpr int32_t READ_BUF_SIZE = 8192;
UsbSrvClient::UsbSrvClient()
{
    callBackService = new UsbdCallBackServer();
    Connect();
}
UsbSrvClient::~UsbSrvClient() {}

UsbSrvClient& UsbSrvClient::GetInstance()
{
    static UsbSrvClient instance;
    return instance;
}

int32_t UsbSrvClient::Connect()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (proxy_ != nullptr) {
        return UEC_OK;
    }
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        USB_HILOGE(MODULE_USB_INNERKIT, "fail to get SystemAbilityManager");
        return UEC_INTERFACE_GET_SYSTEM_ABILITY_MANAGER_FAILED;
    }
    sptr<IRemoteObject> remoteObject = sm->CheckSystemAbility(USB_SYSTEM_ABILITY_ID);
    if (remoteObject == nullptr) {
        USB_HILOGE(MODULE_USB_INNERKIT, "GetSystemAbility failed.");
        return UEC_INTERFACE_GET_USB_SERVICE_FAILED;
    }
    proxy_ = iface_cast<IUsbSrv>(remoteObject);
    USB_HILOGI(MODULE_USB_INNERKIT, "Connect UsbService ok.");
    sptr<IRemoteObject> deathObject = proxy_->AsObject();
    if (deathObject == nullptr) {
        USB_HILOGI(MODULE_USB_INNERKIT, "deathObject is null.");
        return UEC_INTERFACE_DEAD_OBJECT;
    }
    deathRecipient_  = new UsbSrvDeathRecipient();
    deathObject->AddDeathRecipient(deathRecipient_);
    return UEC_OK;
}

void UsbSrvClient::ResetProxy(const wptr<IRemoteObject> &remote)
{
    std::lock_guard<std::mutex> lock(mutex_);
    RETURN_IF(proxy_ == nullptr);
    auto serviceRemote = proxy_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        serviceRemote->RemoveDeathRecipient(deathRecipient_);

        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_SERVICE_LOAD));

        sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sm == nullptr) {
            USB_HILOGE(MODULE_USB_INNERKIT, "fail to get SystemAbilityManager");
            return;
        }
        sptr<IRemoteObject> remoteObject = sm->CheckSystemAbility(USB_SYSTEM_ABILITY_ID);
        if (remoteObject == nullptr) {
            USB_HILOGE(MODULE_USB_INNERKIT, "GetSystemAbility failed.");
            proxy_ = nullptr;
            return;
        }
        proxy_ = iface_cast<IUsbSrv>(remoteObject);
    }
}

void UsbSrvClient::UsbSrvDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    if (remote == nullptr) {
        USB_HILOGE(MODULE_USB_INNERKIT, "UsbSrvDeathRecipient::OnRemoteDied failed, remote is nullptr.");
        return;
    }
    UsbSrvClient::GetInstance().ResetProxy(remote);
    USB_HILOGI(MODULE_USB_INNERKIT, "UsbSrvDeathRecipient::Recv death notice.");
}

int32_t UsbSrvClient::OpenDevice(const UsbDevice &device, USBDevicePipe &pipe)
{
    USB_HILOGI(MODULE_USB_INNERKIT, "Calling OpenDevice Start!");
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->OpenDevice(device.GetBusNum(), device.GetDevAddr());
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "OpenDevice failed with ret = %{public}d !", ret);
        return ret;
    }

    pipe.SetBusNum(device.GetBusNum());
    pipe.SetDevAddr(device.GetDevAddr());
    return UEC_OK;
}

int32_t UsbSrvClient::ResetDevice(const UsbDevice &device, USBDevicePipe &pipe)
{
    USB_HILOGI(MODULE_USB_INNERKIT, "Calling ResetDevice Start!");
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->ResetDevice(device.GetBusNum(), device.GetDevAddr());
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "ResetDevice failed with ret = %{public}d !", ret);
        return ret;
    }

    pipe.SetBusNum(device.GetBusNum());
    pipe.SetDevAddr(device.GetDevAddr());
    return UEC_OK;
}

bool UsbSrvClient::HasRight(const std::string deviceName)
{
    USB_HILOGI(MODULE_USB_INNERKIT, "Calling HasRight Start!");
    RETURN_IF_WITH_RET(Connect() != UEC_OK, false);
    return proxy_->HasRight(deviceName);
}

int32_t UsbSrvClient::RequestRight(const std::string deviceName)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->RequestRight(deviceName);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "Calling RequestRight failed with ret = %{public}d !", ret);
    }
    return ret;
}

int32_t UsbSrvClient::RemoveRight(const std::string deviceName)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->RemoveRight(deviceName);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "Calling RemoveRight failed with ret = %{public}d !", ret);
    }
    return ret;
}

int32_t UsbSrvClient::GetDevices(std::vector<UsbDevice> &deviceList)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->GetDevices(deviceList);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "GetDevices failed ret = %{public}d!", ret);
        return ret;
    }
    USB_HILOGI(MODULE_USB_INNERKIT, "GetDevices deviceList size = %{public}zu!", deviceList.size());
    return ret;
}

int32_t UsbSrvClient::GetCurrentFunctions(int32_t &funcs)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->GetCurrentFunctions(funcs);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed ret = %{public}d!", ret);
        return ret;
    }
    USB_HILOGI(MODULE_USB_INNERKIT, " Calling GetCurrentFunctions Success!");
    USB_HILOGI(MODULE_USB_INNERKIT, "GetCurrentFunctions funcs = %{public}d!", funcs);
    return ret;
}

int32_t UsbSrvClient::SetCurrentFunctions(int32_t funcs)
{
    USB_HILOGI(MODULE_USB_INNERKIT, "SetCurrentFunctions funcs = %{public}d!", funcs);
    RETURN_IF_WITH_RET(Connect() != UEC_OK, false);
    int32_t ret = proxy_->SetCurrentFunctions(funcs);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed ret = %{public}d!", ret);
        return ret;
    }
    USB_HILOGI(MODULE_USB_INNERKIT, " Calling SetCurrentFunctions Success!");
    return ret;
}

int32_t UsbSrvClient::UsbFunctionsFromString(std::string_view funcs)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t result = proxy_->UsbFunctionsFromString(funcs);
    USB_HILOGI(MODULE_USB_INNERKIT, " Calling UsbFunctionsFromString Success!");
    return result;
}

std::string UsbSrvClient::UsbFunctionsToString(int32_t funcs)
{
    std::string result;
    RETURN_IF_WITH_RET(Connect() != UEC_OK, result);
    result = proxy_->UsbFunctionsToString(funcs);
    USB_HILOGI(MODULE_USB_INNERKIT, " Calling UsbFunctionsToString Success!");
    return result;
}

int32_t UsbSrvClient::GetPorts(std::vector<UsbPort> &usbports)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    USB_HILOGI(MODULE_USB_INNERKIT, " Calling GetPorts");
    int32_t ret = proxy_->GetPorts(usbports);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed ret = %{public}d!", ret);
    }
    return ret;
}

int32_t UsbSrvClient::GetSupportedModes(int32_t portId, int32_t &result)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    USB_HILOGI(MODULE_USB_INNERKIT, " Calling GetSupportedModes");
    int32_t ret = proxy_->GetSupportedModes(portId, result);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed ret = %{public}d!", ret);
    }
    return ret;
}

int32_t UsbSrvClient::SetPortRole(int32_t portId, int32_t powerRole, int32_t dataRole)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    USB_HILOGI(MODULE_USB_INNERKIT, "Calling SetPortRole");
    int32_t ret = proxy_->SetPortRole(portId, powerRole, dataRole);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed ret = %{public}d!", ret);
    }
    return ret;
}

int32_t UsbSrvClient::ClaimInterface(USBDevicePipe &pipe, const UsbInterface &interface, bool force)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->ClaimInterface(pipe.GetBusNum(), pipe.GetDevAddr(), interface.GetId(), force);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed width ret = %{public}d !", ret);
    }
    return ret;
}

int32_t UsbSrvClient::UsbAttachKernelDriver(USBDevicePipe &pipe, const UsbInterface &interface)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->UsbAttachKernelDriver(pipe.GetBusNum(), pipe.GetDevAddr(), interface.GetId());
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "UsbAttachKernelDriver failed width ret = %{public}d !", ret);
    }
    return ret;
}

int32_t UsbSrvClient::UsbDetachKernelDriver(USBDevicePipe &pipe, const UsbInterface &interface)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->UsbDetachKernelDriver(pipe.GetBusNum(), pipe.GetDevAddr(), interface.GetId());
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "UsbDetachKernelDriver failed width ret = %{public}d !", ret);
    }
    return ret;
}

int32_t UsbSrvClient::ReleaseInterface(USBDevicePipe &pipe, const UsbInterface &interface)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->ReleaseInterface(pipe.GetBusNum(), pipe.GetDevAddr(), interface.GetId());
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed width ret = %{public}d !", ret);
    }
    return ret;
}

int32_t UsbSrvClient::BulkTransfer(
    USBDevicePipe &pipe, const USBEndpoint &endpoint, std::vector<uint8_t> &bufferData, int32_t timeOut)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    int32_t ret = UEC_INTERFACE_INVALID_VALUE;
    const UsbDev tdev = {pipe.GetBusNum(), pipe.GetDevAddr()};
    const UsbPipe tpipe = {endpoint.GetInterfaceId(), endpoint.GetAddress()};
    if (USB_ENDPOINT_DIR_IN == endpoint.GetDirection()) {
        int32_t length = bufferData.size() > 0 ? static_cast<int32_t>(bufferData.size()) : READ_BUF_SIZE;
        ret = proxy_->BulkTransferReadwithLength(tdev, tpipe, length, bufferData, timeOut);
    } else if (USB_ENDPOINT_DIR_OUT == endpoint.GetDirection()) {
        ret = proxy_->BulkTransferWrite(tdev, tpipe, bufferData, timeOut);
    }
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed width ret = %{public}d !", ret);
    }
    return ret;
}

int32_t UsbSrvClient::ControlTransfer(
    USBDevicePipe &pipe, const UsbCtrlTransfer &ctrl, std::vector<uint8_t> &bufferData)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    const UsbDev dev = {pipe.GetBusNum(), pipe.GetDevAddr()};
    int32_t ret = proxy_->ControlTransfer(dev, ctrl, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed width ret = %{public}d !", ret);
    }

    return ret;
}

int32_t UsbSrvClient::UsbControlTransfer(USBDevicePipe &pipe, const HDI::Usb::V1_2::UsbCtrlTransferParams &ctrlParams,
    std::vector<uint8_t> &bufferData)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    const UsbDev dev = {pipe.GetBusNum(), pipe.GetDevAddr()};
    int32_t ret = proxy_->UsbControlTransfer(dev, ctrlParams, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed width ret = %{public}d !", ret);
    }

    return ret;
}

int32_t UsbSrvClient::SetConfiguration(USBDevicePipe &pipe, const USBConfig &config)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->SetActiveConfig(pipe.GetBusNum(), pipe.GetDevAddr(), config.GetId());
    return ret;
}

int32_t UsbSrvClient::SetInterface(USBDevicePipe &pipe, const UsbInterface &interface)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    return proxy_->SetInterface(
        pipe.GetBusNum(), pipe.GetDevAddr(), interface.GetId(), interface.GetAlternateSetting());
}

int32_t UsbSrvClient::GetRawDescriptors(USBDevicePipe &pipe, std::vector<uint8_t> &bufferData)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->GetRawDescriptor(pipe.GetBusNum(), pipe.GetDevAddr(), bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed ret = %{public}d!", ret);
    }
    return ret;
}

int32_t UsbSrvClient::GetFileDescriptor(USBDevicePipe &pipe, int32_t &fd)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->GetFileDescriptor(pipe.GetBusNum(), pipe.GetDevAddr(), fd);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed ret = %{public}d!", ret);
    }
    return ret;
}

bool UsbSrvClient::Close(const USBDevicePipe &pipe)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, false);
    int32_t ret = proxy_->Close(pipe.GetBusNum(), pipe.GetDevAddr());
    return (ret == UEC_OK);
}

int32_t UsbSrvClient::PipeRequestWait(USBDevicePipe &pipe, int64_t timeOut, UsbRequest &req)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    std::vector<uint8_t> clientData;
    std::vector<uint8_t> bufferData;
    const UsbDev tdev = {pipe.GetBusNum(), pipe.GetDevAddr()};
    int32_t ret = proxy_->RequestWait(tdev, timeOut, clientData, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed width ret = %{public}d.", ret);
        return ret;
    }

    req.SetPipe(pipe);
    req.SetClientData(clientData);
    req.SetReqData(bufferData);
    return ret;
}

int32_t UsbSrvClient::RequestInitialize(UsbRequest &request)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    const USBDevicePipe &pipe = request.GetPipe();
    const USBEndpoint &endpoint = request.GetEndpoint();
    return proxy_->ClaimInterface(pipe.GetBusNum(), pipe.GetDevAddr(), endpoint.GetInterfaceId(), CLAIM_FORCE_1);
}

int32_t UsbSrvClient::RequestFree(UsbRequest &request)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    const USBDevicePipe &pipe = request.GetPipe();
    const USBEndpoint &ep = request.GetEndpoint();
    return proxy_->RequestCancel(pipe.GetBusNum(), pipe.GetDevAddr(), ep.GetInterfaceId(), ep.GetAddress());
}

int32_t UsbSrvClient::RequestAbort(UsbRequest &request)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    const USBDevicePipe &pipe = request.GetPipe();
    const USBEndpoint &ep = request.GetEndpoint();
    return proxy_->RequestCancel(pipe.GetBusNum(), pipe.GetDevAddr(), ep.GetInterfaceId(), ep.GetAddress());
}

int32_t UsbSrvClient::RequestQueue(UsbRequest &request)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    const USBDevicePipe &pipe = request.GetPipe();
    const USBEndpoint &ep = request.GetEndpoint();
    const UsbDev tdev = {pipe.GetBusNum(), pipe.GetDevAddr()};
    const UsbPipe tpipe = {ep.GetInterfaceId(), ep.GetAddress()};
    return proxy_->RequestQueue(tdev, tpipe, request.GetClientData(), request.GetReqData());
}

int32_t UsbSrvClient::UsbCancelTransfer(USBDevicePipe &pip, const int32_t &endpoint)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    const UsbDev tdev = {pip.GetBusNum(), pip.GetDevAddr()};
    int32_t ret = proxy_->UsbCancelTransfer(tdev, endpoint);
    if (ret!= UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "UsbCancelTransfer failed with ret = %{public}d!", ret);
    }
    return ret;
}

int32_t UsbSrvClient::UsbSubmitTransfer(USBDevicePipe &pip, HDI::Usb::V1_2::USBTransferInfo &info,
    const TransferCallback &cb, sptr<Ashmem> &ashmem)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    const UsbDev tdev = {pip.GetBusNum(), pip.GetDevAddr()};
    callBackService->SetTransferCallback(cb);
    int32_t ret = proxy_->UsbSubmitTransfer(tdev, info, callBackService, ashmem);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "UsbSubmitTransfer failed with ret = %{public}d", ret);
    }
    return ret;
}

int32_t UsbSrvClient::RegBulkCallback(USBDevicePipe &pip, const USBEndpoint &endpoint, const sptr<IRemoteObject> &cb)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    const UsbDev tdev = {pip.GetBusNum(), pip.GetDevAddr()};
    const UsbPipe tpipe = {endpoint.GetInterfaceId(), endpoint.GetAddress()};
    int32_t ret = proxy_->RegBulkCallback(tdev, tpipe, cb);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed width ret = %{public}d !", ret);
    }
    return ret;
}

int32_t UsbSrvClient::UnRegBulkCallback(USBDevicePipe &pip, const USBEndpoint &endpoint)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    const UsbDev tdev = {pip.GetBusNum(), pip.GetDevAddr()};
    const UsbPipe tpipe = {endpoint.GetInterfaceId(), endpoint.GetAddress()};
    int32_t ret = proxy_->UnRegBulkCallback(tdev, tpipe);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed width ret = %{public}d !", ret);
    }
    return ret;
}

int32_t UsbSrvClient::BulkRead(USBDevicePipe &pip, const USBEndpoint &endpoint, sptr<Ashmem> &ashmem)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    const UsbDev tdev = {pip.GetBusNum(), pip.GetDevAddr()};
    const UsbPipe tpipe = {endpoint.GetInterfaceId(), endpoint.GetAddress()};
    int32_t ret = proxy_->BulkRead(tdev, tpipe, ashmem);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed width ret = %{public}d !", ret);
    }
    return ret;
}

int32_t UsbSrvClient::BulkWrite(USBDevicePipe &pip, const USBEndpoint &endpoint, sptr<Ashmem> &ashmem)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    const UsbDev tdev = {pip.GetBusNum(), pip.GetDevAddr()};
    const UsbPipe tpipe = {endpoint.GetInterfaceId(), endpoint.GetAddress()};
    int32_t ret = proxy_->BulkWrite(tdev, tpipe, ashmem);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed width ret = %{public}d !", ret);
    }
    return ret;
}

int32_t UsbSrvClient::BulkCancel(USBDevicePipe &pip, const USBEndpoint &endpoint)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    const UsbDev tdev = {pip.GetBusNum(), pip.GetDevAddr()};
    const UsbPipe tpipe = {endpoint.GetInterfaceId(), endpoint.GetAddress()};
    int32_t ret = proxy_->BulkCancel(tdev, tpipe);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed width ret = %{public}d !", ret);
    }
    return ret;
}

int32_t UsbSrvClient::AddRight(const std::string &bundleName, const std::string &deviceName)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    USB_HILOGI(MODULE_USB_INNERKIT, "Calling AddRight");
    int32_t ret = proxy_->AddRight(bundleName, deviceName);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed ret = %{public}d!", ret);
    }
    return ret;
}

int32_t UsbSrvClient::AddAccessRight(const std::string &tokenId, const std::string &deviceName)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    USB_HILOGI(MODULE_USB_INNERKIT, "Calling AddAccessRight");
    int32_t ret = proxy_->AddAccessRight(tokenId, deviceName);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed ret = %{public}d!", ret);
    }
    return ret;
}

int32_t UsbSrvClient::ManageGlobalInterface(bool disable)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->ManageGlobalInterface(disable);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed width ret = %{public}d !", ret);
    }
    return ret;
}

int32_t UsbSrvClient::ManageDevice(int32_t vendorId, int32_t productId, bool disable)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->ManageDevice(vendorId, productId, disable);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed width ret = %{public}d !", ret);
    }
    return ret;
}

int32_t UsbSrvClient::ManageInterfaceType(const std::vector<UsbDeviceType> &disableType, bool disable)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->ManageInterfaceType(disableType, disable);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed width ret = %{public}d !", ret);
    }
    return ret;
}

int32_t UsbSrvClient::ClearHalt(USBDevicePipe &pipe, const USBEndpoint &ep)
{
    RETURN_IF_WITH_RET(proxy_ == nullptr, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->ClearHalt(pipe.GetBusNum(), pipe.GetDevAddr(), ep.GetInterfaceId(), ep.GetAddress());
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "ClearHalt failed ret = %{public}d !", ret);
    }
    return ret;
}

int32_t UsbSrvClient::GetDeviceSpeed(USBDevicePipe &pipe, uint8_t &speed)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->GetDeviceSpeed(pipe.GetBusNum(), pipe.GetDevAddr(), speed);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed ret = %{public}d!", ret);
    }
    USB_HILOGE(MODULE_USB_INNERKIT, "GetDeviceSpeed speed = %{public}u!", speed);
    return ret;
}

int32_t UsbSrvClient::GetInterfaceActiveStatus(USBDevicePipe &pipe, const UsbInterface &interface, bool &unactivated)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->GetInterfaceActiveStatus(pipe.GetBusNum(), pipe.GetDevAddr(), interface.GetId(), unactivated);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed ret = %{public}d!", ret);
    }
    return ret;
}

int32_t UsbSrvClient::GetAccessoryList(std::vector<USBAccessory> &accessList)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->GetAccessoryList(accessList);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "GetAccessoryList failed ret = %{public}d!", ret);
        return ret;
    }
    USB_HILOGI(MODULE_USB_INNERKIT, "GetAccessoryList accessList size = %{public}zu!", accessList.size());
    return ret;
}

int32_t UsbSrvClient::OpenAccessory(const USBAccessory &access, int32_t &fd)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->OpenAccessory(access, fd);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "OpenAccessory ret = %{public}d!", ret);
    }
    return ret;
}

int32_t UsbSrvClient::AddAccessoryRight(const uint32_t tokenId, const USBAccessory &access)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->AddAccessoryRight(tokenId, access);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "AddAccessoryRight ret = %{public}d!", ret);
    }
    return ret;
}

int32_t UsbSrvClient::HasAccessoryRight(const USBAccessory &access, bool &result)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    return proxy_->HasAccessoryRight(access, result);
}

int32_t UsbSrvClient::RequestAccessoryRight(const USBAccessory &access, bool &result)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->RequestAccessoryRight(access, result);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "RequestAccessoryRight ret = %{public}d!", ret);
    }
    return ret;
}

int32_t UsbSrvClient::CancelAccessoryRight(const USBAccessory &access)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->CancelAccessoryRight(access);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "CancelAccessoryRight ret = %{public}d!", ret);
    }
    return ret;
}

int32_t UsbSrvClient::CloseAccessory(const int32_t fd)
{
    RETURN_IF_WITH_RET(Connect() != UEC_OK, UEC_INTERFACE_NO_INIT);
    int32_t ret = proxy_->CloseAccessory(fd);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "CloseAccessory ret = %{public}d!", ret);
    }
    return ret;
}

} // namespace USB
} // namespace OHOS
