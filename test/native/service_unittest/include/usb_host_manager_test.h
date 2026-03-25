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

#ifndef USB_HOST_MANAGER_TEST_H
#define USB_HOST_MANAGER_TEST_H

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "usb_host_manager.h"
#include "usb_device.h"
#include "usb_errors.h"

namespace OHOS {
namespace USB {
namespace ServiceTest {

/**
 * @brief Test class for UsbHostManager
 *
 * This class provides comprehensive unit tests for USB Host Manager functionality,
 * including device management, interface management, data transfer, policy management,
 * authorization, and various edge cases.
 */
class UsbHostManagerTest : public testing::Test {
public:
    /**
     * @brief Set up test suite - called once before all tests
     */
    static void SetUpTestCase(void);

    /**
     * @brief Tear down test suite - called once after all tests
     */
    static void TearDownTestCase(void);

    /**
     * @brief Set up each test case - called before each test
     */
    void SetUp();

    /**
     * @brief Tear down each test case - called after each test
     */
    void TearDown();

    /**
     * @brief Create mock USB devices for testing
     */
    void CreateMockDevices();

    /**
     * @brief Create mock USB interfaces
     */
    void CreateMockInterfaces();

    /**
     * @brief Create mock USB configurations
     */
    void CreateMockConfigs();

    /**
     * @brief Create a test USB device with specified parameters
     * @param busNum Bus number
     * @param devAddr Device address
     * @param vendorId Vendor ID
     * @param productId Product ID
     * @param deviceClass Device class code
     * @return Pointer to created UsbDevice, or nullptr if creation failed
     */
    UsbDevice* CreateTestDevice(uint8_t busNum, uint8_t devAddr,
                                uint16_t vendorId, uint16_t productId,
                                uint8_t deviceClass);

    /**
     * @brief Test device management functions
     */
    void TestDeviceManagement();
    void TestInterfaceManagement();
    void TestConfigurationManagement();
    void TestDataTransfer();
    void TestPolicyManagement();
    void TestAuthorization();
    void TestDescriptorFunctions();
    void TestEdgeCases();
    void TestConcurrentAccess();
    void TestErrorHandling();

    /**
     * @brief Helper functions for test verification
     */
    bool VerifyDeviceProperties(const UsbDevice& device, uint8_t expectedBusNum,
                                uint8_t expectedDevAddr, uint16_t expectedVendorId,
                                uint16_t expectedProductId);

    void CleanupTestDevices();
    void PrintDeviceInfo(const UsbDevice& device);

protected:
    /**
     * @brief USB Host Manager instance
     */
    std::unique_ptr<UsbHostManager> usbHostManager_;

    /**
     * @brief Collection of test devices
     */
    std::vector<UsbDevice*> testDevices_;

    /**
     * @brief Test constants
     */
    const uint8_t TEST_BUS_NUM = 1;
    const uint8_t TEST_DEV_ADDR = 2;
    const uint16_t TEST_VENDOR_ID = 0x1234;
    const uint16_t TEST_PRODUCT_ID = 0x5678;
    const uint8_t TEST_INTERFACE_ID = 0;
    const uint8_t TEST_ENDPOINT_ID = 1;
    const uint8_t TEST_CONFIG_INDEX = 0;
    const uint8_t TEST_ALT_SETTING = 0;

    /**
     * @brief Synchronization objects for concurrent testing
     */
    std::mutex testMutex_;
    std::condition_variable testCv_;
    bool testReady_ = false;

private:
    /**
     * @brief Internal helper to initialize test environment
     */
    void InitializeTestEnvironment();

    /**
     * @brief Internal helper to reset test state
     */
    void ResetTestState();

    /**
     * @brief Validate USB device basic properties
     */
    bool ValidateBasicDevice(const UsbDevice& device);
};

/**
 * @brief Mock class for USB device pipe testing
 */
class MockUsbDevicePipe {
public:
    MockUsbDevicePipe() = default;
    ~MockUsbDevicePipe() = default;

    bool IsValid() const { return isValid_; }
    void SetValid(bool valid) { isValid_ = valid; }

    int32_t GetBusNum() const { return busNum_; }
    int32_t GetDevAddr() const { return devAddr_; }

    void SetBusNum(int32_t busNum) { busNum_ = busNum; }
    void SetDevAddr(int32_t devAddr) { devAddr_ = devAddr_; }

private:
    bool isValid_ = true;
    int32_t busNum_ = 0;
    int32_t devAddr_ = 0;
};

/**
 * @brief Mock class for USB request testing
 */
class MockUsbRequest {
public:
    MockUsbRequest() = default;
    ~MockUsbRequest() = default;

    int32_t GetTimeout() const { return timeout_; }
    void SetTimeout(int32_t timeout) { timeout_ = timeout; }

    bool IsInitialized() const { return initialized_; }
    void SetInitialized(bool initialized) { initialized_ = initialized; }

private:
    int32_t timeout_ = 1000;
    bool initialized_ = false;
};

/**
 * @brief Mock class for USB event testing
 */
class MockUsbEvent {
public:
    enum class EventType {
        DEVICE_ATTACHED,
        DEVICE_DETACHED,
        INTERFACE_ADDED,
        INTERFACE_REMOVED,
        TRANSFER_COMPLETE
    };

    MockUsbEvent(EventType type, uint8_t busNum, uint8_t devAddr)
        : eventType_(type), busNum_(busNum), devAddr_(devAddr) {}

    EventType GetEventType() const { return eventType_; }
    uint8_t GetBusNum() const { return busNum_; }
    uint8_t GetDevAddr() const { return devAddr_; }

private:
    EventType eventType_;
    uint8_t busNum_;
    uint8_t devAddr_;
};

/**
 * @brief Test utility functions
 */
namespace TestUtil {
    /**
     * @brief Generate random test data
     * @param size Size of data to generate
     * @return Vector of random bytes
     */
    std::vector<uint8_t> GenerateRandomData(size_t size);

    /**
     * @brief Compare two data buffers
     * @param buffer1 First buffer
     * @param buffer2 Second buffer
     * @return true if buffers are equal
     */
    bool CompareBuffers(const std::vector<uint8_t>& buffer1,
                        const std::vector<uint8_t>& buffer2);

    /**
     * @brief Print buffer contents for debugging
     * @param buffer Buffer to print
     * @param label Label for the output
     */
    void PrintBuffer(const std::vector<uint8_t>& buffer, const std::string& label);

    /**
     * @brief Wait for a condition with timeout
     * @param condition Condition to wait for
     * @param timeoutMs Timeout in milliseconds
     * @return true if condition was met
     */
    bool WaitForCondition(std::function<bool()> condition, int32_t timeoutMs);

    /**
     * @brief Create a test USB device with default values
     * @return UsbDevice with default test values
     */
    UsbDevice CreateDefaultTestDevice();

    /**
     * @brief Validate error code is within expected range
     * @param errorCode Error code to validate
     * @return true if error code is valid
     */
    bool IsValidErrorCode(int32_t errorCode);
};

/**
 * @brief Test configuration constants
 */
namespace TestConfig {
    constexpr int32_t DEFAULT_TIMEOUT_MS = 1000;
    constexpr int32_t SHORT_TIMEOUT_MS = 100;
    constexpr int32_t LONG_TIMEOUT_MS = 5000;

    constexpr size_t DEFAULT_BUFFER_SIZE = 1024;
    constexpr size_t SMALL_BUFFER_SIZE = 64;
    constexpr size_t LARGE_BUFFER_SIZE = 4096;

    constexpr int32_t MAX_RETRIES = 3;
    constexpr int32_t RETRY_DELAY_MS = 10;

    constexpr uint8_t MAX_BUS_NUM = 255;
    constexpr uint8_t MAX_DEV_ADDR = 127;
}

} // namespace ServiceTest
} // namespace USB
} // namespace OHOS

#endif // USB_HOST_MANAGER_TEST_H
