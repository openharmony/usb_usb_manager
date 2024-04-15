/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "usb_report_sys_event.h"

#include "hilog_wrapper.h"
#include "usb_errors.h"
#include "hisysevent.h"

using namespace OHOS::HiviewDFX;

namespace OHOS {
namespace USB {
void UsbReportSysEvent::ReportTransforFaultSysEvent(const std::string interfaceName,
    const HDI::Usb::V1_0::UsbDev &dev, const HDI::Usb::V1_0::UsbPipe &pipe, int32_t ret)
{
    USB_HILOGI(MODULE_USBD, "report transfor fault sys event");
    int32_t hiSysEventWriteRet = HiSysEventWrite(HiSysEvent::Domain::USB, "USB_MANAGE_TRANSFOR_FAULT",
        HiSysEvent::EventType::FAULT, "INTFACE_NAME", interfaceName, "BUS_NUM", dev.busNum, "DEV_ADDR", dev.devAddr,
        "INFT_ID", pipe.intfId, "ENDPOINT_ID", pipe.endpointId, "FAIL_REASON", ret);
    if (hiSysEventWriteRet != UEC_OK) {
        USB_HILOGI(MODULE_USBD, "HiSysEventWrite ret: %{public}d", hiSysEventWriteRet);
    }
}
} // OHOS
} // USB