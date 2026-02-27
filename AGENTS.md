# AGENTS.md - USB Manager 开发指南

## 概述

这是 OpenHarmony USB Manager 子系统 (`@ohos/usb_manager`)。它提供 USB 设备管理、主机/设备模式、端口管理和串口功能。

## 项目结构

```
base/usb/usb_manager/
├── interfaces/          # API 接口 (innerkits + kits)
│   ├── innerkits/      # Native C++ 内部 API
│   └── kits/js/napi/   # JS/ETS NAPI 绑定
├── services/           # USB 服务 (SA)
│   └── native/         # 服务实现
├── utils/              # 工具和公共代码
├── frameworks/         # 对话框 UI, ETS 框架
├── test/               # 测试 (native, JS, ETS, XTS)
└── etc/                # 配置文件
```

## 构建命令

这是使用 GN 的 OpenHarmony 构建系统（非 CMake）。请从 OHOS 根目录构建：

### 完整构建

```bash
# 构建 usb_manager 组件
./build.sh --product-name rk3568 --build-target usb_manager

# 或使用 hb (OHOS 构建工具)
hb build -f -p usb_manager
```

### 构建特定模块

```bash
# 仅构建 USB 服务
hb build -t //base/usb/usb_manager/services:usbservice

# 构建 innerkits
hb build -t //base/usb/usb_manager/interfaces/innerkits:usbsrv_client
```

### 运行测试

```bash
# 运行所有 native 单元测试
hb build -t //base/usb/usb_manager/test/native:usb_unittest_test

# 运行单个测试 (使用 atest 示例)
atest usb_manager_unittest

# 直接运行特定的测试二进制文件
./out/.../usb_unittest_test --gtest_filter=TestName.Pattern

# 运行 JS/ETS 测试
hb build -t //base/usb/usb_manager/test/native/js_unittest:usb_manager_js_test
```

### 代码检查

```bash
# 使用 clang-format 格式化 C++ 代码
find . -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# 检查格式化但不修改
clang-format -n --style=file <file>

# 运行 cpplint (如果仓库配置了)
python3 $OHOS_ROOT/prebuilts/clang/ohos-linux-x86_64/llvm/bin/clang-format \
  --checks=<check-name> <file>
```

## 代码风格指南

### 文件组织

- **许可证头**: 所有文件必需 (Apache 2.0)
- **包含保护**: `#ifndef USBMGR_<PATH>_H` / `#define USBMGR_<PATH>_H`
- **包含顺序**: 标准库 → 外部 → 内部 (分组、排序)

```cpp
#include <map>
#include <vector>

#include "external_header.h"

#include "internal_header.h"
#include "service_header.h"
```

### 命名约定

| 元素 | 约定 | 示例 |
|---------|------------|---------|
| 命名空间 | 小写 | `OHOS::USB` |
| 类/结构体 | PascalCase | `UsbService`, `UsbDevice` |
| 函数 | PascalCase | `OpenDevice()`, `GetDevices()` |
| 成员变量 | snake_case_ | `ready_`, `deviceList_` |
| 静态变量 | s<PascalCase> | `sInstance` |
| 常量 | UPPER_SNAKE | `INVALID_USB_INT_VALUE` |
| 枚举 | PascalCase | `UsbErrCode::UEC_OK` |
| 宏 | UPPER_SNAKE | `USB_FUNCTION_HDC` |
| 文件 | snake_case | `usb_service.cpp` |

### 代码格式化

遵循 `.clang-format` (BasedOnStyle: Webkit):

- 列限制: 120
- 指针对齐: 右侧 (`int* ptr`)
- case 标签缩进: 是
- 大括号列表: C++11 风格
- 命名空间缩进: 无
- 修复命名空间注释: 是

### 错误处理

- **错误码**: 使用 `usb_errors.h` 中的 `UsbErrCode` 枚举
- **返回模式**:
  - 成功: `UEC_OK` (0)
  - 失败: 负数错误码
- **提前返回**: 使用 `usb_common.h` 中的宏:
  ```cpp
  RETURN_IF(ptr == nullptr);
  RETURN_IF_WITH_RET(ptr == nullptr, UEC_INTERFACE_INVALID_VALUE);
  RETURN_IF_WITH_LOG(condition, "log message");
  ```

### 日志

使用带模块标识符的 HiLog 包装宏:

```cpp
USB_HILOGE(MODULE_USB_SERVICE, "Failed to open device: %{public}d", errCode);
USB_HILOGI(MODULE_USB_HOST, "Device opened successfully");
USB_HILOGW(MODULE_USB_DEVICE, "Invalid parameter");
USB_HILOGD(MODULE_USB_UTILS, "Debug info");
```

可用模块:
- `MODULE_USB_SERVICE`
- `MODULE_USB_HOST`
- `MODULE_USB_DEVICE`
- `MODULE_USB_PORT`
- `MODULE_USB_SERIAL`
- `MODULE_USB_NAPI`
- `MODULE_USB_INNERKIT`
- `MODULE_USB_UTILS`

### Parcel/序列化

使用提供的宏进行 IPC parcel 操作:

```cpp
// 读取，失败时提前返回
READ_PARCEL_NO_RET(parcel, Int32, &value);

// 写入，失败时提前返回
WRITE_PARCEL_NO_RET(parcel, Int32, value);

// 读取，自定义返回值
READ_PARCEL_WITH_RET(parcel, String, out, UEC_SERVICE_READ_PARCEL_ERROR);
```

### 条件编译

`usbmgr.gni` 中的特性标志:
- `usb_manager_feature_host` - 主机模式支持
- `usb_manager_feature_device` - 设备模式支持
- `usb_manager_feature_port` - 端口管理
- `usb_manager_pass_through` - 透传模式

```cpp
#ifdef USB_MANAGER_FEATURE_HOST
    // 主机特定代码
#endif
```

### 内存管理

- 使用 `sptr<T>` 作为共享指针 (IPC)
- 使用 `std::shared_ptr<T>` 进行内部所有权
- 使用 `std::make_shared<T>()` 进行构造
- 避免使用原始 `new`/`delete`

### 测试指南

- 单元测试: `test/native/unittest/`
- JS API 测试: `test/native/js_unittest/`
- 测试命名: `<Module>Test` 或 `<Module>Test.cpp`
- 使用 Google Test 框架
- 使用 `TEST_F(Fixture, TestName)` 分组测试

```cpp
class UsbDeviceManagerTest : public testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;
};

TEST_F(UsbDeviceManagerTest, OpenDevice_Success) {
    // 测试实现
}
```

### 关键模式

1. **单例模式**: 使用 `DECLARE_DELAYED_SP_SINGLETON(ClassName)`
2. **死亡接收器**: 实现用于 IPC 连接监控
3. **系统能力**: 扩展 `SystemAbility` 并实现 `OnStart`/`OnStop`

```cpp
class UsbService : public SystemAbility, public UsbServerStub {
    DECLARE_SYSTEM_ABILITY(UsbService);
    DECLARE_DELAYED_SP_SINGLETON(UsbService);
};
```

### 重要文件

- `usb_errors.h` - 错误码定义
- `usb_common.h` - 公共宏和常量
- `hilog_wrapper.h` - 日志工具
- `bundle.json` - 组件配置
- `usbmgr.gni` - 构建配置

## 常见任务

### 添加新的 USB 功能

1. 在 `usb_errors.h` 中添加错误码
2. 在 `UsbService` 类中实现
3. 在 `UsbServerStub` 中添加 IPC 存根
4. 如果需要 JS API，添加 NAPI 包装器
5. 添加单元测试
6. 如果需要新 syscap，更新 `bundle.json`

### 添加新测试

1. 在适当的 `test/native/` 子目录中创建测试文件
2. 在 `BUILD.gn` 中添加 `sources` 和 `deps`
3. 构建: `hb build -t //path/to/test:target`
