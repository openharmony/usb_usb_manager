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

#include "ipc_types.h"
#include "message_parcel.h"
#include "securec.h"
#include "string_ex.h"
#include "usb_common.h"
#include "usb_errors.h"
#include "usb_request.h"
#include "usb_server_proxy.h"
#include "v1_1/iusb_interface.h"
#include "usb_accessory.h"

using namespace OHOS::HDI::Usb::V1_1;
namespace OHOS {
namespace USB {

constexpr int32_t MAX_DEVICE_NUM = 127;
constexpr int32_t MAX_CONFIG_NUM  = 100;
constexpr int32_t MAX_INTERFACE_NUM = 100;
constexpr int32_t MAX_ENDPOINT_NUM = 32;
constexpr int32_t MAX_PORT_NUM = 100;

int32_t UsbServerProxy::SetDeviceMessage(MessageParcel &data, uint8_t busNum, uint8_t devAddr)
{
    WRITE_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerProxy::SetBufferMessage(MessageParcel &data, const std::vector<uint8_t> &bufferData)
{
    uint32_t length = bufferData.size();
    const uint8_t *ptr = bufferData.data();
    if (!ptr) {
        length = 0;
    }

    if (!data.WriteUint32(length)) {
        USB_HILOGE(MODULE_USBD, "write length failed:%{public}u", length);
        return UEC_SERVICE_WRITE_PARCEL_ERROR;
    }
    if ((ptr) && (length > 0) && !data.WriteBuffer(reinterpret_cast<const void *>(ptr), length)) {
        USB_HILOGE(MODULE_USBD, "write buffer failed length:%{public}u", length);
        return UEC_SERVICE_WRITE_PARCEL_ERROR;
    }

    USB_HILOGI(MODULE_USBD, "success length:%{public}u", length);
    return UEC_OK;
}

int32_t UsbServerProxy::GetBufferMessage(MessageParcel &data, std::vector<uint8_t> &bufferData)
{
    uint32_t dataSize = 0;
    bufferData.clear();
    if (!data.ReadUint32(dataSize)) {
        USB_HILOGE(MODULE_USBD, "read dataSize failed");
        return UEC_SERVICE_READ_PARCEL_ERROR;
    }
    if (dataSize == 0) {
        USB_HILOGI(MODULE_USBD, "invalid size:%{public}u", dataSize);
        return UEC_OK;
    }

    const uint8_t *readData = data.ReadUnpadBuffer(dataSize);
    if (readData == nullptr) {
        USB_HILOGE(MODULE_USBD, "failed size:%{public}u", dataSize);
        return UEC_SERVICE_READ_PARCEL_ERROR;
    }
    std::vector<uint8_t> tdata(readData, readData + dataSize);
    bufferData.swap(tdata);
    return UEC_OK;
}

int32_t UsbServerProxy::GetDevices(std::vector<UsbDevice> &deviceList)
{
    int32_t ret;
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        USB_HILOGE(MODULE_USB_INNERKIT, "remote is failed");
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_INVALID_VALUE;
    }

    ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_GET_DEVICES), data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed code: %{public}d", ret);
        return ret;
    }
    ret = GetDeviceListMessageParcel(reply, deviceList);
    return ret;
}

int32_t UsbServerProxy::GetDeviceListMessageParcel(MessageParcel &data, std::vector<UsbDevice> &deviceList)
{
    int32_t count;
    READ_PARCEL_WITH_RET(data, Int32, count, UEC_SERVICE_READ_PARCEL_ERROR);
    if (count > MAX_DEVICE_NUM) {
        USB_HILOGE(MODULE_USB_INNERKIT, "the maximum number of devices is exceeded!");
        return ERR_INVALID_VALUE;
    }

    for (int32_t i = 0; i < count; ++i) {
        UsbDevice devInfo;
        GetDeviceMessageParcel(data, devInfo);
        deviceList.push_back(devInfo);
    }
    return UEC_OK;
}

int32_t UsbServerProxy::GetDeviceMessageParcel(MessageParcel &data, UsbDevice &devInfo)
{
    int32_t tmp;
    uint8_t tui8;
    uint16_t tui16;
    READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetBusNum(tmp);
    READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetDevAddr(tmp);

    READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetVendorId(tmp);
    READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetProductId(tmp);
    READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetClass(tmp);
    READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetSubclass(tmp);
    READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetProtocol(tmp);
    READ_PARCEL_WITH_RET(data, Uint8, tui8, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetiManufacturer(tui8);
    READ_PARCEL_WITH_RET(data, Uint8, tui8, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetiProduct(tui8);
    READ_PARCEL_WITH_RET(data, Uint8, tui8, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetiSerialNumber(tui8);
    READ_PARCEL_WITH_RET(data, Uint8, tui8, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetbMaxPacketSize0(tui8);
    READ_PARCEL_WITH_RET(data, Uint16, tui16, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetbcdUSB(tui16);
    READ_PARCEL_WITH_RET(data, Uint16, tui16, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetbcdDevice(tui16);
    std::u16string tstr;
    READ_PARCEL_WITH_RET(data, String16, tstr, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetName(Str16ToStr8(tstr));
    READ_PARCEL_WITH_RET(data, String16, tstr, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetManufacturerName(Str16ToStr8(tstr));
    READ_PARCEL_WITH_RET(data, String16, tstr, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetProductName(Str16ToStr8(tstr));
    READ_PARCEL_WITH_RET(data, String16, tstr, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetVersion(Str16ToStr8(tstr));
    READ_PARCEL_WITH_RET(data, String16, tstr, UEC_SERVICE_READ_PARCEL_ERROR);
    devInfo.SetmSerial(Str16ToStr8(tstr));

    USB_HILOGI(MODULE_USB_INNERKIT, "devName:%{public}s Bus:%{public}d dev:%{public}d ", devInfo.GetName().c_str(),
        devInfo.GetBusNum(), devInfo.GetDevAddr());
    std::vector<USBConfig> configs;
    GetDeviceConfigsMessageParcel(data, configs);
    devInfo.SetConfigs(configs);
    return UEC_OK;
}

int32_t UsbServerProxy::GetAccessoryListMessageParcel(MessageParcel &data, std::vector<USBAccessory> &accessoryList)
{
    int32_t count;
    READ_PARCEL_WITH_RET(data, Int32, count, UEC_SERVICE_READ_PARCEL_ERROR);
    if (count > MAX_DEVICE_NUM || count < 0) {
        USB_HILOGE(MODULE_USB_INNERKIT, "the number of accessory is out of range!");
        return ERR_INVALID_VALUE;
    }

    for (int32_t i = 0; i < count; ++i) {
        USBAccessory accInfo;
        GetAccessoryMessageParcel(data, accInfo);
        accessoryList.push_back(accInfo);
    }
    return UEC_OK;
}

int32_t UsbServerProxy::GetAccessoryMessageParcel(MessageParcel &data, USBAccessory &accessoryInfo)
{
    std::string tmp;
    READ_PARCEL_WITH_RET(data, String, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
    accessoryInfo.SetManufacturer(tmp);

    READ_PARCEL_WITH_RET(data, String, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
    accessoryInfo.SetProduct(tmp);

    READ_PARCEL_WITH_RET(data, String, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
    accessoryInfo.SetDescription(tmp);

    READ_PARCEL_WITH_RET(data, String, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
    accessoryInfo.SetVersion(tmp);

    READ_PARCEL_WITH_RET(data, String, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
    accessoryInfo.SetSerialNumber(tmp);
    return UEC_OK;
}

int32_t UsbServerProxy::GetDeviceConfigsMessageParcel(MessageParcel &data, std::vector<USBConfig> &configs)
{
    uint32_t configCount;
    uint8_t tui8;
    std::u16string tstr;
    data.ReadUint32(configCount);

    int32_t tmp;
    uint32_t attributes;
    if (configCount > MAX_CONFIG_NUM) {
        USB_HILOGE(MODULE_USB_SERVICE, "the maximum number of configurations is exceeded!");
        return ERR_INVALID_VALUE;
    }
    for (uint32_t i = 0; i < configCount; ++i) {
        USBConfig config;
        READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
        config.SetId(tmp);
        READ_PARCEL_WITH_RET(data, Uint32, attributes, UEC_SERVICE_READ_PARCEL_ERROR);
        config.SetAttribute(attributes);
        READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
        config.SetMaxPower(tmp);

        READ_PARCEL_WITH_RET(data, Uint8, tui8, UEC_SERVICE_READ_PARCEL_ERROR);
        config.SetiConfiguration(tui8);
        READ_PARCEL_WITH_RET(data, String16, tstr, UEC_SERVICE_READ_PARCEL_ERROR);
        config.SetName(Str16ToStr8(tstr));

        std::vector<UsbInterface> interfaces;
        if (int32_t ret = GetDeviceInterfacesMessageParcel(data, interfaces); ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "GetDeviceInterfacesMessageParcel failed ret:%{public}d", ret);
            return ret;
        }

        config.SetInterfaces(interfaces);
        configs.push_back(config);
        USB_HILOGI(MODULE_USB_SERVICE, "devInfo=%{public}s", config.ToString().c_str());
    }

    return UEC_OK;
}

int32_t UsbServerProxy::GetDeviceInterfacesMessageParcel(MessageParcel &data, std::vector<UsbInterface> &interfaces)
{
    int32_t tmp;
    int32_t interfaceCount;
    uint8_t tui8;
    std::u16string tstr;
    data.ReadInt32(tmp);
    interfaceCount = tmp;
    if (interfaceCount > MAX_INTERFACE_NUM) {
        USB_HILOGE(MODULE_USB_SERVICE, "the maximum number of interfaces is exceeded!");
        return ERR_INVALID_VALUE;
    }
    for (int32_t i = 0; i < interfaceCount; ++i) {
        UsbInterface interface;
        READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
        interface.SetId(tmp);
        READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
        interface.SetClass(tmp);
        READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
        interface.SetSubClass(tmp);
        READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
        interface.SetAlternateSetting(tmp);
        READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
        interface.SetProtocol(tmp);

        READ_PARCEL_WITH_RET(data, Uint8, tui8, UEC_SERVICE_READ_PARCEL_ERROR);
        interface.SetiInterface(tui8);
        READ_PARCEL_WITH_RET(data, String16, tstr, UEC_SERVICE_READ_PARCEL_ERROR);
        interface.SetName(Str16ToStr8(tstr));

        std::vector<USBEndpoint> eps;
        if (int32_t ret = GetDeviceEndpointsMessageParcel(data, eps); ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "GetDeviceEndpointsMessageParcel failed ret:%{public}d", ret);
            return ret;
        }

        for (size_t j = 0; j < eps.size(); ++j) {
            eps[j].SetInterfaceId(interface.GetId());
        }
        interface.SetEndpoints(eps);
        interfaces.push_back(interface);
        USB_HILOGI(MODULE_USB_SERVICE, "devInfo=%{public}s", interface.ToString().c_str());
    }
    return UEC_OK;
}

int32_t UsbServerProxy::GetDeviceEndpointsMessageParcel(MessageParcel &data, std::vector<USBEndpoint> &eps)
{
    int32_t tmp;
    int32_t epCount;
    uint32_t attributes;
    uint32_t address;
    READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
    epCount = tmp;
    if (epCount > MAX_ENDPOINT_NUM) {
        USB_HILOGE(MODULE_USB_SERVICE, "the maximum number of endpoints is exceeded!");
        return ERR_INVALID_VALUE;
    }
    for (int32_t i = 0; i < epCount; ++i) {
        USBEndpoint ep;
        READ_PARCEL_WITH_RET(data, Uint32, address, UEC_SERVICE_READ_PARCEL_ERROR);
        ep.SetAddr(address);
        READ_PARCEL_WITH_RET(data, Uint32, attributes, UEC_SERVICE_READ_PARCEL_ERROR);
        ep.SetAttr(attributes);
        READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
        ep.SetInterval(tmp);
        READ_PARCEL_WITH_RET(data, Int32, tmp, UEC_SERVICE_READ_PARCEL_ERROR);
        ep.SetMaxPacketSize(tmp);
        eps.push_back(ep);
        USB_HILOGI(MODULE_USB_SERVICE, "devInfo=%{public}s", ep.ToString().c_str());
    }
    return UEC_OK;
}

int32_t UsbServerProxy::OpenDevice(uint8_t busNum, uint8_t devAddr)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }

    int32_t ret = SetDeviceMessage(data, busNum, devAddr);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SetDeviceMessage failed, ret:%{public}d", ret);
        return ret;
    }

    ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_OPEN_DEVICE), data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
    }
    return ret;
}

int32_t UsbServerProxy::ResetDevice(uint8_t busNum, uint8_t devAddr)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }

    int32_t ret = SetDeviceMessage(data, busNum, devAddr);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SetDeviceMessage failed, ret:%{public}d", ret);
        return ret;
    }

    ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_RESET_DEVICE), data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
    }
    return ret;
}

bool UsbServerProxy::HasRight(const std::string deviceName)
{
    MessageParcel data;
    MessageOption option;
    MessageParcel reply;
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, false);
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return false;
    }

    WRITE_PARCEL_WITH_RET(data, String16, Str8ToStr16(deviceName), false);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_HAS_RIGHT), data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return false;
    }

    bool result = false;
    READ_PARCEL_WITH_RET(reply, Bool, result, false);

    return result;
}

int32_t UsbServerProxy::RequestRight(const std::string deviceName)
{
    MessageParcel reply;
    MessageOption option;
    MessageParcel data;
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    WRITE_PARCEL_WITH_RET(data, String16, Str8ToStr16(deviceName), UEC_INTERFACE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_REQUEST_RIGHT),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::RemoveRight(const std::string deviceName)
{
    MessageParcel reply;
    MessageOption option;
    MessageParcel data;
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    WRITE_PARCEL_WITH_RET(data, String16, Str8ToStr16(deviceName), UEC_INTERFACE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_REMOVE_RIGHT),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::GetCurrentFunctions(int32_t &funcs)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_USB_SERVICE, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }

    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_GET_CURRENT_FUNCTIONS),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, funcs, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::SetCurrentFunctions(int32_t funcs)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);

    MessageOption option;
    MessageParcel data;
    MessageParcel reply;

    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_USB_SERVICE, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    WRITE_PARCEL_WITH_RET(data, Int32, funcs, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_SET_CURRENT_FUNCTIONS),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "SendRequest is failed, error code: %{public}d", ret);
    }
    return ret;
}

int32_t UsbServerProxy::UsbFunctionsFromString(std::string_view funcs)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;

    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_USB_SERVICE, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    WRITE_PARCEL_WITH_RET(data, String, std::string {funcs}, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_USB_FUNCTIONS_FROM_STRING),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "SendRequest is failed, error code: %{public}d", ret);
        return UEC_INTERFACE_INVALID_VALUE;
    }
    int32_t result = 0;
    READ_PARCEL_WITH_RET(reply, Int32, result, INVALID_USB_INT_VALUE);
    return result;
}

std::string UsbServerProxy::UsbFunctionsToString(int32_t funcs)
{
    sptr<IRemoteObject> remote = Remote();

    MessageParcel data;
    MessageOption option;
    MessageParcel reply;

    RETURN_IF_WITH_RET(remote == nullptr, INVALID_STRING_VALUE);

    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_USB_SERVICE, "write descriptor failed!");
        return INVALID_STRING_VALUE;
    }
    WRITE_PARCEL_WITH_RET(data, Int32, funcs, INVALID_STRING_VALUE);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_USB_FUNCTIONS_TO_STRING),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "SendRequest is failed, error code: %{public}d", ret);
        return INVALID_STRING_VALUE;
    }
    std::string result;
    READ_PARCEL_WITH_RET(reply, String, result, INVALID_STRING_VALUE);
    return result;
}

int32_t UsbServerProxy::GetPorts(std::vector<UsbPort> &ports)
{
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();

    MessageParcel data;
    MessageParcel reply;
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_GET_PORTS), data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    int32_t size;
    READ_PARCEL_WITH_RET(reply, Int32, size, UEC_INTERFACE_READ_PARCEL_ERROR);
    USB_HILOGI(MODULE_USB_INNERKIT, "GetPorts size %{public}d", size);
    if (size > MAX_PORT_NUM) {
        USB_HILOGE(MODULE_INNERKIT, "the maximum number of ports is exceeded!");
        return ERR_INVALID_VALUE;
    }
    for (int32_t i = 0; i < size; ++i) {
        USB_HILOGI(MODULE_USB_INNERKIT, "ParseUsbPort : %{public}d", i);
        ret = ParseUsbPort(reply, ports);
        if (ret) {
            return ret;
        }
    }
    return ret;
}

int32_t UsbServerProxy::ParseUsbPort(MessageParcel &reply, std::vector<UsbPort> &ports)
{
    UsbPort port;
    UsbPortStatus status;
    READ_PARCEL_WITH_RET(reply, Int32, port.id, UEC_INTERFACE_READ_PARCEL_ERROR);
    USB_HILOGI(MODULE_USB_INNERKIT, "UsbServerProxy::port->id %{public}d", port.id);
    port.supportedModes = reply.ReadInt32();
    status.currentMode = reply.ReadInt32();
    status.currentPowerRole = reply.ReadInt32();
    status.currentDataRole = reply.ReadInt32();
    port.usbPortStatus = status;
    USB_HILOGI(MODULE_USB_INNERKIT, "UsbServerProxy::port.usbPortStatus.currentMode %{public}d",
        port.usbPortStatus.currentMode);
    ports.push_back(port);
    return UEC_OK;
}

int32_t UsbServerProxy::GetSupportedModes(int32_t portId, int32_t &supportedModes)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    WRITE_PARCEL_WITH_RET(data, Int32, portId, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_GET_SUPPORTED_MODES),
        data, reply, option);
    if (ret) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, supportedModes, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::SetPortRole(int32_t portId, int32_t powerRole, int32_t dataRole)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    WRITE_PARCEL_WITH_RET(data, Int32, portId, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, powerRole, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, dataRole, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_SET_PORT_ROLE),
        data, reply, option);
    if (ret) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    return ret;
}

int32_t UsbServerProxy::ClaimInterface(uint8_t busNum, uint8_t devAddr, uint8_t interface, uint8_t force)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, busNum, devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, interface, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, force, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_CLAIM_INTERFACE),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::UsbAttachKernelDriver(uint8_t busNum, uint8_t devAddr, uint8_t interface)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, busNum, devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, interface, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_ATTACH_KERNEL_DRIVER),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::UsbDetachKernelDriver(uint8_t busNum, uint8_t devAddr, uint8_t interface)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, busNum, devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, interface, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_DETACH_KERNEL_DRIVER),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::ReleaseInterface(uint8_t busNum, uint8_t devAddr, uint8_t interface)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, busNum, devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, interface, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_RELEASE_INTERFACE),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}
int32_t UsbServerProxy::BulkTransferRead(
    const UsbDev &dev, const UsbPipe &pipe, std::vector<uint8_t> &bufferData, int32_t timeOut)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, dev.busNum, dev.devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.intfId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, timeOut, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_BULK_TRANSFER_READ),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    ret = GetBufferMessage(reply, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "get buffer is failed, error code: %{public}d", ret);
        return ret;
    }
    USB_HILOGI(MODULE_USBD, "Set buffer message. length = %{public}zu", bufferData.size());
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::BulkTransferReadwithLength(const UsbDev &dev, const UsbPipe &pipe,
    int32_t length, std::vector<uint8_t> &bufferData, int32_t timeOut)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, dev.busNum, dev.devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.intfId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, length, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, timeOut, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_BULK_TRANSFER_READ_WITH_LENGTH),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    ret = GetBufferMessage(reply, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "get buffer is failed, error code: %{public}d", ret);
        return ret;
    }
    USB_HILOGI(MODULE_USBD, "Set buffer message. length = %{public}zu", bufferData.size());
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::BulkTransferWrite(
    const UsbDev &dev, const UsbPipe &pipe, const std::vector<uint8_t> &bufferData, int32_t timeOut)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, dev.busNum, dev.devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.intfId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, timeOut, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = SetBufferMessage(data, bufferData);
    if (UEC_OK != ret) {
        USB_HILOGE(MODULE_INNERKIT, "SetBufferMessage ret:%{public}d", ret);
        return ret;
    }
    ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_BULK_TRANSFER_WRITE),
        data, reply, option);
    if (UEC_OK != ret) {
        USB_HILOGE(MODULE_INNERKIT, "SendRequest ret:%{public}d", ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::ControlTransfer(
    const UsbDev &dev, const UsbCtrlTransfer &ctrl, std::vector<uint8_t> &bufferData)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_SERVICE_INNER_ERR;
    }
    SetDeviceMessage(data, dev.busNum, dev.devAddr);
    WRITE_PARCEL_WITH_RET(data, Int32, ctrl.requestType, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, ctrl.requestCmd, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, ctrl.value, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, ctrl.index, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, ctrl.timeout, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = SetBufferMessage(data, bufferData);
    if (UEC_OK != ret) {
        USB_HILOGE(MODULE_INNERKIT, "write failed! len:%{public}d", ret);
        return ret;
    }

    uint32_t reqType = static_cast<uint32_t>(ctrl.requestType);
    bool isWrite = ((reqType & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_DIR_OUT);
    ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_CONTROL_TRANSFER), data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "USB_FUN_CONTROL_TRANSFER ret:%{public}d", ret);
        return ret;
    }
    if (!isWrite) {
        ret = GetBufferMessage(reply, bufferData);
        if (UEC_OK != ret) {
            USB_HILOGE(MODULE_USBD, "Get buffer message error. ret = %{public}d", ret);
            return ret;
        }
        USB_HILOGI(MODULE_USBD, "Get buffer message. length = %{public}zu", bufferData.size());
    }
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::UsbControlTransfer(
    const UsbDev &dev, const UsbCtrlTransferParams &ctrlParams, std::vector<uint8_t> &bufferData)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_SERVICE_INNER_ERR;
    }
    SetDeviceMessage(data, dev.busNum, dev.devAddr);
    WRITE_PARCEL_WITH_RET(data, Int32, ctrlParams.requestType, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, ctrlParams.requestCmd, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, ctrlParams.value, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, ctrlParams.index, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, ctrlParams.length, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, ctrlParams.timeout, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = SetBufferMessage(data, bufferData);
    if (UEC_OK != ret) {
        USB_HILOGE(MODULE_INNERKIT, "write failed! len:%{public}d", ret);
        return ret;
    }

    uint32_t reqType = static_cast<uint32_t>(ctrlParams.requestType);
    bool isWrite = ((reqType & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_DIR_OUT);
    ret = remote->SendRequest(
        static_cast<int32_t>(UsbInterfaceCode::USB_FUN_USB_CONTROL_TRANSFER), data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "USB_FUN_USB_CONTROL_TRANSFER ret:%{public}d", ret);
        return ret;
    }
    if (!isWrite) {
        ret = GetBufferMessage(reply, bufferData);
        if (UEC_OK != ret) {
            USB_HILOGE(MODULE_USBD, "Get buffer message error. ret = %{public}d", ret);
            return ret;
        }
        USB_HILOGI(MODULE_USBD, "Get buffer message. length = %{public}zu", bufferData.size());
    }
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::SetActiveConfig(uint8_t busNum, uint8_t devAddr, uint8_t configIndex)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, busNum, devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, configIndex, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_SET_ACTIVE_CONFIG),
        data, reply, option);
    if (UEC_OK != ret) {
        USB_HILOGE(MODULE_INNERKIT, "USB_FUN_SET_ACTIVE_CONFIG ret:%{public}d", ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}
int32_t UsbServerProxy::GetActiveConfig(uint8_t busNum, uint8_t devAddr, uint8_t &configIndex)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, busNum, devAddr);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_GET_ACTIVE_CONFIG),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "USB_FUN_GET_ACTIVE_CONFIG ret:%{public}d", ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Uint8, configIndex, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return ret;
}
int32_t UsbServerProxy::SetInterface(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid, uint8_t altIndex)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, busNum, devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, interfaceid, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, altIndex, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_SET_INTERFACE),
        data, reply, option);
    if (UEC_OK != ret) {
        USB_HILOGE(MODULE_INNERKIT, "USB_FUN_SET_INTERFACE ret:%{public}d", ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}
int32_t UsbServerProxy::GetRawDescriptor(uint8_t busNum, uint8_t devAddr, std::vector<uint8_t> &bufferData)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, busNum, devAddr);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_GET_DESCRIPTOR),
        data, reply, option);
    if (ret == UEC_OK) {
        ret = GetBufferMessage(reply, bufferData);
        if (UEC_OK != ret) {
            USB_HILOGE(MODULE_INNERKIT, "get failed ret:%{public}d", ret);
        }
    }
    return ret;
}

int32_t UsbServerProxy::GetFileDescriptor(uint8_t busNum, uint8_t devAddr, int32_t &fd)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, busNum, devAddr);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_GET_FILEDESCRIPTOR),
        data, reply, option);
    if (ret == UEC_OK) {
        fd = -1;
        if (!ReadFileDescriptor(reply, fd)) {
            USB_HILOGW(MODULE_USB_SERVICE, "%{public}s: read fd failed!", __func__);
            return UEC_INTERFACE_READ_PARCEL_ERROR;
        }
    }
    return ret;
}

int32_t UsbServerProxy::RequestQueue(const UsbDev &dev, const UsbPipe &pipe, const std::vector<uint8_t> &clientData,
    const std::vector<uint8_t> &bufferData)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "get descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, dev.busNum, dev.devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.intfId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);

    int32_t ret = UsbServerProxy::SetBufferMessage(data, clientData);
    if (UEC_OK != ret) {
        USB_HILOGE(MODULE_INNERKIT, "set clientData failed ret:%{public}d", ret);
        return ERR_INVALID_VALUE;
    }

    ret = UsbServerProxy::SetBufferMessage(data, bufferData);
    if (UEC_OK != ret) {
        USB_HILOGE(MODULE_INNERKIT, "setBuffer failed ret:%{public}d", ret);
        return ERR_INVALID_VALUE;
    }

    ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_REQUEST_QUEUE), data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "SendRequest failed!");
        return ret;
    }
    return ret;
}

int32_t UsbServerProxy::RequestWait(
    const UsbDev &dev, int32_t timeOut, std::vector<uint8_t> &clientData, std::vector<uint8_t> &bufferData)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "get descriptor failed!");
        return ERR_ENOUGH_DATA;
    }

    SetDeviceMessage(data, dev.busNum, dev.devAddr);
    WRITE_PARCEL_WITH_RET(data, Int32, timeOut, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_REQUEST_WAIT),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "queue failed! ret:%{public}d", ret);
        return ret;
    }

    ret = UsbServerProxy::GetBufferMessage(reply, clientData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "get clientData failed! ret:%{public}d", ret);
        return ret;
    }

    ret = UsbServerProxy::GetBufferMessage(reply, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "get buffer failed! ret:%{public}d", ret);
        return ret;
    }

    return ret;
}

int32_t UsbServerProxy::RequestCancel(uint8_t busNum, uint8_t devAddr, uint8_t interfaceid, uint8_t endpointId)
{
    int32_t ret;
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "get descriptor failed!");
        return ERR_ENOUGH_DATA;
    }

    SetDeviceMessage(data, busNum, devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, interfaceid, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_REQUEST_CANCEL), data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "request cancel failed!");
    }

    return ret;
}

int32_t UsbServerProxy::Close(uint8_t busNum, uint8_t devAddr)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "get descriptor failed!");
        return ERR_ENOUGH_DATA;
    }

    SetDeviceMessage(data, busNum, devAddr);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_CLOSE_DEVICE),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "queue failed!");
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::RegBulkCallback(const UsbDev &dev, const UsbPipe &pipe, const sptr<IRemoteObject> &cb)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, dev.busNum, dev.devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.intfId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, RemoteObject, cb, UEC_SERVICE_WRITE_PARCEL_ERROR);
    MessageOption option;
    MessageParcel reply;
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_REG_BULK_CALLBACK),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "SendRequest failed!");
        return ret;
    }
    return ret;
}

int32_t UsbServerProxy::UnRegBulkCallback(const UsbDev &dev, const UsbPipe &pipe)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, dev.busNum, dev.devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.intfId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    MessageOption option;
    MessageParcel reply;
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_UNREG_BULK_CALLBACK),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "SendRequest failed!");
        return ret;
    }
    return ret;
}

int32_t UsbServerProxy::BulkRead(const UsbDev &dev, const UsbPipe &pipe, sptr<Ashmem> &ashmem)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, dev.busNum, dev.devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.intfId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Ashmem, ashmem, UEC_SERVICE_WRITE_PARCEL_ERROR);
    MessageOption option;
    MessageParcel reply;
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_BULK_AYSNC_READ),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "SendRequest failed!");
        return ret;
    }
    return ret;
}

int32_t UsbServerProxy::BulkWrite(const UsbDev &dev, const UsbPipe &pipe, sptr<Ashmem> &ashmem)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, dev.busNum, dev.devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.intfId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Ashmem, ashmem, UEC_SERVICE_WRITE_PARCEL_ERROR);
    MessageOption option;
    MessageParcel reply;
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_BULK_AYSNC_WRITE),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "SendRequest failed!");
        return ret;
    }
    return ret;
}

int32_t UsbServerProxy::BulkCancel(const UsbDev &dev, const UsbPipe &pipe)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, dev.busNum, dev.devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.intfId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    MessageOption option;
    MessageParcel reply;
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_BULK_AYSNC_CANCEL),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "SendRequest failed!");
        return ret;
    }
    return ret;
}

int32_t UsbServerProxy::AddRight(const std::string &bundleName, const std::string &deviceName)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);

    MessageParcel data;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_USB_SERVICE, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    WRITE_PARCEL_WITH_RET(data, String, bundleName, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, String, deviceName, UEC_SERVICE_WRITE_PARCEL_ERROR);

    MessageOption option;
    MessageParcel reply;
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_ADD_RIGHT), data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "SendRequest is failed, error code: %{public}d", ret);
    }
    return ret;
}

int32_t UsbServerProxy::AddAccessRight(const std::string &tokenId, const std::string &deviceName)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);

    MessageParcel data;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_USB_SERVICE, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    WRITE_PARCEL_WITH_RET(data, String, tokenId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, String, deviceName, UEC_SERVICE_WRITE_PARCEL_ERROR);

    MessageOption option;
    MessageParcel reply;
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_ADD_ACCESS_RIGHT),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "SendRequest is failed, error code: %{public}d", ret);
    }
    return ret;
}

int32_t UsbServerProxy::ManageGlobalInterface(bool disable)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);

    MessageParcel data;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_USB_SERVICE, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    WRITE_PARCEL_WITH_RET(data, Bool, disable, UEC_SERVICE_WRITE_PARCEL_ERROR);

    MessageOption option;
    MessageParcel reply;
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_DISABLE_GLOBAL_INTERFACE),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "SendRequest is failed, error code: %{public}d", ret);
    }
    return ret;
}

int32_t UsbServerProxy::ManageDevice(int32_t vendorId, int32_t productId, bool disable)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);

    MessageParcel data;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_USB_SERVICE, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    WRITE_PARCEL_WITH_RET(data, Int32, vendorId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, productId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Bool, disable, UEC_SERVICE_WRITE_PARCEL_ERROR);

    MessageOption option;
    MessageParcel reply;
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_DISABLE_DEVICE),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "SendRequest is failed, error code: %{public}d", ret);
    }
    return ret;
}

int32_t UsbServerProxy::ManageInterfaceType(const std::vector<UsbDeviceType> &disableType, bool disable)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);

    MessageParcel data;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_USB_SERVICE, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    int32_t size = (int32_t)disableType.size();
    WRITE_PARCEL_WITH_RET(data, Int32, size, UEC_SERVICE_WRITE_PARCEL_ERROR);

    for (const auto &type : disableType) {
        WRITE_PARCEL_WITH_RET(data, Int32, type.baseClass, UEC_SERVICE_WRITE_PARCEL_ERROR);
        WRITE_PARCEL_WITH_RET(data, Int32, type.subClass, UEC_SERVICE_WRITE_PARCEL_ERROR);
        WRITE_PARCEL_WITH_RET(data, Int32, type.protocol, UEC_SERVICE_WRITE_PARCEL_ERROR);
        WRITE_PARCEL_WITH_RET(data, Bool, type.isDeviceType, UEC_SERVICE_WRITE_PARCEL_ERROR);
    }
    WRITE_PARCEL_WITH_RET(data, Bool, disable, UEC_SERVICE_WRITE_PARCEL_ERROR);

    MessageOption option;
    MessageParcel reply;
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_DISABLE_INTERFACE_TYPE),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "SendRequest is failed, error code: %{public}d", ret);
    }
    return ret;
}

int32_t UsbServerProxy::ClearHalt(uint8_t busNum, uint8_t devAddr, uint8_t interfaceId, uint8_t endpointId)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, busNum, devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_CLEAR_HALT), data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "ClearHalt is failed, error code: %{public}d", ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::GetDeviceSpeed(uint8_t busNum, uint8_t devAddr, uint8_t &speed)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, busNum, devAddr);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_GET_DEVICE_SPEED),
        data, reply, option);
    if (ret == UEC_OK) {
        READ_PARCEL_WITH_RET(reply, Uint8, speed, UEC_INTERFACE_READ_PARCEL_ERROR);
    }
    USB_HILOGE(MODULE_INNERKIT, "GetDeviceSpeed speed:%{public}u", speed);
    return ret;
}

int32_t UsbServerProxy::GetInterfaceActiveStatus(uint8_t busNum, uint8_t devAddr,
    uint8_t interfaceid,  bool &unactivated)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    SetDeviceMessage(data, busNum, devAddr);
    WRITE_PARCEL_WITH_RET(data, Uint8, interfaceid, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_GET_DRIVER_ACTIVE_STATUS),
        data, reply, option);
    if (ret == UEC_OK) {
        READ_PARCEL_WITH_RET(reply, Bool, unactivated, UEC_INTERFACE_READ_PARCEL_ERROR);
    }
    return ret;
}
bool UsbServerProxy::ReadFileDescriptor(MessageParcel &data, int &fd)
{
    fd = -1;
    bool fdValid = false;
    if (!data.ReadBool(fdValid)) {
        USB_HILOGE(MODULE_USB_SERVICE, "%{public}s: failed to read fdValid", __func__);
        return false;
    }

    if (fdValid) {
        fd = data.ReadFileDescriptor();
        if (fd < 0) {
            USB_HILOGE(MODULE_USB_SERVICE, "%{public}s: failed to read fd", __func__);
            return false;
        }
    }
    return true;
}

int32_t UsbServerProxy::GetAccessoryList(std::vector<USBAccessory> &accessList)
{
    int32_t ret;
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        USB_HILOGE(MODULE_USB_INNERKIT, "remote is failed");
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_INVALID_VALUE;
    }

    ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_GET_ACCESSORY_LIST), data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed code: %{public}d", ret);
        return ret;
    }
    ret = GetAccessoryListMessageParcel(reply, accessList);
    return ret;
}

int32_t UsbServerProxy::SetAccessoryMessageParcel(const USBAccessory &accessoryInfo, MessageParcel &data)
{
    USB_HILOGD(MODULE_USB_INNERKIT, "%{public}s, proxy parse: %{public}s.",
        __func__, accessoryInfo.GetJsonString().c_str());
    WRITE_PARCEL_WITH_RET(data, String, accessoryInfo.GetManufacturer(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, String, accessoryInfo.GetProduct(), UEC_SERVICE_WRITE_PARCEL_ERROR);

    WRITE_PARCEL_WITH_RET(data, String, accessoryInfo.GetDescription(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, String, accessoryInfo.GetVersion(), UEC_SERVICE_WRITE_PARCEL_ERROR);

    WRITE_PARCEL_WITH_RET(data, String, accessoryInfo.GetSerialNumber(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerProxy::OpenAccessory(const USBAccessory &access, int32_t &fd)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    if (SetAccessoryMessageParcel(access, data) != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "write accessory info failed!");
        return ERR_ENOUGH_DATA;
    }
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_OPEN_ACCESSORY),
        data, reply, option);
    if (ret == UEC_OK) {
        fd = -1;
        if (!ReadFileDescriptor(reply, fd)) {
            USB_HILOGW(MODULE_USB_SERVICE, "%{public}s: read fd failed!", __func__);
            return UEC_INTERFACE_READ_PARCEL_ERROR;
        }
    }
    return ret;
}

int32_t UsbServerProxy::AddAccessoryRight(const uint32_t tokenId, const USBAccessory &access)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }
    WRITE_PARCEL_WITH_RET(data, Uint32, tokenId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    if (SetAccessoryMessageParcel(access, data) != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "write accessory info failed!");
        return ERR_ENOUGH_DATA;
    }
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_ADD_ACCESSORY_RIGHT),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_SERVICE, "SendRequest add accessory right is failed, error code: %{public}d", ret);
    }
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::HasAccessoryRight(const USBAccessory &access, bool &result)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_SERVICE_INNER_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return ERR_ENOUGH_DATA;
    }

    if (SetAccessoryMessageParcel(access, data) != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "write accessory info failed!");
        return ERR_ENOUGH_DATA;
    }
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_HAS_ACCESSORY_RIGHT),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest has accessory right is failed, error code: %{public}d", ret);
        return false;
    }

    READ_PARCEL_WITH_RET(reply, Bool, result, false);
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::RequestAccessoryRight(const USBAccessory &access, bool &result)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    MessageParcel reply;
    MessageOption option;
    MessageParcel data;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    if (SetAccessoryMessageParcel(access, data) != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "write accessory info failed!");
        return ERR_ENOUGH_DATA;
    }
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_REQUEST_ACCESSORY_RIGHT),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest request accessory right is failed, error code: %{public}d", ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Bool, result, false);
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::CancelAccessoryRight(const USBAccessory &access)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    MessageParcel reply;
    MessageOption option;
    MessageParcel data;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    if (SetAccessoryMessageParcel(access, data) != UEC_OK) {
        USB_HILOGE(MODULE_INNERKIT, "write accessory info failed!");
        return ERR_ENOUGH_DATA;
    }
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_REMOVE_ACCESSORY_RIGHT),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest remove accessory right is failed, error code: %{public}d", ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::CloseAccessory(int32_t fd)
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    MessageParcel reply;
    MessageOption option;
    MessageParcel data;
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }

    WRITE_PARCEL_WITH_RET(data, Uint32, fd, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::USB_FUN_CLOSE_ACCESSORY),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest close accessory is failed, error code: %{public}d", ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, ret, UEC_INTERFACE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerProxy::SerialOpen(int32_t portId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    WRITE_PARCEL_WITH_RET(data, Int32, portId, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::SERIAL_FUN_SERIAL_OPEN),
        data, reply, option);
    if (ret) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    return ret;
}

int32_t UsbServerProxy::SerialClose(int32_t portId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    WRITE_PARCEL_WITH_RET(data, Int32, portId, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::SERIAL_FUN_SERIAL_CLOSE),
        data, reply, option);
    if (ret) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    return ret;
}

int32_t UsbServerProxy::SerialRead(int32_t portId, std::vector<uint8_t>& data, uint32_t size)
{
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();

    MessageParcel mData;
    MessageParcel reply;
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    if (!mData.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    WRITE_PARCEL_WITH_RET(mData, Int32, portId, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(mData, Uint32, size, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::SERIAL_FUN_SERIAL_READ),
        mData, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    int32_t mSize;
    READ_PARCEL_WITH_RET(reply, Int32, mSize, UEC_INTERFACE_READ_PARCEL_ERROR);
    USB_HILOGI(MODULE_USB_INNERKIT, "SerialRead size %{public}d", mSize);

    for (int32_t i = 0; i < mSize; ++i) {
        USB_HILOGI(MODULE_USB_INNERKIT, "ParseUsbPort : %{public}d", i);
        ret = SerialReadData(reply, data);
        if (ret) {
            return ret;
        }
    }
    return ret;
}

int32_t UsbServerProxy::SerialWrite(int32_t portId, const std::vector<uint8_t>& data, uint32_t size)
{
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();

    MessageParcel mData;
    MessageParcel reply;
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    if (!mData.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    WRITE_PARCEL_WITH_RET(mData, Int32, portId, UEC_INTERFACE_WRITE_PARCEL_ERROR);

    int32_t mSize = (int32_t)data.size();
    WRITE_PARCEL_WITH_RET(mData, Int32, mSize, UEC_INTERFACE_WRITE_PARCEL_ERROR);

    for (auto it : data) {
        WRITE_PARCEL_WITH_RET(mData, Uint8, it, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    }

    WRITE_PARCEL_WITH_RET(mData, Uint32, size, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::SERIAL_FUN_SERIAL_WRITE),
        mData, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }

    return ret;
}

int32_t UsbServerProxy::SerialGetAttribute(int32_t portId, OHOS::HDI::Usb::Serial::V1_0::SerialAttribute& attribute)
{
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();

    MessageParcel data;
    MessageParcel reply;
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }

    WRITE_PARCEL_WITH_RET(data, Int32, portId, UEC_INTERFACE_WRITE_PARCEL_ERROR);

    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::SERIAL_FUN_SERIAL_GET_ATTRIBUTE),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }

    uint32_t intTmp;
    uint8_t strTmp;
    READ_PARCEL_WITH_RET(reply, Uint32, intTmp, UEC_INTERFACE_READ_PARCEL_ERROR);
    attribute.baudrate = intTmp;

    READ_PARCEL_WITH_RET(reply, Uint8, strTmp, UEC_INTERFACE_READ_PARCEL_ERROR);
    attribute.dataBits = strTmp;

    READ_PARCEL_WITH_RET(reply, Uint8, strTmp, UEC_INTERFACE_READ_PARCEL_ERROR);
    attribute.parity = strTmp;

    READ_PARCEL_WITH_RET(reply, Uint8, strTmp, UEC_INTERFACE_READ_PARCEL_ERROR);
    attribute.stopBits = strTmp;

    return ret;
}

int32_t UsbServerProxy::SerialSetAttribute(int32_t portId,
    const OHOS::HDI::Usb::Serial::V1_0::SerialAttribute& attribute)
{
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();

    MessageParcel data;
    MessageParcel reply;
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }

    WRITE_PARCEL_WITH_RET(data, Int32, portId, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint32, attribute.baudrate, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, attribute.dataBits, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, attribute.parity, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, attribute.stopBits, UEC_INTERFACE_WRITE_PARCEL_ERROR);

    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::SERIAL_FUN_SERIAL_SET_ATTRIBUTE),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }

    return ret;
}

int32_t UsbServerProxy::SerialGetPortList(std::vector<OHOS::HDI::Usb::Serial::V1_0::SerialPort>& serialPortList)
{
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();

    MessageParcel data;
    MessageParcel reply;
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::SERIAL_FUN_SERIAL_GET_PORTLIST),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }

    int32_t size;
    READ_PARCEL_WITH_RET(reply, Int32, size, UEC_INTERFACE_READ_PARCEL_ERROR);

    for (int32_t i = 0; i < size; ++i) {
        ParseSerialPort(reply, serialPortList);
    }
    return ret;
}

bool UsbServerProxy::HasSerialRight(int32_t portId)
{
    MessageParcel data;
    MessageOption option;
    MessageParcel reply;
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, false);
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return false;
    }

    WRITE_PARCEL_WITH_RET(data, Int32, portId, false);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::SERIAL_FUN_HAS_SERIAL_RIGHT),
        data, reply, option);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return false;
    }

    bool result = false;
    READ_PARCEL_WITH_RET(reply, Bool, result, false);

    return result;
}

int32_t UsbServerProxy::AddSerialRight(uint32_t tokenId, int32_t portId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    WRITE_PARCEL_WITH_RET(data, Uint32, tokenId, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, portId, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::SERIAL_FUN_ADD_SERIAL_RIGHT),
        data, reply, option);
    if (ret) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    return ret;
}

int32_t UsbServerProxy::CancelSerialRight(int32_t portId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    WRITE_PARCEL_WITH_RET(data, Int32, portId, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::SERIAL_FUN_CANCEL_SERIAL_RIGHT),
        data, reply, option);
    if (ret) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    return ret;
}

int32_t UsbServerProxy::RequestSerialRight(int32_t portId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, UEC_INTERFACE_INVALID_VALUE);
    if (!data.WriteInterfaceToken(UsbServerProxy::GetDescriptor())) {
        USB_HILOGE(MODULE_INNERKIT, "write descriptor failed!");
        return UEC_INTERFACE_WRITE_PARCEL_ERROR;
    }
    WRITE_PARCEL_WITH_RET(data, Int32, portId, UEC_INTERFACE_WRITE_PARCEL_ERROR);
    int32_t ret = remote->SendRequest(static_cast<int32_t>(UsbInterfaceCode::SERIAL_FUN_REQUEST_SERIAL_RIGHT),
        data, reply, option);
    if (ret) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    return ret;
}

int32_t UsbServerProxy::SerialReadData(MessageParcel &reply, std::vector<uint8_t> &data)
{
    uint8_t tmp;
    READ_PARCEL_WITH_RET(reply, Uint8, tmp, UEC_INTERFACE_READ_PARCEL_ERROR);
    USB_HILOGI(MODULE_USB_INNERKIT, "UsbServerProxy::port->id %{public}d", tmp);
    data.push_back(tmp);
    return UEC_OK;
}

void UsbServerProxy::ParseSerialPort(MessageParcel &reply,
    std::vector<OHOS::HDI::Usb::Serial::V1_0::SerialPort>& serialPorts)
{
    int32_t intTmp;
    std::string strTmp;
    uint8_t uint8Tmp;
    OHOS::HDI::Usb::Serial::V1_0::SerialPort port;
    READ_PARCEL_WITH_RET(reply, Int32, intTmp, UEC_INTERFACE_READ_PARCEL_ERROR);
    port.portId = intTmp;

    READ_PARCEL_WITH_RET(reply, Uint8, uint8Tmp, UEC_INTERFACE_READ_PARCEL_ERROR);
    port.deviceInfo.busNum = uint8Tmp;

    READ_PARCEL_WITH_RET(reply, Uint8, uint8Tmp, UEC_INTERFACE_READ_PARCEL_ERROR);
    port.deviceInfo.devAddr = uint8Tmp;

    READ_PARCEL_WITH_RET(reply, Int32, intTmp, UEC_INTERFACE_READ_PARCEL_ERROR);
    port.deviceInfo.vid = intTmp;

    READ_PARCEL_WITH_RET(reply, Int32, intTmp, UEC_INTERFACE_READ_PARCEL_ERROR);
    port.deviceInfo.pid = intTmp;

    READ_PARCEL_WITH_RET(reply, String, strTmp, UEC_INTERFACE_READ_PARCEL_ERROR);
    port.deviceInfo.serialNum = strTmp;

    serialPorts.push_back(port);
}
} // namespace USB
} // namespace OHOS
