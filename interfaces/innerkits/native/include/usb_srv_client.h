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

#ifndef USBMGR_USB_SRV_CLIENT_H
#define USBMGR_USB_SRV_CLIENT_H

#include <map>
#include <memory>
#include <mutex>
#include <singleton.h>

#include "iremote_object.h"
#include "iusb_srv.h"
#include "usb_device.h"
#include "usb_device_pipe.h"
#include "usb_port.h"
#include "usb_request.h"
#include "usb_interface_type.h"

namespace OHOS {
namespace USB {
constexpr uint8_t CLAIM_FORCE_1 = 1;
const std::string MAXVERSION = "001";
const std::string SUBVERSION = "001";
const std::string DLPVERSION = "025";
const std::string SEVVERSION = MAXVERSION + "." + SUBVERSION + "." + DLPVERSION;

class UsbSrvClient final {
public:
    DISALLOW_COPY_AND_MOVE(UsbSrvClient);

    static UsbSrvClient& GetInstance();
    int32_t OpenDevice(const UsbDevice &device, USBDevicePipe &pipe);
    int32_t ResetDevice(const UsbDevice &device, USBDevicePipe &pipe);
    bool HasRight(std::string deviceName);
    int32_t RequestRight(std::string deviceName);
    int32_t RemoveRight(std::string deviceName);
    int32_t GetDevices(std::vector<UsbDevice> &deviceList);
    int32_t GetPorts(std::vector<UsbPort> &usbPorts);
    int32_t GetSupportedModes(int32_t portId, int32_t &supportedModes);
    int32_t SetPortRole(int32_t portId, int32_t powerRole, int32_t dataRole);
    int32_t GetCurrentFunctions(int32_t &funcs);
    int32_t SetCurrentFunctions(int32_t funcs);
    int32_t UsbFunctionsFromString(std::string_view funcs);
    std::string UsbFunctionsToString(int32_t funcs);
    int32_t ClaimInterface(USBDevicePipe &pip, const UsbInterface &interface, bool force);
    int32_t UsbAttachKernelDriver(USBDevicePipe &pip, const UsbInterface &interface);
    int32_t UsbDetachKernelDriver(USBDevicePipe &pip, const UsbInterface &interface);
    int32_t ReleaseInterface(USBDevicePipe &pip, const UsbInterface &interface);
    int32_t BulkTransfer(USBDevicePipe &pip, const USBEndpoint &endpoint, std::vector<uint8_t> &bufferData,
        int32_t timeOut);
    int32_t ControlTransfer(USBDevicePipe &pip, const HDI::Usb::V1_0::UsbCtrlTransfer &ctrl,
        std::vector<uint8_t> &bufferData);
    int32_t UsbControlTransfer(USBDevicePipe &pipe, const HDI::Usb::V1_1::UsbCtrlTransferParams &ctrlParams,
        std::vector<uint8_t> &bufferData);
    int32_t SetConfiguration(USBDevicePipe &pip, const USBConfig &config);
    int32_t SetInterface(USBDevicePipe &pipe, const UsbInterface &interface);
    int32_t GetRawDescriptors(USBDevicePipe &pipe, std::vector<uint8_t> &bufferData);
    int32_t GetFileDescriptor(USBDevicePipe &pipe, int32_t &fd);
    bool Close(const USBDevicePipe &pip);
    int32_t PipeRequestWait(USBDevicePipe &pip, int64_t timeOut, UsbRequest &req);

    int32_t RequestInitialize(UsbRequest &request);
    int32_t RequestFree(UsbRequest &request);
    int32_t RequestAbort(UsbRequest &request);
    int32_t RequestQueue(UsbRequest &request);
    int32_t GetDeviceSpeed(USBDevicePipe &pipe, uint8_t &speed);
    int32_t GetInterfaceActiveStatus(USBDevicePipe &pipe, const UsbInterface &interface, bool &unactivated);

    std::string GetVersion()
    {
        return SEVVERSION;
    }

    int32_t RegBulkCallback(USBDevicePipe &pip, const USBEndpoint &endpoint, const sptr<IRemoteObject> &cb);
    int32_t UnRegBulkCallback(USBDevicePipe &pip, const USBEndpoint &endpoint);
    int32_t BulkRead(USBDevicePipe &pip, const USBEndpoint &endpoint, sptr<Ashmem> &ashmem);
    int32_t BulkWrite(USBDevicePipe &pip, const USBEndpoint &endpoint, sptr<Ashmem> &ashmem);
    int32_t BulkCancel(USBDevicePipe &pip, const USBEndpoint &endpoint);
    int32_t AddRight(const std::string &bundleName, const std::string &deviceName);
    int32_t AddAccessRight(const std::string &tokenId, const std::string &deviceName);
    int32_t ManageGlobalInterface(bool disable);
    int32_t ManageDevice(int32_t vendorId, int32_t productId, bool disable);
    int32_t ManageInterfaceType(const std::vector<UsbDeviceType> &disableType, bool disable);
    int32_t ClearHalt(USBDevicePipe &pipe, const USBEndpoint &endpoint);
    int32_t AddAccessoryRight(const uint32_t tokenId, const USBAccessory &access);
    int32_t HasAccessoryRight(const USBAccessory &access, bool &result);
    int32_t RequestAccessoryRight(const USBAccessory &access, bool &result);
    int32_t CancelAccessoryRight(const USBAccessory &access);
    int32_t GetAccessoryList(std::vector<USBAccessory> &accessList);
    int32_t OpenAccessory(const USBAccessory &access, int32_t &fd);
    int32_t CloseAccessory(const int32_t fd);
private:
    UsbSrvClient();
    ~UsbSrvClient();

    class UsbSrvDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        UsbSrvDeathRecipient() = default;
        ~UsbSrvDeathRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override;

    private:
        DISALLOW_COPY_AND_MOVE(UsbSrvDeathRecipient);
    };

    int32_t Connect();
    void ResetProxy(const wptr<IRemoteObject> &remote);
    sptr<IUsbSrv> proxy_ = nullptr;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
    std::mutex mutex_;
};
} // namespace USB
} // namespace OHOS

#endif // USBMGR_USB_SRV_CLIENT_H
