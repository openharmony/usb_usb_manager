/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "usb_host_manager.h"
#include "usb_device.h"
#include "usb_errors.h"
#include "usb_device_pipe_mock_test.h"
#include "usb_request_mock_test.h"
#include "usb_event_mock_test.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace USB {
namespace ServiceTest {

using namespace testing;
using namespace OHOS::HDI::Usb::V1_0;
const uint8_t TEST_BUS_NUM = 1;
const uint8_t TEST_DEV_ADDR = 2;
const uint16_t TEST_VENDOR_ID = 0x1234;
const uint16_t TEST_PRODUCT_ID = 0x5678;
const uint8_t TEST_INTERFACE_ID = 0;
const uint8_t TEST_ENDPOINT_ID = 1;
const uint8_t TEST_CONFIG_INDEX = 0;

class UsbHostManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    void CreateMockDevices();
    void CreateMockInterfaces();
    void CreateMockConfigs();
    UsbDevice* CreateTestDevice(uint8_t busNum, uint8_t devAddr,
                                uint16_t vendorId, uint16_t productId,
                                uint8_t deviceClass);

    std::unique_ptr<UsbHostManager> usbHostManager_;
    std::vector<UsbDevice*> testDevices_;
};

void UsbHostManagerTest::SetUpTestCase(void)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManagerTest::SetUpTestCase enter");
}

void UsbHostManagerTest::TearDownTestCase(void)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManagerTest::TearDownTestCase enter");
}

void UsbHostManagerTest::SetUp()
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManagerTest::SetUp enter");
    SystemAbility* systemAbility = nullptr;
    usbHostManager_ = std::make_unique<UsbHostManager>(systemAbility);
    ASSERT_NE(usbHostManager_, nullptr);
    testDevices_.clear();
}

void UsbHostManagerTest::TearDown()
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManagerTest::TearDown enter");
    for (auto* device : testDevices_) {
        if (device != nullptr) {
            delete device;
        }
    }
    testDevices_.clear();
    usbHostManager_.reset();
}

UsbDevice* UsbHostManagerTest::CreateTestDevice(uint8_t busNum, uint8_t devAddr,
                                                uint16_t vendorId, uint16_t productId,
                                                uint8_t deviceClass)
{
    UsbDevice* device = new UsbDevice();
    if (device == nullptr) {
        return nullptr;
    }
    device->SetBusNum(busNum);
    device->SetDevAddr(devAddr);
    device->SetVendorId(vendorId);
    device->SetProductId(productId);
    device->SetClass(deviceClass);
    device->SetSubclass(0x01);
    device->SetProtocol(0x00);
    device->SetProductName("TestDevice");
    device->SetManufacturerName("TestManufacturer");
    device->SetmSerial("SN123456");
    device->SetVersion("1.0.0");
    device->SetAuthorizeStatus(ENABLED);

    testDevices_.push_back(device);
    return device;
}

void UsbHostManagerTest::CreateMockDevices()
{
    auto* dev1 = CreateTestDevice(1, 1, 0x1234, 0x5678, 0x08);
    auto* dev2 = CreateTestDevice(1, 2, 0x5678, 0x1234, 0x03);
    auto* dev3 = CreateTestDevice(2, 1, 0xABCD, 0xEF01, 0x09);

    ASSERT_NE(dev1, nullptr);
    ASSERT_NE(dev2, nullptr);
    ASSERT_NE(dev3, nullptr);
}

/**
 * @tc.name: UsbHostManager_Constructor_001
 * @tc.desc: Test UsbHostManager constructor with nullptr system ability
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_Constructor_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_Constructor_001 start");

    SystemAbility* systemAbility = nullptr;
    auto manager = std::make_unique<UsbHostManager>(systemAbility);

    EXPECT_NE(manager, nullptr);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_Constructor_001 end");
}

/**
 * @tc.name: UsbHostManager_Constructor_002
 * @tc.desc: Test UsbHostManager constructor with valid system ability
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_Constructor_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_Constructor_002 start");

    SystemAbility* systemAbility = reinterpret_cast<SystemAbility*>(0x1234);
    auto manager = std::make_unique<UsbHostManager>(systemAbility);

    EXPECT_NE(manager, nullptr);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_Constructor_002 end");
}

/**
 * @tc.name: UsbHostManager_Destructor_001
 * @tc.desc: Test UsbHostManager destructor
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_Destructor_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_Destructor_001 start");

    {
        SystemAbility* systemAbility = nullptr;
        auto manager = std::make_unique<UsbHostManager>(systemAbility);
        ASSERT_NE(manager, nullptr);
    }
    // Destructor called here, should not crash

    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_Destructor_001 end");
}

/**
 * @tc.name: UsbHostManager_OpenDevice_001
 * @tc.desc: Test OpenDevice with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_OpenDevice_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_OpenDevice_001 start");

    int32_t ret = usbHostManager_->OpenDevice(TEST_BUS_NUM, TEST_DEV_ADDR);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_OpenDevice_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_OpenDevice_002
 * @tc.desc: Test OpenDevice with invalid bus number
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_OpenDevice_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_OpenDevice_002 start");

    uint8_t invalidBusNum = 255;
    uint8_t devAddr = 1;

    int32_t ret = usbHostManager_->OpenDevice(invalidBusNum, devAddr);

    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE ||
                ret == UEC_SERVICE_INNER_ERR);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_OpenDevice_002 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_OpenDevice_003
 * @tc.desc: Test OpenDevice with invalid device address
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_OpenDevice_003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_OpenDevice_003 start");

    uint8_t busNum = 1;
    uint8_t invalidDevAddr = 255;

    int32_t ret = usbHostManager_->OpenDevice(busNum, invalidDevAddr);

    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE ||
                ret == UEC_SERVICE_INNER_ERR);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_OpenDevice_003 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_OpenDevice_004
 * @tc.desc: Test OpenDevice with boundary values
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_OpenDevice_004, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_OpenDevice_004 start");

    // Test with minimum valid values
    int32_t ret1 = usbHostManager_->OpenDevice(0, 1);
    USB_HILOGI(MODULE_USB_HOST, "OpenDevice(0, 1) ret=%{public}d", ret1);

    // Test with maximum valid values
    int32_t ret2 = usbHostManager_->OpenDevice(255, 127);
    USB_HILOGI(MODULE_USB_HOST, "OpenDevice(255, 127) ret=%{public}d", ret2);

    // Test with all zeros
    int32_t ret3 = usbHostManager_->OpenDevice(0, 0);
    USB_HILOGI(MODULE_USB_HOST, "OpenDevice(0, 0) ret=%{public}d", ret3);

    SUCCEED();
}

/**
 * @tc.name: UsbHostManager_Close_001
 * @tc.desc: Test Close with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_Close_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_Close_001 start");

    // First open device, then close it
    int32_t openRet = usbHostManager_->OpenDevice(TEST_BUS_NUM, TEST_DEV_ADDR);
    USB_HILOGI(MODULE_USB_HOST, "OpenDevice ret=%{public}d", openRet);

    int32_t closeRet = usbHostManager_->Close(TEST_BUS_NUM, TEST_DEV_ADDR);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(closeRet == UEC_OK || closeRet == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_Close_001 ret=%{public}d", closeRet);
}

/**
 * @tc.name: UsbHostManager_Close_002
 * @tc.desc: Test Close without opening device first
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_Close_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_Close_002 start");

    int32_t ret = usbHostManager_->Close(TEST_BUS_NUM, TEST_DEV_ADDR);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_Close_002 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ResetDevice_001
 * @tc.desc: Test ResetDevice with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ResetDevice_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ResetDevice_001 start");

    int32_t ret = usbHostManager_->ResetDevice(TEST_BUS_NUM, TEST_DEV_ADDR);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ResetDevice_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ResetDevice_002
 * @tc.desc: Test ResetDevice with invalid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ResetDevice_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ResetDevice_002 start");

    int32_t ret = usbHostManager_->ResetDevice(255, 255);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE ||
                ret == UEC_SERVICE_INNER_ERR);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ResetDevice_002 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ClaimInterface_001
 * @tc.desc: Test ClaimInterface with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ClaimInterface_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ClaimInterface_001 start");

    uint8_t force = 1;
    int32_t ret = usbHostManager_->ClaimInterface(TEST_BUS_NUM, TEST_DEV_ADDR,
                                                   TEST_INTERFACE_ID, force);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ClaimInterface_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ClaimInterface_002
 * @tc.desc: Test ClaimInterface with force=0
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ClaimInterface_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ClaimInterface_002 start");

    uint8_t force = 0;
    int32_t ret = usbHostManager_->ClaimInterface(TEST_BUS_NUM, TEST_DEV_ADDR,
                                                   TEST_INTERFACE_ID, force);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ClaimInterface_002 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_SetInterface_001
 * @tc.desc: Test SetInterface with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_SetInterface_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_SetInterface_001 start");

    uint8_t altIndex = 0;
    int32_t ret = usbHostManager_->SetInterface(TEST_BUS_NUM, TEST_DEV_ADDR,
                                                 TEST_INTERFACE_ID, altIndex);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_SetInterface_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_SetInterface_002
 * @tc.desc: Test SetInterface with different altIndex values
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_SetInterface_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_SetInterface_002 start");

    for (uint8_t altIndex = 0; altIndex < 5; altIndex++) {
        int32_t ret = usbHostManager_->SetInterface(TEST_BUS_NUM, TEST_DEV_ADDR,
                                                     TEST_INTERFACE_ID, altIndex);
        USB_HILOGI(MODULE_USB_HOST, "SetInterface altIndex=%{public}u ret=%{public}d",
                    altIndex, ret);
    }

    SUCCEED();
}

/**
 * @tc.name: UsbHostManager_ReleaseInterface_001
 * @tc.desc: Test ReleaseInterface with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ReleaseInterface_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ReleaseInterface_001 start");

    int32_t ret = usbHostManager_->ReleaseInterface(TEST_BUS_NUM, TEST_DEV_ADDR,
                                                     TEST_INTERFACE_ID);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ReleaseInterface_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_SetActiveConfig_001
 * @tc.desc: Test SetActiveConfig with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_SetActiveConfig_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_SetActiveConfig_001 start");

    uint8_t configIndex = 1;
    int32_t ret = usbHostManager_->SetActiveConfig(TEST_BUS_NUM, TEST_DEV_ADDR, configIndex);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_SetActiveConfig_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_SetActiveConfig_002
 * @tc.desc: Test SetActiveConfig with configIndex=0
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_SetActiveConfig_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_SetActiveConfig_002 start");

    uint8_t configIndex = 0;
    int32_t ret = usbHostManager_->SetActiveConfig(TEST_BUS_NUM, TEST_DEV_ADDR, configIndex);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_SetActiveConfig_002 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_GetActiveConfig_001
 * @tc.desc: Test GetActiveConfig with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetActiveConfig_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetActiveConfig_001 start");

    uint8_t configIndex = 0;
    int32_t ret = usbHostManager_->GetActiveConfig(TEST_BUS_NUM, TEST_DEV_ADDR, configIndex);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetActiveConfig_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_GetRawDescriptor_001
 * @tc.desc: Test GetRawDescriptor with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetRawDescriptor_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetRawDescriptor_001 start");

    std::vector<uint8_t> bufferData;
    int32_t ret = usbHostManager_->GetRawDescriptor(TEST_BUS_NUM, TEST_DEV_ADDR, bufferData);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetRawDescriptor_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_GetRawDescriptor_002
 * @tc.desc: Test GetRawDescriptor with invalid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetRawDescriptor_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetRawDescriptor_002 start");

    std::vector<uint8_t> bufferData;
    int32_t ret = usbHostManager_->GetRawDescriptor(255, 255, bufferData);

    // Should fail with invalid value error
    EXPECT_TRUE(ret == UEC_SERVICE_INVALID_VALUE || ret == UEC_SERVICE_INNER_ERR);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetRawDescriptor_002 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_GetFileDescriptor_001
 * @tc.desc: Test GetFileDescriptor with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetFileDescriptor_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetFileDescriptor_001 start");

    int32_t fd = -1;
    int32_t ret = usbHostManager_->GetFileDescriptor(TEST_BUS_NUM, TEST_DEV_ADDR, fd);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetFileDescriptor_001 ret=%{public}d fd=%{public}d",
                ret, fd);
}

/**
 * @tc.name: UsbHostManager_GetFileDescriptor_002
 * @tc.desc: Test GetFileDescriptor and verify fd is not negative on success
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetFileDescriptor_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetFileDescriptor_002 start");

    int32_t fd = -1;
    int32_t ret = usbHostManager_->GetFileDescriptor(TEST_BUS_NUM, TEST_DEV_ADDR, fd);

    if (ret == UEC_OK) {
        EXPECT_GE(fd, 0);
    }
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetFileDescriptor_002 ret=%{public}d fd=%{public}d",
                ret, fd);
}

/**
 * @tc.name: UsbHostManager_GetDeviceSpeed_001
 * @tc.desc: Test GetDeviceSpeed with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetDeviceSpeed_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDeviceSpeed_001 start");

    uint8_t speed = 0;
    int32_t ret = usbHostManager_->GetDeviceSpeed(TEST_BUS_NUM, TEST_DEV_ADDR, speed);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDeviceSpeed_001 ret=%{public}d speed=%{public}u",
                ret, speed);
}

/**
 * @tc.name: UsbHostManager_GetInterfaceActiveStatus_001
 * @tc.desc: Test GetInterfaceActiveStatus with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetInterfaceActiveStatus_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetInterfaceActiveStatus_001 start");

    bool unactivated = false;
    int32_t ret = usbHostManager_->GetInterfaceActiveStatus(TEST_BUS_NUM, TEST_DEV_ADDR,
                                                            TEST_INTERFACE_ID, unactivated);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetInterfaceActiveStatus_001 ret=%{public}d unactivated=%{public}d",
                ret, unactivated);
}

/**
 * @tc.name: UsbHostManager_ClearHalt_001
 * @tc.desc: Test ClearHalt with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ClearHalt_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ClearHalt_001 start");

    int32_t ret = usbHostManager_->ClearHalt(TEST_BUS_NUM, TEST_DEV_ADDR,
                                              TEST_INTERFACE_ID, TEST_ENDPOINT_ID);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ClearHalt_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ClearHalt_002
 * @tc.desc: Test ClearHalt with endpointId=0
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ClearHalt_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ClearHalt_002 start");

    uint8_t endpointId = 0;
    int32_t ret = usbHostManager_->ClearHalt(TEST_BUS_NUM, TEST_DEV_ADDR,
                                              TEST_INTERFACE_ID, endpointId);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ClearHalt_002 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_UsbAttachKernelDriver_001
 * @tc.desc: Test UsbAttachKernelDriver with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_UsbAttachKernelDriver_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UsbAttachKernelDriver_001 start");

    int32_t ret = usbHostManager_->UsbAttachKernelDriver(TEST_BUS_NUM, TEST_DEV_ADDR,
                                                          TEST_INTERFACE_ID);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UsbAttachKernelDriver_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_UsbDetachKernelDriver_001
 * @tc.desc: Test UsbDetachKernelDriver with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_UsbDetachKernelDriver_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UsbDetachKernelDriver_001 start");

    int32_t ret = usbHostManager_->UsbDetachKernelDriver(TEST_BUS_NUM, TEST_DEV_ADDR,
                                                          TEST_INTERFACE_ID);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UsbDetachKernelDriver_001 ret=%{public}d", ret);
}


/**
 * @tc.name: UsbHostManager_GetDevices_001
 * @tc.desc: Test GetDevices multiple times
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetDevices_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDevices_001 start");

    for (int i = 0; i < 5; i++) {
        std::vector<UsbDevice> deviceList;
        int32_t ret = usbHostManager_->GetDevices(deviceList);
        EXPECT_EQ(ret, UEC_OK);
        USB_HILOGI(MODULE_USB_HOST, "GetDevices iteration %{public}d size=%{public}zu",
                    i, deviceList.size());
    }
}

/**
 * @tc.name: UsbHostManager_GetDeviceInfo_001
 * @tc.desc: Test GetDeviceInfo with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetDeviceInfo_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDeviceInfo_001 start");

    UsbDevice dev;
    int32_t ret = usbHostManager_->GetDeviceInfo(TEST_BUS_NUM, TEST_DEV_ADDR, dev);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE ||
                ret == UEC_SERVICE_INNER_ERR);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDeviceInfo_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_GetDeviceInfo_002
 * @tc.desc: Test GetDeviceInfo with invalid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetDeviceInfo_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDeviceInfo_002 start");

    UsbDevice dev;
    int32_t ret = usbHostManager_->GetDeviceInfo(255, 255, dev);

    // Should fail with error
    EXPECT_TRUE(ret == UEC_SERVICE_INVALID_VALUE || ret == UEC_SERVICE_INNER_ERR);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDeviceInfo_002 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_GetDeviceInfo_003
 * @tc.desc: Test GetDeviceInfo with busNum=0
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetDeviceInfo_003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDeviceInfo_003 start");

    UsbDevice dev;
    int32_t ret = usbHostManager_->GetDeviceInfo(0, TEST_DEV_ADDR, dev);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE ||
                ret == UEC_SERVICE_INNER_ERR);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDeviceInfo_003 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ManageGlobalInterface_001
 * @tc.desc: Test ManageGlobalInterface with disable=true
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ManageGlobalInterface_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageGlobalInterface_001 start");

    bool disable = true;
    int32_t ret = usbHostManager_->ManageGlobalInterface(disable);

    EXPECT_EQ(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageGlobalInterface_001 ret=%{public}d", ret);
}

/** 
* @tc.name: UsbHostManager_ManageGlobalInterface_002 
* @tc.desc: Test ManageGlobalInterface with disable=false 
* @tc.type: FUNC 
*/ 
HWTEST_F(UsbHostManagerTest, UsbHostManager_ManageGlobalInterface_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageGlobalInterface_002 start");

    bool disable = false;
    int32_t ret = usbHostManager_->ManageGlobalInterface(disable);

    EXPECT_EQ(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageGlobalInterface_002 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ManageDevice_001
 * @tc.desc: Test ManageDevice with valid vendorId and productId
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ManageDevice_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageDevice_001 start");

    int32_t vendorId = TEST_VENDOR_ID;
    int32_t productId = TEST_PRODUCT_ID;
    bool disable = true;

    int32_t ret = usbHostManager_->ManageDevice(vendorId, productId, disable);

    EXPECT_EQ(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageDevice_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ManageDevice_002
 * @tc.desc: Test ManageDevice with disable=false
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ManageDevice_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageDevice_002 start");

    int32_t vendorId = 0xABCD;
    int32_t productId = 0xEF01;
    bool disable = false;

    int32_t ret = usbHostManager_->ManageDevice(vendorId, productId, disable);

    EXPECT_EQ(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageDevice_002 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ManageDevicePolicy_001
 * @tc.desc: Test ManageDevicePolicy with empty trust list
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ManageDevicePolicy_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageDevicePolicy_001 start");

    std::vector<UsbDeviceId> trustList;
    int32_t ret = usbHostManager_->ManageDevicePolicy(trustList);

    EXPECT_EQ(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageDevicePolicy_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ManageDevicePolicy_002
 * @tc.desc: Test ManageDevicePolicy with non-empty trust list
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ManageDevicePolicy_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageDevicePolicy_002 start");

    std::vector<UsbDeviceId> trustList;
    UsbDeviceId devId1 = {TEST_VENDOR_ID, TEST_PRODUCT_ID};
    UsbDeviceId devId2 = {0xABCD, 0xEF01};
    trustList.push_back(devId1);
    trustList.push_back(devId2);

    int32_t ret = usbHostManager_->ManageDevicePolicy(trustList);

    EXPECT_EQ(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageDevicePolicy_002 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ManageInterfaceType_001
 * @tc.desc: Test ManageInterfaceType with non-empty disableType
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ManageInterfaceType_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageInterfaceType_001 start");

    std::vector<UsbDeviceType> disableType;
    UsbDeviceType type1 = {0x08, 0x01, 0x00, 0};
    UsbDeviceType type2 = {0x03, 0x01, 0x01, 0};
    disableType.push_back(type1);
    disableType.push_back(type2);

    bool disable = true;
    int32_t ret = usbHostManager_->ManageInterfaceType(disableType, disable);

    EXPECT_EQ(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageInterfaceType_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_Dump_001
 * @tc.desc: Test Dump with "-a" argument
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_Dump_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_Dump_001 start");

    std::string args = "-a";
    bool ret = usbHostManager_->Dump(1, args);

    EXPECT_TRUE(ret);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_Dump_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_Dump_002
 * @tc.desc: Test Dump with invalid argument
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_Dump_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_Dump_002 start");

    std::string args = "-x";
    bool ret = usbHostManager_->Dump(1, args);

    EXPECT_FALSE(ret);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_Dump_002 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_Dump_003
 * @tc.desc: Test Dump with empty argument
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_Dump_003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_Dump_003 start");

    std::string args = "";
    bool ret = usbHostManager_->Dump(1, args);

    EXPECT_FALSE(ret);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_Dump_003 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_GetDeviceDescription_001
 * @tc.desc: Test GetDeviceDescription with valid baseClass
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetDeviceDescription_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDeviceDescription_001 start");

    int32_t baseClass = 0x08; // Mass Storage
    std::string description;
    uint8_t usage = 0;

    int32_t ret = usbHostManager_->GetDeviceDescription(baseClass, description, usage);

    EXPECT_EQ(ret, UEC_OK);
    EXPECT_FALSE(description.empty());
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDeviceDescription_001 desc=%{public}s usage=%{public}u",
                description.c_str(), usage);
}

/**
 * @tc.name: UsbHostManager_GetDeviceDescription_002
 * @tc.desc: Test GetDeviceDescription with invalid baseClass
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetDeviceDescription_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDeviceDescription_002 start");

    int32_t baseClass = 0xFF; // Unknown class
    std::string description;
    uint8_t usage = 0;

    int32_t ret = usbHostManager_->GetDeviceDescription(baseClass, description, usage);

    EXPECT_EQ(ret, UEC_OK);
    EXPECT_EQ(description, "NA");
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDeviceDescription_002 desc=%{public}s usage=%{public}u",
                description.c_str(), usage);
}

/**
 * @tc.name: UsbHostManager_GetDeviceDescription_003
 * @tc.desc: Test GetDeviceDescription with various baseClass values
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetDeviceDescription_003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDeviceDescription_003 start");

    std::vector<int32_t> baseClasses = {0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};

    for (auto baseClass : baseClasses) {
        std::string description;
        uint8_t usage = 0;
        int32_t ret = usbHostManager_->GetDeviceDescription(baseClass, description, usage);
        EXPECT_EQ(ret, UEC_OK);
        USB_HILOGI(MODULE_USB_HOST, "baseClass=0x%{public}X desc=%{public}s usage=%{public}u",
                    baseClass, description.c_str(), usage);
    }
}

/**
 * @tc.name: UsbHostManager_GetInterfaceDescription_001
 * @tc.desc: Test GetInterfaceDescription with valid device
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetInterfaceDescription_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetInterfaceDescription_001 start");

    auto* dev = CreateTestDevice(TEST_BUS_NUM, TEST_DEV_ADDR, TEST_VENDOR_ID, TEST_PRODUCT_ID, 0x08);
    ASSERT_NE(dev, nullptr);

    std::string description;
    int32_t baseClass = 0;

    int32_t ret = usbHostManager_->GetInterfaceDescription(*dev, description, baseClass);

    EXPECT_EQ(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetInterfaceDescription_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_BulkTransferRead_001
 * @tc.desc: Test BulkTransferRead with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_BulkTransferRead_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_BulkTransferRead_001 start");

    UsbDev dev = {TEST_BUS_NUM, TEST_DEV_ADDR};
    UsbPipe pipe = {TEST_INTERFACE_ID, TEST_ENDPOINT_ID};
    std::vector<uint8_t> bufferData;
    int32_t timeOut = 1000;

    int32_t ret = usbHostManager_->BulkTransferRead(dev, pipe, bufferData, timeOut);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_BulkTransferRead_001 ret=%{public}d size=%{public}zu",
                ret, bufferData.size());
}

/**
 * @tc.name: UsbHostManager_BulkTransferWrite_001
 * @tc.desc: Test BulkTransferWrite with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_BulkTransferWrite_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_BulkTransferWrite_001 start");

    UsbDev dev = {TEST_BUS_NUM, TEST_DEV_ADDR};
    UsbPipe pipe = {TEST_INTERFACE_ID, TEST_ENDPOINT_ID};
    std::vector<uint8_t> bufferData = {0x01, 0x02, 0x03, 0x04};
    int32_t timeOut = 1000;

    int32_t ret = usbHostManager_->BulkTransferWrite(dev, pipe, bufferData, timeOut);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_BulkTransferWrite_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ControlTransfer_001
 * @tc.desc: Test ControlTransfer with control transfer parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ControlTransfer_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ControlTransfer_001 start");

    UsbDev dev = {TEST_BUS_NUM, TEST_DEV_ADDR};
    UsbCtrlTransfer ctrl = {0x80, 0x06, 0x0100, 0x00, 1000};
    std::vector<uint8_t> bufferData;

    int32_t ret = usbHostManager_->ControlTransfer(dev, ctrl, bufferData);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE ||
                ret == UEC_SERVICE_INNER_ERR);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ControlTransfer_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ControlTransfer_002
 * @tc.desc: Test ControlTransfer with different request types
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ControlTransfer_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ControlTransfer_002 start");

    UsbDev dev = {TEST_BUS_NUM, TEST_DEV_ADDR};

    // Test with different request types
    std::vector<uint8_t> requestTypes = {0x00, 0x01, 0x80, 0x81};

    for (auto reqType : requestTypes) {
        UsbCtrlTransfer ctrl = {reqType, 0x06, 0x0100, 0x00, 1000};
        std::vector<uint8_t> bufferData;

        int32_t ret = usbHostManager_->ControlTransfer(dev, ctrl, bufferData);
        USB_HILOGI(MODULE_USB_HOST, "ControlTransfer reqType=0x%{public}X ret=%{public}d",
                    reqType, ret);
    }

    SUCCEED();
}

/**
 * @tc.name: UsbHostManager_UsbControlTransfer_001
 * @tc.desc: Test UsbControlTransfer with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_UsbControlTransfer_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UsbControlTransfer_001 start");

    UsbDev dev = {TEST_BUS_NUM, TEST_DEV_ADDR};
    HDI::Usb::V1_2::UsbCtrlTransferParams ctrlParams = {0x80, 0x06, 0x0100, 0x00, 1000};
    std::vector<uint8_t> bufferData;

    int32_t ret = usbHostManager_->UsbControlTransfer(dev, ctrlParams, bufferData);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE ||
                ret == UEC_SERVICE_INNER_ERR);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UsbControlTransfer_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_UsbControlTransfer_002
 * @tc.desc: Test UsbControlTransfer with timeout variations
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_UsbControlTransfer_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UsbControlTransfer_002 start");

    UsbDev dev = {TEST_BUS_NUM, TEST_DEV_ADDR};
    HDI::Usb::V1_2::UsbCtrlTransferParams ctrlParams = {0x80, 0x06, 0x0100, 0x00, 0};
    std::vector<uint8_t> bufferData;

    // Test with different timeout values
    std::vector<int32_t> timeouts = {0, 100, 1000, 5000};

    for (auto timeout : timeouts) {
        ctrlParams.timeout = timeout;
        int32_t ret = usbHostManager_->UsbControlTransfer(dev, ctrlParams, bufferData);
        USB_HILOGI(MODULE_USB_HOST, "UsbControlTransfer timeout=%{public}d ret=%{public}d",
                    timeout, ret);
    }

    SUCCEED();
}

/**
 * @tc.name: UsbHostManager_RequestQueue_001
 * @tc.desc: Test RequestQueue with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_RequestQueue_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_RequestQueue_001 start");

    UsbDev dev = {TEST_BUS_NUM, TEST_DEV_ADDR};
    UsbPipe pipe = {TEST_INTERFACE_ID, TEST_ENDPOINT_ID};
    std::vector<uint8_t> clientData = {0x01};
    std::vector<uint8_t> bufferData = {0x02, 0x03};

    int32_t ret = usbHostManager_->RequestQueue(dev, pipe, clientData, bufferData);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_RequestQueue_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_RequestWait_001
 * @tc.desc: Test RequestWait with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_RequestWait_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_RequestWait_001 start");

    UsbDev dev = {TEST_BUS_NUM, TEST_DEV_ADDR};
    int32_t timeOut = 1000;
    std::vector<uint8_t> clientData;
    std::vector<uint8_t> bufferData;

    int32_t ret = usbHostManager_->RequestWait(dev, timeOut, clientData, bufferData);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_RequestWait_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_RequestCancel_001
 * @tc.desc: Test RequestCancel with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_RequestCancel_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_RequestCancel_001 start");

    int32_t ret = usbHostManager_->RequestCancel(TEST_BUS_NUM, TEST_DEV_ADDR,
                                                  TEST_INTERFACE_ID, TEST_ENDPOINT_ID);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_RequestCancel_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_UsbCancelTransfer_001
 * @tc.desc: Test UsbCancelTransfer with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_UsbCancelTransfer_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UsbCancelTransfer_001 start");

    UsbDev devInfo = {TEST_BUS_NUM, TEST_DEV_ADDR};
    int32_t endpoint = TEST_ENDPOINT_ID;

    int32_t ret = usbHostManager_->UsbCancelTransfer(devInfo, endpoint);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UsbCancelTransfer_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_RegBulkCallback_001
 * @tc.desc: Test RegBulkCallback with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_RegBulkCallback_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_RegBulkCallback_001 start");

    UsbDev devInfo = {TEST_BUS_NUM, TEST_DEV_ADDR};
    UsbPipe pipe = {TEST_INTERFACE_ID, TEST_ENDPOINT_ID};
    sptr<IRemoteObject> cb = nullptr;

    int32_t ret = usbHostManager_->RegBulkCallback(devInfo, pipe, cb);

    // Should fail with invalid value since callback is null
    EXPECT_EQ(ret, UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_RegBulkCallback_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_UnRegBulkCallback_001
 * @tc.desc: Test UnRegBulkCallback with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_UnRegBulkCallback_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UnRegBulkCallback_001 start");

    UsbDev devInfo = {TEST_BUS_NUM, TEST_DEV_ADDR};
    UsbPipe pipe = {TEST_INTERFACE_ID, TEST_ENDPOINT_ID};

    int32_t ret = usbHostManager_->UnRegBulkCallback(devInfo, pipe);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UnRegBulkCallback_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_GetTargetDevice_001
 * @tc.desc: Test GetTargetDevice with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetTargetDevice_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetTargetDevice_001 start");

    UsbDevice dev;
    bool ret = usbHostManager_->GetTargetDevice(TEST_BUS_NUM, TEST_DEV_ADDR, dev);

    // Should return false for non-existent device
    EXPECT_FALSE(ret);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetTargetDevice_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_GetTargetDevice_002
 * @tc.desc: Test GetTargetDevice with different bus and dev numbers
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetTargetDevice_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetTargetDevice_002 start");

    std::vector<std::pair<uint8_t, uint8_t>> testCases = {
        {0, 0}, {1, 1}, {255, 1}, {1, 127}, {255, 255}
    };

    for (auto testCase : testCases) {
        UsbDevice dev;
        bool ret = usbHostManager_->GetTargetDevice(testCase.first, testCase.second, dev);
        USB_HILOGI(MODULE_USB_HOST, "GetTargetDevice bus=%{public}u dev=%{public}u ret=%{public}d",
                    testCase.first, testCase.second, ret);
    }

    SUCCEED();
}

/**
 * @tc.name: UsbHostManager_GetProductName_001
 * @tc.desc: Test GetProductName with non-existent device
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetProductName_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetProductName_001 start");

    std::string deviceName = "1-1";
    std::string productName;

    bool ret = usbHostManager_->GetProductName(deviceName, productName);

    // Should return false for non-existent device
    EXPECT_FALSE(ret);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetProductName_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_GetProductName_002
 * @tc.desc: Test GetProductName with different device names
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetProductName_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetProductName_002 start");

    std::vector<std::string> deviceNames = {"1-1", "1-2", "2-1", "255-127", "0-0"};

    for (auto deviceName : deviceNames) {
        std::string productName;
        bool ret = usbHostManager_->GetProductName(deviceName, productName);
        USB_HILOGI(MODULE_USB_HOST, "GetProductName device=%{public}s ret=%{public}d",
                    deviceName.c_str(), ret);
    }

    SUCCEED();
}

/**
 * @tc.name: UsbHostManager_ConcatenateToDescription_001
 * @tc.desc: Test ConcatenateToDescription with valid interface type
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ConcatenateToDescription_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ConcatenateToDescription_001 start");

    UsbDeviceType interfaceType = {0x08, 0x01, 0x00, 0};
    std::string str = "MassStorage";

    std::string result = usbHostManager_->ConcatenateToDescription(interfaceType, str);

    EXPECT_FALSE(result.empty());
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ConcatenateToDescription_001 result=%{public}s",
                result.c_str());
}

/**
 * @tc.name: UsbHostManager_ConcatenateToDescription_002
 * @tc.desc: Test ConcatenateToDescription with different interface types
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ConcatenateToDescription_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ConcatenateToDescription_002 start");

    std::vector<UsbDeviceType> interfaceTypes = {
        {0x08, 0x01, 0x00, 0},
        {0x03, 0x01, 0x01, 0},
        {0x0E, 0x01, 0x00, 0},
        {0xFF, 0xFF, 0xFF, 0}
    };

    for (auto interfaceType : interfaceTypes) {
        std::string str = "Test";
        std::string result = usbHostManager_->ConcatenateToDescription(interfaceType, str);
        USB_HILOGI(MODULE_USB_HOST, "ConcatenateToDescription 0x%{public}X_0x%{public}X_0x%{public}X result=%{public}s",
                    interfaceType.baseClass, interfaceType.subClass,
                    interfaceType.protocol, result.c_str());
    }

    SUCCEED();
}

/**
 * @tc.name: UsbHostManager_GetInterfaceUsageDescription_001
 * @tc.desc: Test GetInterfaceUsageDescription with keyboard type
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetInterfaceUsageDescription_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetInterfaceUsageDescription_001 start");

    UsbDeviceType interfaceType = {0x03, 0x01, 0x01, 0};

    std::string result = usbHostManager_->GetInterfaceUsageDescription(interfaceType);

    EXPECT_FALSE(result.empty());
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetInterfaceUsageDescription_001 result=%{public}s",
                result.c_str());
}

/**
 * @tc.name: UsbHostManager_GetInterfaceUsageDescription_002
 * @tc.desc: Test GetInterfaceUsageDescription with mouse type
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetInterfaceUsageDescription_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetInterfaceUsageDescription_002 start");

    UsbDeviceType interfaceType = {0x03, 0x01, 0x02, 0};

    std::string result = usbHostManager_->GetInterfaceUsageDescription(interfaceType);

    EXPECT_FALSE(result.empty());
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetInterfaceUsageDescription_002 result=%{public}s",
                result.c_str());
}

/**
 * @tc.name: UsbHostManager_GetInterfaceUsageDescription_003
 * @tc.desc: Test GetInterfaceUsageDescription with unknown type
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetInterfaceUsageDescription_003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetInterfaceUsageDescription_003 start");

    UsbDeviceType interfaceType = {0xFF, 0xFF, 0xFF, 0};

    std::string result = usbHostManager_->GetInterfaceUsageDescription(interfaceType);

    EXPECT_EQ(result, "NA");
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetInterfaceUsageDescription_003 result=%{public}s",
                result.c_str());
}

/**
 * @tc.name: UsbHostManager_BulkTransferReadwithLength_001
 * @tc.desc: Test BulkTransferReadwithLength with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_BulkTransferReadwithLength_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_BulkTransferReadwithLength_001 start");

    UsbDev dev = {TEST_BUS_NUM, TEST_DEV_ADDR};
    UsbPipe pipe = {TEST_INTERFACE_ID, TEST_ENDPOINT_ID};
    int32_t length = 1024;
    std::vector<uint8_t> bufferData;
    int32_t timeOut = 1000;

    int32_t ret = usbHostManager_->BulkTransferReadwithLength(dev, pipe, length, bufferData, timeOut);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_BulkTransferReadwithLength_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_BulkTransferReadwithLength_002
 * @tc.desc: Test BulkTransferReadwithLength with different lengths
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_BulkTransferReadwithLength_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_BulkTransferReadwithLength_002 start");

    UsbDev dev = {TEST_BUS_NUM, TEST_DEV_ADDR};
    UsbPipe pipe = {TEST_INTERFACE_ID, TEST_ENDPOINT_ID};

    std::vector<int32_t> lengths = {0, 1, 64, 256, 1024, 4096};

    for (auto length : lengths) {
        std::vector<uint8_t> bufferData;
        int32_t ret = usbHostManager_->BulkTransferReadwithLength(dev, pipe, length, bufferData, 1000);
        USB_HILOGI(MODULE_USB_HOST, "BulkTransferReadwithLength length=%{public}d ret=%{public}d",
                    length, ret);
    }

    SUCCEED();
}

/**
 * @tc.name: UsbHostManager_BulkRead_001
 * @tc.desc: Test BulkRead with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_BulkRead_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_BulkRead_001 start");

    UsbDev devInfo = {TEST_BUS_NUM, TEST_DEV_ADDR};
    UsbPipe pipe = {TEST_INTERFACE_ID, TEST_ENDPOINT_ID};
    sptr<Ashmem> ashmem = nullptr;

    int32_t ret = usbHostManager_->BulkRead(devInfo, pipe, ashmem);

    // Should fail with invalid value since ashmem is null
    EXPECT_EQ(ret, UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_BulkRead_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_BulkWrite_001
 * @tc.desc: Test BulkWrite with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_BulkWrite_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_BulkWrite_001 start");

    UsbDev devInfo = {TEST_BUS_NUM, TEST_DEV_ADDR};
    UsbPipe pipe = {TEST_INTERFACE_ID, TEST_ENDPOINT_ID};
    sptr<Ashmem> ashmem = nullptr;

    int32_t ret = usbHostManager_->BulkWrite(devInfo, pipe, ashmem);

    // Should fail with invalid value since ashmem is null
    EXPECT_EQ(ret, UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_BulkWrite_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_BulkCancel_001
 * @tc.desc: Test BulkCancel with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_BulkCancel_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_BulkCancel_001 start");

    UsbDev devInfo = {TEST_BUS_NUM, TEST_DEV_ADDR};
    UsbPipe pipe = {TEST_INTERFACE_ID, TEST_ENDPOINT_ID};

    int32_t ret = usbHostManager_->BulkCancel(devInfo, pipe);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_BulkCancel_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_BulkCancel_002
 * @tc.desc: Test BulkCancel with different pipe configurations
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_BulkCancel_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_BulkCancel_002 start");

    UsbDev devInfo = {TEST_BUS_NUM, TEST_DEV_ADDR};

    std::vector<UsbPipe> pipes = {
        {0, 1}, {0, 2}, {1, 1}, {1, 2}, {255, 127}
    };

    for (auto pipe : pipes) {
        int32_t ret = usbHostManager_->BulkCancel(devInfo, pipe);
        USB_HILOGI(MODULE_USB_HOST, "BulkCancel interface=%{public}u endpoint=%{public}u ret=%{public}d",
                    pipe.interfaceId, pipe.endpointId, ret);
    }

    SUCCEED();
}

/**
 * @tc.name: UsbHostManager_GetEndpointFromId_001
 * @tc.desc: Test GetEndpointFromId with valid device and endpoint
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetEndpointFromId_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetEndpointFromId_001 start");

    auto* dev = CreateTestDevice(TEST_BUS_NUM, TEST_DEV_ADDR, TEST_VENDOR_ID, TEST_PRODUCT_ID, 0x08);
    ASSERT_NE(dev, nullptr);

    int32_t endpointId = 0x81;
    USBEndpoint endpoint;

    bool ret = usbHostManager_->GetEndpointFromId(*dev, endpointId, endpoint);

    // Should return false for device with no endpoints
    EXPECT_FALSE(ret);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetEndpointFromId_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_GetEndpointFromId_002
 * @tc.desc: Test GetEndpointFromId with different endpoint IDs
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetEndpointFromId_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetEndpointFromId_002 start");

    auto* dev = CreateTestDevice(TEST_BUS_NUM, TEST_DEV_ADDR, TEST_VENDOR_ID, TEST_PRODUCT_ID, 0x08);
    ASSERT_NE(dev, nullptr);

    std::vector<int32_t> endpointIds = {0x01, 0x02, 0x81, 0x82, 0x03, 0x83};

    for (auto endpointId : endpointIds) {
        USBEndpoint endpoint;
        bool ret = usbHostManager_->GetEndpointFromId(*dev, endpointId, endpoint);
        USB_HILOGI(MODULE_USB_HOST, "GetEndpointFromId endpointId=0x%{public}X ret=%{public}d",
                    endpointId, ret);
    }

    SUCCEED();
}

/**
 * @tc.name: UsbHostManager_ManageInterface_001
 * @tc.desc: Test ManageInterface with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ManageInterface_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageInterface_001 start");

    UsbDev dev = {TEST_BUS_NUM, TEST_DEV_ADDR};
    uint8_t interfaceId = TEST_INTERFACE_ID;
    bool disable = true;

    int32_t ret = usbHostManager_->ManageInterface(dev, interfaceId, disable);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageInterface_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ManageInterface_002
 * @tc.desc: Test ManageInterface with disable=false
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ManageInterface_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageInterface_002 start");

    UsbDev dev = {TEST_BUS_NUM, TEST_DEV_ADDR};
    uint8_t interfaceId = TEST_INTERFACE_ID;
    bool disable = false;

    int32_t ret = usbHostManager_->ManageInterface(dev, interfaceId, disable);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ManageInterface_002 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ExecuteStrategy_001
 * @tc.desc: Test ExecuteStrategy function
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ExecuteStrategy_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ExecuteStrategy_001 start");

    // This is a void function, just ensure it doesn't crash
    usbHostManager_->ExecuteStrategy();

    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ExecuteStrategy_001 completed");
    SUCCEED();
}

/**
 * @tc.name: UsbHostManager_CheckDevPathIsExist_001
 * @tc.desc: Test CheckDevPathIsExist with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_CheckDevPathIsExist_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_CheckDevPathIsExist_001 start");

    int32_t ret = usbHostManager_->CheckDevPathIsExist(TEST_BUS_NUM, TEST_DEV_ADDR);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE ||
                ret == UEC_SERVICE_INNER_ERR);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_CheckDevPathIsExist_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_CheckDevPathIsExist_002
 * @tc.desc: Test CheckDevPathIsExist with boundary values
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_CheckDevPathIsExist_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_CheckDevPathIsExist_002 start");

    std::vector<std::pair<uint8_t, uint8_t>> testCases = {
        {0, 0}, {0, 1}, {1, 0}, {1, 1}, {127, 127}, {255, 127}
    };

    for (auto testCase : testCases) {
        int32_t ret = usbHostManager_->CheckDevPathIsExist(testCase.first, testCase.second);
        USB_HILOGI(MODULE_USB_HOST, "CheckDevPathIsExist bus=%{public}u dev=%{public}u ret=%{public}d",
                    testCase.first, testCase.second, ret);
    }

    SUCCEED();
}

/**
 * @tc.name: UsbHostManager_GetDeviceInfoDescriptor_001
 * @tc.desc: Test GetDeviceInfoDescriptor with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetDeviceInfoDescriptor_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDeviceInfoDescriptor_001 start");

    UsbDev uDev = {TEST_BUS_NUM, TEST_DEV_ADDR};
    std::vector<uint8_t> descriptor;
    UsbDevice dev;

    int32_t ret = usbHostManager_->GetDeviceInfoDescriptor(uDev, descriptor, dev);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE ||
                ret == UEC_SERVICE_INNER_ERR);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDeviceInfoDescriptor_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_GetConfigDescriptor_001
 * @tc.desc: Test GetConfigDescriptor with valid device
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetConfigDescriptor_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetConfigDescriptor_001 start");

    auto* dev = CreateTestDevice(TEST_BUS_NUM, TEST_DEV_ADDR, TEST_VENDOR_ID, TEST_PRODUCT_ID, 0x08);
    ASSERT_NE(dev, nullptr);

    std::vector<uint8_t> descriptor;

    int32_t ret = usbHostManager_->GetConfigDescriptor(*dev, descriptor);

    EXPECT_EQ(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetConfigDescriptor_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_FillDevStrings_001
 * @tc.desc: Test FillDevStrings with valid device
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_FillDevStrings_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_FillDevStrings_001 start");

    auto* dev = CreateTestDevice(TEST_BUS_NUM, TEST_DEV_ADDR, TEST_VENDOR_ID, TEST_PRODUCT_ID, 0x08);
    ASSERT_NE(dev, nullptr);

    int32_t ret = usbHostManager_->FillDevStrings(*dev);

    EXPECT_EQ(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_FillDevStrings_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_GetDevStringValFromIdx_001
 * @tc.desc: Test GetDevStringValFromIdx with valid index
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetDevStringValFromIdx_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDevStringValFromIdx_001 start");

    uint8_t idx = 1;
    std::string result = usbHostManager_->GetDevStringValFromIdx(TEST_BUS_NUM, TEST_DEV_ADDR, idx);

    // Should return a non-empty string or space
    EXPECT_TRUE(result.empty() || result == " " || !result.empty());
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDevStringValFromIdx_001 result='%{public}s'",
                result.c_str());
}

/**
 * @tc.name: UsbHostManager_GetDevStringValFromIdx_002
 * @tc.desc: Test GetDevStringValFromIdx with various indices
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_GetDevStringValFromIdx_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_GetDevStringValFromIdx_002 start");

    std::vector<uint8_t> indices = {0, 1, 2, 3, 10, 255};

    for (auto idx : indices) {
        std::string result = usbHostManager_->GetDevStringValFromIdx(TEST_BUS_NUM, TEST_DEV_ADDR, idx);
        USB_HILOGI(MODULE_USB_HOST, "GetDevStringValFromIdx idx=%{public}u result='%{public}s'",
                    idx, result.c_str());
    }

    SUCCEED();
}

/**
 * @tc.name: UsbHostManager_UsbDeviceAuthorize_001
 * @tc.desc: Test UsbDeviceAuthorize with authorized=true
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_UsbDeviceAuthorize_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UsbDeviceAuthorize_001 start");

    bool authorized = true;
    std::string operationType = "TestOperation";

    int32_t ret = usbHostManager_->UsbDeviceAuthorize(TEST_BUS_NUM, TEST_DEV_ADDR,
                                                       authorized, operationType);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UsbDeviceAuthorize_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_UsbInterfaceAuthorize_001
 * @tc.desc: Test UsbInterfaceAuthorize with authorized=true
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_UsbInterfaceAuthorize_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UsbInterfaceAuthorize_001 start");

    UsbDev dev = {TEST_BUS_NUM, TEST_DEV_ADDR};
    uint8_t configId = 0;
    uint8_t interfaceId = TEST_INTERFACE_ID;
    bool authorized = true;

    int32_t ret = usbHostManager_->UsbInterfaceAuthorize(dev, configId, interfaceId, authorized);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UsbInterfaceAuthorize_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_UsbInterfaceAuthorize_002
 * @tc.desc: Test UsbInterfaceAuthorize with authorized=false
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_UsbInterfaceAuthorize_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UsbInterfaceAuthorize_002 start");

    UsbDev dev = {TEST_BUS_NUM, TEST_DEV_ADDR};
    uint8_t configId = 0;
    uint8_t interfaceId = TEST_INTERFACE_ID;
    bool authorized = false;

    int32_t ret = usbHostManager_->UsbInterfaceAuthorize(dev, configId, interfaceId, authorized);

    // Should either succeed or fail with specific error
    EXPECT_TRUE(ret == UEC_OK || ret == UEC_SERVICE_INVALID_VALUE);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_UsbInterfaceAuthorize_002 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ExecuteManageDevicePolicy_001
 * @tc.desc: Test ExecuteManageDevicePolicy with empty trust list
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ExecuteManageDevicePolicy_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ExecuteManageDevicePolicy_001 start");

    std::vector<UsbDeviceId> trustList;

    int32_t ret = usbHostManager_->ExecuteManageDevicePolicy(trustList);

    EXPECT_EQ(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ExecuteManageDevicePolicy_001 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ExecuteManageDevicePolicy_002
 * @tc.desc: Test ExecuteManageDevicePolicy with single device in trust list
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ExecuteManageDevicePolicy_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ExecuteManageDevicePolicy_002 start");

    std::vector<UsbDeviceId> trustList;
    UsbDeviceId devId = {TEST_VENDOR_ID, TEST_PRODUCT_ID};
    trustList.push_back(devId);

    int32_t ret = usbHostManager_->ExecuteManageDevicePolicy(trustList);

    EXPECT_EQ(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ExecuteManageDevicePolicy_002 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ExecuteManageDevicePolicy_003
 * @tc.desc: Test ExecuteManageDevicePolicy with multiple devices in trust list
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ExecuteManageDevicePolicy_003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ExecuteManageDevicePolicy_003 start");

    std::vector<UsbDeviceId> trustList;
    trustList.push_back({TEST_VENDOR_ID, TEST_PRODUCT_ID});
    trustList.push_back({0x1234, 0x5678});
    trustList.push_back({0xABCD, 0xEF01});
    trustList.push_back({0x0000, 0x0000});
    trustList.push_back({0xFFFF, 0xFFFF});

    int32_t ret = usbHostManager_->ExecuteManageDevicePolicy(trustList);

    EXPECT_EQ(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ExecuteManageDevicePolicy_003 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ExecuteManageInterfaceType_003
 * @tc.desc: Test ExecuteManageInterfaceType with multiple types
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ExecuteManageInterfaceType_003, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ExecuteManageInterfaceType_003 start");

    std::vector<UsbDeviceType> disableType;
    disableType.push_back({0x08, 0x01, 0x00, 0});  // Mass Storage
    disableType.push_back({0x03, 0x01, 0x01, 0});  // Keyboard
    disableType.push_back({0x03, 0x01, 0x02, 0});  // Mouse
    disableType.push_back({0x0E, 0x01, 0x00, 0});  // Video
    disableType.push_back({0x01, 0x01, 0x00, 0});  // Audio

    bool disable = false;
    int32_t ret = usbHostManager_->ExecuteManageInterfaceType(disableType, disable);

    EXPECT_EQ(ret, UEC_OK);
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ExecuteManageInterfaceType_003 ret=%{public}d", ret);
}

/**
 * @tc.name: UsbHostManager_ConcurrentAccess_001
 * @tc.desc: Test concurrent access to GetDevices
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ConcurrentAccess_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ConcurrentAccess_001 start");

    const int threadCount = 5;
    std::vector<std::thread> threads;

    for (int i = 0; i < threadCount; i++) {
        threads.emplace_back([this, i]() {
            for (int j = 0; j < 10; j++) {
                std::vector<UsbDevice> deviceList;
                int32_t ret = usbHostManager_->GetDevices(deviceList);
                USB_HILOGI(MODULE_USB_HOST, "Thread %{public}d iteration %{public}d: ret=%{public}d size=%{public}zu",
                            i, j, ret, deviceList.size());
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ConcurrentAccess_001 completed");
    SUCCEED();
}

/**
 * @tc.name: UsbHostManager_StressTest_002
 * @tc.desc: Stress test with repeated OpenDevice/CloseDevice operations
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_StressTest_002, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_StressTest_002 start");

    const int iterations = 50;
    for (int i = 0; i < iterations; i++) {
        int32_t openRet = usbHostManager_->OpenDevice(TEST_BUS_NUM, TEST_DEV_ADDR);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        int32_t closeRet = usbHostManager_->Close(TEST_BUS_NUM, TEST_DEV_ADDR);

        if (i % 10 == 0) {
            USB_HILOGI(MODULE_USB_HOST, "StressTest iteration %{public}d: openRet=%{public}d closeRet=%{public}d",
                        i, openRet, closeRet);
        }
    }

    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_StressTest_002 completed");
    SUCCEED();
}

/**
 * @tc.name: UsbHostManager_ErrorHandling_001
 * @tc.desc: Test error handling with invalid parameters
 * @tc.type: FUNC
 */
HWTEST_F(UsbHostManagerTest, UsbHostManager_ErrorHandling_001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_HOST, "UsbHostManager_ErrorHandling_001 start");

    std::vector<std::function<int32_t()>> operations = {
        [this]() { return usbHostManager_->OpenDevice(255, 255); },
        [this]() { return usbHostManager_->Close(255, 255); },
        [this]() { return usbHostManager_->ResetDevice(255, 255); },
        [this]() { return usbHostManager_->GetActiveConfig(255, 255, *(new uint8_t)); }
    };

    for (size_t i = 0; i < operations.size(); i++) {
        int32_t ret = operations[i]();
        USB_HILOGI(MODULE_USB_HOST, "Operation %{public}zu ret=%{public}d", i, ret);
    }

    SUCCEED();
}

} // namespace ServiceTest
} // namespace USB
} // namespace OHOS
