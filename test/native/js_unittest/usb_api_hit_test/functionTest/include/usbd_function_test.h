/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef USBD_FUNCTION_TEST_H
#define USBD_FUNCTION_TEST_H

#include <gtest/gtest.h>

#define DEFAULT_PORT_ID 1

#define POWER_ROLE_NONE   0
#define POWER_ROLE_SOURCE 1
#define POWER_ROLE_SINK   2

#define DATA_ROLE_NONE   0
#define DATA_ROLE_HOST   1
#define DATA_ROLE_DEVICE 2

#define PORT_MODE_NONE   0

#define USB_FUNCTION_NONE    0
#define USB_FUNCTION_ACM     (1 << 0)
#define USB_FUNCTION_ECM     (1 << 1)
#define USB_FUNCTION_HDC     (1 << 2)
#define USB_FUNCTION_MTP     (1 << 3)
#define USB_FUNCTION_PTP     (1 << 4)
#define USB_FUNCTION_RNDIS   (1 << 5)
#define USB_FUNCTION_STORAGE (1 << 9)

namespace {
class UsbdFunctionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
}
#endif // USBD_FUNCTION_TEST_H