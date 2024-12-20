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

#ifndef USBMGR_INNERKITS_IUSB_SRV_H
#define USBMGR_INNERKITS_IUSB_SRV_H

#include "iremote_broker.h"
#include "iremote_object.h"
#include "usb_device.h"
#include "usb_port.h"
#include "v1_1/usb_types.h"
#include "usb_interface_type.h"
#include "usb_accessory.h"

namespace OHOS {
namespace USB {
class IUsbSrv : public IRemoteBroker {
public:
    virtual int32_t OpenDevice(uint8_t busNum, uint8_t devAddr) = 0;
    virtual int32_t ResetDevice(uint8_t busNum, uint8_t devAddr) = 0;
    virtual bool HasRight(std::string deviceName) = 0;
    virtual int32_t RequestRight(std::string deviceName) = 0;
    virtual int32_t RemoveRight(std::string deviceName) = 0;
    virtual int32_t GetDevices(std::vector<UsbDevice> &deviceList) = 0;
    virtual int32_t GetCurrentFunctions(int32_t &funcs) = 0;
    virtual int32_t SetCurrentFunctions(int32_t funcs) = 0;
    virtual int32_t UsbFunctionsFromString(std::string_view funcs) = 0;
    virtual std::string UsbFunctionsToString(int32_t funcs) = 0;
    virtual int32_t GetPorts(std::vector<UsbPort> &ports) = 0;
    virtual int32_t GetSupportedModes(int32_t portId, int32_t &supportedModes) = 0;
    virtual int32_t SetPortRole(int32_t portId, int32_t powerRole, int32_t dataRole) = 0;
    virtual int32_t ReleaseInterface(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid) = 0;
    virtual int32_t ClaimInterface(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid, uint8_t force) = 0;
    virtual int32_t UsbAttachKernelDriver(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid) = 0;
    virtual int32_t UsbDetachKernelDriver(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid) = 0;
    virtual int32_t BulkTransferRead(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe,
        std::vector<uint8_t> &bufferData, int32_t timeOut) = 0;
    virtual int32_t BulkTransferWrite(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe,
        const std::vector<uint8_t> &bufferData, int32_t timeOut) = 0;
    virtual int32_t BulkTransferReadwithLength(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe,
        int32_t length, std::vector<uint8_t> &bufferData, int32_t timeOut) = 0;
    virtual int32_t ControlTransfer(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbCtrlTransfer &ctrl,
        std::vector<uint8_t> &bufferData) = 0;
    virtual int32_t UsbControlTransfer(const HDI::Usb::V1_0::UsbDev &dev,
        const HDI::Usb::V1_1::UsbCtrlTransferParams &ctrlParams, std::vector<uint8_t> &bufferData) = 0;
    virtual int32_t SetActiveConfig(uint8_t busNum, uint8_t devAddr, uint8_t configId) = 0;
    virtual int32_t GetActiveConfig(uint8_t busNum, uint8_t devAddr, uint8_t &configId) = 0;
    virtual int32_t SetInterface(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid, uint8_t altIndex) = 0;
    virtual int32_t GetRawDescriptor(uint8_t busNum, uint8_t devAddr, std::vector<uint8_t> &bufferData) = 0;
    virtual int32_t GetFileDescriptor(uint8_t busNum, uint8_t devAddr, int32_t &fd) = 0;
    virtual int32_t RequestQueue(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe,
        const std::vector<uint8_t> &clientData, const std::vector<uint8_t> &bufferData) = 0;
    virtual int32_t RequestWait(const HDI::Usb::V1_0::UsbDev &dev, int32_t timeOut, std::vector<uint8_t> &clientData,
        std::vector<uint8_t> &bufferData) = 0;
    virtual int32_t RequestCancel(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid, uint8_t endpointId) = 0;
    virtual int32_t Close(uint8_t busNum, uint8_t devAddr) = 0;

    virtual int32_t RegBulkCallback(const HDI::Usb::V1_0::UsbDev &devInfo, const HDI::Usb::V1_0::UsbPipe &pipe,
        const sptr<IRemoteObject> &cb) = 0;
    virtual int32_t UnRegBulkCallback(const HDI::Usb::V1_0::UsbDev &devInfo, const HDI::Usb::V1_0::UsbPipe &pipe) = 0;
    virtual int32_t BulkRead(const HDI::Usb::V1_0::UsbDev &devInfo, const HDI::Usb::V1_0::UsbPipe &pipe,
        sptr<Ashmem> &ashmem) = 0;
    virtual int32_t BulkWrite(const HDI::Usb::V1_0::UsbDev &devInfo, const HDI::Usb::V1_0::UsbPipe &pipe,
        sptr<Ashmem> &ashmem) = 0;
    virtual int32_t BulkCancel(const HDI::Usb::V1_0::UsbDev &devInfo, const HDI::Usb::V1_0::UsbPipe &pipe) = 0;
    virtual int32_t AddRight(const std::string &bundleName, const std::string &deviceName) = 0;
    virtual int32_t AddAccessRight(const std::string &tokenId, const std::string &deviceName) = 0;
    virtual int32_t ManageGlobalInterface(bool disable) = 0;
    virtual int32_t ManageDevice(int32_t vendorId, int32_t productId, bool disable) = 0;
    virtual int32_t ManageInterfaceType(const std::vector<UsbDeviceType> &disableType, bool disable) = 0;
    virtual int32_t ClearHalt(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid, uint8_t endpointId) = 0;
    virtual int32_t GetDeviceSpeed(uint8_t busNum, uint8_t devAddr, uint8_t &speed)  = 0;
    virtual int32_t GetInterfaceActiveStatus(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid,
        bool &unactivated) = 0;
    virtual int32_t AddAccessoryRight(const uint32_t tokenId, const USBAccessory &access) = 0;
    virtual int32_t HasAccessoryRight(const USBAccessory &access, bool &result) = 0;
    virtual int32_t RequestAccessoryRight(const USBAccessory &access, bool &result) = 0;
    virtual int32_t CancelAccessoryRight(const USBAccessory &access) = 0;
    virtual int32_t GetAccessoryList(std::vector<USBAccessory> &accessList) = 0;
    virtual int32_t OpenAccessory(const USBAccessory &access, int32_t &fd) = 0;
    virtual int32_t CloseAccessory(const int32_t fd) = 0;
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.usb.IUsbSrv");
};
} // namespace USB
} // namespace OHOS

#endif // USBMGR_INNERKITS_IUSB_SRV_H
