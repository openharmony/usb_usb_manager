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

#ifndef NAPI_COMMON_H
#include "napi/native_api.h"
EXTERN_C_START
/*
 * function for module exports
 */
napi_value UsbInit(napi_env env, napi_value exports);
napi_value SerialInit(napi_env env, napi_value exports);
EXTERN_C_END
#define NAPI_COMMON_H
#endif // NAPI_COMMON_H