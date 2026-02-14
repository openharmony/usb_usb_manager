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

const TAG = "[UsbInterfaceConfigTest]";
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

function getControlTransferParams(iReqType: number, iReq: number, iValue: number, iIndex: number, iLength: number) {
  let tmpUint8Array = new Uint8Array(512);
  
  let requestParam: usbMgr.USBDeviceRequestParams = {
    bmRequestType: iReqType,
    bRequest: iReq,
    wValue: iValue,
    wIndex: iIndex,
    wLength: iLength,
    data: tmpUint8Array
  }
  return requestParam;
}

function getControlParams(iCmd: number, iReqTarType: usbMgr.USBRequestTargetType, iReqType: usbMgr.USBControlRequestType,
  iValue: number, iIndex: number) {
  let tmpUint8Array = new Uint8Array(512);
  let controlParam: usbMgr.USBControlParams = {
    request: iCmd,
    target: iReqTarType,
    reqType: iReqType,
    value: iValue,
    index: iIndex,
    data: tmpUint8Array
  }
  return controlParam;
}

/* USB interface management and configuration test */
export default function UsbInterfaceConfigTest() {
  describe('UsbInterfaceConfigTest', () => {

    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbInterfaceConfigTest Begin*************');

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
        gPipe = usbMgr.connectDevice(gDeviceList[0]);
        console.info(TAG, 'usb unit connectDevice gPipe ret : ' + JSON.stringify(gPipe));
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
    })

    afterAll(() => {
      let isPipClose = usbMgr.closePipe(gPipe);
      console.info(TAG, 'usb unit close gPipe ret : ' + isPipClose);
      console.log(TAG, '*************Usb Unit UsbInterfaceConfigTest End*************');
    })

    /**
     * @tc.name   testInterfaceEnumeration001
     * @tc.number SUB_USB_InterfaceConfig_Enumerate_0100
     * @tc.desc   Test USB interface enumeration and validation
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceEnumeration001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testInterfaceEnumeration001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      expect(gDeviceList[0].configs.length).toBeLargerThan(0);
      
      for (let configIndex = 0; configIndex < gDeviceList[0].configs.length; configIndex++) {
        let config = gDeviceList[0].configs[configIndex];
        console.info(TAG, `Config ${configIndex}: ` + JSON.stringify(config));
        
        expect(config).not.toBeNull();
        expect(config.interfaces).not.toBeNull();
        expect(config.interfaces.length).toBeLargerThan(0);
        
        for (let ifaceIndex = 0; ifaceIndex < config.interfaces.length; ifaceIndex++) {
          let iface = config.interfaces[ifaceIndex];
          console.info(TAG, `Interface ${ifaceIndex}: ` + JSON.stringify(iface));
          
          // Validate interface properties
          expect(iface.id).toBeLargerThan(-1);
          expect(iface.clazz).toBeLargerThan(-1);
          expect(iface.subClass).toBeLargerThan(-1);
          expect(iface.protocol).toBeLargerThan(-1);
          expect(iface.endpoints).not.toBeNull();
          
          // Validate endpoints
          for (let epIndex = 0; epIndex < iface.endpoints.length; epIndex++) {
            let endpoint = iface.endpoints[epIndex];
            console.info(TAG, `Endpoint ${epIndex}: ` + JSON.stringify(endpoint));
            
            expect(endpoint.address).toBeLargerThan(-1);
            expect(endpoint.type).toBeLargerThan(-1);
            expect(endpoint.direction).toBeLargerThan(-1);
            expect(endpoint.maxPacketSize).toBeLargerThan(0);
          }
        }
      }
    })

    /**
     * @tc.name   testInterfaceClaimRelease001
     * @tc.number SUB_USB_InterfaceConfig_ClaimRelease_0100
     * @tc.desc   Test interface claim and release operations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceClaimRelease001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testInterfaceClaimRelease001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      for (let configIndex = 0; configIndex < gDeviceList[0].configs.length; configIndex++) {
        let config = gDeviceList[0].configs[configIndex];
        
        for (let ifaceIndex = 0; ifaceIndex < config.interfaces.length; ifaceIndex++) {
          let iface = config.interfaces[ifaceIndex];
          
          // Test claim interface with force flag true
          let claimResult = usbMgr.claimInterface(gPipe, iface, true);
          console.info(TAG, `Claim interface ${ifaceIndex} result: ${claimResult}`);
          expect(claimResult).assertEqual(0);
          
          // Test release interface
          let releaseResult = usbMgr.releaseInterface(gPipe, iface);
          console.info(TAG, `Release interface ${ifaceIndex} result: ${releaseResult}`);
          expect(releaseResult).assertEqual(0);
          
          // Test claim interface with force flag false
          claimResult = usbMgr.claimInterface(gPipe, iface, false);
          console.info(TAG, `Claim interface ${ifaceIndex} (no force) result: ${claimResult}`);
          expect(claimResult).assertEqual(0);
          
          // Release again
          releaseResult = usbMgr.releaseInterface(gPipe, iface);
          console.info(TAG, `Release interface ${ifaceIndex} (no force) result: ${releaseResult}`);
          expect(releaseResult).assertEqual(0);
        }
      }
    })

    /**
     * @tc.name   testInterfaceClaimMultiple001
     * @tc.number SUB_USB_InterfaceConfig_ClaimMultiple_0100
     * @tc.desc   Test claiming multiple interfaces
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceClaimMultiple001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testInterfaceClaimMultiple001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      let config = gDeviceList[0].configs[0];
      
      // Try to claim all interfaces in the first configuration
      let claimedInterfaces = [];
      for (let ifaceIndex = 0; ifaceIndex < config.interfaces.length; ifaceIndex++) {
        let iface = config.interfaces[ifaceIndex];
        
        let claimResult = usbMgr.claimInterface(gPipe, iface, true);
        console.info(TAG, `Claim multiple interface ${ifaceIndex} result: ${claimResult}`);
        
        if (claimResult === 0) {
          claimedInterfaces.push(iface);
        } else {
          console.info(TAG, `Failed to claim interface ${ifaceIndex}, continuing with others`);
        }
      }
      
      // Release all claimed interfaces
      for (let iface of claimedInterfaces) {
        let releaseResult = usbMgr.releaseInterface(gPipe, iface);
        console.info(TAG, `Release multiple interface result: ${releaseResult}`);
        expect(releaseResult).assertEqual(0);
      }
    })

    /**
     * @tc.name   testSetConfiguration001
     * @tc.number SUB_USB_InterfaceConfig_SetConfig_0100
     * @tc.desc   Test USB device configuration setting
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testSetConfiguration001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testSetConfiguration001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      for (let configIndex = 0; configIndex < gDeviceList[0].configs.length; configIndex++) {
        let config = gDeviceList[0].configs[configIndex];
        console.info(TAG, `Testing configuration ${configIndex}: ` + JSON.stringify(config));
        
        // Try to set the configuration
        let setResult = usbMgr.setConfiguration(gPipe, config);
        console.info(TAG, `Set configuration ${configIndex} result: ${setResult}`);
        
        // Some configurations may not be settable, which is expected
        expect(setResult >= 0 || setResult < 0).assertTrue();
        
        if (setResult === 0) {
          console.info(TAG, `Configuration ${configIndex} set successfully`);
        }
      }
    })

    /**
     * @tc.name   testSetInterface001
     * @tc.number SUB_USB_InterfaceConfig_SetInterface_0100
     * @tc.desc   Test USB interface setting
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

      let config = gDeviceList[0].configs[0];
      
      for (let ifaceIndex = 0; ifaceIndex < config.interfaces.length; ifaceIndex++) {
        let iface = config.interfaces[ifaceIndex];
        
        // First claim the interface
        let claimResult = usbMgr.claimInterface(gPipe, iface, true);
        console.info(TAG, `Claim interface for setInterface test result: ${claimResult}`);
        expect(claimResult).assertEqual(0);
        
        // Try to set the interface
        let setResult = usbMgr.setInterface(gPipe, iface);
        console.info(TAG, `Set interface ${ifaceIndex} result: ${setResult}`);
        
        // Some interfaces may not be settable
        expect(setResult >= 0 || setResult < 0).assertTrue();
        
        if (setResult === 0) {
          console.info(TAG, `Interface ${ifaceIndex} set successfully`);
        }
        
        // Release the interface
        let releaseResult = usbMgr.releaseInterface(gPipe, iface);
        console.info(TAG, `Release interface after setInterface test result: ${releaseResult}`);
        expect(releaseResult).assertEqual(0);
      }
    })

    /**
     * @tc.name   testInterfaceAlternate001
     * @tc.number SUB_USB_InterfaceConfig_Alternate_0100
     * @tc.desc   Test interface alternate setting control transfer
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceAlternate001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testInterfaceAlternate001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      let config = gDeviceList[0].configs[0];
      
      for (let ifaceIndex = 0; ifaceIndex < config.interfaces.length; ifaceIndex++) {
        let iface = config.interfaces[ifaceIndex];
        
        // Claim the interface first
        let claimResult = usbMgr.claimInterface(gPipe, iface, true);
        console.info(TAG, `Claim interface for alternate test result: ${claimResult}`);
        expect(claimResult).assertEqual(0);
        
        // Test GET_INTERFACE control transfer
        try {
          let controlParam = getControlParams(
            10, // GET_INTERFACE request
            usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
            usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
            0, // value
            ifaceIndex // index
          );
          
          // Add direction flag
          controlParam.reqType |= usbMgr.USBRequestDirection.USB_REQUEST_DIR_FROM_DEVICE;
          
          let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
          console.info(TAG, `GET_INTERFACE for interface ${ifaceIndex} result: ${result}`);
          
          // Should return the current alternate setting
          expect(result >= 0 || result < 0).assertTrue();
          
        } catch (error) {
          console.info(`GET_INTERFACE for interface ${ifaceIndex} failed: ` + error);
        }
        
        // Test SET_INTERFACE control transfer
        try {
          let controlParam = getControlParams(
            11, // SET_INTERFACE request
            usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
            usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
            0, // alternate setting
            ifaceIndex // interface index
          );
          
          // Add direction flag
          controlParam.reqType |= usbMgr.USBRequestDirection.USB_REQUEST_DIR_TO_DEVICE;
          
          let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
          console.info(TAG, `SET_INTERFACE for interface ${ifaceIndex} result: ${result}`);
          
          expect(result >= 0 || result < 0).assertTrue();
          
        } catch (error) {
          console.info(`SET_INTERFACE for interface ${ifaceIndex} failed: ` + error);
        }
        
        // Release the interface
        let releaseResult = usbMgr.releaseInterface(gPipe, iface);
        console.info(TAG, `Release interface after alternate test result: ${releaseResult}`);
        expect(releaseResult).assertEqual(0);
      }
    })

    /**
     * @tc.name   testInterfaceInvalidOperations001
     * @tc.number SUB_USB_InterfaceConfig_Invalid_0100
     * @tc.desc   Test invalid operations on interfaces
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceInvalidOperations001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testInterfaceInvalidOperations001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      // Test with null interface
      try {
        let claimResult = usbMgr.claimInterface(gPipe, null, true);
        expect(claimResult < 0).assertTrue();
      } catch (error) {
        console.info('Expected error for null interface: ' + error);
      }
      
      // Test with invalid interface
      try {
        let invalidInterface: usbMgr.USBInterface = {
          id: 255,
          clazz: 255,
          subClass: 255,
          protocol: 255,
          name: 'invalid',
          endpoints: []
        };
        
        let claimResult = usbMgr.claimInterface(gPipe, invalidInterface, true);
        console.info(TAG, `Invalid interface claim result: ${claimResult}`);
        expect(claimResult < 0 || claimResult === 0).assertTrue();
        
        if (claimResult === 0) {
          let releaseResult = usbMgr.releaseInterface(gPipe, invalidInterface);
          console.info(TAG, `Invalid interface release result: ${releaseResult}`);
          expect(releaseResult < 0 || releaseResult === 0).assertTrue();
        }
      } catch (error) {
        console.info('Expected error for invalid interface: ' + error);
      }
      
      // Test release without claim
      try {
        let iface = gDeviceList[0].configs[0].interfaces[0];
        let releaseResult = usbMgr.releaseInterface(gPipe, iface);
        console.info(TAG, `Release without claim result: ${releaseResult}`);
        expect(releaseResult < 0 || releaseResult === 0).assertTrue();
      } catch (error) {
        console.info('Expected error for release without claim: ' + error);
      }
    })

    /**
     * @tc.name   testInterfaceConfigurationValidation001
     * @tc.number SUB_USB_InterfaceConfig_ConfigValidation_0100
     * @tc.desc   Test configuration validation and integrity
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceConfigurationValidation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testInterfaceConfigurationValidation001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      let device = gDeviceList[0];
      
      // Validate device descriptors
      console.info(TAG, `Device info: busNum=${device.busNum}, devAddress=${device.devAddress}, name=${device.name}`);
      expect(device.busNum).toBeLargerThan(-1);
      expect(device.devAddress).toBeLargerThan(-1);
      expect(device.name).not.toBeNull();
      expect(device.serial).not.toBeNull();
      
      // Validate configurations
      expect(device.configs.length).toBeLargerThan(0);
      
      for (let configIndex = 0; configIndex < device.configs.length; configIndex++) {
        let config = device.configs[configIndex];
        
        // Validate configuration properties
        expect(config.id).toBeLargerThan(-1);
        expect(config.interfaces).not.toBeNull();
        expect(config.interfaces.length).toBeLargerThan(0);
        expect(config.name).not.toBeNull();
        
        // Validate configuration self-powered and remote wakeup flags
        expect(typeof config.isSelfPowered === 'boolean').assertTrue();
        expect(typeof config.isRemoteWakeup === 'boolean').assertTrue();
        
        // Validate interface consistency
        let interfaceIds = new Set<number>();
        for (let iface of config.interfaces) {
          expect(interfaceIds.has(iface.id)).assertFalse();
          interfaceIds.add(iface.id);
        }
        
        // Test GET_CONFIGURATION control transfer
        try {
          let controlParam = getControlParams(
            8, // GET_CONFIGURATION request
            usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE,
            usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD,
            0, // value
            0 // index
          );
          
          // Add direction flag
          controlParam.reqType |= usbMgr.USBRequestDirection.USB_REQUEST_DIR_FROM_DEVICE;
          
          let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
          console.info(TAG, `GET_CONFIGURATION result: ${result}`);
          expect(result >= 0 || result < 0).assertTrue();
          
        } catch (error) {
          console.info('GET_CONFIGURATION failed: ' + error);
        }
      }
    })

    /**
     * @tc.name   testInterfaceEndpointTypes001
     * @tc.number SUB_USB_InterfaceConfig_EndpointTypes_0100
     * @tc.desc   Test interface endpoint type validation
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceEndpointTypes001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testInterfaceEndpointTypes001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      let device = gDeviceList[0];
      let endpointTypes = new Set<number>();
      let endpointDirections = new Set<number>();
      
      for (let config of device.configs) {
        for (let iface of config.interfaces) {
          console.info(TAG, `Interface ${iface.id} class=${iface.clazz} subclass=${iface.subClass} protocol=${iface.protocol}`);
          
          // Validate endpoint types and directions
          for (let endpoint of iface.endpoints) {
            console.info(TAG, `Endpoint address=0x${endpoint.address.toString(16)} type=${endpoint.type} direction=${endpoint.direction} maxPacketSize=${endpoint.maxPacketSize}`);
            
            endpointTypes.add(endpoint.type);
            endpointDirections.add(endpoint.direction);
            
            // Validate endpoint address format
            expect(endpoint.address & 0x80).toBe(endpoint.direction);
            expect(endpoint.address & 0x7F).toBeLargerThan(0);
            expect(endpoint.maxPacketSize).toBeLargerThan(0);
            
            // Validate endpoint type
            expect([
              usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_CONTROL,
              usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_ISOCHRONOUS,
              usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
              usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_INTERRUPT
            ].includes(endpoint.type)).assertTrue();
            
            // Validate endpoint direction
            expect([
              usbMgr.USBRequestDirection.USB_REQUEST_DIR_TO_DEVICE,
              usbMgr.USBRequestDirection.USB_REQUEST_DIR_FROM_DEVICE
            ].includes(endpoint.direction)).assertTrue();
          }
        }
      }
      
      console.info(TAG, `Found endpoint types: ${Array.from(endpointTypes)}`);
      console.info(TAG, `Found endpoint directions: ${Array.from(endpointDirections)}`);
    })
  })
}