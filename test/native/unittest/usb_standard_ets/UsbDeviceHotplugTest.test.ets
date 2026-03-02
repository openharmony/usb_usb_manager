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

const TAG: string = "[UsbDeviceHotplugTest]";
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

export default function UsbDeviceHotplugTest() {
  describe('UsbDeviceHotplugTest', () => {
    let isDeviceConnected: boolean = false;
    let devices: usbMgr.USBDevice;
    let initialDeviceCount: number = 0;

    beforeAll(async (done: Function) => {
      console.log(TAG, '*************Usb Unit UsbDeviceHotplugTest Begin*************');
      let gDeviceList: Array<usbMgr.USBDevice> = usbMgr.getDevices();
      console.info(TAG, 'usb unit begin test getDevices **********', JSON.stringify(gDeviceList));
      initialDeviceCount = gDeviceList.length;
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
      console.log('*************Usb Unit UsbDeviceHotplugTest End*************');
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test Case*************');
    })

    it('testDeviceEnumeration001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceEnumeration001 enter');
      
      let deviceList = usbMgr.getDevices();
      console.info(TAG, `Device enumeration count: ${deviceList.length}`);
      expect(deviceList.length >= 0).assertTrue();
      
      if (deviceList.length > 0) {
        for (let i = 0; i < deviceList.length; i++) {
          console.info(TAG, `Device ${i}: ${JSON.stringify(deviceList[i])}`);
          expect(deviceList[i].name).assertLarger(0);
          expect(deviceList[i].vendorId).assertLarger(0);
          expect(deviceList[i].productId).assertLarger(0);
        }
      }
    });

    it('testDeviceEnumerationConsistency001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceEnumerationConsistency001 enter');
      
      let deviceList1 = usbMgr.getDevices();
      let deviceList2 = usbMgr.getDevices();
      
      console.info(TAG, `First enumeration count: ${deviceList1.length}`);
      console.info(TAG, `Second enumeration count: ${deviceList2.length}`);
      
      expect(deviceList1.length).assertEqual(deviceList2.length);
      
      for (let i = 0; i < deviceList1.length; i++) {
        expect(deviceList1[i].name).assertEqual(deviceList2[i].name);
        expect(deviceList1[i].vendorId).assertEqual(deviceList2[i].vendorId);
        expect(deviceList1[i].productId).assertEqual(deviceList2[i].productId);
      }
    });

    it('testDeviceConnectionState001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceConnectionState001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let deviceList = usbMgr.getDevices();
      expect(deviceList.length > 0).assertTrue();
      
      let device = deviceList[0];
      console.info(TAG, `Device connected: ${device.name}`);
      
      let hasRight = usbMgr.hasRight(device.name);
      console.info(TAG, `Device has right: ${hasRight}`);
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(device);
      expect(gPipe !== null).assertTrue();
      
      let closeResult = usbMgr.closePipe(gPipe);
      expect(closeResult).assertEqual(0);
    });

    it('testMultipleDeviceEnumeration001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMultipleDeviceEnumeration001 enter');
      
      const enumerationCount = 10;
      let deviceCounts: number[] = [];
      
      for (let i = 0; i < enumerationCount; i++) {
        let deviceList = usbMgr.getDevices();
        deviceCounts.push(deviceList.length);
        console.info(TAG, `Enumeration ${i}: ${deviceList.length} devices`);
      }
      
      for (let i = 1; i < deviceCounts.length; i++) {
        expect(deviceCounts[i]).assertEqual(deviceCounts[0]);
      }
    });

    it('testDeviceProperties001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceProperties001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let deviceList = usbMgr.getDevices();
      let device = deviceList[0];
      
      console.info(TAG, `Device name: ${device.name}`);
      console.info(TAG, `Device vendorId: ${device.vendorId}`);
      console.info(TAG, `Device productId: ${device.productId}`);
      console.info(TAG, `Device serial: ${device.serial}`);
      console.info(TAG, `Device config count: ${device.configs.length}`);
      
      expect(device.name).assertLarger(0);
      expect(device.vendorId).assertLarger(0);
      expect(device.productId).assertLarger(0);
      expect(device.configs.length).assertLarger(0);
    });

    it('testDeviceConfigurations001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceConfigurations001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let deviceList = usbMgr.getDevices();
      let device = deviceList[0];
      
      console.info(TAG, `Device has ${device.configs.length} configurations`);
      expect(device.configs.length).assertLarger(0);
      
      for (let i = 0; i < device.configs.length; i++) {
        let config = device.configs[i];
        console.info(TAG, `Config ${i}: id=${config.id}, name=${config.name}, maxPower=${config.maxPower}`);
        console.info(TAG, `Config ${i}: isRemoteWakeup=${config.isRemoteWakeup}, isSelfPowered=${config.isSelfPowered}`);
        console.info(TAG, `Config ${i}: interfaces count=${config.interfaces.length}`);
        
        expect(config.interfaces.length).assertLarger(0);
      }
    });

    it('testDeviceInterfaces001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceInterfaces001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let deviceList = usbMgr.getDevices();
      let device = deviceList[0];
      let config = device.configs[0];
      
      
      console.info(TAG, `Configuration has ${config.interfaces.length} interfaces`);
      
      for (let i = 0; i < config.interfaces.length; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `Interface ${i}: id=${iface.id}, class=${iface.clazz}, subclass=${iface.subClass}`);
        console.info(TAG, `Interface ${i}: protocol=${iface.protocol}, name=${iface.name}`);
        console.info(TAG, `Interface ${i}: endpoints count=${iface.endpoints.length}`);
        
        expect(iface.endpoints.length).assertLarger(0);
      }
    });

    it('testDeviceEndpoints001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceEndpoints001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let deviceList = usbMgr.getDevices();
      let device = deviceList[0];
      let config = device.configs[0];
      let iface = config.interfaces[0];
      
      console.info(TAG, `Interface has ${iface.endpoints.length} endpoints`);
      
      for (let i = 0; i < iface.endpoints.length; i++) {
        let endpoint = iface.endpoints[i];
        console.info(TAG, `Endpoint ${i}: address=${endpoint.address}, type=${endpoint.type}`);
        console.info(TAG, `Endpoint ${i}: direction=${endpoint.direction}, maxPacketSize=${endpoint.maxPacketSize}`);
        console.info(TAG, `Endpoint ${i}: interval=${endpoint.interval}`);
        
        expect(endpoint.address >= 0).assertTrue();
        expect(endpoint.maxPacketSize > 0).assertTrue();
      }
    });

    it('testDeviceDescriptorAccess001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceDescriptorAccess001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let deviceList = usbMgr.getDevices();
      let device = deviceList[0];
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(device);
      expect(gPipe !== null).assertTrue();
      
      let rawDescriptor = usbMgr.getRawDescriptor(gPipe);
      console.info(TAG, `Raw descriptor length: ${rawDescriptor.length}`);
      expect(rawDescriptor.length).assertLarger(0);
      
      let fileDescriptor = usbMgr.getFileDescriptor(gPipe);
      console.info(TAG, `File descriptor: ${fileDescriptor}`);
      expect(fileDescriptor >= 0).assertTrue();
      
      let closeResult = usbMgr.closePipe(gPipe);
      expect(closeResult).assertEqual(0);
    });

    it('testDeviceReconnection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testDeviceReconnection001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let deviceList = usbMgr.getDevices();
      let device = deviceList[0];
      
      const reconnectCount = 3;
      
      for (let i = 0; i < reconnectCount; i++) {
        console.info(TAG, `Reconnection iteration ${i}`);
        
        let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(device);
        expect(gPipe !== null).assertTrue();
        
        await sleep(100);
        
        let closeResult = usbMgr.closePipe(gPipe);
        expect(closeResult).assertEqual(0);
        
        await sleep(100);
      }
    });

    it('testDeviceListStability001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testDeviceListStability001 enter');
      
      let deviceList1 = usbMgr.getDevices();
      console.info(TAG, `Initial device count: ${deviceList1.length}`);
      
      await sleep(1000);
      
      let deviceList2 = usbMgr.getDevices();
      console.info(TAG, `After 1s device count: ${deviceList2.length}`);
      
      await sleep(2000);
      
      let deviceList3 = usbMgr.getDevices();
      console.info(TAG, `After 3s device count: ${deviceList3.length}`);
      
      expect(deviceList1.length).assertEqual(deviceList2.length);
      expect(deviceList2.length).assertEqual(deviceList3.length);
    });

    it('testDeviceIdentity001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceIdentity001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let deviceList = usbMgr.getDevices();
      let device = deviceList[0];
      
      console.info(TAG, `Device identity - Name: ${device.name}`);
      console.info(TAG, `Device identity - Vendor ID: 0x${device.vendorId.toString(16)}`);
      console.info(TAG, `Device identity - Product ID: 0x${device.productId.toString(16)}`);
      console.info(TAG, `Device identity - Serial: ${device.serial}`);
      
      expect(device.name).assertLarger(0);
      expect(device.vendorId > 0 && device.vendorId <= 0xFFFF).assertTrue();
      expect(device.productId > 0 && device.productId <= 0xFFFF).assertTrue();
    });

    it('testDeviceConnectionLifecycle001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceConnectionLifecycle001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let deviceList = usbMgr.getDevices();
      let device = deviceList[0];
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(device);
      expect(gPipe !== null).assertTrue();
      console.info(TAG, 'Device connected');
      
      let fd = usbMgr.getFileDescriptor(gPipe);
      expect(fd >= 0).assertTrue();
      console.info(TAG, `File descriptor obtained: ${fd}`);
      
      let rawDesc = usbMgr.getRawDescriptor(gPipe);
      expect(rawDesc.length > 0).assertTrue();
      console.info(TAG, `Raw descriptor obtained: ${rawDesc.length} bytes`);
      
      let closeResult = usbMgr.closePipe(gPipe);
      expect(closeResult).assertEqual(0);
      console.info(TAG, 'Device disconnected');
    });

    it('testNoDeviceScenario001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNoDeviceScenario001 enter');
      
      if (isDeviceConnected) {
        console.info(TAG, 'Device is connected, skipping no device test');
        return;
      }
      
      let deviceList = usbMgr.getDevices();
      console.info(TAG, `No device scenario - device count: ${deviceList.length}`);
      expect(deviceList.length).assertEqual(0);
    });
  })
}
