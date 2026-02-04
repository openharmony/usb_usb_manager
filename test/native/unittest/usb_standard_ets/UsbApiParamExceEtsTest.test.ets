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
import CheckEmptyUtils from './CheckEmptyUtils.js';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium';


const TAG = "[UsbApiParamExceEtsTest]";
const PARAM_NULLSTRING = "";
let gDeviceList: Array<usbMgr.USBDevice>;
let gPipe: usbMgr.USBDevicePipe;
let devices: usbMgr.USBDevice;
let isDeviceConnected: boolean = true;
let tmpPipe: usbMgr.USBDevicePipe = {
  busNum: 0,
  devAddress: 0
};
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

function toReleaseInterface(testCaseName: string, tInterface: usbMgr.USBInterface) {
  let ret = usbMgr.releaseInterface(tmpPipe, tInterface);
  console.info(TAG, `usb ${testCaseName} releaseInterface ret: ${ret}`);
  expect(ret).assertEqual(0);
}

function toClosePipe(testCaseName: string) {
  let isPipClose = usbMgr.closePipe(tmpPipe);
  console.info(TAG, `usb ${testCaseName} closePipe ret: ${isPipClose}`);
  expect(isPipClose).assertEqual(0);
}


export default function UsbApiParamExceEtsTest() {
  describe('UsbApiParamExceEtsTest', () => {

    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbApiParamExceEtsTest Begin*************');

      // version > 17  host currentMode = 2 device currentMode = 1
      gDeviceList = usbMgr.getDevices();
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

        tmpPipe.busNum = gDeviceList[0].busNum;
        tmpPipe.devAddress = gDeviceList[0].devAddress;
      }
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test CaseEx*************');
      gDeviceList = usbMgr.getDevices();
      if (isDeviceConnected) {
        devices = gDeviceList[0];
        console.info(TAG, 'beforeEach return devices : ' + JSON.stringify(devices));
      }
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test CaseEx*************');
      console.info(TAG, 'afterEach return devices : ' + JSON.stringify(devices));
    })

    afterAll(() => {
      console.log(TAG, '*************Usb Unit UsbApiParamExceEtsTest End*************');
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0100
     * @tc.name     : testHasRightParamEx001
     * @tc.desc     : Negative test: Param is null string
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testHasRightParamEx001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testHasRightParamEx001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let isHasRight = usbMgr.hasRight(PARAM_NULLSTRING);
        console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
        expect(isHasRight).assertFalse();
      } catch (err) {
        console.info(TAG, 'testHasRightParamEx001 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0200
     * @tc.name     : testHasRightParamEx002
     * @tc.desc     : Negative test: Param add number '123'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testHasRightParamEx002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testHasRightParamEx002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        for (let i = 0; i < gDeviceList.length; i++) {
          let deviceName = gDeviceList[i].name;
          deviceName = deviceName + "123";
          let isHasRight = usbMgr.hasRight(deviceName);
          console.info(TAG, 'usb [', deviceName, '] hasRight ret : ' + isHasRight);
          expect(isHasRight).assertFalse();
        }
      } catch (err) {
        console.info(TAG, 'testHasRightParamEx002 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0300
     * @tc.name     : testRequestRightParamEx001
     * @tc.desc     : Negative test: Param is null string
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRequestRightParamEx001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testRequestRightParamEx001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let isHasRight = await usbMgr.requestRight(PARAM_NULLSTRING);
        console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
        expect(isHasRight).assertFalse();
      } catch (err) {
        console.info(TAG, 'testRequestRightParamEx001 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0400
     * @tc.name     : testRequestRightParamEx002
     * @tc.desc     : Negative test: Param add number 'abc'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRequestRightParamEx002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testRequestRightParamEx002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        for (let i = 0; i < gDeviceList.length; i++) {
          let deviceName = gDeviceList[i].name;
          deviceName = deviceName + "abc";
          let isHasRight = await usbMgr.requestRight(deviceName);
          console.info(TAG, 'usb [', deviceName, '] requestRight ret : ' + isHasRight);
          expect(isHasRight).assertFalse();
        }
      } catch (err) {
        console.info(TAG, 'testRequestRightParamEx002 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0500
     * @tc.name     : testRemoveRightParamEx001
     * @tc.desc     : Negative test: Param is null string
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamEx001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testRemoveRightParamEx001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let remRight = usbMgr.removeRight(PARAM_NULLSTRING);
        console.info(TAG, 'usb case removeRight ret : ' + remRight);
        expect(remRight).assertFalse();
      } catch (err) {
        console.info(TAG, 'testRemoveRightParamEx001 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0600
     * @tc.name     : testRemoveRightParamEx002
     * @tc.desc     : Negative test: Param add letter 'abc'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamEx002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testRemoveRightParamEx002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        for (let i = 0; i < gDeviceList.length; i++) {
          let deviceName = gDeviceList[i].name;
          deviceName = deviceName + "abc";
          let remRight = usbMgr.removeRight(deviceName);
          console.info(TAG, 'usb [', deviceName, '] removeRight ret : ', remRight);
          expect(remRight).assertFalse();
        }
      } catch (err) {
        console.info(TAG, 'testRemoveRightParamEx002 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0700
     * @tc.name     : testRemoveRightParamEx003
     * @tc.desc     : Negative test: Param add special characters '@#'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamEx003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testRemoveRightParamEx003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        for (let i = 0; i < gDeviceList.length; i++) {
          let deviceName = gDeviceList[i].name;
          deviceName = deviceName + "@#";
          let remRight = usbMgr.removeRight(deviceName);
          console.info(TAG, 'usb [', deviceName, '] removeRight ret : ', remRight);
          expect(remRight).assertFalse();
        }
      } catch (err) {
        console.info(TAG, 'testRemoveRightParamEx003 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0800
     * @tc.name     : testRemoveRightParamEx004
     * @tc.desc     : Negative test: Param add number '123'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamEx004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testRemoveRightParamEx004 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        for (let i = 0; i < gDeviceList.length; i++) {
          let deviceName = gDeviceList[i].name;
          deviceName = deviceName + "123";
          let remRight = usbMgr.removeRight(deviceName);
          console.info(TAG, 'usb [', deviceName, '] removeRight ret : ', remRight);
          expect(remRight).assertFalse();
        }
      } catch (err) {
        console.info(TAG, 'testRemoveRightParamEx004 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_0900
     * @tc.name     : testConnectDeviceParamEx001
     * @tc.desc     : Negative test: Param add number '123'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let deviceName = devices.name + "123";
        devices.name = deviceName;
        let gPipe = usbMgr.connectDevice(devices);

        console.info(TAG, 'usb [', devices.name, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx001 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1000
     * @tc.name     : testConnectDeviceParamEx002
     * @tc.desc     : Negative test: Param add letter 'abc'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let deviceName = devices.name + "abc";
        devices.name = deviceName;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.name, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx002 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1100
     * @tc.name     : testConnectDeviceParamEx003
     * @tc.desc     : Negative test: Param add special characters '@#'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let deviceName = devices.name + "@#";
        devices.name = deviceName;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.name, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx003 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1200
     * @tc.name     : testConnectDeviceParamEx004
     * @tc.desc     : Negative test: devices name is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx004 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        devices.name = PARAM_NULLSTRING;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.name, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx004 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1300
     * @tc.name     : testConnectDeviceParamEx005
     * @tc.desc     : Negative test: devices serial is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx005 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        devices.serial = PARAM_NULLSTRING;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.serial, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx005 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1400
     * @tc.name     : testConnectDeviceParamEx006
     * @tc.desc     : Negative test: devices serial add letter abc
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx006 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devSerial = devices.serial + "abc";
        devices.serial = devSerial;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.serial, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx006 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1500
     * @tc.name     : testConnectDeviceParamEx007
     * @tc.desc     : Negative test: devices serial add number 123
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx007 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devSerial = devices.serial + "123";
        devices.serial = devSerial;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.serial, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx007 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1600
     * @tc.name     : testConnectDeviceParamEx008
     * @tc.desc     : Negative test: devices serial add special characters '@#'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx008 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devSerial = devices.serial + "@#";
        devices.serial = devSerial;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.serial, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx008 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1700
     * @tc.name     : testConnectDeviceParamEx009
     * @tc.desc     : Negative test: devices manufacturerName add special characters '@#'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx009 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devManufacturerName = devices.manufacturerName + "@#";
        devices.manufacturerName = devManufacturerName;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.manufacturerName, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx009 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1800
     * @tc.name     : testConnectDeviceParamEx010
     * @tc.desc     : Negative test: devices manufacturerName add special characters 'abc'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx010 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devManufacturerName = devices.manufacturerName + "abc";
        devices.manufacturerName = devManufacturerName;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.manufacturerName, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx010 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_1900
     * @tc.name     : testConnectDeviceParamEx011
     * @tc.desc     : Negative test: devices manufacturerName add special characters '123'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx011', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx011 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devManufacturerName = devices.manufacturerName + "123";
        devices.manufacturerName = devManufacturerName;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.manufacturerName, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx011 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2000
     * @tc.name     : testConnectDeviceParamEx012
     * @tc.desc     : Negative test: devices manufacturerName add special characters ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx012', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx012 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        devices.manufacturerName = PARAM_NULLSTRING;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.manufacturerName, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx012 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2100
     * @tc.name     : testConnectDeviceParamEx013
     * @tc.desc     : Negative test: devices productName add special characters '@#'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx013', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx013 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devProductName = devices.productName + "@#";
        devices.productName = devProductName;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.productName, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx013 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2200
     * @tc.name     : testConnectDeviceParamEx014
     * @tc.desc     : Negative test: devices productName add special characters 'abc'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx014', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx014 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devProductName = devices.productName + "abc";
        devices.productName = devProductName;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.productName, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx014 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2300
     * @tc.name     : testConnectDeviceParamEx015
     * @tc.desc     : Negative test: devices productName add special characters '123'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx015', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx015 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devProductName = devices.productName + "123";
        devices.productName = devProductName;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.productName, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx015 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2400
     * @tc.name     : testConnectDeviceParamEx016
     * @tc.desc     : Negative test: devices productName is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx016', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx016 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        devices.productName = PARAM_NULLSTRING;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.productName, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx016 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2500
     * @tc.name     : testConnectDeviceParamEx017
     * @tc.desc     : Negative test: devices version add special characters '@#'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx017', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx017 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devVersion = devices.version + "@#";
        devices.version = devVersion;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.version, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx017 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2600
     * @tc.name     : testConnectDeviceParamEx018
     * @tc.desc     : Negative test: devices version add special characters 'abc'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx018', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx018 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devVersion = devices.version + "abc";
        devices.version = devVersion;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.version, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx018 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2700
     * @tc.name     : testConnectDeviceParamEx019
     * @tc.desc     : Negative test: devices version add special characters '123'
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx019', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx019 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devVersion = devices.version + "123";
        devices.version = devVersion;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.version, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx019 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2800
     * @tc.name     : testConnectDeviceParamEx020
     * @tc.desc     : Negative test: devices version is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx020', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx020 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        devices.version = PARAM_NULLSTRING;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.version, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx020 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_2900
     * @tc.name     : testConnectDeviceParamEx021
     * @tc.desc     : Negative test: devices vendorId is add number 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx021', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx021 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devVendorId = devices.vendorId + 1000;
        devices.vendorId = devVendorId;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.vendorId, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx021 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3000
     * @tc.name     : testConnectDeviceParamEx022
     * @tc.desc     : Negative test: devices productId is add number 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx022', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx022 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devProductId = devices.productId + 1000;
        devices.productId = devProductId;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.productId, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx022 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3100
     * @tc.name     : testConnectDeviceParamEx023
     * @tc.desc     : Negative test: devices clazz is add number 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx023', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx023 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devClazz = devices.clazz + 1000;
        devices.clazz = devClazz;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.clazz, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx023 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3200
     * @tc.name     : testConnectDeviceParamEx024
     * @tc.desc     : Negative test: devices subClass is add number 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx024', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx024 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devSubClass = devices.subClass + 1000;
        devices.subClass = devSubClass;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.subClass, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx024 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3300
     * @tc.name     : testConnectDeviceParamEx025
     * @tc.desc     : Negative test: devices protocol is add number 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx025', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx025 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devProtocol = devices.protocol + 1000;
        devices.protocol = devProtocol;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.protocol, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx025 catch err code: ', err.code, ', message: ', err.message);
        expect(err !== null).assertFalse();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3400
     * @tc.name     : testConnectDeviceParamEx026
     * @tc.desc     : Negative test: devices busNum is add number 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx026', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx026 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devBusNum = devices.busNum + 1000;
        devices.busNum = devBusNum;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.busNum, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(gPipe !== null).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx026 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(14400001);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3500
     * @tc.name     : testConnectDeviceParamEx027
     * @tc.desc     : Negative test: devices devAddress is add number 10000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamEx027', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConnectDeviceParamEx027 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      try {
        let devDevAddress = devices.devAddress + 1000;
        devices.devAddress = devDevAddress;
        let gPipe = usbMgr.connectDevice(devices);
        console.info(TAG, 'usb [', devices.devAddress, '] connectDevice ret : ', JSON.stringify(gPipe));
        expect(gPipe !== null).assertFalse();
      } catch (err) {
        console.info(TAG, 'testConnectDeviceParamEx027 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(14400001);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamEx_3600
     * @tc.name     : testClosePipeParamEx001
     * @tc.desc     : Negative test: pipe busNum add number 1000
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClosePipeParamEx001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testClosePipeParamEx001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      getPipe("testClosePipeParamEx001");
      try {
        gPipe.busNum = gPipe.busNum + 1000;
        let ret = usbMgr.closePipe(gPipe);
        console.info(TAG, 'usb [', gPipe.busNum, '] closePipe ret : ', ret);
        expect(ret !== 0).assertTrue();
      } catch (err) {
        console.info(TAG, 'testClosePipeParamEx001 catch err code: ', err);
        expect(err !== null).assertFalse();
      }
      toClosePipe("testClosePipeParamEx001");
    })
  })
}
