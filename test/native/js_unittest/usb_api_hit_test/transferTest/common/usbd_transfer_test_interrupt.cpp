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
#include "v2_0/iusb_host_interface.h"
#include "v2_0/iusb_port_interface.h"

const int SLEEP_TIME = 3;
const uint8_t BUS_NUM_INVALID = 255;
const uint8_t DEV_ADDR_INVALID = 255;
const uint32_t MAX_BUFFER_LENGTH = 255;
const uint8_t INTERFACEID_OK = 1;
const uint8_t INTERFACEID_EFFECTIVE = 0;
const uint8_t PIPE_ENDPOINTID_INVALID = 244;
const uint8_t PIPE_INTERFACEID_INVALID = 244;
const uint8_t POINTID_INTERRUPT_IN = 129;
// data interface have 2 point : 1->bulk_out 2->bulk_in
const uint8_t POINTID_BULK_IN = USB_ENDPOINT_DIR_IN | 2;
const uint8_t POINTID_BULK_OUT = USB_ENDPOINT_DIR_OUT | 1;
const int32_t ASHMEM_MAX_SIZE = 1024;
const int32_t TRANSFER_TIME_OUT = 1000;

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::USB;
using namespace std;
using namespace OHOS::HDI::Usb::V2_0;

UsbDev UsbdTransferTest::dev_ = {0, 0};
sptr<UsbSubscriberTest> UsbdTransferTest::subscriber_ = nullptr;

namespace {
sptr<IUsbHostInterface> g_usbHostInterface = nullptr;
sptr<IUsbPortInterface> g_usbPortInterface = nullptr;

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

int32_t SwitchErrCode(int32_t ret)
{
    return ret == HDF_ERR_NOT_SUPPORT ? HDF_SUCCESS : ret;
}

void UsbdTransferTest::SetUpTestCase(void)
{
    g_usbHostInterface = IUsbHostInterface::Get(true);
    g_usbPortInterface = IUsbPortInterface::Get();
    if (g_usbHostInterface == nullptr || g_usbPortInterface == nullptr) {
        HDF_LOGE("%{public}s:g_usbHostInterface or g_usbPortInterface is nullptr.", __func__);
        exit(0);
    }
    const int32_t DEFAULT_PORT_ID = 1;
    const int32_t DEFAULT_ROLE_HOST = 1;
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, DEFAULT_ROLE_HOST, DEFAULT_ROLE_HOST);
    sleep(SLEEP_TIME);
    HDF_LOGI("UsbdTransferTest::[Device] %{public}d SetPortRole=%{public}d", __LINE__, ret);
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
    if (g_usbHostInterface->BindUsbdHostSubscriber(subscriber_) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: bind usbd subscriber_ failed\n", __func__);
        exit(0);
    }

    std::cout << "please connect device, press enter to continue" << std::endl;
    int c;
        do {
        c = getchar();
    } while (c != '\n' && c != EOF);

    dev_ = { subscriber_->busNum_, subscriber_->devAddr_ };
    ret = g_usbHostInterface->OpenDevice(dev_);
    HDF_LOGI("UsbdTransferTest:: %{public}d OpenDevice=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

void UsbdTransferTest::TearDownTestCase(void)
{
    g_usbHostInterface->UnbindUsbdHostSubscriber(subscriber_);
    dev_ = { subscriber_->busNum_, subscriber_->devAddr_ };
    auto ret = g_usbHostInterface->CloseDevice(dev_);
    HDF_LOGI("UsbdTransferTest:: %{public}d Close=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

void UsbdTransferTest::SetUp(void) {}

void UsbdTransferTest::TearDown(void) {}

/**
 * @tc.name: SUB_USB_HostManager_HDI_TranFunc_0400
 * @tc.desc: Test functions to InterruptTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, SUB_USB_HostManager_HDI_TranFunc_0400, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_EFFECTIVE;
    uint8_t pointId = POINTID_INTERRUPT_IN;
    auto ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranFunc_0400 "
        "%{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V2_0::UsbPipe pipe = { interfaceId, pointId };
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    ret = g_usbHostInterface->InterruptTransferRead(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranFunc_0400 "
        "%{public}d UsbdInterruptTransferRead=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_TranCompatibility_3900
 * @tc.desc: Test functions to InterruptTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.desc: Negative test: parameters exception, busNum error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, SUB_USB_HostManager_HDI_TranCompatibility_3900, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_IN;
    auto ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_3900 "
        "%{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    dev.busNum = BUS_NUM_INVALID;
    OHOS::HDI::Usb::V2_0::UsbPipe pipe = { interfaceId, pointId };
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    ret = g_usbHostInterface->InterruptTransferRead(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_3900 "
        "%{public}d UsbdInterruptTransferRead=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_TranCompatibility_4000
 * @tc.desc: Test functions to InterruptTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.desc: Negative test: parameters exception, devAddr error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, SUB_USB_HostManager_HDI_TranCompatibility_4000, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_IN;
    auto ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4000 "
        "%{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    dev.devAddr = DEV_ADDR_INVALID;
    OHOS::HDI::Usb::V2_0::UsbPipe pipe = { interfaceId, pointId };
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    ret = g_usbHostInterface->InterruptTransferRead(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_TranCompatibility_4100
 * @tc.desc: Test functions to InterruptTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.desc: Negative test: parameters exception, intfId error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, SUB_USB_HostManager_HDI_TranCompatibility_4100, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_IN;
    auto ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4100 "
        "%{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V2_0::UsbPipe pipe = { interfaceId, pointId };
    pipe.intfId = PIPE_INTERFACEID_INVALID;
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    ret = g_usbHostInterface->InterruptTransferRead(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4100 "
        "%{public}d UsbdInterruptTransferRead=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_TranCompatibility_4200
 * @tc.desc: Test functions to InterruptTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.desc: Negative test: parameters exception, endpointId error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, SUB_USB_HostManager_HDI_TranCompatibility_4200, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_IN;
    auto ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4200 "
        "%{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V2_0::UsbPipe pipe = { interfaceId, pointId };
    pipe.endpointId = PIPE_ENDPOINTID_INVALID;
    std::vector<uint8_t> bufferData(MAX_BUFFER_LENGTH);
    ret = g_usbHostInterface->InterruptTransferRead(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4200 "
        "%{public}d UsbdInterruptTransferRead=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_TranFunc_1000
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.desc: Positive test: parameters correctly
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, SUB_USB_HostManager_HDI_TranFunc_1000, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranFunc_1000 "
        "%{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V2_0::UsbPipe pipe = { interfaceId, pointId };
    std::vector<uint8_t> bufferData = {'i', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', '1'};
    ret = g_usbHostInterface->InterruptTransferWrite(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranFunc_1000 "
        "%{public}d InterruptTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_TranCompatibility_4300
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.desc: Negative test: parameters exception, busNum error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, SUB_USB_HostManager_HDI_TranCompatibility_4300, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4300 "
        "%{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V2_0::UsbPipe pipe = { interfaceId, pointId };
    dev.busNum = BUS_NUM_INVALID;
    std::vector<uint8_t> bufferData = {'i', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', '2'};
    ret = g_usbHostInterface->InterruptTransferWrite(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4300 "
        "%{public}d InterruptTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_TranCompatibility_4400
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.desc: Negative test: parameters exception, devAddr error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, SUB_USB_HostManager_HDI_TranCompatibility_4400, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4400 "
        "%{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    dev.devAddr = DEV_ADDR_INVALID;
    OHOS::HDI::Usb::V2_0::UsbPipe pipe = { interfaceId, pointId };
    std::vector<uint8_t> bufferData = {'i', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', '3'};
    ret = g_usbHostInterface->InterruptTransferWrite(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4400 "
        "%{public}d InterruptTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_TranCompatibility_4500
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.desc: Negative test: parameters exception, intfId error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, SUB_USB_HostManager_HDI_TranCompatibility_4500, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4500 "
        "%{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V2_0::UsbPipe pipe = { interfaceId, pointId };
    pipe.intfId = PIPE_INTERFACEID_INVALID;
    std::vector<uint8_t> bufferData = {'i', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', '4'};
    ret = g_usbHostInterface->InterruptTransferWrite(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4500 "
        "%{public}d InterruptTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_TranCompatibility_4600
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.desc: Negative test: parameters exception, endpointId error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, SUB_USB_HostManager_HDI_TranCompatibility_4600, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4600 "
        "%{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V2_0::UsbPipe pipe = { interfaceId, pointId };
    pipe.endpointId = PIPE_ENDPOINTID_INVALID;
    std::vector<uint8_t> bufferData = {'i', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', '5'};
    ret = g_usbHostInterface->InterruptTransferWrite(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4600 "
        "%{public}d InterruptTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_TranCompatibility_4700
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.desc: Negative test: parameters exception, busNum && devAddr error
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, SUB_USB_HostManager_HDI_TranCompatibility_4700, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4700 "
        "%{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V2_0::UsbPipe pipe = { interfaceId, pointId };
    dev.busNum = BUS_NUM_INVALID;
    dev.devAddr = DEV_ADDR_INVALID;
    std::vector<uint8_t> bufferData = {'i', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', '6'};
    ret = g_usbHostInterface->InterruptTransferWrite(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4700 "
        "%{public}d InterruptTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_TranCompatibility_4800
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.desc: Negative test: parameters exception, ClaimInterface failed first
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, SUB_USB_HostManager_HDI_TranCompatibility_4800, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = PIPE_ENDPOINTID_INVALID;
    auto ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4800 "
        "%{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V2_0::UsbPipe pipe = { interfaceId, pointId };
    std::vector<uint8_t> bufferData = {'i', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', '7'};
    ret = g_usbHostInterface->InterruptTransferWrite(dev, pipe, TRANSFER_TIME_OUT, bufferData);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4800 "
        "%{public}d InterruptTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SUB_USB_HostManager_HDI_TranCompatibility_4900
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * Positive test: parameters correctly, different in timeout
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, SUB_USB_HostManager_HDI_TranCompatibility_4900, Function | MediumTest | Level1)
{
    struct UsbDev dev = dev_;
    uint8_t interfaceId = INTERFACEID_OK;
    uint8_t pointId = POINTID_BULK_OUT;
    auto ret = g_usbHostInterface->ClaimInterface(dev, interfaceId, 1);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4900 "
        "%{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_EQ(0, ret);
    OHOS::HDI::Usb::V2_0::UsbPipe pipe = { interfaceId, pointId };
    std::vector<uint8_t> bufferData = {'i', 'n', 't', 'w', 'r', 'i', 't', 'e', '0', '8'};
    ret = g_usbHostInterface->InterruptTransferWrite(dev, pipe, -1, bufferData);
    HDF_LOGI("UsbdTransferTest::SUB_USB_HostManager_HDI_TranCompatibility_4900 "
        "%{public}d InterruptTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_NE(0, ret);
}
} // namespace
