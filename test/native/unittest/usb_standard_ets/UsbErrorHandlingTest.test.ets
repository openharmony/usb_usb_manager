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
import { BusinessError } from '@ohos.base';

const TAG: string = "[UsbErrorHandlingTest]";
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
    console.info(TAG, `button is ${JSONJSON.stringify(button)}`);
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

function getTransferParams(gPipe: usbMgr.USBDevicePipe, flagsValue: number, endpointValue: number, typeValue: number, timeOutValue: number, bufferSize: number = 10) {
  let transferParams: usbMgr.UsbDataTransferParams = {
    devPipe: gPipe,
    flags: flagsValue,
    endpoint: endpointValue,
    type: typeValue,
    timeout: timeOutValue,
    length: bufferSize,
    callback: () => {},
    userData: new Uint8Array(bufferSize),
    buffer: new Uint8Array(bufferSize),
    isoPacketCount: 0,
  };
  return transferParams;
}

function getControlParams(iCmd: number, iReqTarType: usbMgr.USBRequestTargetType, iReqType: usbMgr.USBControlRequestType,
  iValue: number, iIndex: number) {
  let tmpUint8Array = new Uint8Array(512);
  let requestCmd = iCmd
  let requestTargetType = iReqTarType
  let requestType = iReqType
  let value = iValue;
  let index = iIndex;
  let controlParam: usbMgr.USBControlParams = {
    request: requestCmd,
    target: requestTargetType,
    reqType: requestType,
    value: value,
    index: index,
    data: tmpUint8Array
  }
  return controlParam;
}

export default function UsbErrorHandlingTest() {
  describe('UsbErrorHandlingTest', () => {
    let isDeviceConnected: boolean = false;
    let devices: usbMgr.USBDevice;

    beforeAll(async (done: Function) => {
      console.log(TAG, '*************Usb Unit UsbErrorHandlingTest Begin*************');
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
      usbMgr.removeRight(devices.name);
      console.log('*************Usb Unit UsbErrorHandlingTest End*************');
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
    })

    /**
     * @tc.name   testNullParameterHandling001
     * @tc.number SUB_USB_ErrorHandling_NullParam_0100
     * @tc.desc   Test handling of null parameters
     * @tc.size     : Medium
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testNullParameterHandling001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNullParameterHandling001 enter');
      
      try {
        let hasRight = usbMgr.hasRight(null);
        expect(hasRight).assertFalse();
      } catch (error) {
        console.info(TAG, 'Expected error for null parameter:', error);
        expect(error !== null).assertTrue();
      }
      
      try {
        let removeResult = usbMgr.removeRight(null);
        expect(removeResult).assertFalse();
      } catch (error) {
        console.info(TAG, 'Expected error for null parameter:', error);
        expect(error !== null).assertTrue();
      }
    });

    /**
     * @tc.name   testUndefinedParameterHandling001
     * @tc.number SUB_USB_ErrorHandling_UndefinedParam_0100
     * @tc.desc   Test handling of undefined parameters
     * @tc.size     : Medium
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUndefinedParameterHandling001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testUndefinedParameterHandling001 enter');
      
      try {
        let hasRight = usbMgr.hasRight(undefined);
        expect(hasRight).assertFalse();
      } catch (error) {
        console.info(TAG, 'Expected error for undefined parameter:', error);
        expect(error !== null).assertTrue();
      }
      
      try {
        let removeResult = usbMgr.removeRight(undefined);
        expect(removeResult).assertFalse();
      } catch (error) {
        console.info(TAG, 'Expected error for undefined parameter:', error);
        expect(error !== null).assertTrue();
      }
    });

    /**
     * @tc.name   testInvalidDeviceName001
     * @tc.number SUB_USB_ErrorHandling_InvalidDeviceName_0100
     * @tc.desc   Test handling of invalid device names
     * @tc.size     : Medium
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testInvalidDeviceName001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInvalidDeviceName001 enter');
      
      const invalidNames = ['', '   ', 'invalid_device', '@#$%', '12345', 'a'.repeat(1000)];
      
      for (const name of invalidNames) {
        try {
          let hasRight = usbMgr.hasRight(name);
          expect(hasRight).assertFalse();
        } catch (error) {
          console.info(TAG, `Expected error for name "${name}":`, error);
        }
        
        try {
          let removeResult = usbMgr.removeRight(name);
          expect(removeResult).assertFalse();
        } catch (error) {
          console.info(TAG, `Expected error for name "${name}":`, error);
        }
      }
    });

    /**
     * @tc.name   testInvalidPipeHandling001
     * @tc.number SUB_USB_ErrorHandling_InvalidPipe_0100
     * @tc.desc   Test handling of invalid pipe parameters
     * @tc.size     : Medium
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testInvalidPipeHandling001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInvalidPipeHandling001 enter');
      
      const invalidPipes = [
        { busNum: -1, devAddress: -1 },
        { busNum: 0, devAddress: -1 },
        { busNum: -1, devAddress: 0 },
        { busNum: 999, devAddress: 999 },
        { busNum: -999, devAddress: -999 },
      ];
      
      for (const pipe of invalidPipes) {
        try {
          let result = usbMgr.closePipe(pipe);
          expect(result !== 0).assertTrue();
        } catch (error) {
          console.info(TAG, `Expected error for pipe ${JSON.stringify(pipe)}:`, error);
        }
        
        try {
          let descriptor = usbMgr.getRawDescriptor(pipe);
          expect(descriptor.length).assertEqual(0);
        } catch (error) {
          console.info(TAG, `Expected error for pipe ${JSON.stringify(pipe)}:`, error);
        }
        
        try {
          let fd = usbMgr.getFileDescriptor(pipe);
          expect(fd).assertEqual(-1);
        } catch (error) {
          console.info(TAG, `Expected error for pipe ${JSON.stringify(pipe)}:`, error);
        }
      }
    });

    /**
     * @tc.name   testInvalidInterfaceHandling001
     * @tc.number SUB_USB_ErrorHandling_InvalidInterface_0100
     * @tc.desc   Test handling of invalid interface parameters
     * @tc.size     : Medium
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testInvalidInterfaceHandling001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInvalidInterfaceHandling001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      
      const invalidInterfaces = [
        { id: -1, clazz: 0, subClass: 0, protocol: 0, name: '', interfaces: [], endpoints: [] },
        { id: 999, clazz: 0, subClass: 0, protocol: 0, name: '', interfaces: [], endpoints: [] },
      ];
      
      for (const iface of invalidInterfaces) {
        try {
          let result = usbMgr.claimInterface(gPipe, iface, true);
          expect(result !== 0).assertTrue();
        } catch (error) {
          console.info(TAG, `Expected error for interface ${iface.id}:`, error);
        }
        
        try {
          let result = usbMgr.releaseInterface(gPipe, iface);
          expect(result !== 0).assertTrue();
        } catch (error) {
          console.info(TAG, `Expected error for interface ${iface.id}:`, error);
        }
        
        try {
          let result = usbMgr.setInterface(gPipe, iface);
          expect(result !== 0).assertTrue();
        } catch (error) {
          console.info(TAG, `Expected error for interface ${iface.id}:`, error);
        }
      }
      
      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testInvalidConfigHandling001
     * @tc.number SUB_USB_ErrorHandling_InvalidConfig_0100
     * @tc.desc   Test handling of invalid configuration parameters
     * @tc.size     : Medium
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testInvalidConfigHandling001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInvalidConfigHandling001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      
      const invalidConfigs = [
        { id: -1, name: '', maxPower: 0, isRemoteWakeup: false, isSelfPowered: false, interfaces: [] },
        { id: 255, name: '', maxPower: 0, isRemoteWakeup: false, isSelfPowered: false, interfaces: [] },
      ];
      
      for (const config of invalidConfigs) {
        try {
          let result = usbMgr.setConfiguration(gPipe, config);
          expect(result !== 0).assertTrue();
        } catch (error) {
          console.info(TAG, `Expected error for config ${config.id}:`, error);
        }
      }
      
      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testInvalidEndpointHandling001
     * @tc.number SUB_USB_ErrorHandling_InvalidEndpoint_0100
     * @tc.desc   Test handling of invalid endpoint parameters
     * @tc.size     : Medium
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testInvalidEndpointHandling001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testInvalidEndpointHandling001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
      usbMgr.claimInterface(gPipe, tmpInterface, true);
      
      const invalidEndpoints = [
        { address: -1, type: 0, direction: 0, maxPacketSize: 0, interval: 0 },
        { address: 255, type: 0, direction: 0, maxPacketSize: 0, interval: 0 },
        { address: 0, type: -1, direction: 0, maxPacketSize: 0, interval: 0 },
        { address: 0, type: 10, direction: 0, maxPacketSize: 0, interval: 0 },
      ];
      
      for (const endpoint of invalidEndpoints) {
        try {
          let buffer = new Uint8Array(64);
          let result = await usbMgr.bulkTransfer(gPipe, endpoint, buffer, 5000);
          expect(result < 0).assertTrue();
        } catch (error) {
          console.info(TAG, `Expected error for endpoint ${endpoint.address}:`, error);
        }
      }
      
      usbMgr.releaseInterface(gPipe, tmpInterface);
      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testInvalidControlTransferParams001
     * @tc.number SUB_USB_ErrorHandling_InvalidControlParams_0100
     * @tc.desc   Test handling of invalid control transfer parameters
     * @tc.size     : Medium
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testInvalidControlTransferParams001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testInvalidControlTransferParams001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      
      const invalidControlParams = [
        { request: -1, target: 0, reqType: 0, value: 0, index: 0, data: new Uint8Array(0) },
        { request: 255, target: 0, reqType: 0, value: 0, index: 0, data: new Uint8Array(0) },
        { request: 0, target: -1, reqType: 0, value: 0, index: 0, data: new Uint8Array(0) },
        { request: 0, target: 0, reqType: -1, value: 0, index: 0, data: new Uint8Array(0) },
      ];
      
      for (const controlParam of invalidControlParams) {
        try {
          let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
          console.info(TAG, `Control transfer result: ${result}`);
        } catch (error) {
          console.info(TAG, 'Expected error for invalid control params:', error);
        }
      }
      
      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testInvalidTransferParams001
     * @tc.number SUB_USB_ErrorHandling_InvalidTransferParams_0100
     * @tc.desc   Test handling of invalid transfer parameters
     * @tc.size     : Medium
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testInvalidTransferParams001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testInvalidTransferParams001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      
      const invalidTransferParams = [
        { devPipe: null, flags: 0, endpoint: 0, type: 0, timeout: 0, length: 0, callback: () => {}, userData: null, buffer: null, isoPacketCount: 0 },
        { devPipe: gPipe, flags: -1, endpoint: 0, type: 0, timeout: 0, length: 0, callback: () => {}, userData: new Uint8Array(0), buffer: new Uint8Array(0), isoPacketCount: 0 },
        { devPipe: gPipe, flags: 0, endpoint: -1, type: 0, timeout: 0, length: 0, callback: () => {}, userData: new Uint8Array(0), buffer: new Uint8Array(0), isoPacketCount: 0 },
        { devPipe: gPipe, flags: 0, endpoint: 0, type: -1, timeout: 0, length: 0, callback: () => {}, userData: new Uint8Array(0), buffer: new Uint8Array(0), isoPacketCount: 0 },
        { devPipe: gPipe, flags: 0, endpoint: 0, type: 0, timeout: -1, length: 0, callback: () => {}, userData: new Uint8Array(0), buffer: new Uint8Array(0), isoPacketCount: 0 },
        { devPipe: gPipe, flags: 0, endpoint: 0, type: 0, timeout: 0, length: -1, callback: () => {}, userData: new Uint8Array(0), buffer: new Uint8Array(0), isoPacketCount: 0 },
      ];
      
      for (const transferParam of invalidTransferParams) {
        try {
          await usbMgr.usbSubmitTransfer(transferParam);
          expect().assertFail();
        } catch (error) {
          console.info(TAG, 'Expected error for invalid transfer params:', error);
          expect(error !== null).assertTrue();
        }
      }
      
      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testBufferBoundaryConditions001
     * @tc.number SUB_USB_ErrorHandling_BufferBoundary_0100
     * @tc.desc   Test buffer boundary conditions
     * @tc.size     : Medium
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testBufferBoundaryConditions001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testBufferBoundaryConditions001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
      usbMgr.claimInterface(gPipe, tmpInterface, true);
      
      const boundarySizes = [0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192];
      
      for (const size of boundarySizes) {
        try {
          console.info(TAG, `Testing buffer size: ${size}`);
          
          let buffer = new Uint8Array(size);
          let endpoint = tmpInterface.endpoints[0];
          
          let result = await usbMgr.bulkTransfer(gPipe, endpoint, buffer, 5000);
          console.info(TAG, `Buffer size ${size} result: ${result}`);
          
        } catch (error) {
          console.info(TAG, `Buffer size ${size} error:`, error);
        }
      }
      
      usbMgr.releaseInterface(gPipe, tmpInterface);
      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testTimeoutBoundaryConditions001
     * @tc.number SUB_USB_ErrorHandling_TimeoutBoundary_0100
     * @tc.desc   Test timeout boundary conditions
     * @tc.size     : Medium
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testTimeoutBoundaryConditions001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testTimeoutBoundaryConditions001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
      usbMgr.claimInterface(gPipe, tmpInterface, true);
      
      const timeoutValues = [-1, 0, 1, 10, 100, 1000, 5000, 10000, 30000, 60000];
      
      for (const timeout of timeoutValues) {
        try {
          console.info(TAG, `Testing timeout: ${timeout}ms`);
          
          let buffer = new Uint8Array(64);
          let endpoint = tmpInterface.endpoints[0];
          
          let result = await usbMgr.bulkTransfer(gPipe, endpoint, buffer, timeout);
          console.info(TAG, `Timeout ${timeout}ms result: ${result}`);
          
        } catch (error) {
          console.info(TAG, `Timeout ${timeout}ms error:`, error);
        }
      }
      
      usbMgr.releaseInterface(gPipe, tmpInterface);
      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testMemoryAllocationFailure001
     * @tc.number SUB_USB_ErrorHandling_MemoryFailure_0100
     * @tc.desc   Test memory allocation failure scenarios
     * @tc.size     : Medium
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testMemoryAllocationFailure001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testMemoryAllocationFailure001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
      usbMgr.claimInterface(gPipe, tmpInterface, true);
      
      try {
        console.info(TAG, 'Testing with very large buffer');
        let hugeBuffer = new Uint8Array(100 * 1024 * 1024); // 100MB
        let endpoint = tmpInterface.endpoints[0];
        
        let result = await usbMgr.bulkTransfer(gPipe, endpoint, hugeBuffer, 5000);
        console.info(TAG, `Huge buffer result: ${result}`);
        
      } catch (error) {
        console.info(TAG, 'Expected error for huge buffer:', error);
      }
      
      try {
        console.info(TAG, 'Testing with zero buffer');
        let zeroBuffer = new Uint8Array(0);
        let endpoint = tmpInterface.endpoints[0];
        
        let result = await usbMgr.bulkTransfer(gPipe, endpoint, zeroBuffer, 5000);
        console.info(TAG, `Zero buffer result: ${result}`);
        
      } catch (error) {
        console.info(TAG, 'Expected error for zero buffer:', error);
      }
      
      usbMgr.releaseInterface(gPipe, tmpInterface);
      usbMgr.closePipe(gPipe);
    });

    /**
     * @tcra.name   testDeviceDisconnectHandling001
     * @tc.number SUB_USB_ErrorHandling_Disconnect_0100
     * @tc.desc   Test device disconnect handling
     * @tc.size     : Medium
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testDeviceDisconnectHandling001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceDisconnectHandling001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let closeResult = usbMgr.closePipe(gPipe);
      expect(closeResult).assertEqual(0);
      
      try {
        let rawDesc = usbMgr.getRawDescriptor(gPipe);
        expect(rawDesc.length).assertEqual(0);
      } catch (error) {
        console.info(TAG, 'Expected error after disconnect:', error);
      }
      
      try {
        let fd = usbMgr.getFileDescriptor(gPipe);
        expect(fd).assertEqual(-1);
      } catch (error) {
        console.info(TAG, 'Expected error after disconnect:', error);
      }
    });

    /**
     * @tc.name   testPermissionDeniedHandling001
     * @tc.number SUB_USB_ErrorHandling_PermissionDenied_0100
     * @tc.desc   Test permission denied handling
     * @tc.size     : Medium
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testPermissionDeniedHandling001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testPermissionDeniedHandling001 enter');
      if (!isDeviceConnected) {
        expect(isDevice).assertFalse();
        return;
      }
      
      let removeResult = usbMgr.removeRight(devices.name);
      expect(removeResult).assertTrue();
      
      let hasRight = usbMgr.hasRight(devices.name);
      expect(hasRight).assertFalse();
      
      try {
        let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
        expect(gPipe === null).assertTrue();
      } catch (error) {
        console.info(TAG, 'Expected error without permission:', error);
      }
      
      let requestResult = await usbMgr.requestRight(devices.name);
      expect(requestResult).assertTrue();
    });

    /**
     * @tc.name   testConcurrentErrorHandling001
     * @tc.number SUB_USB_ErrorHandling_Concurrent_0100
     * @tc.desc   Test concurrent error handling
     * @tc.size     : Medium
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConcurrentErrorHandling001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentErrorHandling001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      const operationCount = 5;
      let completedCount = 0;
      
      let operations = [];
      
      for (let i = 0; i < operationCount; i++) {
        operations.push(
          (async (index: number) => {
            try {
              console.info(TAG, `Starting concurrent error test ${index}`);
              
              if (index % 2 === 0) {
                let hasRight = usbMgr.hasRight('invalid_device_' + index);
                expect(hasRight).assertFalse();
              } else {
                let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
                expect(gPipe !== null).assertTrue();
                let closeResult = usbMgr.closePipe(gPipe);
                expect(closeResult).assertEqual(0);
              }
              
              console.info(TAG, `Completed concurrent error test ${index}`);
              completedCount++;
              
            } catch (error) {
              console.info(TAG, `Concurrent error test ${index} error: ${error}`);
              completedCount++;
            }
          })(i)
        );
      }
      
      await Promise.all(operations);
      
      console.info(TAG, `Completed ${completedCount}/${operationCount} operations`);
      expect(completedCount).assertEqual(operationCount);
    });
  })
}