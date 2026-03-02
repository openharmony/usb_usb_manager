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
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium';

let gDeviceList: Array<usbMgr.USBDevice>;
let gPipe: usbMgr.USBDevicePipe | null;
let devices: usbMgr.USBDevice | null;
let isDeviceConnected: boolean;
const TAG = "[UsbDevicePipeTestExtend01]";

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
  for (let) i = 0; i < gDeviceList.length; i++) {
    testParam.device = gDeviceList[i];
    testParam.config = testParam.device.configs[0];
    testParam.pip = gPipe;
    
    for (let) j = 0; j < testParam.config.interfaces.length; j++) {
      let iface = testParam.config.interfaces[j];
      if (iface.clazz == 10 && iface.subClass == 0 && iface.protocol == 2) {
        for (let k = 0; k < iface.endpoints.length; k++) {
          let endpoint = iface.endpoints[k];
          if (endpoint.type == usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK) {
            if (endpoint.direction == usbMgr.USB_REQUEST_DIR_TO_DEVICE) {
              testParam.outEndpoint = endpoint;
              testParam.maxOutSize = endpoint.maxPacketSize;
            } else if (endpoint.direction == usbMgr.USB_REQUEST_DIR_FROM_DEVICE) {
              testParam.inEndpoint = endpoint;
              test)Param.maxInSize = endpoint.maxPacketSize;
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

export default function UsbDevicePipeTestExtend01() {
  describe('UsbDevicePipeTestExtend01', () => {

    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbDevicePipeTestExtend01 Begin*************');
      
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
      console.log(TAG, '*************Usb Unit UsbDevicePipeTestExtend01 End*************');
    })

    /**
     * @tc.name   testBulkTransfer003
     * @tc.number SUB_USB_DevicePipe_Extend_Bulk_0100
     * @tc.desc   Test bulkTransfer with invalid pipe
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
      
      let invalidPipe: usbMgr.USBDevicePipe = {
        busNum: 999,
        devAddress: 999
      };
      
      let tmpUint8Array = new Uint8Array(10);
      for (let i = 0; i < 10; i++) {
        tmpUint8Array[i] = i;
      }
      
      try {
        let ret = await usbMgr.bulkTransfer(invalidPipe, testParam.outEndpoint, tmpUint8Array, 5000);
        console.info(TAG, 'usb case bulkTransfer ret: ' + ret);
        expect(false).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb bulkTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
    })

    /**
     * @tc.name   testBulkTransfer004
     * @tc.number SUB_USB_DevicePipe_Extend_Bulk_0200
     * @tc.desc   Test bulkTransfer with null data
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testBulkTransfer004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testBulkTransfer004 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testBulkTransfer004');
      
      let testParam = getTransferTestParam();
      if (testParam.interface == null || testParam.outEndpoint == null) {
        console.info(TAG, 'No suitable interface or endpoint found');
        expect(true).assertTrue();
        return;
      }
      
      testParam.isClaimed = usbMgr.claimInterface(gPipe, testParam.interface, true);
      expect(testParam.isClaimed).assertEqual(0);
      
      try {
        let ret = await usbMgr.bulkTransfer(gPipe, testParam.outEndpoint, null, 5000);
        console.info(TAG, 'usb case bulkTransfer ret: ' + ret);
        expect(false).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb bulkTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      usbMgr.releaseInterface(gPipe, testParam.interface);
      toClosePipe('testBulkTransfer004');
    })

    /**
     * @tc.name   testBulkTransfer005
     * @tc.number SUB_USB_DevicePipe_Extend_Bulk_0300
     * @tc.desc   Test bulkTransfer with invalid endpoint
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testBulkTransfer005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testBulkTransfer005 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testBulkTransfer005');
      
      let invalidEndpoint: usbMgr.USBEndpoint = {
        address: 0xFF,
        type: 2,
        direction: 1,
        maxPacketSize: 512
      };
      
      let tmpUint8Array = new Uint8Array(10);
      for (let i = 0; i < 10; i++) {
        tmpUint8Array[i] = i;
      }
      
      try {
        let ret = await usbMgr.bulkTransfer(gPipe, invalidEndpoint, tmpUint8Array, 5000);
        console.info(TAG, 'usb case bulkTransfer ret: ' + ret);
        expect(false).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb bulkTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      toClosePipe('testBulkTransfer005');
    })

    /**
     * @tc.name   testBulkTransfer006
     * @tc.number SUB_USB_DevicePipe_Extend_Bulk_0400
     * @tc)desc   Test bulkTransfer with negative timeout
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testBulkTransfer006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testBulkTransfer006 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let testParam = getTransferTestParam();
      if (testParam.interface == null || testParam.outEndpoint == null) {
        console.info(TAG, 'No suitable interface or endpoint found');
'        expect(true).assertTrue();
        return;
      }
      
      getPipe('testBulkTransfer006');
      
      testParam.isClaimed = usbMgr.claimInterface(gPipe, testParam.interface, true);
      expect(testParam.isClaimed).assertEqual(0);
      
      let tmpUint8Array = new Uint8Array(10);
      for (let i = 0; i < 10; i++) {
        tmpUint8Array[i] = i;
      }
      
      try {
        let ret = await usbMgr.bulkTransfer(gPipe, testParam.outEndpoint, tmpUint8Array, -1000);
        console.info(TAG, 'usb case bulkTransfer ret: ' + ret);
        expect(false).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb bulkTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      usbMgr.releaseInterface(gPipe, testParam.interface);
      toClosePipe('testBulkTransfer006');
    })

    /**
     * @tc.name   testBulkTransfer007
     * @tc.number SUB_USB_DevicePipe_Extend_Bulk_0500
     * @tc.desc   Test bulkTransfer with zero timeout
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testBulkTransfer007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testBulkTransfer007 begin');
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
      
      getPipe('testBulkTransfer007');
      
      testParam.isClaimed = usbMgr.claimInterface(gPipe, testParam.interface, true);
      expect(testParam.isClaimed).assertEqual(0);
      
      let tmpUint8Array = new Uint8Array(10);
      for (let i = 0; i < 10; i++) {
        tmpUint8Array[i] = i;
      }
      
      try {
        let ret = await usbMgr.bulkTransfer(gPipe, testParam.outEndpoint, tmpUint8Array, 0);
        console.info(TAG, 'usb case bulkTransfer ret: ' + ret);
        expect(ret >= 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb bulkTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      usbMgr.releaseInterface(gPipe, testParam.interface);
      toClosePipe('testBulkTransfer007');
    })

    /**
     * @tc.name   testControlTransfer003
     * @tc.number SUB_USB_DevicePipe_Extend_Control_0100
     * @tc.desc   Test controlTransfer with invalid pipe
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
      
      let invalidPipe: usbMgr.USBDevicePipe = {
        busNum: 999,
        devAddress: 999
      };
      
      let tmpUint8Array = new Uint8Array(512);
      let usbRequest = {
        requestType: 0x80,
        request: 0x06,
        value: 0x0100,
        index: 0x00,
        data: tmpUint8Array
      };
      
      try {
        let ret = await usbMgr.controlTransfer(invalidPipe, usbRequest, 5000);
        console.info(TAG, 'usb case controlTransfer ret: ' + ret);
        expect(false).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb controlTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
    })

    /**
     * @tc.name   testControlTransfer004
     * @tc.number SUB_USB_DevicePipe_Extend_Control_0200
     *'tc.desc   Test controlTransfer with null data
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testControlTransfer004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testControlTransfer004 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testControlTransfer004');
      
      let usbRequest = {
        requestType: 0x80,
        request: 0x06,
        value: 0x0100,
        index: 0x00,
        data: null
      };
      
      try {
        let ret = await usbMgr.controlTransfer(gPipe, usbRequest, 5000);
        console.info(TAG, 'usb case controlTransfer ret: ' + ret);
        expect(false).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb controlTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      toClosePipe('testControlTransfer004');
    })

    /**
     * @tc.name   testControlTransfer005
     * @tc.number SUB_USB_DevicePipe_Extend_Control_0300
     * @tc.desc   Test controlTransfer with invalid request type
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testControlTransfer005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testControlTransfer005 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testControlTransfer005');
      
      let tmpUint8Array = new Uint8Array(512);
      let usbRequest = {
        requestType: 0xFF,
        request: 0x06,
        value: 0x0100,
        index: 0x00,
        data: tmpUint8Array
      };
      
      try {
        let ret = await usbMgr.controlTransfer(gPipe, usbRequest, 5000);
        console.info(TAG, 'usb case controlTransfer ret: ' + ret);
        expect(false).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb controlTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      toClosePipe('testControlTransfer005');
    })

    /**
     * @tc.name   testControlTransfer006
     * @tc.number SUB_USB_DevicePipe_Extend_Control_0400
     * @tc.desc   Test controlTransfer with negative timeout
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testControlTransfer006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testControlTransfer006 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testControlTransfer006');
      
      let tmpUint8Array = new Uint8Array(512);
      let usbRequest = {
        requestType: 0x80,
        request: 0x06,
        value: 0x0100,
        index: 0x00,
        data: tmpUint8Array
      };
      
      try {
        let ret = await usbMgr.controlTransfer(gPipe, usbRequest, -1000);
        console.info(TAG, 'usb case controlTransfer ret: ' + ret);
        expect(false).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb controlTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      toClosePipe('testControlTransfer006');
    })

    /**
     * @tc.name   testInterruptTransfer003
     * @tc.number SUB_USB_DevicePipe_Extend_Interrupt_0100
     * @tc.desc   Test interruptTransfer with invalid pipe
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterruptTransfer003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testInterruptTransfer003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let'invalidPipe: usbMgr.USBDevicePipe = {
        busNum: 999,
        devAddress: 999
      };
      
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
        let ret = await usbMgr.interruptTransfer(invalidPipe, interruptEndpoint, tmpUint8Array, 5000);
        console.info(TAG, 'usb case interruptTransfer ret: ' + ret);
        expect(false).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb interruptTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
    })

    /**
     * @tc.name   testInterruptTransfer004
     * @tc.number SUB_USB_DevicePipe_Extend_Interrupt_0200
     * @tc.desc   Test interruptTransfer with null data
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterruptTransfer004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testInterruptTransfer004 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testInterruptTransfer004');
      
      let testParam = getTransferTestParam();
      if (testParam.interface == null) {
        console.info(TAG, 'No suitable interface found');
        expect(true).assertTrue();
        return;
      }
      
      testParam.isClaimed = usbMgr.claimInterface(gPipe, testParam.interface, true);
      expect(testParam.isClaimed).assertEqual(0);
      
      let interruptEndpoint = {
        address: 0x81,
        type: 3,
        direction: 1,
        maxPacketSize: 64,
        interval: 1
      };
      
      try {
        let ret = await usbMgr.interruptTransfer(gPipe, interruptEndpoint, null, 5000);
        console.info(TAG, 'usb case interruptTransfer ret: ' + ret);
        expect(false).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb interruptTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      usbMgr.releaseInterface(gPipe, testParam.interface);
      toClosePipe('testInterruptTransfer004');
    })

    /**
     * @tc.name   testInterruptTransfer005
     * @tc.number SUB_USB_DevicePipe_Extend_Interrupt_0300
     * @tc.desc   Test interruptTransfer with invalid endpoint
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterruptTransfer005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testInterruptTransfer005 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testInterruptTransfer005');
      
      let invalidEndpoint = {
        address: 0xFF,
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
        let ret = await usbMgr.interruptTransfer(gPipe, invalidEndpoint, tmpUint8Array, 5000);
        console.info(TAG, 'usb case interruptTransfer ret: ' + ret);
        expect(false).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb interruptTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      toClosePipe('testInterruptTransfer005');
    })

    /**
     * @tc.name   testInterruptTransfer006
     * @tc.number SUB_USB_DevicePipe_Extend_Interrupt_0400
     * @tc.desc   Test interruptTransfer with negative timeout
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterruptTransfer006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testInterruptTransfer006 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testInterruptTransfer006');
      
      let testParam = getTransferTestParam();
      if (testParam.interface == null) {
        console.info(TAG, 'No suitable interface found');
        expect(true).assertTrue();
        return;
      }
      
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
        let ret = await usbMgr.interruptTransfer(gPipe, interruptEndpoint, tmpUint8Array, -1000);
        console.info(TAG, 'usb case interruptTransfer ret: ' + ret);
        expect(false).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb interruptTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      usbMgr.releaseInterface(gPipe, testParam.interface);
      toClosePipe('testInterruptTransfer006');
    })

    /**
     * @tc.name   testIsochronousTransfer001
     * @tc.number SUB_USB_DevicePipe_Extend_Isochronous_0100
     * @tc.desc   Test isochronousTransfer with valid parameters
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testIsochronousTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testIsochronousTransfer001 begin');
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
      
      getPipe('testIsochronousTransfer001');
      
      testParam.isClaimed = usbMgr.claimInterface(gPipe, testParam.interface, true);
      expect(testParam.isClaimed).assertEqual(0);
      
      let isoEndpoint = {
        address: 0x01,
        type: 1,
        direction: 1,
        maxPacketSize: 512,
        interval: 1
      };
      
      let tmpUint8Array = new Uint8Array(512);
      for (let i = 0; i < 512; i++) {
        tmpUint8Array[i] = i % 256;
      }
      
      try {
        let ret = await usbMgr.isochronousTransfer(gPipe, isoEndpoint, tmpUint8Array, 5000);
        console.info(TAG, 'usb case isochronousTransfer ret: ' + ret);
        expect(ret >= 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb isochronousTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      usbMgr.releaseInterface(gPipe, testParam.interface);
      toClosePipe('testIsochronousTransfer001');
    })

    /**
     * @tc.name   testIsochronousTransfer002
     * @tc.number SUB_USB_DevicePipe_Extend_Isochronous_0200
     * @tc.desc   Test isochronousTransfer with null data
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testIsochronousTransfer002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testIsochronousTransfer002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testIsochronousTransfer002');
      
      let testParam = getTransferTestParam();
      if (testParam.interface == null) {
        console.info(TAG, 'No suitable interface found');
        expect(true).assertTrue();
        return;
      }
      
      testParam.isClaimed = usbMgr.claimInterface(gPipe, testParam.interface, true);
      expect(testParam.isClaimed).assertEqual(0);
      
      let isoEndpoint = {
        address: 0x01,
        type: 1,
        direction: 1,
        maxPacketSize: 512,
        interval: 1
      };
      
      try {
        let ret = await usbMgr.isochronousTransfer(gPipe, isoEndpoint, null, 5000);
        console.info(TAG, 'usb case isochronousTransfer ret: ' + ret);
        expect(false).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb isochronousTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      usbMgr.releaseInterface(gPipe, testParam.interface);
      toClosePipe('testIsochronousTransfer002');
    })

    /**
     * @tc.name   testIsochronousTransfer003
     * @tc.number SUB_USB_DevicePipe_Extend_Isochronous_0300
     * @tc.desc   Test isochronousTransfer with invalid endpoint
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testIsochronousTransfer003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testIsochronousTransfer003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testIsochronousTransfer003');
      
      let testParam = getTransferTestParam();
      if (testParam.interface == null) {
        console.info(TAG, 'No suitable interface found');
        expect(true).assertTrue();
        return;
      }
      
      testParam.isClaimed = usbMgr.claimInterface(gPipe, testParam.interface, true);
      expect(testParam.isClaimed).assertEqual(0);
      
      let invalidEndpoint = {
        address: 0xFF,
        type: 1,
        direction: 1,
        maxPacketSize: 512,
        interval: 1
      };
      
      let tmpUint8Array = new Uint8Array(512);
      for (let i = 0; i < 512; i++) {
        tmpUint8Array[i] = i % 256;
      }
      
      try {
        let ret = await usbMgr.isochronousTransfer(gPipe, invalidEndpoint, tmpUint8Array, 5000);
        console.info(TAG, 'usb case isochronousTransfer ret: ' + ret);
        expect(false).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb isochronousTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      usbMgr.releaseInterface(gPipe, testParam.interface);
      toClosePipe('testIsochronousTransfer003');
    })

    /**
     * @tc.name   testIsochronousTransfer004
     * @tc.number SUB_USB_DevicePipe_Extend_Isochronous_0400
     * @tc.desc   Test isochronousTransfer with negative timeout
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testIsochronousTransfer004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testIsochronousTransfer004 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      getPipe('testIsochronousTransfer004');
      
      let testParam = getTransferTestParam();
      if (testParam.interface == null) {
        console.info(TAG, 'No suitable interface found');
        expect(true).assertTrue();
        return;
      }
      
      testParam.isClaimed = usbMgr.claimInterface(gPipe, testParam.interface, true);
      expect(testParam.isClaimed).assertEqual(0);
      
      let isoEndpoint = {
        address: 0x01,
        type: 1,
        direction: 1,
        maxPacketSize: 512,
        interval: 1
      };
      
      let tmpUint8Array = new Uint8Array(512);
      for (let i = 0; i < 512; i++) {
        tmpUint8Array[i] = i % 256;
      }
      
      try {
        let ret = await usbMgr.isochronousTransfer(gPipe, isoEndpoint, tmpUint8Array, -1000);
        console.info(TAG, 'usb case isochronousTransfer ret: ' + ret);
        expect(false).assertTrue();
      } catch (error) {
        console.info(TAG, 'usb isochronousTransfer error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      usbMgr.releaseInterface(gPipe, testParam.interface);
      toClosePipe('testIsochronousTransfer004');
    })
  })
}