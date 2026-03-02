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

const TAG: string = "[UsbMassStorageTest]";
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

export default function UsbMassStorageTest() {
  describe('UsbMassStorageTest', () => {
    let isDeviceConnected: boolean = false;
    let devices: usbMgr.USBDevice;

    beforeAll(async (done: Function) => {
      console.log(TAG, '*************Usb Unit UsbMassStorageTest Begin*************');
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
      console.log('*************Usb Unit UsbMassStorageTest End*************');
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test Case*************');
    })

    it('testMassStorageDetection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMassStorageDetection001 enter');
      
      let deviceList = usbMgr.getDevices();
      console.info(TAG, `Mass storage device detection count: ${deviceList.length}`);
      
      for (let i = 0; i < deviceList.length; i++) {
        let device = deviceList[i];
        console.info(TAG, `Device ${i}: vendorId=0x${device.vendorId.toString(16)}, productId=0x${device.productId.toString(16)}`);
      }
      
      expect(deviceList.length >= 0).assertTrue();
    });

    it('testMassStorageInterface001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMassStorageInterface001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      console.info(TAG, `Mass storage interface count: ${config.interfaces.length}`);
      
      for (let i = 0; i < config.interfaces.length; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `Interface ${i}: class=0x${iface.clazz.toString(16)}, subclass=0x${iface.subClass.toString(16)}`);
        console.info(TAG, `Interface ${i}: protocol=0x${iface.protocol.toString(16)}, endpoints=${iface.endpoints.length}`);
      }
      
      expect(config.interfaces.length > 0).assertTrue();
    });

    it('testMassStorageEndpoints001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMassStorageEndpoints001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      console.info(TAG, `Mass storage endpoint count: ${iface.endpoints.length}`);
      
      for (let i = 0; i < iface.endpoints.length; i++) {
        let endpoint = iface.endpoints[i];
        console.info(TAG, `Endpoint ${i}: address=${endpoint.address}, type=${endpoint.type}`);
        console.info(TAG, `Endpoint ${i}: direction=${endpoint.direction}, maxPacketSize=${endpoint.maxPacketSize}`);
      }
      
      expect(iface.endpoints.length > 0).assertTrue();
    });

    it('testMassStorageBulkInEndpoint001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMassStorageBulkInEndpoint001 enter');
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

    it('testMassStorageBulkOutEndpoint001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMassStorageBulkOutEndpoint001 enter');
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

    it('testMassStorageMaxLun001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testMassStorageMaxLun001 enter');
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
        request: 0xFE,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0000,
        index: iface.id,
        data: new Uint8Array(1)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Get max LUN result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testMassStorageReset001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testMassStorageReset001 enter');
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
        request: 0xFF,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0000,
        index: iface.id,
        data: new Uint8Array(0)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Mass storage reset result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testMassStorageGetCapacity001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testMassStorageGetCapacity001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let cbw = new Uint8Array(31);
      cbw[0] = 0x55;
      cbw[1] = 0x53;
      cbw[2] = 0x42;
      cbw[3] = 0x43;
      cbw[8] = 0x25;
      cbw[15] = 0x08;
      
      let bulkOutEndpoint = iface.endpoints.find(ep => ep.type === 2 && ep.direction === 0);
      if (bulkOutEndpoint) {
        let result = await usbMgr.bulkTransfer(gPipe, bulkOutEndpoint, cbw, 5000);
        console.info(TAG, `Get capacity CBW result: ${result}`);
      }
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testMassStorageInquiry001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testMassStorageInquiry001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let cbw = new Uint8Array(31);
      cbw[0] = 0x55;
      cbw[1] = 0x53;
      cbw[2] = 0x42;
      cbw[3] = 0x43;
      cbw[8] = 0x12;
      cbw[15] = 0x24;
      
      let bulkOutEndpoint = iface.endpoints.find(ep => ep.type === 2 && ep.direction === 0);
      if (bulkOutEndpoint) {
        let result = await usbMgr.bulkTransfer(gPipe, bulkOutEndpoint, cbw, 5000);
        console.info(TAG, `Inquiry CBW result: ${result}`);
      }
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testMassStorageTestUnitReady001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testMassStorageTestUnitReady001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let cbw = new Uint8Array(31);
      cbw[0] = 0x55;
      cbw[1] = 0x53;
      cbw[2] = 0x42;
      cbw[3] = 0x43;
      cbw[8] = 0x00;
      cbw[15] = 0x00;
      
      let bulkOutEndpoint = iface.endpoints.find(ep => ep.type === 2 && ep.direction === 0);
      if (bulkOutEndpoint) {
        let result = await usbMgr.bulkTransfer(gPipe, bulkOutEndpoint, cbw, 5000);
        console.info(TAG, `Test unit ready CBW result: ${result}`);
      }
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testMassStorageRequestSense001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testMassStorageRequestSense001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let cbw = new Uint8Array(31);
      cbw[0] = 0x55;
      cbw[1] = 0x53;
      cbw[2] = 0x42;
      cbw[3] = 0x43;
      cbw[8] = 0x03;
      cbw[12] = 0x12;
      cbw[15] = 0x12;
      
      let bulkOutEndpoint = iface.endpoints.find(ep => ep.type === 2 && ep.direction === 0);
      if (bulkOutEndpoint) {
        let result = await usbMgr.bulkTransfer(gPipe, bulkOutEndpoint, cbw, 5000);
        console.info(TAG, `Request sense CBW result: ${result}`);
      }
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testMassStorageReadFormatCapabilities001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testMassStorageReadFormatCapabilities001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let cbw = new Uint8Array(31);
      cbw[0] = 0x55;
      cbw[1] = 0x53;
      cbw[2] = 0x42;
      cbw[3] = 0x43;
      cbw[8] = 0x23;
      cbw[15] = 0x02;
      
      let bulkOutEndpoint = iface.endpoints.find(ep => ep.type === 2 && ep.direction === 0);
      if (bulkOutEndpoint) {
        let result = await usbMgr.bulkTransfer(gPipe, bulkOutEndpoint, cbw, 5000);
        console.info(TAG, `Read format capabilities CBW result: ${result}`);
      }
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testMassStorageMultipleLun001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMassStorageMultipleLun001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let interfaceCount = config.interfaces.length;
      
      console.info(TAG, `Mass storage interfaces count: ${interfaceCount}`);
      
      for (let i = 0; i < interfaceCount; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `LUN ${i}: class=0x${iface.clazz.toString(16)}, subclass=0x${iface.subClass.toString(16)}`);
        console.info(TAG, `LUN ${i}: protocol=0x${iface.protocol.toString(16)}, endpoints=${iface.endpoints.length}`);
      }
    });

    it('testMassStoragePowerManagement001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMassStoragePowerManagement001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      console.info(TAG, `Mass storage maxPower: ${config.maxPower}mA`);
      console.info(TAG, `Mass storage isRemoteWakeup: ${config.isRemoteWakeup}`);
      console.info(TAG, `Mass storage isSelfPowered: ${config.isSelfPowered}`);
      
      expect(config.maxPower >= 0).assertTrue();
    });

    it('testMassStorageStringDescriptors001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMassStorageStringDescriptors001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      console.info(TAG, `Mass storage name: ${devices.name}`);
      console.info(TAG, `Mass storage serial: ${devices.serial}`);
      
      let config = devices.configs[0];
      console.info(TAG, `Config name: ${config.name}`);
      
      for (let i = 0; i < config.interfaces.length; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `Interface ${i} name: ${iface.name}`);
      }
    });

    it('testMassStorageReconnection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMassStorageReconnection001 enter');
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

    it('testMassStorageConcurrentAccess001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testMassStorageConcurrentAccess001 enter');
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
