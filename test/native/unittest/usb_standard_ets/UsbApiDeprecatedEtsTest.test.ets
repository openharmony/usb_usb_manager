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

import usb from '@ohos.usb';
import { UiDriver, BY } from '@ohos.UiTest';
import { BusinessError } from '@ohos.base'
import CheckEmptyUtils from './CheckEmptyUtils.js';
import EventConstants from './EventConstants.js';
import parameter from '@ohos.systemparameter';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium';


let gDeviceList: Array<usb.USBDevice>;
let gPipe: usb.USBDevicePipe;
let devices: usb.USBDevice;
let isDeviceConnected: boolean = true;
let testParam: TransferTestParam;
const TAG = "[UsbApiDeprecatedEtsTest]";

interface TransferTestParam {
  config: usb.USBConfig;
  device: usb.USBDevice;
  pip: usb.USBDevicePipe;
  inEndpoint: usb.USBEndpoint;
  interface: usb.USBInterface;
  outEndpoint: usb.USBEndpoint;
  usbRequest: null;
  sendData: string;
  isClaimed: number;
  maxInSize: number;
  maxOutSize: number;
}

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
    usb.requestRight(gDeviceList[0].name).then(hasRight => {
      console.info(TAG, `usb requestRight success, hasRight: ${hasRight}`);
    })
  } catch (err) {
    console.info(TAG, `usb getPermission to requestRight hasRight fail: `, err);
    return
  }
}

function findInitPoint(testParam: TransferTestParam, interfaceIndex: number): boolean {
  let isEndpointFound  = false;
  const endpoints = testParam.config.interfaces[interfaceIndex].endpoints;

  endpoints.forEach((endpoint: usb.USBEndpoint) => {
    if (endpoint.type === EventConstants.USB_ENDPOINT_XFER_BULK) {
      isEndpointFound = true;
      if (endpoint.direction === usb.USBRequestDirection.USB_REQUEST_DIR_TO_DEVICE) {
        testParam.maxOutSize = endpoint.maxPacketSize;
        testParam.outEndpoint = endpoint;
      } else if (endpoint.direction === usb.USBRequestDirection.USB_REQUEST_DIR_FROM_DEVICE) {
        testParam.maxInSize = endpoint.maxPacketSize;
        testParam.inEndpoint = endpoint;
      }
    }
  });

  if (isEndpointFound) {
    testParam.interface = testParam.config.interfaces[interfaceIndex];
    return isEndpointFound;
  }
  return isEndpointFound;
}

function getFlag(testParam: TransferTestParam, j: number) {
  if (testParam.config.interfaces[j].endpoints.length == 0) {
    return false;
  }

  if (testParam.config.interfaces[j].clazz != 10 ||
    testParam.config.interfaces[j].subClass != 0 ||
    testParam.config.interfaces[j].protocol != 2) {
    return false;
  }
  return true;
}

function initPoint(testParam: TransferTestParam) {
  for (let j = 0; j < testParam.config.interfaces.length; j++) {
    if (getFlag(testParam, j) == true) {
      if (findInitPoint(testParam, j) == true) {
        break
      }
    }
  }
}

// Prefabrication transmission related parameters
function getTransferTestParam(): TransferTestParam {

  testParam = {
    config: gDeviceList[0].configs[0],
    device: gDeviceList[0],
    pip: gPipe,
    inEndpoint: gDeviceList[0].configs[0].interfaces[0].endpoints[0],
    interface: gDeviceList[0].configs[0].interfaces[0],
    outEndpoint: gDeviceList[0].configs[0].interfaces[0].endpoints[0],
    usbRequest: null,
    sendData: '',
    isClaimed: 0,
    maxInSize: 1024,
    maxOutSize: 1024,
  };

  console.info(TAG, 'usb case gDeviceList.length: ' + gDeviceList.length);
  for (let i = 0; i < gDeviceList.length; i++) {
    testParam.device = gDeviceList[i];
    testParam.config = testParam.device.configs[0];
    testParam.pip = gPipe;
    initPoint(testParam);
  }
  return testParam;
}


function getTransferParam(iCmd: number, iReqTarType: usb.USBRequestTargetType, iReqType: usb.USBControlRequestType,
  iValue: number, iIndex: number) {
  let tmpUint8Array = new Uint8Array(512);
  let requestCmd = iCmd
  let requestTargetType = iReqTarType
  let requestType = iReqType
  let value = iValue;
  let index = iIndex;
  let controlParam: usb.USBControlParams = {
    request: requestCmd,
    target: requestTargetType,
    reqType: requestType,
    value: value,
    index: index,
    data: tmpUint8Array
  }
  return controlParam;
}

async function callControlTransfer(pip: usb.USBDevicePipe, controlParam: usb.USBControlParams, timeout: number,
  caseName: string) {
  await usb.controlTransfer(pip, controlParam, timeout).then(data => {
    console.info(TAG, 'usb controlTransfer ret data : ' + data + ' ' + caseName);
    console.info(TAG, 'usb controlTransfer controlParam.data buffer : ' + controlParam.data + ' ' + caseName);
    expect(data >= 0).assertTrue();
  }).catch ((error: BusinessError) => {
    console.info(TAG, 'usb ' + caseName + ' controlTransfer error : ' + JSON.stringify(error));
    expect(error === null).assertTrue();
  });
}

async function callControlTransferEx(pip: usb.USBDevicePipe, controlParam: usb.USBControlParams, timeout: number,
  caseName: string) {
  await usb.controlTransfer(pip, controlParam, timeout).then(data => {
    console.info(TAG, 'usb controlTransfer ret data : ' + data + ' ' + caseName);
    expect(data === null).assertTrue();
  });
}

function toReleaseInterface(testCaseName: string, conIndex: number, interIndex: number) {
  gDeviceList = usb.getDevices();
  let tmpInterface = gDeviceList[0].configs[conIndex].interfaces[interIndex];
  let isClaim = usb.releaseInterface(gPipe, tmpInterface);
  console.info(TAG, `usb ${testCaseName} toReleaseInterface ret: ${JSON.stringify(isClaim)}`);
  expect(isClaim).assertEqual(0);
}

function getPipe(testCaseName: string) {
  gPipe = usb.connectDevice(devices);
  console.info(TAG, `usb ${testCaseName} connectDevice getPipe ret: ${JSON.stringify(gPipe)}`);
  expect(gPipe !== null).assertTrue();
}

function toClosePipe(testCaseName: string) {
  let isPipClose = usb.closePipe(gPipe);
  console.info(TAG, `usb ${testCaseName} closePipe getPipe ret: ${isPipClose}`);
  expect(isPipClose).assertEqual(0);
}

/* usb core functions test */
export default function UsbApiDeprecatedEtsTest() {
  describe('UsbApiDeprecatedEtsTest', () => {
    
    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbApiDeprecatedEtsTest Begin*************');

      // version > 17  host currentMode = 2 device currentMode = 1
      try {
        gDeviceList = usb.getDevices();
        console.info(TAG, 'beforeAll: usb case gDeviceList return: ' + gDeviceList);
        isDeviceConnected = deviceConnected();
        if (isDeviceConnected) {
          console.info(TAG, 'beforeAll: usb case gDeviceList return: ' + JSON.stringify(gDeviceList));
          let hasRight = usb.hasRight(gDeviceList[0].name);
          if (!hasRight) {
            console.info(TAG, `beforeAll: usb requestRight start`);
            await getPermission();
            CheckEmptyUtils.sleep(1000);
            await driveFn();
            CheckEmptyUtils.sleep(1000);
          }
        }
      } catch (error) {
        console.info(TAG, `beforeAll usb catch error: ${error}`);
      }
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test  Case*************');
      gDeviceList = usb.getDevices();
      if (isDeviceConnected) {
        devices = gDeviceList[0];
        console.info(TAG, 'beforeEach return devices : ' + JSON.stringify(devices));
      }
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test  Case*************');
      console.info(TAG, 'afterEach return devices : ' + JSON.stringify(devices));
      console.info(TAG, 'afterEach return devices : ' + JSON.stringify(gPipe));
    })

    afterAll(() => {
      console.log(TAG, '*************Usb Unit UsbApiDeprecatedEtsTest End*************');
    })
    

    /**
     * @tc.name   testGetDevices001
     * @tc.number SUB_USB_HostManager_JSDep_Func_0500
     * @tc.desc   【C-ALL-HARDWARE-0502】必须支持连接标准 USB 外围设备
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testGetDevices001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, (done: Function) => {
      console.info(TAG, 'usb testGetDevices001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        done();
        return
      }
      gDeviceList = usb.getDevices();
      console.info(TAG, 'usb case getDevices ret length: ' + gDeviceList.length);
      expect(gDeviceList.length > 0).assertTrue();
      done();
    })

    /**
     * @tc.name   testGetDevices002
     * @tc.number SUB_USB_HostManager_JS_FuncRetValue_0200
     * @tc.desc   Positive test: Get device list is null
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testGetDevices002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, (done: Function) => {
      console.info(TAG, 'usb testGetDevices002 begin');
      if (!isDeviceConnected) {
        let res = usb.getDevices();
        console.info(TAG, 'usb case getDevices ret length: ' + res.length);
        expect(res.length).assertEqual(0);
        done();
      } else {
        done();
        return;
      }
    })

    /**
     * @tc.name   testHasRight001
     * @tc.number SUB_USB_HostManager_JSDep_Func_0900
     * @tc.desc   Positive test: Permission query
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testHasRight001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, (done: Function) => {
      console.info(TAG, 'usb testHasRight001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        done();
        return
      }

      for (let i = 0; i < gDeviceList.length; i++) {
        let hasRight = usb.hasRight(gDeviceList[i].name);
        console.info(TAG, 'usb has_right ret :' + hasRight);
        expect(hasRight).assertTrue();
      }
      done();
    })

    /**
     * @tc.name   testRequestRight001
     * @tc.number SUB_USB_HostManager_JSDep_Func_0800
     * @tc.desc   Positive test: Request permission
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testRequestRight001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testRequestRight001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }

      for (let i = 0; i < gDeviceList.length; i++) {
        try {
          let hasRight = await usb.requestRight(gDeviceList[i].name);
          console.info(TAG, 'usb testRequestRight001 ret :' + hasRight);
          expect(hasRight).assertTrue();
        } catch(error) {
          console.info(TAG, 'usb case device request right failed : ' + error + ' :' + gDeviceList[i].name);
          expect(error !== null).assertFalse();
        }
      }
    })

    /**
     * @tc.name   testConnectDevice001
     * @tc.number SUB_USB_HostManager_JSDep_Func_0100
     * @tc.desc   Positive test: open device
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConnectDevice001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testConnectDevice001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }

      gPipe = usb.connectDevice(devices);
      console.info(TAG, 'usb case testConnectDevice001 ret: ' + JSON.stringify(gPipe));
      expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      toClosePipe('testConnectDevice001');
    })

    /**
     * @tc.name   testClosePipe001
     * @tc.number SUB_USB_HostManager_JSDep_Func_0300
     * @tc.desc   Positive test: close device
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testClosePipe001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testClosePipe001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }
      getPipe('testClosePipe001');

      let isPipClose = usb.closePipe(gPipe);
      console.info(TAG, 'usb case closePipe ret: ' + isPipClose);
      expect(isPipClose).assertEqual(0);
    })

    /**
     * @tc.name   testClosePipe002
     * @tc.number SUB_USB_HostManager_JSDep_Compatibility_1300
     * @tc.desc   Negative test: close device, busNum error -23
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testClosePipe002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testClosePipe002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }

      getPipe('testClosePipe002');
      let tmpPipe: usb.USBDevicePipe = JSON.parse(JSON.stringify(gPipe));
      tmpPipe.busNum = -23;
      console.info(TAG, 'usb case testClosePipe002 param: ' + JSON.stringify(tmpPipe));
      let isPipClose = usb.closePipe(tmpPipe);
      console.info(TAG, 'usb case testClosePipe002 ret: ' + isPipClose);
      expect(isPipClose == 0).assertFalse();
      toClosePipe('testClosePipe002');
    })

    /**
     * @tc.name   testClosePipe003
     * @tc.number SUB_USB_HostManager_JSDep_Compatibility_1400
     * @tc.desc   Negative test: close device, devAddress error -23
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

      getPipe('testClosePipe003');
      let tmpPipe: usb.USBDevicePipe = JSON.parse(JSON.stringify(gPipe));
      tmpPipe.devAddress = -23;
      console.info(TAG, 'usb case testClosePipe003 param: ' + JSON.stringify(tmpPipe));
      let isPipClose = usb.closePipe(tmpPipe);
      console.info(TAG, 'usb case testClosePipe003 ret: ' + isPipClose);
      expect(isPipClose == 0).assertFalse();
      toClosePipe('testClosePipe003');
    })

    /**
     * @tc.name   testClosePipe004
     * @tc.number SUB_USB_HostManager_JSDep_Compatibility_1500
     * @tc.desc   Negative test: close device, devAddress && busNum error -23
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
      let tmpPipe: usb.USBDevicePipe = JSON.parse(JSON.stringify(gPipe));
      tmpPipe.devAddress = -23;
      tmpPipe.busNum = -23;
      console.info(TAG, 'usb case testClosePipe004 param: ' + JSON.stringify(tmpPipe));
      let isPipClose = usb.closePipe(tmpPipe);
      console.info(TAG, 'usb case testClosePipe004 ret: ' + isPipClose);
      expect(isPipClose == 0).assertFalse();
      toClosePipe('testClosePipe004');
    })

    /**
     * @tc.name   testGetRawDescriptor001
     * @tc.number SUB_USB_HostManager_JSDep_Func_0700
     * @tc.desc   Positive test: Get the original USB descriptor
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
      let descriptor = usb.getRawDescriptor(gPipe);
      console.info(TAG, 'usb case testGetRawDescriptor001 ret: ' + JSON.stringify(descriptor));
      expect(descriptor.length).assertLarger(0);
      toClosePipe('testGetRawDescriptor001');
    })

    /**
     * @tc.name   testGetFileDescriptor001
     * @tc.number SUB_USB_HostManager_JSDep_Func_0600
     * @tc.desc   Positive test: Get file descriptor
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testGetFileDescriptor001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testGetFileDescriptor001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }

      getPipe('testGetFileDescriptor001');
      let fileDescriptor = usb.getFileDescriptor(gPipe);
      console.info(TAG, 'usb case testGetFileDescriptor001 ret: ' + fileDescriptor);
      expect(fileDescriptor >= 0).assertTrue();
      toClosePipe('testGetFileDescriptor001');
    })

    /**
     * @tc.name   testGetFileDescriptor002
     * @tc.number SUB_USB_HostManager_JSDep_Compatibility_2900
     * @tc.desc   Negative test: Get file descriptor, error busNum=255 devAddress=255
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testGetFileDescriptor002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testGetFileDescriptor002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }

      getPipe('testGetFileDescriptor002');
      let tempPipe: usb.USBDevicePipe = {busNum : 255, devAddress : 255};
      console.info(TAG, 'usb case testGetFileDescriptor002 param: ' + JSON.stringify(tempPipe));
      let fileDescriptor = usb.getFileDescriptor(tempPipe);
      console.info(TAG, 'usb case testGetFileDescriptor002 ret: ' + fileDescriptor);
      expect(fileDescriptor).assertEqual(-1);
      toClosePipe('testGetFileDescriptor002');
    })

    /**
     * @tc.name   testGetFileDescriptor003
     * @tc.number SUB_USB_HostManager_JSDep_Compatibility_3000
     * @tc.desc   Negative test: Get file descriptor,error busNum=255
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testGetFileDescriptor003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testGetFileDescriptor003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }

      getPipe('testGetFileDescriptor003');
      let tempPipe: usb.USBDevicePipe = {busNum : 255, devAddress : gPipe.devAddress};
      console.info(TAG, 'usb case testGetFileDescriptor003 param: ' + JSON.stringify(tempPipe));
      let fileDescriptor = usb.getFileDescriptor(tempPipe);
      console.info(TAG, 'usb case testGetFileDescriptor003 ret: ' + fileDescriptor);
      expect(fileDescriptor).assertEqual(-1);
      toClosePipe('testGetFileDescriptor003');
    })

    /**
     * @tc.name   testGetFileDescriptor004
     * @tc.number SUB_USB_HostManager_JSDep_Compatibility_3100
     * @tc.desc   Negative test: Get file descriptor,error devAddress=255
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testGetFileDescriptor004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testGetFileDescriptor004 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }

      getPipe('testGetFileDescriptor004');
      let tempPipe: usb.USBDevicePipe = {busNum : gPipe.busNum, devAddress : 255};
      console.info(TAG, 'usb case testGetFileDescriptor004 param: ' + JSON.stringify(tempPipe));
      let fileDescriptor = usb.getFileDescriptor(tempPipe);
      expect(fileDescriptor).assertEqual(-1);
      console.info(TAG, 'usb case testGetFileDescriptor004 ret: ' + fileDescriptor);
      toClosePipe('testGetFileDescriptor004');
    })
    //****************************************************************

    /**
     * @tc.name   testConnectDevice002
     * @tc.number SUB_USB_HostManager_JSDep_Compatibility_0100
     * @tc.desc   Negative test: open device, error devAddress 2+10000
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConnectDevice002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb SUB_USB_HostManager_JS_Compatibility_0100 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }

      let device: usb.USBDevice = JSON.parse(JSON.stringify(devices));
      device.devAddress = 2 + 10000;
      try {
        gPipe = usb.connectDevice(device);
        console.info(TAG, 'usb case testConnectDevice002 ret: ' + JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertTrue();
      } catch (err) {
        console.info(TAG, 'usb testConnectDevice002 fail: ' + err);
        expect(err !== null).assertTrue();
      }
    })

    /**
     * @tc.name   testConnectDevice003
     * @tc.number SUB_USB_HostManager_JSDep_Compatibility_0200
     * @tc.desc   Negative test: open device, error busNum 2+1000
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

      let device: usb.USBDevice = JSON.parse(JSON.stringify(devices));
      device.busNum = 2 + 1000;
      try {
        gPipe = usb.connectDevice(device);
        console.info(TAG, 'usb case testConnectDevice003 ret: ' + JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertTrue();
      } catch (err) {
        console.info(TAG, 'usb testConnectDevice003 fail: ' + err);
        expect(err !== null).assertTrue();
      }

    })

    /**
     * @tc.name   testConnectDevice004
     * @tc.number SUB_USB_HostManager_JSDep_Compatibility_0300
     * @tc.desc   Negative test: open device, error serial 'asdfsd'
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConnectDevice004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testConnectDevice004 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }

      let device: usb.USBDevice = JSON.parse(JSON.stringify(devices));
      device.serial = 'asdfsd';
      try {
        gPipe = usb.connectDevice(device);
        console.info(TAG, 'usb case testConnectDevice004 ret: ' + JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'usb testConnectDevice004 fail: ' + err);
        expect(err !== null).assertTrue();
      }
      toClosePipe('testConnectDevice004');
    })

    /**
     * @tc.name   testConnectDevice006
     * @tc.number SUB_USB_HostManager_JSDepr_Compatibility_0600
     * @tc.desc   Negative test: open device, error productName 'sdfsdfe'
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConnectDevice006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testConnectDevice006 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }

      let device: usb.USBDevice = JSON.parse(JSON.stringify(devices));
      device.productName = 'sdfsdfe';
      try {
        gPipe = usb.connectDevice(device);
        console.info(TAG, 'usb case testConnectDevice006 ret: ' + JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'usb testConnectDevice006 fail: ' + err);
        expect(err !== null).assertTrue();
      }
      toClosePipe('testConnectDevice006');
    })

    /**
     * @tc.name   testConnectDevice007
     * @tc.number SUB_USB_HostManager_JSDep_Compatibility_0600
     * @tc.desc   Negative test: open device, error manufacturerName 'sdfsdfe'
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConnectDevice007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testConnectDevice007 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }

      let device: usb.USBDevice = JSON.parse(JSON.stringify(devices));
      device.manufacturerName = 'sdfsdfe';
      try {
        gPipe = usb.connectDevice(device);
        console.info(TAG, 'usb case testConnectDevice007 ret: ' + JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'usb testConnectDevice007 fail: ' + err);
        expect(err !== null).assertTrue();
      }
      toClosePipe('testConnectDevice007');
    })

    /**
     * @tc.name   testConnectDevice008
     * @tc.number SUB_USB_HostManager_JSDep_Compatibility_0700
     * @tc.desc   Negative test: open device, error version 'gwefsdf'
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConnectDevice008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb connect_device_08 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }

      let device: usb.USBDevice = JSON.parse(JSON.stringify(devices));
      device.version = 'gwefsdf';
      try {
        gPipe = usb.connectDevice(device);
        console.info(TAG, 'usb case testConnectDevice008 ret: ' + JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'usb testConnectDevice008 fail: ' + err);
        expect(err !== null).assertTrue();
      }
      toClosePipe('testConnectDevice008');
    })

    /**
     * @tc.name   testConnectDevice009
     * @tc.number SUB_USB_HostManager_JSDep_Compatibility_0800
     * @tc.desc   Negative test: open device, error vendorId 2+10000
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConnectDevice009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testConnectDevice009 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return
      }

      let device: usb.USBDevice = JSON.parse(JSON.stringify(devices));
      device.vendorId = 2 + 10000;
      try {
        gPipe = usb.connectDevice(device);
        console.info(TAG, 'usb case testConnectDevice009 ret: ' + JSON.stringify(gPipe));
        expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
      } catch (err) {
        console.info(TAG, 'usb testConnectDevice009 fail: ' + err);
        expect(err !== null).assertTrue();
      }
      toClosePipe('testConnectDevice009');
    })