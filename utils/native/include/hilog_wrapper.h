/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HILOG_WRAPPER_H
#define HILOG_WRAPPER_H

#define CONFIG_HILOG
#ifdef CONFIG_HILOG
#include "hilog/log.h"
namespace OHOS {
namespace USB {
#define USBFILENAME (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef USB_HILOGF
#undef USB_HILOGF
#endif

#ifdef USB_HILOGE
#undef USB_HILOGE
#endif

#ifdef USB_HILOGW
#undef USB_HILOGW
#endif

#ifdef USB_HILOGI
#undef USB_HILOGI
#endif

#ifdef USB_HILOGD
#undef USB_HILOGD
#endif

struct UsbLable {
    uint32_t domainId;
    const char* tag;
};

// param of log interface, such as USB_HILOGF.
enum UsbMgrSubModule {
    MODULE_USB_SERVICE = 0,
    MODULE_USB_HOST,
    MODULE_USB_DEVICE,
    MODULE_USB_PORT,
    MODULE_USB_SERIAL,
    MODULE_USB_NAPI,
    MODULE_USB_INNERKIT,
    MODULE_USB_UTILS,
    MODULE_USB_BUTTOM,
};

// 0xD002900: subsystem:Usb module:Usb, reserved 8 bit.
constexpr unsigned int BASE_USB_DOMAIN_ID = 0xD002A00;

enum UsbMgrDomainId {
    USB_SERVICE_SUBDOMAIN = BASE_USB_DOMAIN_ID + 1,
    USB_HOST_MGR_SUBDOMAIN,     //0xD002A02
    USB_DEVICE_MGR_SUBDOMAIN,   //0xD002A03
    USB_PORT_MGR_SUBDOMAIN,     //0xD002A04
    USB_SERIAL_SUBDOMAIN,       //0xD002A05
    USB_NAPI_SUBDOMAIN,         //0xD002A06
    USB_INNERKIT_SUBDOMAIN,     //0xD002A07
    USB_UTILS_SUBDOMAIN,        //0xD002A08
    USB_BUTTOM,
};

static const UsbLable USB_MGR_LABEL[MODULE_USB_BUTTOM] = {
    {USB_SERVICE_SUBDOMAIN, "USBService"},
    {USB_HOST_MGR_SUBDOMAIN, "USBHostMgr"},
    {USB_DEVICE_MGR_SUBDOMAIN, "USBDeviceMgr"},
    {USB_PORT_MGR_SUBDOMAIN, "USBPortMgr"},
    {USB_SERIAL_SUBDOMAIN, "USBSerial"},
    {USB_NAPI_SUBDOMAIN, "USBNapi"},
    {USB_INNERKIT_SUBDOMAIN, "USBInnerkit"},
    {USB_UTILS_SUBDOMAIN, "USBUtils"},
};

// In order to improve performance, do not check the module range, module should less than USBMGR_MODULE_BUTT.
#define USB_HILOGF(module, ...) \
    ((void)HILOG_IMPL(LOG_CORE, LOG_FATAL, USB_MGR_LABEL[module].domainId, USB_MGR_LABEL[module].tag, ##__VA_ARGS__))
#define USB_HILOGE(module, ...) \
    ((void)HILOG_IMPL(LOG_CORE, LOG_ERROR, USB_MGR_LABEL[module].domainId, USB_MGR_LABEL[module].tag, ##__VA_ARGS__))
#define USB_HILOGW(module, ...) \
    ((void)HILOG_IMPL(LOG_CORE, LOG_WARN, USB_MGR_LABEL[module].domainId, USB_MGR_LABEL[module].tag, ##__VA_ARGS__))
#define USB_HILOGI(module, ...) \
    ((void)HILOG_IMPL(LOG_CORE, LOG_INFO, USB_MGR_LABEL[module].domainId, USB_MGR_LABEL[module].tag, ##__VA_ARGS__))
#define USB_HILOGD(module, ...) \
    ((void)HILOG_IMPL(LOG_CORE, LOG_DEBUG, USB_MGR_LABEL[module].domainId, USB_MGR_LABEL[module].tag, ##__VA_ARGS__))
} // namespace USB
} // namespace OHOS

#else

#define USB_HILOGF(...)
#define USB_HILOGE(...)
#define USB_HILOGW(...)
#define USB_HILOGI(...)
#define USB_HILOGD(...)

#endif // CONFIG_HILOG

#endif // HILOG_WRAPPER_H
