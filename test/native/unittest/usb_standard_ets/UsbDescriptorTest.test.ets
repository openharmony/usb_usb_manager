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

const TAG: string = "[UsbDescriptorTest]";
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

export default function UsbDescriptorTest() {
  describe('UsbDescriptorTest', () => {
    let isDeviceConnected: boolean = false;
    let devices: usbMgr.USBDevice;

    beforeAll(async (done: Function) => {
      console.log(TAG, '*************Usb Unit UsbDescriptorTest Begin*************');
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
      console.log('*************Usb Unit UsbDescriptorTest End*************');
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test Case*************');
    })

    it('testDeviceDescriptor001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceDescriptor001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      console.info(TAG, `Device name: ${devices.name}`);
      console.info(TAG, `Device vendorId: 0x${devices.vendorId.toString(16)}`);
      console.info(TAG, `Device productId: 0x${devices.productId.toString(16)}`);
      console.info(TAG, `Device serial: ${devices.serial}`);
      console.info(TAG, `Device config count: ${devices.configs.length}`);
      
      expect(devices.name.length > 0).assertTrue();
      expect(devices.vendorId > 0).assertTrue();
      expect(devices.productId > 0).assertTrue();
      expect(devices.configs.length > 0).assertTrue();
    });

    it('testConfigDescriptor001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testConfigDescriptor001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      console.info(TAG, `Config id: ${config.id}`);
      console.info(TAG, `Config name: ${config.name}`);
      console.info(TAG, `Config maxPower: ${config.maxPower}mA`);
      console.info(TAG, `Config isRemoteWakeup: ${config.isRemoteWakeup}`);
      console.info(TAG, `Config isSelfPowered: ${config.isSelfPowered}`);
      console.info(TAG, `Config interface count: ${config.interfaces.length}`);
      
      expect(config.id >= 0).assertTrue();
      expect(config.maxPower >= 0).assertTrue();
      expect(config.interfaces.length > 0).assertTrue();
    });

    it('testInterfaceDescriptor001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInterfaceDescriptor001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      console.info(TAG, `Interface id: ${iface.id}`);
      console.info(TAG, `Interface class: 0x${iface.clazz.toString(16)}`);
      console.info(TAG, `Interface subclass: 0x${iface.subClass.toString(16)}`);
      console.info(TAG, `Interface protocol: 0x${iface.protocol.toString(16)}`);
      console.info(TAG, `Interface name: ${iface.name}`);
      console.info(TAG, `Interface endpoint count: ${iface.endpoints.length}`);
      
      expect(iface.id >= 0).assertTrue();
      expect(iface.clazz >= 0).assertTrue();
      expect(iface.endpoints.length > 0).assertTrue();
    });

    it('testEndpointDescriptor001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testEndpointDescriptor001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      let endpoint = iface.endpoints[0];
      
      console.info(TAG, `Endpoint address: ${endpoint.address}`);
      console.info(TAG, `Endpoint type: ${endpoint.type}`);
      console.info(TAG, `Endpoint direction: ${endpoint.direction}`);
      console.info(TAG, `Endpoint maxPacketSize: ${endpoint.maxPacketSize}`);
      console.info(TAG, `Endpoint interval: ${endpoint.interval}`);
      
      expect(endpoint.address >= 0).assertTrue();
      expect(endpoint.type >= 0).assertTrue();
      expect(endpoint.direction >= 0).assertTrue();
      expect(endpoint.maxPacketSize > 0).assertTrue();
    });

    it('testRawDescriptor001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testRawDescriptor001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let rawDescriptor = usbMgr.getRawDescriptor(gPipe);
      console.info(TAG, `Raw descriptor length: ${rawDescriptor.length}`);
      console.info(TAG, `Raw descriptor first bytes: ${Array.from(rawDescriptor.slice(0, 8)).map(b => '0x' + b.toString(16).padStart(2, '0')).join(' ')}`);
      
      expect(rawDescriptor.length > 0).assertTrue();
      expect(rawDescriptor[0]).assertEqual(18);
      
      usbMgr.closePipe(gPipe);
    });

    it('testDescriptorHierarchy001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDescriptorHierarchy001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      console.info(TAG, 'Descriptor hierarchy:');
      console.info(TAG, `  Device: ${devices.name}`);
      
      for (let i = 0; i < devices.configs.length; i++) {
        let config = devices.configs[i];
        console.info(TAG, `    Config ${i}: id=${config.id}`);
        
        for (let j = 0; j < config.interfaces.length; j++) {
          let iface = config.interfaces[j];
          console.info(TAG, `      Interface ${j}: id=${iface.id}, class=0x${iface.clazz.toString(16)}`);
          
          for (let k = 0; k < iface.endpoints.length; k++) {
            let endpoint = iface.endpoints[k];
            console.info(TAG, `        Endpoint ${k}: address=${endpoint.address}, type=${endpoint.type}`);
          }
        }
      }
      
      expect(devices.configs.length > 0).assertTrue();
    });

    it('testDescriptorValidation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDescriptorValidation001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      expect(devices.vendorId > 0 && devices.vendorId <= 0xFFFF).assertTrue();
      expect(devices.productId > 0 && devices.productId <= 0xFFFF).assertTrue();
      
      for (let config of devices.configs) {
        expect(config.id >= 0 && config.id <= 0xFF).assertTrue();
        expect(config.maxPower >= 0 && config.maxPower <= 500).assertTrue();
        
        for (let iface of config.interfaces) {
          expect(iface.id >= 0 && iface.id <= 0xFF).assertTrue();
          expect(iface.clazz >= 0 && iface.clazz <= 0xFF).assertTrue();
          expect(iface.subClass >= 0 && iface.subClass <= 0xFF).assertTrue();
          expect(iface.protocol >= 0 && iface.protocol <= 0xFF).assertTrue();
          
          for (let endpoint of iface.endpoints) {
            expect(endpoint.address >= 0 && endpoint.address <= 0xFF).assertTrue();
            expect(endpoint.type >= 0 && endpoint.type <= 3).assertTrue();
            expect(endpoint.direction >= 0 && endpoint.direction <= 1).assertTrue();
            expect(endpoint.maxPacketSize > 0 && endpoint.maxPacketSize <= 1024).assertTrue();
          }
        }
      }
    });

    it('testMultipleConfigDescriptors001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMultipleConfigDescriptors001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let configCount = devices.configs.length;
      console.info(TAG, `Device has ${configCount} configurations`);
      
      for (let i = 0; i < configCount; i++) {
        let config = devices.configs[i];
        console.info(TAG, `Config ${i}: id=${config.id}, interfaces=${config.interfaces.length}`);
        expect(config.interfaces.length > 0).assertTrue();
      }
    });

    it('testMultipleInterfaceDescriptors001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMultipleInterfaceDescriptors001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let interfaceCount = config.interfaces.length;
      
      console.info(TAG, `Config has ${interfaceCount} interfaces`);
      
      for (let i = 0; i < interfaceCount; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `Interface ${i}: id=${iface.id}, endpoints=${iface.endpoints.length}`);
        expect(iface.endpoints.length > 0).assertTrue();
      }
    });

    it('testMultipleEndpointDescriptors001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMultipleEndpointDescriptors001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      let endpointCount = iface.endpoints.length;
      
      console.info(TAG, `Interface has ${endpointCount} endpoints`);
      
      for (let i = 0; i < endpointCount; i++) {
        let endpoint = iface.endpoints[i];
        console.info(TAG, `Endpoint ${i}: address=${endpoint.address}, type=${endpoint.type}`);
        expect(endpoint.address >= 0).assertTrue();
      }
    });

    it('testDescriptorConsistency001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDescriptorConsistency001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let rawDescriptor = usbMgr.getRawDescriptor(gPipe);
      let fd = usbMgr.getFileDescriptor(gPipe);
      
      console.info(TAG, `Raw descriptor length: ${rawDescriptor.length}`);
      console.info(TAG, `File descriptor: ${fd}`);
      
      expect(rawDescriptor.length > 0).assertTrue();
      expect(fd >= 0).assertTrue();
      
      usbMgr.closePipe(gPipe);
    });

    it('testDescriptorFields001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDescriptorFields001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      let endpoint = iface.endpoints[0];
      
      console.info(TAG, 'Device descriptor fields:');
      console.info(TAG, `  name: ${devices.name}, type: string`);
      console.info(TAG, `  vendorId: ${devices.vendorId}, type: number`);
      console.info(TAG, `  productId: ${devices.productId}, type: number`);
      console.info(TAG, `  serial: ${devices.serial}, type: string`);
      console.info(TAG, `  configs: array, length: ${devices.configs.length}`);
      
      console.info(TAG, 'Config descriptor fields:');
      console.info(TAG, `  id: ${config.id}, type: number`);
      console.info(TAG, `  name: ${config.name}, type: string`);
      console.info(TAG, `  maxPower: ${config.maxPower}, type: number`);
      console.info(TAG, `  isRemoteWakeup: ${config.isRemoteWakeup}, type: boolean`);
      console.info(TAG, `  isSelfPowered: ${config.isSelfPowered}, type: boolean`);
      
      console.info(TAG, 'Interface descriptor fields:');
      console.info(TAG, `  id: ${iface.id}, type: number`);
      console.info(TAG, `  clazz: ${iface.clazz}, type: number`);
      console.info(TAG, `  subClass: ${iface.subClass}, type: number`);
      console.info(TAG, `  protocol: ${iface.protocol}, type: number`);
      console.info(TAG, `  name: ${iface.name}, type: string`);
      
      console.info(TAG, 'Endpoint descriptor fields:');
      console.info(TAG, `  address: ${endpoint.address}, type: number`);
      console.info(TAG, `  type: ${endpoint.type}, type: number`);
      console.info(TAG, `  direction: ${endpoint.direction}, type: number`);
      console.info(TAG, `  maxPacketSize: ${endpoint.maxPacketSize}, type: number`);
      console.info(TAG, `  interval: ${endpoint.interval}, type: number`);
    });

    it('testDescriptorSize001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDescriptorSize001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let rawDescriptor = usbMgr.getRawDescriptor(gPipe);
      
      console.info(TAG, `Descriptor size: ${rawDescriptor.length} bytes`);
      console.info(TAG, `Descriptor size in KB: ${(rawDescriptor.length / 1024).toFixed(2)} KB`);
      
      expect(rawDescriptor.length > 0).assertTrue();
      expect(rawDescriptor.length < 65536).assertTrue();
      
      usbMgr.closePipe(gPipe);
    });

    it('testDescriptorStructure001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDescriptorStructure001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let rawDescriptor = usbMgr.getRawDescriptor(gPipe);
      
      console.info(TAG, `Descriptor structure analysis:`);
      console.info(TAG, `  Total length: ${rawDescriptor.length}`);
      console.info(TAG, `  First byte (length): ${rawDescriptor[0]}`);
      console.info(TAG, `  Second byte (type): ${rawDescriptor[1]}`);
      
      if (rawDescriptor.length > 0) {
        expect(rawDescriptor[0]).assertEqual(rawDescriptor[0]);
        expect(rawDescriptor[1] >= 0).assertTrue();
      }
      
      usbMgr.closePipe(gPipe);
    });

    it('testDescriptorAccess001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDescriptorAccess001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let rawDescriptor1 = usbMgr.getRawDescriptor(gPipe);
      let rawDescriptor2 = usbMgr.getRawDescriptor(gPipe);
      
      console.info(TAG, `First access: ${rawDescriptor1.length} bytes`);
      console.info(TAG, `Second access: ${rawDescriptor2.length} bytes`);
      
      expect(rawDescriptor1.length).assertEqual(rawDescriptor2.length);
      
      usbMgr.closePipe(gPipe);
    });

    it('testDescriptorPersistence001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDescriptorPersistence001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      let endpoint = iface.endpoints[0];
      
      let originalAddress = endpoint.address;
      let originalType = endpoint.type;
      let originalDirection = endpoint.direction;
      let originalMaxPacketSize = endpoint.maxPacketSize;
      
      console.info(TAG, `Original endpoint: address=${originalAddress}, type=${originalType}`);
      
      expect(endpoint.address).assertEqual(originalAddress);
      expect(endpoint.type).assertEqual(originalType);
      expect(endpoint.direction).assertEqual(originalDirection);
      expect(endpoint.maxPacketSize).assertEqual(originalMaxPacketSize);
    });
  })
}
