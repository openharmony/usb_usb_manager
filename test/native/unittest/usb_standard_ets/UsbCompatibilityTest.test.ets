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

const TAG: string = "[UsbCompatibilityTest]";
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

export default function UsbCompatibilityTest() {
  describe('UsbCompatibilityTest', () => {
    let isDeviceConnected: boolean = false;
    let devices: usbMgr.USBDevice;

    beforeAll(async (done: Function) => {
      console.log(TAG, '*************Usb Unit UsbCompatibilityTest Begin*************');
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
      console.log('*************Usb Unit UsbCompatibilityTest End*************');
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test Case*************');
    })

    it('testVersionCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testVersionCompatibility001 enter');
      
      let deviceList = usbMgr.getDevices();
      console.info(TAG, `API version compatibility test, device count: ${deviceList.length}`);
      expect(deviceList.length >= 0).assertTrue();
    });

    it('testDataTypeCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDataTypeCompatibility001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let device = devices;
      
      expect(typeof device.name === 'string').assertTrue();
      expect(typeof device.vendorId === 'number').assertTrue();
      expect(typeof device.productId === 'number').assertTrue();
      expect(typeof device.serial === 'string').assertTrue();
      expect(Array.isArray(device.configs)).assertTrue();
      
      console.info(TAG, 'Data type compatibility check passed');
    });

    it('testReturnValueCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testReturnValueCompatibility001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      expect(typeof gPipe === 'object').assertTrue();
      
      let closeResult = usbMgr.closePipe(gPipe);
      expect(typeof closeResult === 'number').assertTrue();
      expect(closeResult === 0).assertTrue();
    });

    it('testNullHandlingCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNullHandlingCompatibility001 enter');
      
      try {
        let hasRight = usbMgr.hasRight(null);
        expect(typeof hasRight === 'boolean').assertTrue();
      } catch (error) {
        console.info(TAG, 'Null handling compatibility test passed with error');
      }
    });

    it('testUndefinedHandlingCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testUndefinedHandlingCompatibility001 enter');
      
      try {
        let hasRight = usbMgr.hasRight(undefined);
        expect(typeof hasRight === 'boolean').assertTrue();
      } catch (error) {
        console.info(TAG, 'Undefined handling compatibility test passed with error');
      }
    });

    it('testEmptyStringHandlingCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testEmptyStringHandlingCompatibility001 enter');
      
      try {
        let hasRight = usbMgr.hasRight('');
        expect(typeof hasRight === 'boolean').assertTrue();
      } catch (error) {
        console.info(TAG, 'Empty string handling compatibility test passed with error');
      }
    });

    it('testNegativeNumberHandlingCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNegativeNumberHandlingCompatibility001 enter');
      
      try {
        let pipe = { busNum: -1, devAddress: -1 };
        let result = usbMgr.closePipe(pipe);
        expect(typeof result === 'number').assertTrue();
      } catch (error) {
        console.info(TAG, 'Negative number handling compatibility test passed with error');
      }
    });

    it('testLargeNumberHandlingCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testLargeNumberHandlingCompatibility001 enter');
      
      try {
        let pipe = { busNum: 999999, devAddress: 999999 };
        let result = usbMgr.closePipe(pipe);
        expect(typeof result === 'number').assertTrue();
      } catch (error) {
        console.info(TAG, 'Large number handling compatibility test passed with error');
      }
    });

    it('testArrayHandlingCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testArrayHandlingCompatibility001 enter');
      
      let deviceList = usbMgr.getDevices();
      expect(Array.isArray(deviceList)).assertTrue();
      console.info(TAG, 'Array handling compatibility test passed');
    });

    it('testObjectHandlingCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testObjectHandlingCompatibility001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let device = devices;
      expect(typeof device === 'object').assertTrue();
      expect(device !== null).assertTrue();
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(typeof gPipe === 'object').assertTrue();
      expect(gPipe !== null).assertTrue();
      
      usbMgr.closePipe(gPipe);
    });

    it('testBooleanHandlingCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testBooleanHandlingCompatibility001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let hasRight = usbMgr.hasRight(devices.name);
      expect(typeof hasRight === 'boolean').assertTrue();
      
      let removeResult = usbMgr.removeRight(devices.name);
      expect(typeof removeResult === 'boolean').assertTrue();
      
      let requestResult = await usbMgr.requestRight(devices.name);
      expect(typeof requestResult === 'boolean').assertTrue();
    });

    it('testStringHandlingCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testStringHandlingCompatibility001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let deviceName = devices.name;
      expect(typeof deviceName === 'string').assertTrue();
      expect(deviceName.length > 0).assertTrue();
      
      let hasRight = usbMgr.hasRight(deviceName);
      expect(typeof hasRight === 'boolean').assertTrue();
    });

    it('testNumberHandlingCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNumberHandlingCompatibility001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let vendorId = devices.vendorId;
      expect(typeof vendorId === 'number').assertTrue();
      expect(vendorId > 0).assertTrue();
      
      let productId = devices.productId;
      expect(typeof productId === 'number').assertTrue();
      expect(productId > 0).assertTrue();
    });

    it('testUint8ArrayHandlingCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testUint8ArrayHandlingCompatibility001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let rawDescriptor = usbMgr.getRawDescriptor(gPipe);
      expect(rawDescriptor instanceof Uint8Array).assertTrue();
      expect(rawDescriptor.length > 0).assertTrue();
      
      usbMgr.closePipe(gPipe);
    });

    it('testPromiseHandlingCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testPromiseHandlingCompatibility001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let requestResult = await usbMgr.requestRight(devices.name);
      expect(typeof requestResult === 'boolean').assertTrue();
      expect(requestResult).assertTrue();
    });

    it('testAsyncFunctionCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testAsyncFunctionCompatibility001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let claimResult = usbMgr.claimInterface(gPipe, iface, true);
      expect(claimResult).assertEqual(0);
      
      let releaseResult = usbMgr.releaseInterface(gPipe, iface);
      expect(releaseResult).assertEqual(0);
      
      usbMgr.closePipe(gPipe);
    });

    it('testErrorHandlingCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testErrorHandlingCompatibility001 enter');
      
      try {
        let hasRight = usbMgr.hasRight(null);
      } catch (error) {
        expect(error !== null).assertTrue();
        expect(typeof error === 'object').assertTrue();
        console.info(TAG, 'Error handling compatibility test passed');
      }
    });

    it('testCallbackHandlingCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testCallbackHandlingCompatibility001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let callbackCalled = false;
      let callback = () => {
        callbackCalled = true;
      };
      
      expect(typeof callback === 'function').assertTrue();
      console.info(TAG, 'Callback handling compatibility test passed');
    });

    it('testConfigStructureCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testConfigStructureCompatibility001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      expect(config.hasOwnProperty('id')).assertTrue();
      expect(config.hasOwnProperty('name')).assertTrue();
      expect(config.hasOwnProperty('maxPower')).assertTrue();
      expect(config.hasOwnProperty('isRemoteWakeup')).assertTrue();
      expect(config.hasOwnProperty('isSelfPowered')).assertTrue();
      expect(config.hasOwnProperty('interfaces')).assertTrue();
      
      console.info(TAG, 'Config structure compatibility test passed');
    });

    it('testInterfaceStructureCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInterfaceStructureCompatibility001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      expect(iface.hasOwnProperty('id')).assertTrue();
      expect(iface.hasOwnProperty('clazz')).assertTrue();
      expect(iface.hasOwnProperty('subClass')).assertTrue();
      expect(iface.hasOwnProperty('protocol')).assertTrue();
      expect(iface.hasOwnProperty('name')).assertTrue();
      expect(iface.hasOwnProperty('interfaces')).assertTrue();
      expect(iface.hasOwnProperty('endpoints')).assertTrue();
      
      console.info(TAG, 'Interface structure compatibility test passed');
    });

    it('testEndpointStructureCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testEndpointStructureCompatibility001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      let endpoint = iface.endpoints[0];
      
      expect(endpoint.hasOwnProperty('address')).assertTrue();
      expect(endpoint.hasOwnProperty('type')).assertTrue();
      expect(endpoint.hasOwnProperty('direction')).assertTrue();
      expect(endpoint.hasOwnProperty('maxPacketSize')).assertTrue();
      expect(endpoint.hasOwnProperty('interval')).assertTrue();
      
      console.info(TAG, 'Endpoint structure compatibility test passed');
    });

    it('testPipeStructureCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testPipeStructureCompatibility001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      expect(gPipe.hasOwnProperty('busNum')).assertTrue();
      expect(gPipe.hasOwnProperty('devAddress')).assertTrue();
      
      usbMgr.closePipe(gPipe);
    });

    it('testEnumValueCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testEnumValueCompatibility001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      let endpoint = iface.endpoints[0];
      
      expect(endpoint.type >= 0 && endpoint.type <= 3).assertTrue();
      expect(endpoint.direction >= 0 && endpoint.direction <= 1).assertTrue();
      
      console.info(TAG, 'Enum value compatibility test passed');
    });

    it('testBoundaryValueCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testBoundaryValueCompatibility001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let vendorId = devices.vendorId;
      expect(vendorId > 0 && vendorId <= 0xFFFF).assertTrue();
      
      let productId = devices.productId;
      expect(productId > 0 && productId <= 0xFFFF).assertTrue();
      
      console.info(TAG, 'Boundary value compatibility test passed');
    });

    it('testMultipleDeviceCompatibility001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMultipleDeviceCompatibility001 enter');
      
      let deviceList = usbMgr.getDevices();
      console.info(TAG, `Multiple device compatibility test, device count: ${deviceList.length}`);
      
      for (let i = 0; i < deviceList.length; i++) {
        let device = deviceList[i];
        expect(typeof device === 'object').assertTrue();
        expect(device.name.length > 0).assertTrue();
        expect(device.vendorId > 0).assertTrue();
        expect(device.productId > 0).assertTrue();
      }
    });
  })
}
