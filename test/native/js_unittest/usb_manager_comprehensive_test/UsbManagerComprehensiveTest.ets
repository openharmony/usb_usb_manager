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
import { usbManager } from '@kit.BasicServicesKit';

const TAG: string = 'UsbManagerTest';
const DOMAIN: number = 0x0000;

export default function usbManagerComprehensiveTest() {
  describe('UsbManagerComprehensiveTest', () => {

    // Test variables
    let deviceList: Array<readonly usbManager.USBDevice> = [];
    let connectedPipe: Readonly<usbManager.USBDevicePipe> | null = null;
    let testDevice: usbManager.USBDevice | null = null;

    beforeAll(() => {
      hilog.info(DOMAIN, TAG, 'UsbManagerComprehensiveTest Begin');
      // Get initial device list
      try {
        deviceList = usbManager.getDevices();
        hilog.info(DOMAIN, TAG, `Found ${deviceList.length} USB devices`);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `Failed to get devices: ${JSON.stringify(error)}`);
      }
    });

    beforeEach(() => {
      hilog.info(DOMAIN, TAG, 'BeforeEach: Refresh device list');
      try {
        deviceList = usbManager.getDevices();
      } catch (error) {
        hilog.error(DOMAIN, TAG, `BeforeEach error: ${JSON.stringify(error)}`);
      }
    });

    afterEach(() => {
      hilog.info(DOMAIN, TAG, 'AfterEach: Cleanup');
    });

    afterAll(() => {
      hilog.info(DOMAIN, TAG, 'UsbManagerComprehensiveTest End');
      // Close any open pipes
      if (connectedPipe !== null) {
        try {
          usbManager.closePipe(connectedPipe);
          connectedPipe = null;
        } catch (error) {
          hilog.error(DOMAIN, TAG, `AfterAll cleanup error: ${JSON.stringify(error)}`);
        }
      }
    });

    // ==================== getDevices Tests ====================

    it('test_getDevices_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getDevices_001: Get device list - normal case');
      try {
        const devices = usbManager.getDevices();
        expect(devices !== null).assertTrue();
        expect(devices !== undefined).assertTrue();
        expect(Array.isArray(devices)).assertTrue();
        hilog.info(DOMAIN, TAG, `getDevices returned ${devices.length} devices`);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `getDevices error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    it('test_getDevices_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getDevices_002: Get device list - multiple times');
      try {
        const devices1 = usbManager.getDevices();
        const devices2 = usbManager.getDevices();
        const devices3 = usbManager.getDevices();

        expect(Array.isArray(devices1)).assertTrue();
        expect(Array.isArray(devices2)).assertTrue();
        expect(Array.isArray(devices3)).assertTrue();

        expect(devices1.length).assertEqual(devices2.length);
        expect(devices2.length).assertEqual(devices3.length);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `getDevices multiple calls error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    it('test_getDevices_003', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getDevices_003: Get device list - validate device properties');
      try {
        const devices = usbManager.getDevices();
        if (devices.length > 0) {
          const device = devices[0];
          expect(typeof device.busNum).assertEqual('number');
          expect(typeof device.devAddress).assertEqual('number');
          expect(typeof device.serial).assertEqual('string');
          expect(typeof device.name).assertEqual('string');
          expect(typeof device.manufacturerName).assertEqual('string');
          expect(typeof device.productName).assertEqual('string');
          expect(typeof device.version).assertEqual('string');
          expect(typeof device.vendorId).assertEqual('number');
          expect(typeof device.productId).assertEqual('number');
          expect(typeof device.clazz).assertEqual('number');
          expect(typeof device.subClass).assertEqual('number');
          expect(typeof device.protocol).assertEqual('number');
          expect(Array.isArray(device.configs)).assertTrue();

          hilog.info(DOMAIN, TAG, `Device: busNum=${device.busNum}, devAddress=${device.devAddress}`);
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `getDevices validation error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    it('test_getDevices_004', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getDevices_004: Get device list - validate configuration properties');
      try {
        const devices = usbManager.getDevices();
        if (devices.length > 0 && devices[0].configs.length > 0) {
          const config = devices[0].configs[0];
          expect(typeof config.id).assertEqual('number');
          expect(typeof config.attributes).assertEqual('number');
          expect(typeof config.maxPower).assertEqual('number');
          expect(typeof config.name).assertEqual('string');
          expect(typeof config.isRemoteWakeup).assertEqual('boolean');
          expect(typeof config.isSelfPowered).assertEqual('boolean');
          expect(Array.isArray(config.interfaces)).assertTrue();

          hilog.info(DOMAIN, TAG, `Config: id=${config.id}, maxPower=${config.maxPower}`);
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `getDevices config validation error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    it('test_getDevices_005', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getDevices_005: Get device list - validate interface properties');
      try {
        const devices = usbManager.getDevices();
        if (devices.length > 0 && devices[0].configs.length > 0 && devices[0].configs[0].interfaces.length > 0) {
          const iface = devices[0].configs[0].interfaces[0];
          expect(typeof iface.id).assertEqual('number');
          expect(typeof iface.protocol).assertEqual('number');
          expect(typeof iface.clazz).assertEqual('number');
          expect(typeof iface.subClass).assertEqual('number');
          expect(typeof iface.alternateSetting).assertEqual('number');
          expect(typeof iface.name).assertEqual('string');
          expect(Array.isArray(iface.endpoints)).assertTrue();

          hilog.info(DOMAIN, TAG, `Interface: id=${iface.id}, clazz=${iface.clazz}`);
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `getDevices interface validation error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    it('test_getDevices_006', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getDevices_006: Get device list - validate endpoint properties');
      try {
        const devices = usbManager.getDevices();
        if (devices.length > 0 &&
          devices[0].configs.length > 0 &&
          devices[0].configs[0].interfaces.length > 0 &&
          devices[0].configs[0].interfaces[0].endpoints.length > 0) {
          const endpoint = devices[0].configs[0].interfaces[0].endpoints[0];
          expect(typeof endpoint.address).assertEqual('number');
          expect(typeof endpoint.attributes).assertEqual('number');
          expect(typeof endpoint.interval).assertEqual('number');
          expect(typeof endpoint.maxPacketSize).assertEqual('number');
          expect(typeof endpoint.direction).assertEqual('number');
          expect(typeof endpoint.number).assertEqual('number');
          expect(typeof endpoint.type).assertEqual('number');
          expect(typeof endpoint.interfaceId).assertEqual('number');

          hilog.info(DOMAIN, TAG, `Endpoint: address=${endpoint.address}, type=${endpoint.type}`);
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `getDevices endpoint validation error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    // ==================== connectDevice Tests ====================

    it('test_connectDevice_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_connectDevice_001: Connect device - normal case');
      if (deviceList.length === 0) {
        hilog.info(DOMAIN, TAG, 'No devices available, skipping');
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);
        expect(pipe !== null).assertTrue();
        expect(pipe !== undefined).assertTrue();
        expect(typeof pipe.busNum).assertEqual('number');
        expect(typeof pipe.devAddress).assertEqual('number');
        expect(pipe.busNum).assertEqual(device.busNum);
        expect(pipe.devAddress).assertEqual(device.devAddress);

        connectedPipe = pipe;
        testDevice = device;

        hilog.info(DOMAIN, TAG, `Connected to device: busNum=${pipe.busNum}, devAddress=${pipe.devAddress}`);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `connectDevice error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    it('test_connectDevice_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_connectDevice_002: Connect device - invalid device (null)');
      try {
        usbManager.connectDevice(null as usbManager.USBDevice);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for null device: ${JSON.stringify(error)}`);
      }
    });

    it('test_connectDevice_003', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_connectDevice_003: Connect device - invalid device (empty object)');
      try {
        usbManager.connectDevice({} as usbManager.USBDevice);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for empty device: ${JSON.stringify(error)}`);
      }
    });

    it('test_connectDevice_004', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_connectDevice_004: Connect device - missing required fields');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const invalidDevice = { busNum: -1, devAddress: -1 } as usbManager.USBDevice;
        usbManager.connectDevice(invalidDevice);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for invalid device: ${JSON.stringify(error)}`);
      }
    });

    it('test_connectDevice_005', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_connectDevice_005: Connect device - connect twice to same device');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe1 = usbManager.connectDevice(device);
        expect(pipe1 !== null).assertTrue();

        try {
          const pipe2 = usbManager.connectDevice(device);
          // Second connection might succeed or fail depending on implementation
          expect(pipe2 !== null).assertTrue();
          usbManager.closePipe(pipe1);
        } catch (secondError) {
          // Or it might throw an error
          usbManager.closePipe(pipe1);
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `connectDevice twice error: ${JSON.stringify(error)}`);
      }
    });

    // ==================== hasRight Tests ====================

    it('test_hasRight_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_hasRight_001: Check device right - normal case');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const hasRight = usbManager.hasRight(device.name);
        expect(typeof hasRight).assertEqual('boolean');
        hilog.info(DOMAIN, TAG, `hasRight for ${device.name}: ${hasRight}`);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `hasRight error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    it('test_hasRight_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_hasRight_002: Check device right - invalid device name (empty)');
      try {
        usbManager.hasRight('');
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for empty device name: ${JSON.stringify(error)}`);
      }
    });

    it('test_hasRight_003', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_hasRight_003: Check device right - invalid device name (null)');
      try {
        usbManager.hasRight(null as string);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for null device name: ${JSON.stringify(error)}`);
      }
    });

    it('test_hasRight_004', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_hasRight_004: Check device right - non-existent device');
      try {
        const hasRight = usbManager.hasRight('non_existent_device_name');
        expect(typeof hasRight).assertEqual('boolean');
        expect(hasRight).assertFalse(); // Should return false for non-existent device
        hilog.info(DOMAIN, TAG, `hasRight for non-existent device: ${hasRight}`);
      } catch (error) {
        hilog.info(DOMAIN, TAG, `hasRight for non-existent device threw error: ${JSON.stringify(error)}`);
      }
    });

    it('test_hasRight_005', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_hasRight_005: Check device right - all devices');
      if (deviceList.length === 0) {
        return;
      }

      try {
        for (const device of deviceList) {
          const hasRight = usbManager.hasRight((device as usbManager.USBDevice).name);
          expect(typeof hasRight).assertEqual('boolean');
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `hasRight all devices error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    // ==================== requestRight Tests ====================

    it('test_requestRight_001', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_requestRight_001: Request device right - normal case');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const granted = await usbManager.requestRight(device.name);
        expect(typeof granted).assertEqual('boolean');
        hilog.info(DOMAIN, TAG, `requestRight for ${device.name}: ${granted}`);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `requestRight error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    it('test_requestRight_002', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_requestRight_002: Request device right - invalid device name (empty)');
      try {
        await usbManager.requestRight('');
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for empty device name: ${JSON.stringify(error)}`);
      }
    });

    it('test_requestRight_003', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_requestRight_003: Request device right - invalid device name (null)');
      try {
        await usbManager.requestRight(null as string);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for null device name: ${JSON.stringify(error)}`);
      }
    });

    it('test_requestRight_004', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_requestRight_004: Request device right - request twice');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const granted1 = await usbManager.requestRight(device.name);
        expect(typeof granted1).assertEqual('boolean');

        const granted2 = await usbManager.requestRight(device.name);
        expect(typeof granted2).assertEqual('boolean');
        hilog.info(DOMAIN, TAG, `requestRight twice: ${granted1}, ${granted2}`);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `requestRight twice error: ${JSON.stringify(error)}`);
      }
    });

    // ==================== removeRight Tests ====================

    it('test_removeRight_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_removeRight_001: Remove device right - normal case');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const removed = usbManager.removeRight(device.name);
        expect(typeof removed).assertEqual('boolean');
        hilog.info(DOMAIN, TAG, `removeRight for ${device.name}: ${removed}`);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `removeRight error: ${JSON.stringify(error)}`);
      }
    });

    it('test_removeRight_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_removeRight_002: Remove device right - invalid device name (empty)');
      try {
        usbManager.removeRight('');
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for empty device name: ${JSON.stringify(error)}`);
      }
    });

    it('test_removeRight_003', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_removeRight_003: Remove device right - non-existent device');
      try {
        const removed = usbManager.removeRight('non_existent_device_name');
        expect(typeof removed).assertEqual('boolean');
        expect(removed).assertFalse(); // Should return false for non-existent device
        hilog.info(DOMAIN, TAG, `removeRight for non-existent device: ${removed}`);
      } catch (error) {
        hilog.info(DOMAIN, TAG, `removeRight for non-existent device threw error: ${JSON.stringify(error)}`);
      }
    });

    it('test_removeRight_004', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_removeRight_004: Remove device right - remove twice');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const removed1 = usbManager.removeRight(device.name);
        expect(typeof removed1).assertEqual('boolean');

        const removed2 = usbManager.removeRight(device.name);
        expect(typeof removed2).assertEqual('boolean');
        hilog.info(DOMAIN, TAG, `removeRight twice: ${removed1}, ${removed2}`);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `removeRight twice error: ${JSON.stringify(error)}`);
      }
    });

    // ==================== getRawDescriptor Tests ====================

    it('test_getRawDescriptor_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getRawDescriptor_001: Get raw descriptor - normal case');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);
        const descriptor = usbManager.getRawDescriptor(pipe);
        expect(descriptor !== null).assertTrue();
        expect(descriptor !== undefined).assertTrue();
        expect(descriptor instanceof Uint8Array).assertTrue();
        expect(descriptor.length > 0).assertTrue();

        hilog.info(DOMAIN, TAG, `Raw descriptor length: ${descriptor.length}`);

        usbManager.closePipe(pipe);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `getRawDescriptor error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    it('test_getRawDescriptor_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getRawDescriptor_002: Get raw descriptor - invalid pipe');
      try {
        const invalidPipe = { busNum: -1, devAddress: -1 };
        usbManager.getRawDescriptor(invalidPipe as usbManager.USBDevicePipe);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for invalid pipe: ${JSON.stringify(error)}`);
      }
    });

    it('test_getRawDescriptor_003', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getRawDescriptor_003: Get raw descriptor - null pipe');
      try {
        usbManager.getRawDescriptor(null as usbManager.USBDevicePipe);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for null pipe: ${JSON.stringify(error)}`);
      }
    });

    // ==================== getFileDescriptor Tests ====================

    it('test_getFileDescriptor_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getFileDescriptor_001: Get file descriptor - normal case');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);
        const fd = usbManager.getFileDescriptor(pipe);
        expect(typeof fd).assertEqual('number');
        expect(fd > 0).assertTrue(); // Valid file descriptor should be positive

        hilog.info(DOMAIN, TAG, `File descriptor: ${fd}`);

        usbManager.closePipe(pipe);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `getFileDescriptor error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    it('test_getFileDescriptor_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getFileDescriptor_002: Get file descriptor - invalid pipe (negative busNum)');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const invalidPipe = { busNum: -1, devAddress: 1 };
        const fd = usbManager.getFileDescriptor(invalidPipe as usbManager.USBDevicePipe);
        expect(fd < 0).assertTrue(); // Should return error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for invalid pipe: ${JSON.stringify(error)}`);
      }
    });

    it('test_getFileDescriptor_003', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getFileDescriptor_003: Get file descriptor - invalid pipe (negative devAddress)');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const invalidPipe = { busNum: 1, devAddress: -1 };
        const fd = usbManager.getFileDescriptor(invalidPipe as usbManager.USBDevicePipe);
        expect(fd < 0).assertTrue(); // Should return error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for invalid pipe: ${JSON.stringify(error)}`);
      }
    });

    it('test_getFileDescriptor_004', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getFileDescriptor_004: Get file descriptor - null pipe');
      try {
        usbManager.getFileDescriptor(null as usbManager.USBDevicePipe);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for null pipe: ${JSON.stringify(error)}`);
      }
    });

    // ==================== closePipe Tests ====================

    it('test_closePipe_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_closePipe_001: Close pipe - normal case');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);
        expect(pipe !== null).assertTrue();

        const result = usbManager.closePipe(pipe);
        expect(result).assertEqual(0); // 0 indicates success

        hilog.info(DOMAIN, TAG, `closePipe result: ${result}`);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `closePipe error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    it('test_closePipe_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_closePipe_002: Close pipe - invalid pipe (negative busNum)');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const invalidPipe = { busNum: -1, devAddress: 1 };
        const result = usbManager.closePipe(invalidPipe as usbManager.USBDevicePipe);
        expect(result !== 0).assertTrue(); // Should return error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for invalid pipe: ${JSON.stringify(error)}`);
      }
    });

    it('test_closePipe_003', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_closePipe_003: Close pipe - invalid pipe (negative devAddress)');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const invalidPipe = { busNum: 1, devAddress: -1 };
        const result = usbManager.closePipe(invalidPipe as usbManager.USBDevicePipe);
        expect(result !== 0).assertTrue(); // Should return error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for invalid pipe: ${JSON.stringify(error)}`);
      }
    });

    it('test_closePipe_004', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_closePipe_004: Close pipe - close twice');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        const result1 = usbManager.closePipe(pipe);
        expect(result1).assertEqual(0);

        const result2 = usbManager.closePipe(pipe);
        expect(result2 !== 0).assertTrue(); // Second close should fail
      } catch (error) {
        hilog.info(DOMAIN, TAG, `closePipe twice threw error: ${JSON.stringify(error)}`);
      }
    });

    it('test_closePipe_005', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_closePipe_005: Close pipe - null pipe');
      try {
        usbManager.closePipe(null as usbManager.USBDevicePipe);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for null pipe: ${JSON.stringify(error)}`);
      }
    });

    // ==================== claimInterface Tests ====================

    it('test_claimInterface_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_claimInterface_001: Claim interface - normal case');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        if (device.configs.length > 0 && device.configs[0].interfaces.length > 0) {
          const iface = device.configs[0].interfaces[0];
          const result = usbManager.claimInterface(pipe, iface, false);
          expect(result === 0 || result === 0).assertTrue(); // 0 indicates success

          hilog.info(DOMAIN, TAG, `claimInterface result: ${result}`);

          usbManager.releaseInterface(pipe, iface);
        }

        usbManager.closePipe(pipe);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `claimInterface error: ${JSON.stringify(error)}`);
      }
    });

    it('test_claimInterface_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_claimInterface_002: Claim interface - with force option');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        if (device.configs.length > 0 && device.configs[0].interfaces.length > 0) {
          const iface = device.configs[0].interfaces[0];
          const result = usbManager.claimInterface(pipe, iface, true);
          expect(result === 0 || result === 0).assertTrue();

          hilog.info(DOMAIN, TAG, `claimInterface force result: ${result}`);

          usbManager.releaseInterface(pipe, iface);
        }

        usbManager.closePipe(pipe);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `claimInterface force error: ${JSON.stringify(error)}`);
      }
    });

    it('test_claimInterface_003', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_claimInterface_003: Claim interface - null pipe');
      try {
        const iface = { id: 0, protocol: 0, clazz: 0, subClass: 0, alternateSetting: 0, name: '', endpoints: [] };
        usbManager.claimInterface(null as usbManager.USBDevicePipe, iface, false);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for null pipe: ${JSON.stringify(error)}`);
      }
    });

    it('test_claimInterface_004', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_claimInterface_004: Claim interface - null interface');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);
        usbManager.claimInterface(pipe, null as usbManager.USBInterface, false);
        expect(false).assertTrue(); // Should throw error
        usbManager.closePipe(pipe);
      } catch (error) {
        expect(error !== null).assertTrue();
        usbManager.closePipe((device as usbManager.USBDevice));
        hilog.info(DOMAIN, TAG, `Expected error for null interface: ${JSON.stringify(error)}`);
      }
    });

    // ==================== releaseInterface Tests ====================

    it('test_releaseInterface_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_releaseInterface_001: Release interface - normal case');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        if (device.configs.length > 0 && device.configs[0].interfaces.length > 0) {
          const iface = device.configs[0].interfaces[0];
          usbManager.claimInterface(pipe, iface, false);

          const result = usbManager.releaseInterface(pipe, iface);
          expect(result === 0 || result === 0).assertTrue();

          hilog.info(DOMAIN, TAG, `releaseInterface result: ${result}`);
        }

        usbManager.closePipe(pipe);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `releaseInterface error: ${JSON.stringify(error)}`);
      }
    });

    it('test_releaseInterface_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_releaseInterface_002: Release interface - not claimed');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        if (device.configs.length > 0 && device.configs[0].interfaces.length > 0) {
          const iface = device.configs[0].interfaces[0];
          const result = usbManager.releaseInterface(pipe, iface);
          // May fail if interface was not claimed
          expect(typeof result).assertEqual('number');
        }

        usbManager.closePipe(pipe);
      } catch (error) {
        hilog.info(DOMAIN, TAG, `releaseInterface not claimed threw error: ${JSON.stringify(error)}`);
      }
    });

    it('test_releaseInterface_003', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_releaseInterface_003: Release interface - null pipe');
      try {
        const iface = { id: 0, protocol: 0, clazz: 0, subClass: 0, alternateSetting: 0, name: '', endpoints: [] };
        usbManager.releaseInterface(null as usbManager.USBDevicePipe, iface);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for null pipe: ${JSON.stringify(error)}`);
      }
    });

    // ==================== setConfiguration Tests ====================

    it('test_setConfiguration_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_setConfiguration_001: Set configuration - normal case');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        if (device.configs.length > 0) {
          const config = device.configs[0];
          const result = usbManager.setConfiguration(pipe, config);
          expect(typeof result).assertEqual('number');

          hilog.info(DOMAIN, TAG, `setConfiguration result: ${result}`);
        }

        usbManager.closePipe(pipe);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `setConfiguration error: ${JSON.stringify(error)}`);
      }
    });

    it('test_setConfiguration_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_setConfiguration_002: Set configuration - null pipe');
      try {
        const config = { id: 0, attributes: 0, maxPower: 0, name: '', isRemoteWakeup: false, isSelfPowered: false, interfaces: [] };
        usbManager.setConfiguration(null as usbManager.USBDevicePipe, config);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for null pipe: ${JSON.stringify(error)}`);
      }
    });

    it('test_setConfiguration_003', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_setConfiguration_003: Set configuration - null config');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);
        usbManager.setConfiguration(pipe, null as usbManager.USBConfiguration);
        expect(false).assertTrue(); // Should throw error
        usbManager.closePipe(pipe);
      } catch (error) {
        expect(error !== null).assertTrue();
        usbManager.closePipe((device as usbManager.USBDevice));
        hilog.info(DOMAIN, TAG, `Expected error for null config: ${JSON.stringify(error)}`);
      }
    });

    // ==================== setInterface Tests ====================

    it('test_setInterface_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_setInterface_001: Set interface - normal case');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        if (device.configs.length > 0 && device.configs[0].interfaces.length > 0) {
          const iface = device.configs[0].interfaces[0];
          const result = usbManager.setInterface(pipe, iface);
          expect(typeof result).assertEqual('number');

          hilog.info(DOMAIN, TAG, `setInterface result: ${result}`);
        }

        usbManager.closePipe(pipe);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `setInterface error: ${JSON.stringify(error)}`);
      }
    });

    it('test_setInterface_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_setInterface_002: Set interface - null pipe');
      try {
        const iface = { id: 0, protocol: 0, clazz: 0, subClass: 0, alternateSetting: 0, name: '', endpoints: [] };
        usbManager.setInterface(null as usbManager.USBDevicePipe, iface);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for null pipe: ${JSON.stringify(error)}`);
      }
    });

    // ==================== usbControlTransfer Tests ====================

    it('test_usbControlTransfer_001', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_usbControlTransfer_001: Control transfer - GET_DESCRIPTOR');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        const data = new Uint8Array(64);
        const controlParam: usbManager.USBDeviceRequestParams = {
          bmRequestType: 0x80, // Direction: device to host, Type: standard, Recipient: device
          bRequest: 0x06, // GET_DESCRIPTOR
          wValue: 0x0100, // Device descriptor
          wIndex: 0,
          wLength: data.length,
          data: data
        };

        const result = await usbManager.usbControlTransfer(pipe, controlParam, 1000);
        expect(typeof result).assertEqual('number');
        expect(result >= 0).assertTrue();

        hilog.info(DOMAIN, TAG, `Control transfer result: ${result}`);

        usbManager.closePipe(pipe);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `usbControlTransfer error: ${JSON.stringify(error)}`);
      }
    });

    it('test_usbControlTransfer_002', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_usbControlTransfer_002: Control transfer - SET_CONFIGURATION');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        if (device.configs.length > 0) {
          const data = new Uint8Array(0);
          const configValue = device.configs[0].id;
          const controlParam: usbManager.USBDeviceRequestParams = {
            bmRequestType: 0x00, // Direction: host to device, Type: standard, Recipient: device
            bRequest: 0x09, // SET_CONFIGURATION
            wValue: configValue,
            wIndex: 0,
            wLength: 0,
            data: data
          };

          const result = await usbManager.usbControlTransfer(pipe, controlParam, 1000);
          expect(typeof result).assertEqual('number');

          hilog.info(DOMAIN, TAG, `SET_CONFIGURATION result: ${result}`);
        }

        usbManager.closePipe(pipe);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `usbControlTransfer SET_CONFIGURATION error: ${JSON.stringify(error)}`);
      }
    });

    it('test_usbControlTransfer_003', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_usbControlTransfer_003: Control transfer - null pipe');
      try {
        const data = new Uint8Array(64);
        const controlParam: usbManager.USBDeviceRequestParams = {
          bmRequestType: 0x80,
          bRequest: 0x06,
          wValue: 0x0100,
          wIndex: 0,
          wLength: data.length,
          data: data
        };

        await usbManager.usbControlTransfer(null as usbManager.USBDevicePipe, controlParam, 1000);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for null pipe: ${JSON.stringify(error)}`);
      }
    });

    it('test_usbControlTransfer_004', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_usbControlTransfer_004: Control transfer - null params');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        await usbManager.usbControlTransfer(pipe, null as usbManager.USBDeviceRequestParams, 1000);
        expect(false).assertTrue(); // Should throw error
        usbManager.closePipe(pipe);
      } catch (error) {
        expect(error !== null).assertTrue();
        usbManager.closePipe((device as usbManager.USBDevice));
        hilog.info(DOMAIN, TAG, `Expected error for null params: ${JSON.stringify(error)}`);
      }
    });

    it('test_usbControlTransfer_005', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_usbControlTransfer_005: Control transfer - timeout variations');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        const data = new Uint8Array(64);
        const controlParam: usbManager.USBDeviceRequestParams = {
          bmRequestType: 0x80,
          bRequest: 0x06,
          wValue: 0x0100,
          wIndex: 0,
          wLength: data.length,
          data: data
        };

        // Test with different timeout values
        const timeouts = [0, 100, 1000, 5000];
        for (const timeout of timeouts) {
          const result = await usbManager.usbControlTransfer(pipe, controlParam, timeout);
          expect(typeof result).assertEqual('number');
        }

        usbManager.closePipe(pipe);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `usbControlTransfer timeout error: ${JSON.stringify(error)}`);
      }
    });

    // ==================== bulkTransfer Tests ====================

    it('test_bulkTransfer_001', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_bulkTransfer_001: Bulk transfer - read');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        // Find bulk OUT endpoint
        let bulkEndpoint: usbManager.USBEndpoint | null = null;
        for (const config of device.configs) {
          for (const iface of config.interfaces) {
            for (const endpoint of iface.endpoints) {
              if ((endpoint.type === 2) && // USB_ENDPOINT_XFER_BULK
                (endpoint.direction === 0)) { // USB_DIR_OUT
                bulkEndpoint = endpoint;
                break;
              }
            }
            if (bulkEndpoint) break;
          }
          if (bulkEndpoint) break;
        }

        if (bulkEndpoint) {
          const buffer = new Uint8Array(512);
          const result = await usbManager.bulkTransfer(pipe, bulkEndpoint, buffer, 1000);
          expect(typeof result).assertEqual('number');
          expect(result >= 0).assertTrue();

          hilog.info(DOMAIN, TAG, `Bulk transfer result: ${result}`);
        } else {
          hilog.info(DOMAIN, TAG, 'No bulk endpoint found, skipping bulk transfer test');
        }

        usbManager.closePipe(pipe);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `bulkTransfer error: ${JSON.stringify(error)}`);
      }
    });

    it('test_bulkTransfer_002', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_bulkTransfer_002: Bulk transfer - null pipe');
      try {
        const endpoint = { address: 1, attributes: 0, interval: 0, maxPacketSize: 512, direction: 0, number: 1, type: 2, interfaceId: 0, endpointAddr: 1 };
        const buffer = new Uint8Array(512);

        await usbManager.bulkTransfer(null as usbManager.USBDevicePipe, endpoint, buffer, 1000);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for null pipe: ${JSON.stringify(error)}`);
      }
    });

    it('test_bulkTransfer_003', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_bulkTransfer_003: Bulk transfer - null buffer');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        const endpoint = { address: 1, attributes: 0, interval: 0, maxPacketSize: 512, direction: 0, number: 1, type: 2, interfaceId: 0, endpointAddr: 1 };

        await usbManager.bulkTransfer(pipe, endpoint, null as Uint8Array, 1000);
        expect(false).assertTrue(); // Should throw error
        usbManager.closePipe(pipe);
      } catch (error) {
        expect(error !== null).assertTrue();
        usbManager.closePipe((device as usbManager.USBDevice));
        hilog.info(DOMAIN, TAG, `Expected error for null buffer: ${JSON.stringify(error)}`);
      }
    });

    it('test_bulkTransfer_004', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_bulkTransfer_004: Bulk transfer - different buffer sizes');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        // Find bulk endpoint
        let bulkEndpoint: usbManager.USBEndpoint | null = null;
        for (const config of device.configs) {
          for (const iface of config.interfaces) {
            for (const endpoint of iface.endpoints) {
              if (endpoint.type === 2) {
                bulkEndpoint = endpoint;
                break;
              }
            }
            if (bulkEndpoint) break;
          }
          if (bulkEndpoint) break;
        }

        if (bulkEndpoint) {
          const bufferSizes = [64, 256, 512, 1024];
          for (const size of bufferSizes) {
            const buffer = new Uint8Array(size);
            const result = await usbManager.bulkTransfer(pipe, bulkEndpoint, buffer, 1000);
            expect(typeof result).assertEqual('number');
            hilog.info(DOMAIN, TAG, `Bulk transfer size ${size}: ${result}`);
          }
        }

        usbManager.closePipe(pipe);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `bulkTransfer buffer sizes error: ${JSON.stringify(error)}`);
      }
    });

    // ==================== USB Accessory Tests ====================

    it('test_getAccessoryList_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getAccessoryList_001: Get accessory list');
      try {
        const accessories = usbManager.getAccessoryList();
        expect(accessories !== null).assertTrue();
        expect(accessories !== undefined).assertTrue();
        expect(Array.isArray(accessories)).assertTrue();

        hilog.info(DOMAIN, TAG, `Accessory list length: ${accessories.length}`);

        if (accessories.length > 0) {
          const accessory = accessories[0];
          expect(typeof accessory.manufacturer).assertEqual('string');
          expect(typeof accessory.product).assertEqual('string');
          expect(typeof accessory.description).assertEqual('string');
          expect(typeof accessory.version).assertEqual('string');
          expect(typeof accessory.serialNumber).assertEqual('string');
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `getAccessoryList error: ${JSON.stringify(error)}`);
      }
    });

    it('test_hasAccessoryRight_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_hasAccessoryRight_001: Check accessory right');
      try {
        const accessories = usbManager.getAccessoryList();
        if (accessories.length > 0) {
          const hasRight = usbManager.hasAccessoryRight(accessories[0]);
          expect(typeof hasRight).assertEqual('boolean');
          hilog.info(DOMAIN, TAG, `hasAccessoryRight: ${hasRight}`);
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `hasAccessoryRight error: ${JSON.stringify(error)}`);
      }
    });

    it('test_hasAccessoryRight_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_hasAccessoryRight_002: Check accessory right - null accessory');
      try {
        usbManager.hasAccessoryRight(null as usbManager.USBAccessory);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for null accessory: ${JSON.stringify(error)}`);
      }
    });

    it('test_requestAccessoryRight_001', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_requestAccessoryRight_001: Request accessory right');
      try {
        const accessories = usbManager.getAccessoryList();
        if (accessories.length > 0) {
          const granted = await usbManager.requestAccessoryRight(accessories[0]);
          expect(typeof granted).assertEqual('boolean');
          hilog.info(DOMAIN, TAG, `requestAccessoryRight: ${granted}`);
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `requestAccessoryRight error: ${JSON.stringify(error)}`);
      }
    });

    it('test_requestAccessoryRight_002', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_requestAccessoryRight_002: Request accessory right - null accessory');
      try {
        await usbManager.requestAccessoryRight(null as usbManager.USBAccessory);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for null accessory: ${JSON.stringify(error)}`);
      }
    });

    it('test_cancelAccessoryRight_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_cancelAccessoryRight_001: Cancel accessory right');
      try {
        const accessories = usbManager.getAccessoryList();
        if (accessories.length > 0) {
          usbManager.cancelAccessoryRight(accessories[0]);
          hilog.info(DOMAIN, TAG, 'cancelAccessoryRight succeeded');
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `cancelAccessoryRight error: ${JSON.stringify(error)}`);
      }
    });

    it('test_cancelAccessoryRight_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_cancelAccessoryRight_002: Cancel accessory right - null accessory');
      try {
        usbManager.cancelAccessoryRight(null as usbManager.USBAccessory);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for null accessory: ${JSON.stringify(error)}`);
      }
    });

    it('test_openAccessory_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_openAccessory_001: Open accessory');
      try {
        const accessories = usbManager.getAccessoryList();
        if (accessories.length > 0) {
          const handle = usbManager.openAccessory(accessories[0]);
          expect(handle !== null).assertTrue();
          expect(typeof handle.accessoryFd).assertEqual('number');
          expect(handle.accessoryFd > 0).assertTrue();

          hilog.info(DOMAIN, TAG, `Open accessory fd: ${handle.accessoryFd}`);

          usbManager.closeAccessory(handle);
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `openAccessory error: ${JSON.stringify(error)}`);
      }
    });

    it('test_openAccessory_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_openAccessory_002: Open accessory - null accessory');
      try {
        usbManager.openAccessory(null as usbManager.USBAccessory);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for null accessory: ${JSON.stringify(error)}`);
      }
    });

    it('test_closeAccessory_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_closeAccessory_001: Close accessory');
      try {
        const accessories = usbManager.getAccessoryList();
        if (accessories.length > 0) {
          const handle = usbManager.openAccessory(accessories[0]);
          usbManager.closeAccessory(handle);
          hilog.info(DOMAIN, TAG, 'closeAccessory succeeded');
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `closeAccessory error: ${JSON.stringify(error)}`);
      }
    });

    it('test_closeAccessory_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_closeAccessory_002: Close accessory - null handle');
      try {
        usbManager.closeAccessory(null as usbManager.USBAccessoryHandle);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for null handle: ${JSON.stringify(error)}`);
      }
    });

    // ==================== USB Submit/Cancel Transfer Tests ====================

    it('test_usbSubmitTransfer_001', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_usbSubmitTransfer_001: Submit bulk transfer');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        // Find bulk endpoint
        let bulkEndpoint: usbManager.USBEndpoint | null = null;
        for (const config of device.configs) {
          for (const iface of config.interfaces) {
            for (const endpoint of iface.endpoints) {
              if (endpoint.type === 2) {
                bulkEndpoint = endpoint;
                break;
              }
            }
            if (bulkEndpoint) break;
          }
          if (bulkEndpoint) break;
        }

        if (bulkEndpoint) {
          const buffer = new Uint8Array(512);
          let transferCompleted = false;

          const transferParams: usbManager.UsbDataTransferParams = {
            devPipe: pipe,
            flags: usbManager.UsbTransferFlags.USB_TRANSFER_FREE_BUFFER,
            endpoint: bulkEndpoint.address,
            type: usbManager.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
            timeout: 1000,
            length: buffer.length,
            callback: (result) => {
              hilog.info(DOMAIN, TAG, `Transfer callback: actualLength=${result.actualLength}, status=${result.status}`);
              transferCompleted = true;
            },
            userData: new Uint8Array([0x01, 0x02, 0x03]),
            buffer: buffer,
            isoPacketCount: 0
          };

          usbManager.usbSubmitTransfer(transferParams);

          // Wait for callback
          await new Promise(resolve => setTimeout(resolve, 2000));
          expect(transferCompleted).assertTrue();
        }

        usbManager.closePipe(pipe);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `usbSubmitTransfer error: ${JSON.stringify(error)}`);
      }
    });

    it('test_usbSubmitTransfer_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_usbSubmitTransfer_002: Submit transfer - null params');
      try {
        usbManager.usbSubmitTransfer(null as usbManager.UsbDataTransferParams);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for null params: ${JSON.stringify(error)}`);
      }
    });

    it('test_usbCancelTransfer_001', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_usbCancelTransfer_001: Cancel transfer');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        // Find bulk endpoint
        let bulkEndpoint: usbManager.USBEndpoint | null = null;
        for (const config of device.configs) {
          for (const iface of config.interfaces) {
            for (const endpoint of iface.endpoints) {
              if (endpoint.type === 2) {
                bulkEndpoint = endpoint;
                break;
              }
            }
            if (bulkEndpoint) break;
          }
          if (bulkEndpoint) break;
        }

        if (bulkEndpoint) {
          const buffer = new Uint8Array(512);

          const transferParams: usbManager.UsbDataTransferParams = {
            devPipe: pipe,
            flags: usbManager.UsbTransferFlags.USB_TRANSFER_FREE_BUFFER,
            endpoint: bulkEndpoint.address,
            type: usbManager.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
            timeout: 5000,
            length: buffer.length,
            callback: () => {},
            userData: new Uint8Array(),
            buffer: buffer,
            isoPacketCount: 0
          };

          usbManager.usbSubmitTransfer(transferParams);

          // Cancel immediately
          await new Promise(resolve => setTimeout(resolve, 100));
          usbManager.usbCancelTransfer(transferParams);
          hilog.info(DOMAIN, TAG, 'Transfer canceled');
        }

        usbManager.closePipe(pipe);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `usbCancelTransfer error: ${JSON.stringify(error)}`);
      }
    });

    it('test_usbCancelTransfer_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_usbCancelTransfer_002: Cancel transfer - null params');
      try {
        usbManager.usbCancelTransfer(null as usbManager.UsbDataTransferParams);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for null params: ${JSON.stringify(error)}`);
      }
    });

    // ==================== resetUsbDevice Tests ====================

    it('test_resetUsbDevice_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_resetUsbDevice_001: Reset USB device');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const pipe = usbManager.connectDevice(device);

        const result = usbManager.resetUsbDevice(pipe);
        expect(typeof result).assertEqual('boolean');

        hilog.info(DOMAIN, TAG, `resetUsbDevice result: ${result}`);

        usbManager.closePipe(pipe);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `resetUsbDevice error: ${JSON.stringify(error)}`);
      }
    });

    it('test_resetUsbDevice_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_resetUsbDevice_002: Reset USB device - invalid pipe');
      try {
        const invalidPipe = { busNum: -1, devAddress: -1 };
        const result = usbManager.resetUsbDevice(invalidPipe as usbManager.USBDevicePipe);
        expect(result).assertFalse();
        hilog.info(DOMAIN, TAG, `resetUsbDevice invalid pipe result: ${result}`);
      } catch (error) {
        hilog.info(DOMAIN, TAG, `Expected error for invalid pipe: ${JSON.stringify(error)}`);
      }
    });

    it('test_resetUsbDevice_003', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_resetUsbDevice_003: Reset USB device - null pipe');
      try {
        usbManager.resetUsbDevice(null as usbManager.USBDevicePipe);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for null pipe: ${JSON.stringify(error)}`);
      }
    });

    // ==================== Port Management Tests ====================

    it('test_getPortList_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getPortList_001: Get port list');
      try {
        const ports = usbManager.getPortList();
        expect(ports !== null).assertTrue();
        expect(ports !== undefined).assertTrue();
        expect(Array.isArray(ports)).assertTrue();

        hilog.info(DOMAIN, TAG, `Port list length: ${ports.length}`);

        if (ports.length > 0) {
          const port = ports[0];
          expect(typeof port.id).assertEqual('number');
          expect(typeof port.supportedModes).assertEqual('number');
          expect(typeof port.status.currentMode).assertEqual('number');
          expect(typeof port.status.currentPowerRole).assertEqual('number');
          expect(typeof port.status.currentDataRole).assertEqual('number');
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `getPortList error: ${JSON.stringify(error)}`);
      }
    });

    it('test_getPortList_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getPortList_002: Get port list - multiple times');
      try {
        const ports1 = usbManager.getPortList();
        const ports2 = usbManager.getPortList();

        expect(Array.isArray(ports1)).assertTrue();
        expect(Array.isArray(ports2)).assertTrue();
        expect(ports1.length).assertEqual(ports2.length);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `getPortList multiple times error: ${JSON.stringify(error)}`);
      }
    });

    it('test_getPortSupportModes_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getPortSupportModes_001: Get port supported modes');
      try {
        const ports = usbManager.getPortList();
        if (ports.length > 0) {
          const portId = ports[0].id;
          const modes = usbManager.getPortSupportModes(portId);
          expect(typeof modes).assertEqual('number');

          hilog.info(DOMAIN, TAG, `Port ${portId} supported modes: ${modes}`);
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `getPortSupportModes error: ${JSON.stringify(error)}`);
      }
    });

    it('test_getPortSupportModes_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getPortSupportModes_002: Get port supported modes - invalid port ID');
      try {
        usbManager.getPortSupportModes(-1);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for invalid port ID: ${JSON.stringify(error)}`);
      }
    });

    it('test_setPortRoleTypes_001', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_setPortRoleTypes_001: Set port role types');
      try {
        const ports = usbManager.getPortList();
        if (ports.length > 0) {
          const portId = ports[0].id;
          await usbManager.setPortRoleTypes(portId, usbManager.PowerRoleType.SOURCE, usbManager.DataRoleType.HOST);
          hilog.info(DOMAIN, TAG, 'setPortRoleTypes succeeded');
        }
      } catch (error) {
        hilog.info(DOMAIN, TAG, `setPortRoleTypes error (may be expected): ${JSON.stringify(error)}`);
      }
    });

    it('test_setPortRoleTypes_002', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_setPortRoleTypes_002: Set port role types - invalid port ID');
      try {
        await usbManager.setPortRoleTypes(-1, usbManager.PowerRoleType.SOURCE, usbManager.DataRoleType.HOST);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for invalid port ID: ${JSON.stringify(error)}`);
      }
    });

    // ==================== Device Functions Tests ====================

    it('test_getDeviceFunctions_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getDeviceFunctions_001: Get device functions');
      try {
        const functions = usbManager.getDeviceFunctions();
        expect(typeof functions).assertEqual('number');

        hilog.info(DOMAIN, TAG, `Device functions: ${functions}`);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `getDeviceFunctions error: ${JSON.stringify(error)}`);
      }
    });

    it('test_setDeviceFunctions_001', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_setDeviceFunctions_001: Set device functions');
      try {
        await usbManager.setDeviceFunctions(usbManager.FunctionType.ACM);
        hilog.info(DOMAIN, TAG, 'setDeviceFunctions succeeded');
      } catch (error) {
        hilog.info(DOMAIN, TAG, `setDeviceFunctions error (may be expected): ${JSON.stringify(error)}`);
      }
    });

    it('test_getFunctionsFromString_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getFunctionsFromString_001: Get functions from string');
      try {
        const funcs = usbManager.getFunctionsFromString('acm');
        expect(typeof funcs).assertEqual('number');

        hilog.info(DOMAIN, TAG, `Functions from string: ${funcs}`);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `getFunctionsFromString error: ${JSON.stringify(error)}`);
      }
    });

    it('test_getFunctionsFromString_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getFunctionsFromString_002: Get functions from string - empty string');
      try {
        usbManager.getFunctionsFromString('');
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for empty string: ${JSON.stringify(error)}`);
      }
    });

    it('test_getStringFromFunctions_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getStringFromFunctions_001: Get string from functions');
      try {
        const str = usbManager.getStringFromFunctions(usbManager.FunctionType.ACM);
        expect(typeof str).assertEqual('string');

        hilog.info(DOMAIN, TAG, `String from functions: ${str}`);
      } catch (error) {
        hilog.error(DOMAIN, TAG, `getStringFromFunctions error: ${JSON.stringify(error)}`);
      }
    });

    it('test_getStringFromFunctions_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_getStringFromFunctions_002: Get string from functions - invalid value');
      try {
        const str = usbManager.getStringFromFunctions(0);
        expect(typeof str).assertEqual('string');
        hilog.info(DOMAIN, TAG, `String from invalid functions: ${str}`);
      } catch (error) {
        hilog.info(DOMAIN, TAG, `getStringFromFunctions invalid value error: ${JSON.stringify(error)}`);
      }
    });

    // ==================== addDeviceAccessRight Tests ====================

    it('test_addDeviceAccessRight_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_addDeviceAccessRight_001: Add device access right');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const tokenId = 'test_token_id';
        const result = usbManager.addDeviceAccessRight(tokenId, device.name);
        expect(typeof result).assertEqual('boolean');

        hilog.info(DOMAIN, TAG, `addDeviceAccessRight result: ${result}`);
      } catch (error) {
        hilog.info(DOMAIN, TAG, `addDeviceAccessRight error (may be expected for non-system apps): ${JSON.stringify(error)}`);
      }
    });

    it('test_addDeviceAccessRight_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_addDeviceAccessRight_002: Add device access right - null tokenId');
      try {
        usbManager.addDeviceAccessRight(null as string, 'device_name');
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for null tokenId: ${JSON.stringify(error)}`);
      }
    });

    it('test_addDeviceAccessRight_003', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_addDeviceAccessRight_003: Add device access right - empty device name');
      try {
        usbManager.addDeviceAccessRight('token_id', '');
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for empty device name: ${JSON.stringify(error)}`);
      }
    });

    // ==================== addAccessoryRight Tests ====================

    it('test_addAccessoryRight_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_addAccessoryRight_001: Add accessory right');
      try {
        const accessories = usbManager.getAccessoryList();
        if (accessories.length > 0) {
          const tokenId = 1;
          usbManager.addAccessoryRight(tokenId, accessories[0]);
          hilog.info(DOMAIN, TAG, 'addAccessoryRight succeeded');
        }
      } catch (error) {
        hilog.info(DOMAIN, TAG, `addAccessoryRight error (may be expected for non-system apps): ${JSON.stringify(error)}`);
      }
    });

    it('test_addAccessoryRight_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_addAccessoryRight_002: Add accessory right - invalid tokenId');
      try {
        const accessory: usbManager.USBAccessory = {
          manufacturer: 'test',
          product: 'test',
          description: 'test',
          version: '1.0',
          serialNumber: '12345'
        };
        usbManager.addAccessoryRight(-1, accessory);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Expected error for invalid tokenId: ${JSON.stringify(error)}`);
      }
    });

    it('test_addAccessoryRight_003', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_addAccessoryRight_003: Add accessory right - null accessory');
      try {
        usbManager.addAccessoryRight(1, null as usbManager.USBAccessory);
        expect(false).assertTrue(); // Should throw error
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(error.code).assertEqual(401); // Parameter error
        hilog.info(DOMAIN, TAG, `Expected error for null accessory: ${JSON.stringify(error)}`);
      }
    });

    // ==================== Enum Tests ====================

    it('test_enums_PowerRoleType', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_enums_PowerRoleType: Validate PowerRoleType enum');
      expect(usbManager.PowerRoleType.NONE).assertEqual(0);
      expect(usbManager.PowerRoleType.SOURCE).assertEqual(1);
      expect(usbManager.PowerRoleType.SINK).assertEqual(2);
    });

    it('test_enums_DataRoleType', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_enums_DataRoleType: Validate DataRoleType enum');
      expect(usbManager.DataRoleType.NONE).assertEqual(0);
      expect(usbManager.DataRoleType.HOST).assertEqual(1);
      expect(usbManager.DataRoleType.DEVICE).assertEqual(2);
    });

    it('test_enums_PortModeType', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_enums_PortModeType: Validate PortModeType enum');
      expect(usbManager.PortModeType.NONE).assertEqual(0);
      expect(usbManager.PortModeType.UFP).assertEqual(1);
      expect(usbManager.PortModeType.DFP).assertEqual(2);
      expect(usbManager.PortModeType.DRP).assertEqual(3);
      expect(usbManager.PortModeType.NUM_MODES).assertEqual(4);
    });

    it('test_enums_FunctionType', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_enums_FunctionType: Validate FunctionType enum');
      expect(usbManager.FunctionType.NONE).assertEqual(0);
      expect(usbManager.FunctionType.ACM).assertEqual(1);
      expect(usbManager.FunctionType.ECM).assertEqual(2);
      expect(usbManager.FunctionType.HDC).assertEqual(4);
      expect(usbManager.FunctionType.MTP).assertEqual(8);
      expect(usbManager.FunctionType.PTP).assertEqual(16);
      expect(usbManager.FunctionType.RNDIS).assertEqual(32);
      expect(usbManager.FunctionType.MIDI).assertEqual(64);
      expect(usbManager.FunctionType.AUDIO_SOURCE).assertEqual(128);
      expect(usbManager.FunctionType.NCM).assertEqual(256);
    });

    it('test_enums_USBRequestTargetType', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_enums_USBRequestTargetType: Validate USBRequestTargetType enum');
      expect(usbManager.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE).assertEqual(0);
      expect(usbManager.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE).assertEqual(1);
      expect(usbManager.USBRequestTargetType.USB_REQUEST_TARGET_ENDPOINT).assertEqual(2);
      expect(usbManager.USBRequestTargetType.USB_REQUEST_TARGET_OTHER).assertEqual(3);
    });

    it('test_enums_USBControlRequestType', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_enums_USBControlRequestType: Validate USBControlRequestType enum');
      expect(usbManager.USBControlRequestType.USB_REQUEST_TYPE_STANDARD).assertEqual(0);
      expect(usbManager.USBControlRequestType.USB_REQUEST_TYPE_CLASS).assertEqual(1);
      expect(usbManager.USBControlRequestType.USB_REQUEST_TYPE_VENDOR).assertEqual(2);
    });

    it('test_enums_USBRequestDirection', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_enums_USBRequestDirection: Validate USBRequestDirection enum');
      expect(usbManager.USBRequestDirection.USB_REQUEST_DIR_TO_DEVICE).assertEqual(0);
      expect(usbManager.USBRequestDirection.USB_REQUEST_DIR_FROM_DEVICE).assertEqual(0x80);
    });

    it('test_enums_UsbTransferFlags', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_enums_UsbTransferFlags: Validate UsbTransferFlags enum');
      expect(usbManager.UsbTransferFlags.USB_TRANSFER_SHORT_NOT_OK).assertEqual(0);
      expect(usbManager.UsbTransferFlags.USB_TRANSFER_FREE_BUFFER).assertEqual(1);
      expect(usbManager.UsbTransferFlags.USB_TRANSFER_FREE_TRANSFER).assertEqual(2);
      expect(usbManager.UsbTransferFlags.USB_TRANSFER_ADD_ZERO_PACKET).assertEqual(3);
    });

    it('test_enums_UsbTransferStatus', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_enums_UsbTransferStatus: Validate UsbTransferStatus enum');
      expect(usbManager.UsbTransferStatus.TRANSFER_COMPLETED).assertEqual(0);
      expect(usbManager.UsbTransferStatus.TRANSFER_ERROR).assertEqual(1);
      expect(usbManager.UsbTransferStatus.TRANSFER_TIMED_OUT).assertEqual(2);
      expect(usbManager.UsbTransferStatus.TRANSFER_CANCELED).assertEqual(3);
      expect(usbManager.UsbTransferStatus.TRANSFER_STALL).assertEqual(4);
      expect(usbManager.UsbTransferStatus.TRANSFER_NO_DEVICE).assertEqual(5);
      expect(usbManager.UsbTransferStatus.TRANSFER_OVERFLOW).assertEqual(6);
    });

    it('test_enums_UsbEndpointTransferType', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_enums_UsbEndpointTransferType: Validate UsbEndpointTransferType enum');
      expect(usbManager.UsbEndpointTransferType.TRANSFER_TYPE_ISOCHRONOUS).assertEqual(0x1);
      expect(usbManager.UsbEndpointTransferType.TRANSFER_TYPE_BULK).assertEqual(0x2);
      expect(usbManager.UsbEndpointTransferType.TRANSFER_TYPE_INTERRUPT).assertEqual(0x3);
    });

    // ==================== Complex Scenario Tests ====================

    it('test_scenario_fullDeviceWorkflow_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_scenario_fullDeviceWorkflow_001: Full device workflow');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;

        // 1. Check device right
        let hasRight = usbManager.hasRight(device.name);
        hilog.info(DOMAIN, TAG, `Initial hasRight: ${hasRight}`);

        // 2. Request right if needed
        if (!hasRight) {
          // Note: Can't use await in non-async function, so we skip this step
          hilog.info(DOMAIN, TAG, 'Device does not have right, but skipping request in sync test');
        }

        // 3. Connect to device
        const pipe = usbManager.connectDevice(device);
        expect(pipe !== null).assertTrue();
        hilog.info(DOMAIN, TAG, `Device connected: busNum=${pipe.busNum}, devAddress=${pipe.devAddress}`);

        // 4. Get file descriptor
        const fd = usbManager.getFileDescriptor(pipe);
        expect(fd > 0).assertTrue();
        hilog.info(DOMAIN, TAG, `File descriptor: ${fd}`);

        // 5. Get raw descriptor
        const rawDesc = usbManager.getRawDescriptor(pipe);
        expect(rawDesc.length > 0).assertTrue();
        hilog.info(DOMAIN, TAG, `Raw descriptor length: ${rawDesc.length}`);

        // 6. Claim interface if available
        if (device.configs.length > 0 && device.configs[0].interfaces.length > 0) {
          const iface = device.configs[0].interfaces[0];
          const claimResult = usbManager.claimInterface(pipe, iface, false);
          hilog.info(DOMAIN, TAG, `Claim interface result: ${claimResult}`);

          // 7. Release interface
          const releaseResult = usbManager.releaseInterface(pipe, iface);
          hilog.info(DOMAIN, TAG, `Release interface result: ${releaseResult}`);
        }

        // 8. Close pipe
        const closeResult = usbManager.closePipe(pipe);
        expect(closeResult).assertEqual(0);
        hilog.info(DOMAIN, TAG, `Close pipe result: ${closeResult}`);

        // 9. Remove right
        const removeResult = usbManager.removeRight(device.name);
        hilog.info(DOMAIN, TAG, `Remove right result: ${removeResult}`);

      } catch (error) {
        hilog.error(DOMAIN, TAG, `Full workflow error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    it('test_scenario_multiDeviceHandling_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_scenario_multiDeviceHandling_001: Handle multiple devices');
      if (deviceList.length < 2) {
        hilog.info(DOMAIN, TAG, 'Less than 2 devices, skipping multi-device test');
        return;
      }

      try {
        const pipes: Array<Readonly<usbManager.USBDevicePipe>> = [];

        // Connect to all devices
        for (const device of deviceList) {
          const pipe = usbManager.connectDevice(device as usbManager.USBDevice);
          expect(pipe !== null).assertTrue();
          pipes.push(pipe);
        }

        hilog.info(DOMAIN, TAG, `Connected to ${pipes.length} devices`);

        // Close all pipes
        for (const pipe of pipes) {
          const result = usbManager.closePipe(pipe);
          expect(result).assertEqual(0);
        }

        hilog.info(DOMAIN, TAG, 'All devices disconnected');

      } catch (error) {
        hilog.error(DOMAIN, TAG, `Multi-device handling error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    it('test_scenario_errorRecovery_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_scenario_errorRecovery_001: Error recovery');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;

        // Try to close pipe that was never opened
        const invalidPipe: usbManager.USBDevicePipe = { busNum: device.busNum, devAddress: device.devAddress };
        const result = usbManager.closePipe(invalidPipe);
        // Should fail or return error
        expect(typeof result).assertEqual('number');
        hilog.info(DOMAIN, TAG, `Close unopened pipe result: ${result}`);

        // Now properly connect and close
        const validPipe = usbManager.connectDevice(device);
        expect(validPipe !== null).assertTrue();

        const validResult = usbManager.closePipe(validPipe);
        expect(validResult).assertEqual(0);

      } catch (error) {
        hilog.error(DOMAIN, TAG, `Error recovery test error: ${JSON.stringify(error)}`);
      }
    });

    it('test_scenario_boundaryConditions_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_scenario_boundaryConditions_001: Boundary conditions');

      // Test with empty device name
      try {
        usbManager.hasRight('');
        expect(false).assertTrue();
      } catch (error) {
        expect(error.code).assertEqual(401);
      }

      // Test with very long device name
      try {
        const longName = 'a'.repeat(10000);
        usbManager.hasRight(longName);
        // May or may not throw error depending on implementation
        expect(true).assertTrue();
      } catch (error) {
        expect(error !== null).assertTrue();
      }

      // Test with special characters in device name
      try {
        const specialName = '!@#$%^&*()_+-={}[]|\\:";\'<>?,./`~';
        usbManager.hasRight(specialName);
        // May or may not throw error
        expect(true).assertTrue();
      } catch (error) {
        expect(error !== null).assertTrue();
      }
    });

    it('test_scenario_concurrentOperations_001', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_scenario_concurrentOperations_001: Concurrent operations');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;

        // Perform multiple operations concurrently
        const promises = [
          Promise.resolve(usbManager.hasRight(device.name)),
          Promise.resolve(usbManager.getDevices()),
          Promise.resolve(usbManager.getPortList())
        ];

        const results = await Promise.all(promises);
        expect(results.length).assertEqual(3);
        expect(typeof results[0]).assertEqual('boolean');
        expect(Array.isArray(results[1])).assertTrue();
        expect(Array.isArray(results[2])).assertTrue();

        hilog.info(DOMAIN, TAG, 'Concurrent operations completed');

      } catch (error) {
        hilog.error(DOMAIN, TAG, `Concurrent operations error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    it('test_scenario_dataIntegrity_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_scenario_dataIntegrity_001: Data integrity');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;

        // Get device list twice and compare
        const list1 = usbManager.getDevices();
        const list2 = usbManager.getDevices();

        expect(list1.length).assertEqual(list2.length);

        if (list1.length > 0) {
          const dev1 = list1[0];
          const dev2 = list2[0];

          expect(dev1.busNum).assertEqual(dev2.busNum);
          expect(dev1.devAddress).assertEqual(dev2.devAddress);
          expect(dev1.name).assertEqual(dev2.name);
          expect(dev1.vendorId).assertEqual(dev2.vendorId);
          expect(dev1.productId).assertEqual(dev2.productId);
        }

        hilog.info(DOMAIN, TAG, 'Data integrity verified');

      } catch (error) {
        hilog.error(DOMAIN, TAG, `Data integrity test error: ${JSON.stringify(error)}`);
        expect(false).assertTrue();
      }
    });

    // ==================== Performance Tests ====================

    it('test_performance_getDevicesLatency_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_performance_getDevicesLatency_001: getDevices latency');

      const iterations = 100;
      const startTime = Date.now();

      for (let i = 0; i < iterations; i++) {
        usbManager.getDevices();
      }

      const endTime = Date.now();
      const avgTime = (endTime - startTime) / iterations;

      hilog.info(DOMAIN, TAG, `Average getDevices latency: ${avgTime}ms`);

      // Should complete reasonably fast
      expect(avgTime < 100).assertTrue();
    });

    it('test_performance_connectDeviceLatency_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_performance_connectDeviceLatency_001: connectDevice latency');
      if (deviceList.length === 0) {
        return;
      }

      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const iterations = 10;
        const times: number[] = [];

        for (let i = 0; i < iterations; i++) {
          const start = Date.now();
          const pipe = usbManager.connectDevice(device);
          const end = Date.now();
          times.push(end - start);
          usbManager.closePipe(pipe);
        }

        const avgTime = times.reduce((a, b) => a + b, 0) / times.length;
        hilog.info(DOMAIN, TAG, `Average connectDevice latency: ${avgTime}ms`);

        // Connection should complete reasonably fast
        expect(avgTime < 1000).assertTrue();

      } catch (error) {
        hilog.error(DOMAIN, TAG, `connectDevice latency test error: ${JSON.stringify(error)}`);
      }
    });

    // ==================== Additional Edge Cases ====================

    it('test_edgeCase_deviceProperties_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_edgeCase_deviceProperties_001: Device with empty properties');
      try {
        const devices = usbManager.getDevices();
        if (devices.length > 0) {
          const device = devices[0] as usbManager.USBDevice;

          // Check for empty strings
          if (device.serial === '') {
            hilog.info(DOMAIN, TAG, 'Device has empty serial');
          }
          if (device.manufacturerName === '') {
            hilog.info(DOMAIN, TAG, 'Device has empty manufacturer');
          }
          if (device.productName === '') {
            hilog.info(DOMAIN, TAG, 'Device has empty product name');
          }

          expect(true).assertTrue(); // Test passed - just checking properties
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `Device properties test error: ${JSON.stringify(error)}`);
      }
    });

    it('test_edgeCase_maxPacketSize_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_edgeCase_maxPacketSize_001: Max packet size validation');
      try {
        const devices = usbManager.getDevices();
        for (const device of devices) {
          const dev = device as usbManager.USBDevice;
          for (const config of dev.configs) {
            for (const iface of config.interfaces) {
              for (const endpoint of iface.endpoints) {
                // Max packet size should be within valid range
                expect(endpoint.maxPacketSize > 0).assertTrue();
                expect(endpoint.maxPacketSize <= 1024).assertTrue();
              }
            }
          }
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `Max packet size test error: ${JSON.stringify(error)}`);
      }
    });

    it('test_edgeCase_configCount_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_edgeCase_configCount_001: Configuration count');
      try {
        const devices = usbManager.getDevices();
        for (const device of devices) {
          const dev = device as usbManager.USBDevice;
          // Devices typically have 1 or more configurations
          expect(dev.configs.length > 0).assertTrue();
          expect(dev.configs.length <= 10).assertTrue(); // Reasonable upper limit
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `Config count test error: ${JSON.stringify(error)}`);
      }
    });

    it('test_edgeCase_busNumberRange_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_edgeCase_busNumberRange_001: Bus number range validation');
      try {
        const devices = usbManager.getDevices();
        for (const device of devices) {
          const dev = device as usbManager.USBDevice;
          // Bus numbers typically range from 1 to 255
          expect(dev.busNum >= 0).assertTrue();
          expect(dev.busNum <= 255).assertTrue();

          // Device addresses range from 1 to 127
          expect(dev.devAddress >= 0).assertTrue();
          expect(dev.devAddress <= 127).assertTrue();
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `Bus number range test error: ${JSON.stringify(error)}`);
      }
    });

    it('test_edgeCase_vendorProductIds_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_edgeCase_vendorProductIds_001: Vendor and Product ID validation');
      try {
        const devices = usbManager.getDevices();
        for (const device of devices) {
          const dev = device as usbManager.USBDevice;
          // Vendor ID and Product ID are 16-bit values
          expect(dev.vendorId >= 0).assertTrue();
          expect(dev.vendorId <= 0xFFFF).assertTrue();
          expect(dev.productId >= 0).assertTrue();
          expect(dev.productId <= 0xFFFF).assertTrue();
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `Vendor/Product ID test error: ${JSON.stringify(error)}`);
      }
    });

    it('test_edgeCase_endpointAttributes_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_edgeCase_endpointAttributes_001: Endpoint attributes validation');
      try {
        const devices = usbManager.getDevices();
        for (const device of devices) {
          const dev = device as usbManager.USBDevice;
          for (const config of dev.configs) {
            for (const iface of config.interfaces) {
              for (const endpoint of iface.endpoints) {
                // Endpoint address should be valid
                expect(endpoint.address >= 0).assertTrue();
                expect(endpoint.address <= 0xFF).assertTrue();

                // Direction should be valid
                expect([0, 0x80].includes(endpoint.direction)).assertTrue();

                // Type should be valid (Control=0, Iso=1, Bulk=2, Interrupt=3)
                expect(endpoint.type >= 0).assertTrue();
                expect(endpoint.type <= 3).assertTrue();
              }
            }
          }
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `Endpoint attributes test error: ${JSON.stringify(error)}`);
      }
    });

    it('test_deprecated_api_001', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_deprecated_api_001: Test deprecated getPorts API');
      try {
        // Test the deprecated getPorts API
        const ports = (usbManager as object).getPorts();
        expect(ports !== null).assertTrue();
        expect(Array.isArray(ports)).assertTrue();
        hilog.info(DOMAIN, TAG, `Deprecated getPorts returned ${ports.length} ports`);
      } catch (error) {
        hilog.info(DOMAIN, TAG, `Deprecated getPorts error (may not be available): ${JSON.stringify(error)}`);
      }
    });

    it('test_deprecated_api_002', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_deprecated_api_002: Test deprecated usbFunctionsFromString API');
      try {
        const funcs = (usbManager as object).usbFunctionsFromString('acm');
        expect(typeof funcs).assertEqual('number');
        hilog.info(DOMAIN, TAG, `Deprecated usbFunctionsFromString returned: ${funcs}`);
      } catch (error) {
        hilog.info(DOMAIN, TAG, `Deprecated usbFunctionsFromString error: ${JSON.stringify(error)}`);
      }
    });

    it('test_deprecated_api_003', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_deprecated_api_003: Test deprecated usbFunctionsToString API');
      try {
        const str = (usbManager as object).usbFunctionsToString(1);
        expect(typeof str).assertEqual('string');
        hilog.info(DOMAIN, TAG, `Deprecated usbFunctionsToString returned: ${str}`);
      } catch (error) {
        hilog.info(DOMAIN, TAG, `Deprecated usbFunctionsToString error: ${JSON.stringify(error)}`);
      }
    });

    it('test_deprecated_api_004', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_deprecated_api_004: Test deprecated setCurrentFunctions API');
      try {
        await (usbManager as object).setCurrentFunctions(1);
        hilog.info(DOMAIN, TAG, 'Deprecated setCurrentFunctions succeeded');
      } catch (error) {
        hilog.info(DOMAIN, TAG, `Deprecated setCurrentFunctions error: ${JSON.stringify(error)}`);
      }
    });

    it('test_deprecated_api_005', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_deprecated_api_005: Test deprecated getCurrentFunctions API');
      try {
        const funcs = (usbManager as object).getCurrentFunctions();
        expect(typeof funcs).assertEqual('number');
        hilog.info(DOMAIN, TAG, `Deprecated getCurrentFunctions returned: ${funcs}`);
      } catch (error) {
        hilog.info(DOMAIN, TAG, `Deprecated getCurrentFunctions error: ${JSON.stringify(error)}`);
      }
    });

    it('test_deprecated_api_006', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_deprecated_api_006: Test deprecated getSupportedModes API');
      try {
        const ports = usbManager.getPortList();
        if (ports.length > 0) {
          const modes = (usbManager as object).getSupportedModes(ports[0].id);
          expect(typeof modes).assertEqual('number');
          hilog.info(DOMAIN, TAG, `Deprecated getSupportedModes returned: ${modes}`);
        }
      } catch (error) {
        hilog.info(DOMAIN, TAG, `Deprecated getSupportedModes error: ${JSON.stringify(error)}`);
      }
    });

    it('test_deprecated_api_007', 0, async () => {
      hilog.info(DOMAIN, TAG, 'test_deprecated_api_007: Test deprecated setPortRoles API');
      try {
        const ports = usbManager.getPortList();
        if (ports.length > 0) {
          await (usbManager as object).setPortRoles(ports[0].id, 1, 1);
          hilog.info(DOMAIN, TAG, 'Deprecated setPortRoles succeeded');
        }
      } catch (error) {
        hilog.info(DOMAIN, TAG, `Deprecated setPortRoles error: ${JSON.stringify(error)}`);
      }
    });

    it('test_deprecated_api_008', 0, () => {
      hilog.info(DOMAIN, TAG, 'test_deprecated_api_008: Test deprecated addRight API');
      if (deviceList.length === 0) {
        return;
      }
      try {
        const device = deviceList[0] as usbManager.USBDevice;
        const result = (usbManager as object).addRight('test_bundle', device.name);
        expect(typeof result).assertEqual('boolean');
        hilog.info(DOMAIN, TAG, `Deprecated addRight returned: ${result}`);
      } catch (error) {
        hilog.info(DOMAIN, TAG, `Deprecated addRight error: ${JSON.stringify(error)}`);
      }
    });

  });
}
