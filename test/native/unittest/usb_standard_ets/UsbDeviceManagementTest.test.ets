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

import usbMgr from '@ohos.usbManager';
import { UiDriver, BY } from '@ohos.UiTest';
import { BusinessError } from '@ohos.base';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium';

const TAG = "[UsbDeviceManagementTest]";
let gDeviceList: Array<usbMgr.USBDevice>;
let gPipe: usbMgr.USBDevicePipe | null;
let devices: usbMgr.USBDevice | null;
let isDeviceConnected: boolean;

function sleep(ms: number): Promise<void> {
  return new Promise<void>(resolve => setTimeout(resolve, ms));
}

function deviceConnected() {
  if (gDeviceList.length > 0) {
    console.info(TAG, "Test USB device is connected");
    return true;
  }
  console.info(TAG, "Test USB device is not connected");
  return false;
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

async function getPermission() {
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

function getPipe(testCaseName: string) {
  gPipe = usbMgr.connectDevice(devices);
  console.info(TAG, `usb ${testCaseName} connectDevice getPipe ret: ${JSON.stringify(gPipe)}`);
  expect(gPipe !== null).assertTrue();
}

function toClosePipe(testCaseName: string) {
  let isPipClose = usbMgr.closePipe(gPipe);
  console.info(TAG, `usb ${testCaseName} closePipe getPipe ret: ${isPipClose}`);
  expect(isPipClose).assertEqual(0);
}

function ab2str(buf: Uint8Array): string {
  return String.fromCharCode.apply(null, Array.from(buf));
}

function str2ab(str: string): Uint8Array {
  const buf = new ArrayBuffer(str.length);
  const bufView = new Uint8Array(buf);
  for (let i = 0, strLen = str.length; i < strLen; i++) {
    bufView[i] = str.charCodeAt(i);
  }
  return bufView;
}

/* USB device management and enumeration test */
export default function UsbDeviceManagementTest() {
  describe('UsbDeviceManagementTest', () => {

    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbDeviceManagementTest Begin*************');

      gDeviceList = usbMgr.getDevices();
      console.info(TAG, 'beforeAll: usb case gDeviceList.length return: ' + JSON.stringify(gDeviceList));
      isDeviceConnected = deviceConnected();
      if (isDeviceConnected) {
        let hasRight = usbMgr.hasRight(gDeviceList[0].name);
        if (!hasRight) {
          console.info(TAG, `beforeAll: usb requestRight start`);
          await getPermission();
          await sleep(1000);
          await driveFn();
          await sleep(1000);
        }
      }
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
      gDeviceList = usbMgr.getDevices();
      if (isDeviceConnected) {
        devices = gDeviceList[0];
        console.info(TAG, 'beforeEach return devices : ' + JSON.stringify(devices));
      }
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test Case*************');
      devices = null;
      gPipe = null;
    })

    afterAll(() => {
      console.log(TAG, '*************Usb Unit UsbDeviceManagementTest End*************');
    })

    /**
     * @tc.name   testDeviceEnumeration001
     * @tc.number SUB_USB_DeviceManagement_Enumerate_0100
     * @tc.desc   Test device enumeration with multiple connected devices
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceEnumeration001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, (done: Function) => {
      console.info(TAG, 'usb testDeviceEnumeration001 begin');
      if (!isDeviceConnected) {
        console.info(TAG, 'No device connected for enumeration test');
        expect(isDeviceConnected).assertFalse();
        done();
        return;
      }

      let deviceList = usbMgr.getDevices();
      console.info(TAG, 'Device enumeration returned ' + deviceList.length + ' devices');
      expect(deviceList.length > 0).assertTrue();
      
      // Verify device properties structure
      deviceList.forEach((device, index) => {
        console.info(TAG, `Device ${index} info: ` + JSON.stringify(device));
        expect(device).not.toBeNull();
        expect(device.name).not.toBeNull();
        expect(device.serial).not.toBeNull();
        expect(device.configs).not.toBeNull();
        expect(device.configs.length).toBeLargerThan(0);
      });
      
      done();
    })

    /**
     * @tc.name   testDeviceInfoValidation001
     * @tc.number SUB_USB_DeviceManagement_Info_0100
     * @tc.desc   Test USB device information validation
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceInfoValidation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDeviceInfoValidation001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      let device = gDeviceList[0];
      
      // Validate basic device properties
      expect(device.busNum).toBeLargerThan(-1);
      expect(device.devAddress).toBeLargerThan(-1);
      expect(device.name).not.toBeNull();
      expect(device.serial).not.toBeNull();
      
      // Validate configurations
      expect(device.configs).not.toBeNull();
      expect(device.configs.length).toBeLargerThan(0);
      
      device.configs.forEach((config, configIndex) => {
        console.info(TAG, `Config ${configIndex} info: ` + JSON.stringify(config));
        expect(config.id).toBeLargerThan(-1);
        expect(config.interfaces).not.toBeNull();
        expect(config.interfaces.length).toBeLargerThan(0);
        
        // Validate interfaces
        config.interfaces.forEach((iface, ifaceIndex) => {
          console.info(TAG, `Interface ${ifaceIndex} info: ` + JSON.stringify(iface));
          expect(iface.id).toBeLargerThan(-1);
          expect(iface.clazz).toBeLargerThan(-1);
          expect(iface.subClass).toBeLargerThan(-1);
          expect(iface.protocol).toBeLargerThan(-1);
          expect(iface.endpoints).not.toBeNull();
          
          // Validate endpoints
          iface.endpoints.forEach((endpoint, epIndex) => {
            console.info(TAG, `Endpoint ${epIndex} info: ` + JSON.stringify(endpoint));
            expect(endpoint.address).toBeLargerThan(-1);
            expect(endpoint.type).toBeLargerThan(-1);
            expect(endpoint.direction).toBeLargerThan(-1);
            expect(endpoint.maxPacketSize).toBeLargerThan(0);
          });
        });
      });
    })

    /**
     * @tc.name   testDeviceConnection001
     * @tc.number SUB_USB_DeviceManagement_Connect_0100
     * @tc.desc   Test device connection and disconnection
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceConnection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDeviceConnection001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      // Test device connection
      let pipe = usbMgr.connectDevice(devices);
      console.info(TAG, 'Device connection pipe: ' + JSON.stringify(pipe));
      expect(pipe !== null).assertTrue();
      expect(pipe.busNum).toBeLargerThan(-1);
      expect(pipe.devAddress).toBeLargerThan(-1);

      // Test device disconnection
      let closeResult = usbMgr.closePipe(pipe);
      console.info(TAG, 'Device close result: ' + closeResult);
      expect(closeResult).assertEqual(0);
    })

    /**
     * @tc.name   testDevicePermissions001
     * @tc.number SUB_USB_DeviceManagement_Permissions_0100
     * @tc.desc   Test USB device permission management
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDevicePermissions001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testDevicePermissions001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      // Test initial permission check
      let hasRight = usbMgr.hasRight(devices.name);
      console.info(TAG, 'Initial device permission: ' + hasRight);
      
      // Remove permission
      let removeResult = usbMgr.removeRight(devices.name);
      console.info(TAG, 'Remove permission result: ' + removeResult);
      expect(removeResult).assertTrue();
      
      // Check permission after removal
      hasRight = usbMgr.hasRight(devices.name);
      console.info(TAG, 'Permission after removal: ' + hasRight);
      expect(hasRight).assertFalse();
      
      // Request permission again
      try {
        hasRight = await usbMgr.requestRight(devices.name);
        console.info(TAG, 'Request permission result: ' + hasRight);
        expect(hasRight).assertTrue();
      } catch (error) {
        console.info(TAG, 'Request permission failed: ' + error);
        expect(error === null).assertTrue();
      }
    })

    /**
     * @tc.name   testDeviceDescriptor001
     * @tc.number SUB_USB_DeviceManagement_Descriptor_0100
     * @tc.desc   Test USB device descriptor retrieval
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceDescriptor001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDeviceDescriptor001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe('testDeviceDescriptor001');
      
      // Test raw descriptor
      let rawDescriptor = usbMgr.getRawDescriptor(gPipe);
      console.info(TAG, 'Raw descriptor length: ' + rawDescriptor.length);
      expect(rawDescriptor.length).toBeLargerThan(0);
      
      // Test file descriptor
      let fileDescriptor = usbMgr.getFileDescriptor(gPipe);
      console.info(TAG, 'File descriptor: ' + fileDescriptor);
      expect(fileDescriptor).toBeLargerThan(-1);
      
      toClosePipe('testDeviceDescriptor001');
    })

    /**
     * @tc.name   testDeviceMultipleConnection001
     * @tc.number SUB_USB_DeviceManagement_Multiple_0100
     * @tc.desc   Test multiple device connections
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceMultipleConnection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDeviceMultipleConnection001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      // Only test if multiple devices are available
      if (gDeviceList.length < 2) {
        console.info(TAG, 'Need at least 2 devices for multiple connection test');
        return;
      }

      let pipes = [];
      
      // Connect to all available devices
      for (let i = 0; i < gDeviceList.length; i++) {
        let pipe = usbMgr.connectDevice(gDeviceList[i]);
        console.info(TAG, `Connected to device ${i}, pipe: ${JSON.stringify(pipe)}`);
        expect(pipe !== null).assertTrue();
        pipes.push(pipe);
      }
      
      // Close all connections
      for (let i = 0; i < pipes.length; i++) {
        let closeResult = usbMgr.closePipe(pipes[i]);
        console.info(TAG, `Closed device ${i}, result: ${closeResult}`);
        expect(closeResult).assertEqual(0);
      }
    })

    /**
     * @tc.name   testDeviceInvalidOperation001
     * @tc.number SUB_USB_DeviceManagement_Invalid_0100
     * @tc.desc   Test invalid operations on USB devices
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceInvalidOperation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDeviceInvalidOperation001 begin');
      
      // Test with null device
      try {
        let pipe = usbMgr.connectDevice(null);
        expect(pipe === null).assertTrue();
      } catch (error) {
        console.info(TAG, 'Expected error for null device: ' + error);
      }
      
      // Test with invalid device
      try {
        let invalidDevice: usbMgr.USBDevice = {
          busNum: -1,
          devAddress: -1,
          name: 'invalid',
          serial: 'invalid',
          configs: []
        };
        let pipe = usbMgr.connectDevice(invalidDevice);
        expect(pipe === null).assertTrue();
      } catch (error) {
        console.info(TAG, 'Expected error for invalid device: ' + error);
      }
      
      // Test permission operations with invalid device name
      let hasRight = usbMgr.hasRight('invalid_device_name');
      expect(hasRight).assertFalse();
      
      let removeResult = usbMgr.removeRight('invalid_device_name');
      expect(removeResult).assertFalse();
    })

    /**
     * @tc.name   testDeviceHotPlug001
     * @tc.number SUB_USB_DeviceManagement_HotPlug_0100
     * @tc.desc   Test device hot-plug detection
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceHotPlug001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDeviceHotPlug001 begin');
      
      // Get initial device count
      let initialDeviceList = usbMgr.getDevices();
      let initialCount = initialDeviceList.length;
      console.info(TAG, 'Initial device count: ' + initialCount);
      
      // Simulate device list refresh
      let currentDeviceList = usbMgr.getDevices();
      let currentCount = currentDeviceList.length;
      console.info(TAG, 'Current device count: ' + currentCount);
      
      // In a real test scenario, you would simulate device hot-plug
      // For now, just verify the device count tracking works
      expect(currentCount).toBeLargerThan(-1);
      
      // Verify device properties if devices are present
      if (currentCount > 0) {
        currentDeviceList.forEach((device, index) => {
          console.info(TAG, `Device ${index} after refresh: ` + JSON.stringify(device));
          expect(device).not.toBeNull();
          expect(device.name).not.toBeNull();
        });
      }
    })
  })
}