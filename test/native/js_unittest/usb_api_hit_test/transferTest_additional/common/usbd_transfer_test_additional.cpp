/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
const uint8_t SAMPLE_DATA_1 = 1;
const uint8_t SAMPLE_DATA_2 = 2;
const uint8_t SAMPLE_DATA_3 = 3;
const uint8_t INTERFACEID_OK = 1;
const int32_t TRANSFER_TIME_OUT = 1000;
const uint8_t POINTID_BULK_OUT = USB_ENDPOINT_DIR_OUT | 1;
const uint8_t POINTID_BULK_IN = USB_ENDPOINT_DIR_IN | 2;
const uint8_t PIPE_ENDPOINTID_INVALID = 244;
const int32_t ASHMEM_MAX_SIZE = 1024;

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::USB;
using namespace std;
using namespace OHOS::HDI::Usb::V1_0;

UsbDev UsbdTransferTestAdditional::dev_ = {0, 0};
sptr<UsbSubscriberTest> UsbdTransferTestAdditional::subscriber_ = nullptr;

namespace {
sptr<IUsbInterface> g_usbInterface = nullptr;

int32_t InitAshmemOne(sptr<Ashmem> &asmptr, int32_t asmSize, uint8_t rflg)
{
    asmptr = Ashmem::CreateAshmem("ttashmem000", asmSize);
    if (asmptr == nullptr) {
        HDF_LOGE("InitAshmemOne CreateAshmem failed\n");
        return HDF_FAILURE;
    }

    asmptr->MapReadAndWriteAshmem();

    if (rflg == 0) {
        uint8_t tdata[ASHMEM_MAX_SIZE];
        int32_t offset = 0;
        int32_t tlen = 0;

        int32_t retSafe = memset_s(tdata, sizeof(tdata), 'Y', ASHMEM_MAX_SIZE);
        if (retSafe != EOK) {
            HDF_LOGE("InitAshmemOne memset_s failed\n");
            return HDF_FAILURE;
        }
        while (offset < asmSize) {
            tlen = (asmSize - offset) < ASHMEM_MAX_SIZE ? (asmSize - offset) : ASHMEM_MAX_SIZE;
            asmptr->WriteToAshmem(tdata, tlen, offset);
            offset += tlen;
        }
    }
    return HDF_SUCCESS;
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
    if (ret != 0) {
        ASSERT_EQ(HDF_ERR_NOT_SUPPORT, ret);
    } else {
        ASSERT_EQ(0, ret);
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
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_0100
 * @tc.name: testHdiUsbTransferTestBulkTransferWrite001
 * @tc.desc: Writes data on a specified endpoint during bulk transfer.
 * bufferData = {'B', 'u', 'l', 'k', 'w', 'r', 'i', 't', 'e', '0', ','}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestBulkTransferWrite001, TestSize.Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'B', 'u', 'l', 'k', 'w', 'r', 'i', 't', 'e', '0', ','};
    ret = g_usbInterface->BulkTransferWrite(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_0200
 * @tc.name: testHdiUsbTransferTestBulkTransferWrite002
 * @tc.desc: Writes data on a specified endpoint during bulk transfer.
 * bufferData = {'B', 'u', 'l', 'k', 'w', 'r', 'i', 't', 'e', '0', '!'}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestBulkTransferWrite002, TestSize.Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'B', 'u', 'l', 'k', 'w', 'r', 'i', 't', 'e', '0', '!'};
    ret = g_usbInterface->BulkTransferWrite(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_0300
 * @tc.name: testHdiUsbTransferTestBulkTransferWrite003
 * @tc.desc: Writes data on a specified endpoint during bulk transfer.
 * bufferData = {'B', 'u', 'l', 'k', 'w', 'r', 'i', 't', 'e', '0', '~'}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestBulkTransferWrite003, TestSize.Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'B', 'u', 'l', 'k', 'w', 'r', 'i', 't', 'e', '0', '~'};
    ret = g_usbInterface->BulkTransferWrite(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_0400
 * @tc.name: testHdiUsbTransferTestBulkTransferRead001
 * @tc.desc: Writes data on a specified endpoint during bulk transfer.
 * bufferData = {'B', 'u', 'l', 'k', 'w', 'r', 'i', 't', 'e', '0', '~'} Check if the read is correct.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestBulkTransferRead001, TestSize.Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'B', 'u', 'l', 'k', 'w', 'r', 'i', 't', 'e', '0', '~'};
    std::vector<uint8_t> bufferData1(MAX_BUFFER_LENGTH);
    ret = g_usbInterface->BulkTransferWrite(dev, pipe, -1, bufferData);
    ASSERT_EQ(0, ret);
    pipe = {interfaceId, POINTID_BULK_IN};
    ret = g_usbInterface->BulkTransferRead(dev, pipe, -1, bufferData1);
    ASSERT_EQ(0, ret);
    for (uint8_t i = 0; i < bufferData1.size(); ++i) {
        std::cout << bufferData1[i] << " ";
    }
    std::cout << std::endl;
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_0500
 * @tc.name: testHdiUsbTransferTestBulkTransferRead002
 * @tc.desc: Writes data on a specified endpoint during bulk transfer.
 * bufferData = {'B', 'u', 'l', 'k', 'w', 'r', 'i', 't', 'e', '0', '!'} Check if the read is correct.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestBulkTransferRead002, TestSize.Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'B', 'u', 'l', 'k', 'w', 'r', 'i', 't', 'e', '0', '!'};
    std::vector<uint8_t> bufferData1(MAX_BUFFER_LENGTH);
    ret = g_usbInterface->BulkTransferWrite(dev, pipe, -1, bufferData);
    ASSERT_EQ(0, ret);
    pipe = {interfaceId, POINTID_BULK_IN};
    ret = g_usbInterface->BulkTransferRead(dev, pipe, -1, bufferData1);
    ASSERT_EQ(0, ret);
    for (uint8_t i = 0; i < bufferData1.size(); ++i) {
        std::cout << bufferData1[i] << " ";
    }
    std::cout << std::endl;
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_0600
 * @tc.name: testHdiUsbTransferTestBulkTransferRead003
 * @tc.desc: Writes data on a specified endpoint during bulk transfer.
 * bufferData = {'B', 'u', 'l', 'k', 'w', 'r', 'i', 't', 'e', '0', ','} Check if the read is correct.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestBulkTransferRead003, TestSize.Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'B', 'u', 'l', 'k', 'w', 'r', 'i', 't', 'e', '0', ','};
    std::vector<uint8_t> bufferData1(MAX_BUFFER_LENGTH);
    ret = g_usbInterface->BulkTransferWrite(dev, pipe, -1, bufferData);
    ASSERT_EQ(0, ret);
    pipe = {interfaceId, POINTID_BULK_IN};
    ret = g_usbInterface->BulkTransferRead(dev, pipe, -1, bufferData1);
    ASSERT_EQ(0, ret);
    for (uint8_t i = 0; i < bufferData1.size(); ++i) {
        std::cout << bufferData1[i] << " ";
    }
    std::cout << std::endl;
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_1300
 * @tc.name: testHdiUsbTransferTestUnbindUsbdSubscriber001
 * @tc.desc: Unbinds a subscriber twice.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestUnbindUsbdSubscriber001, Function | MediumTest | Level2)
{
    subscriber_ = new UsbSubscriberTest();
    if (subscriber_ == nullptr) {
        HDF_LOGE("%{public}s:UsbSubscriberTest new failed.", __func__);
        exit(0);
    }
    auto ret = g_usbInterface->UnbindUsbdSubscriber(subscriber_);
    ASSERT_NE(0, ret);
    ret = g_usbInterface->UnbindUsbdSubscriber(subscriber_);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_1400
 * @tc.name: testHdiUsbTransferTestUnbindUsbdSubscriber002
 * @tc.desc: Unbinds a subscriber for twice.unbind and bind, then Unbind twice.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestUnbindUsbdSubscriber002, Function | MediumTest | Level2)
{
    subscriber_ = new UsbSubscriberTest();
    if (subscriber_ == nullptr) {
        HDF_LOGE("%{public}s:UsbSubscriberTest new failed.", __func__);
        exit(0);
    }
    auto ret = g_usbInterface->UnbindUsbdSubscriber(subscriber_);
    ASSERT_NE(0, ret);
    ret = g_usbInterface->BindUsbdSubscriber(subscriber_);
    ASSERT_EQ(0, ret);
    ret = g_usbInterface->UnbindUsbdSubscriber(subscriber_);
    ASSERT_EQ(0, ret);
    ret = g_usbInterface->UnbindUsbdSubscriber(subscriber_);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_1500
 * @tc.name: testHdiUsbTransferTestUnbindUsbdSubscriber003
 * @tc.desc: Unbinds a subscriber twice.then unbind and bind.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestUnbindUsbdSubscriber003, Function | MediumTest | Level2)
{
    subscriber_ = new UsbSubscriberTest();
    if (subscriber_ == nullptr) {
        HDF_LOGE("%{public}s:UsbSubscriberTest new failed.", __func__);
        exit(0);
    }
    auto ret = g_usbInterface->UnbindUsbdSubscriber(subscriber_);
    ASSERT_NE(0, ret);
    ret = g_usbInterface->UnbindUsbdSubscriber(subscriber_);
    ASSERT_NE(0, ret);
    ret = g_usbInterface->BindUsbdSubscriber(subscriber_);
    ASSERT_EQ(0, ret);
    ret = g_usbInterface->UnbindUsbdSubscriber(subscriber_);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_1600
 * @tc.name: testHdiUsbTransferTestbindUsbdSubscriber001
 * @tc.desc: Binds a subscriber.unbind and bind twice.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestbindUsbdSubscriber001, Function | MediumTest | Level1)
{
    subscriber_ = new UsbSubscriberTest();
    if (subscriber_ == nullptr) {
        HDF_LOGE("%{public}s:UsbSubscriberTest new failed.", __func__);
        exit(0);
    }
    auto ret = g_usbInterface->BindUsbdSubscriber(subscriber_);
    ASSERT_EQ(0, ret);
    ret = g_usbInterface->UnbindUsbdSubscriber(subscriber_);
    ASSERT_EQ(0, ret);

    sptr<UsbSubscriberTest> subscriber2 = new UsbSubscriberTest();
    if (subscriber_ == nullptr) {
        HDF_LOGE("%{public}s:UsbSubscriberTest new failed.", __func__);
        exit(0);
    }
    ret = g_usbInterface->BindUsbdSubscriber(subscriber2);
    ASSERT_EQ(0, ret);
    ret = g_usbInterface->UnbindUsbdSubscriber(subscriber2);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_1700
 * @tc.name: testHdiUsbTransferTestControlTransferRead001
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (0,0).see if bufferData is equal to
 * bufferData1.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferRead001, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    std::vector<uint8_t> bufferData = {'r', 'e', 'a', 'd', 't', 'e', 's', 't', '0', '1'};
    std::vector<uint8_t> bufferData1(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT, USB_DDK_REQ_GET_CONFIGURATION, 0, 0, -1};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_EQ(0, ret);
    ctrlparmas = {USB_ENDPOINT_DIR_IN, USB_DDK_REQ_GET_CONFIGURATION, 0, 0, -1};
    ret = g_usbInterface->ControlTransferRead(dev, ctrlparmas, bufferData1);
    ASSERT_EQ(0, ret);
    for (uint8_t i = 0; i < bufferData1.size(); ++i) {
        std::cout << bufferData1[i] << " ";
    }
    std::cout << std::endl;
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_1800
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite001
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.timeout = 1.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite001, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '0', '1'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 1, bufferData);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_1900
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite002
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.timeout = 50.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite002, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '0', '2'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 50, bufferData);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_2000
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite003
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.timeout = 100.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite003, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '0', '3'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 100, bufferData);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_2100
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite004
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.timeout = 255.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite004, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '0', '4'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 255, bufferData);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_2200
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite005
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 255} timeout = 1.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite005, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '0', '5'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 1, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_2300
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite006
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 255} timeout = 50.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite006, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '0', '6'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 50, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_2400
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite007
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 255} timeout = 100.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite007, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '0', '7'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 100, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_2500
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite008
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 255} timeout = 255.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite008, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '0', '8'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 255, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_2600
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite009
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {0, 255} timeout = 1.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite009, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {0, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '0', '9'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 1, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_2700
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite010
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {0, 255} timeout = 50.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite010, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {0, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '1', '0'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 50, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_2800
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite011
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {0, 255} timeout = 100.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite011, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {0, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '1', '1'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 100, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_2900
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite012
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {0, 255} timeout = 255.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite012, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {0, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '1', '2'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 255, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_3000
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite013
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 0} timeout = 1.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite013, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 0};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '1', '3'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 1, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_3100
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite014
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 0} timeout = 50.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite014, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 0};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '1', '4'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 50, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_3200
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite015
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 0} timeout = 100.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite015, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 0};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '1', '5'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 100, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_3300
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite016
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 0} timeout = 255.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite016, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 0};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '1', '6'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 255, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_3400
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite017
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 255} timeout = 1 pipe = { 1,
 * 244 }.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite017, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = PIPE_ENDPOINTID_INVALID;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '1', '7'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 1, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_3500
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite018
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 255} timeout = 50 pipe = { 1,
 * 244 }.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite018, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = PIPE_ENDPOINTID_INVALID;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '1', '8'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 50, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_3600
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite019
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 255} timeout = 100 pipe = { 1,
 * 244 }.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite019, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = PIPE_ENDPOINTID_INVALID;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '1', '9'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 100, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_3700
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite020
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 255} timeout = 255 pipe = { 1,
 * 244 }.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite020, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = PIPE_ENDPOINTID_INVALID;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '2', '0'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 255, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_3800
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite021
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {0, 255} timeout = 1 pipe = { 1, 244
 * }.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite021, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = PIPE_ENDPOINTID_INVALID;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {0, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '2', '1'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 1, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_3900
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite022
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {0, 255} timeout = 50 pipe = { 1, 244
 * }.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite022, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = PIPE_ENDPOINTID_INVALID;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {0, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '2', '2'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 50, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_4000
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite023
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {0, 255} timeout = 100 pipe = { 1,
 * 244 }.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite023, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = PIPE_ENDPOINTID_INVALID;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {0, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '2', '3'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 100, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_4100
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite024
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {0, 255} timeout = 255 pipe = { 1,
 * 244 }.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite024, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = PIPE_ENDPOINTID_INVALID;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {0, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '2', '4'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 255, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_4200
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite025
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 0} timeout = 1 pipe = { 1, 244
 * }.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite025, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = PIPE_ENDPOINTID_INVALID;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 0};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '2', '5'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 1, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_4300
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite026
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 0} timeout = 50 pipe = { 1, 244
 * }.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite026, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = PIPE_ENDPOINTID_INVALID;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 0};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '2', '6'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 50, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_4400
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite027
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 0} timeout = 100 pipe = { 1,
 * 244 }.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite027, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = PIPE_ENDPOINTID_INVALID;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 0};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '2', '7'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 100, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_4500
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite028
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 0} timeout = 255 pipe = { 1,
 * 244 }.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite028, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = PIPE_ENDPOINTID_INVALID;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 0};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '2', '8'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 255, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_4600
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite029
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 255}  pointId = POINTID_BULK_IN
 * timeout = 1.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite029, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_IN;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '2', '9'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 1, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_4700
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite030
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 255} pointId = POINTID_BULK_IN
 * timeout = 50.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite030, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_IN;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '3', '0'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 50, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_4800
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite031
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 255} pointId = POINTID_BULK_IN
 * timeout = 100.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite031, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_IN;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '3', '1'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 100, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_4900
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite032
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 255} pointId = POINTID_BULK_IN
 * timeout = 255.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite032, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_IN;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '3', '2'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 255, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_5000
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite033
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {0, 255} pointId = POINTID_BULK_IN
 * timeout = 1.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite033, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_IN;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {0, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '3', '3'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 1, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_5100
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite034
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {0, 255} pointId = POINTID_BULK_IN
 * timeout = 50.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite034, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_IN;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {0, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '3', '4'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 50, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_5200
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite035
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {0, 255} pointId = POINTID_BULK_IN
 * timeout = 100.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite035, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_IN;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {0, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '3', '5'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 100, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_5300
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite036
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {0, 255} pointId = POINTID_BULK_IN
 * timeout = 255.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite036, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_IN;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {0, 255};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '3', '6'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 255, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_5400
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite037
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 0} pointId = POINTID_BULK_IN
 * timeout = 1.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite037, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_IN;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 0};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '3', '7'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 1, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_5500
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite038
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 0} pointId = POINTID_BULK_IN
 * timeout = 50.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite038, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_IN;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 0};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '3', '8'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 50, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_5600
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite039
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 0} pointId = POINTID_BULK_IN
 * timeout = 100.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite039, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_IN;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 0};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '3', '9'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 100, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_5700
 * @tc.name: testHdiUsbTransferTestIsoTransferWrite040
 * @tc.desc: Writes data on a specified endpoint during isochronous transfer.dev = {255, 0} pointId = POINTID_BULK_IN
 * timeout = 255.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferWrite040, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_IN;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    dev = {255, 0};
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    std::vector<uint8_t> bufferData = {'i', 's', 'o', 't', 'e', 's', 't', '0', '4', '0'};
    ret = g_usbInterface->IsoTransferWrite(dev, pipe, 255, bufferData);
    ASSERT_NE(0, ret);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_5800
 * @tc.name: testHdiUsbTransferTestIsoTransferRead001
 * @tc.desc: Reads data on a specified endpoint during isochronous transfer.see if bufferData1 is equal to bufferData.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestIsoTransferRead001, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    std::vector<uint8_t> bufferData = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd'};
    std::vector<uint8_t> bufferData1(MAX_BUFFER_LENGTH);
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};

    ret = g_usbInterface->IsoTransferWrite(dev, pipe, -1, bufferData);
    ASSERT_EQ(0, ret);
    pipe = {interfaceId, POINTID_BULK_IN};
    ret = g_usbInterface->IsoTransferRead(dev, pipe, -1, bufferData1);
    ASSERT_EQ(0, ret);
    for (uint8_t i = 0; i < bufferData1.size(); ++i) {
        std::cout << bufferData1[i] << " ";
    }
    std::cout << std::endl;
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_5900
 * @tc.name: testHdiUsbTransferTestBulkRead001
 * @tc.desc: Reads data during isochronous bulk transfer. dev = {1, 255}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestBulkRead001, Function | MediumTest | Level2)
{
    sptr<Ashmem> ashmem;
    uint8_t rflg = 0;
    int32_t asmSize = MAX_BUFFER_LENGTH;
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    dev = {1, 255};
    (void)InitAshmemOne(ashmem, asmSize, rflg);
    ret = g_usbInterface->BulkRead(dev, pipe, ashmem);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_6000
 * @tc.name: testHdiUsbTransferTestBulkRead002
 * @tc.desc: Reads data during isochronous bulk transfer. dev = {255, 1}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestBulkRead002, Function | MediumTest | Level2)
{
    sptr<Ashmem> ashmem;
    uint8_t rflg = 0;
    int32_t asmSize = MAX_BUFFER_LENGTH;
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    dev = {255, 1};
    (void)InitAshmemOne(ashmem, asmSize, rflg);
    ret = g_usbInterface->BulkRead(dev, pipe, ashmem);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_6100
 * @tc.name: testHdiUsbTransferTestBulkRead003
 * @tc.desc: Reads data during isochronous bulk transfer. dev = {255, 255}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestBulkRead003, Function | MediumTest | Level2)
{
    sptr<Ashmem> ashmem;
    uint8_t rflg = 0;
    int32_t asmSize = MAX_BUFFER_LENGTH;
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    dev = {255, 255};
    (void)InitAshmemOne(ashmem, asmSize, rflg);
    ret = g_usbInterface->BulkRead(dev, pipe, ashmem);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_6200
 * @tc.name: testHdiUsbTransferTestBulkRead004
 * @tc.desc: Reads data during isochronous bulk transfer. dev = {100, 255}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestBulkRead004, Function | MediumTest | Level2)
{
    sptr<Ashmem> ashmem;
    uint8_t rflg = 0;
    int32_t asmSize = MAX_BUFFER_LENGTH;
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    dev = {100, 255};
    (void)InitAshmemOne(ashmem, asmSize, rflg);
    ret = g_usbInterface->BulkRead(dev, pipe, ashmem);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_6300
 * @tc.name: testHdiUsbTransferTestBulkRead005
 * @tc.desc: Reads data during isochronous bulk transfer. dev = {255, 100}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestBulkRead005, Function | MediumTest | Level2)
{
    sptr<Ashmem> ashmem;
    uint8_t rflg = 0;
    int32_t asmSize = MAX_BUFFER_LENGTH;
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    dev = {255, 100};
    (void)InitAshmemOne(ashmem, asmSize, rflg);
    ret = g_usbInterface->BulkRead(dev, pipe, ashmem);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_6400
 * @tc.name: testHdiUsbTransferTestBulkWrite001
 * @tc.desc: Writes data during isochronous bulk transfer. dev = {1, 255}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestBulkWrite001, Function | MediumTest | Level2)
{
    sptr<Ashmem> ashmem;
    uint8_t rflg = 0;
    int32_t asmSize = MAX_BUFFER_LENGTH;
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    dev = {1, 255};
    (void)InitAshmemOne(ashmem, asmSize, rflg);
    ret = g_usbInterface->BulkWrite(dev, pipe, ashmem);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_6500
 * @tc.name: testHdiUsbTransferTestBulkWrite002
 * @tc.desc: Writes data during isochronous bulk transfer. dev = {255, 1}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestBulkWrite002, Function | MediumTest | Level2)
{
    sptr<Ashmem> ashmem;
    uint8_t rflg = 0;
    int32_t asmSize = MAX_BUFFER_LENGTH;
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    dev = {255, 1};
    (void)InitAshmemOne(ashmem, asmSize, rflg);
    ret = g_usbInterface->BulkWrite(dev, pipe, ashmem);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_6600
 * @tc.name: testHdiUsbTransferTestBulkWrite003
 * @tc.desc: Writes data during isochronous bulk transfer. dev = {255, 255}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestBulkWrite003, Function | MediumTest | Level2)
{
    sptr<Ashmem> ashmem;
    uint8_t rflg = 0;
    int32_t asmSize = MAX_BUFFER_LENGTH;
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    dev = {255, 255};
    (void)InitAshmemOne(ashmem, asmSize, rflg);
    ret = g_usbInterface->BulkWrite(dev, pipe, ashmem);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_6700
 * @tc.name: testHdiUsbTransferTestBulkWrite004
 * @tc.desc: Writes data during isochronous bulk transfer. dev = {100, 255}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestBulkWrite004, Function | MediumTest | Level2)
{
    sptr<Ashmem> ashmem;
    uint8_t rflg = 0;
    int32_t asmSize = MAX_BUFFER_LENGTH;
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    dev = {100, 255};
    (void)InitAshmemOne(ashmem, asmSize, rflg);
    ret = g_usbInterface->BulkWrite(dev, pipe, ashmem);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_6800
 * @tc.name: testHdiUsbTransferTestBulkWrite005
 * @tc.desc: Writes data during isochronous bulk transfer. dev = {255, 100}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestBulkWrite005, Function | MediumTest | Level2)
{
    sptr<Ashmem> ashmem;
    uint8_t rflg = 0;
    int32_t asmSize = MAX_BUFFER_LENGTH;
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbInterface->ClaimInterface(dev, interfaceId, 1);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    dev = {255, 100};
    (void)InitAshmemOne(ashmem, asmSize, rflg);
    ret = g_usbInterface->BulkWrite(dev, pipe, ashmem);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_6900
 * @tc.name: testHdiUsbTransferTestRegBulkCallback001
 * @tc.desc: Registers a callback for isochronous bulk transfer. dev = {1, 255}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestRegBulkCallback001, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    dev = {1, 255};
    sptr<UsbdBulkCallbackTest> usbdBulkCallback = new UsbdBulkCallbackTest();
    if (usbdBulkCallback == nullptr) {
        HDF_LOGE("%{public}s:UsbdBulkCallbackTest new failed.", __func__);
        exit(0);
    }
    auto ret = g_usbInterface->RegBulkCallback(dev, pipe, usbdBulkCallback);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_USB_HostManager_HDI_CompatibilityTransferAddTest_7000
 * @tc.name: testHdiUsbTransferTestUnRegBulkCallback001
 * @tc.desc: Unregisters the callback for isochronous bulk transfer. dev = {1, 255}.
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestUnRegBulkCallback001, Function | MediumTest | Level2)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    OHOS::HDI::Usb::V1_0::UsbPipe pipe = {interfaceId, pointId};
    sptr<UsbdBulkCallbackTest> usbdBulkCallback = new UsbdBulkCallbackTest();
    if (usbdBulkCallback == nullptr) {
        HDF_LOGE("%{public}s:UsbdBulkCallbackTest new failed.", __func__);
        exit(0);
    }
    auto ret = g_usbInterface->RegBulkCallback(dev, pipe, usbdBulkCallback);
    ASSERT_EQ(ret, 0);
    dev = {1, 255};
    ret = g_usbInterface->UnRegBulkCallback(dev, pipe);
    ASSERT_NE(ret, 0);
    dev = dev_;
    ret = g_usbInterface->UnRegBulkCallback(dev, pipe);
    ASSERT_EQ(ret, 0);
}
} // namespace
