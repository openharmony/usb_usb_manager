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

const TAG: string = "[UsbControlTransferTest]";
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

export default function UsbControlTransferTest() {
  describe('UsbControlTransferTest', () => {
    let isDeviceConnected: boolean = false;
    let devices: usbMgr.USBDevice;

    beforeAll(async (done: Function) => {
      console.log(TAG, '*************Usb Unit UsbControlTransferTest Begin*************');
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
      console.log('*************Usb Unit UsbControlTransferTest End*************');
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test Case*************');
    })

    it('testGetDeviceDescriptor001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testGetDeviceDescriptor001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x06,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
        value: 0x0100,
        index: 0x0000,
        data: new Uint8Array(18)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Get device descriptor result: ${result}`);
      expect(result >= 0).assertTrue();
      
      usbMgr.closePipe(gPipe);
    });

    it('testGetConfigDescriptor001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testGetConfigDescriptor001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x06,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
        value: 0x0200,
        index: 0x0000,
        data: new Uint8Array(9)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Get config descriptor result: ${result}`);
      expect(result >= 0).assertTrue();
      
      usbMgr.closePipe(gPipe);
    });

    it('testGetStringDescriptor001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testGetStringDescriptor001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x06,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
        value: 0x0300,
        index: 0x0409,
        data: new Uint8Array(2)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Get string descriptor result: ${result}`);
      expect(result >= 0).assertTrue();
      
      usbMgr.closePipe(gPipe);
    });

    it('testSetConfiguration001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testSetConfiguration001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let configValue = devices.configs[0].id;
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x09,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
        value: configValue,
        index: 0x0000,
        data: new Uint8Array(0)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Set configuration result: ${result}`);
      expect(result >= 0).assertTrue();
      
      usbMgr.closePipe(gPipe);
    });

    it('testGetConfiguration001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testGetConfiguration001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x08,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
        value: 0x0000,
        index: 0x0000,
        data: new Uint8Array(1)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Get configuration result: ${result}`);
      expect(result >= 0).assertTrue();
      
      usbMgr.closePipe(gPipe);
    });

    it('testClearFeature001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testClearFeature001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x01,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
        value: 0x0001,
        index: 0x0000,
        data: new Uint8Array(0)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Clear feature result: ${result}`);
      
      usbMgr.closePipe(gPipe);
    });

    it('testSetFeature001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testSetFeature001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x03,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
        value: 0x0001,
        index: 0x0000,
        data: new Uint8Array(0)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Set feature result: ${result}`);
      
      usbMgr.closePipe(gPipe);
    });

    it('testGetStatus001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testGetStatus001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x00,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
        value: 0x0000,
        index: 0x0000,
        data: new Uint8Array(2)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Get status result: ${result}`);
      expect(result >= 0).assertTrue();
      
      usbMgr.closePipe(gPipe);
    });

    it('testSetInterface001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testSetInterface001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x0B,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
        value: 0x0000,
        index: iface.id,
        data: new Uint8Array(0)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Set interface result: ${result}`);
      
      usbMgr.closePipe(gPipe);
    });

    it('testGetInterface001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testGetInterface001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x0A,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
        value: 0x0000,
        index: iface.id,
        data: new Uint8Array(1)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Get interface result: ${result}`);
      
      usbMgr.closePipe(gPipe);
    });

    it('testControlTransferTimeout001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testControlTransferTimeout001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x06,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
        value: 0x0100,
        index: 0x0000,
        data: new Uint8Array(18)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 100);
      console.info(TAG, `Control transfer with 100ms timeout result: ${result}`);
      expect(result >= 0).assertTrue();
      
      usbMgr.closePipe(gPipe);
    });

    it('testControlTransferLargeData001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testControlTransferLargeData001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x06,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
        value: 0x0200,
        index: 0x0000,
        data: new Uint8Array(255)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Control transfer with large data result: ${result}`);
      
      usbMgr.closePipe(gPipe);
    });

    it('testControlTransferMultiple001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testControlTransferMultiple001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      const iterations = 10;
      let successCount = 0;
      
      for (let i = 0; i < iterations; i++) {
        let controlParam: usbMgr.USBControlParams = {
          request: 0x06,
          target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
          reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
          value: 0x0100,
          index: 0x0000,
          data: new Uint8Array(18)
        };
        
        let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
        if (result >= 0) {
          successCount++;
        }
      }
      
      console.info(TAG, `Multiple control transfers: ${successCount}/${iterations} successful`);
      expect(successCount).assertLarger(iterations * 0.8);
      
      usbMgr.closePipe(gPipe);
    });

    it('testControlTransferDifferentTargets001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testControlTransferDifferentTargets001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let targets = [
        usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
        usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_ENDPOINT,
        usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_OTHER
      ];
      
      for (let target of targets) {
        let controlParam: usbMgr.USBControlParams = {
          request: 0x00,
          target: target,
          reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
          value: 0x0000,
          index: 0x0000,
          data: new Uint8Array(2)
        };
        
        let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
        console.info(TAG, `Control transfer to target ${target} result: ${result}`);
      }
      
      usbMgr.closePipe(gPipe);
    });

    it('testControlTransferDifferentTypes001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testControlTransferDifferentTypes001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let types = [
        usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
        usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        usbMgr.USBControlRequestType.USB_REQUEST_TYPE_VENDOR,
        usbMgr.USBControlRequestType.USB_REQUEST_TYPE_RESERVED
      ];
      
      for (let type of types) {
        let controlParam: usbMgr.USBControlParams = {
          request: 0x06,
          target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
          reqType: type,
          value: 0x0100,
          index: 0x0000,
          data: new Uint8Array(18)
        };
        
        let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
        console.info(TAG, `Control transfer with type ${type} result: ${result}`);
      }
      
      usbMgr.closePipe(gPipe);
    });
  })
}
