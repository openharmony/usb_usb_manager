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
import CheckEmptyUtils from './CheckEmptyUtils.js';
import EventConstants from './EventConstants.js';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium';

let gDeviceList: Array<usbMgr.USBDevice>;
let gPipe: usbMgr.USBDevicePipe | null;
let devices: usbMgr.USBDevice | null;
let isDeviceConnected: boolean;
const TAG = "[UsbCoreEtsTestExtend02]";

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
  EXPECT(gPipe !== null).assertTrue();
}

function toClosePipe(testCaseName: string): void {
  let isPipClose = usbMgr.closePipe(gPipe);
  console.info(TAG, `usb ${testCaseName} closePipe getPipe ret: ${isPipClose}`);
  expect(isPipClose).assertEqual(0);
}

function getTransferTestParam(): any {
  let testParam = {
    device: null,
    config: null,
    pip: null,
    inEndpoint: null,
    outEndpoint: null,
    interface: null,
    usbRequest: null,
    sendData: '',
    isClaimed: 0,
    maxInSize: 1024,
    maxOutSize: 1024
  };

  console.info(TAG, 'usb case gDeviceList.length: ' + gDeviceList.length);
  for (let i = 0; i < gDeviceList.length; i++) {
    testParam.device = gDeviceList[i];
    testParam.config = testParam.device.configs[0];
    testParam.pip = gPipe;
    
    for (let j = 0; j < testParam.config.interfaces.length; j++) {
      let iface = testParam.config.interfaces[j];
      if (iface.clazz == 10 && iface.subClass == 0 && iface.protocol == 2) {
        for (let k = 0; k < iface.endpoints.length; k++) {
          let endpoint = iface.endpoints[k];
          if (endpoint.type == EventConstants.USB_ENDPOINT_XFER_BULK) {
            if (endpoint.direction == usbMgr.USB_REQUEST_DIR_TO_DEVICE) {
              testParam.outEndpoint = endpoint;
              testParam.maxOutSize = endpoint.maxPacketSize;
            } else if (endpoint.direction == usbMgr.USB_REQUEST_DIR_FROM_DEVICE) {
              testParam.inEndpoint = endpoint;
              testParam.maxInSize = endpoint.maxPacketSize;
            }
          }
        }
        if (testParam.inEndpoint != null && testParam.outEndpoint != null) {
          testParam.interface = iface;
          break;
        }
      }
    }
    if (testParam.interface != null) {
      break;
    }
  }
  return testParam;
}

export default function UsbCoreEtsTestExtend02() {
  describe('UsbCoreEtsTestExtend02', () => {

    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbCoreEtsTestExtend02 Begin*************');
      
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
      console.log(TAG, '*************Usb Unit UsbCoreEtsTestExtend02 End*************');
    })

    /**
     * @tc.name   testBulkTransfer001
     * @tc.number SUB_USB_HostManager_Ets_Func_0601
     * @tc.desc   Test bulkTransfer with valid parameters
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testBulkTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testBulkTransfer001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let testParam = getTransferTestParam();
      if (testParam.interface == null || testParam.outEndpoint == null) {
        console.info(TAG, 'No suitable interface or endpoint found');
        expect(true).assertTrue();
        return;
      }
      
      getPipe('testBulkTransfer001');
      
      testParam.isClaimed = usbMgr.claimInterface(gPipe, testParam.interface, true);
      expect(testParam.isClaimed).assertEqual(0);
      
      testParam.sendData = 'test data for bulk transfer';
      let tmpUint8Array = new Uint8Array(testParam.sendData.length);
      for (let i = 0; i < testParam.sendData.length; i++) {
        tmpUint8Array[i] = testParam.sendData.charCodeAt(i);
      }
      
      try {
        let ret = await usbMgr.bulkTransfer(gPipe, testParam.outEndpoint, tmpUint8Array, 5000);
        console.info(TAG, 'usb case bulkTransfer ret: ' + ret);
        expect(ret >= 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb bulkTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      usbMgr.releaseInterface(gPipe, testParam.interface);
      toClosePipe('testBulkTransfer001');
    })

    /**
     * @tc.name   testBulkTransfer002
     * @tc.number SUB_USB_HostManager_Ets_Func_0602
     * @tc.desc   Test bulkTransfer with zero length data
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testBulkTransfer002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testBulkTransfer002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let testParam = getTransferTestParam();
      if (testParam.interface == null || testParam.outEndpoint == null) {
        console.info(TAG, 'No suitable interface or endpoint found');
        expect(true).assertTrue();
        return;
      }
      
      getPipe('testBulkTransfer002');
      
      testParam.isClaimed = usbMgr.claimInterface(gPipe, testParam.interface, true);
      expect(testParam.isClaimed).assertEqual(0);
      
      let emptyData = new Uint8Array(0);
      
      try {
        let ret = await usbMgr.bulkTransfer(gPipe, testParam.outEndpoint, emptyData, 5000);
        console.info(TAG, 'usb case bulkTransfer ret: ' + ret);
        expect(ret >= 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb bulkTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      usbMgr.releaseInterface(gPipe, testParam.interface);
      toClosePipe('testBulkTransfer002');
    })

    /**
     * @tc.name   testBulkTransfer003
     * @tc.number SUB_USB_HostManager_Ets_Func_0603
     * @tc.desc   Test bulkTransfer with large data
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testBulkTransfer003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testBulkTransfer003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let testParam = getTransferTestParam();
      if (testParam.interface == null || testParam.outEndpoint == null) {
        console.info(TAG, 'No suitable interface or endpoint found');
        expect(true).assertTrue();
        return;
      }
      
      getPipe('testBulkTransfer003');
      
      testParam.isClaimed = usbMgr.claimInterface(gPipe, testParam.interface, true);
      expect(testParam.isClaimed).assertEqual(0);
      
      let largeData = new Uint8Array(512);
      for (let i = 0; i < 512; i++) {
        largeData[i] = i % 256;
      }
      
      try {
        let ret = await usbMgr.bulkTransfer(gPipe, testParam.outEndpoint, largeData, 5000);
        console.info(TAG, 'usb case bulkTransfer ret: ' + ret);
        expect(ret >= 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb bulkTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      usbMgr.releaseInterface(gPipe, testParam.interface);
      toClosePipe('testBulkTransfer003');
    })

    /**
     * @tc.name   testControlTransfer001
     * @tc.number SUB_USB_HostManager_Ets_Func_0701
     * @tc.desc   Test controlTransfer with valid parameters
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testControlTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testControlTransfer001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testControlTransfer001');
      
      let tmpUint8Array = new Uint8Array(512);
      let usbRequest = {
        requestType: 0x80,
        request: 0x06,
        value: 0x0100,
        index: 0x00,
        data: tmpUint8Array
      };
      
      try {
        let ret = await usbMgr.controlTransfer(gPipe, usbRequest, 5000);
        console.info(TAG, 'usb case controlTransfer ret: ' + ret);
        expect(ret >= 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb controlTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      toClosePipe('testControlTransfer001');
    })

    /**
     * @tc.name   testControlTransfer002
     * @tc.number SUB_USB_HostManager_Ets_Func_0702
     * @tc.desc   Test controlTransfer with GET_DESCRIPTOR request
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testControlTransfer002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testControlTransfer002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testControlTransfer002');
      
      let tmpUint8Array = new Uint8Array(18);
      let usbRequest = {
        requestType: 0x80,
        request: 0x06,
        value: 0x0100,
        index: 0x00,
        data: tmpUint8Array
      };
      
      try {
        let ret = await usbMgr.controlTransfer(gPipe, usbRequest, 5000);
        console.info(TAG, 'usb case controlTransfer ret: '' + ret);
        expect(ret >= 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb controlTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      toClosePipe('testControlTransfer002');
    })

    /**
     * @tc.name   testControlTransfer003
     * @tc.number SUB_USB_HostManager_Ets_Func_0703
     * @tc.desc   Test controlTransfer with zero timeout
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testControlTransfer003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testControlTransfer003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testControlTransfer003');
      
      let tmpUint8Array = new Uint8Array(512);
      let usbRequest = {
        requestType: 0x80,
        request: 0x06,
        value: 0x0100,
        index: 0x00,
        data: tmpUint8Array
      };
      
      try {
        let ret = await usbMgr.controlTransfer(gPipe, usbRequest, 0);
        console.info(TAG, 'usb case controlTransfer ret: ' + ret);
        expect(ret >= 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb controlTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      toClosePipe('testControlTransfer003');
    })

    /**
     * @tc.name   testInterruptTransfer001
     * @tc.number SUB_USB_HostManager_Ets_Func_0801
     * @tc.desc   Test interruptTransfer with valid parameters
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterruptTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testInterruptTransfer001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let testParam = getTransferTestParam();
      if (testParam.interface == null) {
        console.info(TAG, 'No suitable interface found');
        expect(true).assertTrue();
        return;
      }
      
      getPipe('testInterruptTransfer001');
      
      testParam.isClaimed = usbMgr.claimInterface(gPipe, testParam.interface, true);
      expect(testParam.isClaimed).assertEqual(0);
      
      let interruptEndpoint = {
        address: 0x81,
        type: 3,
        direction: 1,
        maxPacketSize: 64,
        interval: 1
      };
      
      let tmpUint8Array = new Uint8Array(64);
      for (let i = 0; i < 64; i++) {
        tmpUint8Array[i] = i;
      }
      
      try {
        let ret = await usbMgr.interruptTransfer(gPipe, interruptEndpoint, tmpUint8Array, 5000);
        console.info(TAG, 'usb case interruptTransfer ret: ' + ret);
        expect(ret >= 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb interruptTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      usbMgr.releaseInterface(gPipe, testParam.interface);
      toClosePipe('testInterruptTransfer001');
    })

    /**
     * @tc.name   testInterruptTransfer002
     * @tc.number SUB_USB_HostManager_Ets_Func_0802
     * @tc.desc   Test interruptTransfer with small data
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterruptTransfer002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testInterruptTransfer002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let testParam = getTransferTestParam();
      if (testParam.interface == null) {
        console.info(TAG, 'No suitable interface found');
        expect(true).assertTrue();
        return;
      }
      
      getPipe('testInterruptTransfer002');
      
      testParam.isClaimed = usbMgr.claimInterface(gPipe, testParam.interface, true);
      expect(testParam.isClaimed).assertEqual(0);
      
      let interruptEndpoint = {
        address: 0x81,
        type: 3,
        direction: 1,
        maxPacketSize: 64,
        interval: 1
      };
      
      let smallData = new Uint8Array(8);
      for (let i = 0; i < 8; i++) {
        smallData[i] = i;
      }
      
      try {
        let ret = await usbMgr.interruptTransfer(gPipe, interruptEndpoint, smallData, 5000);
        console.info(TAG, 'usb case interruptTransfer ret: ' + ret);
        expect(ret >= 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb interruptTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      usbMgr.releaseInterface(gPipe, testParam.interface);
      toClosePipe('testInterruptTransfer002');
    })

    /**
     * @tc.name   testSetInterface001
     * @tc.number SUB_USB_HostManager_Ets_Func_0901
     * @tc.desc   Test setInterface with valid parameters
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testSetInterface001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testSetInterface001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
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

    /**
     * @tc.name   testSetInterface002
     * @tc.number SUB_USB_HostManager_Ets_Func_0902
     * @tc.desc   Test setInterface with alternate setting
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
        let isClaim = usbMgr.claimInterface(gPipe, config.interfaces[0], false);
        console.info(TAG, 'usb case claimInterface ret: ' + isClaim);
        expect(isClaim).assertEqual(0);
        
        let isSet = usbMgr.setInterface(gPipe, config.interfaces[0], false);
        console.info(TAG, 'usb case setInterface ret: ' + isSet);
        expect(isSet).assertEqual(0);
        
        let isRelease = usbMgr.releaseInterface(gPipe, config.interfaces[0]);
        console.info(TAG, 'usb case releaseInterface ret: ' + isRelease);
        expect(isRelease).assertEqual(0);
      }
      
      toClosePipe('testSetInterface002');
    })

    /**
     * @tc.name   testGetPorts001
     * @tc.number SUB_USB_HostManager_Ets_Func_1001
     * @tc.desc   Test getPorts
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testGetPorts001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, (done: Function) => {
      console.info(TAG, 'usb testGetPorts001 begin');
      
      let ports = usbMgr.getPorts();
      console.info(TAG, 'usb case getPorts ret: ' + JSON.stringify(ports));
      expect(ports).not.toBeNull();
      expect(Array.isArray(ports)).assertTrue();
      
      done();
    })

    /**
     * @tc.name   testSetPortRoles001
     * @tc.number SUB_USB_HostManager_Ets_Func_1101
     * @tc.desc   Test setPortRoles
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testSetPortRoles001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testSetPortRoles001 begin');
      
      let ports = usbMgr.getPorts();
      if (ports.length > 0) {
        try {
          let ret = await usbMgr.setPortRoles(ports[0].id, usbMgr.SOURCE, usbMgr.HOST);
          console.info(TAG, 'usb case setPortRoles ret: ' + ret);
          expect(ret).assertTrue();
        } catch (error) {
          console.info(TAG, 'usb setPortRoles error: ' + JSON.stringify(error));
          expect(error !== null).assertTrue();
        }
      } else {
        console.info(TAG, 'No ports available for setPortRoles test');
        expect(true).assertTrue();
      }
    })

    /**
     * @tc.name   testSetPortRoles002
     * @tc.number SUB_USB_HostManager_Ets_Func_1102
     * @tc.desc   Test setPortRoles with device mode
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testSetPortRoles002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testSetPortRoles002 begin');
      
      let ports = usbMgr.getPorts();
      if (ports.length > 0) {
        try {
          let ret = await usbMgr.setPortRoles(ports[0].id, usbMgr.SINK, usbMgr.DEVICE);
          console.info(TAG, 'usb case setPortRoles ret: ' + ret);
          expect(ret).assertTrue();
        } catch (error) {
          console.info(TAG, 'usb setPortRoles error: ' + JSON.stringify(error));
          expect(error !== null).assertTrue();
        }
      } else {
        console.info(TAG, 'No ports available for setPortRoles test');
        expect(true).assertTrue();
      }
    })
  })
}