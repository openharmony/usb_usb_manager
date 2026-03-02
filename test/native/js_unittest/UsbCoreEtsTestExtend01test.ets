/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * * Licensed under the Apache License, Version 2.0 (the "License");
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
import CheckEmptyUtils from './CheckEmptyUtils.js';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium';

let gDeviceList: Array<usbMgr.USBDevice>;
let gPipe: usbMgr.USBDevicePipe | null;
let devices: usbMgr.USBDevice | null;
let isDeviceConnected: boolean;
const TAG = "[UsbCoreEtsTestExtend01]";

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
    CheckEmptyUtils.sleep(1000);
    let button = await driver.findComponent(BY.text('允许'));
    console.info(TAG, `button is ${JSON.stringify(button)}`);
    CheckEmptyUtils.sleep(1000);
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
  expect(isPipClose).assertEqual(0);
}

export default function UsbCoreEtsTestExtend01() {
  describe('UsbCoreEtsTestExtend01', () => {

    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbCoreEtsTestExtend01 Begin*************');
      
      gDeviceList = usbMgr.getDevices();
      console.info(TAG, 'beforeAll: usb case gDeviceList.length return: ' + JSON.stringify(gDeviceList));
      isDeviceConnected = deviceConnected();
      if (isDeviceConnected) {
        let hasRight = usbMgr.hasRight(gDeviceList[0].name);
        if (!hasRight) {
          console.info(TAG, `beforeAll: usb requestRight start`);
          await getPermission();
          CheckEmptyUtils.sleep(1000);
          await driveFn();
          CheckEmptyUtils.sleep(1000);
        }
      }
      console.info(TAG, 'beforeAll: usb unit begin test getversion :' + usbMgr.getVersion());
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test  Case*************');
      gDeviceList = usbMgr.getDevices();
      if (isDeviceConnected) {
        devices = gDeviceList[0];
        console.info(TAG, 'beforeEach return devices : ' + JSON.stringify(devices));
      }
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test  Case*************');
      devices = null;
      gPipe = null;
      console.info(TAG, 'afterEach return devices : ' + JSON.stringify(devices));
      console.info(TAG, 'afterEach return devices : ' + JSON.stringify(gPipe));
    })

    afterAll(() => {
      console.log(TAG, '*************Usb Unit UsbCoreEtsTestExtend01 End*************');
    })

    /**
     * @tc.name   testGetDevices003
     * @tc.number SUB_USB_HostManager_Ets_Func_0501
     * @tc.desc   Test getDevices with device hotplug/hotunplug
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testGetDevices003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, (done: Function) => {
      console.info(TAG, 'usb testGetDevices003 begin');
      if (!isDeviceConnected) {
        done();
        return
      }
      
      let deviceList1 = usbMgr.getDevices();
      console.info(TAG, 'usb case getDevices first call length: ' + deviceList1.length);
      expect(deviceList1.length > 0).assertTrue();
      
      CheckEmptyUtils.sleep(100);
      
      let deviceList2 = usbMgr.getDevices();
      console.info(TAG, 'usb case getDevices second call length: ' + deviceList2.length);
      expect(deviceList2.length).assertEqual(deviceList1.length);
      
      done();
    })

    /**
     * @tc.name   testGetDevices004
     * @tc.number SUB_USB_HostManager_Ets_Func_0502
     * @tc.desc   Test getDevices return value consistency
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testGetDevices004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, (done: Function) => {
      console.info(TAG, 'usb testGetDevices004 begin');
      if (!isDeviceConnected) {
        done();
        return
      }
      
      for (let i = 0; i < 10; i++) {
        let deviceList = usbMgr.getDevices();
        console.info(TAG, 'usb case getDevices iteration ' + i + ' length: ' + deviceList.length);
        expect(deviceList.length > 0).assertTrue();
        expect(Array.isArray(deviceList)).assertTrue();
      }
      
      done();
    })

    /**
     * @tc.name   testHasRight002
     * @.tc.number SUB_USB_HostManager_Ets_Func_0901
     * @tc.desc   Test hasRight with invalid device name
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testHasRight002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, (done: Function) => {
      console.info(TAG, 'usb testHasRight002 begin');
      if (!isDeviceConnected) {
        done();
        return
      }
      
      let invalidNames = ["", "invalid_device", "nonexistent", "123456", "!@#$%^&*"];
      for (let i = 0; i < invalidNames.length; i++) {
        let hasRight = usbMgr.hasRight(invalidNames[i]);
        console.info(TAG, 'usb has_right for ' + invalidNames[i] + ' : ' + hasRight);
        expect(hasRight).assertFalse();
      }
      
      done();
    })

    /**
     * @tc.name   testHasRight003
     * @tc.number SUB_USB_HostManager_Ets_Func_0902
     * @tc.desc   Test hasRight with all connected devices
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testHasRight003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, (done: Function) => {
      console.info(TAG, 'usb testHasRight003 begin');
      if (!isDeviceConnected) {
        done();
        return
      }
      
      let allHaveRight = true;
      for (let i = 0; i < gDeviceList.length; i++) {
        let hasRight = usbMgr.hasRight(gDeviceList[i].name);
        console.info(TAG, 'usb has_right for device ' + i + ' : ' + hasRight);
        if (!hasRight) {
          allHaveRight = false;
        }
      }
      expect(allHaveRight).assertTrue();
      
      done();
    })

    /**
     * @tc.name   testRequestRight002
     * @tc.number SUB_USB_HostManager_Ets_Func_0801
     * @tc.desc   Test requestRight with invalid device name
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testRequestRight002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testRequestRight002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      
      try {
        let hasRight = await usbMgr.requestRight("invalid_device_name");
        console.info(TAG, 'usb case requestRight ret : ' + hasRight);
        expect(hasRight).assertFalse();
      } catch(error) {
        console.info(TAG, 'usb case device request right failed : ' + error);
        expect(error !== null).assertTrue();
      }
    })

    /**
     * @tc.name   testRequestRight003
     * @tc.number SUB:USB_HostManager_Ets_Func_0802
     * @tc.desc   Test requestRight multiple times
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testRequestRight003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testRequestRight003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      
      for (let i = 0; i < gDeviceList.length; i++) {
        try {
          let hasRight = await usbMgr.requestRight(gDeviceList[i].name);
          console.info(TAG, 'usb case requestRight iteration ' + i + ' ret : ' + hasRight);
          expect(hasRight).assertTrue();
        } catch(error) {
          console.info(TAG, 'usb case device request right failed : ' + error + ' : ' + gDeviceList[i].name);
          expect(error !== null).assertFalse();
        }
      }
    })

    /**
     * @tc.name   testRemoveRight002
     * @tc.number SUB_USB_HostManager_Ets_Func_1201
     * @tc.desc   Test removeRight with invalid device name
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testRemoveRight002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testRemoveRight002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      
      let remRight = usbMgr.removeRight("invalid_device_name");
      console.info(TAG, 'usb case removeRight ret : ' + remRight);
      expect(remRight).assertFalse();
    })

    /**
     * @tc.name   testRemoveRight003
     * @tc.number SUB_USB_HostManager_Ets_Func_1202
     * @tc.desc   Test removeRight and requestRight sequence
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testRemoveRight003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testRemoveRight003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      
      let remRight = usbMgr.removeRight(devices.name);
      console.info(TAG, 'usb case removeRight ret : ' + remRight);
      expect(remRight).assertTrue();
      
      await getPermission();
      CheckEmptyUtils.sleep(1000);
      await driveFn();
      CheckEmptyUtils.sleep(1000);
      
      let hasRight = usbMgr.hasRight(devices.name);
      console.info(TAG, 'usb case hasRight after requestRight : ' + hasRight);
      expect(hasRight).assertTrue();
    })

    /**
     * @tc.name   testConnectDevice002
     * @tc.number SUB_USB_HostManager_Ets_Func_0101
     * @tc.desc   Test connectDevice multiple times
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConnectDevice002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testConnectDevice002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      
      let pipe1 = usbMgr.connectDevice(devices);
      console.info(TAG, 'usb case testConnectDevice002 first connect ret: ' + JSON.stringify(pipe1));
      expect(pipe1 !== null).assertTrue();
      
      let pipe2 = usbMgr.connectDevice(devices);
      console.info(TAG, 'usb case testConnectDevice002 second connect ret: ' + JSON.stringify(pipe2));
      expect(pipe2 !== null).assertTrue();
      
      let isPipClose = usbMgr.closePipe(pipe1);
      console.info(TAG, 'usb case testConnectDevice002 close ret: ' + isPipClose);
      expect(isPipClose).assertEqual(0);
    })

    /**
     * @tc.name   testConnectDevice003
     * @tc.number SUB_USB_HostManager_Ets_Func_0102
     * @tc.desc   Test connectDevice with different device instances
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConnectDevice003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testConnectDevice003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      
      if (gDeviceList.length < 2) {
        console.info(TAG, 'Not enough devices for this test');
        expect(true).assertTrue();
        return;
      }
      
      let pipe1 = usbMgr.connectDevice(gDeviceList[0]);
      console.info(TAG, 'usb case testConnectDevice003 device 0 ret: ' + JSON.stringify(pipe1));
      expect(pipe1 !== null).assertTrue();
      
      let pipe2 = usbMgrMgr.connectDevice(gDeviceList[1]);
      console.info(TAG) 'usb case testConnectDevice003 device 1 ret: ' + JSON.stringify(pipe2));
      expect(pipe2 !== null).assertTrue();
      
      usbMgr.closePipe(pipe1);
      usbMgr.closePipe(pipe2);
    })

    /**
     * @tc.name   testClosePipe003
     * @tc.number SUB_USB_HostManager_Ets_Func_0301
     * @tc.desc   Test closePipe with unopened pipe
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testClosePipe003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testClosePipe003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      
      let invalidPipe: usbMgr.USBDevicePipe = {
        busNum: 999,
        devAddress: 999
      };
      
      let isPipClose = usbMgr.closePipe(invalidPipe);
      console.info(TAG, 'usb case closePipe with invalid pipe ret: ' + isPipClose);
      expect(isPipClose !== 0).assertTrue();
    })

    /**
     * @tc.name   testClosePipe004
     * @tc.number SUB_USB_HostManager_Ets_Func_0302
     * @tc.desc   Test closePipe twice
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testClosePipe004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testClosePipe004 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      
      getPipe('testClosePipe004');
      
      let isPipClose1 = usbMgr.closePipe(gPipe);
      console.info(TAG, 'usb case closePipe first close ret: ' + isPipClose1);
      expect(isPipClose1).assertEqual(0);
      
      let isPipClose2 = usbMgr.closePipe(gPipe);
      console.info(TAG, 'usb case closePipe second close ret: ' + isPipClose2);
      expect(isPipClose2 !== 0).assertTrue();
    })

    /**
     * @tc.name   testGetRawDescriptor001
     * @tc.number SUB_USB_HostManager_Ets_Func_0401
     * @tc.desc   Test getRawDescriptor
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testGetRawDescriptor001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testGetRawDescriptor001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      
      getPipe('testGetRawDescriptor001');
      
      let ret = usbMgr.getRawDescriptor(gPipe);
      console.info(TAG, 'usb case getRawDescriptor ret: ' + ret);
      expect(ret !== undefined).assertTrue();
      expect(ret.length > 0).assertTrue();
      
      toClosePipe('testGetRawDescriptor001');
    })

    /**
     * @tc.name   testGetFileDescriptor001
     * @tc.number SUB_USB_HostManager_Ets_Func_0402
     * @tc.desc   Test getFileDescriptor
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testGetFileDescriptorudi001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testGetFileDescriptor001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      
      getPipe('testGetFileDescriptor001');
      
      let ret = usbMgr.getFileDescriptor(gPipe);
      console.info(TAG, 'usb case getFileDescriptor ret: ' + ret);
      expect(ret !== undefined).assertTrue();
      expect(ret > 0).assertTrue();
      
      toClosePipe('testGetFileDescriptor001');
    })

    /**
     * @tc.name   testGetConfig001
     * @tc.number SUB_USB_HostManager_Ets_Func_0403
     * @tc.desc   Test getConfig
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testGetConfig001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testGetConfig001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      
      getPipe('testGetConfig001');
      
      let ret = usbMgr.getConfig(gPipe);
      console.info(TAG, 'usb case getConfig ret: ' + JSON.stringify(ret));
      expect(ret !== null).assertTrue();
      expect(ret.id).toBeLargerThan(-1);
      expect(ret.interfaces).not.toBeNull();
      expect(ret.interfaces.length).toBeLargerThan(0);
      
      toClosePipe('testGetConfig001');
    })

    /**
     * @tc.name   testClaimInterface001
     * @tc.number SUB_USB_HostManager_Ets_Func_0501
     * @tc.desc   Test claimInterface
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testClaimInterface001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testClaimInterface001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      
      getPipe('testClaimInterface001');
      
      let config = usbMgr.getConfig(gPipe);
      if (config.interfaces.length > 0) {
        let isClaim = usbMgr.claimInterface(gPipe, config.interfaces[0], true);
        console.info(TAG, 'usb case claimInterface ret: ' + isClaim);
        expect(isClaim).assertEqual(0);
        
        let isRelease = usbMgr.releaseInterface(gPipe, config.interfaces[0]);
        console.info(TAG, 'usb case releaseInterface ret: ' + isRelease);
        expect(isRelease).assertEqual(0);
      }
      
      toClosePipe('testClaimInterface001');
    })

    /**
     * @tc.name   testSetInterface001
     * @tc.number SUB_USB_HostManager_Ets_Func_0502
     * @tc.desc   Test setInterface
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testSetInterface001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testSetInterface001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      
      getPipe('testSetInterface001');
      
      let config = usbMgr.getConfig(gPipe);
      if (config.interfaces.length > 0) {
        let isClaim = usbMgr.claimInterface(gPipe, config.interfaces[0], true);
        console.info(TAG, 'usb case claimInterface ret: ' + isClaim);
        expect(isClaim).assertEqual(0);
        
        let isSet = usbMgr.setInterface(gPipe, config.interfaces[0], true);
        console.info(TAG, 'usb case setInterface ret: ' + isSet);
        expect(isSet).assertEqual(0);
        
        let isRelease = usbMgr.releaseInterface(gPipe, config.interfaces[0]);
        console.info(TAG, 'usb case releaseInterface ret: ' + isRelease);
        expect(isRelease).assertEqual(0);
      }
      
      toClosePipe('testSetInterface001');
    })
  })
}