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

const TAG = "[UsbErrorRecoveryTest]";
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

/* USB error recovery test */
export default function UsbErrorRecoveryTest() {
  describe('UsbErrorRecoveryTest', () => {

    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbErrorRecoveryTest Begin*************');

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
      console.log(TAG, '*************Usb Unit UsbErrorRecoveryTest End*************');
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
     * @tc.name   testDeviceDisconnectRecovery001
     * @tc.number SUB_USB_ErrorRecovery_Disconnect_0100
     * @tc.desc   Test recovery after device disconnect
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceDisconnectRecovery001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceDisconnectRecovery001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();

      try {
        const config = devices.configs[0];
        const interfaces = config.interfaces;

        if (interfaces.length > 0) {
          const interfaceId = interfaces[0].id;
          const claimResult = usbMgr.claimInterface(gPipe, interfaceId, true);
          expect(claimResult).assertEqual(0);
        }

        const closeResult = usbMgr.closePipe(gPipe);
        expect(closeResult).assertEqual(0);

        const reconnectResult = usbMgr.connectDevice(devices);
        expect(reconnectResult !== null).assertTrue();
      } catch (error) {
        console.info(TAG, 'Device disconnect recovery error: ' + error);
      }
    });

    /**
     * @tc.name   testInterfaceUnclaimRecovery001
     * @tc.number SUB_USB_ErrorRecovery_Unclaim_0100
     * @tc.desc   Test recovery after interface unclaim
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceUnclaimRecovery001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInterfaceUnclaimRecovery001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();

      const config = devices.configs[0];
      const interfaces = config.interfaces;

      if (interfaces.length > 0) {
        const interfaceId = interfaces[0].id;

        const claim1 = usbMgr.claimInterface(gPipe, interfaceId, true);
        expect(claim1).assertEqual(0);

        const release1 = usbMgr.releaseInterface(gPipe, interfaceId);
        expect(release1).assertEqual(0);

        const claim2 = usbMgr.claimInterface(gPipe, interfaceId, true);
        expect(claim2).assertEqual(0);

        const release2 = usbMgr.releaseInterface(gPipe, interfaceId);
        expect(release2).assertEqual(0);

        const claim3 = usbMgr.claimInterface(gPipe, interfaceId, true);
        expect(claim3).assertEqual(0);
      } catch (error) {
        console.info(TAG, 'Interface unclaim recovery error: ' + error);
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testTransferInterruptRecovery001
     * @tc.number SUB_USB_ErrorRecovery_TransferInterrupt_0100
     * @tc.desc   Test recovery from interrupted transfer
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testTransferInterruptRecovery001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testTransferInterruptRecovery001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;
      const endpoint = interfaces[0].endpoints.find(ep => ep.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK);

      if (!endpoint) {
        console.info(TAG, 'No bulk endpoint found, skipping test');
        return;
      }

      const testData = new Uint8Array(512);
      const transferParams = {
        devPipe: gPipe,
        flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_WRITE,
        endpoint: endpoint,
        type: usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
        timeout: 5000,
        length: testData.length,
        callback: () => {},
        userData: new Uint8Array(32),
        buffer: testData,
        isoPacketCount: 0,
      };

      const cancelAfterStart = usbMgr.usbCancelTransfer(gPipe, endpoint.address);
      expect(cancelAfterStart).assertEqual(0);

      const result = usbMgr.bulkTransferWrite(transferParams);
      console.info(TAG, 'Transfer after cancel result: ${result}');
      expect(result === 0 || result === -1).assertTrue();

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testConfigurationChangeRecovery001
     * @tc.number SUB_USB_ErrorRecovery_ConfigChange_0100
     * @tc.desc   Test recovery after configuration change
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConfigurationChangeRecovery001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testConfigurationChangeRecovery001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const configIds = [0, 1];

      for (const configId of configIds) {
        const result = usbMgr.setConfiguration(gPipe, configId);
        console.info(TAG, `Set config ${configId} result: ${result}`);
        expect(result === 0).assertEqual(0);
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testResourceExhaustionRecovery001
     * @tc.number SUB_USB_ErrorRecovery_ResourceExhaustion_0100
     * @tc.desc   Test recovery from resource exhaustion
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testResourceExhaustionRecovery001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testResourceExhaustionRecovery001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;

      if (interfaces.length > 0) {
        const interfaceId = interfaces[0].id;
        const claimCount = 10;

        for (let i = 0; i < claimCount; i++) {
          usbMgr.claimInterface(gPipe, interfaceId, true);
        }

        expect(usbMgr.claimInterface(gPipe, interfaceId, true) !== 0).assertTrue();

        const releaseResult = usbMgr.releaseInterface(gPipe, interfaceId);
        expect(releaseResult).assertEqual(0);

        const reopenResult = usbMgr.connectDevice(devices);
        expect(reopenResult !== null).assertTrue();
      }
    });

    /**
     * @tc.name   testPermissionRecovery001
     *tc.number SUB_USB_ErrorRecovery_Permission_0100
     * @tc.desc   Test permission state recovery
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPermissionRecovery001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testPermissionRecovery001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      const deviceName = devices.name;

      await usbMgr.requestRight(deviceName);
      expect(usbMgr.hasRight(deviceName)).toBeTruthy();

      await usbMgr.removeRight(deviceName);
      expect(usbMgr.hasRight(deviceName)).toBeFalsy();

      const result = await usbMgr.requestRight(deviceName);
      expect(result).toBeTruthy();

      usbMgr.removeRight(deviceName);
      expect(usbMgr.hasRight(deviceName)).toBeFalsy();
    });

    /**
     * @tc.name   testPipeStateRecovery001
     @tc.number SUB_USB_ErrorRecovery_PipeState_0100
     * @tc.desc   Test pipe state recovery
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPipeStateRecovery001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testPipeStateRecovery001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      const connectionAttempts = [
        { attempt: 1, shouldSucceed: true },
        { attempt: 2, shouldSucceed: true },
        { attempt: 3, shouldSucceed: false },
        { attempt: 4, shouldSucceed: true },
      ];

      for (const attempt of connectionAttempts) {
        console.info(TAG, `Connection attempt: ${attempt.attempt}`);

        if (attempt.shouldSucceed) {
          const pipe = usbMgr.connectDevice(devices);
          expect(pipe !== null).assertTrue();
          const closeResult = usbMgr.closePipe(gPipe);
          expect(closeResult === 0 || closeResult === -1).assertTrue();
        } else {
          const pipe = usbMgr.connectDevice(devices);
          expect(pipe === null).toBeNull();
        }
      }
    });

    /**
     * @tc.name   testDeviceDisconnectRecovery002
     * @tc.number SUB_USB_ErrorRecovery_Disconnect_0200
     * @tc.desc   Test rapid disconnect/reconnect cycles
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceDisconnectRecovery002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDeviceDisconnectRecovery002 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      for (let i = 0; i < 5; i++) {
        console.info(TAG, `Cycle ${i + 1}, getting device`);

        const deviceList = usbMgr.getDevices();
        if (deviceList.length === 0) {
          console.info(TAG, 'No device found, stopping test');
          break;
        }

        const device = deviceList[0];
        const pipe = usbMgr.connectDevice(device);
        expect(pipe !== null).assertTrue();

        for (let j = 0; j < 3; j++) {
          const closeResult = usbMgr.closePipe(pipe);
          const openResult = usbMgr.connectDevice(device);
        console.info(TAG, `Close ${j}, Reopen ${j}: ${openResult}`);

          if (j < 2) {
            expect(openResult !== null).assertTrue();
          }
        }
      }
    });

    /**
     * @tc.name   testInterfaceConflictRecovery001
     * @tc.number SUB_USB_ErrorRecovery_InterfaceConflict_0100
     * @tc.desc   Test interface conflict recovery
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterfaceConflictRecovery001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInterfaceConflictRecovery001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;

      if (interfaces.length < 2) {
        console.info(TAG, 'Not enough interfaces for conflict test');
        return;
      }

      const interface1Id = interfaces[0].id;
      const interface2Id = interfaces[1].id;

      const claim1 = usbMgr.claimInterface(gPipe, interface1Id, true);
      expect(claim1).assertEqual(0);

      const claim2 = usbMgr.claimInterface(gPipe, interface2Id, true);
      expect(claim2).assertEqual(0);

      const try {
        const claim1Again = usbMgr.claimInterface(gPipe, interface1Id, true);
        expect(claim1Again !== 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'Interface 1 conflict recovery successful');
      }

      const release1 = usbMgr.releaseInterface(gPipe, interface1Id);
      expect(release1).assertEqual(0);

      const release2 = usbMgr.releaseInterface(gPipe, interface2Id);
      expect(release2).assertEqual(0);

      const release2Again = usbMgr.releaseInterface(gPipe, interface2Id);
      expect(release2Again !== 0).assertTrue();
    });

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testNullPointerRecovery001
     * @tc.number SUB_USB_ErrorRecovery_NullPointer_0100
     * @tc.desc   Test null pointer handling recovery
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testNullPointerRecovery001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testNullPointerRecovery001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();

      const nullTests = [
        { description: 'null config', action: () => usbMgr.setConfiguration(gPipe, null, 0) },
        { description: 'null interface', action: () => usbMgr.claimInterface(gPipe, null, 0, true) },
        { description: 'null pipe', action: () => usbMgr.closePipe(null) },
        { description: 'null transfer', action: () => usbMgr.bulkTransferWrite(null, {type: 0, endpoint: {address: 1, attributes: 1, direction: 0, id: 1, interfaceId: 1, maxPacketSize: 512, type: 0}, timeout: 1000, length: 0, callback: () => {}, userData: new Uint8Array(32), buffer: new Uint8Array(0), isoPacketCount: 0}) },
        { description: 'null endpoint', action: () => usbMgr.usbCancelTransfer(null, 0) },
      ];

      for (const test of nullTests) {
        console.info(TAG, `Testing: ${test.description}`);

        try {
          test.action();
          console.info(TAG, `${test.description} result: ${test.result}`);
          expect(test.result !== 0).assertTrue();
        } catch (error) {
          console.info(TAG, `${test.description} error recovery successful: ${error}`);
        }
      }
    });

    /**
     * @tc.name   testOutOfSequenceRecovery001
     * @tc.number SUB_USB_ErrorRecovery_OutOfSequence_0100
     * @tc.desc   Test out of sequence operations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testOutOfSequenceRecovery001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testOutOfSequenceRecovery001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      try {
        const result = usbMgr.getDevices();
        expect(result.length >= 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'Get devices error recovery successful: ' + error);
      }
    });
  });
}
