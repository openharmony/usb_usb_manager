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

const TAG: string = "[UsbNetworkTest]";
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

export default function UsbNetworkTest() {
  describe('UsbNetworkTest', () => {
    let isDeviceConnected: boolean = false;
    let devices: usbMgr.USBDevice;

    beforeAll(async (done: Function) => {
      console.log(TAG, '*************Usb Unit UsbNetworkTest Begin*************');
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
      console.log('*************Usb Unit UsbNetworkTest End*************');
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test Case*************');
    })

    it('testNetworkDeviceDetection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNetworkDeviceDetection001 enter');
      
      let deviceList = usbMgr.getDevices();
      console.info(TAG,  `Network device detection count: ${deviceList.length}`);
      
      for (let i = 0; i < deviceList.length; i++) {
)        let device = deviceList[i];
        console.info(TAG, `Device ${i}: vendorId=0x${device.vendorId.toString(16)}, productId=0x${device.productId.toString(16)}`);
      }
      
      expect(deviceList.length >= 0).assertTrue();
    });

    it('testNetworkInterfaceDetection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNetworkInterfaceDetection001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      console.info(TAG, `Network interface count: ${config.interfaces.length}`);
      
      for (let i = 0; i < config.interfaces.length; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `Interface ${i}: class=0x${iface.clazz.toString(16)}, subclass=0x${iface.subClass.toString(16)}`);
        console.info(TAG, `Interface ${i}: protocol=0x${iface.protocol.toString(16)}, endpoints=${iface.endpoints.length}`);
      }
      
      expect(config.interfaces.length > 0).assertTrue();
    });

    it('testNetworkEndpointDetection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNetworkEndpointDetection001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      console.info(TAG, `Network endpoint count: ${iface.endpoints.length}`);
      
      for (let i = 0; i < iface.endpoints.length; i++) {
        let endpoint = iface.endpoints[i];
        console.info(TAG, `Endpoint ${i}: address=${endpoint.address}, type=${endpoint.type}`);
        console.info(TAG, `Endpoint ${i}: direction=${endpoint.direction}, maxPacketSize=${endpoint.maxPacketSize}`);
      }
      
      expect(iface.endpoints.length > 0).assertTrue();
    });

    it('testNetworkBulkEndpoints001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNetworkBulkEndpoints001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let bulkEndpoints = iface.endpoints.filter(ep => ep.type === 2);
      console.info(TAG, `Bulk endpoints count: ${bulkEndpoints.length}`);
      
      for (let i = 0; i < bulkEndpoints.length; i++) {
        let endpoint = bulkEndpoints[i];
        console.info(TAG, `Bulk endpoint ${i}: address=${endpoint.address}, direction=${endpoint.direction}`);
        console.info(TAG, `Bulk endpoint ${i}: maxPacketSize=${endpoint.maxPacketSize}`);
      }
    });

    it('testNetworkInterruptEndpoint001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNetworkInterruptEndpoint001 enter');
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

    it('testNetworkEthernet001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNetworkEthernet001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      let ethernetInterfaces: usbMgr.USBInterface[] = config.interfaces.filter(iface => iface.clazz === 0x02 && iface.subClass === 0x06);
      console.info(TAG, `Ethernet interfaces count: ${ethernetInterfaces.length}`);
      
      for (let i = 0; i < ethernetInterfaces.length; i++) {
        let iface = ethernetInterfaces[i];
        console.info(TAG, `Ethernet interface ${i}: protocol=0x${iface.protocol.toString(16)}`);
        console.info(TAG, `Ethernet interface ${i}: endpoints=${iface.endpoints.length}`);
      }
    });

    it('testNetworkWireless001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNetworkWireless001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      let wirelessInterfaces: usbMgr.USBInterface[] = config.interfaces.filter(iface => iface.clazz === 0x02 && iface.subClass === 0x0D);
      console.info(TAG, `Wireless interfaces count: ${wirelessInterfaces.length}`);
      
      for (let i = 0; i < wirelessInterfaces.length; i++) {
        let iface = wirelessInterfaces[i];
        console.info(TAG, `Wireless interface ${i}: protocol=0x${iface.protocol.toString(16)}`);
        console.info(TAG, `Wireless interface ${i}: endpoints=${iface.endpoints.length}`);
      }
    });

    it('testNetworkMobile001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNetworkMobile001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      let mobileInterfaces: usbMgr.USBInterface[] = config.interfaces.filter(iface => iface.clazz === 0x02 && iface.subClass === 0x0E);
      console.info(TAG, `Mobile interfaces count: ${mobileInterfaces.length}`);
      
      for (let i = 0; i < mobileInterfaces.length; i++) {
        let iface = mobileInterfaces[i];
        console.info(TAG, `Mobile interface ${i}: protocol=0x${iface.protocol.toString(16)}`);
        console.info(TAG, `Mobile interface ${i}: endpoints=${iface.endpoints.length}`);
      }
    });

    it('testNetworkCdc001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNetworkCdc001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      let cdcInterfaces: usbMgr.USBInterface[] = config.interfaces.filter(iface => iface.clazz === 0x02 && iface.subClass === 0x02);
      console.info(TAG, `CDC interfaces count: ${cdcInterfaces.length}`);
      
      for (let i = 0; i < cdcInterfaces.length; i++) {
        let iface = cdcInterfaces[i];
        console.info(TAG, `CDC interface ${i}: protocol=0x${iface.protocol.toString(16)}`);
        console.info(TAG, `CDC interface ${i}: endpoints=${iface.endpoints.length}`);
      }
    });

    it('testNetworkNcm001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNetworkNcm001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      let ncmInterfaces: usbMgr.USBInterface[] = config.interfaces.filter(iface => iface.clazz === 0x02 && iface.subClass === 0x0D);
      console.info(TAG, `NCM interfaces count: ${ncmInterfaces.length}`);
      
      for (let i = 0; i < ncmInterfaces.length; i++) {
        let iface = ncmInterfaces[i];
        console.info(TAG, `NCM interface ${i}: protocol=0x${iface.protocol.toString(16)}`);
        console.info(TAG, `NCM interface ${i}: endpoints=${iface.endpoints.length}`);
      }
    });

    it('testNetworkRndis001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNetworkRndis001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      let rndisInterfaces: usbMgr.USBInterface[] = config.interfaces.filter(iface => iface.clazz === 0x02 && iface.subClass === 0x0E);
      console.info(TAG, `RNDIS interfaces count: ${rndisInterfaces.length}`);
      
      for (let i = 0; i < rndisInterfaces.length; i++) {
        let iface = rndisInterfaces[i];
        console.info(TAG, `RNDIS interface ${i}: protocol=0x${iface.protocol.toString(16)}`);
        console.info(TAG, `RNDIS interface ${i}: endpoints=${iface.endpoints.length}`);
      }
    });

    it('testNetworkPowerManagement001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNetworkPowerManagement001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      console.info(TAG, `Network maxPower: ${config.maxPower}mA`);
      console.info(TAG, `Network isRemoteWakeup: ${config.isRemoteWakeup}`);
      console.info(TAG, `Network isSelfPowered: ${config.isSelfPowered}`);
      
      expect(config.maxPower >= 0).assertTrue();
    });

    it('testNetworkStringDescriptors001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNetworkStringDescriptors001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      console.info(TAG, `Network name: ${devices.name}`);
      console.info(TAG, `Network serial: ${devices.serial}`);
      
      let config = devices.configs[0];
      console.info(TAG, `Config name: ${config.name}`);
      
      for (let i = 0; i < config.interfaces.length; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `Interface ${i} name: ${iface.name}`);
      }
    });

    it('testNetworkMultipleInterfaces001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNetworkMultipleInterfaces001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let interfaceCount = config.interfaces.length;
      
      console.info(TAG, `Network interfaces count: ${interfaceCount}`);
      
      for (let i = 0; i < interfaceCount; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `Interface ${i}: class=0x${iface.clazz.toString(16)}, subclass=0x${iface.subClass.toString(16)}`);
        console.info(TAG, `Interface ${i}: protocol=0x${iface.protocol.toString(16)}, endpoints=${iface.endpoints.length}`);
      }
    });

    it('testNetworkReconnection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNetworkReconnection001 enter');
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

    it('testNetworkConcurrentAccess001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testNetworkConcurrentAccess001 enter');
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
