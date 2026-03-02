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

const TAG: string = "[UsbAccessoryTest]";
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

export default function UsbAccessoryTest() {
  describe('UsbAccessoryTest', () => {
    let isDeviceConnected: boolean = false;
    let devices: usbMgr.USBDevice;

    beforeAll(async (done: Function) => {
      console.log(TAG, '*************Usb Unit UsbAccessoryTest Begin*************');
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
      console.log('*************Usb Unit UsbAccessoryTest End*************');
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test Case*************');
    })

    it('testAccessoryModeDetection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAccessoryModeDetection001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let deviceList = usbMgr.getDevices();
      expect(deviceList.length > 0).assertTrue();
      
      for (let i = 0; i < deviceList.length; i++) {
        let device = deviceList[i];
        console.info(TAG, `Device ${i}: vendorId=0x${device.vendorId.toString(16)}, productId=0x${device.productId.toString(16)}`);
        console.info(TAG, `Device ${i}: name=${device.name}, serial=${device.serial}`);
      }
    });

    it('testAccessoryConnection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAccessoryConnection001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      console.info(TAG, `Accessory connected: busNum=${gPipe.busNum}, devAddress=${gPipe.devAddress}`);
      
      let closeResult = usbMgr.closePipe(gPipe);
      expect(closeResult).assertEqual(0);
    });

    it('testAccessoryDescriptor001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAccessoryDescriptor001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let rawDescriptor = usbMgr.getRawDescriptor(gPipe);
      expect(rawDescriptor.length > 0).assertTrue();
      console.info(TAG, `Accessory descriptor length: ${rawDescriptor.length}`);
      
      usbMgr.closePipe(gPipe);
    });

    it('testAccessoryInterfaces001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAccessoryInterfaces001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      console.info(TAG, `Accessory interfaces count: ${config.interfaces.length}`);
      
      for (let i = 0; i < config.interfaces.length; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `Interface ${i}: id=${iface.id}, class=${iface.clazz}, subclass=${iface.subClass}`);
        console.info(TAG, `Interface ${i}: protocol=${iface.protocol}, endpoints=${iface.endpoints.length}`);
      }
    });

    it('testAccessoryEndpoints001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAccessoryEndpoints001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      console.info(TAG, `Accessory endpoints count: ${iface.endpoints.length}`);
      
      for (let i = 0; i < iface.endpoints.length; i++) {
        let endpoint = iface.endpoints[i];
        console.info(TAG, `Endpoint ${i}: address=${endpoint.address}, type=${endpoint.type}`);
        console.info(TAG, `Endpoint ${i}: direction=${endpoint.direction}, maxPacketSize=${endpoint.maxPacketSize}`);
      }
    });

    it('testAccessoryBulkEndpoints001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAccessoryBulkEndpoints001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let bulkEndpoints = iface.endpoints.filter(ep => ep.type === 2);
      console.info(TAG, `Accessory bulk endpoints count: ${bulkEndpoints.length}`);
      
      for (let i = 0; i < bulkEndpoints.length; i++) {
        let endpoint = bulkEndpoints[i];
        console.info(TAG, `Bulk endpoint ${i}: address=${endpoint.address}, direction=${endpoint.direction}`);
      }
    });

    it('testAccessoryInterruptEndpoints001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAccessoryInterruptEndpoints001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let interruptEndpoints = iface.endpoints.filter(ep => ep.type === 3);
      console.info(TAG, `Accessory interrupt endpoints count: ${interruptEndpoints.length}`);
      
      for (let i = 0; i < interruptEndpoints.length; i++) {
        let endpoint = interruptEndpoints[i];
        console.info(TAG, `Interrupt endpoint ${i}: address=${endpoint.address}, direction=${endpoint.direction}`);
      }
    });

    it('testAccessoryPowerManagement001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAccessoryPowerManagement001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      console.info(TAG, `Accessory maxPower: ${config.maxPower}mA`);
      console.info(TAG, `Accessory isRemoteWakeup: ${config.isRemoteWakeup}`);
      console.info(TAG, `Accessory isSelfPowered: ${config.isSelfPowered}`);
      
      expect(config.maxPower >= 0).assertTrue();
    });

    it('testAccessoryStringDescriptors001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAccessoryStringDescriptors001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      console.info(TAG, `Accessory name: ${devices.name}`);
      console.info(TAG, `Acessory serial: ${devices.serial}`);
      
      let config = devices.configs[0];
      console.info(TAG, `Config name: ${config.name}`);
      
      for (let i = 0; i < config.interfaces.length; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `Interface ${i} name: ${iface.name}`);
      }
    });

    it('testAccessoryVendorSpecific001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAccessoryVendorSpecific001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      console.info(TAG, `Accessory vendorId: 0x${devices.vendorId.toString(16)}`);
      console.info(TAG, `Accessory productId: 0x${devices.productId.toString(16)}`);
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      console.info(TAG, `Interface class: 0x${iface.clazz.toString(16)}`);
      console.info(TAG, `Interface subclass: 0x${iface.subClass.toString(16)}`);
      console.info(TAG, `Interface protocol: 0x${iface.protocol.toString(16)}`);
    });

    it('testAccessoryMultipleConfigs001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAccessoryMultipleConfigs001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let configCount = devices.configs.length;
      console.info(TAG, `Accessory configurations count: ${configCount}`);
      
      for (let i = 0; i < configCount; i++) {
        let config = devices.configs[i];
        console.info(TAG, `Config ${i}: id=${config.id}, interfaces=${config.interfaces.length}`);
      }
    });

    it('testAccessoryReconnection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAccessoryReconnection001 enter');
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

    it('testAccessoryConcurrentAccess001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testAccessoryConcurrentAccess001 enter');
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

    it('testAccessoryErrorRecovery001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAccessoryErrorRecovery001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let closeResult = usbMgr.closePipe(gPipe);
      expect(closeResult).assertEqual(0);
      
      let gPipe2: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe2 !== null).assertTrue();
      
      let closeResult2 = usbMgr.closePipe(gPipe2);
      expect(closeResult2).assertEqual(0);
      
      console.info(TAG, 'Error recovery test completed');
    });
  })
}
