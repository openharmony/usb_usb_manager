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

const TAG = "[UsbDeviceStateManagementTest]";
let gDeviceList: Array<usbMgr.USBDevice>;
let gPipe: usbMgr.USBDevicePipe | null;
let devices: usbMgr.USBDevice;
let isDeviceConnected: boolean;
let savedStates: Map<string, any> = new Map();

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

function saveState(key: string, value: any): void {
  savedStates.set(key, value);
  console.info(TAG, `Saved state ${key}: ${JSON.stringify(value)}`);
}

function loadState(key: string): any {
  return savedStates.get(key);
}

/* USB device state management test */
export default function UsbDeviceStateManagementTest() {
  describe('UsbDeviceStateManagementTest', () => {

    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbDeviceStateManagementTest Begin*************');

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
      savedStates.clear();
      console.log(TAG, '*************Usb Unit UsbDeviceStateManagementTest End*************');
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
     * @tc.name   testPipeConnectionState001
     * @tc.number SUB_USB_DeviceState_PipeState_0100
     * @tc.desc   Test pipe connection state management
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPipeConnectionState001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testPipeConnectionState001 begin');

      const stateSequence = ['disconnected', 'connected', 'closed'];

      for (const state of stateSequence) {
        console.info(TAG, `Testing state: ${state}`);

        if (state === 'disconnected') {
          const result = usbMgr.connectDevice(devices);
          expect(result !== null).assertTrue();
          saveState('pipeState', 'connected');
        } else if (state === 'connected') {
          const currentState = loadState('pipeState');
          expect(currentState).toBe('connected');

          const result = usbMgr.connectDevice(devices);
          if (result !== null) {
            console.info(TAG, 'Already connected, pipe is not null');
          }
        } else if (state === 'closed') {
          const closeResult = usbMgr.closePipe(gPipe);
          console.info(TAG, `Close pipe result: ${closeResult}`);
          expect(closeResult === 0 || closeResult === -1).assertTrue();

          const reopenResult = usbMgr.connectDevice(devices);
          console.info(TAG, `Reopen pipe result: ${reopenResult}`);
          expect(reopenResult !== null).assertTrue();

          saveState('pipeState', 'connected');
        }
      }
    });

    /**
     * @tc.name   testConfigurationState001
     * @tc.number SUB_USB_DeviceState_ConfigState_0100
     * @tc.desc   Test configuration state management
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConfigurationState001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testConfigurationState001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const initialConfigId = config.id;

      try {
        const configIds = [0, 1];
        for (let i = 0; i < configIds.length; i++) {
          const result = usbMgr.setConfiguration(gPipe, configIds[i]);
          console.info(TAG, `Set config ${configIds[i]} result: ${result}`);

          if (i === 0) {
            expect(result).assertEqual(0);
          } else {
            expect(result === 0 || result === -1).assertTrue();
          }

          if (result === 0) {
            const currentConfig = devices.configs.find(c => c.id === configIds[i]);
            expect(currentConfig).toBeTruthy();
          }
        }
      } catch (error) {
        console.info(TAG, 'Configuration state test error: ' + error);
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testInterfaceClaimState001
     * @tc.number SUB_USB_DeviceState_InterfaceState_0100
     * @tc.desc   Test interface claim state management
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceClaimState001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInterfaceClaimState001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interface = config.interfaces[0];
      const interfaceId = interface.id;

      const stateSequence = [
        { state: 'unclaimed', action: 'claim', expectedSuccess: true },
        { state: 'claimed', action: 'reclaim', expectedSuccess: false },
        { state: 'claimed', action: 'claim', expectedSuccess: false },
        { state: 'claimed', action: 'release', expectedSuccess: true },
        { state: 'claimed', action: 'release', expectedSuccess: false },
      ];

      for (const stateOp of stateSequence) {
        console.info(TAG, `State: ${stateOp.state}, action: ${stateOp.action}`);

        let result: number;
        if (stateOp.action === 'claim') {
          result = usbMgr.claimInterface(gPipe, interfaceId, true);
        } else if (stateOp.action === 'reclaim') {
          result = usbMgr.claimInterface(gPipe, interfaceId, true);
        } else if (stateOp.action === 'release') {
          result = usbMgr.releaseInterface(gPipe, interfaceId);
        }

        console.info(TAG, `Operation result: ${result}`);

        if (stateOp.expectedSuccess) {
          expect(result === 0).assertTrue();
        } else {
          expect(result !== 0).assertTrue();
        }
      }
    });

    /**
     * @tc.name   testDeviceEnumerationState001
     * @tc.number SUB_USB_DeviceState_EnumerationState_0100
     * @tc.desc   Test device enumeration state persistence
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceEnumerationState001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceEnumerationState001 begin');

      let initialDeviceCount = gDeviceList.length;
      console.info(TAG, `Initial device count: ${initialDeviceCount}`);

      const enumerationResults = [];

      for (let i = 0; i < 3; i++) {
        const devices = usbMgr.getDevices();
        enumerationResults.push(devices);
        console.info(TAG, `Enumeration ${i} returned ${devices.length} devices`);

        expect(devices.length).toBe(initialDeviceCount);
      }

      expect(enumerationResults.length).toBe(3);
    });

    /**
     * @tc.name   testPermissionStatePersistence001
     * @tc.number SUB_USB_DeviceState_PermissionState_0100
     * @tc.desc   Test permission state persistence
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPermissionStatePersistence001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testPermissionStatePersistence001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      const deviceName = devices.name;

      const permissionOperations = [
        { op: 'request', expected: true, action: () => usbMgr.requestRight(deviceName) },
        { op: 'check', expected: true, action: () => usbMgr.hasRight(deviceName) },
        { op: 'check', expected: true, action: () => usbMgr.hasRight(deviceName) },
        { op: 'remove', expected: true, action: () => usbMgr.removeRight(deviceName) },
        { op: 'check', expected: false, action: () => usbMgr.hasRight(deviceName) },
        { op: 'request', expected: true, action: () => usbMgr.requestRight(deviceName) },
      ];

      for (const permOp of permissionOperations) {
        console.info(TAG, `Permission op: ${permOp.op}`);

        if (permOp.op === 'request') {
          const result = await permOp.action();
          console.info(TAG, `Permission request result: ${result}`);
          expect(result).toBe(permOp.expected);
        } else if (permOp.op === 'check') {
          const result = permOp.action();
          console.info(TAG, `Permission check result: ${result}`);
          expect(result).toBe(permOp.expected);
        } else if (permOp.op === 'remove') {
          const result = permOp.action();
          console.info(TAG, `Permission remove result: ${result}`);
          expect(result).toBe(permOp.expected);
        } else if (permOp.op === 'check') {
          const result = permOp.action();
          console.info(TAG, `Permission check result: ${result}`);
          expect(result).toBe(permOp.expected);
        } else if (permOp.op === 'request') {
          const result = await permOp.action();
          console.info(TAG, `Second permission request result: ${result}`);
          expect(result).toBe(permOp.expected);
        }
      }
    });

    /**
     * @tc.name   testStateAfterDisconnect001
     * @tc.number SUB_USB_DeviceState_AfterDisconnect_0100
     * @tc.desc   Test state management after disconnect
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testStateAfterDisconnect001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testStateAfterDisconnect001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();

      const deviceList = usbMgr.getDevices();
      const deviceName = devices.name;

      await usbMgr.requestRight(deviceName);
      const hasRightBefore = usbMgr.hasRight(deviceName);
      console.info(TAG, `HasRight before close: ${hasRightBefore}`);

      const closeResult = usbMgr.closePipe(gPipe);
      expect(closeResult).assertEqual(0);
      console.info(TAG, 'Close pipe result: ' + closeResult);

      const hasRightAfter = usbMgr.hasRight(deviceName);
      console.info(TAG, `HasRight after close: ${hasRightAfter}`);
      expect(hasRightAfter).toBeFalsy();

      const openResult = usbMgr.connectDevice(devices);
      console.info(TAG, `Open pipe result: ${JSON.stringify(openResult)}`);
      expect(openResult !== null).assertTrue();
    });

    /**
     * @tc.name   testConfigurationChangeNotification001
     * @tc.number SUB_USB_DeviceState_ConfigChange_0100
     * @tc.desc   Test configuration change notification
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConfigurationChangeNotification001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testConfigurationChangeNotification001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const configIds = [0, 1];

      try {
        for (const configId of configIds) {
          const result = usbMgr.setConfiguration(gPipe, configId);
          console.info(TAG, `Set config ${configId} result: ${result}`);
          expect(result).assertEqual(0);
        }
      } catch (error) {
        console.info(TAG, 'Configuration change test error: ' + error);
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testMultipleOperationSequence001
     * @tc.number SUB_USB_DeviceState_OperationSequence_0100
     * @tc.desc   Test device operation sequence
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testMultipleOperationSequence001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMultipleOperationSequence001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      const operationSequence = [
        { name: 'connect', action: () => usbMgr.connectDevice(devices) },
        { name: 'check right', action: () => usbMgr.hasRight(devices.name) },
        { name: 'claim interface', action: () => {
          const config = devices.configs[0];
          const interface = config.interfaces[0];
          return usbMgr.claimInterface(gPipe, interface.id, true);
        } },
        { name: 'set config', action: () => {
          const config = devices.configs[0];
          return usbMgr.setConfiguration(gPipe, config.id);
        } },
        { name: 'close', action: () => usbMgr.closePipe(gPipe) },
      ];

      let currentState = 'initial';

      for (const op of operationSequence) {
        console.info(TAG, `Operation: ${op.name}, Current state: ${currentState}`);

        const result = op.action();
        console.info(TAG, `${op.name} result: ${JSON.stringify(result)}`);

        if (op.name === 'connect') {
          expect(result !== null).assertTrue();
          currentState = 'connected';
        } else if (op.name === 'check right') {
          expect(result).toBeTruthy();
        } else if (op.name === 'claim interface') {
          expect(result === 0).assertTrue();
          currentState = 'claimed';
        } else if (op.name === 'set config') {
          expect(result).assertEqual(0);
        } else if (op.name === 'close') {
          expect(result === 0 || result === -1).assertTrue();
          currentState = 'disconnected';
        }
      }

      expect(currentState).toBe('disconnected');
    });

    /**
     * @tc.name   testStateConsistency001
     * @tc.number SUB_USB_DeviceState_Consistency_0100
     * @tc.desc   Test state consistency across operations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testStateConsistency001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testStateConsistency001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      const deviceList1 = usbMgr.getDevices();
      const deviceName = devices.name;

      await usbMgr.requestRight(deviceName);
      expect(usbMgr.hasRight(deviceName)).toBeTruthy();

      getPipe();

      const deviceList2 = usbMgr.getDevices();
      expect(deviceList2.length).toBe(deviceList1.length);

      const hasRight1 = usbMgr.hasRight(deviceName);
      expect(hasRight1).toBeTruthy();

      usbMgr.closePipe(gPipe);

      const hasRight2 = usbMgr.hasRight(deviceName);
      expect(hasRight2).toBeFalsy();

      const deviceList3 = usbMgr.getDevices();
      expect(deviceList3.length).toBe(deviceList1.length);
    });

    /**
     * @tc.name   testPipeReuseState001
     * @tc.number SUB_USB_DeviceState_PipeReuse_0100
     * @tc.desc   Test pipe reuse behavior
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPipeReuseState001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testPipeReuseState001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      const openAttempts = [
        { attempt: 1, shouldSucceed: true },
        { attempt: 2, shouldSucceed: true },
        { attempt: 3, shouldSucceed: false },
        { attempt: 4, shouldSucceed: true },
      ];

      for (const openAttempt of openAttempts) {
        console.info(TAG, `Open attempt: ${openAttempt.attempt}, should succeed: ${openAttempt.shouldSucceed}`);

        if (openAttempt.shouldSucceed) {
          const pipe = usbMgr.connectDevice(devices);
          expect(pipe !== null).assertTrue();

          if (openAttempt.attempt === 3) {
            const closeResult = usbMgr.closePipe(pipe);
            expect(closeResult === 0 || closeResult === -1).assertTrue();
          }
        } else {
          const pipe = usbMgr.connectDevice(devices);
          expect(pipe).toBeNull();
        }
      }
    });

    /**
     * @tc.name   testDeviceNamePersistence001
     * @tc.number SUB_USB_DeviceState_DeviceName_0100
     * @tc.desc   Test device name persistence
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceNamePersistence001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceNamePersistence001 begin');

      const deviceList1 = usbMgr.getDevices();
      const deviceName1 = deviceList1[0]?.name || '';

      if (!deviceName1) {
        expect(deviceName1).toBe('');
        return;
      }

      const deviceList2 = usbMgr.getDevices();
      const deviceName2 = deviceList2[0]?.name || '';

      expect(deviceName1).toBe(deviceName2);
    });
  });
}
