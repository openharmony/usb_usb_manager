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

#ifndef USBMGR_USB_SERVER_PROXY_H
#define USBMGR_USB_SERVER_PROXY_H

#include <map>
#include <string>
#include "iremote_proxy.h"
#include "iusb_srv.h"
#include "nocopyable.h"
#include "usb_device.h"
#include "usb_interface_type.h"
#include "usb_service_ipc_interface_code.h"

namespace OHOS {
namespace USB {
class UsbServerProxy : public IRemoteProxy<IUsbSrv> {
public:
    explicit UsbServerProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IUsbSrv>(impl) {}
    ~UsbServerProxy() = default;
    DISALLOW_COPY_AND_MOVE(UsbServerProxy);

    int32_t GetDevices(std::vector<UsbDevice> &deviceList) override;
    int32_t OpenDevice(uint8_t busNum, uint8_t devAddr) override;
    int32_t ResetDevice(uint8_t busNum, uint8_t devAddr) override;
    bool HasRight(const std::string deviceName) override;
    int32_t RequestRight(const std::string deviceName) override;
    int32_t RemoveRight(const std::string deviceName) override;
    int32_t GetCurrentFunctions(int32_t &funcs) override;
    int32_t SetCurrentFunctions(int32_t funcs) override;
    int32_t UsbFunctionsFromString(std::string_view funcs) override;
    std::string UsbFunctionsToString(int32_t funcs) override;
    int32_t GetPorts(std::vector<UsbPort> &ports) override;
    int32_t GetSupportedModes(int32_t portId, int32_t &supportedModes) override;
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

    int32_t RegBulkCallback(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe,
        const sptr<IRemoteObject> &cb) override;
    int32_t UnRegBulkCallback(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe) override;
    int32_t BulkRead(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe,
        sptr<Ashmem> &ashmem) override;
    int32_t BulkWrite(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe,
        sptr<Ashmem> &ashmem) override;
    int32_t BulkCancel(const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe) override;
    int32_t AddRight(const std::string &bundleName, const std::string &deviceName) override;
    int32_t AddAccessRight(const std::string &tokenId, const std::string &deviceName) override;
    int32_t ManageGlobalInterface(bool disable) override;
    int32_t ManageDevice(int32_t vendorId, int32_t productId, bool disable) override;
    int32_t ManageInterfaceType(const std::vector<UsbDeviceType> &disableType, bool disable) override;
    int32_t ClearHalt(uint8_t busNum, uint8_t devAddr, uint8_t interfaceId, uint8_t endpointId) override;
    int32_t GetDeviceSpeed(uint8_t busNum, uint8_t devAddr, uint8_t &speed) override;
    int32_t GetInterfaceActiveStatus(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid, bool &unactivated) override;
    int32_t AddAccessoryRight(const uint32_t tokenId, const USBAccessory &access) override;
    int32_t HasAccessoryRight(const USBAccessory &access, bool &result) override;
    int32_t RequestAccessoryRight(const USBAccessory &access, bool &result) override;
    int32_t CancelAccessoryRight(const USBAccessory &access) override;
    int32_t GetAccessoryList(std::vector<USBAccessory> &accessList) override;
    int32_t OpenAccessory(const USBAccessory &access, int32_t &fd) override;
    int32_t CloseAccessory(int32_t fd) override;

    int32_t SerialOpen(int32_t portId) override;
    int32_t SerialClose(int32_t portId) override;
    int32_t SerialRead(int32_t portId, std::vector<uint8_t>& data, uint32_t size) override;
    int32_t SerialWrite(int32_t portId, const std::vector<uint8_t>& data, uint32_t size) override;
    int32_t SerialGetAttribute(int32_t portId, OHOS::HDI::Usb::Serial::V1_0::SerialAttribute& attribute) override;
    int32_t SerialSetAttribute(int32_t portId, const OHOS::HDI::Usb::Serial::V1_0::SerialAttribute& attribute) override;
    int32_t SerialGetPortList(std::vector<OHOS::HDI::Usb::Serial::V1_0::SerialPort>& serialPortList) override;
    bool HasSerialRight(int32_t portId) override;
    int32_t AddSerialRight(uint32_t tokenId, int32_t portId) override;
    int32_t CancelSerialRight(int32_t portId) override;
    int32_t RequestSerialRight(int32_t portId) override;
    int32_t SerialReadData(MessageParcel &reply, std::vector<uint8_t> &data);
    void ParseSerialPort(MessageParcel &reply,
                            std::vector<OHOS::HDI::Usb::Serial::V1_0::SerialPort>& serialPorts);

private:
    static inline BrokerDelegator<UsbServerProxy> delegator_;
    int32_t ParseUsbPort(MessageParcel &reply, std::vector<UsbPort> &result);
    int32_t SetDeviceMessage(MessageParcel &data, uint8_t busNum, uint8_t devAddr);
    int32_t SetBufferMessage(MessageParcel &data, const std::vector<uint8_t> &bufferData);
    int32_t GetBufferMessage(MessageParcel &data, std::vector<uint8_t> &bufferData);
    int32_t GetDeviceListMessageParcel(MessageParcel &data, std::vector<UsbDevice> &deviceList);
    int32_t GetDeviceMessageParcel(MessageParcel &data, UsbDevice &devInfo);
    int32_t GetDeviceConfigsMessageParcel(MessageParcel &data, std::vector<USBConfig> &configs);
    int32_t GetDeviceInterfacesMessageParcel(MessageParcel &data, std::vector<UsbInterface> &interfaces);
    int32_t GetDeviceEndpointsMessageParcel(MessageParcel &data, std::vector<USBEndpoint> &eps);
    int32_t GetAccessoryListMessageParcel(MessageParcel &data, std::vector<USBAccessory> &accessoryList);
    int32_t GetAccessoryMessageParcel(MessageParcel &data, USBAccessory &accessoryInfo);
    int32_t SetAccessoryMessageParcel(const USBAccessory &accessoryInfo, MessageParcel &data);
    bool ReadFileDescriptor(MessageParcel &data, int &fd);
};
} // namespace USB
} // namespace OHOS

#endif // USBMGR_USB_SERVER_PROXY_H
