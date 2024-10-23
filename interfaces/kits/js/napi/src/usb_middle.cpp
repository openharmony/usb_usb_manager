/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <cstdio>
#include <dlfcn.h>

#include "hilog_wrapper.h"
#include "napi/native_api.h"

using namespace OHOS::USB;

/*
 * Module definition
 */
static napi_module g_module = {.nm_version = 1,
    .nm_flags = 0,
    .nm_filename = "usb",
    .nm_register_func = nullptr,
    .nm_modname = "usb",
    .nm_priv = nullptr,
    .reserved = {nullptr}};

static void *g_libHandle = nullptr;

/*
 * Module registration
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    g_libHandle = dlopen("libusbmanager.z.so", RTLD_LAZY);
    if (g_libHandle == nullptr) {
        USB_HILOGE(MODULE_JS_NAPI, "open libusbmanager.z.so failed, %{public}s", dlerror());
        return;
    }
    void *funcPtr = dlsym(g_libHandle, "UsbInit");
    if (funcPtr == nullptr) {
        USB_HILOGE(MODULE_JS_NAPI, "UsbInit not found, %{public}s", dlerror());
        return;
    }
    g_module.nm_register_func = reinterpret_cast<napi_addon_register_func>(funcPtr);
    napi_module_register(&g_module);
}

extern "C" __attribute__((destructor)) void ReleaseModule(void)
{
    if (g_libHandle == nullptr) {
        return;
    }
    int ret = dlclose(g_libHandle);
    USB_HILOGI(MODULE_JS_NAPI, "release module: %{public}d", ret);
    g_libHandle = nullptr;
}