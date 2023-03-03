/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "usb_impl_mock.h"
#include "if_system_ability_manager.h"
#include "iproxy_broker.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "system_ability_status_change_stub.h"

namespace OHOS {
namespace USB {
using namespace OHOS;
using namespace OHOS::HDI;

std::vector<uint8_t> g_descBuf {
    0x12, 0x01, 0x20, 0x03, 0x00, 0x00, 0x00, 0x09, 0x07, 0x22, 0x18, 0x00, 0x23, 0x02, 0x01, 0x02, 0x03, 0x01, 0x09,
    0x02, 0x5D, 0x00, 0x02, 0x01, 0x04, 0xC0, 0x3E, 0x08, 0x0B, 0x00, 0x02, 0x02, 0x02, 0x01, 0x07, 0x09, 0x04, 0x00,
    0x00, 0x01, 0x02, 0x02, 0x01, 0x05, 0x05, 0x24, 0x00, 0x10, 0x01, 0x05, 0x24, 0x01, 0x00, 0x01, 0x04, 0x24, 0x02,
    0x02, 0x05, 0x24, 0x06, 0x00, 0x01, 0x07, 0x05, 0x81, 0x03, 0x0A, 0x00, 0x09, 0x06, 0x30, 0x00, 0x00, 0x00, 0x00,
    0x09, 0x04, 0x01, 0x00, 0x02, 0x0A, 0x00, 0x02, 0x06, 0x07, 0x05, 0x82, 0x02, 0x00, 0x04, 0x00, 0x06, 0x30, 0x00,
    0x00, 0x00, 0x00, 0x07, 0x05, 0x01, 0x02, 0x00, 0x04, 0x00, 0x06, 0x30, 0x00, 0x00, 0x00, 0x00
};

MockUsbImpl::MockUsbImpl() {}

MockUsbImpl::~MockUsbImpl() {}

UsbDevice MockUsbImpl::FindDeviceInfo(std::vector<UsbDevice> &devi)
{
    size_t devLen = devi.size();
    UsbDevice info;
    for (size_t i = 0; i < devLen; i++) {
        info = devi.at(i);
        if ((info.GetDevAddr() == DEV_ADDR_OK) && (info.GetBusNum() == BUS_NUM_OK)) {
            break;
        }
    }
    return info;
}

int32_t MockUsbImpl::GetRawDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor)
{
    if ((BUS_NUM_OK != dev.busNum) || (DEV_ADDR_OK != dev.devAddr)) {
        return HDF_DEV_ERR_NO_DEVICE;
    }
    decriptor = g_descBuf;
    return HDF_SUCCESS;
}

int32_t MockUsbImpl::GetStringDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor)
{
    (void)descId;
    if ((BUS_NUM_OK != dev.busNum) || (DEV_ADDR_OK != dev.devAddr)) {
        return HDF_DEV_ERR_NO_DEVICE;
    }
    decriptor = g_descBuf;
    return HDF_SUCCESS;
}

int32_t MockUsbImpl::QueryPort(int32_t &portId, int32_t &powerRole, int32_t &dataRole, int32_t &mode)
{
    USB_HILOGI(MODULE_USB_SERVICE, "pob usb 1 MockUsbImpl::QueryPort enter");
    portId = DEFAULT_PORT_ID;
    powerRole = UsbSrvSupport::POWER_ROLE_SINK;
    dataRole = UsbSrvSupport::DATA_ROLE_DEVICE;
    mode = UsbSrvSupport::PORT_MODE_DEVICE;
    return HDF_SUCCESS;
}

int32_t MockUsbImpl::BindUsbdSubscriber(const sptr<IUsbdSubscriber> &subscriber)
{
    subscriber_ = subscriber;
    return HDF_SUCCESS;
}

int32_t MockUsbImpl::UnbindUsbdSubscriber(const sptr<IUsbdSubscriber> &subscriber)
{
    (void)subscriber;
    subscriber_ = nullptr;
    return HDF_SUCCESS;
}

int32_t MockUsbImpl::SetPortRole(int32_t portId, int32_t powerRole, int32_t dataRole)
{
    int32_t mode = UsbSrvSupport::PORT_MODE_DEVICE;

    if (portId != DEFAULT_PORT_ID) {
        return HDF_FAILURE;
    }
    if (powerRole <= UsbSrvSupport::POWER_ROLE_NONE || powerRole > UsbSrvSupport::POWER_ROLE_SINK) {
        return HDF_FAILURE;
    }
    if (dataRole <= UsbSrvSupport::DTA_ROLE_NONE || dataRole > UsbSrvSupport::DATA_ROLE_DEVICE) {
        return HDF_FAILURE;
    }

    if (powerRole == UsbSrvSupport::POWER_ROLE_SOURCE && dataRole == UsbSrvSupport::DATA_ROLE_HOST) {
        mode = UsbSrvSupport::PORT_MODE_HOST;
    }

    if (powerRole == UsbSrvSupport::POWER_ROLE_SINK && dataRole == UsbSrvSupport::DATA_ROLE_DEVICE) {
        mode = UsbSrvSupport::PORT_MODE_DEVICE;
    }

    portInfo_.portId = portId;
    portInfo_.powerRole = powerRole;
    portInfo_.dataRole = dataRole;
    portInfo_.mode = mode;
    auto ret = subscriber_->PortChangedEvent(portInfo_);

    return ret ? HDF_SUCCESS : HDF_FAILURE;
}

int32_t MockUsbImpl::SubscriberDeviceEvent(const USBDeviceInfo &info)
{
    auto ret = subscriber_->DeviceEvent(info);
    return ret;
}
} // namespace USB
} // namespace OHOS
