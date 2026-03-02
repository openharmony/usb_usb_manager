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

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium'
import usbMgr from '@ohos.usbManager';
import { UiDriver, BY } from '@ohos.UiTest';

const TAG: string = "[UsbHidTest]";
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

export default function UsbHidTest() {
  describe('UsbHidTest', () => {
    let isDeviceConnected: boolean = false;
    let devices: usbMgr.USBDevice;

    beforeAll(async (done: Function) => {
      console.log(TAG, '*************Usb Unit UsbHidTest Begin*************');
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
      console.log('*************Usb Unit UsbHidTest End*************');
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test Case*************');
    })

    it('testHidDeviceDetection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testHidDeviceDetection001 enter');
      
      let deviceList = usbMgr.getDevices();
      console.info(TAG, `HID device detection count: ${deviceList.length}`);
      
      for (let i = 0; i < deviceList.length; i++) {
        let device = deviceList[i];
        console.info(TAG, `Device ${i}: vendorId=0x${device.vendorId.toString(16)}, productId=0x${device.productId.toString(16)}`);
      }
      
      expect(deviceList.length >= 0).assertTrue();
    });

    it('testHidInterfaceDetection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testHidInterfaceDetection001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      console.info(TAG, `HID interface count: ${config.interfaces.length}`);
      
      for (let i = 0; i < config.interfaces.length; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `Interface ${i}: class=0x${iface.clazz.toString(16)}, subclass=0x${iface.subClass.toString(16)}`);
        console.info(TAG, `Interface ${i}: protocol=0x${iface.protocol.toString(16)}, endpoints=${iface.endpoints.length}`);
      }
      
      expect(config.interfaces.length > 0).assertTrue();
    });

    it('testHidEndpointDetection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testHidEndpointDetection001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      console.info(TAG, `HID endpoint count: ${iface.endpoints.length}`);
      
      for (let i = 0; i < iface.endpoints.length; i++) {
        let endpoint = iface.endpoints[i];
        console.info(TAG, `Endpoint ${i}: address=${endpoint.address}, type=${endpoint.type}`);
        console.info(TAG, `Endpoint ${i}: direction=${endpoint.direction}, maxPacketSize=${endpoint.maxPacketSize}`);
      }
      
      expect(iface.endpoints.length > 0).assertTrue();
    });

    it('testHidInterruptInEndpoint001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testHidInterruptInEndpoint001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let interruptInEndpoints = iface.endpoints.filter(ep => ep.type === 3 && ep.direction === 1);
      console.info(TAG, `Interrupt IN: endpoints count: ${interruptInEndpoints.length}`);
      
      for (let i = 0; i < interruptInEndpoints.length; i++) {
        let endpoint = interruptInEndpoints[i];
        console.info(TAG, `Interrupt IN endpoint ${i}: address=${endpoint.address}, maxPacketSize=${endpoint.maxPacketSize}`);
      }
    });

    it('testHidInterruptOutEndpoint001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testHidInterruptOutEndpoint001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let interruptOutEndpoints = iface.endpoints.filter(ep => ep.type === 3 && ep.direction === 0);
      console.info(TAG, `Interrupt OUT endpoints count: ${interruptOutEndpoints.length}`);
      
      for (let i = 0; i < interruptOutEndpoints.length; i++) {
        let endpoint = interruptOutEndpoints[i];
        console.info(TAG, `Interrupt OUT endpoint ${i}: address=${endpoint.address}, maxPacketSize=${endpoint.maxPacketSize}`);
      }
    });

    it('testHidGetReportDescriptor001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testHidGetReportDescriptor001 enter');
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
        request: 0x06,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
        value: 0x2200,
        index: iface.id,
        data: new Uint8Array(256)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Get HID report descriptor result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testHidSetIdle001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testHidSetIdle001 enter');
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
        request: 0x0A,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0000,
        index: iface.id,
        data: new Uint8Array(0)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Set HID idle result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testHidGetIdle001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testHidGetIdle001 enter');
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
        request: 0x02,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0000,
        index: iface.id,
        data: new Uint8Array(1)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Get HID idle result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testHidSetProtocol001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testHidSetProtocol001 enter');
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
        request: 0x0B,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0000,
        index: iface.id,
        data: new Uint8Array(0)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Set HID protocol result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testHidGetProtocol001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testHidGetProtocol001 enter');
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
        request: 0x03,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0000,
        index: iface.id,
        data: new Uint8Array(1)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Get HID protocol result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testHidSetReport001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testHidSetReport001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let reportData = new Uint8Array(8);
      reportData[0] = 0x00;
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x09,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0200,
        index: iface.id,
        data: reportData
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Set HID report result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
            usbMgr.closePipe(gPipe);
    });

    it('testHidGetReport001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testHidGetReport001 enter');
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
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0100,
        index: iface.id,
        data: new Uint8Array(8)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Get HID report result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testHidPowerManagement001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testHidPowerManagement001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      console.info(TAG, `HID maxPower: ${config.maxPower}mA`);
      console.info(TAG, `HID isRemoteWakeup: ${config.isRemoteWakeup}`);
      console.info(TAG, `HID isSelfPowered: ${config.isSelfPowered}`);
      
      expect(config.maxPower >= 0).assertTrue();
    });

    it('testHidStringDescriptors001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testHidStringDescriptors001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      console.info(TAG, `HID name: ${devices.name}`);
      console.info(TAG, `HID serial: ${devices.serial}`);
      
      let config = devices.configs[0];
      console.info(TAG, `Config name: ${config.name}`);
      
      for (let i = 0; i < config.interfaces.length; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `Interface ${i} name: ${iface.name}`);
      }
    });

    it('testHidMultipleInterfaces001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testHidMultipleInterfaces001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let interfaceCount = config.interfaces.length;
      
      console.info(TAG, `HID interfaces count: ${interfaceCount}`);
      
      for (let i = 0; i < interfaceCount; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `Interface ${i}: class=0x${iface.clazz.toString(16)}, subclass=0x${iface.subClass.toString(16)}`);
        console.info(TAG, `Interface ${i}: protocol=0x${iface.protocol.toString(16)}, endpoints=${iface.endpoints.length}`);
      }
    });

    it('testHidReconnection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testHidReconnection001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      const reconnectCount = 3;
      
      for (let i = 0; i < reconnectCount; i++) {
        let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
        expect(gPipe !== null).assertTrue();
        
        let closeResult = usbMgr.closePipe(gPipe);
        expect(closeResult).assertEqual(0);
        
        console.info(TAG, `Reconnection ${i + 1}/${reconnectCount} completed`);
      }
    });

    it('testHidConcurrentAccess001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testHidConcurrentAccess001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let operations = [];
      
      for (let i = 0; i < 5; i++) {
        operations.push(
          new Promise<void>((resolve) => {
            let descriptor = usbMgr.getRawDescriptor(gPipe);
            expect(descriptor.length > 0).assertTrue();
            resolve();
          })
        );
      }
      
      await Promise.all(operations);
      console.info(TAG, 'Concurrent access test completed');
      
      usbMgr.closePipe(gPipe);
    });
  })
}
