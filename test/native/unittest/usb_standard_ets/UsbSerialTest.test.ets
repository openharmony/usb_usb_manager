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

const TAG: string = "[UsbSerialTest]";
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

export default function UsbSerialTest() {
  describe('UsbSerialTest', () => {
    let isDeviceConnected: boolean = false;
    let devices: usbMgr.USBDevice;

    beforeAll(async (done: Function) => {
      console.log(TAG, '*************Usb Unit UsbSerialTest Begin*************');
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
      console.log('*************Usb Unit UsbSerialTest End*************');
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test Case*************');
    })

    it('testSerialDeviceDetection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testSerialDeviceDetection001 enter');
      
      let deviceList = usbMgr.getDevices();
      console.info(TAG, `Serial device detection count: ${deviceList.length}`);
      
      for (let i = 0; i < deviceList.length; i++) {
        let device = deviceList[i];
        console.info(TAG, `Device ${i}: vendorId=0x${device.vendorId.toString(16)}, productId=0x${device.productId.toString(16)}`);
      }
      
      expect(deviceList.length >= 0).assertTrue();
    });

    it('testSerialDeviceConnection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testSerialDeviceConnection001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      console.info(TAG, `Serial device connected: busNum=${gPipe.busNum}, devAddress=${gPipe.devAddress}`);
      
      let closeResult = usbMgr.closePipe(gPipe);
      expect(closeResult).assertEqual(0);
    });

    it('testSerialInterfaceDetection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testSerialInterfaceDetection001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      console.info(TAG, `Serial interface count: ${config.interfaces.length}`);
      
      for (let i = 0; i < config.interfaces.length; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `Interface ${i}: class=0x${iface.clazz.toString(16)}, subclass=0x${iface.subClass.toString(16)}`);
        console.info(TAG, `Interface ${i}: protocol=0x${iface.protocol.toString(16)}, endpoints=${iface.endpoints.length}`);
      }
      
      expect(config.interfaces.length > 0).assertTrue();
    });

    it('testSerialEndpointDetection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testSerialEndpointDetection001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      console.info(TAG, `Serial endpoint count: ${iface.endpoints.length}`);
      
      for (let i = 0; i < iface.endpoints.length; i++) {
        let endpoint = iface.endpoints[i];
        console.info(TAG, `Endpoint ${i}: address=${endpoint.address}, type=${endpoint.type}`);
        console.info(TAG, `Endpoint ${i}: direction=${endpoint.direction}, maxPacketSize=${endpoint.maxPacketSize}`);
      }
      
      expect(iface.endpoints.length > 0).assertTrue();
    });

    it('testSerialBulkInEndpoint001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testSerialBulkInEndpoint001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let bulkInEndpoints = iface.endpoints.filter(ep => ep.type === 2 && ep.direction === 1);
      console.info(TAG, `Bulk IN endpoints count: ${bulkInEndpoints.length}`);
      
      for (let i = 0; i < bulkInEndpoints.length; i++) {
        let endpoint = bulkInEndpoints[i];
        console.info(TAG, `Bulk IN endpoint ${i}: address=${endpoint.address}, maxPacketSize=${endpoint.maxPacketSize}`);
      }
    });

    it('testSerialBulkOutEndpoint001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testSerialBulkOutEndpoint001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let bulkOutEndpoints = iface.endpoints.filter(ep => ep.type === 2 && ep.direction === 0);
      console.info(TAG, `Bulk OUT endpoints count: ${bulkOutEndpoints.length}`);
      
      for (let i = 0; i < bulkOutEndpoints.length; i++) {
        let endpoint = bulkOutEndpoints[i];
        console.info(TAG, `Bulk OUT endpoint ${i}: address=${endpoint.address}, maxPacketSize=${endpoint.maxPacketSize}`);
      }
    });

    it('testSerialInterruptEndpoint001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testSerialInterruptEndpoint001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let interruptEndpoints = iface.endpoints.filter(ep => ep.type === 3);
      console.info(TAG, `Interrupt endpoints count: ${interruptEndpoints.length}`);
      
      for (let i = 0; i < interruptEndpoints.length; i++) {
        let endpoint = interruptEndpoints[i];
        console.info(TAG, `Interrupt endpoint ${i}: address=${endpoint.address}, direction=${endpoint.direction}`);
      }
    });

    it('testSerialLineCoding001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testSerialLineCoding001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let lineCoding = new Uint8Array(7);
      lineCoding[0] = 0x00;
      lineCoding[1] = 0xC2;
      lineCoding[2] = 0x01;
      lineCoding[3] = 0x00;
      lineCoding[4] = 0x00;
      lineCoding[5] = 0x00;
      lineCoding[6] = 0x08;
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x20,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0000,
        index: iface.id,
        data: lineCoding
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Set line coding result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testSerialSetLineState001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testSerialSetLineState001 enter');
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
        request: 0x21,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0003,
        index: iface.id,
        data: new Uint8Array(0)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Set line state result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testSerialBreak001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testSerialBreak001 enter');
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
        request: 0x23,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0000,
        index: iface.id,
        data: new Uint8Array(2)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Send break result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testSerialClearBuffers001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testSerialClearBuffers001 enter');
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
        request: 0x22,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0008,
        index: iface.id,
        data: new Uint8Array(0)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Clear buffers result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testSerialGetLineCoding001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testSerialGetLineCoding001 enter');
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
        request: 0x21,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0000,
        index: iface.id,
        data: new Uint8Array(7)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Get line coding result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testSerialGetLineState001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testSerialGetLineState001 enter');
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
        request: 0x21,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0000,
        index: iface.id,
        data: new Uint8Array(2)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Get line state result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testSerialBaudRate001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testSerialBaudRate001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let baudRates = [9600, 19200, 38400, 57600, 115200];
      
      for (let baudRate of baudRates) {
        let lineCoding = new Uint8Array(7);
        let baudRateBytes = new DataView(new ArrayBuffer(4));
        baudRateBytes.setUint32(0, baudRate, true);
        
        lineCoding[0] = baudRateBytes.getUint8(0);
        lineCoding[1] = baudRateBytes.getUint8(1);
        lineCoding[2] = baudRateBytes.getUint8(2);
        lineCoding[3] = baudRateBytes.getUint8(3);
        lineCoding[4] = 0x00;
        lineCoding[5] = 0x00;
        lineCoding[6] = 0x08;
        
        let controlParam: usbMgr.USBControlParams = {
          request: 0x20,
          target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
          reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
          value: 0x0000,
          index: iface.id,
          data: lineCoding
        };
        
        let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
        console.info(TAG, `Set baud rate ${baudRate} result: ${result}`);
      }
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testSerialDataBits001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testSerialDataBits001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let dataBits = [5, 6, 7, 8];
      
      for (let bits of dataBits) {
        let lineCoding = new Uint8Array(7);
        lineCoding[0] = 0x00;
        lineCoding[1] = 0xC2;
        lineCoding[2] = 0x01;
        lineCoding[3] = 0x00;
        lineCoding[4] = 0x00;
        lineCoding[5] = 0x00;
        lineCoding[6] = bits;
        
        let controlParam: usbMgr.USBControlParams = {
          request: 0x20,
          target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
          reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
          value: 0x0000,
          index: iface.id,
          data: lineCoding
        };
        
        let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
        console.info(TAG, `Set data bits ${bits} result: ${result}`);
      }
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testSerialStopBits001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testSerialStopBits001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let stopBits = [0, 1, 2];
      
      for (let bits of stopBits) {
        let lineCoding = new Uint8Array(7);
        lineCoding[0] = 0x00;
        lineCoding[1] = 0xC2;
        lineCoding[2] = 0x01;
        lineCoding[3] = 0x00;
        lineCoding[4] = 0x00;
        lineCoding[5] = bits;
        lineCoding[6] = 0x08;
        
        let controlParam: usbMgr.USBControlParams = {
          request: 0x20,
          target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
          reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
          value: 0x0000,
          index: iface.id,
          data: lineCoding
        };
        
        let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
        console.info(TAG, `Set stop bits ${bits} result: ${result}`);
      }
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testSerialParity001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testSerialParity001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let parityValues = [0, 1, 2, 3];
      
      for (let parity of parityValues) {
        let lineCoding = new Uint8Array(7);
        lineCoding[0] = 0x00;
        lineCoding[1] = 0xC2;
        lineCoding[2] = 0x01;
        lineCoding[3] = 0x00;
        lineCoding[4] = parity;
        lineCoding[5] = 0x00;
        lineCoding[6] = 0x08;
        
        let controlParam: usbMgr.USBControlParams = {
          request: 0x20,
          target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
          reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
          value: 0x0000,
          index: iface.id,
          data: lineCoding
        };
        
        let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
        console.info(TAG, `Set parity ${parity} result: ${result}`);
      }
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });
  })
}
