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
#include <iostream>
#include <vector>

#include "hdf_log.h"
#include "securec.h"
#include "usbd_transfer_test.h"
#include "v1_0/iusb_interface.h"
#include "v1_0/usb_types.h"

const int SLEEP_TIME = 3;
const uint32_t MAX_BUFFER_LENGTH = 255;
const uint8_t INTERFACEID_OK = 0;
const uint8_t POINTID_BULK_OUT = 0;
const uint8_t POINTID_INTERRUPT_IN = 129;
const int32_t TRANSFER_TIME_OUT = 1000;

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::USB;
using namespace std;
using namespace OHOS::HDI::Usb::V1_0;

UsbDev UsbdTransferTestAdditional::dev_ = {0, 0};
sptr<UsbSubscriberTest> UsbdTransferTestAdditional::subscriber_ = nullptr;

namespace {
sptr<IUsbInterface> g_usbInterface = nullptr;

int32_t SwitchErrCode(int32_t ret)
{
    return ret == HDF_ERR_NOT_SUPPORT ? HDF_SUCCESS : ret;
}

void UsbdTransferTestAdditional::SetUpTestCase(void)
{
    g_usbInterface = IUsbInterface::Get();
    if (g_usbInterface == nullptr) {
        HDF_LOGE("%{public}s:IUsbInterface::Get() failed.", __func__);
        exit(0);
    }
    const int32_t DEFAULT_PORT_ID = 1;
    const int32_t DEFAULT_ROLE_HOST = 1;
    auto ret = g_usbInterface->SetPortRole(DEFAULT_PORT_ID, DEFAULT_ROLE_HOST, DEFAULT_ROLE_HOST);
    sleep(SLEEP_TIME);
    HDF_LOGI("UsbdTransferTestAdditional::[Device] %{public}d SetPortRole=%{public}d", __LINE__, ret);
    ret = SwitchErrCode(ret);
    ASSERT_EQ(0, ret);
    if (ret != 0) {
        exit(0);
    }

    subscriber_ = new UsbSubscriberTest();
    if (subscriber_ == nullptr) {
        HDF_LOGE("%{public}s:UsbSubscriberTest new failed.", __func__);
        exit(0);
    }
    if (g_usbInterface->BindUsbdSubscriber(subscriber_) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: bind usbd subscriber_ failed\n", __func__);
        exit(0);
    }

    std::cout << "please connect device, press enter to continue" << std::endl;
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);

    dev_ = {subscriber_->busNum_, subscriber_->devAddr_};
    ret = g_usbInterface->OpenDevice(dev_);
    HDF_LOGI("UsbdTransferTestAdditional:: %{public}d OpenDevice=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

void UsbdTransferTestAdditional::TearDownTestCase(void)
{
    g_usbInterface->UnbindUsbdSubscriber(subscriber_);
    dev_ = {subscriber_->busNum_, subscriber_->devAddr_};
    auto ret = g_usbInterface->CloseDevice(dev_);
    HDF_LOGI("UsbdTransferTestAdditional:: %{public}d Close=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

void UsbdTransferTestAdditional::SetUp(void) {}

void UsbdTransferTestAdditional::TearDown(void) {}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_0700
 * @tc.name: testHdiUsbTransferTestInterruptTransferWrite001
 * @tc.desc: Writes data on a specified endpoint during interrupt transfer.
 * bufferData = {'I', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', ','}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestInterruptTransferWrite001, TestSize.Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'I', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', ','};
    ret = g_usbInterface->InterruptTransferWrite(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_0800
 * @tc.name: testHdiUsbTransferTestInterruptTransferWrite002
 * @tc.desc: Writes data on a specified endpoint during interrupt transfer.
 * bufferData = {'I', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', '!'}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestInterruptTransferWrite002, TestSize.Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'I', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', '!'};
    ret = g_usbInterface->InterruptTransferWrite(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_0900
 * @tc.name: testHdiUsbTransferTestInterruptTransferWrite003
 * @tc.desc: Writes data on a specified endpoint during interrupt transfer.
 * bufferData = {'I', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', '~'}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestInterruptTransferWrite003, TestSize.Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'I', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', '~'};
    ret = g_usbInterface->InterruptTransferWrite(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_1000
 * @tc.name: testHdiUsbTransferTestInterruptTransferRead001
 * @tc.desc: Reads data on a specified endpoint during interrupt transfer.
 * bufferData = {'I', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', '~'} Check if the read is correct.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestInterruptTransferRead001, TestSize.Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData1(MAX_BUFFER_LENGTH);
    pipe = {interfaceId, POINTID_INTERRUPT_IN};
    ret = g_usbInterface->InterruptTransferRead(dev, pipe, -1, bufferData1);
    ASSERT_EQ(0, ret);
    for (uint8_t i = 0; i < bufferData1.size(); ++i) {
        std::cout << bufferData1[i] << " ";
    }
    std::cout << std::endl;
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_1100
 * @tc.name: testHdiUsbTransferTestInterruptTransferRead002
 * @tc.desc: Reads data on a specified endpoint during interrupt transfer.
 * bufferData = {'I', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', '!'} Check if the read is correct.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestInterruptTransferRead002, TestSize.Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData1(MAX_BUFFER_LENGTH);
    pipe = {interfaceId, POINTID_INTERRUPT_IN};
    ret = g_usbInterface->InterruptTransferRead(dev, pipe, -1, bufferData1);
    ASSERT_EQ(0, ret);
    for (uint8_t i = 0; i < bufferData1.size(); ++i) {
        std::cout << bufferData1[i] << " ";
    }
    std::cout << std::endl;
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_1200
 * @tc.name: testHdiUsbTransferTestInterruptTransferRead003
 * @tc.desc: Reads data on a specified endpoint during interrupt transfer.
 * bufferData = {'I', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', ','} Check if the read is correct.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestInterruptTransferRead003, TestSize.Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData1(MAX_BUFFER_LENGTH);
    pipe = {interfaceId, POINTID_INTERRUPT_IN};
    ret = g_usbInterface->InterruptTransferRead(dev, pipe, -1, bufferData1);
    ASSERT_EQ(0, ret);
    for (uint8_t i = 0; i < bufferData1.size(); ++i) {
        std::cout << bufferData1[i] << " ";
    }
    std::cout << std::endl;
}
} // namespace
