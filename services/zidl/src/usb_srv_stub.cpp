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

#include "message_parcel.h"
#include "securec.h"
#include "string_ex.h"
#include "usb_common.h"
#include "usb_errors.h"
#include "usb_server_stub.h"
#include "usb_interface_type.h"
#include "v1_1/iusb_interface.h"
#include "usb_report_sys_event.h"
#include "hitrace_meter.h"
#include "usb_accessory.h"

using namespace OHOS::HDI::Usb::V1_1;
namespace OHOS {
namespace USB {
constexpr int32_t MAX_EDM_LIST_SIZE = 200;
int32_t UsbServerStub::GetDeviceMessage(MessageParcel &data, uint8_t &busNum, uint8_t &devAddr)
{
    if (!data.ReadUint8(busNum)) {
        return UEC_SERVICE_READ_PARCEL_ERROR;
    }
    if (!data.ReadUint8(devAddr)) {
        return UEC_SERVICE_READ_PARCEL_ERROR;
    }
    return UEC_OK;
}

int32_t UsbServerStub::SetBufferMessage(MessageParcel &data, const std::vector<uint8_t> &bufferData)
{
    uint32_t length = bufferData.size();
    const uint8_t *ptr = bufferData.data();
    if (!ptr) {
        length = 0;
    }

    if (!data.WriteUint32(length)) {
        USB_HILOGE(MODULE_USBD, "write length failed length:%{public}u", length);
        return UEC_SERVICE_WRITE_PARCEL_ERROR;
    }
    if ((ptr) && (length > 0) && !data.WriteBuffer(reinterpret_cast<const void *>(ptr), length)) {
        USB_HILOGE(MODULE_USBD, "writer buffer failed length:%{public}u", length);
        return UEC_SERVICE_WRITE_PARCEL_ERROR;
    }
    return UEC_OK;
}

int32_t UsbServerStub::GetBufferMessage(MessageParcel &data, std::vector<uint8_t> &bufferData)
{
    uint32_t dataSize = 0;
    bufferData.clear();
    if (!data.ReadUint32(dataSize)) {
        USB_HILOGE(MODULE_USBD, "read dataSize failed");
        return UEC_SERVICE_READ_PARCEL_ERROR;
    }
    if (dataSize == 0) {
        USB_HILOGW(MODULE_USBD, "size:%{public}u", dataSize);
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

bool UsbServerStub::WriteFileDescriptor(MessageParcel &data, int fd)
{
    if (!data.WriteBool(fd >= 0 ? true : false)) {
        USB_HILOGE(MODULE_USB_SERVICE, "%{public}s: failed to write fd vailed", __func__);
        return false;
    }
    if (fd < 0) {
        return true;
    }
    if (!data.WriteFileDescriptor(fd)) {
        USB_HILOGE(MODULE_USB_SERVICE, "%{public}s: failed to write fd", __func__);
        return false;
    }
    return true;
}

bool UsbServerStub::StubDevice(
    uint32_t code, int32_t &result, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    switch (code) {
        case static_cast<int>(UsbInterfaceCode::USB_FUN_OPEN_DEVICE):
            result = DoOpenDevice(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_RESET_DEVICE):
            result = DoResetDevice(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_HAS_RIGHT):
            result = DoHasRight(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_REQUEST_RIGHT):
            result = DoRequestRight(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_REMOVE_RIGHT):
            result = DoRemoveRight(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_GET_PORTS):
            result = DoGetPorts(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_GET_SUPPORTED_MODES):
            result = DoGetSupportedModes(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_SET_PORT_ROLE):
            result = DoSetPortRole(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_CLAIM_INTERFACE):
            result = DoClaimInterface(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_ATTACH_KERNEL_DRIVER):
            result = DoUsbAttachKernelDriver(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_DETACH_KERNEL_DRIVER):
            result = DoUsbDetachKernelDriver(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_RELEASE_INTERFACE):
            result = DoReleaseInterface(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_REG_BULK_CALLBACK):
            result = DoRegBulkCallback(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_UNREG_BULK_CALLBACK):
            result = DoUnRegBulkCallback(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_GET_FILEDESCRIPTOR):
            result = DoGetFileDescriptor(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_ADD_RIGHT):
            result = DoAddRight(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_ADD_ACCESS_RIGHT):
            result = DoAddAccessRight(data, reply, option);
            return true;
        default:;
    }
    return false;
}

bool UsbServerStub::StubHost(
    uint32_t code, int32_t &result, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    switch (code) {
        case static_cast<int>(UsbInterfaceCode::USB_FUN_GET_DEVICES):
            result = DoGetDevices(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_GET_CURRENT_FUNCTIONS):
            result = DoGetCurrentFunctions(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_SET_CURRENT_FUNCTIONS):
            result = DoSetCurrentFunctions(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_USB_FUNCTIONS_FROM_STRING):
            result = DoUsbFunctionsFromString(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_USB_FUNCTIONS_TO_STRING):
            result = DoUsbFunctionsToString(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_CLOSE_DEVICE):
            result = DoClose(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_REQUEST_QUEUE):
            result = DoRequestQueue(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_REQUEST_WAIT):
            result = DoRequestWait(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_SET_INTERFACE):
            result = DoSetInterface(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_SET_ACTIVE_CONFIG):
            result = DoSetActiveConfig(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_REQUEST_CANCEL):
            result = DoRequestCancel(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_BULK_AYSNC_READ):
            result = DoBulkRead(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_BULK_AYSNC_WRITE):
            result = DoBulkWrite(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_BULK_AYSNC_CANCEL):
            result = DoBulkCancel(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_GET_DESCRIPTOR):
            result = DoGetRawDescriptor(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_DISABLE_GLOBAL_INTERFACE):
            result = DoManageGlobalInterface(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_DISABLE_DEVICE):
            result = DoManageDevice(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_DISABLE_INTERFACE_TYPE):
            result = DoManageInterfaceType(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_CLEAR_HALT):
            result = DoClearHalt(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_GET_DEVICE_SPEED):
            result = DoGetDeviceSpeed(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_GET_DRIVER_ACTIVE_STATUS):
            result = DoGetInterfaceActiveStatus(data, reply, option);
            return true;
        default:;
    }
    return false;
}

bool UsbServerStub::StubHostTransfer(uint32_t code, int32_t &result,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    switch (code) {
        case static_cast<int>(UsbInterfaceCode::USB_FUN_BULK_TRANSFER_READ):
            result = DoBulkTransferRead(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_BULK_TRANSFER_READ_WITH_LENGTH):
            result = DoBulkTransferReadwithLength(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_BULK_TRANSFER_WRITE):
            result = DoBulkTransferWrite(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_CONTROL_TRANSFER):
            result = DoControlTransfer(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_USB_CONTROL_TRANSFER):
            result = DoUsbControlTransfer(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_GET_ACCESSORY_LIST):
            result = DoGetAccessoryList(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_OPEN_ACCESSORY):
            result = DoOpenAccessory(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_CLOSE_ACCESSORY):
            result = DoCloseAccessory(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_HAS_ACCESSORY_RIGHT):
            result = DoHasAccessoryRight(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_REMOVE_ACCESSORY_RIGHT):
            result = DoCancelAccessoryRight(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_ADD_ACCESSORY_RIGHT):
            result = DoAddAccessoryRight(data, reply, option);
            return true;
        case static_cast<int>(UsbInterfaceCode::USB_FUN_REQUEST_ACCESSORY_RIGHT):
            result = DoRequestAccessoryRight(data, reply, option);
            return true;
        default:;
    }
    return false;
}

int32_t UsbServerStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    USB_HILOGD(MODULE_USB_SERVICE, "UsbServerStub::OnRemoteRequest, cmd = %{public}u, flags = %{public}d", code,
        option.GetFlags());
    std::u16string descriptor = UsbServerStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        USB_HILOGE(MODULE_SERVICE, "UsbServerStub::OnRemoteRequest failed, descriptor is not matched!");
        return UEC_SERVICE_INNER_ERR;
    }

    int32_t ret = 0;
    if (StubHost(code, ret, data, reply, option)) {
        return ret;
    } else if (StubDevice(code, ret, data, reply, option)) {
        return ret;
    } else if (StubHostTransfer(code, ret, data, reply, option)) {
        return ret;
    } else {
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return UEC_OK;
}

int32_t UsbServerStub::DoGetCurrentFunctions(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t functions;
    int32_t ret = GetCurrentFunctions(functions);
    if (ret != UEC_OK) {
        UsbReportSysEvent::ReportTransforFaultSysEvent("GetCurrentFunctions", {0, 0}, {0, 0}, ret);
        return ret;
    }
    WRITE_PARCEL_WITH_RET(reply, Int32, functions, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerStub::DoSetCurrentFunctions(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HITRACE_METER_NAME(HITRACE_TAG_USB, "SetCurrentFunctions");
    int32_t funcs;
    READ_PARCEL_WITH_RET(data, Int32, funcs, UEC_SERVICE_READ_PARCEL_ERROR);
    int32_t ret = SetCurrentFunctions(funcs);
    if (ret != UEC_OK) {
        UsbReportSysEvent::ReportTransforFaultSysEvent("SetCurrentFunctions", {0, 0}, {0, 0}, ret);
    }
    return ret;
}

int32_t UsbServerStub::DoUsbFunctionsFromString(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::string funcs;
    READ_PARCEL_WITH_RET(data, String, funcs, UEC_SERVICE_READ_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(reply, Int32, UsbFunctionsFromString(funcs), UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerStub::DoUsbFunctionsToString(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t funcs;
    READ_PARCEL_WITH_RET(data, Int32, funcs, UEC_SERVICE_READ_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(reply, String, UsbFunctionsToString(funcs), UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerStub::DoOpenDevice(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_READ_PARCEL_ERROR);
    int32_t ret = OpenDevice(busNum, devAddr);
    if (ret != UEC_OK) {
        UsbReportSysEvent::ReportTransforFaultSysEvent("OpenDevice", {busNum, devAddr}, {0, 0}, ret);
        return ret;
    }

    return UEC_OK;
}

int32_t UsbServerStub::DoResetDevice(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_READ_PARCEL_ERROR);
    int32_t ret = ResetDevice(busNum, devAddr);
    if (ret != UEC_OK) {
        UsbReportSysEvent::ReportTransforFaultSysEvent("ResetDevice", {busNum, devAddr}, {0, 0}, ret);
        return ret;
    }

    return UEC_OK;
}

int32_t UsbServerStub::DoHasRight(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string deviceName = u"";
    READ_PARCEL_WITH_RET(data, String16, deviceName, UEC_SERVICE_READ_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(reply, Bool, HasRight(Str16ToStr8(deviceName)), UEC_SERVICE_WRITE_PARCEL_ERROR);

    return UEC_OK;
}

int32_t UsbServerStub::DoRequestRight(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string deviceName = u"";
    READ_PARCEL_WITH_RET(data, String16, deviceName, UEC_SERVICE_READ_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(reply, Int32, RequestRight(Str16ToStr8(deviceName)), UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerStub::DoRemoveRight(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string deviceName = u"";
    READ_PARCEL_WITH_RET(data, String16, deviceName, UEC_SERVICE_READ_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(reply, Int32, RemoveRight(Str16ToStr8(deviceName)), UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerStub::DoGetPorts(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::vector<UsbPort> ports;
    int32_t ret = GetPorts(ports);
    USB_HILOGI(MODULE_SERVICE, "UsbServerStub::GetPorts ret %{public}d ", ret);
    if (ret != UEC_OK) {
        UsbReportSysEvent::ReportTransforFaultSysEvent("GetPorts", {0, 0}, {0, 0}, ret);
        return ret;
    }
    uint32_t size = ports.size();
    USB_HILOGI(MODULE_SERVICE, "UsbServerStub::GetPorts size %{public}d ", size);
    WRITE_PARCEL_WITH_RET(reply, Int32, size, UEC_SERVICE_WRITE_PARCEL_ERROR);
    for (uint32_t i = 0; i < size; ++i) {
        ret = WriteUsbPort(reply, ports[i]);
        if (ret) {
            return ret;
        }
    }
    return ret;
}

int32_t UsbServerStub::WriteUsbPort(MessageParcel &reply, const UsbPort &port)
{
    WRITE_PARCEL_WITH_RET(reply, Int32, port.id, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(reply, Int32, port.supportedModes, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(reply, Int32, port.usbPortStatus.currentMode, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(reply, Int32, port.usbPortStatus.currentPowerRole, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(reply, Int32, port.usbPortStatus.currentDataRole, UEC_SERVICE_WRITE_PARCEL_ERROR);
    USB_HILOGI(MODULE_SERVICE, "UsbServerStub::GetPorts supportedModes %{public}d ", port.supportedModes);
    return UEC_OK;
}

int32_t UsbServerStub::DoGetSupportedModes(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t supportedModes = 0;
    int32_t portId = 0;
    READ_PARCEL_WITH_RET(data, Int32, portId, UEC_SERVICE_READ_PARCEL_ERROR);
    int32_t ret = GetSupportedModes(portId, supportedModes);
    if (ret != UEC_OK) {
        return ret;
    }
    WRITE_PARCEL_WITH_RET(reply, Int32, supportedModes, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerStub::DoSetPortRole(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HITRACE_METER_NAME(HITRACE_TAG_USB, "SetPortRole");
    int32_t portId = 0;
    int32_t powerRole = 0;
    int32_t dataRole = 0;
    READ_PARCEL_WITH_RET(data, Int32, portId, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, powerRole, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, dataRole, UEC_SERVICE_READ_PARCEL_ERROR);
    int32_t ret = SetPortRole(portId, powerRole, dataRole);
    if (ret != UEC_OK) {
        UsbReportSysEvent::ReportTransforFaultSysEvent("SetPortRole", {0, 0}, {0, 0}, ret);
        return ret;
    }
    return ret;
}

int32_t UsbServerStub::DoClaimInterface(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HITRACE_METER_NAME(HITRACE_TAG_USB, "ClaimInterface");
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t interface = 0;
    uint8_t force = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, interface, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, force, UEC_SERVICE_READ_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(
        reply, Int32, ClaimInterface(busNum, devAddr, interface, force), UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerStub::DoUsbAttachKernelDriver(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HITRACE_METER_NAME(HITRACE_TAG_USB, "UsbAttachKernelDriver");
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t interface = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, interface, UEC_SERVICE_READ_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(
        reply, Int32, UsbAttachKernelDriver(busNum, devAddr, interface), UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerStub::DoUsbDetachKernelDriver(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HITRACE_METER_NAME(HITRACE_TAG_USB, "UsbDetachKernelDriver");
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t interface = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, interface, UEC_SERVICE_READ_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(
        reply, Int32, UsbDetachKernelDriver(busNum, devAddr, interface), UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerStub::DoReleaseInterface(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HITRACE_METER_NAME(HITRACE_TAG_USB, "ReleaseInterface");
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t interface = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, interface, UEC_SERVICE_READ_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(reply, Int32, ReleaseInterface(busNum, devAddr, interface), UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerStub::DoBulkTransferRead(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HITRACE_METER_NAME(HITRACE_TAG_USB, "BulkTransferRead");
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t interface = 0;
    uint8_t endpoint = 0;
    int32_t timeOut = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, interface, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, endpoint, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, timeOut, UEC_SERVICE_WRITE_PARCEL_ERROR);
    std::vector<uint8_t> bufferData;
    const UsbDev tmpDev = {busNum, devAddr};
    const UsbPipe tmpPipe = {interface, endpoint};
    int32_t ret = BulkTransferRead(tmpDev, tmpPipe, bufferData, timeOut);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "read failed ret:%{public}d", ret);
        UsbReportSysEvent::ReportTransforFaultSysEvent("BulkTransferRead", tmpDev, tmpPipe, ret);
        return ret;
    }
    ret = SetBufferMessage(reply, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "set buffer failed ret:%{public}d", ret);
    }
    WRITE_PARCEL_WITH_RET(reply, Int32, ret, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerStub::DoBulkTransferReadwithLength(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HITRACE_METER_NAME(HITRACE_TAG_USB, "BulkTransferRead");
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t interface = 0;
    uint8_t endpoint = 0;
    int32_t length = 0;
    int32_t timeOut = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, interface, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, endpoint, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, length, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, timeOut, UEC_SERVICE_WRITE_PARCEL_ERROR);
    std::vector<uint8_t> bufferData;
    const UsbDev tmpDev = {busNum, devAddr};
    const UsbPipe tmpPipe = {interface, endpoint};
    int32_t ret = BulkTransferReadwithLength(tmpDev, tmpPipe, length, bufferData, timeOut);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "read failed ret:%{public}d", ret);
        UsbReportSysEvent::ReportTransforFaultSysEvent("BulkTransferRead", tmpDev, tmpPipe, ret);
        return ret;
    }
    ret = SetBufferMessage(reply, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "set buffer failed ret:%{public}d", ret);
    }
    WRITE_PARCEL_WITH_RET(reply, Int32, ret, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerStub::DoBulkTransferWrite(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HITRACE_METER_NAME(HITRACE_TAG_USB, "BulkTransferWrite");
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t interface = 0;
    uint8_t endpoint = 0;
    int32_t timeOut = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, interface, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, endpoint, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, timeOut, UEC_SERVICE_WRITE_PARCEL_ERROR);
    std::vector<uint8_t> bufferData;
    const UsbDev tmpDev = {busNum, devAddr};
    const UsbPipe tmpPipe = {interface, endpoint};
    int32_t ret = GetBufferMessage(data, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "GetBufferMessage failedret:%{public}d", ret);
        return ret;
    }
    ret = BulkTransferWrite(tmpDev, tmpPipe, bufferData, timeOut);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "BulkTransferWrite error ret:%{public}d", ret);
        UsbReportSysEvent::ReportTransforFaultSysEvent("BulkTransferWrite", tmpDev, tmpPipe, ret);
    }
    WRITE_PARCEL_WITH_RET(reply, Int32, ret, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerStub::DoControlTransfer(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HITRACE_METER_NAME(HITRACE_TAG_USB, "ControlTransfer");
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    int32_t requestType;
    int32_t request;
    int32_t value;
    int32_t index;
    int32_t timeOut;

    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, requestType, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, request, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, value, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, index, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, timeOut, UEC_SERVICE_WRITE_PARCEL_ERROR);
    std::vector<uint8_t> bufferData;
    int32_t ret = GetBufferMessage(data, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "get error ret:%{public}d", ret);
        return ret;
    }

    bool bWrite = ((static_cast<uint32_t>(requestType) & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_DIR_OUT);
    const UsbDev tmpDev = {busNum, devAddr};
    const UsbCtrlTransfer tctrl = {requestType, request, value, index, timeOut};
    ret = ControlTransfer(tmpDev, tctrl, bufferData);
    if (ret != UEC_OK) {
        UsbReportSysEvent::ReportTransforFaultSysEvent("ControlTransfer", tmpDev, {0, 0}, ret);
        USB_HILOGE(MODULE_USBD, "ControlTransfer error ret:%{public}d", ret);
        return ret;
    }

    if (!bWrite) {
        ret = SetBufferMessage(reply, bufferData);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USBD, "Set buffer message error length = %{public}d", ret);
        }
    }
    WRITE_PARCEL_WITH_RET(reply, Int32, ret, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerStub::DoUsbControlTransfer(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HITRACE_METER_NAME(HITRACE_TAG_USB, "ControlTransfer");
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    int32_t requestType;
    int32_t request;
    int32_t value;
    int32_t index;
    int32_t length;
    int32_t timeOut;

    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, requestType, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, request, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, value, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, index, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, length, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, timeOut, UEC_SERVICE_WRITE_PARCEL_ERROR);
    std::vector<uint8_t> bufferData;
    int32_t ret = GetBufferMessage(data, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "get error ret:%{public}d", ret);
        return ret;
    }

    bool bWrite = ((static_cast<uint32_t>(requestType) & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_DIR_OUT);
    const UsbDev tmpDev = {busNum, devAddr};
    const UsbCtrlTransferParams tctrlParams = {requestType, request, value, index, length, timeOut};
    ret = UsbControlTransfer(tmpDev, tctrlParams, bufferData);
    if (ret != UEC_OK) {
        UsbReportSysEvent::ReportTransforFaultSysEvent("ControlTransfer", tmpDev, {0, 0}, ret);
        USB_HILOGE(MODULE_USBD, "ControlTransfer error ret:%{public}d", ret);
        return ret;
    }

    if (!bWrite) {
        ret = SetBufferMessage(reply, bufferData);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USBD, "Set buffer message error length = %{public}d", ret);
        }
    }
    WRITE_PARCEL_WITH_RET(reply, Int32, ret, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerStub::DoSetActiveConfig(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HITRACE_METER_NAME(HITRACE_TAG_USB, "SetActiveConfig");
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t config = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, config, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(reply, Int32, SetActiveConfig(busNum, devAddr, config), UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerStub::DoGetActiveConfig(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t config = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = GetActiveConfig(busNum, devAddr, config);
    if (ret == UEC_OK) {
        WRITE_PARCEL_WITH_RET(reply, Uint8, config, UEC_SERVICE_WRITE_PARCEL_ERROR);
    }
    return ret;
}

int32_t UsbServerStub::DoSetInterface(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HITRACE_METER_NAME(HITRACE_TAG_USB, "SetInterface");
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t interfaceId = 0;
    uint8_t altIndex = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, altIndex, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(
        reply, Int32, SetInterface(busNum, devAddr, interfaceId, altIndex), UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerStub::DoGetRawDescriptor(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    std::vector<uint8_t> bufferData;
    int32_t ret = GetRawDescriptor(busNum, devAddr, bufferData);
    if (ret == UEC_OK) {
        ret = SetBufferMessage(reply, bufferData);
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USBD, "SetBufferMessage failed ret:%{public}d", ret);
        }
    } else {
        USB_HILOGW(MODULE_USBD, "GetRawDescriptor failed ret:%{public}d", ret);
        UsbReportSysEvent::ReportTransforFaultSysEvent("GetRawDescriptor", {busNum, devAddr}, {0, 0}, ret);
    }
    return ret;
}

int32_t UsbServerStub::DoGetFileDescriptor(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t fd = -1;
    int32_t ret = GetFileDescriptor(busNum, devAddr, fd);
    if (ret == UEC_OK) {
        if (!WriteFileDescriptor(reply, fd)) {
            USB_HILOGW(MODULE_USB_SERVICE, "%{public}s: write fd failed!", __func__);
            return UEC_INTERFACE_WRITE_PARCEL_ERROR;
        }
    } else {
        USB_HILOGE(MODULE_USBD, "ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbServerStub::DoRequestQueue(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t ifId = 0;
    uint8_t endpoint = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, ifId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, endpoint, UEC_SERVICE_WRITE_PARCEL_ERROR);
    std::vector<uint8_t> clientData;
    std::vector<uint8_t> bufferData;

    int32_t ret = UsbServerStub::GetBufferMessage(data, clientData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "GetBufferMessage failed  ret:%{public}d", ret);
        return ret;
    }
    ret = UsbServerStub::GetBufferMessage(data, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "GetBufferMessage failed  ret:%{public}d", ret);
        return ret;
    }
    const UsbDev tmpDev = {busNum, devAddr};
    const UsbPipe tmpPipe = {ifId, endpoint};
    ret = RequestQueue(tmpDev, tmpPipe, clientData, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "GetBufferMessage failed ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbServerStub::DoRequestWait(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    int32_t timeOut = 0;
    std::vector<uint8_t> clientData;
    std::vector<uint8_t> bufferData;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, timeOut, UEC_SERVICE_WRITE_PARCEL_ERROR);

    const UsbDev tmpDev = {busNum, devAddr};
    int32_t ret = RequestWait(tmpDev, timeOut, clientData, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "RequestWait failed ret:%{public}d", ret);
        return ret;
    }

    ret = SetBufferMessage(reply, clientData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "Set clientData failed ret:%{public}d", ret);
        return ret;
    }

    ret = SetBufferMessage(reply, bufferData);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "Set bufferData failed ret:%{public}d", ret);
        return ret;
    }
    return ret;
}

int32_t UsbServerStub::DoRequestCancel(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t interfaceId = 0;
    uint8_t endpointId = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = RequestCancel(busNum, devAddr, interfaceId, endpointId);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbServerStub::DoClose(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = Close(busNum, devAddr);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "failed ret:%{public}d", ret);
        UsbReportSysEvent::ReportTransforFaultSysEvent("CloseDevice", {busNum, devAddr}, {0, 0}, ret);
    }
    WRITE_PARCEL_WITH_RET(reply, Int32, ret, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerStub::DoGetDevices(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::vector<UsbDevice> deviceList;
    int32_t ret = GetDevices(deviceList);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_SERVICE, "GetDevices failed ret = %{public}d", ret);
        UsbReportSysEvent::ReportTransforFaultSysEvent("GetDevices", {0, 0}, {0, 0}, ret);
        return ret;
    }
    USB_HILOGI(MODULE_SERVICE, "list size = %{public}zu", deviceList.size());
    ret = SetDeviceListMessageParcel(deviceList, reply);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SetDeviceListMessageParcel failed ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbServerStub::SetAccessoryListMessageParcel(std::vector<USBAccessory> &accessoryList, MessageParcel &data)
{
    int32_t accessoryCount = (int32_t)accessoryList.size();
    WRITE_PARCEL_WITH_RET(data, Int32, accessoryCount, UEC_SERVICE_WRITE_PARCEL_ERROR);
    for (int32_t i = 0; i < accessoryCount; ++i) {
        int32_t ret = SetAccessoryMessageParcel(accessoryList[i], data);
        if (ret) {
            return ret;
        }
    }
    return UEC_OK;
}

int32_t UsbServerStub::SetAccessoryMessageParcel(USBAccessory &accessoryInfo, MessageParcel &data)
{
    WRITE_PARCEL_WITH_RET(data, String, accessoryInfo.GetManufacturer(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, String, accessoryInfo.GetProduct(), UEC_SERVICE_WRITE_PARCEL_ERROR);

    WRITE_PARCEL_WITH_RET(data, String, accessoryInfo.GetDescription(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, String, accessoryInfo.GetVersion(), UEC_SERVICE_WRITE_PARCEL_ERROR);

    WRITE_PARCEL_WITH_RET(data, String, accessoryInfo.GetSerialNumber(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerStub::SetDeviceListMessageParcel(std::vector<UsbDevice> &deviceList, MessageParcel &data)
{
    int32_t deviceCount = (int32_t)deviceList.size();
    WRITE_PARCEL_WITH_RET(data, Int32, deviceCount, UEC_SERVICE_WRITE_PARCEL_ERROR);
    for (int32_t i = 0; i < deviceCount; ++i) {
        UsbDevice &devInfo = deviceList[i];
        int32_t ret = SetDeviceMessageParcel(devInfo, data);
        if (ret) {
            return ret;
        }
    }
    return UEC_OK;
}

int32_t UsbServerStub::SetDeviceMessageParcel(UsbDevice &devInfo, MessageParcel &data)
{
    WRITE_PARCEL_WITH_RET(data, Int32, devInfo.GetBusNum(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, devInfo.GetDevAddr(), UEC_SERVICE_WRITE_PARCEL_ERROR);

    WRITE_PARCEL_WITH_RET(data, Int32, devInfo.GetVendorId(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, devInfo.GetProductId(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, devInfo.GetClass(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, devInfo.GetSubclass(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, devInfo.GetProtocol(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, devInfo.GetiManufacturer(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, devInfo.GetiProduct(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, devInfo.GetiSerialNumber(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, devInfo.GetbMaxPacketSize0(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint16, devInfo.GetbcdUSB(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint16, devInfo.GetbcdDevice(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, String16, Str8ToStr16(devInfo.GetName()), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, String16, Str8ToStr16(devInfo.GetManufacturerName()), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, String16, Str8ToStr16(devInfo.GetProductName()), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, String16, Str8ToStr16(devInfo.GetVersion()), UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, String16, Str8ToStr16(devInfo.GetmSerial()), UEC_SERVICE_WRITE_PARCEL_ERROR);

    USB_HILOGE(MODULE_USB_INNERKIT, "devInfo:%{public}s", devInfo.ToString().c_str());
    WRITE_PARCEL_WITH_RET(data, Int32, devInfo.GetConfigCount(), UEC_SERVICE_WRITE_PARCEL_ERROR);
    return SetDeviceConfigsMessageParcel(devInfo.GetConfigs(), data);
}

int32_t UsbServerStub::SetDeviceConfigsMessageParcel(std::vector<USBConfig> &configs, MessageParcel &data)
{
    for (auto it = configs.begin(); it != configs.end(); ++it) {
        USBConfig config = *it;
        WRITE_PARCEL_WITH_RET(data, Int32, config.GetId(), UEC_SERVICE_WRITE_PARCEL_ERROR);
        WRITE_PARCEL_WITH_RET(data, Uint32, config.GetAttributes(), UEC_SERVICE_WRITE_PARCEL_ERROR);
        WRITE_PARCEL_WITH_RET(data, Int32, config.GetMaxPower(), UEC_SERVICE_WRITE_PARCEL_ERROR);

        WRITE_PARCEL_WITH_RET(data, Uint8, config.GetiConfiguration(), UEC_SERVICE_WRITE_PARCEL_ERROR);
        WRITE_PARCEL_WITH_RET(data, String16, Str8ToStr16(config.GetName()), UEC_SERVICE_WRITE_PARCEL_ERROR);

        WRITE_PARCEL_WITH_RET(data, Uint32, config.GetInterfaceCount(), UEC_SERVICE_WRITE_PARCEL_ERROR);
        USB_HILOGI(MODULE_USB_SERVICE, "devInfo=%{public}s", config.ToString().c_str());
        int32_t ret = SetDeviceInterfacesMessageParcel(config.GetInterfaces(), data);
        if (ret) {
            return ret;
        }
    }
    return UEC_OK;
}

int32_t UsbServerStub::SetDeviceInterfacesMessageParcel(std::vector<UsbInterface> &interfaces, MessageParcel &data)
{
    for (auto it = interfaces.begin(); it != interfaces.end(); ++it) {
        UsbInterface interface = *it;
        WRITE_PARCEL_WITH_RET(data, Int32, interface.GetId(), UEC_SERVICE_WRITE_PARCEL_ERROR);
        WRITE_PARCEL_WITH_RET(data, Int32, interface.GetClass(), UEC_SERVICE_WRITE_PARCEL_ERROR);
        WRITE_PARCEL_WITH_RET(data, Int32, interface.GetSubClass(), UEC_SERVICE_WRITE_PARCEL_ERROR);
        WRITE_PARCEL_WITH_RET(data, Int32, interface.GetAlternateSetting(), UEC_SERVICE_WRITE_PARCEL_ERROR);
        WRITE_PARCEL_WITH_RET(data, Int32, interface.GetProtocol(), UEC_SERVICE_WRITE_PARCEL_ERROR);

        WRITE_PARCEL_WITH_RET(data, Uint8, interface.GetiInterface(), UEC_SERVICE_WRITE_PARCEL_ERROR);
        WRITE_PARCEL_WITH_RET(data, String16, Str8ToStr16(interface.GetName()), UEC_SERVICE_WRITE_PARCEL_ERROR);

        WRITE_PARCEL_WITH_RET(data, Int32, interface.GetEndpointCount(), UEC_SERVICE_WRITE_PARCEL_ERROR);
        USB_HILOGI(MODULE_USB_SERVICE, "interface=%{public}s", interface.ToString().c_str());
        int32_t ret = SetDeviceEndpointsMessageParcel(interface.GetEndpoints(), data);
        if (ret) {
            return ret;
        }
    }
    return UEC_OK;
}

int32_t UsbServerStub::SetDeviceEndpointsMessageParcel(std::vector<USBEndpoint> &eps, MessageParcel &data)
{
    for (auto it = eps.begin(); it != eps.end(); ++it) {
        USBEndpoint ep = *it;
        WRITE_PARCEL_WITH_RET(data, Uint32, ep.GetAddress(), UEC_SERVICE_WRITE_PARCEL_ERROR);
        WRITE_PARCEL_WITH_RET(data, Uint32, ep.GetAttributes(), UEC_SERVICE_WRITE_PARCEL_ERROR);
        WRITE_PARCEL_WITH_RET(data, Int32, ep.GetInterval(), UEC_SERVICE_WRITE_PARCEL_ERROR);
        WRITE_PARCEL_WITH_RET(data, Int32, ep.GetMaxPacketSize(), UEC_SERVICE_WRITE_PARCEL_ERROR);
        USB_HILOGI(MODULE_USB_SERVICE, "ep=%{public}s", ep.ToString().c_str());
    }
    return UEC_OK;
}

int32_t UsbServerStub::DoRegBulkCallback(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t interface = 0;
    uint8_t endpoint = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, interface, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, endpoint, UEC_SERVICE_WRITE_PARCEL_ERROR);
    const sptr<IRemoteObject> cb = data.ReadRemoteObject();
    const UsbDev tmpDev = {busNum, devAddr};
    const UsbPipe tmpPipe = {interface, endpoint};
    int32_t ret = RegBulkCallback(tmpDev, tmpPipe, cb);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "ret:%{public}d", ret);
        return ret;
    }
    return ret;
}

int32_t UsbServerStub::DoUnRegBulkCallback(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t interface = 0;
    uint8_t endpoint = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, interface, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, endpoint, UEC_SERVICE_WRITE_PARCEL_ERROR);
    const UsbDev tmpDev = {busNum, devAddr};
    const UsbPipe tmpPipe = {interface, endpoint};
    int32_t ret = UnRegBulkCallback(tmpDev, tmpPipe);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "ret:%{public}d", ret);
        return ret;
    }
    return ret;
}

int32_t UsbServerStub::DoBulkRead(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HITRACE_METER_NAME(HITRACE_TAG_USB, "BulkRead");
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t interface = 0;
    uint8_t endpoint = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, interface, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, endpoint, UEC_SERVICE_WRITE_PARCEL_ERROR);
    sptr<Ashmem> ashmem = data.ReadAshmem();
    const UsbDev tmpDev = {busNum, devAddr};
    const UsbPipe tmpPipe = {interface, endpoint};
    int32_t ret = BulkRead(tmpDev, tmpPipe, ashmem);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "BulkRead failed ret:%{public}d", ret);
        UsbReportSysEvent::ReportTransforFaultSysEvent("BulkRead", tmpDev, tmpPipe, ret);
        return ret;
    }
    return ret;
}

int32_t UsbServerStub::DoBulkWrite(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HITRACE_METER_NAME(HITRACE_TAG_USB, "BulkWrite");
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t interface = 0;
    uint8_t endpoint = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, interface, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, endpoint, UEC_SERVICE_WRITE_PARCEL_ERROR);
    sptr<Ashmem> ashmem = data.ReadAshmem();
    const UsbDev tmpDev = {busNum, devAddr};
    const UsbPipe tmpPipe = {interface, endpoint};
    int32_t ret = BulkWrite(tmpDev, tmpPipe, ashmem);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "ret:%{public}d", ret);
        UsbReportSysEvent::ReportTransforFaultSysEvent("BulkWrite", tmpDev, tmpPipe, ret);
        return ret;
    }
    return ret;
}

int32_t UsbServerStub::DoBulkCancel(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t interface = 0;
    uint8_t endpoint = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, interface, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, endpoint, UEC_SERVICE_WRITE_PARCEL_ERROR);
    const UsbDev tmpDev = {busNum, devAddr};
    const UsbPipe tmpPipe = {interface, endpoint};
    int32_t ret = BulkCancel(tmpDev, tmpPipe);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "ret:%{public}d", ret);
        return ret;
    }
    return ret;
}

int32_t UsbServerStub::DoAddRight(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::string bundleName;
    std::string deviceName;
    READ_PARCEL_WITH_RET(data, String, bundleName, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, String, deviceName, UEC_SERVICE_READ_PARCEL_ERROR);
    int32_t ret = AddRight(bundleName, deviceName);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbServerStub::DoAddAccessRight(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::string tokenId;
    std::string deviceName;
    READ_PARCEL_WITH_RET(data, String, tokenId, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, String, deviceName, UEC_SERVICE_READ_PARCEL_ERROR);
    int32_t ret = AddAccessRight(tokenId, deviceName);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbServerStub::DoManageGlobalInterface(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    bool disable = false;
    READ_PARCEL_WITH_RET(data, Bool, disable, UEC_SERVICE_READ_PARCEL_ERROR);
    int32_t ret = ManageGlobalInterface(disable);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbServerStub::DoManageDevice(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t vendorId = 0;
    int32_t productId = 0;
    bool disable = false;
    READ_PARCEL_WITH_RET(data, Int32, vendorId, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Int32, productId, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Bool, disable, UEC_SERVICE_READ_PARCEL_ERROR);
    int32_t ret = ManageDevice(vendorId, productId, disable);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbServerStub::DoManageInterfaceType(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t count;
    READ_PARCEL_WITH_RET(data, Int32, count, UEC_SERVICE_READ_PARCEL_ERROR);
    if (count > MAX_EDM_LIST_SIZE || count < 0) {
        USB_HILOGE(MODULE_USBD, "EDM list size count:%{public}d", count);
        return UEC_SERVICE_READ_PARCEL_ERROR;
    }
    bool disable = false;
    std::vector<UsbDeviceType> disableType;
    for (int32_t i = 0; i < count; ++i) {
        UsbDeviceType usbDeviceType;
        READ_PARCEL_WITH_RET(data, Int32, usbDeviceType.baseClass, UEC_SERVICE_READ_PARCEL_ERROR);
        READ_PARCEL_WITH_RET(data, Int32, usbDeviceType.subClass, UEC_SERVICE_READ_PARCEL_ERROR);
        READ_PARCEL_WITH_RET(data, Int32, usbDeviceType.protocol, UEC_SERVICE_READ_PARCEL_ERROR);
        READ_PARCEL_WITH_RET(data, Bool, usbDeviceType.isDeviceType, UEC_SERVICE_READ_PARCEL_ERROR);
        disableType.emplace_back(usbDeviceType);
    }
    READ_PARCEL_WITH_RET(data, Bool, disable, UEC_SERVICE_READ_PARCEL_ERROR);
    int32_t ret = ManageInterfaceType(disableType, disable);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbServerStub::DoClearHalt(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HITRACE_METER_NAME(HITRACE_TAG_USB, "ClearHalt");
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t interfaceId = 0;
    uint8_t endpointId = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = ClearHalt(busNum, devAddr, interfaceId, endpointId);
    WRITE_PARCEL_WITH_RET(reply, Int32, ret, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerStub::DoGetInterfaceActiveStatus(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    uint8_t interfaceId = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    bool unactivated;
    int32_t ret = GetInterfaceActiveStatus(busNum, devAddr, interfaceId, unactivated);
    if (ret == UEC_OK) {
        WRITE_PARCEL_WITH_RET(reply, Bool, unactivated, UEC_SERVICE_WRITE_PARCEL_ERROR);
    } else {
        USB_HILOGE(MODULE_USBD, "ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbServerStub::DoGetDeviceSpeed(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    uint8_t busNum = 0;
    uint8_t devAddr = 0;
    READ_PARCEL_WITH_RET(data, Uint8, busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(data, Uint8, devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    uint8_t speed;
    int32_t ret = GetDeviceSpeed(busNum, devAddr, speed);
    if (ret == UEC_OK) {
        WRITE_PARCEL_WITH_RET(reply, Uint8, speed, UEC_SERVICE_WRITE_PARCEL_ERROR);
    } else {
        USB_HILOGE(MODULE_USBD, "ret:%{public}d", ret);
    }
    USB_HILOGE(MODULE_USBD, "DoGetDeviceSpeed speed:%{public}u", speed);
    return ret;
}

int32_t UsbServerStub::DoGetAccessoryList(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::vector<USBAccessory> accessoryList;
    int32_t ret = GetAccessoryList(accessoryList);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_SERVICE, "GetDevices failed ret = %{public}d", ret);
        UsbReportSysEvent::ReportTransforFaultSysEvent("GetAccessoryList", {0, 0}, {0, 0}, ret);
        return ret;
    }
    USB_HILOGI(MODULE_SERVICE, "accessory list size = %{public}zu", accessoryList.size());
    ret = SetAccessoryListMessageParcel(accessoryList, reply);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "SetDeviceListMessageParcel failed ret:%{public}d", ret);
    }
    return ret;
}
int32_t UsbServerStub::GetAccessoryMessageParcel(MessageParcel &data, USBAccessory &accessoryInfo)
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
int32_t UsbServerStub::DoOpenAccessory(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    USBAccessory accessory;
    int32_t ret = GetAccessoryMessageParcel(data, accessory);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_SERVICE, "GetDevices failed ret = %{public}d", ret);
        UsbReportSysEvent::ReportTransforFaultSysEvent("GetAccessoryList", {0, 0}, {0, 0}, ret);
        return ret;
    }
    int32_t fd = -1;
    ret = OpenAccessory(accessory, fd);
    if (ret == UEC_OK) {
        if (!WriteFileDescriptor(reply, fd)) {
            USB_HILOGW(MODULE_USB_SERVICE, "%{public}s: write fd failed!", __func__);
            return UEC_INTERFACE_WRITE_PARCEL_ERROR;
        }
    } else {
        USB_HILOGE(MODULE_USBD, "OpenAccessory ret:%{public}d", ret);
    }
    return ret;
}

int32_t UsbServerStub::DoCloseAccessory(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t fd = -1;
    READ_PARCEL_WITH_RET(data, Int32, fd, UEC_SERVICE_READ_PARCEL_ERROR);
    int32_t ret = CloseAccessory(fd);
    if (ret == UEC_OK) {
        USB_HILOGE(MODULE_USBD, "%{public}s, ret:%{public}d", __func__, ret);
    }
    WRITE_PARCEL_WITH_RET(reply, Int32, ret, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerStub::DoAddAccessoryRight(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    uint32_t tokenId;
    READ_PARCEL_WITH_RET(data, Uint32, tokenId, UEC_SERVICE_READ_PARCEL_ERROR);
    USBAccessory accessory;
    int32_t ret = GetAccessoryMessageParcel(data, accessory);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_SERVICE, "GetAccessory failed ret = %{public}d", ret);
        return ret;
    }
    ret = AddAccessoryRight(tokenId, accessory);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "%{public}s, ret:%{public}d", __func__, ret);
    }
    WRITE_PARCEL_WITH_RET(reply, Int32, ret, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerStub::DoHasAccessoryRight(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    USBAccessory accessory;
    int32_t ret = GetAccessoryMessageParcel(data, accessory);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_SERVICE, "GetAccessory failed ret = %{public}d", ret);
        return ret;
    }
    bool result = false;
    ret = HasAccessoryRight(accessory, result);
    if (!ret) {
        USB_HILOGE(MODULE_USBD, "%{public}s, ret:%{public}d", __func__, result);
    }
    WRITE_PARCEL_WITH_RET(reply, Bool, result, false);
    WRITE_PARCEL_WITH_RET(reply, Int32, ret, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbServerStub::DoRequestAccessoryRight(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    USBAccessory accessory;
    int32_t ret = GetAccessoryMessageParcel(data, accessory);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_SERVICE, "GetAccessory failed ret = %{public}d", ret);
        return ret;
    }
    bool result = false;
    ret = RequestAccessoryRight(accessory, result);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "%{public}s, ret:%{public}d", __func__, ret);
    }
    WRITE_PARCEL_WITH_RET(reply, Bool, result, false);
    WRITE_PARCEL_WITH_RET(reply, Int32, ret, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return ret;
}

int32_t UsbServerStub::DoCancelAccessoryRight(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    USBAccessory accessory;
    int32_t ret = GetAccessoryMessageParcel(data, accessory);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_SERVICE, "GetAccessory failed ret = %{public}d", ret);
        return ret;
    }
    ret = CancelAccessoryRight(accessory);
    if (ret != UEC_OK) {
        USB_HILOGE(MODULE_USBD, "%{public}s, ret:%{public}d", __func__, ret);
    }
    WRITE_PARCEL_WITH_RET(reply, Int32, ret, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return ret;
}
} // namespace USB
} // namespace OHOS
