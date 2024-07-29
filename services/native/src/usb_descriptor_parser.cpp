/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "usb_descriptor_parser.h"
#include "hilog_wrapper.h"
#include "message_parcel.h"
#include "securec.h"
#include "usb_config.h"
#include "usb_endpoint.h"
#include "usb_errors.h"
#include "usb_interface.h"
#include "usbd_type.h"

static constexpr uint8_t NORMAL_ENDPOINT_DESCRIPTOR = 7;
static constexpr uint8_t AUDIO_ENDPOINT_DESCRIPTOR = 9;
namespace OHOS {
namespace USB {
enum class DescriptorType {
    DESCRIPTOR_TYPE_DEVICE = 1,
    DESCRIPTOR_TYPE_CONFIG = 2,
    DESCRIPTOR_TYPE_INTERFACE = 4,
    DESCRIPTOR_TYPE_ENDPOINT = 5
};

UsbDescriptorParser::UsbDescriptorParser() {}

UsbDescriptorParser::~UsbDescriptorParser() {}

int32_t UsbDescriptorParser::ParseDeviceDescriptor(const uint8_t *buffer, uint32_t length, UsbDevice &dev)
{
    if (buffer == nullptr || length == 0) {
        USB_HILOGE(MODULE_USB_SERVICE, "buffer is null");
        return UEC_SERVICE_INVALID_VALUE;
    }

    USB_HILOGD(MODULE_USB_SERVICE, "parse begin length=%{public}u", length);
    uint32_t deviceDescriptorSize = sizeof(UsbdDeviceDescriptor);
    if (length < deviceDescriptorSize) {
        USB_HILOGE(MODULE_USB_SERVICE, "buffer size error");
        return UEC_SERVICE_INVALID_VALUE;
    }

    UsbdDeviceDescriptor deviceDescriptor = *(reinterpret_cast<const UsbdDeviceDescriptor *>(buffer));
    if (deviceDescriptor.bLength != deviceDescriptorSize) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbdDeviceDescriptor size error");
        return UEC_SERVICE_INVALID_VALUE;
    }

    dev.SetVendorId(deviceDescriptor.idVendor);
    dev.SetProductId(deviceDescriptor.idProduct);
    dev.SetClass(deviceDescriptor.bDeviceClass);
    dev.SetSubclass(deviceDescriptor.bDeviceSubClass);
    dev.SetDescConfigCount(deviceDescriptor.bNumConfigurations);

    dev.SetbMaxPacketSize0(deviceDescriptor.bMaxPacketSize0);
    dev.SetbcdDevice(deviceDescriptor.bcdDevice);
    dev.SetbcdUSB(deviceDescriptor.bcdUSB);
    dev.SetiManufacturer(deviceDescriptor.iManufacturer);
    dev.SetiProduct(deviceDescriptor.iProduct);
    dev.SetiSerialNumber(deviceDescriptor.iSerialNumber);
    return UEC_OK;
}

int32_t UsbDescriptorParser::ParseConfigDescriptor(
    const uint8_t *buffer, uint32_t length, uint32_t &cursor, USBConfig &config)
{
    if (buffer == nullptr || length == 0) {
        return UEC_SERVICE_INVALID_VALUE;
    }

    USB_HILOGD(MODULE_USB_SERVICE, "parse begin length=%{public}u, cursor=%{public}u", length, cursor);
    uint32_t configDescriptorSize = sizeof(UsbdConfigDescriptor);
    UsbdConfigDescriptor configDescriptor = *(reinterpret_cast<const UsbdConfigDescriptor *>(buffer));
    cursor += configDescriptorSize;
    if (length < configDescriptorSize || configDescriptor.bLength != configDescriptorSize) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbdDeviceDescriptor size error or buffer size error");
        return UEC_SERVICE_INVALID_VALUE;
    }

    config.SetId(configDescriptor.bConfigurationValue);
    config.SetAttribute(configDescriptor.bmAttributes);
    config.SetMaxPower(configDescriptor.bMaxPower);
    config.SetiConfiguration(configDescriptor.iConfiguration);

    std::vector<UsbInterface> interfaces;
    for (int32_t i = 0; (i < configDescriptor.bNumInterfaces) && (cursor < length); ++i) {
        uint32_t interfaceCursor = 0;
        UsbInterface interface;
        int32_t ret = ParseInterfaceDescriptor(
            buffer + cursor + interfaceCursor, length - cursor - interfaceCursor, interfaceCursor, interface);
        if (ret == UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ParseInterfaceDescriptor failed");
            return UEC_SERVICE_INVALID_VALUE;
        }
        bool isRepeat = false;
        auto iter = interfaces.begin();
        while (iter != interfaces.end()) {
            if (iter->GetId() == interface.GetId()) {
                isRepeat = true;
                break;
            }
            iter++;
        }
        if (interface.GetEndpointCount() >= 0 && !isRepeat) {
            interfaces.push_back(interface);
        } else {
            // retry
            if (interface.GetEndpointCount() > 0 && iter != interfaces.end()) {
                USB_HILOGE(MODULE_USB_SERVICE, "get repeat interface id info, and has endpoints");
                *iter = interface;
            }
            --i;
        }
        cursor += interfaceCursor;
    }
    config.SetInterfaces(interfaces);
    return UEC_OK;
}

int32_t UsbDescriptorParser::ParseInterfaceDescriptor(
    const uint8_t *buffer, uint32_t length, uint32_t &cursor, UsbInterface &interface)
{
    if (buffer == nullptr || length == 0) {
        return UEC_SERVICE_INVALID_VALUE;
    }

    uint32_t descriptorHeaderSize = sizeof(UsbdDescriptorHeader);
    while (static_cast<uint32_t>(cursor) < length) {
        if (descriptorHeaderSize >= length) {
            USB_HILOGE(MODULE_USB_SERVICE, "length error");
            return UEC_SERVICE_INVALID_VALUE;
        }
        UsbdDescriptorHeader descriptorHeader = *(reinterpret_cast<const UsbdDescriptorHeader *>(buffer + cursor));
        if (descriptorHeader.bLength > length) {
            USB_HILOGE(MODULE_USB_SERVICE, "descriptor size error");
            return UEC_SERVICE_INVALID_VALUE;
        }
        if (descriptorHeader.bDescriptorType == static_cast<uint8_t>(DescriptorType::DESCRIPTOR_TYPE_INTERFACE)) {
            break;
        }
        cursor += descriptorHeader.bLength;
        USB_HILOGD(MODULE_USB_SERVICE, "type = %{public}d, length=%{public}d", descriptorHeader.bDescriptorType,
            descriptorHeader.bLength);
    }

    UsbdInterfaceDescriptor interfaceDescriptor = *(reinterpret_cast<const UsbdInterfaceDescriptor *>(buffer + cursor));
    if (interfaceDescriptor.bLength != sizeof(UsbdInterfaceDescriptor)) {
        USB_HILOGE(MODULE_USB_SERVICE, "UsbdInterfaceDescriptor size error");
        return UEC_SERVICE_INVALID_VALUE;
    }
    cursor += interfaceDescriptor.bLength;

    interface.SetId(interfaceDescriptor.bInterfaceNumber);
    interface.SetProtocol(interfaceDescriptor.bInterfaceProtocol);
    interface.SetAlternateSetting(interfaceDescriptor.bAlternateSetting);
    interface.SetClass(interfaceDescriptor.bInterfaceClass);
    interface.SetSubClass(interfaceDescriptor.bInterfaceSubClass);
    interface.SetiInterface(interfaceDescriptor.iInterface);

    std::vector<USBEndpoint> eps;
    for (int32_t j = 0; j < interfaceDescriptor.bNumEndpoints; ++j) {
        uint32_t epCursor = 0;
        USBEndpoint ep;
        ParseEndpointDescriptor(buffer + cursor + epCursor, length - cursor - epCursor, epCursor, ep);
        ep.SetInterfaceId(interfaceDescriptor.bInterfaceNumber);
        eps.push_back(ep);
        cursor += epCursor;
    }
    interface.SetEndpoints(eps);
    USB_HILOGD(MODULE_USB_SERVICE, "interface to string : %{public}s", interface.ToString().c_str());
    return UEC_OK;
}

int32_t UsbDescriptorParser::ParseEndpointDescriptor(
    const uint8_t *buffer, uint32_t length, uint32_t &cursor, USBEndpoint &ep)
{
    USB_HILOGD(MODULE_USB_SERVICE, "parse begin, length=%{public}u, cursor=%{public}u", length, cursor);
    if (buffer == nullptr || length == 0) {
        return UEC_SERVICE_INVALID_VALUE;
    }

    uint32_t descriptorHeaderSize = sizeof(UsbdDescriptorHeader);
    while (static_cast<uint32_t>(cursor) < length) {
        if (descriptorHeaderSize >= length) {
            USB_HILOGE(MODULE_USB_SERVICE, "length error");
            return UEC_SERVICE_INVALID_VALUE;
        }
        UsbdDescriptorHeader descriptorHeader = *(reinterpret_cast<const UsbdDescriptorHeader *>(buffer + cursor));
        if (descriptorHeader.bLength > length) {
            USB_HILOGE(MODULE_USB_SERVICE, "descriptor size error");
            return UEC_SERVICE_INVALID_VALUE;
        }
        if (descriptorHeader.bDescriptorType == static_cast<uint8_t>(DescriptorType::DESCRIPTOR_TYPE_ENDPOINT)) {
            break;
        }
        cursor += descriptorHeader.bLength;
        USB_HILOGD(MODULE_USB_SERVICE, "error type = %{public}d, length=%{public}d", descriptorHeader.bDescriptorType,
            descriptorHeader.bLength);
    }

    UsbdEndpointDescriptor endpointDescriptor = *(reinterpret_cast<const UsbdEndpointDescriptor *>(buffer + cursor));
    if (endpointDescriptor.bLength != NORMAL_ENDPOINT_DESCRIPTOR &&
        endpointDescriptor.bLength != AUDIO_ENDPOINT_DESCRIPTOR) {
        USB_HILOGE(MODULE_USB_SERVICE, "Endpoint descriptor size error, length=%{public}d", endpointDescriptor.bLength);
        return UEC_SERVICE_INVALID_VALUE;
    }
    cursor += endpointDescriptor.bLength;

    ep.SetAddr(endpointDescriptor.bEndpointAddress);
    ep.SetAttr(endpointDescriptor.bmAttributes);
    ep.SetInterval(endpointDescriptor.bInterval);
    ep.SetMaxPacketSize(endpointDescriptor.wMaxPacketSize);
    return UEC_OK;
}
} // namespace USB
} // namespace OHOS
