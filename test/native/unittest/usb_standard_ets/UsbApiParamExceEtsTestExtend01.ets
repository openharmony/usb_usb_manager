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

let gDeviceList: Array<usbMgr.USBDevice>;
let gPipe: usbMgr.USBDevicePipe | null;
let devices: usbMgr.USBDevice | null;
let isDeviceConnected: boolean;
const TAG = "[UsbApiParamExceEtsTestExtend01]";

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

function toReleaseInterface(testCaseName: string, tInterface: usbMgr.USBInterface): void {
  let ret = usbMgr.releaseInterface(gPipe, tInterface);
  console.info(TAG, `usb ${testCaseName} releaseInterface ret: ${ret}`);
  expect(ret).assertEqual(0);
}

function toClosePipe(testCaseName: string): void {
  let isPipClose = usbMgr.closePipe(gPipe);
  console.info(TAG, `usb ${testCaseName} closePipe ret: ${isPipClose}`);
  expect(isPipClose).assertEqual(0);
}

export default function UsbApiParamExceEtsTestExtend01() {
  describe('UsbApiParamExceEtsTestExtend01', () => {

    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbApiParamExceEtsTestExtend01 Begin*************');
      
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
      console.log(TAG, '*************Usb Unit UsbApiParamExceEtsTestExtend01 End*************');
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_0100
     * @tc.name     : testHasRightParamEx003
     * @tc.desc     : Negative test: Param is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testHasRightParamEx003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testHasRightParamEx003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let isHasRight = usbMgr.hasRight(null);
        console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testHasRightParamEx003 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_0200
     * @tc.name     : testHasRightParamEx004
     * @tc.desc     : Negative test: Param is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testHasRightParamEx004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testHasRightParamEx004 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let isHasRight = usbMgr.hasRight(undefined);
        console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testHasRightParamEx004 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_0300
     * @tc.name     : testHasRightParamEx005
     * @tc.desc     : Negative test: Param is number
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testHasRightParamEx005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testHasRightParamEx005 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let isHasRight = usbMgr.hasRight(123);
        console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testHasRightParamEx005 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_0400
     * @tc.name     : testHasRightParamEx006
     * @tc.desc     : Negative test: Param is object
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testHasRightParamEx006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testHasRightParamEx006 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let isHasRight = usbMgr.hasRight({});
        console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testHasRightParamEx006 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_0500
     * @tc.name     : testHasRightParamEx007
     * @tc.desc     : Negative test: Param is array
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testHasRightParamEx007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testHasRightParamEx007 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let isHasRight = usbMgr.hasRight([]);
        console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testHasRightParamEx007 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401) ;
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_0600
     * @tc.name     : testHasRightParamEx008
     * @tc.desc     : Negative test: Param is boolean
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testHasRightParamEx008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testHasRightParamEx008 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let isHasRight = usbMgr.hasRight(true);
        console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testHasRightParamEx008 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_0700
     * @tc.name     : testRequestRightParamEx003
     * @tc.desc     : Negative test: Param is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRequestRightParamEx003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testRequestRightParamEx003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let isHasRight = await usbMgr.requestRight(null);
        console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testRequestRightParamEx003 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_0800
     * @tc.name     : testRequestRightParamEx004
     * @tc.desc     : Negative test: Param is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRequestRightParamEx004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testRequestRightParamEx004 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let isHasRight = await usbMgr.requestRight(undefined);
        console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testRequestRightParamEx004 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_0900
     * @tc.name     : testRequestRightParamEx005
     * @tc.desc     : Negative test: Param is number
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRequestRightParamEx005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testRequestRightParamEx005 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let isHasRight = await usbMgr.requestRight(123);
        console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testRequestRightParamEx005 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_1000
     * @tc.name     : testRequestRightParamEx006
     * @tc.desc     : Negative test: Param is object
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRequestRightParamEx006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testRequestRightParamEx006 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let isHasRight = await usbMgr.requestRight({});
        console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testRequestRightParamEx006 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_1100
     * @tc.name     : testRequestRightParamEx007
     * @tc.desc     : Negative test: Param is array
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRequestRightParamEx007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testRequestRightParamEx007 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let isHasRight = await usbMgr.requestRight([]);
        console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testRequestRightParamEx007 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_1200
     * @tc.name     : testRequestRightParamEx008
     * @tc.desc     : Negative test: Param is boolean
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRequestRightParamEx008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testRequestRightParamEx008 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let isHasRight = await usbMgr.requestRight(true);
        console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testRequestRightParamEx008 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_1300
     * @tc.name     : testRemoveRightParamEx005
     * @tc.desc     : Negative test: Param is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamEx005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG) 'usb testRemoveRightParamEx005 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let remRight = usbMgr.removeRight(null);
        console.info(TAG, 'usb case removeRight ret : ' + remRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testRemoveRightParamEx005 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_1400
     * @tc.name     : testRemoveRightParamEx006
     * @tc.desc     : Negative test: Param is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamEx006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testRemoveRightParamEx006 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let remRight = usbMgr.removeRight(undefined);
        console.info(TAG, 'usb case removeRight ret : ' + remRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testRemoveRightParamEx006 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_1500
     * @tc.name     : testRemoveRightParamEx007
     * @tc.desc     : Negative test: Param is number
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamEx007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testRemoveRightParamEx007 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let remRight = usbMgr.removeRight(123);
        console.info(TAG, 'usb case removeRight ret : ' + remRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testRemoveRightParamEx007 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_1600
     * @tc.name     : testRemoveRightParamEx008
     * @tc.desc     : Negative test: Param is object
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamEx008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testRemoveRightParamEx008 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let remRight = usbMgr.removeRight({});
        console.info(TAG, 'usb case removeRight ret : ' + remRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testRemoveRightParamEx008 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_1700
     * @tc.name     : testRemoveRightParamEx009
     * @tc.desc     : Negative test: Param is array
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamEx009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testRemoveRightParamEx009 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let remRight = usbMgr.removeRight([]);
        console.info(TAG, 'usb case removeRight ret : ' + remRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testRemoveRightParamEx009 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_Ets_ParamEx_Extend_1800
     * @tc.name     : testRemoveRightParamEx010
     * @tc.desc     : Negative test: Param is boolean
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamEx010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testRemoveRightParamEx010 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      try {
        let remRight = usbMgr.removeRight(true);
        console.info(TAG, 'usb case removeRight ret : ' + remRight);
        expect(false).assertTrue();
      } catch (err) {
        console.info(TAG, 'testRemoveRightParamEx010 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(401);
      }
    })
  })
}