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

const TAG: string = "[UsbConfigurationTest]";
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

export default function UsbConfigurationTest() {
  describe('UsbConfigurationTest', () => {
    let isDeviceConnected: boolean = false;
    let devices: usbMgr.USBDevice;

    beforeAll(async (done: Function) => {
      console.log(TAG, '*************Usb Unit UsbConfigurationTest Begin*************');
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
      console.log('*************Usb Unit UsbConfigurationTest End*************');
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test Case*************');
    })

    it('testGetConfiguration001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testGetConfiguration001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = usbMgr.getConfiguration(gPipe);
      console.info(TAG, `Current configuration: ${JSON.stringify(config)}`);
      expect(config !== null).assertTrue();
      
      usbMgr.closePipe(gPipe);
    });

    it('testSetConfiguration001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testSetConfiguration001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      console.info(TAG, `Setting configuration: ${config.id}`);
      
      let result = usbMgr.setConfiguration(gPipe, config);
      console.info(TAG, `Set configuration result: ${result}`);
      expect(result).assertEqual(0);
      
      usbMgr.closePipe(gPipe);
    });

    it('testConfigurationProperties001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testConfigurationProperties001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      console.info(TAG, `Configuration ID: ${config.id}`);
      console.info(TAG, `Configuration name: ${config.name}`);
      console.info(TAG, `Configuration maxPower: ${config.maxPower}`);
      console.info(TAG, `Configuration isRemoteWakeup: ${config.isRemoteWakeup}`);
      console.info(TAG, `Configuration isSelfPowered: ${config.isSelfPowered}`);
      console.info(TAG, `Configuration interfaces count: ${config.interfaces.length}`);
      
      expect(config.id >= 0).assertTrue();
      expect(config.maxPower >= 0).assertTrue();
      expect(config.interfaces.length > 0).assertTrue();
    });

    it('testMultipleConfigurations001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMultipleConfigurations001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let configCount = devices.configs.length;
      console.info(TAG, `Device has ${configCount} configurations`);
      
      for (let i = 0; i < configCount; i++) {
        let config = devices.configs[i];
        console.info(TAG, `Configuration ${i}: ID=${config.id}, interfaces=${config.interfaces.length}`);
        expect(config.interfaces.length > 0).assertTrue();
      }
    });

    it('testConfigurationSwitch001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testConfigurationSwitch001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      if (devices.configs.length < 2) {
        console.info(TAG, 'Device has only one configuration, skipping switch test');
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config1 = devices.configs[0];
      let result1 = usbMgr.setConfiguration(gPipe, config1);
      console.info(TAG, `Set config ${config1.id} result: ${result1}`);
      expect(result1).assertEqual(0);
      
      let config2 = devices.configs[1];
      let result2 = usbMgr.setConfiguration(gPipe, config2);
      console.info(TAG, `Set config ${config2.id} result: ${result2}`);
      expect(result2).assertEqual(0);
      
      let result3 = usbMgr.setConfiguration(gPipe, config1);
      console.info(TAG, `Switch back to config ${config1.id} result: ${result3}`);
      expect(result3).assertEqual(0);
      
      usbMgr.closePipe(gPipe);
    });

    it('testInterfaceClaim001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInterfaceClaim001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let result = usbMgr.claimInterface(gPipe, iface, true);
      console.info(TAG, `Claim interface result: ${result}`);
      expect(result).assertEqual(0);
      
      let releaseResult = usbMgr.releaseInterface(gPipe, iface);
      console.info(TAG, `Release interface result: ${releaseResult}`);
      expect(releaseResult).assertEqual(0);
      
      usbMgr.closePipe(gPipe);
    });

    it('testInterfaceRelease001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInterfaceRelease001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let claimResult = usbMgr.claimInterface(gPipe, iface, true);
      expect(claimResult).assertEqual(0);
      
      let releaseResult = usbMgr.releaseInterface(gPipe, iface);
      console.info(TAG, `Release interface result: ${releaseResult}`);
      expect(releaseResult).assertEqual(0);
      
      usbMgr.closePipe(gPipe);
    });

    it('testSetInterface001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testSetInterface001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let claimResult = usbMgr.claimInterface(gPipe, iface, true);
      expect(claimResult).assertEqual(0);
      
      let setResult = usbMgr.setInterface(gPipe, iface);
      console.info(TAG, `Set interface result: ${setResult}`);
      expect(setResult).assertEqual(0);
      
      let releaseResult = usbMgr.releaseInterface(gPipe, iface);
      expect(releaseResult).assertEqual(0);
      
      usbMgr.closePipe(gPipe);
    });

    it('testInterfaceProperties001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInterfaceProperties001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      console.info(TAG, `Interface ID: ${iface.id}`);
      console.info(TAG, `Interface class: ${iface.clazz}`);
      console.info(TAG, `Interface subclass: ${iface.subClass}`);
      console.info(TAG, `Interface protocol: ${iface.protocol}`);
      console.info(TAG, `Interface name: ${iface.name}`);
      console.info(TAG, `Interface endpoints count: ${iface.endpoints.length}`);
      
      expect(iface.id >= 0).assertTrue();
      expect(iface.clazz >= 0).assertTrue();
      expect(iface.subClass >= 0).assertTrue();
      expect(iface.protocol >= 0).assertTrue();
      expect(iface.endpoints.length > 0).assertTrue();
    });

    it('testMultipleInterfaces001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMultipleInterfaces001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let interfaceCount = config.interfaces.length;
      
      console.info(TAG, `Configuration has ${interfaceCount} interfaces`);
      
      for (let i = 0; i < interfaceCount; i++) {
        let iface = config.interfaces[i];
        let result = usbMgr.claimInterface(gPipe, iface, true);
        console.info(TAG, `Claim interface ${iface.id} result: ${result}`);
        expect(result).assertEqual(0);
        
        let releaseResult = usbMgr.releaseInterface(gPipe, iface);
        console.info(TAG, `Release interface ${iface.id} result: ${releaseResult}`);
        expect(releaseResult).assertEqual(0);
      }
      
      usbMgr.closePipe(gPipe);
    });

    it('testInterfaceEndpoints001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInterfaceEndpoints001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
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

    it('testEndpointProperties001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testEndpointProperties001 enter');
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
      expect(endpoint.interval >= 0).assertTrue();
    });

    it('testBulkEndpoint001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testBulkEndpoint001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let bulkEndpoints = iface.endpoints.filter(ep => ep.type === 2);
      console.info(TAG, `Found ${bulkEndpoints.length} bulk endpoints`);
      
      for (let i = 0; i < bulkEndpoints.length; i++) {
        let endpoint = bulkEndpoints[i];
        console.info(TAG, `Bulk endpoint ${i}: address=${endpoint.address}, direction=${endpoint.direction}`);
        expect(endpoint.address >= 0).assertTrue();
      }
    });

    it('testInterruptEndpoint001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInterruptEndpoint001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let interruptEndpoints = iface.endpoints.filter(ep => ep.type === 3);
      console.info(TAG, `Found ${interruptEndpoints.length} interrupt endpoints`);
      
      for (let i = 0; i < interruptEndpoints.length; i++) {
        let endpoint = interruptEndpoints[i];
        console.info(TAG, `Interrupt endpoint ${i}: address=${endpoint.address}, direction=${endpoint.direction}`);
        expect(endpoint.address >= 0).assertTrue();
      }
    });

    it('testIsochronousEndpoint001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testIsochronousEndpoint001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let isochronousEndpoints = iface.endpoints.filter(ep => ep.type === 1);
      console.info(TAG, `Found ${isochronousEndpoints.length} isochronous endpoints`);
      
      for (let i = 0; i < isochronousEndpoints.length; i++) {
        let endpoint = isochronousEndpoints[i];
        console.info(TAG, `Isochronous endpoint ${i}: address=${endpoint.address}, direction=${endpoint.direction}`);
        expect(endpoint.address >= 0).assertTrue();
      }
    });

    it('testConfigurationLifecycle001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testConfigurationLifecycle001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let setResult = usbMgr.setConfiguration(gPipe, config);
      expect(setResult).assertEqual(0);
      
      let claimResult = usbMgr.claimInterface(gPipe, iface, true);
      expect(claimResult).assertEqual(0);
      
      let releaseResult = usbMgr.releaseInterface(gPipe, iface);
      expect(releaseResult).assertEqual(0);
      
      usbMgr.closePipe(gPipe);
    });

    it('testAlternateInterface001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAlternateInterface001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let claimResult = usbMgr.claimInterface(gPipe, iface, true);
      expect(claimResult).assertEqual(0);
      
      let setResult = usbMgr.setInterface(gPipe, iface);
      console.info(TAG, `Set interface result: ${setResult}`);
      
      let releaseResult = usbMgr.releaseInterface(gPipe, iface);
      expect(releaseResult).assertEqual(0);
      
      usbMgr.closePipe(gPipe);
    });
  })
}
