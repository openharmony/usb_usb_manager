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

import { hilog } from '@kit.PerformanceAnalysisKit';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from '@ohos/hypium';
import { serialManager } from '@kit.BasicServicesKit';

export default function serialManagerTest() {
  describe('ActsAbilityTest', () => {
    // Defines a test suite. Two parameters are supported: test suite name and test suite function.
    beforeAll(() => {
      // Presets an action, which is performed only once before all test cases of the test suite start.
      // This API supports only one parameter: preset action function.
    })
    beforeEach(() => {
      // Presets an action, which is performed before each unit test case starts.
      // The number of execution times is the same as the number of test cases defined by **it**.
      // This API supports only one parameter: preset action function.
    })
    afterEach(() => {
      // Presets a clear action, which is performed after each unit test case ends.
      // The number of execution times is the same as the number of test cases defined by **it**.
      // This API supports only one parameter: clear action function.
    })
    afterAll(() => {
      // Presets a clear action, which is performed after all test cases of the test suite end.
      // This API supports only one parameter: clear action function.
    })
    it('getPortList', 0, () => {
      // Defines a test case. This API supports three parameters: test case name, filter parameter, and test case function.
      hilog.info(0x0000, 'testTag', '%{public}s', 'it begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        expect(ports != null).assertTrue()
        expect(ports != undefined).assertTrue()
        expect(Array.isArray(ports)).assertTrue()

        // 遍历端口列表，验证每个端口的属性
        for (let i = 0; i < ports.length; i++) {
          let port = ports[i];
          expect(port != null).assertTrue()
          expect(port != undefined).assertTrue()

          // 验证port对象的属性
          expect(typeof port.portId).assertEqual('number')
          expect(typeof port.deviceName).assertEqual('string')

          // 验证portId的范围
          expect(port.portId >= 0).assertTrue()

          // 验证deviceName的长度
          expect(port.deviceName.length > 0).assertTrue()
          expect(port.deviceName.length <= 256).assertTrue() // 假设设备名称不超过256个字符


        }

      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.getPortList).assertUndefined()
      }
    })

    it('getPortList_empty', 0, () => {
      // 测试当没有串口设备时的情况
      hilog.info(0x0000, 'testTag', '%{public}s', 'getPortList_empty test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        expect(ports != null).assertTrue()
        expect(ports != undefined).assertTrue()
        expect(Array.isArray(ports)).assertTrue()

        // 即使没有设备，也应该返回一个空数组而不是null或undefined
        expect(ports.length >= 0).assertTrue()
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.getPortList).assertUndefined()
      }
    })

    it('getPortList_multipleTimes', 0, () => {
      // 测试多次调用getPortList是否返回一致的结果
      hilog.info(0x0000, 'testTag', '%{public}s', 'getPortList_multipleTimes test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports1 = serialManager.getPortList();
        let ports2 = serialManager.getPortList();
        let ports3 = serialManager.getPortList();

        // 验证三次调用的结果是否一致
        expect(ports1 != null).assertTrue()
        expect(ports2 != null).assertTrue()
        expect(ports3 != null).assertTrue()

        expect(ports1.length).assertEqual(ports2.length)
        expect(ports2.length).assertEqual(ports3.length)

        // 验证每个端口的属性是否一致
        for (let i = 0; i < ports1.length; i++) {
          expect(ports1[i].portId).assertEqual(ports2[i].portId)
          expect(ports1[i].deviceName).assertEqual(ports2[i].deviceName)
          expect(ports2[i].portId).assertEqual(ports3[i].portId)
          expect(ports2[i].deviceName).assertEqual(ports3[i].deviceName)
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.getPortList).assertUndefined()
      }
    })

    it('getPortList_portProperties', 0, () => {
      // 测试端口属性的详细验证
      hilog.info(0x0000, 'testTag', '%{public}s', 'getPortList_portProperties test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        expect(ports != null).assertTrue();
        expect(Array.isArray(ports)).assertTrue();

        for (let i = 0; i < ports.length; i++) {
          let port = ports[i];

          // 验证portId的范围和类型
          expect(typeof port.portId).assertEqual('number');
          expect(port.portId > 0).assertTrue();
          expect(port.portId < 10000).assertTrue(); // 合理的端口ID范围
          expect(Number.isInteger(port.portId)).assertTrue(); // 必须是整数

          // 验证deviceName的格式和长度
          expect(typeof port.deviceName).assertEqual('string');
          expect(port.deviceName.length > 0).assertTrue();
          expect(port.deviceName.length < 256).assertTrue(); // 合理的设备名称长度


        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.getPortList).assertUndefined()
      }
    })

    it('hasSerialRight', 0, () => {
      // Defines a test case. This API supports three parameters: test case name, filter parameter, and test case function.
      hilog.info(0x0000, 'testTag', '%{public}s', 'it begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        for (let i = 0; i < ports.length; i++) {
          let port = ports[i];
          let hasRight = serialManager.hasSerialRight(port.portId);
          expect(typeof hasRight).assertEqual('boolean');
          // 非系统应用默认没有权限
          expect(hasRight).assertFalse();
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.getPortList).assertUndefined()
      }
    })

    it('hasSerialRight_invalidPortId', 0, () => {
      // 测试使用无效的portId
      hilog.info(0x0000, 'testTag', '%{public}s', 'hasSerialRight_invalidPortId test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          // 使用负数作为portId
          serialManager.hasSerialRight(-1);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(401); // 参数错误
        }

        try {
          // 使用0作为portId（通常portId从1开始）
          serialManager.hasSerialRight(0);
        } catch (error) {
          // 可能会抛出异常，也可能返回false，取决于实现
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(31400003); // PortId不存在
        }

        try {
          // 使用非常大的数字作为portId
          serialManager.hasSerialRight(999999999);
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(31400003); // PortId不存在
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.hasSerialRight).assertUndefined()
      }
    })

    it('hasSerialRight_boundaryValues', 0, () => {
      // 测试边界值
      hilog.info(0x0000, 'testTag', '%{public}s', 'hasSerialRight_boundaryValues test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          // 使用有效的portId
          let portId = ports[0].portId;
          let hasRight = serialManager.hasSerialRight(portId);
          expect(typeof hasRight).assertEqual('boolean');

          // 使用portId的边界值
          try {
            serialManager.hasSerialRight(1); // 最小的可能portId
          } catch (error) {
            // 可能会抛出异常，也可能返回false，取决于是否存在这个portId
            expect(error != null).assertTrue();
            expect([401, 31400003].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.hasSerialRight).assertUndefined()
      }
    })

    it('hasSerialRight_multipleTimes', 0, () => {
      // 测试多次调用hasSerialRight是否返回一致的结果
      hilog.info(0x0000, 'testTag', '%{public}s', 'hasSerialRight_multipleTimes test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;
          let hasRight1 = serialManager.hasSerialRight(portId);
          let hasRight2 = serialManager.hasSerialRight(portId);
          let hasRight3 = serialManager.hasSerialRight(portId);

          // 验证三次调用的结果是否一致
          expect(typeof hasRight1).assertEqual('boolean');
          expect(typeof hasRight2).assertEqual('boolean');
          expect(typeof hasRight3).assertEqual('boolean');

          expect(hasRight1).assertEqual(hasRight2);
          expect(hasRight2).assertEqual(hasRight3);
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.hasSerialRight).assertUndefined()
      }
    })

    it('hasSerialRight_withDifferentPorts', 0, () => {
      // 测试对不同的端口调用hasSerialRight
      hilog.info(0x0000, 'testTag', '%{public}s', 'hasSerialRight_withDifferentPorts test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        let results = new Set<boolean>();

        for (let i = 0; i < ports.length; i++) {
          let port = ports[i];
          let hasRight = serialManager.hasSerialRight(port.portId);
          expect(typeof hasRight).assertEqual('boolean');
          results.add(hasRight);
        }

        // 非系统应用对所有端口都应该没有权限
        expect(results.size <= 1).assertTrue();
        expect(results.has(false)).assertFalse();
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.hasSerialRight).assertUndefined()
      }
    })

    it('requestSerialRight', 0, async () => {
      // Defines a test case for requestSerialRight
      hilog.info(0x0000, 'testTag', '%{public}s', 'requestSerialRight test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          try {
            let portId = ports[0].portId;
            let result = await serialManager.requestSerialRight(portId);
            // 测试可能返回true或false，具体取决于用户是否授权
            expect(typeof result).assertEqual('boolean');
          } catch (error) {
            hilog.info(0x0000, 'testTag', 'requestSerialRight failed: %{public}s', JSON.stringify(error));
            // 可能因为各种原因失败，比如权限被拒绝等
            expect(error != null).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.requestSerialRight).assertUndefined()
      }
    })

    it('requestSerialRight_invalidPortId', 0, async () => {
      // 测试使用无效的portId
      hilog.info(0x0000, 'testTag', '%{public}s', 'requestSerialRight_invalidPortId test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          // 使用负数作为portId
          await serialManager.requestSerialRight(-1);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(401); // 参数错误
        }

        try {
          // 使用0作为portId（通常portId从1开始）
          await serialManager.requestSerialRight(0);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
        }

        try {
          // 使用非常大的数字作为portId
          await serialManager.requestSerialRight(999999999);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.requestSerialRight).assertUndefined()
      }
    })

    it('requestSerialRight_boundaryValues', 0, async () => {
      // 测试边界值
      hilog.info(0x0000, 'testTag', '%{public}s', 'requestSerialRight_boundaryValues test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          // 使用有效的portId
          let portId = ports[0].portId;
          try {
            let result = await serialManager.requestSerialRight(portId);
            expect(typeof result).assertEqual('boolean');
          } catch (error) {
            // 可能因为各种原因失败，比如权限被拒绝等
            expect(error != null).assertTrue();
          }

          // 使用portId的边界值
          try {
            await serialManager.requestSerialRight(1); // 最小的可能portId
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 可能会抛出异常，也可能返回false，取决于是否存在这个portId
            expect(error != null).assertTrue();
            expect([401, 31400003].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.requestSerialRight).assertUndefined()
      }
    })

    it('requestSerialRight_multipleTimes', 0, async () => {
      // 测试多次调用requestSerialRight
      hilog.info(0x0000, 'testTag', '%{public}s', 'requestSerialRight_multipleTimes test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;

          // 第一次调用
          try {
            let result1 = await serialManager.requestSerialRight(portId);
            expect(typeof result1).assertEqual('boolean');
          } catch (error1) {
            // 可能因为各种原因失败，比如权限被拒绝等
            expect(error1 != null).assertTrue();
          }

          // 第二次调用
          try {
            let result2 = await serialManager.requestSerialRight(portId);
            expect(typeof result2).assertEqual('boolean');
          } catch (error2) {
            // 可能因为各种原因失败，比如权限被拒绝等
            expect(error2 != null).assertTrue();
          }

          // 第三次调用
          try {
            let result3 = await serialManager.requestSerialRight(portId);
            expect(typeof result3).assertEqual('boolean');
          } catch (error3) {
            // 可能因为各种原因失败，比如权限被拒绝等
            expect(error3 != null).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.requestSerialRight).assertUndefined()
      }
    })

    it('requestSerialRight_withDifferentPorts', 0, async () => {
      // 测试对不同的端口调用requestSerialRight
      hilog.info(0x0000, 'testTag', '%{public}s', 'requestSerialRight_withDifferentPorts test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        for (let i = 0; i < ports.length; i++) {
          let port = ports[i];
          try {
            let result = await serialManager.requestSerialRight(port.portId);
            expect(typeof result).assertEqual('boolean');
          } catch (error) {
            // 可能因为各种原因失败，比如权限被拒绝等
            expect(error != null).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.requestSerialRight).assertUndefined()
      }
    })

    it('cancelSerialRight', 0, () => {
      // Defines a test case for cancelSerialRight
      hilog.info(0x0000, 'testTag', '%{public}s', 'cancelSerialRight test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          try {
            let portId = ports[0].portId;
            serialManager.cancelSerialRight(portId);
            // 如果没有抛出异常，说明取消成功或操作被忽略
          } catch (error) {
            // 可能因为权限问题而失败，这是预期行为
            hilog.info(0x0000, 'testTag', 'cancelSerialRight failed as expected: %{public}s', JSON.stringify(error));
            // 非系统应用没有权限，应该抛出异常
            expect(error != null).assertTrue();
            expect([31400002, 31400003].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.cancelSerialRight).assertUndefined()
      }
    })

    it('cancelSerialRight_invalidPortId', 0, () => {
      // 测试使用无效的portId
      hilog.info(0x0000, 'testTag', '%{public}s', 'cancelSerialRight_invalidPortId test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          // 使用负数作为portId
          serialManager.cancelSerialRight(-1);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(401); // 参数错误
        }

        try {
          // 使用0作为portId（通常portId从1开始）
          serialManager.cancelSerialRight(0);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(31400003); // PortId不存在
        }

        try {
          // 使用非常大的数字作为portId
          serialManager.cancelSerialRight(999999999);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(31400003); // PortId不存在
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.cancelSerialRight).assertUndefined()
      }
    })

    it('cancelSerialRight_boundaryValues', 0, () => {
      // 测试边界值
      hilog.info(0x0000, 'testTag', '%{public}s', 'cancelSerialRight_boundaryValues test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          // 使用有效的portId
          let portId = ports[0].portId;
          try {
            serialManager.cancelSerialRight(portId);
            // 如果没有抛出异常，说明取消成功或操作被忽略
          } catch (error) {
            // 可能因为各种原因失败，比如权限被拒绝等
            expect(error != null).assertTrue();
            expect([31400002, 31400003].includes(error.code)).assertTrue();
          }

          // 使用portId的边界值
          try {
            serialManager.cancelSerialRight(1); // 最小的可能portId
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 可能会抛出异常，也可能返回false，取决于是否存在这个portId
            expect(error != null).assertTrue();
            expect([401, 31400002, 31400003].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.cancelSerialRight).assertUndefined()
      }
    })

    it('cancelSerialRight_multipleTimes', 0, () => {
      // 测试多次调用cancelSerialRight
      hilog.info(0x0000, 'testTag', '%{public}s', 'cancelSerialRight_multipleTimes test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;

          // 第一次调用
          try {
            serialManager.cancelSerialRight(portId);
            // 如果没有抛出异常，说明取消成功或操作被忽略
          } catch (error1) {
            // 可能因为各种原因失败，比如权限被拒绝等
            expect(error1 != null).assertTrue();
            expect([31400002, 31400003].includes(error1.code)).assertTrue();
          }

          // 第二次调用
          try {
            serialManager.cancelSerialRight(portId);
            // 如果没有抛出异常，说明取消成功或操作被忽略
          } catch (error2) {
            // 可能因为各种原因失败，比如权限被拒绝等
            expect(error2 != null).assertTrue();
            expect([31400002, 31400003].includes(error2.code)).assertTrue();
          }

          // 第三次调用
          try {
            serialManager.cancelSerialRight(portId);
            // 如果没有抛出异常，说明取消成功或操作被忽略
          } catch (error3) {
            // 可能因为各种原因失败，比如权限被拒绝等
            expect(error3 != null).assertTrue();
            expect([31400002, 31400003].includes(error3.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.cancelSerialRight).assertUndefined()
      }
    })

    it('cancelSerialRight_withDifferentPorts', 0, () => {
      // 测试对不同的端口调用cancelSerialRight
      hilog.info(0x0000, 'testTag', '%{public}s', 'cancelSerialRight_withDifferentPorts test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        for (let i = 0; i < ports.length; i++) {
          let port = ports[i];
          try {
            serialManager.cancelSerialRight(port.portId);
            // 如果没有抛出异常，说明取消成功或操作被忽略
          } catch (error) {
            // 可能因为各种原因失败，比如权限被拒绝等
            expect(error != null).assertTrue();
            expect([31400002, 31400003].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.cancelSerialRight).assertUndefined()
      }
    })

    it('open', 0, () => {
      // Defines a test case for open
      hilog.info(0x0000, 'testTag', '%{public}s', 'open test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          try {
            let portId = ports[0].portId;
            serialManager.open(portId);
            // 如果没有抛出异常，说明打开成功
          } catch (error) {
            // 可能因为权限问题或端口被占用而失败，这是预期行为
            hilog.info(0x0000, 'testTag', 'open failed as expected: %{public}s', JSON.stringify(error));
            // 非系统应用没有权限，应该抛出异常
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400004].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.open).assertUndefined()
      }
    })

    it('open_invalidPortId', 0, () => {
      // 测试使用无效的portId
      hilog.info(0x0000, 'testTag', '%{public}s', 'open_invalidPortId test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          // 使用负数作为portId
          serialManager.open(-1);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(401); // 参数错误
        }

        try {
          // 使用0作为portId（通常portId从1开始）
          serialManager.open(0);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(31400003); // PortId不存在
        }

        try {
          // 使用非常大的数字作为portId
          serialManager.open(999999999);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(31400003); // PortId不存在
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.open).assertUndefined()
      }
    })

    it('open_boundaryValues', 0, () => {
      // 测试边界值
      hilog.info(0x0000, 'testTag', '%{public}s', 'open_boundaryValues test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          // 使用有效的portId
          let portId = ports[0].portId;
          try {
            serialManager.open(portId);
            // 如果没有抛出异常，说明打开成功
          } catch (error) {
            // 可能因为各种原因失败，比如权限被拒绝等
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400004].includes(error.code)).assertTrue();
          }

          // 使用portId的边界值
          try {
            serialManager.open(1); // 最小的可能portId
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 可能会抛出异常，也可能返回false，取决于是否存在这个portId
            expect(error != null).assertTrue();
            expect([401, 31400002, 31400003, 31400004].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.open).assertUndefined()
      }
    })

    it('open_multipleTimes', 0, () => {
      // 测试多次调用open
      hilog.info(0x0000, 'testTag', '%{public}s', 'open_multipleTimes test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;

          // 第一次调用
          try {
            serialManager.open(portId);
            // 如果没有抛出异常，说明打开成功
          } catch (error1) {
            // 可能因为各种原因失败，比如权限被拒绝等
            expect(error1 != null).assertTrue();
            expect([31400002, 31400003, 31400004].includes(error1.code)).assertTrue();
            // 如果第一次调用失败，后面的调用也会失败，所以直接返回
            return;
          }

          // 第二次调用（同一端口）
          try {
            serialManager.open(portId);
            expect(false).assertTrue(); // 同一端口已经打开，应该抛出异常
          } catch (error2) {
            // 预期会抛出异常
            expect(error2 != null).assertTrue();
            expect(error2.code).assertEqual(31400004); // 串口设备被占用
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.open).assertUndefined()
      }
    })

    it('open_withDifferentPorts', 0, () => {
      // 测试对不同的端口调用open
      hilog.info(0x0000, 'testTag', '%{public}s', 'open_withDifferentPorts test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        for (let i = 0; i < ports.length; i++) {
          let port = ports[i];
          try {
            serialManager.open(port.portId);
            // 如果没有抛出异常，说明打开成功
            // 打开成功后立即关闭，以便测试下一个端口
            try {
              serialManager.close(port.portId);
            } catch (closeError) {
              // 忽略关闭时的错误
            }
          } catch (error) {
            // 可能因为各种原因失败，比如权限被拒绝等
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400004].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.open).assertUndefined()
      }
    })

    it('close', 0, () => {
      // Defines a test case for close
      hilog.info(0x0000, 'testTag', '%{public}s', 'close test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          try {
            let portId = ports[0].portId;
            serialManager.close(portId);
            // 如果没有抛出异常，说明关闭成功或操作被忽略
          } catch (error) {
            // 可能因为端口未打开而失败，这是预期行为
            hilog.info(0x0000, 'testTag', 'close failed as expected: %{public}s', JSON.stringify(error));
            // 端口未打开，应该抛出异常
            expect(error != null).assertTrue();
            expect([31400003, 31400005].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.close).assertUndefined()
      }
    })

    it('close_invalidPortId', 0, () => {
      // 测试使用无效的portId
      hilog.info(0x0000, 'testTag', '%{public}s', 'close_invalidPortId test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          // 使用负数作为portId
          serialManager.close(-1);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(401); // 参数错误
        }

        try {
          // 使用0作为portId（通常portId从1开始）
          serialManager.close(0);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(31400003); // PortId不存在
        }

        try {
          // 使用非常大的数字作为portId
          serialManager.close(999999999);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(31400003); // PortId不存在
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.close).assertUndefined()
      }
    })

    it('close_boundaryValues', 0, () => {
      // 测试边界值
      hilog.info(0x0000, 'testTag', '%{public}s', 'close_boundaryValues test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          // 使用有效的portId
          let portId = ports[0].portId;
          try {
            serialManager.close(portId);
            // 如果没有抛出异常，说明关闭成功或操作被忽略
          } catch (error) {
            // 可能因为各种原因失败，比如端口未打开等
            expect(error != null).assertTrue();
            expect([31400003, 31400005].includes(error.code)).assertTrue();
          }

          // 使用portId的边界值
          try {
            serialManager.close(1); // 最小的可能portId
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 可能会抛出异常，也可能返回false，取决于是否存在这个portId
            expect(error != null).assertTrue();
            expect([401, 31400003, 31400005].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.close).assertUndefined()
      }
    })

    it('close_multipleTimes', 0, () => {
      // 测试多次调用close
      hilog.info(0x0000, 'testTag', '%{public}s', 'close_multipleTimes test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;

          // 第一次调用
          try {
            serialManager.close(portId);
            // 如果没有抛出异常，说明关闭成功或操作被忽略
          } catch (error1) {
            // 可能因为各种原因失败，比如端口未打开等
            expect(error1 != null).assertTrue();
            expect([31400003, 31400005].includes(error1.code)).assertTrue();
          }

          // 第二次调用
          try {
            serialManager.close(portId);
            // 如果没有抛出异常，说明关闭成功或操作被忽略
          } catch (error2) {
            // 可能因为各种原因失败，比如端口未打开等
            expect(error2 != null).assertTrue();
            expect([31400003, 31400005].includes(error2.code)).assertTrue();
          }

          // 第三次调用
          try {
            serialManager.close(portId);
            // 如果没有抛出异常，说明关闭成功或操作被忽略
          } catch (error3) {
            // 可能因为各种原因失败，比如端口未打开等
            expect(error3 != null).assertTrue();
            expect([31400003, 31400005].includes(error3.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.close).assertUndefined()
      }
    })

    it('close_afterOpen', 0, () => {
      // 测试先打开端口再关闭
      hilog.info(0x0000, 'testTag', '%{public}s', 'close_afterOpen test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;

          try {
            // 尝试打开端口
            serialManager.open(portId);
            // 如果没有抛出异常，说明打开成功

            try {
              // 打开成功后关闭端口
              serialManager.close(portId);
              // 如果没有抛出异常，说明关闭成功
            } catch (closeError) {
              // 关闭失败
              expect(closeError != null).assertTrue();
              expect([31400003, 31400005].includes(closeError.code)).assertTrue();
            }
          } catch (openError) {
            // 打开失败，这是预期行为（非系统应用没有权限）
            expect(openError != null).assertTrue();
            expect([31400002, 31400003, 31400004].includes(openError.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.close).assertUndefined()
      }
    })

    it('close_withDifferentPorts', 0, () => {
      // 测试对不同的端口调用close
      hilog.info(0x0000, 'testTag', '%{public}s', 'close_withDifferentPorts test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        for (let i = 0; i < ports.length; i++) {
          let port = ports[i];
          try {
            serialManager.close(port.portId);
            // 如果没有抛出异常，说明关闭成功或操作被忽略
          } catch (error) {
            // 可能因为各种原因失败，比如端口未打开等
            expect(error != null).assertTrue();
            expect([31400003, 31400005].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.close).assertUndefined()
      }
    })

    it('getAttribute', 0, () => {
      // Defines a test case for getAttribute
      hilog.info(0x0000, 'testTag', '%{public}s', 'getAttribute test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          try {
            let portId = ports[0].portId;
            let attribute = serialManager.getAttribute(portId);
            expect(attribute != null).assertTrue();
            expect(attribute != undefined).assertTrue();
            expect(typeof attribute.baudRate).assertEqual('number');
            expect(typeof attribute.dataBits).assertEqual('number');
            expect(typeof attribute.parity).assertEqual('number');
            expect(typeof attribute.stopBits).assertEqual('number');
            // 验证默认值
            expect(attribute.baudRate).assertEqual(serialManager.BaudRates.BAUDRATE_9600);
            expect(attribute.dataBits).assertEqual(serialManager.DataBits.DATABIT_8);
            expect(attribute.parity).assertEqual(serialManager.Parity.PARITY_NONE);
            expect(attribute.stopBits).assertEqual(serialManager.StopBits.STOPBIT_1);
          } catch (error) {
            // 可能因为端口未打开而失败，这是预期行为
            hilog.info(0x0000, 'testTag', 'getAttribute failed as expected: %{public}s', JSON.stringify(error));
            // 非系统应用没有权限，应该抛出异常
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400005].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.getAttribute).assertUndefined()
      }
    })

    it('getAttribute_invalidPortId', 0, () => {
      // 测试使用无效的portId
      hilog.info(0x0000, 'testTag', '%{public}s', 'getAttribute_invalidPortId test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          // 使用负数作为portId
          serialManager.getAttribute(-1);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(401); // 参数错误
        }

        try {
          // 使用0作为portId（通常portId从1开始）
          serialManager.getAttribute(0);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(31400003); // PortId不存在
        }

        try {
          // 使用非常大的数字作为portId
          serialManager.getAttribute(999999999);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(31400003); // PortId不存在
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.getAttribute).assertUndefined()
      }
    })

    it('getAttribute_beforeOpen', 0, () => {
      // 测试在打开端口之前获取属性
      hilog.info(0x0000, 'testTag', '%{public}s', 'getAttribute_beforeOpen test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;

          try {
            // 端口未打开，尝试获取属性
            serialManager.getAttribute(portId);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(31400005); // 串口设备未打开
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.getAttribute).assertUndefined()
      }
    })

    it('setAttribute', 0, () => {
      // Defines a test case for setAttribute
      hilog.info(0x0000, 'testTag', '%{public}s', 'setAttribute test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          try {
            let portId = ports[0].portId;
            let attribute: serialManager.SerialAttribute = {
              baudRate: serialManager.BaudRates.BAUDRATE_9600,
              dataBits: serialManager.DataBits.DATABIT_8,
              parity: serialManager.Parity.PARITY_NONE,
              stopBits: serialManager.StopBits.STOPBIT_1
            };
            serialManager.setAttribute(portId, attribute);
          } catch (error) {
            // 可能因为端口未打开而失败，这是预期行为
            hilog.info(0x0000, 'testTag', 'setAttribute failed as expected: %{public}s', JSON.stringify(error));
            // 非系统应用没有权限或端口未打开，应该抛出异常
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400005, 401].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.setAttribute).assertUndefined()
      }
    })

    it('setAttribute_invalidPortId', 0, () => {
      // 测试使用无效的portId
      hilog.info(0x0000, 'testTag', '%{public}s', 'setAttribute_invalidPortId test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_9600,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };

        try {
          // 使用负数作为portId
          serialManager.setAttribute(-1, attribute);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(401); // 参数错误
        }

        try {
          // 使用0作为portId（通常portId从1开始）
          serialManager.setAttribute(0, attribute);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(31400003); // PortId不存在
        }

        try {
          // 使用非常大的数字作为portId
          serialManager.setAttribute(999999999, attribute);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(31400003); // PortId不存在
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.setAttribute).assertUndefined()
      }
    })

    it('setAttribute_invalidValues', 0, () => {
      // 测试设置无效的属性值
      hilog.info(0x0000, 'testTag', '%{public}s', 'setAttribute_invalidValues test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;

          try {
            // 设置无效的波特率
            let invalidBaudRate: serialManager.SerialAttribute = {
              baudRate: 9999, // 无效的波特率
              dataBits: serialManager.DataBits.DATABIT_8,
              parity: serialManager.Parity.PARITY_NONE,
              stopBits: serialManager.StopBits.STOPBIT_1
            };
            serialManager.setAttribute(portId, invalidBaudRate);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }

          try {
            // 设置无效的数据位
            let invalidDataBits: serialManager.SerialAttribute = {
              baudRate: serialManager.BaudRates.BAUDRATE_9600,
              dataBits: 9, // 无效的数据位（支持5-8）
              parity: serialManager.Parity.PARITY_NONE,
              stopBits: serialManager.StopBits.STOPBIT_1
            };
            serialManager.setAttribute(portId, invalidDataBits);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }

          try {
            // 设置无效的校验位
            let invalidParity: serialManager.SerialAttribute = {
              baudRate: serialManager.BaudRates.BAUDRATE_9600,
              dataBits: serialManager.DataBits.DATABIT_8,
              parity: 99, // 无效的校验位
              stopBits: serialManager.StopBits.STOPBIT_1
            };
            serialManager.setAttribute(portId, invalidParity);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.setAttribute).assertUndefined()
      }
    })

    it('setAttribute_beforeOpen', 0, () => {
      // 测试在打开端口之前设置属性
      hilog.info(0x0000, 'testTag', '%{public}s', 'setAttribute_beforeOpen test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;
          let attribute: serialManager.SerialAttribute = {
            baudRate: serialManager.BaudRates.BAUDRATE_9600,
            dataBits: serialManager.DataBits.DATABIT_8,
            parity: serialManager.Parity.PARITY_NONE,
            stopBits: serialManager.StopBits.STOPBIT_1
          };

          try {
            // 端口未打开，尝试设置属性
            serialManager.setAttribute(portId, attribute);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(31400005); // 串口设备未打开
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.setAttribute).assertUndefined()
      }
    })

    it('setAttribute_differentValues', 0, () => {
      // 测试设置不同的属性值
      hilog.info(0x0000, 'testTag', '%{public}s', 'setAttribute_differentValues test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          try {
            let portId = ports[0].portId;
            serialManager.open(portId);

            // 设置不同的波特率
            let attribute1: serialManager.SerialAttribute = {
              baudRate: serialManager.BaudRates.BAUDRATE_115200,
              dataBits: serialManager.DataBits.DATABIT_8,
              parity: serialManager.Parity.PARITY_NONE,
              stopBits: serialManager.StopBits.STOPBIT_1
            };
            serialManager.setAttribute(portId, attribute1);

            // 设置不同的数据位
            let attribute2: serialManager.SerialAttribute = {
              baudRate: serialManager.BaudRates.BAUDRATE_9600,
              dataBits: serialManager.DataBits.DATABIT_7,
              parity: serialManager.Parity.PARITY_NONE,
              stopBits: serialManager.StopBits.STOPBIT_1
            };
            serialManager.setAttribute(portId, attribute2);

            // 设置不同的校验位
            let attribute3: serialManager.SerialAttribute = {
              baudRate: serialManager.BaudRates.BAUDRATE_9600,
              dataBits: serialManager.DataBits.DATABIT_8,
              parity: serialManager.Parity.PARITY_EVEN,
              stopBits: serialManager.StopBits.STOPBIT_1
            };
            serialManager.setAttribute(portId, attribute3);

            serialManager.close(portId);
          } catch (error) {
            // 可能因为权限问题或端口被占用而失败，这是预期行为
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400004, 31400005, 401].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.setAttribute).assertUndefined()
      }
    })

    it('read', 0, async () => {
      // Defines a test case for read
      hilog.info(0x0000, 'testTag', '%{public}s', 'read test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          try {
            let portId = ports[0].portId;
            let buffer = new Uint8Array(1024);
            let timeout = 1000; // 1 second timeout
            let bytesRead = await serialManager.read(portId, buffer, timeout);
            expect(typeof bytesRead).assertEqual('number');
            expect(bytesRead >= 0).assertTrue();
            expect(bytesRead <= buffer.length).assertTrue();
          } catch (error) {
            // 可能因为端口未打开或超时而失败，这是预期行为
            hilog.info(0x0000, 'testTag', 'read failed as expected: %{public}s', JSON.stringify(error));
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400005, 401, 31400006].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.read).assertUndefined()
      }
    })

    it('read_invalidPortId', 0, async () => {
      // 测试使用无效的portId
      hilog.info(0x0000, 'testTag', '%{public}s', 'read_invalidPortId test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let buffer = new Uint8Array(1024);
        let timeout = 1000;

        try {
          // 使用负数作为portId
          await serialManager.read(-1, buffer, timeout);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(401); // 参数错误
        }

        try {
          // 使用0作为portId（通常portId从1开始）
          await serialManager.read(0, buffer, timeout);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.read).assertUndefined()
      }
    })

    it('read_invalidBuffer', 0, async () => {
      // 测试使用无效的buffer
      hilog.info(0x0000, 'testTag', '%{public}s', 'read_invalidBuffer test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;
          let timeout = 1000;

          try {
            // 使用null作为buffer
            await serialManager.read(portId, null, timeout);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }

          try {
            // 使用undefined作为buffer
            await serialManager.read(portId, undefined, timeout);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }

          try {
            // 使用空buffer
            let emptyBuffer = new Uint8Array(0);
            await serialManager.read(portId, emptyBuffer, timeout);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.read).assertUndefined()
      }
    })

    it('read_invalidTimeout', 0, async () => {
      // 测试使用无效的timeout
      hilog.info(0x0000, 'testTag', '%{public}s', 'read_invalidTimeout test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;
          let buffer = new Uint8Array(1024);

          try {
            // 使用负数作为timeout
            await serialManager.read(portId, buffer, -1);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }

          try {
            // 使用0作为timeout（非阻塞读取）
            let bytesRead = await serialManager.read(portId, buffer, 0);
            expect(typeof bytesRead).assertEqual('number');
            expect(bytesRead >= 0).assertTrue();
          } catch (error) {
            // 可能会抛出异常，也可能返回0，取决于实现
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400005, 401, 31400006].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.read).assertUndefined()
      }
    })

    it('read_beforeOpen', 0, async () => {
      // 测试在打开端口之前读取数据
      hilog.info(0x0000, 'testTag', '%{public}s', 'read_beforeOpen test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;
          let buffer = new Uint8Array(1024);
          let timeout = 1000;

          try {
            // 端口未打开，尝试读取数据
            await serialManager.read(portId, buffer, timeout);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(31400005); // 串口设备未打开
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.read).assertUndefined()
      }
    })

    it('read_differentBufferSizes', 0, async () => {
      // 测试使用不同大小的buffer
      hilog.info(0x0000, 'testTag', '%{public}s', 'read_differentBufferSizes test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          try {
            let portId = ports[0].portId;
            serialManager.open(portId);

            // 测试小buffer
            let smallBuffer = new Uint8Array(16);
            let bytesRead1 = await serialManager.read(portId, smallBuffer, 100);
            expect(typeof bytesRead1).assertEqual('number');
            expect(bytesRead1 >= 0).assertTrue();
            expect(bytesRead1 <= smallBuffer.length).assertTrue();

            // 测试大buffer
            let largeBuffer = new Uint8Array(4096);
            let bytesRead2 = await serialManager.read(portId, largeBuffer, 100);
            expect(typeof bytesRead2).assertEqual('number');
            expect(bytesRead2 >= 0).assertTrue();
            expect(bytesRead2 <= largeBuffer.length).assertTrue();

            serialManager.close(portId);
          } catch (error) {
            // 可能因为权限问题或端口被占用而失败，这是预期行为
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400004, 31400005, 401, 31400006].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.read).assertUndefined()
      }
    })

    it('readSync', 0, () => {
      // Defines a test case for readSync
      hilog.info(0x0000, 'testTag', '%{public}s', 'readSync test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          try {
            let portId = ports[0].portId;
            let buffer = new Uint8Array(1024);
            let timeout = 1000; // 1 second timeout
            let bytesRead = serialManager.readSync(portId, buffer, timeout);
            expect(typeof bytesRead).assertEqual('number');
            expect(bytesRead >= 0).assertTrue();
            expect(bytesRead <= buffer.length).assertTrue();
          } catch (error) {
            // 可能因为端口未打开或超时而失败，这是预期行为
            hilog.info(0x0000, 'testTag', 'readSync failed as expected: %{public}s', JSON.stringify(error));
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400005, 401, 31400006].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.readSync).assertUndefined()
      }
    })

    it('readSync_invalidPortId', 0, () => {
      // 测试使用无效的portId
      hilog.info(0x0000, 'testTag', '%{public}s', 'readSync_invalidPortId test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let buffer = new Uint8Array(1024);
        let timeout = 1000;

        try {
          // 使用负数作为portId
          serialManager.readSync(-1, buffer, timeout);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(401); // 参数错误
        }

        try {
          // 使用0作为portId（通常portId从1开始）
          serialManager.readSync(0, buffer, timeout);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(31400003); // PortId不存在
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.readSync).assertUndefined()
      }
    })

    it('readSync_invalidBuffer', 0, () => {
      // 测试使用无效的buffer
      hilog.info(0x0000, 'testTag', '%{public}s', 'readSync_invalidBuffer test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;
          let timeout = 1000;

          try {
            // 使用null作为buffer
            serialManager.readSync(portId, null, timeout);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }

          try {
            // 使用undefined作为buffer
            serialManager.readSync(portId, undefined, timeout);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }

          try {
            // 使用空buffer
            let emptyBuffer = new Uint8Array(0);
            serialManager.readSync(portId, emptyBuffer, timeout);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.readSync).assertUndefined()
      }
    })

    it('readSync_invalidTimeout', 0, () => {
      // 测试使用无效的timeout
      hilog.info(0x0000, 'testTag', '%{public}s', 'readSync_invalidTimeout test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;
          let buffer = new Uint8Array(1024);

          try {
            // 使用负数作为timeout
            serialManager.readSync(portId, buffer, -1);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }

          try {
            // 使用0作为timeout（非阻塞读取）
            let bytesRead = serialManager.readSync(portId, buffer, 0);
            expect(typeof bytesRead).assertEqual('number');
            expect(bytesRead >= 0).assertTrue();
          } catch (error) {
            // 可能会抛出异常，也可能返回0，取决于实现
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400005, 401, 31400006].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.readSync).assertUndefined()
      }
    })

    it('readSync_beforeOpen', 0, () => {
      // 测试在打开端口之前读取数据
      hilog.info(0x0000, 'testTag', '%{public}s', 'readSync_beforeOpen test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;
          let buffer = new Uint8Array(1024);
          let timeout = 1000;

          try {
            // 端口未打开，尝试读取数据
            serialManager.readSync(portId, buffer, timeout);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(31400005); // 串口设备未打开
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.readSync).assertUndefined()
      }
    })

    it('readSync_differentBufferSizes', 0, () => {
      // 测试使用不同大小的buffer
      hilog.info(0x0000, 'testTag', '%{public}s', 'readSync_differentBufferSizes test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          try {
            let portId = ports[0].portId;
            serialManager.open(portId);

            // 测试小buffer
            let smallBuffer = new Uint8Array(16);
            let bytesRead1 = serialManager.readSync(portId, smallBuffer, 100);
            expect(typeof bytesRead1).assertEqual('number');
            expect(bytesRead1 >= 0).assertTrue();
            expect(bytesRead1 <= smallBuffer.length).assertTrue();

            // 测试大buffer
            let largeBuffer = new Uint8Array(4096);
            let bytesRead2 = serialManager.readSync(portId, largeBuffer, 100);
            expect(typeof bytesRead2).assertEqual('number');
            expect(bytesRead2 >= 0).assertTrue();
            expect(bytesRead2 <= largeBuffer.length).assertTrue();

            serialManager.close(portId);
          } catch (error) {
            // 可能因为权限问题或端口被占用而失败，这是预期行为
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400004, 31400005, 401, 31400006].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.readSync).assertUndefined()
      }
    })

    it('write', 0, async () => {
      // Defines a test case for write
      hilog.info(0x0000, 'testTag', '%{public}s', 'write test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          try {
            let portId = ports[0].portId;
            let data = new Uint8Array([0x48, 0x65, 0x6C, 0x6C, 0x6F]); // "Hello" in ASCII
            let timeout = 1000; // 1 second timeout
            let bytesWritten = await serialManager.write(portId, data, timeout);
            expect(typeof bytesWritten).assertEqual('number');
            expect(bytesWritten >= 0).assertTrue();
            expect(bytesWritten <= data.length).assertTrue();
          } catch (error) {
            // 可能因为端口未打开而失败，这是预期行为
            hilog.info(0x0000, 'testTag', 'write failed as expected: %{public}s', JSON.stringify(error));
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400005, 401, 31400006].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.write).assertUndefined()
      }
    })

    it('write_invalidPortId', 0, async () => {
      // 测试使用无效的portId
      hilog.info(0x0000, 'testTag', '%{public}s', 'write_invalidPortId test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let data = new Uint8Array([0x48, 0x65, 0x6C, 0x6C, 0x6F]); // "Hello" in ASCII
        let timeout = 1000;

        try {
          // 使用负数作为portId
          await serialManager.write(-1, data, timeout);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(401); // 参数错误
        }

        try {
          // 使用0作为portId（通常portId从1开始）
          await serialManager.write(0, data, timeout);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.write).assertUndefined()
      }
    })

    it('write_invalidData', 0, async () => {
      // 测试使用无效的数据
      hilog.info(0x0000, 'testTag', '%{public}s', 'write_invalidData test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;
          let timeout = 1000;

          try {
            // 使用null作为数据
            await serialManager.write(portId, null, timeout);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }

          try {
            // 使用undefined作为数据
            await serialManager.write(portId, undefined, timeout);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }

          try {
            // 使用空数据
            let emptyData = new Uint8Array(0);
            let bytesWritten = await serialManager.write(portId, emptyData, timeout);
            expect(bytesWritten).assertEqual(0); // 写入空数据应该返回0
          } catch (error) {
            // 可能会抛出异常，也可能返回0，取决于实现
            expect(error != null).assertTrue();
            expect([401, 31400005].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.write).assertUndefined()
      }
    })

    it('write_invalidTimeout', 0, async () => {
      // 测试使用无效的timeout
      hilog.info(0x0000, 'testTag', '%{public}s', 'write_invalidTimeout test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;
          let data = new Uint8Array([0x48, 0x65, 0x6C, 0x6C, 0x6F]);

          try {
            // 使用负数作为timeout
            await serialManager.write(portId, data, -1);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }

          try {
            // 使用0作为timeout（非阻塞写入）
            let bytesWritten = await serialManager.write(portId, data, 0);
            expect(typeof bytesWritten).assertEqual('number');
            expect(bytesWritten >= 0).assertTrue();
          } catch (error) {
            // 可能会抛出异常，也可能返回写入的字节数，取决于实现
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400005, 401, 31400006].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.write).assertUndefined()
      }
    })

    it('write_beforeOpen', 0, async () => {
      // 测试在打开端口之前写入数据
      hilog.info(0x0000, 'testTag', '%{public}s', 'write_beforeOpen test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;
          let data = new Uint8Array([0x48, 0x65, 0x6C, 0x6C, 0x6F]);
          let timeout = 1000;

          try {
            // 端口未打开，尝试写入数据
            await serialManager.write(portId, data, timeout);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(31400005); // 串口设备未打开
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.write).assertUndefined()
      }
    })

    it('write_differentDataSizes', 0, async () => {
      // 测试写入不同大小的数据
      hilog.info(0x0000, 'testTag', '%{public}s', 'write_differentDataSizes test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          try {
            let portId = ports[0].portId;
            serialManager.open(portId);

            // 测试写入少量数据
            let smallData = new Uint8Array([0x48]); // "H" in ASCII
            let bytesWritten1 = await serialManager.write(portId, smallData, 100);
            expect(typeof bytesWritten1).assertEqual('number');
            expect(bytesWritten1 >= 0).assertTrue();
            expect(bytesWritten1 <= smallData.length).assertTrue();

            // 测试写入大量数据
            let largeData = new Uint8Array(1024);
            // 填充数据
            for (let i = 0; i < largeData.length; i++) {
              largeData[i] = i % 256;
            }
            let bytesWritten2 = await serialManager.write(portId, largeData, 1000);
            expect(typeof bytesWritten2).assertEqual('number');
            expect(bytesWritten2 >= 0).assertTrue();
            expect(bytesWritten2 <= largeData.length).assertTrue();

            serialManager.close(portId);
          } catch (error) {
            // 可能因为权限问题或端口被占用而失败，这是预期行为
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400004, 31400005, 401, 31400006].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.write).assertUndefined()
      }
    })

    it('writeSync', 0, () => {
      // Defines a test case for writeSync
      hilog.info(0x0000, 'testTag', '%{public}s', 'writeSync test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          try {
            let portId = ports[0].portId;
            let data = new Uint8Array([0x48, 0x65, 0x6C, 0x6C, 0x6F]); // "Hello" in ASCII
            let timeout = 1000; // 1 second timeout
            let bytesWritten = serialManager.writeSync(portId, data, timeout);
            expect(typeof bytesWritten).assertEqual('number');
            expect(bytesWritten >= 0).assertTrue();
            expect(bytesWritten <= data.length).assertTrue();
          } catch (error) {
            // 可能因为端口未打开而失败，这是预期行为
            hilog.info(0x0000, 'testTag', 'writeSync failed as expected: %{public}s', JSON.stringify(error));
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400005, 401, 31400006].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.writeSync).assertUndefined()
      }
    })

    it('writeSync_invalidPortId', 0, () => {
      // 测试使用无效的portId
      hilog.info(0x0000, 'testTag', '%{public}s', 'writeSync_invalidPortId test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let data = new Uint8Array([0x48, 0x65, 0x6C, 0x6C, 0x6F]); // "Hello" in ASCII
        let timeout = 1000;

        try {
          // 使用负数作为portId
          serialManager.writeSync(-1, data, timeout);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(401); // 参数错误
        }

        try {
          // 使用0作为portId（通常portId从1开始）
          serialManager.writeSync(0, data, timeout);
          expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
        } catch (error) {
          // 预期会抛出异常
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(31400003); // PortId不存在
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.writeSync).assertUndefined()
      }
    })

    it('writeSync_invalidData', 0, () => {
      // 测试使用无效的数据
      hilog.info(0x0000, 'testTag', '%{public}s', 'writeSync_invalidData test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;
          let timeout = 1000;

          try {
            // 使用null作为数据
            serialManager.writeSync(portId, null, timeout);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }

          try {
            // 使用undefined作为数据
            serialManager.writeSync(portId, undefined, timeout);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }

          try {
            // 使用空数据
            let emptyData = new Uint8Array(0);
            let bytesWritten = serialManager.writeSync(portId, emptyData, timeout);
            expect(bytesWritten).assertEqual(0); // 写入空数据应该返回0
          } catch (error) {
            // 可能会抛出异常，也可能返回0，取决于实现
            expect(error != null).assertTrue();
            expect([401, 31400005].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.writeSync).assertUndefined()
      }
    })

    it('writeSync_invalidTimeout', 0, () => {
      // 测试使用无效的timeout
      hilog.info(0x0000, 'testTag', '%{public}s', 'writeSync_invalidTimeout test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;
          let data = new Uint8Array([0x48, 0x65, 0x6C, 0x6C, 0x6F]);

          try {
            // 使用负数作为timeout
            serialManager.writeSync(portId, data, -1);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(401); // 参数错误
          }

          try {
            // 使用0作为timeout（非阻塞写入）
            let bytesWritten = serialManager.writeSync(portId, data, 0);
            expect(typeof bytesWritten).assertEqual('number');
            expect(bytesWritten >= 0).assertTrue();
          } catch (error) {
            // 可能会抛出异常，也可能返回写入的字节数，取决于实现
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400005, 401, 31400006].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.writeSync).assertUndefined()
      }
    })

    it('writeSync_beforeOpen', 0, () => {
      // 测试在打开端口之前写入数据
      hilog.info(0x0000, 'testTag', '%{public}s', 'writeSync_beforeOpen test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          let portId = ports[0].portId;
          let data = new Uint8Array([0x48, 0x65, 0x6C, 0x6C, 0x6F]);
          let timeout = 1000;

          try {
            // 端口未打开，尝试写入数据
            serialManager.writeSync(portId, data, timeout);
            expect(false).assertTrue(); // 应该抛出异常，所以这里断言失败
          } catch (error) {
            // 预期会抛出异常
            expect(error != null).assertTrue();
            expect(error.code).assertEqual(31400005); // 串口设备未打开
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.writeSync).assertUndefined()
      }
    })

    it('writeSync_differentDataSizes', 0, () => {
      // 测试写入不同大小的数据
      hilog.info(0x0000, 'testTag', '%{public}s', 'writeSync_differentDataSizes test begin');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        let ports = serialManager.getPortList();
        if (ports.length > 0) {
          try {
            let portId = ports[0].portId;
            serialManager.open(portId);

            // 测试写入少量数据
            let smallData = new Uint8Array([0x48]); // "H" in ASCII
            let bytesWritten1 = serialManager.writeSync(portId, smallData, 100);
            expect(typeof bytesWritten1).assertEqual('number');
            expect(bytesWritten1 >= 0).assertTrue();
            expect(bytesWritten1 <= smallData.length).assertTrue();

            // 测试写入大量数据
            let largeData = new Uint8Array(1024);
            // 填充数据
            for (let i = 0; i < largeData.length; i++) {
              largeData[i] = i % 256;
            }
            let bytesWritten2 = serialManager.writeSync(portId, largeData, 1000);
            expect(typeof bytesWritten2).assertEqual('number');
            expect(bytesWritten2 >= 0).assertTrue();
            expect(bytesWritten2 <= largeData.length).assertTrue();

            serialManager.close(portId);
          } catch (error) {
            // 可能因为权限问题或端口被占用而失败，这是预期行为
            expect(error != null).assertTrue();
            expect([31400002, 31400003, 31400004, 31400005, 401, 31400006].includes(error.code)).assertTrue();
          }
        }
      } else {
        // Fallback for unsupported SystemCapability
        expect(serialManager.writeSync).assertUndefined()
      }
    })


  })
}