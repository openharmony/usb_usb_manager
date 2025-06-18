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

#include "usbfn_mtp_test.h"

#include <cinttypes>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <vector>
#include <fcntl.h>

#include "directory_ex.h"
#include "file_ex.h"
#include "hdf_log.h"
#include "securec.h"
#include "v1_0/iusbfn_mtp_interface.h"
#include "v1_0/usbfn_mtp_types.h"
#include "v2_0/iusb_port_interface.h"
#include "v2_0/iusb_device_interface.h"
#include "v2_0/usb_types.h"

#define HDF_LOG_TAG usbfn_mtp_ut

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::HDI::Usb::V2_0;
using namespace std;
using namespace OHOS::HDI::Usb::Gadget::Mtp::V1_0;

namespace {
constexpr int32_t SLEEP_TIME = 3;
constexpr int32_t MTP_EVENT_PACKET_MAX_BYTES = 28;
constexpr int32_t MTP_EVENT_PACKET_VALID_LEN = 20;
constexpr int32_t MTP_EVENT_PACKET_INVALID_LEN = 29;
constexpr uint16_t CMD_CODE_GET_DEVICE_INFO = 0x1001;
constexpr uint32_t MTP_PACKET_HEADER_SIZE = 12;
constexpr uint32_t BULK_IN_ONCE_MAX_SIZE = 1024;
constexpr uint32_t BULK_IN_LESS_THEN_ONCE = 45;
constexpr uint32_t BULK_IN_MORE_THEN_ONCE = 2023;
constexpr int64_t MTP_MAX_FILE_SIZE = 0xFFFFFFFFLL;
constexpr int64_t GEN_FILE_BUF_SIZE = 1024;
constexpr int64_t GEN_FILE_LIMIT_512MB = 512 * 1024 * 1024;
constexpr int32_t PRINT_VECTOR_MAX_LENGTH = 30;
const std::string WORKED_UT_PATH = "/data/local/tmp/";
const std::string MTP_TEST_SEND_FILE = "/data/local/tmp/sampleFile.mtp";
const std::string MTP_TEST_RECV_FILE = "/data/local/tmp/sampleFile.mtp";

sptr<IUsbfnMtpInterface> g_usbfnMtpInterface = nullptr;
sptr<IUsbPortInterface> g_usbPortInterface = nullptr;
sptr<IUsbDeviceInterface> g_usbDeviceInterface = nullptr;
int32_t g_currentFunc = USB_FUNCTION_NONE;
int32_t g_fileTestCount = 0;

struct UsbFnMtpFileSlice g_mfs = {
    .offset = 0,
    .length = 0,
    .command = 0,
    .transactionId = 0,
};

void PrintVector(const std::string &msg, std::vector<uint8_t> &data, bool hexFormat)
{
    size_t printLen = data.size();
    bool ignore = false;
    if (printLen > static_cast<size_t>(PRINT_VECTOR_MAX_LENGTH)) {
        printLen = static_cast<size_t>(PRINT_VECTOR_MAX_LENGTH);
        ignore = true;
    }
    std::stringstream ss;
    for (size_t i = 0; i < printLen; i++) {
        if (hexFormat) {
            ss << std::hex << "0x" << (0xFF & data.at(i)) << " ";
        } else {
            ss << data.at(i);
        }
    }
    std::string output = msg + std::string("(") + std::to_string(printLen) + std::string("):") + ss.str();
    if (ignore) {
        output += "......";
    }
    HDF_LOGV("UsbfnMtpTestAdditional::PrintVector %{public}s", output.c_str());
}

uint64_t GetFileSize(const std::string &pathName)
{
    struct stat statbuf;
    uint64_t ret = stat(pathName.c_str(), &statbuf);
    if (ret != 0) {
        return 0;
    }
    return static_cast<uint64_t>(statbuf.st_size);
}

bool WriteRandomDataToFile(const std::string &pathName, uint64_t fileSize)
{
    int32_t random = open("/dev/urandom", O_RDONLY);
    if (random < 0) {
        HDF_LOGE("UsbfnMtpTestAdditional::WriteRandomDataToFile get random data failed");
        return false;
    }
    FILE *opFile = std::fopen(pathName.c_str(), "w");
    if (opFile == nullptr) {
        HDF_LOGE("UsbfnMtpTestAdditional::WriteRandomDataToFile create file failed: %{public}s", pathName.c_str());
        return false;
    }
    char buffer[GEN_FILE_BUF_SIZE];
    int64_t count = static_cast<int64_t>(fileSize);
    while (count > 0) {
        (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
        int64_t readSize = count > GEN_FILE_BUF_SIZE ? GEN_FILE_BUF_SIZE : count;
        ssize_t readActual = read(random, static_cast<void *>(buffer), static_cast<size_t>(readSize));
        if (readActual != static_cast<ssize_t>(readSize)) {
            HDF_LOGW("UsbfnMtpTestAdditional::WriteRandomDataToFile read random failed");
            break;
        }
        size_t writeActual = std::fwrite(static_cast<void *>(buffer), 1, static_cast<size_t>(readSize), opFile);
        if (writeActual != static_cast<size_t>(readSize)) {
            HDF_LOGW("UsbfnMtpTestAdditional::WriteRandomDataToFile write failed");
            break;
        }
        count -= readSize;
    }
    std::fflush(opFile);
    std::fclose(opFile);
    close(random);
    HDF_LOGV("UsbfnMtpTestAdditional::WriteRandomDataToFile file %{public}s: %{public}" PRIu64 "/%{public}" PRIu64 "",
             pathName.c_str(), GetFileSize(pathName), fileSize);
    return count > 0 ? false : true;
}

bool GenerateFile(const std::string &pathName, int64_t fileSize)
{
    if (GetFileSize(pathName) == static_cast<uint64_t>(fileSize)) {
        HDF_LOGW("UsbfnMtpTestAdditional::GenerateFile file already exist");
        return true;
    }
    if (fileSize > GEN_FILE_LIMIT_512MB) {
        int32_t ret = truncate(pathName.c_str(), static_cast<off_t>(fileSize));
        if (ret != 0) {
            HDF_LOGE("UsbfnMtpTestAdditional::GenerateFile fail to truncate file to size: %{public}" PRId64 "",
                     fileSize);
            return false;
        }
        HDF_LOGV("UsbfnMtpTestAdditional::GenerateFile truncate %{public}s %{public}" PRId64 "", pathName.c_str(),
                 fileSize);
        return true;
    }
    return WriteRandomDataToFile(pathName, static_cast<uint64_t>(fileSize));
}

void UsbfnMtpTestAdditional::SetUpTestCase(void)
{
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    std::cout << "===>please connect to PC use USB 3.0 interface, press enter to continue set function to mtp"
              << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    g_usbPortInterface = IUsbPortInterface::Get();
    g_usbDeviceInterface = IUsbDeviceInterface::Get();
    ASSERT_TRUE(g_usbPortInterface != nullptr);
    ASSERT_TRUE(g_usbDeviceInterface != nullptr);
    auto ret = g_usbPortInterface->SetPortRole(DEFAULT_PORT_ID, POWER_ROLE_SINK, DATA_ROLE_DEVICE);
    sleep(SLEEP_TIME);
    if (ret != 0) {
        ASSERT_EQ(HDF_ERR_NOT_SUPPORT, ret);
    } else {
        ASSERT_EQ(0, ret);
    }
    ret = g_usbDeviceInterface->GetCurrentFunctions(g_currentFunc);
    ASSERT_EQ(0, ret);
    std::cout << "===>current function=" << g_currentFunc << ", set function to mtp, please wait" << std::endl;
    ret = g_usbDeviceInterface->SetCurrentFunctions(USB_FUNCTION_MTP);
    ASSERT_EQ(0, ret);

    g_usbfnMtpInterface = IUsbfnMtpInterface::Get();
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ret = g_usbfnMtpInterface->Start();
    ASSERT_EQ(0, ret);
}

void UsbfnMtpTestAdditional::TearDownTestCase(void)
{
    HDF_LOGV("UsbfnMtpTestAdditional::TearDownTestCase");
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    auto ret = g_usbfnMtpInterface->Stop();
    ASSERT_EQ(0, ret);
    ASSERT_TRUE(g_usbDeviceInterface != nullptr);
    ret = g_usbDeviceInterface->SetCurrentFunctions(g_currentFunc);
    ASSERT_EQ(0, ret);
    if (g_fileTestCount == 0) {
        return;
    }
    /* 1 means single test, run with '--gtest_filter=' option */
    if (g_fileTestCount == 1) {
        std::cout << "===>please delete temporary test file if needed: sendfile=" << MTP_TEST_SEND_FILE
                  << " recvfile=" << MTP_TEST_RECV_FILE << std::endl;
        return;
    }
    if (FileExists(std::string(MTP_TEST_SEND_FILE))) {
        if (remove(MTP_TEST_SEND_FILE.c_str()) != 0) {
            std::cout << "[-] remove send file failed: " << MTP_TEST_SEND_FILE << std::endl;
        }
    }
    if (FileExists(std::string(MTP_TEST_RECV_FILE))) {
        if (remove(MTP_TEST_RECV_FILE.c_str()) != 0) {
            std::cout << "[-] remove recv file failed: " << MTP_TEST_RECV_FILE << std::endl;
        }
    }
}

void UsbfnMtpTestAdditional::SetUp(void) {}

void UsbfnMtpTestAdditional::TearDown(void) {}
/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_0100
 * @tc.name: testHdiUsbMtpTestReceiveFile001
 * @tc.desc: mfs Indicates the mtp file slice info. mfs.length = 1.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestReceiveFile001, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 1;
    std::cout << "testHdiUsbMtpTestReceiveFile001===>use libusb in PC launch bulk-out transfer(size = " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    std::string filePathName = MTP_TEST_RECV_FILE;
    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->ReceiveFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(GetFileSize(filePathName), static_cast<uint64_t>(mfs.length));
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_0200
 * @tc.name: testHdiUsbMtpTestReceiveFile002
 * @tc.desc: mfs Indicates the mtp file slice info. mfs.length = 100.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestReceiveFile002, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 100;
    std::cout << "testHdiUsbMtpTestReceiveFile002===>use libusb in PC launch bulk-out transfer(size = " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    std::string filePathName = MTP_TEST_RECV_FILE;
    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->ReceiveFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(GetFileSize(filePathName), static_cast<uint64_t>(mfs.length));
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_0300
 * @tc.name: testHdiUsbMtpTestReceiveFile003
 * @tc.desc: mfs Indicates the mtp file slice info. mfs.length = 255.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestReceiveFile003, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 255;
    std::cout << "testHdiUsbMtpTestReceiveFile003===>use libusb in PC launch bulk-out transfer(size = " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    std::string filePathName = MTP_TEST_RECV_FILE;
    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->ReceiveFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(GetFileSize(filePathName), static_cast<uint64_t>(mfs.length));
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_0400
 * @tc.name: testHdiUsbMtpTestReceiveFile004
 * @tc.desc: mfs Indicates the mtp file slice info. mfs.length = 1000.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestReceiveFile004, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 1000;
    std::cout << "testHdiUsbMtpTestReceiveFile004===>use libusb in PC launch bulk-out transfer(size = " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    std::string filePathName = MTP_TEST_RECV_FILE;
    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->ReceiveFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(GetFileSize(filePathName), static_cast<uint64_t>(mfs.length));
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_0500
 * @tc.name: testHdiUsbMtpTestReceiveFile005
 * @tc.desc: mfs Indicates the mtp file slice info. mfs.length = MTP_MAX_FILE_SIZE - 2.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestReceiveFile005, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = MTP_MAX_FILE_SIZE - 2;
    std::cout << "testHdiUsbMtpTestReceiveFile005===>use libusb in PC launch bulk-out transfer(size = " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    std::string filePathName = MTP_TEST_RECV_FILE;
    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->ReceiveFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(GetFileSize(filePathName), static_cast<uint64_t>(mfs.length));
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_0600
 * @tc.name: testHdiUsbMtpTestReceiveFile006
 * @tc.desc: mfs Indicates the mtp file slice info. mfs.length = MTP_MAX_FILE_SIZE - 1024.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestReceiveFile006, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = MTP_MAX_FILE_SIZE - 1024;
    std::cout << "testHdiUsbMtpTestReceiveFile006===>use libusb in PC launch bulk-out transfer(size = " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    std::string filePathName = MTP_TEST_RECV_FILE;
    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->ReceiveFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(GetFileSize(filePathName), static_cast<uint64_t>(mfs.length));
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_0700
 * @tc.name: testHdiUsbMtpTestReceiveFile007
 * @tc.desc: mfs Indicates the mtp file slice info.mfs.length = 1 mfs.command = 1 mfs.transactionId = 1.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestReceiveFile007, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 1;
    mfs.command = 1;
    mfs.transactionId = 1;
    std::cout << "testHdiUsbMtpTestReceiveFile007===>use libusb in PC launch bulk-out transfer(size = " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    std::string filePathName = MTP_TEST_RECV_FILE;
    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->ReceiveFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(GetFileSize(filePathName), static_cast<uint64_t>(mfs.length));
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_0800
 * @tc.name: testHdiUsbMtpTestReceiveFile008
 * @tc.desc: mfs Indicates the mtp file slice info. mfs.length = 100 mfs.command = 100 mfs.transactionId = 100.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestReceiveFile008, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 100;
    mfs.command = 100;
    mfs.transactionId = 100;
    std::cout << "testHdiUsbMtpTestReceiveFile008===>use libusb in PC launch bulk-out transfer(size = " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    std::string filePathName = MTP_TEST_RECV_FILE;
    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->ReceiveFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(GetFileSize(filePathName), static_cast<uint64_t>(mfs.length));
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_0900
 * @tc.name: testHdiUsbMtpTestReceiveFile009
 * @tc.desc: mfs Indicates the mtp file slice info. mfs.length = 1000 mfs.command = 1000 mfs.transactionId = 1000.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestReceiveFile009, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 1000;
    mfs.command = 1000;
    mfs.transactionId = 1000;
    std::cout << "testHdiUsbMtpTestReceiveFile009===>use libusb in PC launch bulk-out transfer(size = " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    std::string filePathName = MTP_TEST_RECV_FILE;
    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->ReceiveFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(GetFileSize(filePathName), static_cast<uint64_t>(mfs.length));
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_1000
 * @tc.name: testHdiUsbMtpTestReceiveFile010
 * @tc.desc: mfs Indicates the mtp file slice info. mfs.length = 25 mfs.command = 100 mfs.transactionId = 100.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestReceiveFile010, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 25;
    mfs.command = 100;
    mfs.transactionId = 100;
    std::cout << "testHdiUsbMtpTestReceiveFile010===>use libusb in PC launch bulk-out transfer(size = " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    std::string filePathName = MTP_TEST_RECV_FILE;
    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->ReceiveFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(GetFileSize(filePathName), static_cast<uint64_t>(mfs.length));
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_1100
 * @tc.name: testHdiUsbMtpTestReceiveFile011
 * @tc.desc: mfs Indicates the mtp file slice info. mfs.length = 100 mfs.command = 200 mfs.transactionId = 300.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestReceiveFile011, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 100;
    mfs.command = 200;
    mfs.transactionId = 300;
    std::cout << "testHdiUsbMtpTestReceiveFile011===>use libusb in PC launch bulk-out transfer(size = " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    std::string filePathName = MTP_TEST_RECV_FILE;
    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->ReceiveFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(GetFileSize(filePathName), static_cast<uint64_t>(mfs.length));
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_1200
 * @tc.name: testHdiUsbMtpTestReceiveFile012
 * @tc.desc: mfs Indicates the mtp file slice info. mfs.length = 1000 mfs.command = 2000 mfs.transactionId = 3000.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestReceiveFile012, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 1000;
    mfs.command = 2000;
    mfs.transactionId = 3000;
    std::cout << "testHdiUsbMtpTestReceiveFile012===>use libusb in PC launch bulk-out transfer(size = " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    std::string filePathName = MTP_TEST_RECV_FILE;
    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->ReceiveFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(GetFileSize(filePathName), static_cast<uint64_t>(mfs.length));
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_1300
 * @tc.name: testHdiUsbMtpTestReceiveFile013
 * @tc.desc: mfs Indicates the mtp file slice info. mfs.length = 1 mfs.command = 1 mfs.transactionId = 100.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestReceiveFile013, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 1;
    mfs.command = 1;
    mfs.transactionId = 100;
    std::cout << "testHdiUsbMtpTestReceiveFile013===>use libusb in PC launch bulk-out transfer(size = " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    std::string filePathName = MTP_TEST_RECV_FILE;
    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->ReceiveFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(GetFileSize(filePathName), static_cast<uint64_t>(mfs.length));
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_1400
 * @tc.name: testHdiUsbMtpTestSendEvent001
 * @tc.desc: Send event data by USB MTP/PTP driver. devData.assign(MTP_EVENT_PACKET_VALID_LEN, 'S').
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendEvent001, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    std::vector<uint8_t> devData;
    devData.assign(MTP_EVENT_PACKET_VALID_LEN, 'S');
    std::cout << "testHdiUsbMtpTestSendEvent001===>use libusb in PC launch intr-in transfer(expect=" << devData.size()
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    auto ret = g_usbfnMtpInterface->SendEvent(devData);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_1500
 * @tc.name: testHdiUsbMtpTestSendEvent002
 * @tc.desc: Send event data by USB MTP/PTP driver. devData.assign(MTP_EVENT_PACKET_MAX_BYTES, 'S').
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendEvent002, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    std::vector<uint8_t> devData;
    devData.assign(MTP_EVENT_PACKET_MAX_BYTES, 'S');
    std::cout << "testHdiUsbMtpTestSendEvent002===>use libusb in PC launch intr-in transfer(expect=" << devData.size()
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
    auto ret = g_usbfnMtpInterface->SendEvent(devData);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_1600
 * @tc.name: testHdiUsbMtpTestSendEvent003
 * @tc.desc: Send event data by USB MTP/PTP driver. devData.assign(MTP_EVENT_PACKET_INVALID_LEN, 'S').
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendEvent003, Function | MediumTest | Level2)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    std::vector<uint8_t> devData;
    devData.assign(MTP_EVENT_PACKET_INVALID_LEN, 'S');
    std::cout << "testHdiUsbMtpTestSendEvent003===>use libusb in PC launch intr-in transfer(expect=no data, "
              << "or error), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    auto ret = g_usbfnMtpInterface->SendEvent(devData);
    ASSERT_NE(0, ret);
    std::cout << "testHdiUsbMtpTestSendEvent003===>make sure transfer timeout in PC, then start next test "
              << std::endl;
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_1700
 * @tc.name: testHdiUsbMtpTestSendEvent004
 * @tc.desc: Send event data by USB MTP/PTP driver. Cycle 10 times.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendEvent004, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    std::vector<uint8_t> devData = {0x65, 0x76, 0x65, 0x6E, 0x74, 0x30, 0x30, 0x34};
    int32_t ret;
    int32_t c;
    for (int i = 0; i < 5; i++) {
        std::cout
            << "testHdiUsbMtpTestSendEvent004===>use libusb in PC launch intr-in transfer(expect string=event004), "
            << "press enter to continue" << std::endl;

        while ((c = getchar()) != '\n' && c != EOF) {
        }

        ret = g_usbfnMtpInterface->SendEvent(devData);
        ASSERT_EQ(0, ret);

        PrintVector("event004", devData, true);
    }
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_1800
 * @tc.name: testHdiUsbMtpTestRead001
 * @tc.desc: Read data by USB MTP/PTP driver.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestRead001, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    std::vector<uint8_t> devData;

    std::cout << "testHdiUsbMtpTestRead001===>use libusb in PC launch bulk-out transfer(size=" << 5
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    int32_t ret = g_usbfnMtpInterface->Read(devData);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(devData.size(), static_cast<size_t>(5));
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_1900
 * @tc.name: testHdiUsbMtpTestWrite001
 * @tc.desc: Write data by USB MTP/PTP driver.devData.assign(BULK_IN_LESS_THEN_ONCE, 'r').
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestWrite001, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    uint32_t length = BULK_IN_LESS_THEN_ONCE;
    std::vector<uint8_t> devData;
    devData.assign(length, 'r');

    std::cout << "testHdiUsbMtpTestWrite001===>use libusb in PC launch bulk-in transfer(expect=" << length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    auto ret = g_usbfnMtpInterface->Write(devData);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_2000
 * @tc.name: testHdiUsbMtpTestWrite002
 * @tc.desc: Write data by USB MTP/PTP driver.devData.assign(BULK_IN_ONCE_MAX_SIZE, 'r').
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestWrite002, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    uint32_t length = BULK_IN_ONCE_MAX_SIZE;
    std::vector<uint8_t> devData;
    devData.assign(length, 'r');
    std::cout << "testHdiUsbMtpTestWrite002===>use libusb in PC launch bulk-in transfer(expect=" << length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    auto ret = g_usbfnMtpInterface->Write(devData);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_2100
 * @tc.name: testHdiUsbMtpTestWrite003
 * @tc.desc: Write data by USB MTP/PTP driver.devData.assign(BULK_IN_MORE_THEN_ONCE, 'r').
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestWrite003, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    uint32_t length = BULK_IN_MORE_THEN_ONCE;
    std::vector<uint8_t> devData;
    devData.assign(length, 'r');
    std::cout << "testHdiUsbMtpTestWrite003===>use libusb in PC launch bulk-in transfer(expect=" << length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    auto ret = g_usbfnMtpInterface->Write(devData);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_2200
 * @tc.name: testHdiUsbMtpTestWrite004
 * @tc.desc: Write data by USB MTP/PTP driver.devData.assign(BULK_IN_MORE_THEN_ONCE, 'i').
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestWrite004, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    uint32_t length = BULK_IN_MORE_THEN_ONCE;
    std::vector<uint8_t> devData;
    devData.assign(length, 'i');
    std::cout << "testHdiUsbMtpTestWrite004===>use libusb in PC launch bulk-in transfer(expect=" << length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    auto ret = g_usbfnMtpInterface->Write(devData);
    ASSERT_EQ(ret, 0);
}
/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_2300
 * @tc.name: testHdiUsbMtpTestWrite005
 * @tc.desc: Write data by USB MTP/PTP driver.Cycle 10 times.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestWrite005, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    int32_t ret;
    int i;
    int32_t c;
    std::vector<uint8_t> devData = {0x77, 0x72, 0x69, 0x74, 0x65, 0x30, 0x30, 0x35};
    for (i = 0; i < 5; i++) {
        std::cout << "testHdiUsbMtpTestWrite005===>use libusb in PC launch bulk-in transfer(expect string=write005), "
                  << "press enter to continue" << std::endl;

        while ((c = getchar()) != '\n' && c != EOF) {
        }

        ret = g_usbfnMtpInterface->Write(devData);
        ASSERT_EQ(ret, 0);

        PrintVector("write005", devData, true);
    }
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_2400
 * @tc.name: testHdiUsbMtpTestSendFile001
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = 1.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile001, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 1;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile001===>use libusb in PC launch bulk-in transfer(expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_2500
 * @tc.name: testHdiUsbMtpTestSendFile002
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = 100.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile002, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 100;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile002===>use libusb in PC launch bulk-in transfer(expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_2600
 * @tc.name: testHdiUsbMtpTestSendFile003
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = 1000.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile003, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 1000;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile003===>use libusb in PC launch bulk-in transfer(expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_2700
 * @tc.name: testHdiUsbMtpTestSendFile004
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = 1024.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile004, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 1024;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile004===>use libusb in PC launch bulk-in transfer(expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_2800
 * @tc.name: testHdiUsbMtpTestSendFile005
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = 255.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile005, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 255;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile005===>use libusb in PC launch bulk-in transfer(expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_2900
 * @tc.name: testHdiUsbMtpTestSendFile006
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = 1 mfs.command = CMD_CODE_GET_DEVICE_INFO.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile006, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 1;
    mfs.command = CMD_CODE_GET_DEVICE_INFO;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile006===>use libusb in PC launch bulk-in transfer(expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_3000
 * @tc.name: testHdiUsbMtpTestSendFile007
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = 100 command = CMD_CODE_GET_DEVICE_INFO.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile007, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 100;
    mfs.command = CMD_CODE_GET_DEVICE_INFO;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile007===>use libusb in PC launch bulk-in transfer(expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_3100
 * @tc.name: testHdiUsbMtpTestSendFile008
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = 1000 command = CMD_CODE_GET_DEVICE_INFO.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile008, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 1000;
    mfs.command = CMD_CODE_GET_DEVICE_INFO;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile008===>use libusb in PC launch bulk-in transfer(expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_3200
 * @tc.name: testHdiUsbMtpTestSendFile009
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = 1024 command = CMD_CODE_GET_DEVICE_INFO.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile009, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 1024;
    mfs.command = CMD_CODE_GET_DEVICE_INFO;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile009===>use libusb in PC launch bulk-in transfer(expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_3300
 * @tc.name: testHdiUsbMtpTestSendFile010
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = 255 command = CMD_CODE_GET_DEVICE_INFO.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile010, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 255;
    mfs.command = CMD_CODE_GET_DEVICE_INFO;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile010===>use libusb in PC launch bulk-in transfer(expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_3400
 * @tc.name: testHdiUsbMtpTestSendFile011
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = MTP_MAX_FILE_SIZE - 1.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile011, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = MTP_MAX_FILE_SIZE - 1;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile011===>use libusb in PC launch bulk-in transfer(speed, expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_3500
 * @tc.name: testHdiUsbMtpTestSendFile012
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = MTP_MAX_FILE_SIZE - 2.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile012, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = MTP_MAX_FILE_SIZE - 2;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile012===>use libusb in PC launch bulk-in transfer(speed, expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_3600
 * @tc.name: testHdiUsbMtpTestSendFile013
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.mfs.length = BULK_IN_LESS_THEN_ONCE mfs.command =
 * CMD_CODE_GET_DEVICE_INFO Cycle 10 times.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile013, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = BULK_IN_LESS_THEN_ONCE;
    mfs.command = CMD_CODE_GET_DEVICE_INFO;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    int32_t ret;
    int i;
    int32_t c;
    for (i = 0; i < 10; i++) {
        std::cout << "testHdiUsbMtpTestSendFile013===>use libusb in PC launch bulk-in transfer(expect "
                  << mfs.length + MTP_PACKET_HEADER_SIZE << "), press enter to continue" << std::endl;

        while ((c = getchar()) != '\n' && c != EOF) {
        }
        mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
        ASSERT_GT(mfs.fd, 0);

        ret = g_usbfnMtpInterface->SendFile(mfs);
        close(mfs.fd);
        ASSERT_EQ(0, ret);
    }
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_3700
 * @tc.name: testHdiUsbMtpTestSendFile014
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = 1 mfs.command = 1.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile014, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 1;
    mfs.command = 1;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile014===>use libusb in PC launch bulk-in transfer(expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_3800
 * @tc.name: testHdiUsbMtpTestSendFile015
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = 100 mfs.command = 100.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile015, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 100;
    mfs.command = 100;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile015===>use libusb in PC launch bulk-in transfer(expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_3900
 * @tc.name: testHdiUsbMtpTestSendFile016
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = 1000 mfs.command = 1000.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile016, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 1000;
    mfs.command = 1000;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile016===>use libusb in PC launch bulk-in transfer(expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_4000
 * @tc.name: testHdiUsbMtpTestSendFile017
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = 1024 mfs.command = 1024.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile017, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 1024;
    mfs.command = 1024;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile017===>use libusb in PC launch bulk-in transfer(expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_4100
 * @tc.name: testHdiUsbMtpTestSendFile018
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = 255 mfs.command = 255.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile018, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 255;
    mfs.command = 255;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile018===>use libusb in PC launch bulk-in transfer(expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_4200
 * @tc.name: testHdiUsbMtpTestSendFile019
 * @tc.desc: Send file by USB MTP/PTP driver.mfs.length = 200.mfs.command = 200.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestSendFile019, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    ASSERT_TRUE(GetCurrentProcPath() == WORKED_UT_PATH);
    g_fileTestCount++;
    struct UsbFnMtpFileSlice mfs = g_mfs;
    mfs.length = 200;
    mfs.command = 200;
    std::string filePathName = MTP_TEST_SEND_FILE;
    EXPECT_TRUE(GenerateFile(filePathName, mfs.length));
    std::cout << "testHdiUsbMtpTestSendFile019===>use libusb in PC launch bulk-in transfer(expect " << mfs.length
              << "), press enter to continue" << std::endl;
    int32_t c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }

    mfs.fd = open(filePathName.c_str(), O_CREAT | O_RDONLY);
    ASSERT_GT(mfs.fd, 0);
    auto ret = g_usbfnMtpInterface->SendFile(mfs);
    close(mfs.fd);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_4300
 * @tc.name: testHdiUsbMtpTestStart001
 * @tc.desc: Opens a USB MTP/PTP driver.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestStart001, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    auto ret = g_usbfnMtpInterface->Start();
    ASSERT_EQ(0, ret);
    ret = g_usbfnMtpInterface->Stop();
    ASSERT_EQ(0, ret);
}

/**
 * @tc.number: SUB_USB_DeviceManager_HDI_MTPPTPTest_4400
 * @tc.name: testHdiUsbMtpTestStop001
 * @tc.desc: Closes a USB MTP/PTP driver.
 */
HWTEST_F(UsbfnMtpTestAdditional, testHdiUsbMtpTestStop001, Function | MediumTest | Level1)
{
    ASSERT_TRUE(g_usbfnMtpInterface != nullptr);
    int32_t ret;
    int i;
    for (i = 0; i < 10; i++) {
        ret = g_usbfnMtpInterface->Start();
        ASSERT_EQ(0, ret);
        ret = g_usbfnMtpInterface->Stop();
        ASSERT_EQ(0, ret);
    }
}
} // namespace
