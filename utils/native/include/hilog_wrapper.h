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
    MODULE_INNERKIT = 0,
    MODULE_SERVICE,
    MODULE_USB_INNERKIT, // below used by usb service
    MODULE_USB_SERVICE,
    MODULE_USBD,
    MODULE_COMMON,
    MODULE_JS_NAPI,
    MODULE_JAVAKIT, // java kit used, define to avoid repeat used domain
    MODULE_JNI,
    USBMGR_MODULE_BUTT,
};

// 0xD002900: subsystem:Usb module:Usb, reserved 8 bit.
constexpr unsigned int BASE_USB_DOMAIN_ID = 0xD002A00;

enum UsbMgrDomainId {
    USBMGR_INNERKIT_DOMAIN = BASE_USB_DOMAIN_ID + MODULE_INNERKIT + 1,
    USBMGR_SERVICE_DOMAIN, //0xD002A02
    USB_INNERKIT_DOMAIN,   //0xD002A03
    USB_SERVICE_DOMAIN,    //0xD002A04
    USBD_DOMAIN,           //0xD002A05
    COMMON_DOMAIN,         //0xD002A06
    USB_JS_NAPI,           //0xD002A07
    USBMGR_JAVAKIT_DOMAIN,
    USB_BUTT,
};

static const UsbLable USB_MGR_LABEL[USBMGR_MODULE_BUTT] = {
    {USBMGR_INNERKIT_DOMAIN, "UsbMgrClient"},
    {USBMGR_SERVICE_DOMAIN, "UsbMgrService"},
    {USB_INNERKIT_DOMAIN, "UsbSrvClient"},
    {USB_SERVICE_DOMAIN, "UsbService"},
    {USBD_DOMAIN, "Usbd"},
    {COMMON_DOMAIN, "UsbMgrCommon"},
    {USB_JS_NAPI, "UsbMgrJSNAPI"},
    {USBMGR_JAVAKIT_DOMAIN, "UsbMgrJavaService"},
    {USBMGR_INNERKIT_DOMAIN, "UsbMgrJni"},
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
