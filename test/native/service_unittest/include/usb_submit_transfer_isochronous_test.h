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

#ifndef USB_REQUEST_ISOCHRONOUS_TEST_H
#define USB_REQUEST_ISOCHRONOUS_TEST_H

#include <gtest/gtest.h>

namespace OHOS {
namespace USB {
namespace SubmitTransfer {
class UsbSubmitTransferIsochronousTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

enum UsbIsochronousTestErrCode {
    OHEC_COMMON_PARAM_ERROR = 401,
    USB_SUBMIT_TRANSFER_IO_ERROR = 14400006,
    USB_SUBMIT_TRANSFER_NOT_FOUND_ERROR = 14400011,
};

} // SubmitTransfer
} // USB
} // OHOS
#endif // USB_REQUEST_ISOCHRONOUS_TEST_H
