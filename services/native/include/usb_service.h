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

#ifndef USBMGR_USB_SERVICE_H
#define USBMGR_USB_SERVICE_H

#include <map>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <chrono>
#include <thread>

#include "delayed_sp_singleton.h"
#include "iremote_object.h"
#include "iusb_srv.h"
#include "usb_interface_type.h"
#include "system_ability.h"
#include "system_ability_status_change_stub.h"
#include "timer.h"
#include "usb_device_manager.h"
#include "usb_accessory_manager.h"
#include "usb_host_manager.h"
#include "usb_port_manager.h"
#include "usb_right_manager.h"
#include "usb_server_stub.h"
#include "usb_service_subscriber.h"
#include "usbd_type.h"
#include "v1_1/iusb_interface.h"
#include "v1_0/iusbd_bulk_callback.h"
#include "v1_0/iusbd_subscriber.h"
#include "v1_1/usb_types.h"

namespace OHOS {
namespace USB {
const std::string USB_HOST = "usb_host";
const std::string USB_DEVICE = "usb_device";
const std::string USB_PORT = "usb_port";
const std::string USB_HELP = "-h";
class UsbService : public SystemAbility, public UsbServerStub {
    DECLARE_SYSTEM_ABILITY(UsbService)
    DECLARE_DELAYED_SP_SINGLETON(UsbService);

public:
    enum UnLoadSaType { UNLOAD_SA_DELAY, UNLOAD_SA_IMMEDIATELY };
    void OnStart() override;
    void OnStop() override;
    int Dump(int fd, const std::vector<std::u16string> &args) override;

    bool IsServiceReady() const
    {
        return ready_;
    }

    static sptr<UsbService> GetGlobalInstance();
    int32_t SetUsbd(const sptr<HDI::Usb::V1_1::IUsbInterface> &usbd);
    int32_t OpenDevice(uint8_t busNum, uint8_t devAddr) override;
    int32_t ResetDevice(uint8_t busNum, uint8_t devAddr) override;
    bool CheckDevicePermission(uint8_t busNum, uint8_t devAddr);
    bool HasRight(std::string deviceName) override;
    int32_t RequestRight(std::string deviceName) override;
    int32_t RemoveRight(std::string deviceName) override;
    int32_t GetDevices(std::vector<UsbDevice> &deviceList) override;
    int32_t GetCurrentFunctions(int32_t &funcs) override;
    int32_t SetCurrentFunctions(int32_t funcs) override;
    int32_t UsbFunctionsFromString(std::string_view funcs) override;
    std::string UsbFunctionsToString(int32_t funcs) override;
    int32_t GetPorts(std::vector<UsbPort> &ports) override;
    int32_t GetSupportedModes(int32_t portId, int32_t &result) override;
    int32_t SetPortRole(int32_t portId, int32_t powerRole, int32_t dataRole) override;

    int32_t ClaimInterface(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid, uint8_t force) override;
    int32_t UsbAttachKernelDriver(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid) override;
    int32_t UsbDetachKernelDriver(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid) override;
    int32_t ReleaseInterface(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid) override;
    int32_t BulkTransferRead(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe,
        std::vector<uint8_t> &bufferData, int32_t timeOut) override;
    int32_t BulkTransferReadwithLength(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe,
        int32_t length, std::vector<uint8_t> &bufferData, int32_t timeOut) override;
    int32_t BulkTransferWrite(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe,
        const std::vector<uint8_t> &bufferData, int32_t timeOut) override;
    int32_t ControlTransfer(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbCtrlTransfer &ctrl,
        std::vector<uint8_t> &bufferData) override;
    int32_t UsbControlTransfer(const HDI::Usb::V1_0::UsbDev &dev,
        const HDI::Usb::V1_1::UsbCtrlTransferParams &ctrlParams, std::vector<uint8_t> &bufferData) override;
    int32_t SetActiveConfig(uint8_t busNum, uint8_t devAddr, uint8_t configIndex) override;
    int32_t GetActiveConfig(uint8_t busNum, uint8_t devAddr, uint8_t &configIndex) override;
    int32_t SetInterface(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid, uint8_t altIndex) override;
    int32_t GetRawDescriptor(uint8_t busNum, uint8_t devAddr, std::vector<uint8_t> &bufferData) override;
    int32_t GetFileDescriptor(uint8_t busNum, uint8_t devAddr, int32_t &fd) override;
    int32_t RequestQueue(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe,
        const std::vector<uint8_t> &clientData, const std::vector<uint8_t> &bufferData) override;
    int32_t RequestWait(const HDI::Usb::V1_0::UsbDev &dev, int32_t timeOut, std::vector<uint8_t> &clientData,
        std::vector<uint8_t> &bufferData) override;
    int32_t RequestCancel(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid, uint8_t endpointId) override;
    int32_t Close(uint8_t busNum, uint8_t devAddr) override;
    bool AddDevice(uint8_t busNum, uint8_t devAddr);
    bool DelDevice(uint8_t busNum, uint8_t devAddr);
    void UpdateUsbPort(int32_t portId, int32_t powerRole, int32_t dataRole, int32_t mode);
    void UpdateDeviceState(int32_t status);
    int32_t GetDeviceInfo(uint8_t busNum, uint8_t devAddr, UsbDevice &dev);
    int32_t GetDeviceInfoDescriptor(
        const HDI::Usb::V1_0::UsbDev &uDev, std::vector<uint8_t> &descriptor, UsbDevice &dev);
    int32_t GetConfigDescriptor(UsbDevice &dev, std::vector<uint8_t> &descriptor);

    int32_t RegBulkCallback(const HDI::Usb::V1_0::UsbDev &devInfo, const HDI::Usb::V1_0::UsbPipe &pipe,
        const sptr<IRemoteObject> &cb) override;
    int32_t UnRegBulkCallback(const HDI::Usb::V1_0::UsbDev &devInfo, const HDI::Usb::V1_0::UsbPipe &pipe) override;
    int32_t BulkRead(
        const HDI::Usb::V1_0::UsbDev &devInfo, const HDI::Usb::V1_0::UsbPipe &pipe, sptr<Ashmem> &ashmem) override;
    int32_t BulkWrite(
        const HDI::Usb::V1_0::UsbDev &devInfo, const HDI::Usb::V1_0::UsbPipe &pipe, sptr<Ashmem> &ashmem) override;
    int32_t BulkCancel(const HDI::Usb::V1_0::UsbDev &devInfo, const HDI::Usb::V1_0::UsbPipe &pipe) override;
    int32_t AddRight(const std::string &bundleName, const std::string &deviceName) override;
    int32_t AddAccessRight(const std::string &tokenId, const std::string &deviceName) override;
    void UnLoadSelf(UnLoadSaType type);
    int32_t ManageGlobalInterface(bool disable) override;
    int32_t ManageDevice(int32_t vendorId, int32_t productId, bool disable) override;
    int32_t ManageInterfaceType(const std::vector<UsbDeviceType> &disableType, bool disable) override;
    int32_t GetInterfaceActiveStatus(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid, bool &unactivated) override;
    int32_t ClearHalt(uint8_t busNum, uint8_t devAddr, uint8_t interfaceId, uint8_t endpointId) override;
    int32_t GetDeviceSpeed(uint8_t busNum, uint8_t devAddr, uint8_t &speed) override;

    bool GetDeviceProductName(const std::string &deviceName, std::string &productName);
    int32_t UserChangeProcess();
    int32_t GetAccessoryList(std::vector<USBAccessory> &accessList) override;
    int32_t OpenAccessory(const USBAccessory &access, int32_t &fd) override;
    int32_t CloseAccessory(int32_t fd) override;
    int32_t AddAccessoryRight(const uint32_t tokenId, const USBAccessory &access) override;
    int32_t HasAccessoryRight(const USBAccessory &access, bool &result) override;
    int32_t RequestAccessoryRight(const USBAccessory &access, bool &result) override;
    int32_t CancelAccessoryRight(const USBAccessory &access) override;
private:
    class SystemAbilityStatusChangeListener : public SystemAbilityStatusChangeStub {
    public:
        explicit SystemAbilityStatusChangeListener(sptr<UsbServiceSubscriber> usbdSubscriber);
        ~SystemAbilityStatusChangeListener() = default;
        void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
        void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;

    private:
        sptr<UsbServiceSubscriber> usbdSubscriber_;
    };

    class UsbdDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;
    };

private:
    bool Init();
    bool InitUsbd();
    bool IsCommonEventServiceAbilityExist();
    bool GetBundleName(std::string &bundleName);
    bool GetCallingInfo(std::string &bundleName, std::string &tokenId, int32_t &userId);
    bool GetBundleInfo(std::string &tokenId, int32_t &userId);
    std::string GetDeviceVidPidSerialNumber(std::string deviceName);
    int32_t GetDeviceVidPidSerialNumber(std::string deviceName, std::string& strDesc);
    int32_t FillDevStrings(UsbDevice &dev);
    std::string GetDevStringValFromIdx(uint8_t busNum, uint8_t devAddr, uint8_t idx);
    int32_t InitUsbRight();
    void DumpHelp(int32_t fd);
    int32_t PreCallFunction();
    bool IsEdmEnabled();
    int32_t ExecuteManageDevicePolicy(std::vector<UsbDeviceId> &whiteList);
    int32_t ExecuteManageInterfaceType(const std::vector<UsbDeviceType> &disableType, bool disable);
    int32_t GetEdmPolicy(bool &IsGlobalDisabled, std::vector<UsbDeviceType> &disableType,
        std::vector<UsbDeviceId> &trustUsbDeviceId);
    int32_t GetUsbPolicy(bool &IsGlobalDisabled, std::vector<UsbDeviceType> &disableType,
        std::vector<UsbDeviceId> &trustUsbDeviceId);
    void ExecuteStrategy(UsbDevice *devInfo);
    int32_t GetEdmTypePolicy(sptr<IRemoteObject> remote, std::vector<UsbDeviceType> &disableType);
    int32_t GetEdmGlobalPolicy(sptr<IRemoteObject> remote, bool &IsGlobalDisabled);
    int32_t GetEdmStroageTypePolicy(sptr<IRemoteObject> remote, std::vector<UsbDeviceType> &disableType);
    int32_t GetEdmWhiteListPolicy(sptr<IRemoteObject> remote, std::vector<UsbDeviceId> &trustUsbDeviceId);
    int32_t ManageInterface(const HDI::Usb::V1_0::UsbDev &dev, uint8_t interfaceId, bool disable);
    void ExecuteManageDeviceType(const std::vector<UsbDeviceType> &disableType, bool disable,
        const std::unordered_map<InterfaceType, std::vector<int32_t>> &map, bool isDev);
    int32_t ManageGlobalInterfaceImpl(bool disable);
    int32_t ManageDeviceImpl(int32_t vendorId, int32_t productId, bool disable);
    int32_t ManageInterfaceTypeImpl(InterfaceType interfaceType, bool disable);
    int32_t ManageDeviceTypeImpl(InterfaceType interfaceType, bool disable);
    int32_t CheckUecValue();
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    bool ready_ = false;
    int32_t commEventRetryTimes_ = 0;
    std::mutex mutex_;
    std::mutex hdiCbMutex_;
    std::mutex functionMutex_;
    std::shared_ptr<UsbHostManager> usbHostManager_;
    std::shared_ptr<UsbRightManager> usbRightManager_;
    std::shared_ptr<UsbPortManager> usbPortManager_;
    std::shared_ptr<UsbDeviceManager> usbDeviceManager_;
    std::shared_ptr<UsbAccessoryManager> usbAccessoryManager_;
    sptr<UsbServiceSubscriber> usbdSubscriber_;
    sptr<HDI::Usb::V1_0::IUsbdBulkCallback> hdiCb_ = nullptr;
    sptr<HDI::Usb::V1_1::IUsbInterface> usbd_ = nullptr;
    std::map<std::string, std::string> deviceVidPidMap_;
    Utils::Timer unloadSelfTimer_ {"unLoadTimer"};
    uint32_t unloadSelfTimerId_ {UINT32_MAX};
    sptr<IRemoteObject::DeathRecipient> recipient_;
};
} // namespace USB
} // namespace OHOS
#endif // USBMGR_USB_SERVICE_H
