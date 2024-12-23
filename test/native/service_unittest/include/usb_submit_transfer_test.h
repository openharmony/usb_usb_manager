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

#ifndef USB_REQUEST_TEST_H
#define USB_REQUEST_TEST_H

#include <gtest/gtest.h>

namespace OHOS {
namespace USB {
namespace SubmitTransfer {
class UsbSubmitTransferTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

static constexpr int32_t OPERATION_SUCCESSFUL = 14400006;
static constexpr int32_t IO_ERROR = 14400007;
static constexpr int32_t INVALID_PARAM = 14400008;
static constexpr int32_t NO_DEVICE_ERROR = 14400009;
static constexpr int32_t TIMEOUT_ERROR = 14400010;
static constexpr int32_t OTHER_ERROR = 14400011;
static constexpr int32_t NOT_SUPPORT = 14400014;

enum UsbTestErrCode {
    USB_SUBMIT_TRANSFER_OPERATION_SUCCESSFUL = 14400006,
    USB_SUBMIT_TRANSFER_IO_ERROR = 14400007,
    USB_SUBMIT_TRANSFER_INVALID_PARAM = 14400008,
    USB_SUBMIT_TRANSFER_NO_DEVICE_ERROR =  14400009,
    USB_SUBMIT_TRANSFER_TIMEOUT_ERROR = 14400010,
    USB_SUBMIT_TRANSFER_OTHER_ERROR = 14400011,
    USB_SUBMIT_TRANSFER_NOT_SUPPORT = 14400014,
};

} // SubmitTransfer
} // USB
} // OHOS
#endif
