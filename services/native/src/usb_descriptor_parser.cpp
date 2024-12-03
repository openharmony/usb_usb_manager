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
        USB_HILOGE(MODULE_USB_SERVICE, "buffer is null or length is zero");
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

static int32_t AddConfig(std::vector<USBConfig> &configs, const UsbdConfigDescriptor *configDescriptor)
{
    if (configDescriptor == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "configDescriptor is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    USBConfig config;
    config.SetId(configDescriptor->bConfigurationValue);
    config.SetAttribute(configDescriptor->bmAttributes);
    config.SetMaxPower(configDescriptor->bMaxPower);
    config.SetiConfiguration(configDescriptor->iConfiguration);
    configs.emplace_back(config);
    USB_HILOGD(MODULE_USB_SERVICE, "add config, interfaces=%{public}u", configDescriptor->bNumInterfaces);
    return UEC_OK;
}

static int32_t AddInterface(std::vector<USBConfig> &configs, const UsbdInterfaceDescriptor *interfaceDescriptor)
{
    if (interfaceDescriptor == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "interfaceDescriptor is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    if (configs.empty()) {
        USB_HILOGE(MODULE_USB_SERVICE, "config descriptor not found");
        return UEC_SERVICE_INVALID_VALUE;
    }
    if ( configs.back().GetInterfaces().empty()) {
        USB_HILOGE(MODULE_USB_SERVICE, "interface list is empty for the last USBConfig");
        return UEC_SERVICE_INVALID_VALUE;
    }
    UsbInterface interface;
    interface.SetId(interfaceDescriptor->bInterfaceNumber);
    interface.SetProtocol(interfaceDescriptor->bInterfaceProtocol);
    interface.SetAlternateSetting(interfaceDescriptor->bAlternateSetting);
    interface.SetClass(interfaceDescriptor->bInterfaceClass);
    interface.SetSubClass(interfaceDescriptor->bInterfaceSubClass);
    interface.SetiInterface(interfaceDescriptor->iInterface);
    configs.back().GetInterfaces().emplace_back(interface);
    USB_HILOGD(MODULE_USB_SERVICE, "add interface, endpoints=%{public}u", interfaceDescriptor->bNumEndpoints);
    return UEC_OK;
}

static int32_t AddEndpoint(std::vector<USBConfig> &configs, const UsbdEndpointDescriptor *endpointDescriptor)
{
    if (endpointDescriptor == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "endpointDescriptor is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }
    if (configs.empty() || configs.back().GetInterfaces().empty()) {
        USB_HILOGE(MODULE_USB_SERVICE, "interface descriptor not found");
        return UEC_SERVICE_INVALID_VALUE;
    }

    USBEndpoint endpoint;
    endpoint.SetAddr(endpointDescriptor->bEndpointAddress);
    endpoint.SetAttr(endpointDescriptor->bmAttributes);
    endpoint.SetInterval(endpointDescriptor->bInterval);
    endpoint.SetMaxPacketSize(endpointDescriptor->wMaxPacketSize);
    endpoint.SetInterfaceId(configs.back().GetInterfaces().back().GetId());
    configs.back().GetInterfaces().back().GetEndpoints().emplace_back(endpoint);
    USB_HILOGD(MODULE_USB_SERVICE, "add endpoint, address=%{public}u", endpointDescriptor->bEndpointAddress);
    return UEC_OK;
}

int32_t UsbDescriptorParser::ParseConfigDescriptors(std::vector<uint8_t> &descriptor, uint32_t offset,
    std::vector<USBConfig> &configs)
{
    uint8_t *buffer = descriptor.data();
    uint32_t length = descriptor.size();
    uint32_t cursor = offset;
    int32_t ret = UEC_OK;

    while (cursor < length) {
        if ((length - cursor) < sizeof(UsbdDescriptorHeader)) {
            USB_HILOGW(MODULE_USB_SERVICE, "invalid desc data, length=%{public}u, cursor=%{public}u", length, cursor);
            break;
        }
        UsbdDescriptorHeader descriptorHeader = *(reinterpret_cast<const UsbdDescriptorHeader *>(buffer + cursor));
        if (descriptorHeader.bLength > (length - cursor)) {
            USB_HILOGW(MODULE_USB_SERVICE, "invalid data length, length=%{public}u, cursor=%{public}u", length, cursor);
            break;
        }
        switch (descriptorHeader.bDescriptorType) {
            case static_cast<uint8_t>(DescriptorType::DESCRIPTOR_TYPE_CONFIG):
                if (descriptorHeader.bLength != sizeof(UsbdConfigDescriptor)) {
                    USB_HILOGE(MODULE_USB_SERVICE, "invalid config, length=%{public}u", descriptorHeader.bLength);
                    return UEC_SERVICE_INVALID_VALUE;
                }
                ret = AddConfig(configs, reinterpret_cast<const UsbdConfigDescriptor *>(buffer + cursor));
                break;
            case static_cast<uint8_t>(DescriptorType::DESCRIPTOR_TYPE_INTERFACE):
                if (descriptorHeader.bLength != sizeof(UsbdInterfaceDescriptor)) {
                    USB_HILOGE(MODULE_USB_SERVICE, "invalid interface, length=%{public}u", descriptorHeader.bLength);
                    return UEC_SERVICE_INVALID_VALUE;
                }
                ret = AddInterface(configs, reinterpret_cast<const UsbdInterfaceDescriptor *>(buffer + cursor));
                break;
            case static_cast<uint8_t>(DescriptorType::DESCRIPTOR_TYPE_ENDPOINT):
                if (descriptorHeader.bLength != NORMAL_ENDPOINT_DESCRIPTOR
                    && descriptorHeader.bLength != AUDIO_ENDPOINT_DESCRIPTOR) {
                    USB_HILOGE(MODULE_USB_SERVICE, "invalid endpoint, length=%{public}u", descriptorHeader.bLength);
                    return UEC_SERVICE_INVALID_VALUE;
                }
                ret = AddEndpoint(configs, reinterpret_cast<const UsbdEndpointDescriptor *>(buffer + cursor));
                break;
            default:
                USB_HILOGW(MODULE_USB_SERVICE, "unrecognized type=%{public}d", descriptorHeader.bDescriptorType);
                break;
        }
        if (ret != UEC_OK) {
            return ret;
        }
        cursor += descriptorHeader.bLength;
    }
    return ret;
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
    if (length < configDescriptorSize || configDescriptor.bLength != configDescriptorSize) {
        USB_HILOGE(MODULE_USB_SERVICE, "size error length=%{public}u, configDescriptor.bLength=%{public}d",
            length, configDescriptor.bLength);
        return UEC_SERVICE_INVALID_VALUE;
    }
    cursor += configDescriptorSize;

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
        if (ret != UEC_OK) {
            USB_HILOGE(MODULE_USB_SERVICE, "ParseInterfaceDescriptor failed, ret=%{public}d", ret);
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
    while (cursor < length) {
        if (descriptorHeaderSize >= length - cursor) {
            USB_HILOGE(MODULE_USB_SERVICE, "length error");
            return UEC_SERVICE_INVALID_VALUE;
        }
        UsbdDescriptorHeader descriptorHeader = *(reinterpret_cast<const UsbdDescriptorHeader *>(buffer + cursor));
        if (descriptorHeader.bLength >= descriptorHeaderSize) {
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

    if (length - cursor < sizeof(UsbdInterfaceDescriptor)) {
        return UEC_SERVICE_INVALID_VALUE;
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
    while (cursor < length) {
        if (descriptorHeaderSize >= length - cursor) {
            USB_HILOGE(MODULE_USB_SERVICE, "length error");
            return UEC_SERVICE_INVALID_VALUE;
        }
        UsbdDescriptorHeader descriptorHeader = *(reinterpret_cast<const UsbdDescriptorHeader *>(buffer + cursor));
        if (descriptorHeader.bLength >= descriptorHeaderSize) {
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

    if (length - cursor < sizeof(UsbdInterfaceDescriptor)) {
        USB_HILOGE(MODULE_USB_SERVICE, "insufficient length to parse interface descriptor");
        return UEC_SERVICE_INVALID_VALUE;
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
