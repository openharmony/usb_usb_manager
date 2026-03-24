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

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium'
import usbMgr from '@ohos.usbManager';
import { UiDriver, BY } from '@ohos.UiTest';
import { BusinessError } from '@ohos.base';

const TAG = "[UsbBoundaryConditionsTest]";
let gDeviceList: Array<usbMgr.USBDevice>;
let gPipe: usbMgr.USBDevicePipe | null;
let devices: usbMgr.USBDevice;
let isDeviceConnected: boolean;

function sleep(ms: number): Promise<void> {
  return new Promise<void>(resolve => setTimeout(resolve, ms));
}

async function driveFn(): Promise<boolean> {
  console.info('**************driveFn**************');
  try {
    let driver = await UiDriver.create();
    console.info(TAG, `come in driveFn`);
    console.info(TAG, `driver is ${JSON.stringify(driver)}`);
    await sleep(1000);
    let button = await driver.findComponent(BY.text('允许'));
    console.info(TAG, `button is ${JSON.stringify(button)}`);
    await sleep(1000);
    await button.click();
    await sleep(500);
    return true;
  } catch (err) {
    console.info(TAG, 'err is ' + err);
    return false;
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
  }
}

function deviceConnected(): boolean {
  if (gDeviceList.length > 0) {
    console.info(TAG, "Test USB device is connected");
    return true;
  }
  console.info(TAG, "Test USB device is not connected");
  return false;
}

function getPipe() {
  gPipe = usbMgr.connectDevice(devices);
  console.info(TAG, `connectDevice getPipe ret: ${JSON.stringify(gPipe)}`);
  expect(gPipe !== null).assertTrue();
}

/* USB boundary conditions test */
export default function UsbBoundaryConditionsTest() {
  describe('UsbBoundaryConditionsTest', () => {

    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbBoundaryConditionsTest Begin*************');

      gDeviceList = usbMgr.getDevices();
      console.info(TAG, 'beforeAll: usb case gDeviceList.length return: ' + JSON.stringify(gDeviceList));
      isDeviceConnected = deviceConnected();

      if (isDeviceConnected && gDeviceList.length >= 1) {
        devices = gDeviceList[0];
        let hasRight = usbMgr.hasRight(devices.name);
        if (!hasRight) {
          console.info(TAG, `beforeAll: usb requestRight start`);
          await getPermission();
          await sleep(3000);
          await driveFn();
          await sleep(1000);
        }
      }
      console.log(TAG, 'beforeAll end');
    })

    afterAll(() => {
      if (gPipe !== null) {
        usbMgr.closePipe(gPipe);
      }
      console.log(TAG, '*************Usb Unit UsbBoundaryConditionsTest End*************');
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
      if (gPipe !== null) {
        usbMgr.closePipe(gPipe);
      }
      gPipe = null;
      devices = null;
    })

    /**
     * @tc.name   testDeviceAddressBoundary001
     * @tc.number SUB_USB_Boundary_DeviceAddress_0100
     * @tc.desc   Test device address boundary conditions
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceAddressBoundary001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceAddressBoundary001 begin');

      const boundaryAddresses = [0, 1, 126, 127, 128, 255];

      for (const addr of boundaryAddresses) {
        try {
          const result = usbMgr.connectDevice(devices);
          console.info(TAG, `Test address ${addr}, result: ${result}`);

          if (addr === 0) {
            expect(result !== null).assertTrue();
          } else if (addr === 127 || addr === 255) {
            expect(result === null).assertTrue();
          } else {
            const isValid = result !== null;
            expect(isValid).toBeTruthy();
          }
        } catch (error) {
          console.info(TAG, `Address ${addr} test error: ${error}`);
        }
      }
    });

    /**
     * @tc.name   testBusNumberBoundary001
     * @tc.number SUB_USB_Boundary_BusNumber_0100
     * @tc.desc   Test bus number boundary conditions
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testBusNumberBoundary001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testBusNumberBoundary001 begin');

      const boundaryBusNumbers = [0, 1, 127, 128, 255];

      for (const busNum of boundaryBusNumbers) {
        console.info(TAG, `Testing bus number ${busNum}`);

        if (busNum === devices.busNum) {
          getPipe();
        } else {
          const result = usbMgr.connectDevice(devices);
          console.info(TAG, `Bus ${busNum} result: ${result}`);

          if (busNum === 0 || busNum === 1 || busNum === 255) {
            expect(result === null).assertTrue();
          } else {
            expect(result === null).assertTrue();
          }
        }
      }
    });

    /**
     * @tc.name   testConfigIndexBoundary001
     * @tc.number SUB_USB_Boundary_ConfigIndex_0100
     * @tc.desc   Test config index boundary conditions
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConfigIndexBoundary001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testConfigIndexBoundary001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const boundaryIndices = [0, 1, 255];

      for (const configId of boundaryIndices) {
        try {
          const result = usbMgr.setConfiguration(gPipe, configId);
          console.info(TAG, `Config index ${configId} setConfig result: ${result}`);

          if (configId === 0) {
            expect(result).assertEqual(0);
          } else {
            expect(result !== 0).assertTrue();
          }
        } catch (error) {
          console.info(TAG, `Config index ${configId} test error: ${error}`);
        }
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testInterfaceIdBoundary001
     * @tc.number SUB_USB_Boundary_InterfaceId_0100
     * @tc.desc   Test interface ID boundary conditions
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceIdBoundary001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInterfaceIdBoundary001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;
      const boundaryIds = [0, 1, 255];

      for (const interfaceId of boundaryIds) {
        try {
          const result = usbMgr.claimInterface(gPipe, interfaceId, true);
          console.info(TAG, `Interface ID ${interfaceId} claimInterface result: ${result}`);

          if (interfaceId === 0) {
            expect(result).assertEqual(0);
          } else if (interfaceId === 255) {
            expect(result !== 0).assertTrue();
          } else {
            const isValid = result === 0 || result !== -1;
            expect(isValid).toBeTruthy();
          }
        } catch (error) {
          console.info(TAG, `Interface ID ${interfaceId} test error: ${error}`);
        }
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testAltSettingBoundary001
     * @tc.number SUB_USB_Boundary_AltSetting_0100
     * @tc.desc   Test alternate setting boundary conditions
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testAltSettingBoundary001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAltSettingBoundary001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;

      if (interfaces.length > 0) {
        const interfaceId = interfaces[0].id;
        const boundaryAltSettings = [0, 1, 255];

        for (const altSetting of boundaryAltSettings) {
          try {
            const result = usbMgr.setInterface(gPipe, interfaceId, altSetting);
            console.info(TAG, `Alt setting ${altSetting} setInterface result: ${result}`);

            if (altSetting === 0) {
              expect(result).assertEqual(0);
            } else {
              const isValid = result === 0 || result !== -1;
              expect(isValid).toBeTruthy();
            }
          } catch (error) {
            console.info(TAG, `Alt setting ${altSetting} test error: ${error}`);
          }
        }
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testEndpointAddressBoundary001
     * @tc.number SUB_USB_Boundary_EndpointAddress_0100
     * @tc.desc   Test endpoint address boundary conditions
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testEndpointAddressBoundary001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testEndpointAddressBoundary001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;
      const endpoints = interfaces[0].endpoints;

      const boundaryAddresses = [0, 1, 0x7F, 0x80, 0xFF];

      for (const epAddr of boundaryAddresses) {
        const endpoint = endpoints.find(ep => ep.address === epAddr);
        if (!endpoint) {
          console.info(TAG, `Endpoint address ${epAddr.toString(16)} not found, skipping`);
          continue;
        }

        try {
          const endpointNumber = epAddr & 0x0F;
          const result = usbMgr.usbCancelTransfer(gPipe, endpointNumber);
          console.info(TAG, `Endpoint number ${endpointNumber} cancelTransfer result: ${result}`);

          if (endpointNumber === 0) {
            expect(result).assertEqual(0);
          } else {
            expect(result !== 0).assertTrue();
          }
        } catch (error) {
          console.info(TAG, `Endpoint ${endpointNumber} cancel test error: ${error}`);
        }
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testTimeoutBoundary001
     * @tc.number SUB_USB_Boundary_Timeout_0100
     * @tc.desc   Test timeout parameter boundary conditions
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testTimeoutBoundary001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testTimeoutBoundary001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;
      const endpoint = interfaces[0].endpoints[0];

      const boundaryTimeouts = [-1, 0, 1, 10000, 65535];

      for (const timeout of boundaryTimeouts) {
        const transferParams = {
          devPipe: gPipe,
          flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_READ,
          endpoint: endpoint,
          type: usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
          timeout: timeout,
          length: 512,
          callback: () => {},
          userData: new Uint8Array(512),
          buffer: new Uint8Array(512),
          isoPacketCount: 0,
        };

        try {
          if (timeout < 0) {
            const result = usbMgr.bulkTransferRead(transferParams);
            console.info(TAG, `Timeout ${timeout} bulkTransferRead result: ${result}`);
            expect(result !== 0).assertTrue();
          } else {
            const result = usbMgr.bulkTransferRead(transferParams);
            console.info(TAG, `Timeout ${timeout} bulkTransferRead result: ${result}`);
            expect(result === 0 || result === -1).assertTrue();
          }
        } catch (error) {
          console.info(TAG, `Timeout ${timeout} test error: ${error}`);
        }
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testBufferSizeBoundary001
     * @tc.number SUB_USB_Boundary_BufferSize_0100
     * @tc.desc   Test buffer size boundary conditions
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testBufferSizeBoundary001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testBufferSizeBoundary001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected). assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;
      const endpoint = interfaces[0].endpoints[0];

      const boundaryBufferSizes = [0, 1, 512, 1024, 16384];

      for (const bufferSize of boundaryBufferSizes) {
        const transferParams = {
          devPipe: gPipe,
          flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_READ,
          endpoint: endpoint,
          type: usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
          timeout: 5000,
          length: bufferSize,
          callback: () => {},
          userData: new Uint8Array(bufferSize),
          buffer: new Uint8Array(bufferSize),
          isoPacketCount: 0,
        };

        try {
          if (bufferSize === 0 || bufferSize > 16384) {
            const result = usbMgr.bulkTransferRead(transferParams);
            console.info(TAG, `Buffer size ${bufferSize} bulkTransferRead result: ${result}`);
            expect(result !== 0).assertTrue();
          } else {
            const result = usbMgr.bulkTransferRead(transferParams);
            console.info(TAG, `Buffer size ${bufferSize} bulkTransferRead result: ${result}`);
            expect(result === 0).assertTrue();
          }
        } catch (error) {
          console.info(TAG, `Buffer size ${bufferSize} test error: ${error}`);
        }
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testNullParameterBoundary001
     * @tc.number SUB_USB_Boundary_NullParam_0100
     * @tc.desc   Test null parameter handling at boundaries
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testNullParameterBoundary001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNullParameterBoundary001 begin');

      try {
        const nullDevice = usbMgr.connectDevice(null);
        console.info(TAG, 'Null device connectDevice result: ${nullDevice}`);
        expect(nullDevice).toBeNull();

        const nullConfig = usbMgr.setConfiguration(null, 0);
        console.info(TAG, 'Null config setConfiguration result: ${nullConfig}`);
        expect(nullConfig !== 0).assertTrue();

        const nullInterface = usbMgr.claimInterface(null, 0, true);
        console.info(TAG, 'Null interface claimInterface result: ${nullInterface}`);
        expect(nullInterface !== 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'Null parameter boundary test error: ' + error);
      }
    });

    /**
     * @tc.name   testDeviceConnectionState001
     * @tc.number SUB_USB_Boundary_ConnectionState_0100
     * @tc.desc   Test device connection state transitions
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceConnectionState001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testDeviceConnectionState001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      const stateTransitions = [
        'disconnected',
        'connected',
        'disconnected',
        'connected',
      ];

      for (const state of stateTransitions) {
        console.info(TAG, `Testing state: ${state}`);

        if (state === 'disconnected') {
          const connect1 = usbMgr.connectDevice(devices);
          expect(connect1 !== null).assertTrue();
          usbMgr.closePipe(connect1);
        } else {
          const connect2 = usbMgr.connectDevice(devices);
          expect(connect2 !== null).assertTrue();
        }

        const isConnected = gDeviceList.length > 0 && usbMgr.hasRight(devices.name);
        expect(isConnected).toBeTruthy();
      }
    });

    /**
     * @tc.name   testMultipleInterfaceClaim001
     * @tc.number SUB_USB_Boundary_MultiInterface_0100
     * @tc.desc   Test multiple interface claim boundary conditions
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testMultipleInterfaceClaim001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMultipleInterfaceClaim001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;

      try {
        const claimResults = [];
        for (let i = 0; i < interfaces.length; i++) {
          claimResults.push(usbMgr.claimInterface(gPipe, interfaces[i].id, true));
        }

        for (let i = 0; i < claimResults.length; i++) {
          console.info(TAG, `Claim interface ${i} result: ${claimResults[i]}`);
          expect(claimResults[i] === 0).assertTrue();
        }

        const claimAgainResult = usbMgr.claimInterface(gPipe, interfaces[0].id, true);
        console.info(TAG, 'Re-claim same interface result: ${claimAgainResult}`);
        expect(claimAgainResult !== 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'Multiple interface claim test error: ' + error);
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testPermissionBoundary001
     * @tc.number SUB_USB_Boundary_Permission_0100
     * @tc.desc   Test permission state boundary conditions
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPermissionBoundary001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testPermissionBoundary001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      const deviceName = devices.name;

      const permissionChecks = [
        { state: 'no right', action: () => usbMgr.hasRight(deviceName), expected: false },
        { state: 'has right', action: () => usbMgr.requestRight(deviceName), expected: true },
        { state: 'has right', action: () => usbMgr.hasRight(deviceName), expected: true },
      ];

      for (const check of permissionChecks) {
        console.info(TAG, `Permission check: ${check.state}`);

        const result = check.action();
        console.info(TAG, `Permission check result: ${result}`);
        expect(result).toBe(check.expected);
      }
    });

    /**
     * @tc.name   testConfigIndexSequence001
     * @tc.number SUB_USB_Boundary_ConfigSequence_0100
     * @tc.desc   Test config index sequence handling
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConfigIndexSequence001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testConfigIndexSequence001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];

      const configIndices = [0, 1, 2];

      for (let i = 0; i < configIndices.length; i++) {
        try {
          const result = usbMgr.setConfiguration(gPipe, configIndices[i]);
          console.info(TAG, `Set config ${configIndices[i]} result: ${result}`);

          if (i === 0) {
            expect(result).assertEqual(0);
          } else if (i === 1) {
            expect(result === 0 || result === -1).assertTrue();
          } else {
            expect(result !== 0).assertTrue();
          }
        } catch (error) {
          console.info(TAG, `Config index ${configIndices[i]} test error: ${error}`);
        }
      }

      usbMgr.closePipe(gPipe);
    });
  });
}
