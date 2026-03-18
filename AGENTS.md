# USB Manager - Agentic Coding Guide

This document provides essential information for agentic coding agents working in this OpenHarmony USB Manager repository.

## Build System

This project uses the GN (Generate Ninja) build system for OpenHarmony.

### Build Commands

```bash
# Build the entire usb_manager component
./build.sh --product-name <product> --build-target usb_manager

# Build specific service
./build.sh --product-name <product> --build-target usbservice

# Build with feature flags
./build.sh --product-name <product> --build-target usb_manager --ccache
```

### Test Commands

This project uses Google Test (gtest) for C++ unit tests and Hypium for JavaScript/ArkTS tests.

```bash
# Run all USB manager tests
./run_unittest.sh usb_manager

# Run specific test suite
./run_unittest.sh test_serial
./run_unittest.sh usbmgr_host_test

# Run benchmark tests
./run_benchmarktest.sh usbmgr_benchmark

# Run JavaScript/ArkTS tests
./run_js_test.sh UsbManagerJsTest

# Run individual C++ test binary
/data/test/unittest/usb_manager/test_serial --gtest_filter=SerialTest.*

# Run tests with specific filter
./run_unittest.sh usbmgr_host_test --gtest_filter=*OpenDevice*
```

### Code Formatting

```bash
# Format C++ code according to project style
clang-format -i --style=file <file_path>

# Check formatting
clang-format --dry-run --Werror <file_path>
```

## Code Style Guidelines

### C++ Code Style

**Formatting** (defined in `.clang-format`):
- Based on WebKit style with modifications
- Line limit: 120 characters
- Pointer alignment: Right (`type* var`)
- Namespace indentation: None
- Indent case labels: true
- Use `constexpr` for compile-time constants

**Naming Conventions**:
- **Classes**: PascalCase (`UsbHostManager`, `UsbService`)
- **Functions/Methods**: PascalCase (`GetDevices`, `OpenDevice`, `ClaimInterface`)
- **Member variables**: snake_case with trailing underscore (`usbRightManager_`, `devices_`, `mutex_`)
- **Local variables**: camelCase (`deviceList`, `hasDevice`)
- **Constants**: UPPER_SNAKE_CASE (`USB_ENDPOINT_XFER_CONTROL`, `INVALID_USB_INT_VALUE`)
- **Files**: snake_case (`usb_host_manager.h`, `usb_service.cpp`)

**Headers**:
```cpp
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

#include "usb_service.h"  // Own header first
```

**Imports**: Group in order: own headers, system headers, third-party headers

**Error Handling**:
- Use `UsbErrCode` enum from `usb_errors.h` for error codes
- Functions return `int32_t` where 0 = success, negative = error
- Log errors using `USB_HILOGE(MODULE_USB_SERVICE, "message")` macro
- Check return values and handle errors appropriately

**Logging**:
```cpp
USB_HILOGI(MODULE_USB_SERVICE, "Info message: param=%{public}d", value);
USB_HILOGE(MODULE_USB_SERVICE, "Error message: %{public}s", errorStr);
USB_HILOGD(MODULE_USB_SERVICE, "Debug message");
```

**Constants**: Define in anonymous namespace at top of `.cpp` files:
```cpp
namespace {
constexpr int32_t SLEEP_TIME = 3;
constexpr uint32_t BUFFER_SIZE = 255;
}  // namespace
```

### NAPI/JavaScript Style

**Naming**:
- Functions: camelCase (`usb.connectDevice`, `usb.getDevices`)
- Constants: UPPER_SNAKE_CASE (`USB_ENDPOINT_XFER_CONTROL`)
- Module names: lowercase (`usb`, `usbManager`, `serial`)

### ArkTS/ETS Style

**Naming**:
- Functions: camelCase (`deviceConnected`, `getPipe`)
- Variables: camelCase (`gDeviceList`, `isDeviceConnected`)
- Constants: UPPER_SNAKE_CASE (`PARAM_NULLSTRING`)
- Files: snake_case with `.ets` extension

**Imports**: Use ES6 import statements

## Project Structure

```
usb_manager/
├── interfaces/
│   ├── innerkits/       # Internal C++ APIs
│   └── kits/js/napi/    # NAPI bindings for JS/TS
├── services/
│   ├── native/          # C++ service implementation
│   └── zidl/            # IPC interface definitions
├── frameworks/
│   ├── dialog/          # UI dialog components
│   └── ets/taihe/       # ArkTS UI components
├── test/
│   ├── native/          # C++ unit tests (gtest)
│   └── fuzztest/        # Fuzzing tests
├── utils/               # Common utilities
└── sa_profile/          # System ability profiles
```

## Feature Flags

Configure features in `usbmgr.gni`:
- `usb_manager_feature_host` - USB host mode support
- `usb_manager_feature_device` - USB device mode support
- `usb_manager_feature_port` - USB port management
- `usb_manager_pass_through` - Pass-through mode

```python
if (usb_manager_feature_host) {
  sources += [ "usb_host_manager.cpp" ]
}
```

## Testing Guidelines

**C++ Tests**:
- Use Google Test framework
- Test class naming: `<Module>Test` (e.g., `UsbDevicePipeTest`)
- Test case naming: `HWTEST_F(ClassName, testName, TestSize.Level1)`
- Place tests in `test/native/` with matching directory structure

**Test Example**:
```cpp
HWTEST_F(UsbDevicePipeTest, getDevices001, TestSize.Level1)
{
    USB_HILOGI(MODULE_USB_SERVICE, "Case Start : getDevices001");
    vector<UsbDevice> delist;
    auto &UsbSrvClient = UsbSrvClient::GetInstance();
    auto ret = UsbSrvClient.GetDevices(delist);
    ASSERT_EQ(ret, 0);
    EXPECT_TRUE(!(delist.empty()));
}
```

## Important Notes

- Always include the Apache 2.0 license header in new files
- Use namespace `OHOS::USB` for USB manager code
- Follow `.clang-format` for C++ code formatting
- When adding new APIs, update both C++ and NAPI bindings
- Run full test suite before committing changes
- Check for existing implementations before adding new code
- Use `std::shared_ptr` and `sptr<T>` (from OHOS) for smart pointers
- Protect shared data with `std::mutex` or `std::shared_mutex`
- Use feature flags for conditional compilation
