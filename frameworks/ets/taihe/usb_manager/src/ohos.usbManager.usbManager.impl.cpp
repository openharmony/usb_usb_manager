/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "ohos.usbManager.usbManager.proj.hpp"
#include "ohos.usbManager.usbManager.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"
#include "usb_srv_client.h"

using namespace taihe;
using namespace ohos::usbManager::usbManager;

namespace {
constexpr int32_t CAPACITY_NOT_SUPPORT = 801;

static OHOS::USB::UsbSrvClient &g_usbClient = OHOS::USB::UsbSrvClient::GetInstance();

enum UsbManagerFeature {
    FEATURE_HOST = 0,
    FEATURE_DEVICE = 1,
    FEATURE_PORT = 2,
};

static bool HasFeature(UsbManagerFeature feature)
{
    switch (feature) {
        case FEATURE_HOST:
#ifndef USB_MANAGER_FEATURE_HOST
            return false;
#else
            return true;
#endif // USB_MANAGER_FEATURE_HOST
        case FEATURE_DEVICE:
#ifndef USB_MANAGER_FEATURE_DEVICE
            return false;
#else
            return true;
#endif // USB_MANAGER_FEATURE_DEVICE
        case FEATURE_PORT:
#ifndef USB_MANAGER_FEATURE_PORT
            return false;
#else
            return true;
#endif // USB_MANAGER_FEATURE_PORT
        default:;
    }
    return false;
}

static ohos::usbManager::usbManager::USBEndpoint ParseToUSBEndpoint(const OHOS::USB::USBEndpoint &usbEndpoint)
{
    auto directionKey = usbEndpoint.GetDirection() == OHOS::USB::USB_ENDPOINT_DIR_OUT
                            ? ohos::usbManager::usbManager::USBRequestDirection::key_t::USB_REQUEST_DIR_TO_DEVICE
                            : ohos::usbManager::usbManager::USBRequestDirection::key_t::USB_REQUEST_DIR_FROM_DEVICE;
    return {
        .address = usbEndpoint.GetAddress(),
        .attributes = usbEndpoint.GetAttributes(),
        .interval = usbEndpoint.GetInterval(),
        .maxPacketSize = usbEndpoint.GetMaxPacketSize(),
        .direction = ohos::usbManager::usbManager::USBRequestDirection(directionKey),
        .endpointNumber = usbEndpoint.GetEndpointNumber(),
        .type = usbEndpoint.GetType(),
        .interfaceId = usbEndpoint.GetInterfaceId()
    };
}

static ohos::usbManager::usbManager::USBInterface ParseToUSBInterface(OHOS::USB::UsbInterface &usbInterface)
{
    std::vector<ohos::usbManager::usbManager::USBEndpoint> endpoints;
    for (const auto &endpoint : usbInterface.GetEndpoints()) {
        endpoints.push_back(ParseToUSBEndpoint(endpoint));
    }
    return {
        .id = usbInterface.GetId(),
        .protocol = usbInterface.GetProtocol(),
        .clazz = usbInterface.GetClass(),
        .subClass = usbInterface.GetSubClass(),
        .alternateSetting = usbInterface.GetAlternateSetting(),
        .name = usbInterface.GetName(),
        .endpoints = array<ohos::usbManager::usbManager::USBEndpoint>(endpoints)
    };
}

static ohos::usbManager::usbManager::USBConfiguration ParseToUSBConfiguration(OHOS::USB::USBConfig &usbConfig)
{
    std::vector<ohos::usbManager::usbManager::USBInterface> interfaces;
    for (auto &interface : usbConfig.GetInterfaces()) {
        interfaces.push_back(ParseToUSBInterface(interface));
    }
    return {
        .id = usbConfig.GetId(),
        .attributes = usbConfig.GetAttributes(),
        .maxPower = usbConfig.GetMaxPower(),
        .name = usbConfig.GetName(),
        .isRemoteWakeup = usbConfig.IsRemoteWakeup(),
        .isSelfPowered = usbConfig.IsSelfPowered(),
        .interfaces = array<ohos::usbManager::usbManager::USBInterface>(interfaces)
    };
}

static ohos::usbManager::usbManager::USBDevice ParseToUSBDevice(OHOS::USB::UsbDevice &usbDevice)
{
    std::vector<ohos::usbManager::usbManager::USBConfiguration> configs;
    for (auto &config : usbDevice.GetConfigs()) {
        configs.push_back(ParseToUSBConfiguration(config));
    }
    return {
        .busNum = usbDevice.GetBusNum(),
        .devAddress = usbDevice.GetDevAddr(),
        .serial = usbDevice.GetmSerial(),
        .name = usbDevice.GetName(),
        .manufacturerName = usbDevice.GetManufacturerName(),
        .productName = usbDevice.GetProductName(),
        .version = usbDevice.GetVersion(),
        .vendorId = usbDevice.GetVendorId(),
        .productId = usbDevice.GetProductId(),
        .clazz = usbDevice.GetClass(),
        .subClass = usbDevice.GetSubclass(),
        .protocol = usbDevice.GetProtocol(),
        .configs = array<ohos::usbManager::usbManager::USBConfiguration>(configs)
    };
}

array<ohos::usbManager::usbManager::USBDevice> getDevices()
{
    std::vector<ohos::usbManager::usbManager::USBDevice> res;
    if (!HasFeature(FEATURE_HOST)) {
        set_business_error(CAPACITY_NOT_SUPPORT, "");
        return array<ohos::usbManager::usbManager::USBDevice>(res);
    }
    std::vector<OHOS::USB::UsbDevice> deviceList;
    auto ret = g_usbClient.GetDevices(deviceList);
    if (ret != 0) {
        return array<ohos::usbManager::usbManager::USBDevice>(res);
    }
    for (auto &usbDevice : deviceList) {
        res.push_back(ParseToUSBDevice(usbDevice));
    }
    return array<ohos::usbManager::usbManager::USBDevice>(res);
}
} // namespace

TH_EXPORT_CPP_API_getDevices(getDevices);
