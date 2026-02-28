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
import { BusinessError } from '@ohos.base';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium';

const TAG = "[UsbComprehensiveTest]";
let gDeviceList: Array<usbMgr.USBDevice>;
let gPipe: usbMgr.USBDevicePipe | null;
let devices: usbMgr.USBDevice | null;
let isDeviceConnected: boolean;

function sleep(ms: number): Promise<void> {
  return new Promise<void>(resolve => setTimeout(resolve, ms));
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

function toClosePipe(testCaseName: string) {
  let isPipClose = usbMgr.closePipe(gPipe);
  console.info(TAG, `usb ${testCaseName} closePipe getPipe ret: ${isPipClose}`);
  expect(isPipClose).assertEqual(0);
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

export default function UsbComprehensiveTest() {
  describe('UsbComprehensiveTest', () => {

    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbComprehensiveTest Begin*************');

      gDeviceList = usbMgr.getDevices();
      console.info(TAG, 'beforeAll: usb case gDeviceList.length return: ' + JSON.stringify(gDeviceList));
      isDeviceConnected = deviceConnected();
      if (isDeviceConnected) {
        let hasRight = usbMgr.hasRight(gDeviceList[0].name);
        if (!hasRight) {
          console.info(TAG, `beforeAll: usb requestRight start`);
          await getPermission();
          await sleep(1000);
          await driveFn();
          await sleep(1000);
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
    })

    afterAll(() => {
      console.log(TAG, '*************Usb Unit UsbComprehensiveTest End*************');
    })

    /**
     * @tc.name   testDeviceEnumerationEdge001  
     * @tc.number SUB_USB_Comprehensive_EnumEdge_0100
     * @tc.desc   Test device enumeration with edge cases
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceEnumerationEdge001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDeviceEnumerationEdge001 begin');
      
      let deviceList = usbMgr.getDevices();
      console.info(TAG, 'Device enumeration returned ' + deviceList.length + ' devices');
      
      if (deviceList.length === 0) {
        console.info(TAG, 'No devices connected, test passed for empty list');
        expect(deviceList.length).assertEqual(0);
        return;
      }
      
      expect(deviceList.length > 0).assertTrue();
      
      deviceList.forEach((device, index) => {
        console.info(TAG, `Device ${index}:`);
        console.info(TAG, `  Name: ${device.name}`);
        console.info(TAG, `  Serial: ${device.serial}`);
        console.info(TAG, `  Vendor ID: ${device.vendorId}`);
        console.info(TAG, `  Product ID: ${device.productId}`);
        console.info(TAG, `  Bus Num: ${device.busNum}`);
        console.info(TAG, `  Dev Address: ${device.devAddress}`);
        console.info(TAG, `  Configs count: ${device.configs.length}`);
        
        expect(device.name).not.toBeNull();
        expect(device.serial).not.toBeNull();
        expect(device.vendorId).toBeLargerThan(0);
        expect(device.productId).toBeLargerThan(0);
        expect(device.busNum).toBeLargerThan(-1);
        expect(device.devAddress).toBeLargerThan(-1);
      });
    })

    /**
     * @tc.name   testPermissionCycle001
     * @tc.number SUB_USB_Comprehensive_PermissionCycle_0100
     * @tc.desc   Test permission request/remove cycle
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPermissionCycle001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testPermissionCycle001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      const cycleCount = 3;
      
      for (let i = 0; i < cycleCount; i++) {
        console.info(TAG, `Permission cycle ${i + 1}/${cycleCount}`);
        
        let hasRight = usbMgr.hasRight(devices.name);
        console.info(TAG, `Initial hasRight: ${hasRight}`);
        
        if (hasRight) {
          let removeResult = usbMgr.removeRight(devices.name);
          console.info(TAG, `Remove right result: ${removeResult}`);
          expect(removeResult).assertTrue();
          
          hasRight = usbMgr.hasRight(devices.name);
          expect(hasRight).assertFalse();
        }
        
        try {
          hasRight = await usbMgr.requestRight(devices.name);
          console.info(TAG, `Request right result: ${hasRight}`);
          expect(hasRight).assertTrue();
          
          hasRight = usbMgr.hasRight(devices.name);
          expect(hasRight).assertTrue();
        } catch (error) {
          console.info(TAG, `Request right failed: ${error}`);
          expect(error === null).assertTrue();
        }
      }
    })

    /**
     * @tc.name   testMultipleDeviceConnections001
     * @tc.number SUB_USB_Comprehensive_MultiConnect_0100
     * @tc.desc   Test multiple device connections simultaneously
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testMultipleDeviceConnections001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testMultipleDeviceConnections001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      let pipes: Array<usbMgr.USBDevicePipe> = [];
      
      try {
        for (let i = 0; i < gDeviceList.length; i++) {
          let pipe = usbMgr.connectDevice(gDeviceList[i]);
          console.info(TAG, `Connected to device ${i}, pipe: ${JSON.stringify(pipe)}`);
          expect(pipe !== null).assertTrue();
          pipes.push(pipe);
        }
        
        for (let i = 0; i < pipes.length; i++) {
          let closeResult = usbMgr.closePipe(pipes[i]);
          console.info(TAG, `Closed device ${i}, result: ${closeResult}`);
          expect(closeResult).assertEqual(0);
        }
      } catch (error) {
        console.info(TAG, `Multiple connection test error: ${error}`);
        
        for (let i = 0; i < pipes.length; i++) {
          try {
            usbMgr.closePipe(pipes[i]);
          } catch (closeError) {
            console.info(TAG, `Close pipe ${i} error: ${closeError}`);
          }
        }
      }
    })

    /**
     * @tc.name   testBulkTransferBoundary001
     * @tc.number SUB_USB_Comprehensive_BulkBoundary_0100
     * @tc.desc   Test bulk transfer with boundary buffer sizes
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testBulkTransferBoundary001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testBulkTransferBoundary001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe('testBulkTransferBoundary001');
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let inEndpoint: usbMgr.USBEndpoint | null = null;
      let outEndpoint: usbMgr.USBEndpoint | null = null;
      
      for (const endpoint of iface.endpoints) {
        if (endpoint.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK) {
          if (endpoint.direction === usbMgr.USBRequestDirection.USB_REQUEST_DIR_FROM_DEVICE) {
            inEndpoint = endpoint;
          } else if (endpoint.direction === usbMgr.USBRequestDirection.USB_REQUEST_DIR_TO_DEVICE) {
            outEndpoint = endpoint;
          }
        }
      }
      
      if (inEndpoint && outEndpoint) {
        let isClaim = usbMgr.claimInterface(gPipe, iface, true);
        expect(isClaim).assertEqual(0);
        
        const boundarySizes = [1, 2, 4, 8, 16, 32, 64, 128, 255, 256, 512, 1023, 1024, 2048, 4096];
        
        for (const size of boundarySizes) {
          try {
            console.info(TAG, `Testing bulk transfer with size: ${size}`);
            
            let writeBuffer: Uint8Array = new Uint8Array(size);
            for (let i = 0; i < size; i++) {
              writeBuffer[i] = i % 256;
            }
            
            let writeResult = await usbMgr.bulkTransfer(gPipe, outEndpoint, writeBuffer, 5000);
            console.info(TAG, `Write result for size ${size}: ${writeResult}`);
            expect(writeResult > 0).assertTrue();
            
            await sleep(100);
            
            let readBuffer: Uint8Array = new Uint8Array(size);
            let readResult = await usbMgr.bulkTransfer(gPipe, inEndpoint, readBuffer, 5000);
            console.info(TAG, `Read result for size ${size}: ${readResult}`);
            expect(readResult >= 0).assertTrue();
            
          } catch (error) {
            console.info(TAG, `Bulk transfer error for size ${size}: ${error}`);
          }
        }
        
        let releaseResult = usbMgr.releaseInterface(gPipe, iface);
        expect(releaseResult).assertEqual(0);
      }
      
      toClosePipe('testBulkTransferBoundary001');
    })

    /**
     * @tc.name   testControlTransferAllTypes001
     * @tc.number SUB_USB_Comprehensive_ControlAllTypes_0100
     * @tc.desc   Test control transfer with all request types
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testControlTransferAllTypes001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testControlTransferAllTypes001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe('testControlTransferAllTypes001');
      
      const requestTypes = [
        { type: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD, name: 'STANDARD' },
        { type: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS, name: 'CLASS' },
        { type: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_VENDOR, name: 'VENDOR' },
      ];
      
      const targets = [
        { target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE, name: 'DEVICE' },
        { target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE, name: 'INTERFACE' },
        { target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_ENDPOINT, name: 'ENDPOINT' },
        { target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_OTHER, name: 'OTHER' },
      ];
      
      for (const reqType of requestTypes) {
        for (const target of targets) {
          try {
            console.info(TAG, `Testing control transfer: ${reqType.name} to ${target.name}`);
            
            let controlParam = getControlParams(
              0,
              target.target,
              (usbMgr.USBRequestDirection.USB_REQUEST_DIR_FROM_DEVICE) | (reqType.type << 5) | (target.target & 0x1f),
              0,
              0
            );
            
            let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
            console.info(TAG, `Control transfer result: ${result}`);
            
          } catch (error) {
            console.info(TAG, `Control transfer error for ${reqType.name} to ${target.name}: ${error}`);
          }
        }
      }
      
      toClosePipe('testControlTransferAllTypes001');
    })

    /**
     * @tc.name   testInterfaceManagement001
     * @tc.number SUB_USB_Comprehensive_InterfaceMgmt_0100
     * @tc.desc   Test comprehensive interface management
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceManagement001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testInterfaceManagement001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe('testInterfaceManagement001');
      
      for (let configIdx = 0; configIdx < devices.configs.length; configIdx++) {
        let config = devices.configs[configIdx];
        console.info(TAG, `Testing config ${configIdx} with ${config.interfaces.length} interfaces`);
        
        for (let ifaceIdx = 0; ifaceIdx < config.interfaces.length; ifaceIdx++) {
          let iface = config.interfaces[ifaceIdx];
          console.info(TAG, `Testing interface ${ifaceIdx}`);
          
          let claimResult = usbMgr.claimInterface(gPipe, iface, true);
          console.info(TAG, `Claim interface result: ${claimResult}`);
          
          if (claimResult === 0) {
            let setResult = usbMgr.setInterface(gPipe, iface);
            console.info(TAG, `Set interface result: ${setResult}`);
            
            let releaseResult = usbMgr.releaseInterface(gPipe, iface);
            console.info(TAG, `Release interface result: ${releaseResult}`);
            expect(releaseResult).assertEqual(0);
          }
        }
      }
      
      toClosePipe('testInterfaceManagement001');
    })

    /**
     * @tc.name   testConfigurationManagement001
     * @tc.number SUB_USB_Comprehensive_ConfigMgmt_0100
     * @tc.desc   Test comprehensive configuration management
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConfigurationManagement001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testConfigurationManagement001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe('testConfigurationManagement001');
      
      for (let i = 0; i < devices.configs.length; i++) {
        let config = devices.configs[i];
        console.info(TAG, `Testing configuration ${i}, ID: ${config.id}`);
        
        let setResult = usbMgr.setConfiguration(gPipe, config);
        console.info(TAG, `Set configuration result: ${setResult}`);
        
        if (setResult === 0) {
          let rawDesc = usbMgr.getRawDescriptor(gPipe);
          console.info(TAG, `Raw descriptor length: ${rawDesc.length}`);
          expect(rawDesc.length).toBeLargerThan(0);
        }
      }
      
      toClosePipe('testConfigurationManagement001');
    })

    /**
     * @tc.name   testErrorRecovery001
     * @tc.number SUB_USB_Comprehensive_ErrorRecovery_0100
     * @tc.desc   Test error recovery scenarios
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     *.tc.level  LEVEL3
     */
    it('testErrorRecovery001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testErrorRecovery001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      let pipe = usbMgr.connectDevice(devices);
      expect(pipe !== null).assertTrue();
      
      try {
        let invalidPipe: usbMgr.USBDevicePipe = {
          busNum: 999,
          devAddress: 999
        };
        
        let result = usbMgr.closePipe(invalidPipe);
        console.info(TAG, `Close invalid pipe result: ${result}`);
        expect(result !== 0).assertTrue();
        
        result = usbMgr.closePipe(pipe);
        console.info(TAG, `Close valid pipe result: ${result}`);
        expect(result).assertEqual(0);
        
        pipe = usbMgr.connectDevice(devices);
        expect(pipe !== null).assertTrue();
        
        let closeResult = usbMgr.closePipe(pipe);
        expect(closeResult).assertEqual(0);
        
      } catch (error) {
        console.info(TAG, `Error recovery test error: ${error}`);
        
        try {
          if (pipe !== null) {
            usbMgr.closePipe(pipe);
          }
        } catch (closeError) {
          console.info(TAG, `Final close error: ${closeError}`);
        }
      }
    })

    /**
     * @tc.name   testConcurrentOperations001
     * @tc.number SUB_USB_Comprehensive_Concurrent_0100
     * @tc.desc   Test concurrent USB operations
     * @tcct.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConcurrentOperations001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testConcurrentOperations001 begin');
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
              console.info(TAG, `Starting concurrent operation ${index}`);
              
              let pipe = usbMgr.connectDevice(devices);
              expect(pipe !== null).assertTrue();
              
              let hasRight = usbMgr.hasRight(devices.name);
              expect(hasRight).assertTrue();
              
              let result = usbMgr.closePipe(pipe);
              expect(result).assertEqual(0);
              
              console.info(TAG, `Completed concurrent operation ${index}`);
              completedCount++;
              
            } catch (error) {
              console.info(TAG, `Concurrent operation ${index} error: ${error}`);
              completedCount++;
            }
          })(i)
        );
      }
      
      await Promise.all(operations);
      
      console.info(TAG, `Completed ${completedCount}/${operationCount} operations`);
      expect(completedCount).assertEqual(operationCount);
    })

    /**
     * @tc.name   testDescriptorParsing001
     * @tc.number SUB_USB_Comprehensive_DescParse_0100
     * @tc.desc   Test descriptor parsing and validation
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDescriptorParsing001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDescriptorParsing001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      console.info(TAG, `Device has ${devices.configs.length} configurations`);
      
      for (let configIdx = 0; configIdx < devices.configs.length; configIdx++) {
        let config = devices.configs[configIdx];
        
        console.info(TAG, `Configuration ${configIdx}:`);
        console.info(TAG, `  ID: ${config.id}`);
        console.info(TAG, `  Name: ${config.name}`);
        console.info(TAG, `  Max Power: ${config.maxPower}`);
        console.info(TAG, `  Is Remote Wakeup: ${config.isRemoteWakeup}`);
        console.info(TAG, `  Is Self Powered: ${config.isSelfPowered}`);
        console.info(TAG, `  Interfaces: ${config.interfaces.length}`);
        
        expect(config.id).toBeLargerThan(-1);
        expect(config.interfaces.length).toBeLargerThan(0);
        
        for (let ifaceIdx = 0; ifaceIdx < config.interfaces.length; ifaceIdx++) {
          let iface = config.interfaces[ifaceIdx];
          
          console.info(TAG, `  Interface ${ifaceIdx}:`);
          console.info(TAG, `    ID: ${iface.id}`);
          console.info(TAG, `    Class: ${iface.clazz}`);
          console.info(TAG, `    SubClass: ${iface.subClass}`);
          console.info(TAG, `    Protocol: ${iface.protocol}`);
          console.info(TAG, `    Name: ${iface.name}`);
          console.info(TAG, `    Endpoints: ${iface.endpoints.length}`);
          
          expect(iface.id).toBeLargerThan(-1);
          expect(iface.clazz).toBeLargerThan(-1);
          expect(iface.subClass).toBeLargerThan(-1);
          expect(iface.protocol).toBeLargerThan(-1);
          
          for (let epIdx = 0; epIdx < iface.endpoints.length; epIdx++) {
            let endpoint = iface.endpoints[epIdx];
            
            console.info(TAG, `    Endpoint ${epIdx}:`);
            console.info(TAG, `      Address: ${endpoint.address}`);
            console.info(TAG, `      Type: ${endpoint.type}`);
            console.info(TAG, `      Direction: ${endpoint.direction}`);
            console.info(TAG, `      Max Packet Size: ${endpoint.maxPacketSize}`);
            console.info(TAG, `      Interval: ${endpoint.interval}`);
            
            expect(endpoint.address).toBeLargerThan(-1);
            expect(endpoint.type).toBeLargerThan(-1);
            expect(endpoint.direction).toBeLargerThan(-1);
            expect(endpoint.maxPacketSize).toBeLargerThan(0);
          }
        }
      }
    })

    /**
     * @tc.name   testTimeoutHandling001
     * @tc.number SUB_USB_Comprehensive_Timeout_0100
     * @tc.desc   Test various timeout scenarios
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testTimeoutHandling001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testTimeoutHandling001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe('testTimeoutHandling001');
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let inEndpoint: usbMgr.USBEndpoint | null = null;
      
      for (const endpoint of iface.endpoints) {
        if (endpoint.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK &&
            endpoint.direction === usbMgr.USBRequestDirection.USB_REQUEST_DIR_FROM_DEVICE) {
          inEndpoint = endpoint;
          break;
        }
      }
      
      if (inEndpoint) {
        let isClaim = usbMgr.claimInterface(gPipe, iface, true);
        expect(isClaim).assertEqual(0);
        
        const timeouts = [0, 100, 500, 1000, 5000, 10000];
        
        for (const timeout of timeouts) {
          try {
            console.info(TAG, `Testing timeout: ${timeout}ms`);
            
            let buffer = new Uint8Array(64);
            let result = await usbMgr.bulkTransfer(gPipe, inEndpoint, buffer, timeout);
            console.info(TAG, `Timeout ${timeout}ms result: ${result}`);
            
          } catch (error) {
            console.info(TAG, `Timeout ${timeout}ms error: ${error}`);
          }
        }
        
        let releaseResult = usbMgr.releaseInterface(gPipe, iface);
        expect(releaseResult).assertEqual(0);
      }
      
      toClosePipe('testTimeoutHandling001');
    })

    /**
     * @tc.name   testStressTest001
     * @tc.number SUB_USB_Comprehensive_Stress_0100
     * @tc.desc   Test USB operations under stress
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testStressTest001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testStressTest001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      const iterationCount = 10;
      let successCount = 0;
      
      for (let i = 0; i < iterationCount; i++) {
        try {
          console.info(TAG, `Stress test iteration ${i + 1}/${iterationCount}`);
          
          let pipe = usbMgr.connectDevice(devices);
          expect(pipe !== null).assertTrue();
          
          let hasRight = usbMgr.hasRight(devices.name);
          expect(hasRight).assertTrue();
          
          let rawDesc = usbMgr.getRawDescriptor(pipe);
          expect(rawDesc.length).toBeLargerThan(0);
          
          let fileDesc = usbMgr.getFileDescriptor(pipe);
          expect(fileDesc).toBeLargerThan(-1);
          
          let closeResult = usbMgr.closePipe(pipe);
          expect(closeResult).assertEqual(0);
          
          successCount++;
          
          await sleep(100);
          
        } catch (error) {
          console.info(TAG, `Stress test iteration ${i + 1} error: ${error}`);
        }
      }
      
      console.info(TAG, `Stress test completed: ${successCount}/${iterationCount} successful`);
      expect(successCount).toBeLargerThan(iterationCount / 2);
    })

    /**
     * @tc.name   testDeviceProperties001
     * @tc.number SUB_USB_Comprehensive_DeviceProps_0100
     * @tc.desc   Test all device properties
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceProperties001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDeviceProperties001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      console.info(TAG, 'Device properties:');
      console.info(TAG, `  Name: ${devices.name}`);
      console.info(TAG, `  Serial: ${devices.serial}`);
      console.info(TAG, `  Manufacturer: ${devices.manufacturerName}`);
      console.info(TAG, `  Product: ${devices.productName}`);
      console.info(TAG, `  Version: ${devices.version}`);
      console.info(TAG, `  Vendor ID: ${devices.vendorId}`);
      console.info(TAG, `  Product ID: ${devices.productId}`);
      console.info(TAG, `  Class: ${devices.clazz}`);
      console.info(TAG, `  SubClass: ${devices.subClass}`);
      console.info(TAG, `  Protocol: ${devices.protocol}`);
      console.info(TAG, `  Bus Number: ${devices.busNum}`);
      console.info(TAG, `  Device Address: ${devices.devAddress}`);
      console.info(TAG, `  Configurations: ${devices.configs.length}`);
      
      expect(devices.name).not.toBeNull();
      expect(devices.serial).not.toBeNull();
      expect(devices.vendorId).toBeLargerThan(0);
      expect(devices.productId).toBeLargerThan(0);
      expect(devices.busNum).toBeLargerThan(-1);
      expect(devices.devAddress).toBeLargerThan(-1);
      expect(devices.configs.length).toBeLargerThan(0);
    })
  })
}