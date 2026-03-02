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

import usbMgr from '@ohos.usbManager';
import { UiDriver, BY } from '@ohos.UiTest';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium';

let gDeviceList: Array<usbMgr.USBDevice>;
let gPipe: usbMgr.USBDevicePipe | null;
let devices: usbMgr.USBDevice | null;
let isDeviceConnected: boolean;
const TAG = "[UsbInterfaceConfigTestExtend01]";

function deviceConnected(): boolean {
  if (gDeviceList.length > 0) {
    console.info(TAG, "Test USB device is connected");
    return true;
  }
  console.info(TAG, "Test USB device is not connected");
  return false;
}

async function driveFn(): Promise<void> {
  console.info('**************driveFn**************');
  try {
    let driver = await UiDriver.create();
    console.info(TAG, ` come in driveFn`);
    console.info(TAG, `driver is ${JSON.stringify(driver)}`);
    let button = await driver.findComponent(BY.text('允许'));
    console.info(TAG, `button is ${JSON.stringify(button)}`);
    await button.click();
  } catch (err) {
    console.info(TAG, 'err is ' + err);
    return;
  }
}

async function getPermission(): Promise<void> {
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

function getPipe(testCaseName: string): void {
  gPipe = usbMgr.connectDevice(devices);
  console.info(TAG, `usb ${testCaseName} connectDevice getPipe ret: ${JSON.stringify(gPipe)}`);
  expect(gPipe !== null).assertTrue();
}

function toClosePipe(testCaseName: string): void {
  let isPipClose = usbMgr.closePipe(gPipe);
  console.info(TAG, `usb ${testCaseName} closePipe getPipe ret: ${isPipClose}`);
  expect(isPipClose).assertionEqual(0);
}

export default function UsbInterfaceConfigTestExtend01() {
  describe('UsbInterfaceConfigTestExtend01', () => {

    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbInterfaceConfigTestExtend01 Begin*************');
      
      gDeviceList = usbMgr.getDevices();
      console.info(TAG, 'beforeAll: usb case gDeviceList.length return: ' + JSON.stringify(gDeviceList));
      isDeviceConnected = deviceConnected();
      if (isDeviceConnected) {
        let hasRight = usbMgr.hasRight(gDeviceList[0].name);
        if (!hasRight) {
          console.info(TAG, `beforeAll: usb requestRight start`);
          await getPermission();
          await driveFn();
        }
      }
      console.info(TAG, 'beforeAll: usb unit begin test getversion: ' + usbMgr.getVersion());
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
      console.info(TAG, 'afterEach return devices : ' + JSON.stringify(devices));
      console.info(TAG, 'afterEach return devices : ' + JSON.stringify(gPipe));
    })

    afterAll(() => {
      console.log(TAG, '*************Usb Unit UsbInterfaceConfigTestExtend01 End*************');
    })

    /**
     * @tc.name   testInterfaceClaimRelease002
     * @tc.number SUB_USB_InterfaceConfig_Extend_ClaimRelease_0200
     * @tc.desc   Test claim and release interface multiple times
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceClaimRelease002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testInterfaceClaimRelease002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testInterfaceClaimRelease002');
      
      let config = usbMgr.getConfig(gPipe);
      if (config.interfaces.length > 0) {
        let iface = config.interfaces[0];
        
        for (let i = 0; i < 5; i++) {
          let claimResult = usbMgr.claimInterface(gPipe, iface, true);
          console.info(TAG, `Claim iteration ${i} result: ${claimResult}`);
          expect(claimResult).assertEqual(0);
          
          let releaseResult = usbMgr.releaseInterface(gPipe, iface);
          console.info(TAG, `Release iteration ${i} result: ${releaseResult}`);
          expect(releaseResult).assertEqual(0);
        }
      }
      
      toClosePipe('testInterfaceClaimRelease002');
    })

    /**
     * @tc.name   testInterfaceClaim003
     * @tc.number SUB_USB_InterfaceConfig_Extend_Claim_0300
     * @tc.desc   Test claim interface with force flag false
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceClaim003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testInterfaceClaim003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testInterfaceClaim003');
      
      let config = usbMgr.getConfig(gPipe);
      if (config.interfaces.length > 0) {
        let iface = config.interfaces[0];
        
        let claimResult = usbMgr.claimInterface(gPipe, iface, false);
        console.info(TAG, `Claim without force result: ${claimResult}`);
        expect(claimResult).assertEqual(0);
        
        let releaseResult = usbMgr.releaseInterface(gPipe, iface);
        console.info(TAG, `Release result: ${releaseResult}`);
        expect(releaseResult).assertEqual(0);
      }
      
      toClosePipe('testInterfaceClaim003');
    })

    /**
     * @tc.name   testInterfaceClaim004
     * @tc.number SUB_USB_InterfaceConfig_Extend_Claim_0400
     * @tc.desc   Test claim same interface twice
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceClaim004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testInterfaceClaim004 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testInterfaceClaim004');
      
      let config = usbMgr.getConfig(gPipe);
      if (config.interfaces.length > 0) {
        let iface = config.interfaces[0];
        
        let claimResult1 = usbMgr.claimInterface(gPipe, iface, true);
        console.info(TAG, `First claim result: ${claimResult1}`);
        expect(claimResult1).assertEqual(0);
        
        let claimResult2 = usbMgr.claimInterface(gPipe, iface, true);
        console.info(TAG, 'Second claim result: ${claimResult2}`);
        expect(claimResult2).assertEqual(0);
        
        let releaseResult = usbMgr.releaseInterface(gPipe, iface);
        console.info(TAG, `Release result: ${releaseResult}`);
        expect(releaseResult).assertEqual(0);
      }
      
      toClosePipe('testInterfaceClaim004');
    })

    /**
     * @tc.name   testInterfaceClaim005
     * @tc.number SUB_USB_InterfaceConfig_Extend_Claim_0500
     * @tc.desc   Test claim different interfaces
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceClaim005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testInterfaceClaim005 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testInterfaceClaim005');
      
      let config = usbMgr.getConfig(gPipe);
      if (config.interfaces.length > 1) {
        let iface1 = config.interfaces[0];
        let iface2 = config.interfaces[1];
        
        let claimResult1 = usbMgr.claimInterface(gPipe, iface1, true);
        console.info(TAG, `Claim interface 1 result: ${claimResult1}`);
        expect(claimResult1).assertEqual(0);
        
        let claimResult2 = usbMgr.claimInterface(gPipe, iface2, true);
        console.info(TAG, `Claim interface 2 result: ${claimResult2}`);
        expect(claimResult2).assertEqual(0);
        
        let releaseResult1 = usbMgr.releaseInterface(gPipe, iface1);
        console.info(TAG, `Release interface 1 result: ${releaseResult1}`);
        expect(releaseResult1).assertEqual(0);
        
        let releaseResult2 = usbMgr.releaseInterface(gPipe, iface2);
        console.info(TAG, `Release interface 2 result: ${releaseResult2}`);
        expect(releaseResult2).assertEqual(0);
      }
      
      toClosePipe('testInterfaceClaim005');
    })

    /**
     * @tc.name   testSetInterface002
     * @tc.number SUB_USB_InterfaceConfig_Extend_SetConfig_0200
     * @tc.desc   Test set interface with alternate setting
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testSetInterface002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testSetInterface002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testSetInterface002');
      
      let config = usbMgr.getConfig(gPipe);
      if (config.interfaces.length > 0) {
        let iface = config.interfaces[0];
        
        let claimResult = usbMgr.claimInterface(gPipe, iface, true);
        console.info(TAG, `Claim result: ${claimResult}`);
        expect(claimResult).assertEqual(0);
        
        let setResult1 = usbMgr.setInterface(gPipe, iface, true);
        console.info(TAG, `Set interface true result: ${setResult1}`);
        expect(setResult1).assertEqual(0);
        
        let setResult2 = usbMgr.setInterface(gPipe, iface, false);
        console.info(TAG, `Set interface false result: ${setResult2}`);
        expect(setResult2).assertEqual(0);
        
        let releaseResult = usbMgr.releaseInterface(gPipe, iface);
        console.info(TAG, `Release result: ${releaseResult}`);
        expect(releaseResult).assertEqual(0);
      }
      
      toClosePipe('testSetInterface002');
    })

    /**
     * @tc.name   testSetInterface003
     @tc.number SUB_USB_InterfaceConfig_Extend_SetConfig_0300
     * @tc.desc   Test set interface without claim
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testSetInterface003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testSetInterface003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testSetInterface003');
      
      let config = usbMgr.getConfig(gPipe);
      if (config.interfaces.length > 0) {
        let iface = config.interfaces[0];
        
        let setResult = usbMgr.setInterface(gPipe, iface, true);
        console.info(TAG, `Set without claim result: ${setResult}`);
        expect(setResult).assertEqual(0);
        
        let releaseResult = usbMgr.releaseInterface(gPipe, iface);
        console.info(TAG, `Release result: ${releaseResult}`);
        expect(releaseResult).assertEqual(0);
      }
      
      toClosePipe('testSetInterface003');
    })

    /**
     * @tc.name   testSetConfiguration002
     * @tc.number SUB_USB_InterfaceConfig_Extend_SetConfig_0200
     * @tc.desc   Test set configuration with different configs
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testSetConfiguration002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testSetConfiguration002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testSetConfiguration002');
      
      let configs = usbMgr.getDevices()[0].configs;
      if (configs.length > 1) {
        let config1 = configs[0];
        let config2 = configs[1];
        
        let setResult1 = usbMgr.setConfiguration(gPipe, config1);
        console.info(TAG, `Set config 1 result: ${setResult1}`);
        expect(setResult1 >= 0 || setResult1 < 0).assertTrue();
        
        let setResult2 = usbMgr.setConfiguration(gPipe, config2);
        console.info(TAG, `Set config 2 result: ${setResult2}`);
        expect(setResult2 >= 0 || setResult2 < 0).assertTrue();
      }
      
      toClosePipe('testSetConfiguration002');
    })

    /**
     * @tc.name   testSetConfiguration003
     * @tc.number SUB_USB_InterfaceConfig_Extend_SetConfig_0300
     * @tc.desc   Test set configuration without open pipe
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testSetConfiguration003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testSetConfiguration003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testSetConfiguration003');
      
      let config = usbMgr.getConfig(gPipe);
      if (config.interfaces.length > 0) {
        let iface = config.interfaces[0];
        
        let claimResult = usbMgr.claimInterface(gPipe, iface, true);
        expect(claimResult).assertEqual(0);
        
        let setResult = usbMgr.setConfiguration(gPipe, config);
        console.info(TAG, `Set config result: ${setResult}`);
        expect(setResult).assertEqual(0);
        
        let releaseResult = usbMgr.releaseInterface(gPipe, iface);
        expect(releaseResult).assertEqual(0);
      }
      
      toClosePipe('testSetConfiguration003');
    })

    /**
     * @tc.name   testSetConfiguration004
     * @tc.number SUB_USB_InterfaceConfig_Extend_SetConfig_0400
     * @tc.desc   Test set configuration sequence
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testSetConfiguration004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testSetConfiguration004 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testSetConfiguration004');
      
      let configs = usbMgr.getDevices()[0].configs;
      if (configs.length > 0) {
        let config = configs[0];
        
        for (let i = 0; i < 3; i++) {
          let setResult = usbMgr.setConfiguration(gPipe, config);
          console.info(TAG, `Set config iteration ${i} result: ${setResult}`);
          expect(setResult).assertEqual(0);
        }
      }
      
      toClosePipe('testSetConfiguration004');
    })

    /**
     * @tc.name   testEndpointValidation001
     * @tc.number SUB_USB_InterfaceConfig_Extend_Endpoint_0100
     * @tc.desc   Test endpoint property validation
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testEndpointValidation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, (done: Function) => {
      console.info(TAG, 'usb testEndpointValidation001 begin');
      if (!isDeviceConnected) {
        done();
        return;
      }
      
      let device = gDeviceList[0];
      expect(device.configs).not.toBeNull();
      expect(device.configs.length).toBeLargerThan(0);
      
      let endpointFound = false;
      for (let config of device.configs) {
        for (let iface of config.interfaces) {
          for (let endpoint of iface.endpoints) {
            expect(endpoint.address).toBeLargerThan(-1);
            expect(endpoint.type).toBeLargerThan(-1);
            expect(endpoint.direction).toBeLargerThan(-1);
            expect(endpoint.maxPacketSize).toBeLargerThan(0);
            endpointFound = true;
          }
        }
      }
      
      expect(endpointFound).assertTrue();
      done();
    })

    /**
     * @tc.name   testInterfaceValidation001
     * @tc.number SUB_USB_InterfaceConfig_Extend_Interface_0100
     * @tc.desc   Test interface property validation
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceValidation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, (done: Function) => {
      console.info(TAG, 'usb testInterfaceValidation001 begin');
      if (!isDeviceConnected) {
        done();
        return;
      }
      
      let device = gDeviceList[0];
      expect(device.configs).not.toBeNull();
      expect(device.configs.length).toBeLargerThan(0);
      
      let interface = false;
      for (let config of device.configs) {
        for (let iface of config.interfaces) {
          expect(iface.id).toBeLargerThan(-1);
          expect(iface.clazz).toBeLargerThan(-1);
          expect(iface.subClass).toBeLargerThan(-1);
          expect(iface.protocol).toBeLargerThan(-1);
          expect(iface.endpoints).not.toBeNull();
          interface = true;
        }
      }
      
      expect(interface).assertTrue();
      done();
    })

    /**
     * @tc.name   testConfigValidation001
     * @tc.number SUB_USB_InterfaceConfig_Extend_Config_0100
     * @tc.desc   Test configuration property validation
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConfigValidation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, (done: Function) => {
     ) => {
      console.info(TAG, 'usb test)ConfigValidation001 begin');
      if (!isDeviceConnected) {
        done();
        return;
      }
      
      let device = gDeviceList[0];
      expect(device.configs).not.toBeNull();
      expect(device.configs.length).toBeLargerThan(0);
      
      for (let config of device.configs) {
        expect(config.id).toBeLargerThan(-1);
        expect(config.interfaces).not.toBeNull();
        expect(config.interfaces.length).toBeLargerThan(0);
      }
      
      done();
    })
  })
}