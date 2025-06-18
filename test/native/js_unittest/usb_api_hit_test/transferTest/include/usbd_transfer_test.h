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
#ifndef USBD_TRANSFER_TEST_H
#define USBD_TRANSFER_TEST_H

#include <gtest/gtest.h>
#include "UsbSubscriberTest.h"
#include "v2_0/iusbd_subscriber.h"
#include "v2_0/iusbd_bulk_callback.h"
#include "v2_0/usb_types.h"

using OHOS::HDI::Usb::V2_0::UsbDev;

#define USB_DDK_REQ_GET_DESCRIPTOR    0x06
#define USB_DDK_REQ_GET_CONFIGURATION 0x08
#define USB_DDK_REQ_GET_INTERFACE     0x0A
#define USB_DDK_REQ_SYNCH_FRAME       0x0C
const int32_t USB_ENDPOINT_DIR_IN = 0x80;
const int32_t USB_ENDPOINT_DIR_OUT = 0;

typedef enum {
    USB_REQUEST_TARGET_DEVICE,
    USB_REQUEST_TARGET_INTERFACE,
    USB_REQUEST_TARGET_ENDPOINT,
    USB_REQUEST_TARGET_OTHER,
} UsbRequestTargetType;

namespace {
class UsbdTransferTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static UsbDev dev_;
    static OHOS::sptr<OHOS::USB::UsbSubscriberTest> subscriber_;
};
class UsbdBulkCallbackTest : public OHOS::HDI::Usb::V2_0::IUsbdBulkCallback {
public:
    UsbdBulkCallbackTest() = default;
    ~UsbdBulkCallbackTest() = default;
    int32_t OnBulkWriteCallback(int32_t status, int32_t actLength) override
    {
        return 0;
    };
    int32_t OnBulkReadCallback(int32_t status, int32_t actLength) override
    {
        return 0;
    };
};
}
#endif // USBD_TRANSFER_TEST_H
