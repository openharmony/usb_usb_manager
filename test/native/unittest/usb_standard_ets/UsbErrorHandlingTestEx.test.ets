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

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium'
import usbMgr from '@ohos.usbManager';
import { UiDriver, BY } from '@ohos.UiTest';
import { BusinessError } from '@ohos.base';

const TAG: string = "[UsbErrorHandlingTestEx]";
let gDeviceList: Array<usbMgr.USBDevice>;
let gPipe: usbMgr.USBDevicePipe | null;
let devices: usbMgr.USBDevice;
let isDeviceConnected: boolean;

function sleep(ms: number): Promise<void> {
  return new Promise<void>(resolve => setTimeout(resolve, ms));
}

async function getPermission(gDeviceList: Array<usbMgr.USBDevice>) {
  console.info('**************getPermission**************');
  try {
    usbMgr.requestRight(gDeviceList[0].name).then(hasRight => {
      console.info(TAG, `usb requestRight success, hasRight: ${hasRight}`);
    })
  } catch (err) {
    console.info(TAG, `usb getPermission to requestRight hasRight fail: `, err);
    return
  }
}

async function driveFn() {
  console.info('**************driveFn**************');
  try {
    let driver = await UiDriver.create();
    console.info(TAG, ` come in driveFn`);
    console.info(TAG, `driver is ${JSON.stringify(driver)}`);
    await sleep(1000);
    let button = await driver.findComponent(BY.text('允许'));
    console.info(TAG, `button is ${JSON.stringify(button)}`);
    await sleep(1000);
    await button.click();
  } catch (err) {
    console.info(TAG, 'err is ' + err);
    return;
  }
}

function deviceConnected(gDeviceList: Array<usbMgr.USBDevice>) {
  if (gDeviceList.length > 0) {
    console.info(TAG, "Test USB device is connected");
    return true;
  }
  console.info(TAG, "Test USB device is not connected");
  return false;
}

export default function UsbErrorHandlingTestEx() {
  describe('UsbErrorHandlingTestEx', () => {
    let isDeviceConnected: boolean = false;
    let devices: usbMgr.USBDevice;

    beforeAll(async (done: Function) => {
      console.log(TAG, '*************Usb Unit UsbErrorHandlingTestEx Begin*************');
      let gDeviceList: Array<usbMgr.USBDevice> = usbMgr.getDevices();
      console.info(TAG, 'usb unit begin test getDevices **********', JSON.stringify(gDeviceList));
      isDeviceConnected = deviceConnected(gDeviceList);
      console.info(TAG, 'isDeviceConnected:', isDeviceConnected);
      if (isDeviceConnected) {
        devices = gDeviceList[0];
        let hasRight: boolean = usbMgr.hasRight(devices.name);
        if (!hasRight) {
          console.info(TAG, `beforeAll: usb requestRight start`);
          await getPermission(gDeviceList);
          await sleep(3000);
          await driveFn();
          await sleep(1000);
        }
      }
      done();
    })

    afterAll(() => {
      if (isDeviceConnected) {
        usbMgr.removeRight(devices.name);
      }
      console.log('*************Usb Unit UsbErrorHandlingTestEx End*************');
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test Case*************');
    })

    it('testSpecialCharacterDeviceName001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testSpecialCharacterDeviceName001 enter');
      
      const specialNames = [
        'device/name',
        'device\\name',
        'device@name',
        'device#name',
        'device$name',
        'device%name',
        'device^name',
        'device&name',
        'device*name',
        'device(name)',
        'device[name]',
        'device{name}',
        'device|name',
        'device;name',
        'device:name',
        'device\'name',
        'device"name',
        'device<name>',
        'device,name',
        'device.name',
        'device-name',
        'device+name',
        'device=name',
        'device!name',
        'device~name',
        'device`name',
        'device\tname',
        'device\nname',
        'device\rname',
      ];
      
      for (const name of specialNames) {
        try {
          let hasRight = usbMgr.hasRight(name);
          console.info(TAG, `hasRight for "${name}": ${hasRight}`);
          expect(hasRight).assertFalse();
        } catch (error) {
          console.info(TAG, `Expected error for name "${name}":`, error);
        }
      }
    });

    it('testNumericStringDeviceName001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNumericStringDeviceName001 enter');
      
      const numericNames = [
        '0',
        '1',
        '123',
        '999999999',
        '-1',
        '-123',
        '1.23',
        '0.0',
        '1e10',
        'NaN',
        'Infinity',
        '-Infinity',
      ];
      
      for (const name of numericNames) {
        try {
          let hasRight = usbMgr.hasRight(name);
          console.info(TAG, `hasRight for "${name}": ${hasRight}`);
          expect(hasRight).assertFalse();
        } catch (error) {
          console.info(TAG, `Expected error for name "${name}":`, error);
        }
      }
    });

    it('testUnicodeDeviceName001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testUnicodeDeviceName001 enter');
      
      const unicodeNames = [
        '设备名',
        'デバイス名',
        '장치이름',
        'устройство',
        'dispositivo',
        'Gerät',
        'périphérique',
        'αποθήκη',
        'डिवाइस',
        'الجهاز',
        '📱设备',
        'Device😀',
        '设备🔌',
      ];
      
      for (const name of unicodeNames) {
        try {
          let hasRight = usbMgr.hasRight(name);
          console.info(TAG, `hasRight for "${name}": ${hasRight}`);
          expect(hasRight).assertFalse();
        } catch (error) {
          console.info(TAG, `Expected error for name "${name}":`, error);
        }
      }
    });

    it('testVeryLongDeviceName001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testVeryLongDeviceName001 enter');
      
      const longNames = [
        'a'.repeat(100),
        'a'.repeat(256),
        'a'.repeat(512),
        'a'.repeat(1024),
        'a'.repeat(4096),
        'a'.repeat(8192),
        'a'.repeat(16384),
      ];
      
      for (const name of longNames) {
        try {
          let hasRight = usbMgr.hasRight(name);
          console.info(TAG, `hasRight for length ${name.length}: ${hasRight}`);
          expect(hasRight).assertFalse();
        } catch (error) {
          console.info(TAG, `Expected error for length ${name.length}:`, error);
        }
      }
    });

    it('testInvalidBusNumValues001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInvalidBusNumValues001 enter');
      
      const invalidBusNums = [
        -999, -100, -10, -2, -1,
        256, 512, 1024, 65535, 999999,
        Number.MIN_SAFE_INTEGER,
        Number.MAX_SAFE_INTEGER,
        Number.NaN,
        Number.POSITIVE_INFINITY,
        Number.NEGATIVE_INFINITY,
      ];
      
      for (const busNum of invalidBusNums) {
        try {
          let pipe = { busNum: busNum, devAddress: 1 };
          let result = usbMgr.closePipe(pipe);
          console.info(TAG, `closePipe for busNum ${busNum}: ${result}`);
          expect(result !== 0).assertTrue();
        } catch (error) {
          console.info(TAG, `Expected error for busNum ${busNum}:`, error);
        }
      }
    });

    it('testInvalidDevAddressValues001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInvalidDevAddressValues001 enter');
      
      const invalidDevAddresses = [
        -999, -100, -10, -2, -1,
        128, 129, 255, 256, 512, 1024,
        Number.MIN_SAFE_INTEGER,
        Number.MAX_SAFE_INTEGER,
        Number.NaN,
        Number.POSITIVE_INFINITY,
        Number.NEGATIVE_INFINITY,
      ];
      
      for (const devAddress of invalidDevAddresses) {
        try {
          let pipe = { busNum: 1, devAddress: devAddress };
          let result = usbMgr.closePipe(pipe);
          console.info(TAG, `closePipe for devAddress ${devAddress}: ${result}`);
          expect(result !== 0).assertTrue();
        } catch (error) {
          console.info(TAG, `Expected error for devAddress ${devAddress}:`, error);
        }
      }
    });

    it('testInterfaceIdBoundaryValues001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInterfaceIdBoundaryValues001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      
      const interfaceIds = [
        -1, 0, 1, 127, 128, 255, 256, 512, 1024,
      ];
      
      for (const id of interfaceIds) {
        try {
          let iface = { 
            id: id, 
            clazz: 0, 
            subClass: 0, 
            protocol: 0, 
            name: '', 
            interfaces: [], 
            endpoints: [] 
          };
          let result = usbMgr.claimInterface(gPipe, iface, true);
          console.info(TAG, `claimInterface for id ${id}: ${result}`);
          expect(result !== 0).assertTrue();
        } catch (error) {
          console.info(TAG, `Expected error for interface id ${id}:`, error);
        }
      }
      
      usbMgr.closePipe(gPipe);
    });

    it('testConfigIdBoundaryValues001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testConfigIdBoundaryValues001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      
      const configIds = [
        -1, 0, 1, 127, 128, 255, 256, 512, 1024,
      ];
      
      for (const id of configIds) {
        try {
          let config = { 
            id: id, 
            name: '', 
            maxPower: 0, 
            isRemoteWakeup: false, 
            isSelfPowered: false, 
            interfaces: [] 
          };
          let result = usbMgr.setConfiguration(gPipe, config);
          console.info(TAG, `setConfiguration for id ${id}: ${result}`);
          expect(result !== 0).assertTrue();
        } catch (error) {
          console.info(TAG, `Expected error for config id ${id}:`, error);
        }
      }
      
      usbMgr.closePipe(gPipe);
    });

    it('testEndpointAddressBoundaryValues001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testEndpointAddressBoundaryValues001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
      usbMgr.claimInterface(gPipe, tmpInterface, true);
      
      const endpointAddresses = [
        -1, 0, 1, 127, 128, 255, 256, 512,
      ];
      
      for (const address of endpointAddresses) {
        try {
          let endpoint = { 
            address: address, 
            type: 0, 
            direction: 0, 
            maxPacketSize: 0, 
            interval: 0 
          };
          let buffer = new Uint8Array(64);
          let result = await usbMgr.bulkTransfer(gPipe, endpoint, buffer, 5000);
          console.info(TAG, `bulkTransfer for endpoint address ${address}: ${result}`);
          expect(result < 0).assertTrue();
        } catch (error) {
          console.info(TAG, `Expected error for endpoint address ${address}:`, error);
        }
      }
      
      usbMgr.releaseInterface(gPipe, tmpInterface);
      usbMgr.closePipe(gPipe);
    });

    it('testControlRequestBoundaryValues001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testControlRequestBoundaryValues001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      
      const requestValues = [
        -1, 0, 1, 127, 128, 255, 256, 512, 1024, 65535,
      ];
      
      for (const request of requestValues) {
        try {
          let controlParam = {
            request: request,
            target: 0,
            reqType: 0,
            value: 0,
            index: 0,
            data: new Uint8Array(64)
          };
          let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
          console.info(TAG, `controlTransfer for request ${request}: ${result}`);
        } catch (error) {
          console.info(TAG, `Expected error for request ${request}:`, error);
        }
      }
      
      usbMgr.closePipe(gPipe);
    });

    it('testControlValueBoundaryValues001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testControlValueBoundaryValues001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      
      const valueValues = [
        -1, 0, 1, 127, 128, 255, 256, 512, 1024, 65535,
      ];
      
      for (const value of valueValues) {
        try {
          let controlParam = {
            request: 0,
            target: 0,
            reqType: 0,
            value: value,
            index: 0,
            data: new Uint8Array(64)
          };
          let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
          console.info(TAG, `controlTransfer for value ${value}: ${result}`);
        } catch (error) {
          console.info(TAG, `Expected error for value ${value}:`, error);
        }
      }
      
      usbMgr.closePipe(gPipe);
    });

    it('testControlIndexBoundaryValues001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testControlIndexBoundaryValues001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      
      const indexValues = [
        -1, 0, 1, 127, 128, 255, 256, 512, 1024, 65535,
      ];
      
      for (const index of indexValues) {
        try {
          let controlParam = {
            request: 0,
            target: 0,
            reqType: 0,
            value: 0,
            index: index,
            data: new Uint8Array(64)
          };
          let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
          console.info(TAG, `controlTransfer for index ${index}: ${result}`);
        } catch (error) {
          console.info(TAG, `Expected error for index ${index}:`, error);
        }
      }
      
      usbMgr.closePipe(gPipe);
    });

    it('testControlDataSizeBoundaryValues001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testControlDataSizeBoundaryValues001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      
      const dataSizes = [
        0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384,
      ];
      
      for (const size of dataSizes) {
        try {
          let controlParam = {
            request: 0,
            target: 0,
            reqType: 0,
            value: 0,
            index: 0,
            data: new Uint8Array(size)
          };
          let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
          console.info(TAG, `controlTransfer for data size ${size}: ${result}`);
        } catch (error) {
          console.info(TAG, `Expected error for data size ${size}:`, error);
        }
      }
      
      usbMgr.closePipe(gPipe);
    });

    it('testNullBufferTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testNullBufferTransfer001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
      usbMgr.claimInterface(gPipe, tmpInterface, true);
      
      try {
        let endpoint = tmpInterface.endpoints[0];
        let result = await usbMgr.bulkTransfer(gPipe, endpoint, null, 5000);
        console.info(TAG, `bulkTransfer with null buffer: ${result}`);
        expect(result < 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'Expected error for null buffer:', error);
      }
      
      usbMgr.releaseInterface(gPipe, tmpInterface);
      usbMgr.closePipe(gPipe);
    });

    it('testUndefinedBufferTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testUndefinedBufferTransfer001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
      usbMgr.claimInterface(gPipe, tmpInterface, true);
      
      try {
        let endpoint = tmpInterface.endpoints[0];
        let result = await usbMgr.bulkTransfer(gPipe, endpoint, undefined, 5000);
        console.info(TAG, `bulkTransfer with undefined buffer: ${result}`);
        expect(result < 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'Expected error for undefined buffer:', error);
      }
      
      usbMgr.releaseInterface(gPipe, tmpInterface);
      usbMgr.closePipe(gPipe);
    });

    it('testNegativeTimeoutTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testNegativeTimeoutTransfer001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
      usbMgr.claimInterface(gPipe, tmpInterface, true);
      
      try {
        let endpoint = tmpInterface.endpoints[0];
        let buffer = new Uint8Array(64);
        let result = await usbMgr.bulkTransfer(gPipe, endpoint, buffer, -1);
        console.info(TAG, `bulkTransfer with negative timeout: ${result}`);
      } catch (error) {
        console.info(TAG, 'Expected error for negative timeout:', error);
      }
      
      usbMgr.releaseInterface(gPipe, tmpInterface);
      usbMgr.closePipe(gPipe);
    });

    it('testVeryLargeTimeoutTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testVeryLargeTimeoutTransfer001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
      usbMgr.claimInterface(gPipe, tmpInterface, true);
      
      try {
        let endpoint = tmpInterface.endpoints[0];
        let buffer = new Uint8Array(64);
        let result = await usbMgr.bulkTransfer(gPipe, endpoint, buffer, 2147483647);
        console.info(TAG, `bulkTransfer with large timeout: ${result}`);
      } catch (error) {
        console.info(TAG, 'Expected error for large timeout:', error);
      }
      
      usbMgr.releaseInterface(gPipe, tmpInterface);
      usbMgr.closePipe(gPipe);
    });

    it('testNullPipeOperations001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNullPipeOperations001 enter');
      
      try {
        let result = usbMgr.closePipe(null);
        console.info(TAG, `closePipe with null pipe: ${result}`);
        expect(result !== 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'Expected error for null pipe:', error);
      }
      
      try {
        let descriptor = usbMgr.getRawDescriptor(null);
        console.info(TAG, `getRawDescriptor with null pipe: ${descriptor.length}`);
        expect(descriptor.length).assertEqual(0);
      } catch (error) {
        console.info(TAG, 'Expected error for null pipe:', error);
      }
      
      try {
        let fd = usbMgr.getFileDescriptor(null);
        console.info(TAG, `getFileDescriptor with null pipe: ${fd}`);
        expect(fd).assertEqual(-1);
      } catch (error) {
        console.info(TAG, 'Expected error for null pipe:', error);
      }
    });

    it('testUndefinedPipeOperations001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testUndefinedPipeOperations001 enter');
      
      try {
        let result = usbMgr.closePipe(undefined);
        console.info(TAG, `closePipe with undefined pipe: ${result}`);
        expect(result !== 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'Expected error for undefined pipe:', error);
      }
      
      try {
        let descriptor = usbMgr.getRawDescriptor(undefined);
        console.info(TAG, `getRawDescriptor with undefined pipe: ${descriptor.length}`);
        expect(descriptor.length).assertEqual(0);
      } catch (error) {
        console.info(TAG, 'Expected error for undefined pipe:', error);
      }
      
      try {
        let fd = usbMgr.getFileDescriptor(undefined);
        console.log(TAG, `getFileDescriptor with undefined pipe: ${fd}`);
        expect(fd).assertEqual(-1);
      } catch (error) {
        console.info(TAG, 'Expected error for undefined pipe:', error);
      }
    });

    it('testInvalidPipeStructure001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInvalidPipeStructure001 enter');
      
      const invalidPipes = [
        {},
        { busNum: null, devAddress: null },
        { busNum: undefined, devAddress: undefined },
        { busNum: 'string', devAddress: 'string' },
        { busNum: 1.5, devAddress: 2.5 },
        { busNum: true, devAddress: false },
        { busNum: [], devAddress: [] },
        { busNum: {}, devAddress: {} },
        { busNum: NaN, devAddress: NaN },
        { busNum: Infinity, devAddress: Infinity },
      ];
      
      for (const pipe of invalidPipes) {
        try {
          let result = usbMgr.closePipe(pipe);
          console.info(TAG, `closePipe for invalid pipe ${JSON.stringify(pipe)}: ${result}`);
          expect(result !== 0).assertTrue();
        } catch (error) {
          console.info(TAG, `Expected error for invalid pipe:`, error);
        }
      }
    });

    it('testEmptyStringDeviceName001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testEmptyStringDeviceName001 enter');
      
      const emptyStrings = [
        '',
        ' ',
        '  ',
        '   ',
        '\t',
        '\n',
        '\r',
        '\r\n',
      ];
      
      for (const name of emptyStrings) {
        try {
          let hasRight = usbMgr.hasRight(name);
          console.info(TAG, `hasRight for empty string "${name}": ${hasRight}`);
          expect(hasRight).assertFalse();
        } catch (error) {
          console.info(TAG, `Expected error for empty string:`, error);
        }
      }
    });

    it('testMultipleOperationsSameDevice001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testMultipleOperationsSameDevice001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let descriptor = usbMgr.getRawDescriptor(gPipe);
      expect(descriptor.length).assertLarger(0);
      
      let fd = usbMgr.getFileDescriptor(gPipe);
      expect(fd >= 0).assertTrue();
      
      let closeResult = usbMgr.closePipe(gPipe);
      expect(closeResult).assertEqual(0);
    });

    it('testRepeatedConnectDisconnect001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testRepeatedConnectDisconnect001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      const iterations = 5;
      
      for (let i = 0; i < iterations; i++) {
        try {
          let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
          console.info(TAG, `Iteration ${i}: connectDevice result: ${JSON.stringify(gPipe)}`);
          expect(gPipe !== null).assertTrue();
          
          let closeResult = usbMgr.closePipe(gPipe);
          console.info(TAG, `Iteration ${i}: closePipe result: ${closeResult}`);
          expect(closeResult).assertEqual(0);
        } catch (error) {
          console.info(TAG, `Iteration ${i} error:`, error);
        }
      }
    });

    it('testMixedValidInvalidOperations001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMixedValidInvalidOperations001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      try {
        let invalidPipe = { busNum: -1, devAddress: -1 };
        let result = usbMgr.closePipe(invalidPipe);
        expect(result !== 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'Expected error for invalid pipe:', error);
      }
      
      let validCloseResult = usbMgr.closePipe(gPipe);
      expect(validCloseResult).assertEqual(0);
    });
  })
}
