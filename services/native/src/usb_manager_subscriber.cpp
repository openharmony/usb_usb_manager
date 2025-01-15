/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "usb_manager_subscriber.h"
#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "delayed_sp_singleton.h"
#include "cJSON.h"
#include "string_ex.h"
#include "usb_common.h"
#include "usb_errors.h"
#include "usb_service.h"
#include "want.h"

using namespace OHOS::AAFwk;
using namespace OHOS::EventFwk;
using namespace OHOS::HDI::Usb::V2_0;

namespace OHOS {
namespace USB {
UsbManagerSubscriber::UsbManagerSubscriber() {}
int32_t UsbManagerSubscriber::PortChangedEvent(const PortInfo &info)
{
#ifdef USB_MANAGER_FEATURE_PORT
    auto pms = UsbService::GetGlobalInstance();
    if (pms == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "failed to GetInstance");
        return UEC_SERVICE_GET_USB_SERVICE_FAILED;
    }
    cJSON* portJson = cJSON_CreateObject();
    if (!portJson) {
        USB_HILOGE(MODULE_USB_SERVICE, "Create portJson error");
        return UEC_SERVICE_OBJECT_CREATE_FAILED;
    }
    cJSON_AddNumberToObject(portJson, "portId", static_cast<double>(info.portId));
    cJSON_AddNumberToObject(portJson, "powerRole", static_cast<double>(info.powerRole));
    cJSON_AddNumberToObject(portJson, "dataRole", static_cast<double>(info.dataRole));
    cJSON_AddNumberToObject(portJson, "mode", static_cast<double>(info.mode));
    auto jsonString = cJSON_PrintUnformatted(portJson);
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_USB_PORT_CHANGED);
    pms->UpdateUsbPort(info.portId, info.powerRole, info.dataRole, info.mode);
    CommonEventData data;
    data.SetData(jsonString);
    data.SetWant(want);
    cJSON_Delete(portJson);
    cJSON_free(jsonString);
    CommonEventPublishInfo publishInfo;
    bool isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
    if (!isSuccess) {
        USB_HILOGE(MODULE_USB_SERVICE, "failed to publish PortChangedEvent");
    }
    return isSuccess;
#else
    USB_HILOGE(MODULE_USB_SERVICE, "%{public}s: Port feature is not supported currently.", __FUNCTION__);
    return UEC_SERVICE_NOT_SUPPORT_SWITCH_PORT;
#endif
}

int32_t UsbManagerSubscriber::DeviceEvent(const USBDeviceInfo &info)
{
    auto pms = UsbService::GetGlobalInstance();
    if (pms == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "failed to GetInstance");
        return UEC_SERVICE_GET_USB_SERVICE_FAILED;
    }

    OHOS::HDI::Usb::V1_0::USBDeviceInfo deviceInfo {info.status, info.busNum, info.devNum};
    return pms->DeviceEvent(deviceInfo);
}
} // namespace USB
} // namespace OHOS
