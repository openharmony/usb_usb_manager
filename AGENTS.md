# USB Manager - Agentic Coding Guide

This document provides essential information for agentic coding agents working in this OpenHarmony USB Manager repository.

## Build System

This project uses the GN (Generate Ninja) build system for OpenHarmony.

```bash
# Build the entire usb_manager component
./build.sh --product-name <product> --build-target usb_manager

# Build specific service
./build.sh --product-name <product> --build-target usbservice
```

```bash
# Run all USB manager tests
./run_unittest.sh usb_manager

# Run specific test suite
./run_unittest.sh test_serial
./run_unittest.sh usbmgr_host_test

# Run JavaScript/ArkTS tests
./run_js_test.sh UsbManagerJsTest
```

## Project Structure

```
usb_manager/
├── interfaces/
│   ├── innerkits/       # Internal C++ APIs (UsbSrvClient, data models)
│   └── kits/js/napi/    # NAPI bindings for JS/TS interfaces
├── services/
│   ├── native/          # C++ service implementation (host/device managers)
│   └── zidl/            # IPC interface definitions
├── frameworks/
│   ├── dialog/          # UI dialog components for device permission
│   └── ets/taihe/       # ArkTS UI components and settings
├── test/
│   ├── native/          # C++ unit tests (gtest)
│   └── fuzztest/        # Fuzzing tests
├── utils/               # Common utilities (logger, file operations)
└── sa_profile/          # System ability profiles
```

## Code Style (Simplified)

**Naming Conventions**:
- C++ Classes: PascalCase (`UsbHostManager`)
- C++ Methods: PascalCase (`GetDevices`, `OpenDevice`)
- C++ Members: snake_case with underscore (`usbRightManager_`)
- JS Functions: camelCase (`usb.connectDevice`)
- Constants: UPPER_SNAKE_CASE (`USB_ENDPOINT_XFER_CONTROL`)

**Header**: Include Apache 2.0 license in all new files
**Imports**: Own headers → System headers → Third-party headers
**Error Codes**: Use `UsbErrCode` enum, return `int32_t` (0 = success, negative = error)
**Logging**: `USB_HILOGI/E/D(MODULE_USB_SERVICE, "message")`

## Feature Flags

Configure features in `usbmgr.gni`:
- `usb_manager_feature_host` - USB host mode support
- `usb_manager_feature_device` - USB device mode support
- `usb_manager_feature_port` - USB port management
- `usb_manager_pass_through` - Pass-through mode

## Testing

**C++ Tests**: Use Google Test, naming `<Module>Test` class and `HWTEST_F` macros
**ArkTS Tests**: Use Hypium framework
**Location**: `test/native/` matches source directory structure

## Business Logic Overview

### USB Device Permission Request Flow

**Request Process** (Host Mode):
1. App calls `UsbSrvClient` API to access USB device
2. `UsbService` checks if app has device access rights via `UsbRightManager::HasRight()`
3. If no right, calls `UsbRightManager::RequestRight()` → `GetUserAgreementByDiag()`
4. `ShowUsbDialog()` sets dialog parameters and connects to UI Extension Ability
5. `UsbDialogAbility` loads `UsbDialog.ets` page and shows permission dialog
6. User clicks "Allow" or "Deny" button
7. Dialog calls NAPI API (`usbMgr.addDeviceAccessRight()`) or `serialManager.addSerialRight()`
8. `OnAbilityDisconnectDone()` releases semaphore and returns to caller
9. Permission record added to `UsbRightDatabase`

**Key Components**:
- `services/native/src/usb_right_manager.cpp` - Permission management logic
- `frameworks/dialog/dialog_ui/usb_right_dialog/src/main/ets/ServiceExtAbility/UsbDialogAbility.ts` - Dialog lifecycle
- `frameworks/dialog/dialog_ui/usb_right_dialog/src/main/ets/pages/UsbDialog.ets` - UI interface
- `services/native/src/usb_right_db_helper.cpp` - Database operations

### Dialog UI Components

**UsbDialogAbility** (`frameworks/dialog/dialog_ui/usb_right_dialog/src/main/ets/ServiceExtAbility/UsbDialogAbility.ts:26`):
- Extends `UIExtensionAbility`
- `onSessionCreate()`: Initializes dialog, stores want parameters in `globalThis.want`
- `onSessionDestroy()`: Cleanup on dialog destruction

**UsbDialog.ets** (`frameworks/dialog/dialog_ui/usb_right_dialog/src/main/ets/pages/UsbDialog.ets:23`):
- Shows permission dialog with app name and device/product name
- Supports three types:
  - USB Device (`addDeviceAccessRight()`)
  - USB Accessory (`addAccessoryRight()`)
  - Serial Port (`addSerialRight()`)
- `closeDialog()`: Calls `terminateDialog()` to close dialog

**DialogUtil** (`frameworks/dialog/dialog_ui/usb_right_dialog/src/main/ets/util/DialogUtil.ts:18`):
- `terminateDialog()`: Calls `session.terminateSelf()` to close dialog session

**UserAuth** (`frameworks/dialog/dialog_ui/usb_right_dialog/src/main/ets/util/UserAuth.ts:20`):
- `getUserAuth()`: Provides PIN/FACE/FINGERPRINT authentication
- Generates random challenge using `cryptoFramework`
- Returns result via callbacks

### Permission Database Management

**Database Operations** (`services/native/src/usb_right_db_helper.cpp`):
- Store/retrieve device access rights per app and device
- Support temporary and permanent permissions
- Track expiration timestamps
- Clean up records for: uninstalled apps, deleted users, expired permissions

**Event Listeners** (`services/native/src/usb_right_manager.cpp:77`):
- `COMMON_EVENT_PACKAGE_REMOVED/BUNDLE_REMOVED`: Clean up app rights on uninstall
- `COMMON_EVENT_UID_REMOVED/USER_REMOVED`: Clean up user rights on user deletion
- `COMMON_EVENT_PACKAGE_ADDED`: Detect app reinstallation for cleanup

### Device Access Flow

**OpenDevice Process**:
1. `UsbHostManager::OpenDevice()` checks if device has pipe
2. If no pipe, requests permission via `UsbRightManager::RequestRight()`
3. If permission granted, creates device pipe via `UsbDevicePipe`
4. Returns device handle to caller

**Serial Port Access**:
- Similar flow using `SerialDeviceIdentity`
- Permissions managed via `serialManager.addSerialRight()`
- Uses portId instead of device VID-PID

## Important Business Rules

- Use namespace `OHOS::USB` for USB manager code
- When adding new APIs, update both C++ and NAPI bindings
- Use `std::shared_ptr` and `sptr<T>` (from OHOS) for smart pointers
- Protect shared data with `std::mutex` or `std::shared_mutex`
- Use feature flags for conditional compilation
- Check for existing implementations before adding new code
- Run tests before committing changes
