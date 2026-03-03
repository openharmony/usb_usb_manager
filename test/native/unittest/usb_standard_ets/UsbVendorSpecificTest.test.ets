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

const TAG: string = "[UsbVendorSpecificTest]";
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

export default function UsbVendorSpecificTest() {
  describe('UsbVendorSpecificTest', () => {
    let isDeviceConnected: boolean = false;
    let devices: usbMgr.USBDevice;

    beforeAll(async (done: Function) => {
      console.log(TAG, '*************Usb Unit UsbVendorSpecificTest Begin*************');
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
      console.log('*************Usb Unit UsbVendorSpecificTest End*************');
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test Case*************');
    })

    it('testVendorDeviceRequest001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testVendorDeviceRequest001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x01,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_VENDOR,
        value: 0x0000,
        index: 0x0000,
        data: new Uint8Array(64)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Vendor device request result: ${result}`);
      
      usbMgr.closePipe(gPipe);
    });

    it('testVendorInterfaceRequest001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testVendorInterfaceRequest001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x01,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_VENDOR,
        value: 0x0000,
        index: iface.id,
        data: new Uint8Array(64)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Vendor interface request result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testVendorEndpointRequest001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testVendorEndpointRequest001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      let endpoint = iface.endpoints[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x01,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_ENDPOINT,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_VENDOR,
        value: 0x0000,
        index: endpoint.address,
        data: new Uint8Array(64)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Vendor endpoint request result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testVendorOtherRequest001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testVendorOtherRequest001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x01,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_OTHER,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_VENDOR,
        value: 0x0000,
        index: 0x0000,
        data: new Uint8Array(64)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Vendor other request result: ${result}`);
      
      usbMgr.closePipe(gPipe);
    });

    it('testVendorRequestValues001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testVendorRequestValues001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let valueValues = [0x0000, 0x0100, 0x0200, 0x0300, 0x0400];
      
      for (let value of valueValues) {
        let controlParam: usbMgr.USBControlParams = {
          request: 0x01,
          target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
          reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_VENDOR,
          value: value,
          index: 0x0000,
          data: new Uint8Array(64)
        };
        
        let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
        console.info(TAG, `Vendor request with value 0x${value.toString(16)} result: ${result}`);
      }
      
      usbMgr.closePipe(gPipe);
    });

    it('testVendorRequestIndex001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testVendorRequestIndex001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let indexValues = [0x0000, 0x0001, 0x0002, 0x0003, 0x0004];
      
      for (let index of indexValues) {
        let controlParam: usbMgr.USBControlParams = {
          request: 0x01,
          target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
          reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_VENDOR,
          value: 0x0000,
          index: index,
          data: new Uint8Array(64)
        };
        
        let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
        console.info(TAG, `Vendor request with index 0x${index.toString(16)} result: ${result}`);
      }
      
      usbMgr.closePipe(gPipe);
    });

    it('testVendorRequestDataSize001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testVendorRequestDataSize001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let dataSizes = [0, 1, 8, 16, 32, 64, 128, 256];
      
      for (let size of dataSizes) {
        let controlParam: usbMgr.USBControlParams = {
          request: 0x01,
          target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
          reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_VENDOR,
          value: 0x0000,
          index: 0x0000,
          data: new Uint8Array(size)
        };
        
        let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
        console.info(TAG, `Vendor request with data size ${size} result: ${result}`);
      }
      
      usbMgr.closePipe(gPipe);
    });

    it('testVendorRequestCodes001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testVendorRequestCodes001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let requestCodes = [0x01, 0x02, 0x03, 0x04, 0x05];
      
      for (let request of requestCodes) {
        let controlParam: usbMgr.USBControlParams = {
          request: request,
          target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
          reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_VENDOR,
          value: 0x0000,
          index: 0x0000,
          data: new Uint8Array(64)
        };
        
        let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
        console.info(TAG, `Vendor request with code 0x${request.toString(16)} result: ${result}`);
      }
      
      usbMgr.closePipe(gPipe);
    });

    it('testVendorBulkTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testVendorBulkTransfer001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let bulkEndpoint = iface.endpoints.find(ep => ep.type === 2);
      if (bulkEndpoint) {
        let buffer = new Uint8Array(64);
        for (let i = 0; i < buffer.length; i++) {
          buffer[i] = i;
        }
        
        let result = await usbMgr.bulkTransfer(gPipe, bulkEndpoint, buffer, 5000);
        console.info(TAG, `Vendor bulk transfer result: ${result}`);
      }
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testVendorInterruptTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testVendorInterruptTransfer001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let interruptEndpoint = iface.endpoints.find(ep => ep.type === 3);
      if (interruptEndpoint) {
        let buffer = new Uint8Array(64);
        for (let i = 0; i < buffer.length; i++) {
          buffer[i] = i;
        }
        
        let result = await usbMgr.bulkTransfer(gPipe, interruptEndpoint, buffer, 5000);
        console.info(TAG, `Vendor interrupt transfer result: ${result}`);
      }
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testVendorIdentification001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testVendorIdentification001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      console.info(TAG, `Vendor device identification:`);
      console.info(TAG, `  Vendor ID: 0x${devices.vendorId.toString(16)}`);
      console.info(TAG, `  Product ID: 0x${devices.productId.toString(16)}`);
      console.info(TAG, `  Device name: ${devices.name}`);
      console.info(TAG, `  Serial number: ${devices.serial}`);
      
      expect(devices.vendorId > 0).assertTrue();
      expect(devices.productId > 0).assertTrue();
      expect(devices.name.length > 0).assertTrue();
    });

    it('testVendorSpecificClass001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testVendorSpecificClass001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      console.info(TAG, `Vendor specific class information:`);
      console.info(TAG, `  Interface class: 0x${iface.clazz.toString(16)}`);
      console.info(TAG, `  Interface subclass: 0x${iface.subClass.toString(16)}`);
      console.info(TAG, `  Interface protocol: 0x${iface.protocol.toString(16)}`);
      
      expect(iface.clazz >= 0xFF).assertTrue();
    });

    it('testVendorMultipleRequests001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testVendorMultipleRequests001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      const requestCount = 10;
      let successCount = 0;
      
      for (let i = 0; i < requestCount; i++) {
        let controlParam: usbMgr.USBControlParams = {
          request: 0x01,
          target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
          reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_VENDOR,
          value: i,
          index: 0x0000,
          data: new Uint8Array(64)
        };
        
        let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
        if (result >= 0) {
          successCount++;
        }
      }
      
      console.info(TAG, `Vendor multiple requests: ${successCount}/${requestCount} successful`);
      
      usbMgr.closePipe(gPipe);
    });

    it('testVendorErrorHandling001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testVendorErrorHandling001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      try {
        let controlParam: usbMgr.USBControlParams = {
          request: 0xFF,
          target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
          reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_VENDOR,
          value: 0xFFFF,
          index: 0xFFFF,
          data: new Uint8Array(1024)
        };
        
        let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
        console.info(TAG, `Vendor error handling result: ${result}`);
      } catch (error) {
        console.info(TAG, 'Expected vendor error:', error);
      }
      
      usbMgr.closePipe(gPipe);
    });

    it('testVendorConcurrentRequests001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testVendorConcurrentRequests001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let operations = [];
      
      for (let i = 0; i < 5; i++) {
        operations.push(
          (async (index: number) => {
            let controlParam: usbMgr.USBControlParams = {
              request: 0x01,
              target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
              reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_VENDOR,
              value: index,
              index: 0x0000,
              data: new Uint8Array(64)
            };
            
            let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
            console.info(TAG, `Concurrent vendor request ${index} result: ${result}`);
          })(i)
        );
      }
      
      await Promise.all(operations);
      
      usbMgr.closePipe(gPipe);
    });
  })
}
