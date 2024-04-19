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

#include "usb_service_subscriber.h"
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
using namespace OHOS::HDI::Usb::V1_0;

namespace OHOS {
namespace USB {
UsbServiceSubscriber::UsbServiceSubscriber() {}
extern sptr<UsbService> g_serviceInstance;
int32_t UsbServiceSubscriber::PortChangedEvent(const PortInfo &info)
{
    auto pms = g_serviceInstance;
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
}

int32_t UsbServiceSubscriber::DeviceEvent(const USBDeviceInfo &info)
{
    int32_t status = info.status;
    auto pms = g_serviceInstance;
    USB_HILOGE(MODULE_USB_SERVICE, "DeviceEvent =========");
    if (pms == nullptr) {
        USB_HILOGE(MODULE_USB_SERVICE, "failed to GetInstance");
        return UEC_SERVICE_GET_USB_SERVICE_FAILED;
    }

    if (status == ACT_UPDEVICE || status == ACT_DOWNDEVICE) {
        pms->UpdateDeviceState(status);
        pms->UnLoadSelf(UsbService::UnLoadSaType::UNLOAD_SA_DELAY);
        return UEC_OK;
    }

    int32_t busNum = info.busNum;
    int32_t devAddr = info.devNum;
    if (status == ACT_DEVUP) {
        USB_HILOGI(MODULE_USB_SERVICE, "usb attached");
        pms->AddDevice(busNum, devAddr);
    } else {
        USB_HILOGI(MODULE_USB_SERVICE, "usb detached");
        pms->DelDevice(busNum, devAddr);
    }
    pms->UnLoadSelf(UsbService::UnLoadSaType::UNLOAD_SA_DELAY);
    return UEC_OK;
}
} // namespace USB
} // namespace OHOS
