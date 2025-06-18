/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
const int32_t TRANSFER_TIME_OUT = 1000;
const int32_t CTL_VALUE = 0x100;

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::USB;
using namespace std;
using namespace OHOS::HDI::Usb::V1_0;

UsbDev UsbdTransferTestAdditional::dev_ = {0, 0};
sptr<UsbSubscriberTest> UsbdTransferTestAdditional::subscriber_ = nullptr;

namespace {
sptr<IUsbInterface> g_usbInterface = nullptr;

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
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_2100
 * @tc.name: testHdiUsbTransferTestControlTransferWrite001
 * @tc.desc: Performs control transfer for endpoint 0 of the device.
 * dev = (0,255). ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, 0, 0, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite001, TestSize.Level2)
{
    struct UsbDev dev = {0, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, 0, 0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_2200
 * @tc.name: testHdiUsbTransferTestControlTransferWrite002
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (0,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_DESCRIPTOR, 0, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite002, TestSize.Level2)
{
    struct UsbDev dev = {0, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_DESCRIPTOR,
                                         0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_2300
 * @tc.name: testHdiUsbTransferTestControlTransferWrite003
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (0,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_CONFIGURATION, 0, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite003, TestSize.Level2)
{
    struct UsbDev dev = {0, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE,
                                         USB_DDK_REQ_GET_CONFIGURATION, 0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_2400
 * @tc.name: testHdiUsbTransferTestControlTransferWrite004
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (0,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_INTERFACE, 0, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite004, TestSize.Level2)
{
    struct UsbDev dev = {0, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_INTERFACE, 0,
                                         0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_2500
 * @tc.name: testHdiUsbTransferTestControlTransferWrite005
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (0,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_SYNCH_FRAME, 0, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite005, TestSize.Level2)
{
    struct UsbDev dev = {0, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_SYNCH_FRAME, 0,
                                         0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_2600
 * @tc.name: testHdiUsbTransferTestControlTransferWrite006
 * @tc.desc: Performs control transfer for endpoint 0 of the device.
 * dev = (255,0). ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, 0, 0, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite006, TestSize.Level2)
{
    struct UsbDev dev = {255, 0};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, 0, 0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_2700
 * @tc.name: testHdiUsbTransferTestControlTransferWrite007
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,0).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_DESCRIPTOR, 0, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite007, TestSize.Level2)
{
    struct UsbDev dev = {255, 0};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_DESCRIPTOR,
                                         0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_2800
 * @tc.name: testHdiUsbTransferTestControlTransferWrite008
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,0).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_CONFIGURATION, 0, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite008, TestSize.Level2)
{
    struct UsbDev dev = {255, 0};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE,
                                         USB_DDK_REQ_GET_CONFIGURATION, 0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_2900
 * @tc.name: testHdiUsbTransferTestControlTransferWrite009
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,0).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_INTERFACE, 0, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite009, TestSize.Level2)
{
    struct UsbDev dev = {255, 0};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_INTERFACE, 0,
                                         0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_3000
 * @tc.name: testHdiUsbTransferTestControlTransferWrite010
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,0).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_SYNCH_FRAME, 0, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite010, TestSize.Level2)
{
    struct UsbDev dev = {255, 0};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_SYNCH_FRAME, 0,
                                         0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_3100
 * @tc.name: testHdiUsbTransferTestControlTransferWrite011
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE, 0, 0, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite011, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE, 0, 0, 0,
                                         TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_3200
 * @tc.name: testHdiUsbTransferTestControlTransferWrite012
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE, USB_DDK_REQ_GET_DESCRIPTOR, 0, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite012, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE,
                                         USB_DDK_REQ_GET_DESCRIPTOR, 0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_3300
 * @tc.name: testHdiUsbTransferTestControlTransferWrite013
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE, USB_DDK_REQ_GET_CONFIGURATION, 0, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite013, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE,
                                         USB_DDK_REQ_GET_CONFIGURATION, 0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_3400
 * @tc.name: testHdiUsbTransferTestControlTransferWrite014
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE, USB_DDK_REQ_GET_INTERFACE, 0, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite014, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE, USB_DDK_REQ_GET_INTERFACE,
                                         0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_3500
 * @tc.name: testHdiUsbTransferTestControlTransferWrite015
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE, USB_DDK_REQ_SYNCH_FRAME, 0, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite015, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE, USB_DDK_REQ_SYNCH_FRAME,
                                         0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_3600
 * @tc.name: testHdiUsbTransferTestControlTransferWrite016
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, 0, 0, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite016, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, 0, 0, 0,
                                         TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_3700
 * @tc.name: testHdiUsbTransferTestControlTransferWrite017
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, USB_DDK_REQ_GET_DESCRIPTOR, 0, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite017, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, USB_DDK_REQ_GET_DESCRIPTOR,
                                         0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_3800
 * @tc.name: testHdiUsbTransferTestControlTransferWrite018
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, USB_DDK_REQ_GET_CONFIGURATION, 0, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite018, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT,
                                         USB_DDK_REQ_GET_CONFIGURATION, 0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_3900
 * @tc.name: testHdiUsbTransferTestControlTransferWrite019
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, USB_DDK_REQ_GET_INTERFACE, 0, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite019, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, USB_DDK_REQ_GET_INTERFACE,
                                         0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_4000
 * @tc.name: testHdiUsbTransferTestControlTransferWrite020
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, USB_DDK_REQ_SYNCH_FRAME, 0, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite020, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, USB_DDK_REQ_SYNCH_FRAME, 0,
                                         0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_4100
 * @tc.name: testHdiUsbTransferTestControlTransferWrite021
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, 0, 0, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite021, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, 0, 0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_4200
 * @tc.name: testHdiUsbTransferTestControlTransferWrite022
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, USB_DDK_REQ_GET_DESCRIPTOR, 0, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite022, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, USB_DDK_REQ_GET_DESCRIPTOR, 0,
                                         0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_4300
 * @tc.name: testHdiUsbTransferTestControlTransferWrite023
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, USB_DDK_REQ_GET_CONFIGURATION, 0, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite023, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, USB_DDK_REQ_GET_CONFIGURATION,
                                         0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_4400
 * @tc.name: testHdiUsbTransferTestControlTransferWrite024
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, USB_DDK_REQ_GET_INTERFACE, 0, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite024, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, USB_DDK_REQ_GET_INTERFACE, 0,
                                         0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_4500
 * @tc.name: testHdiUsbTransferTestControlTransferWrite025
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, USB_DDK_REQ_SYNCH_FRAME, 0, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite025, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, USB_DDK_REQ_SYNCH_FRAME, 0, 0,
                                         TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_4600
 * @tc.name: testHdiUsbTransferTestControlTransferWrite026
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, 0, 0, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite026, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, 0, 0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_4700
 * @tc.name: testHdiUsbTransferTestControlTransferWrite027
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_DESCRIPTOR, 0, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite027, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_DESCRIPTOR,
                                         0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_4800
 * @tc.name: testHdiUsbTransferTestControlTransferWrite028
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_CONFIGURATION, 0, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite028, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE,
                                         USB_DDK_REQ_GET_CONFIGURATION, 0, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_4900
 * @tc.name: testHdiUsbTransferTestControlTransferWrite029
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_INTERFACE, 0, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite029, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_INTERFACE, 0,
                                         0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_5000
 * @tc.name: testHdiUsbTransferTestControlTransferWrite030
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_SYNCH_FRAME, 0, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite030, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_SYNCH_FRAME, 0,
                                         0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_5100
 * @tc.name: testHdiUsbTransferTestControlTransferWrite031
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, 0, CTL_VALUE, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite031, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, 0, CTL_VALUE, 0,
                                         TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_5200
 * @tc.name: testHdiUsbTransferTestControlTransferWrite032
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_DESCRIPTOR, CTL_VALUE, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite032, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_DESCRIPTOR,
                                         CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_5300
 * @tc.name: testHdiUsbTransferTestControlTransferWrite033
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_CONFIGURATION, CTL_VALUE, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite033, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE,
                                         USB_DDK_REQ_GET_CONFIGURATION, CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_5400
 * @tc.name: testHdiUsbTransferTestControlTransferWrite034
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_INTERFACE, CTL_VALUE, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite034, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_INTERFACE,
                                         CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_5500
 * @tc.name: testHdiUsbTransferTestControlTransferWrite035
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_SYNCH_FRAME, CTL_VALUE, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite035, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_SYNCH_FRAME,
                                         CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_5600
 * @tc.name: testHdiUsbTransferTestControlTransferWrite036
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE, 0, CTL_VALUE, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite036, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE, 0, CTL_VALUE, 0,
                                         TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_5700
 * @tc.name: testHdiUsbTransferTestControlTransferWrite037
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE, USB_DDK_REQ_GET_DESCRIPTOR, CTL_VALUE, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite037, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE,
                                         USB_DDK_REQ_GET_DESCRIPTOR, CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_5800
 * @tc.name: testHdiUsbTransferTestControlTransferWrite038
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE, USB_DDK_REQ_GET_CONFIGURATION, CTL_VALUE, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite038, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE,
                                         USB_DDK_REQ_GET_CONFIGURATION, CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_5900
 * @tc.name: testHdiUsbTransferTestControlTransferWrite039
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE, USB_DDK_REQ_GET_INTERFACE, CTL_VALUE, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite039, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE, USB_DDK_REQ_GET_INTERFACE,
                                         CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_6000
 * @tc.name: testHdiUsbTransferTestControlTransferWrite040
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE, USB_DDK_REQ_SYNCH_FRAME, CTL_VALUE, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite040, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_INTERFACE, USB_DDK_REQ_SYNCH_FRAME,
                                         CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_6100
 * @tc.name: testHdiUsbTransferTestControlTransferWrite041
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, 0, CTL_VALUE, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite041, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, 0, CTL_VALUE, 0,
                                         TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_6200
 * @tc.name: testHdiUsbTransferTestControlTransferWrite042
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, USB_DDK_REQ_GET_DESCRIPTOR, CTL_VALUE, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite042, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, USB_DDK_REQ_GET_DESCRIPTOR,
                                         CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_6300
 * @tc.name: testHdiUsbTransferTestControlTransferWrite043
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, USB_DDK_REQ_GET_CONFIGURATION, CTL_VALUE, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite043, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT,
                                         USB_DDK_REQ_GET_CONFIGURATION, CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_6400
 * @tc.name: testHdiUsbTransferTestControlTransferWrite044
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, USB_DDK_REQ_GET_INTERFACE, CTL_VALUE, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite044, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, USB_DDK_REQ_GET_INTERFACE,
                                         CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_6500
 * @tc.name: testHdiUsbTransferTestControlTransferWrite045
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, USB_DDK_REQ_SYNCH_FRAME, CTL_VALUE, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite045, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_ENDPOINT, USB_DDK_REQ_SYNCH_FRAME,
                                         CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_6600
 * @tc.name: testHdiUsbTransferTestControlTransferWrite046
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, 0, CTL_VALUE, 0,TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite046, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, 0, CTL_VALUE, 0,
                                         TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_6700
 * @tc.name: testHdiUsbTransferTestControlTransferWrite047
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, USB_DDK_REQ_GET_DESCRIPTOR, CTL_VALUE,
 * 0,TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite047, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, USB_DDK_REQ_GET_DESCRIPTOR,
                                         CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_6800
 * @tc.name: testHdiUsbTransferTestControlTransferWrite048
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, USB_DDK_REQ_GET_CONFIGURATION, CTL_VALUE,
 * 0,TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite048, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, USB_DDK_REQ_GET_CONFIGURATION,
                                         CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_6900
 * @tc.name: testHdiUsbTransferTestControlTransferWrite049
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, USB_DDK_REQ_GET_INTERFACE, CTL_VALUE,
 * 0,TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite049, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, USB_DDK_REQ_GET_INTERFACE,
                                         CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_7000
 * @tc.name: testHdiUsbTransferTestControlTransferWrite050
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (255,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, USB_DDK_REQ_SYNCH_FRAME, CTL_VALUE,
 * 0,TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite050, TestSize.Level2)
{
    struct UsbDev dev = {255, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_OTHER, USB_DDK_REQ_SYNCH_FRAME,
                                         CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_7100
 * @tc.name: testHdiUsbTransferTestControlTransferWrite051
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (0,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, 0, CTL_VALUE, 0, TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite051, TestSize.Level2)
{
    struct UsbDev dev = {0, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, 0, CTL_VALUE, 0,
                                         TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_7200
 * @tc.name: testHdiUsbTransferTestControlTransferWrite052
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (0,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_DESCRIPTOR, CTL_VALUE, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite052, TestSize.Level2)
{
    struct UsbDev dev = {0, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_DESCRIPTOR,
                                         CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_7300
 * @tc.name: testHdiUsbTransferTestControlTransferWrite053
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (0,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_CONFIGURATION, CTL_VALUE, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite053, TestSize.Level2)
{
    struct UsbDev dev = {0, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE,
                                         USB_DDK_REQ_GET_CONFIGURATION, CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_7400
 * @tc.name: testHdiUsbTransferTestControlTransferWrite054
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (0,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_INTERFACE, CTL_VALUE, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite054, TestSize.Level2)
{
    struct UsbDev dev = {0, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_GET_INTERFACE,
                                         CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.number: SUB_Driver_Usb_TransferTest_ControlTransferWrite_7500
 * @tc.name: testHdiUsbTransferTestControlTransferWrite055
 * @tc.desc: Performs control transfer for endpoint 0 of the device. dev = (0,255).
 * ctrlparmas = (USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_SYNCH_FRAME, CTL_VALUE, 0,
 * TRANSFER_TIME_OUT).
 */
HWTEST_F(UsbdTransferTestAdditional, testHdiUsbTransferTestControlTransferWrite055, TestSize.Level2)
{
    struct UsbDev dev = {0, 255};
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    bufferData.push_back(SAMPLE_DATA_1);
    bufferData.push_back(SAMPLE_DATA_2);
    bufferData.push_back(SAMPLE_DATA_3);
    struct UsbCtrlTransfer ctrlparmas = {USB_ENDPOINT_DIR_OUT | USB_REQUEST_TARGET_DEVICE, USB_DDK_REQ_SYNCH_FRAME,
                                         CTL_VALUE, 0, TRANSFER_TIME_OUT};
    auto ret = g_usbInterface->ControlTransferWrite(dev, ctrlparmas, bufferData);
    ASSERT_NE(ret, 0);
}

} // namespace
