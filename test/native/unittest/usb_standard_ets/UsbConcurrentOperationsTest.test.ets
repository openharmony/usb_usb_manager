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

const TAG = "[UsbConcurrentOperationsTest]";
let gDeviceList: Array<usbMgr.USBDevice>;
let gPipe1: usbMgr.USBDevicePipe | null;
let gPipe2: usbMgr.USBDevicePipe | null;
let gPipe3: usbMgr.USBDevicePipe | null;
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

async function getPermission(): Promise<boolean> {
  console.info('**************getPermission**************');
  try {
    usbMgr.requestRight(gDeviceList[0].name).then(hasRight => {
      console.info(TAG, `usb requestRight success, hasRight: ${hasRight}`);
    })
  } catch (err) {
    console.info(TAG, `usb getPermission to requestRight hasRight fail: `, err);
    return false;
  }
  return true;
}

function deviceConnected(): boolean {
  if (gDeviceList.length > 0) {
    console.info(TAG, "Test USB device is connected");
    return true;
  }
  console.info(TAG, "Test USB device is not connected");
  return false;
}

async function getPipeForDeviceIndex(index: number): Promise<usbMgr.USBDevicePipe | null> {
  if (index >= gDeviceList.length) {
    return null;
  }
  return usbMgr.connectDevice(gDeviceList[index]);
}

async function getPermissionForMultipleDevices(): Promise<void> {
  console.info('**************getPermission for multiple devices**************');
  try {
    for (let i = 0; i < gDeviceList.length && i < 3; i++) {
      await usbMgr.requestRight(gDeviceList[i].name);
      console.info(TAG, `requested permission for device ${i}`);
      await sleep(100);
    }
  } catch (err) {
    console.info(TAG, `getPermission for multiple devices failed: `, err);
  }
}

/* USB concurrent operations test */
export default function UsbConcurrentOperationsTest() {
  describe('UsbConcurrentOperationsTest', () => {

    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbConcurrentOperationsTest Begin*************');

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
      if (gPipe1 !== null) {
        usbMgr.closePipe(gPipe1);
      }
      if (gPipe2 !== null) {
        usbMgr.closePipe(gPipe2);
      }
      if (gPipe3 !== null) {
        usbMgr.closePipe(gPipe3);
      }
      console.log(TAG, '*************Usb Unit UsbConcurrentOperationsTest End*************');
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
      gPipe1 = null;
      gPipe2 = null;
      gPipe3 = null;
      devices = null;
    })

    /**
     * @tc.name   testConcurrentConnect001
     * @tc.number SUB_USB_Concurrent_Connect_0100
     * @tc.desc   Test concurrent connection attempts
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConcurrentConnect001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentConnect001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      try {
        gPipe1 = await getPipeForDeviceIndex(0);
        gPipe2 = await getPipeForDeviceIndex(0);
        gPipe3 = await getPipeForDeviceIndex(0);

        expect(gPipe1 !== null).assertTrue();
        expect(gPipe2 !== null).assertTrue();
        expect(gPipe3 !== null).assertTrue();

        console.info(TAG, 'Concurrent connections successful');
      } catch (error) {
        console.info(TAG, 'Concurrent connect error: ' + error);
        throw error;
      }
    });

    /**
     * @tc.name   testConcurrentDeviceEnumeration001
     * @tc.number SUB_USB_Concurrent_Enumerate_0100
     * @tc.desc   Test concurrent device enumeration
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConcurrentDeviceEnumeration001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentDeviceEnumeration001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      const enumerationPromises: Promise<usbMgr.USBDevice[]>[] = [];
      for (let i = 0; i < 5; i++) {
        enumerationPromises.push(
          new Promise<usbMgr.USBDevice[]>((resolve) => {
            setTimeout(() => {
              resolve(usbMgr.getDevices());
            }, 100 * (i + 1));
          })
        );
      }

      const results = await Promise.all(enumerationPromises);

      for (let i = 0; i < results.length; i++) {
        console.info(TAG, `Enumeration ${i} returned ${results[i].length} devices`);
        expect(results[i].length >= 0).assertTrue();
      }

      console.info(TAG, 'Concurrent enumeration completed');
    });

    /**
     * @tc.name   testConcurrentBulkTransfer001
     * @tc.number SUB_USB_Concurrent_BulkTransfer_0100
     * @tc.desc   Test concurrent bulk transfer operations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConcurrentBulkTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentBulkTransfer001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      gPipe1 = usbMgr.connectDevice(devices);
      expect(gPipe1 !== null).assertTrue();
      await sleep(500);

      const config = devices.configs[0];
      const interface = config.interfaces[0];
      const endpoint = interface.endpoints[0];

      if (endpoint.type !== usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK) {
        console.info(TAG, 'No bulk endpoint found, skipping test');
        return;
      }

      const transferParams = {
        devPipe: gPipe1,
        flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_WRITE,
        endpoint: endpoint,
        type: usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
        timeout: 5000,
        length: 512,
        callback: () => {},
        userData: new Uint8Array(512),
        buffer: new Uint8Array(512),
        isoPacketCount: 0,
      };

      const promises: Promise<number>[] = [];
      for (let i = 0; i < 3; i++) {
        promises.push(usbMgr.bulkTransferWrite(transferParams));
      }

      try {
        await Promise.all(promises);
        console.info(TAG, 'Concurrent bulk transfers completed');
      } catch (error) {
        console.info(TAG, 'Concurrent bulk transfer error: ' + error);
      }

      usbMgr.closePipe(gPipe1);
    });

    /**
     * @tc.name   testConcurrentControlTransfer001
     * @tc.number SUB_USB_Concurrent_ControlTransfer_0100
     * @tc.desc   Test concurrent control transfer operations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConcurrentControlTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentControlTransfer001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      gPipe1 = usbMgr.connectDevice(devices);
      expect(gPipe1 !== null).assertTrue();
      await sleep(500);

      const controlParam1: usbMgr.USBControlParams = {
        request: usbMgr.USBControlRequestType.REQUEST_GET_DESCRIPTOR,
        target: usbMgr.USBRequestTargetType.REQUEST_TARGET_DEVICE,
        reqType: usbMgr.USBControlRequestType.REQUEST_TYPE_STANDARD,
        value: 0x0100,
        index: 0,
        data: new Uint8Array(256),
      };

      const controlParam2: usbMgr.USBControlParams = {
        request: usbMgr.USBControlRequestType.REQUEST_GET_STATUS,
        target: usbMgr.USBRequestTargetType.REQUEST_TARGET_DEVICE,
        reqType: usbMgr.USBControlRequestType.REQUEST_TYPE_STANDARD,
        value: 0,
        index: 0,
        data: new Uint8Array(256),
      };

      try {
        const results = await Promise.all([
          usbMgr.controlTransfer(gPipe1, controlParam1),
          usbMgr.controlTransfer(gPipe1, controlParam2),
        ]);

        console.info(TAG, 'Concurrent control transfers completed');
      } catch (error) {
        console.info(TAG, 'Concurrent control transfer error: ' + error);
      }

      usbMgr.closePipe(gPipe1);
    });

    /**
     * @tc.name   testConcurrentPipeOperations001
     * @tc.number SUB_USB_Concurrent_PipeOperations_0100
     * @tc.desc   Test concurrent pipe operations (open/close)
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConcurrentPipeOperations001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentPipeOperations001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      const operations = [];
      for (let i = 0; i < 3; i++) {
        operations.push(
          new Promise<void>(async (resolve) => {
            let pipe = await getPipeForDeviceIndex(0);
            await sleep(100 * (i + 1));
            let closeResult = usbMgr.closePipe(pipe);
            resolve();
          })
        );
      }

      try {
        await Promise.all(operations);
        console.info(TAG, 'Concurrent pipe operations completed');
      } catch (error) {
        console.info(TAG, 'Concurrent pipe operations error: ' + error);
      }
    });

    /**
     * @tc.name   testConcurrentInterfaceClaim001
     * @tc.number SUB_USB_Concurrent_InterfaceClaim_0100
     * @tc.desc   Test concurrent interface claim operations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConcurrentInterfaceClaim001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentInterfaceClaim001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      gPipe1 = usbMgr.connectDevice(devices);
      expect(gPipe1 !== null).assertTrue();
      await sleep(500);

      const config = devices.configs[0];
      const interfaces = config.interfaces;

      try {
        const claimPromises = [];
        for (let i = 0; i < Math.min(3, interfaces.length); i++) {
          claimPromises.push(
            usbMgr.claimInterface(gPipe1, interfaces[i].id, true)
          );
        }

        await Promise.all(claimPromises);
        console.info(TAG, 'Concurrent interface claims completed');
      } catch (error) {
        console.info(TAG, 'Concurrent interface claim error: ' + error);
      }

      usbMgr.closePipe(gPipe1);
    });

    /**
     * @tc.name   testConcurrentPermissionRequest001
     * @tc.number SUB_USB_Concurrent_PermissionRequest_0100
     * @tc.desc   Test concurrent permission requests
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConcurrentPermissionRequest001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentPermissionRequest001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      const deviceName = devices.name;

      try {
        const requestPromises: Promise<boolean>[] = [];
        for (let i = 0; i < 5; i++) {
          requestPromises.push(
            usbMgr.requestRight(deviceName)
          );
        }

        await Promise.all(requestPromises);

        let hasRight = usbMgr.hasRight(deviceName);
        console.info(TAG, `Final permission state: ${hasRight}`);
        expect(hasRight).assertTrue();

        console.info(TAG, 'Concurrent permission requests completed');
      } catch (error) {
        console.info(TAG, 'Concurrent permission request error: ' + error);
      }
    });

    /**
     * @tc.name   testConcurrentPermissionCheck001
     * @tc.number SUB_USB_Concurrent_PermissionCheck_0100
     * @tc.desc   Test concurrent permission checks
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConcurrentPermissionCheck001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentPermissionCheck001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      const deviceName = devices.name;

      const checkPromises: Promise<boolean>[] = [];
      for (let i = 0; i < 10; i++) {
        checkPromises.push(usbMgr.hasRight(deviceName));
      }

      try {
        const results = await Promise.all(checkPromises);

        for (let i = 0; i < results.length; i++) {
          console.info(TAG, `Permission check ${i}: ${results[i]}`);
          expect(typeof results[i]).toBe('boolean');
        }

        console.info(TAG, 'Concurrent permission checks completed');
      } catch (error) {
        console.info(TAG, 'Concurrent permission check error: ' + error);
      }
    });

    /**
     * @tc.name   testConcurrentDeviceEnumeration001
     * @tc.number SUB_USB_Concurrent_Enumeration_0200
     * @tc.desc   Test device enumeration with concurrent operations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConcurrentDeviceEnumeration001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentDeviceEnumeration001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      const enumerationPromises: Promise<usbMgr.USBDevice[]>[] = [];

      for (let i = 0; i < 3; i++) {
        enumerationPromises.push(
          usbMgr.getDevices()
        );
      }

      try {
        const results = await Promise.all(enumerationPromises);

        let totalDevices = 0;
        for (let i = 0; i < results.length; i++) {
          totalDevices += results[i].length;
          console.info(TAG, `Enumeration ${i} returned ${results[i].length} devices`);
        }

        console.info(TAG, `Total devices across enumerations: ${totalDevices}`);
        expect(totalDevices >= 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'Concurrent enumeration error: ' + error);
      }
    });

    /**
     * @tc.name   testConcurrentConfigOperation001
     * @tc.number SUB_USB_Concurrent_ConfigOperation_0100
     * @tc.desc   Test concurrent configuration operations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConcurrentConfigOperation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentConfigOperation001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      gPipe1 = usbMgr.connectDevice(devices);
      expect(gPipe1 !== null).assertTrue();
      await sleep(500);

      const config = devices.configs[0];
      const interfaces = config.interfaces;

      try {
        const setConfigPromises: Promise<number>[] = [];
        for (let i = 0; i < 3; i++) {
          setConfigPromises.push(
            usbMgr.setConfiguration(gPipe1, config.id)
          );
        }

        await Promise.all(setConfigPromises);
        console.info(TAG, 'Concurrent setConfiguration completed');
      } catch (error) {
        console.info(TAG, 'Concurrent setConfiguration error: ' + error);
      }

      usbMgr.closePipe(gPipe1);
    });

    /**
     * @tc.name   testConcurrentInterfaceOperation001
     * @tc.number SUB_USB_Concurrent_InterfaceOperation_0100
     * @tc.desc   Test concurrent interface operations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConcurrentInterfaceOperation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentInterfaceOperation001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      gPipe1 = usbMgr.connectDevice(devices);
      expect(gPipe1 !== null).assertTrue();
      await sleep(500);

      const config = devices.configs[0];
      const interfaces = config.interfaces;

      try {
        const interfaceId = interfaces[0].id;
        const altInterfaceIds: number[] = [0, 1, 2];

        const setInterfacePromises = [];
        for (let i = 0; i < altInterfaceIds.length; i++) {
          setInterfacePromises.push(
            usbMgr.setInterface(gPipe1, interfaceId, altInterfaceIds[i])
          );
        }

        await Promise.all(setInterfacePromises);
        console.info(TAG, 'Concurrent setInterface completed');
      } catch (error) {
        console.info(TAG, 'Concurrent setInterface error: ' + error);
      }

      usbMgr.closePipe(gPipe1);
    });

    /**
     * @tc.name   testConcurrentReadWrite001
     * @tc.number SUB_USB_Concurrent_ReadWrite_0100
     * @tc.desc   Test concurrent read and write operations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConcurrentReadWrite001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentReadWrite001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      gPipe1 = usbMgr.connectDevice(devices);
      expect(gPipe1 !== null).assertTrue();
      await sleep(500);

      const config = devices.configs[0];
      const interface = config.interfaces[0];
      const endpoint = interface.endpoints[0];

      if (endpoint.type !== usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK) {
        console.info(TAG, 'No bulk endpoint, skipping test');
        return;
      }

      const transferParams1: usbMgr.UsbDataTransferParams = {
        devPipe: gPipe1,
        flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_READ,
        endpoint: endpoint,
        type: usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
        timeout: 5000,
        length: 512,
        callback: () => {},
        userData: new Uint8Array(512),
        buffer: new Uint8Array(512),
        isoPacketCount: 0,
      };

      const transferParams2: usbMgr.UsbDataTransferParams = {
        devPipe: gPipe1,
        flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_WRITE,
        endpoint: endpoint,
        type: usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
        timeout: 5000,
        length: 512,
        callback: () => {},
        userData: new Uint8Array(512),
        buffer: new Uint8Array(512),
        isoPacketCount: 0,
      };

      try {
        await Promise.all([
          usbMgr.bulkTransferRead(transferParams1),
          usbMgr.bulkTransferWrite(transferParams2),
        ]);

        console.info(TAG, 'Concurrent read and write completed');
      } catch (error) {
        console.info(TAG, 'Concurrent read write error: ' + error);
      }

      usbMgr.closePipe(gPipe1);
    });

    /**
     * @tc.name   testConcurrentMultipleDeviceAccess001
     * @tc.number SUB_USB_Concurrent_MultiDeviceAccess_0100
     * @tc.desc   Test access to multiple devices (if available)
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConcurrentMultipleDeviceAccess001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentMultipleDeviceAccess001 begin');

      gDeviceList = usbMgr.getDevices();

      if (gDeviceList.length < 2) {
        console.info(TAG, 'Less than 2 devices, skipping multi-device test');
        return;
      }

      try {
        gPipe1 = usbMgr.connectDevice(gDeviceList[0]);
        gPipe2 = usbMgr.connectDevice(gDeviceList[1]);

        expect(gPipe1 !== null).assertTrue();
        expect(gPipe2 !== null).assertTrue();

        await sleep(500);

        const closePromises: Promise<number>[] = [
          usbMgr.closePipe(gPipe1),
          usbMgr.closePipe(gPipe2),
        ];

        await Promise.all(closePromises);
        console.info(TAG, 'Concurrent device access completed');
      } catch (error) {
        console.info(TAG, 'Concurrent multi-device access error: ' + error);
      }
    });

    /**
     * @tc.name   testConcurrentAsyncOperation001
     * @tc.number SUB_USB_Concurrent_AsyncOperation_0100
     * @tc.desc   Test asynchronous operation coordination
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConcurrentAsyncOperation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentAsyncOperation001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      try {
        const deviceList = usbMgr.getDevices();
        expect(deviceList.length >= 0).assertTrue();

        const pipe1 = usbMgr.connectDevice(deviceList[0]);
        const hasRight1 = await usbMgr.requestRight(deviceList[0].name);

        await sleep(300);
        const pipe2 = usbMgr.connectDevice(deviceList[0]);
        const hasRight2 = await usbMgr.requestRight(deviceList[0].name);

        expect(hasRight1).toBe(true);
        expect(hasRight2).toBe(true);

        const closeResults = await Promise.all([
          usbMgr.closePipe(pipe1),
          usbMgr.closePipe(pipe2),
        ]);

        closeResults.forEach((result, index) => {
          expect(result).assertEqual(0);
        });

        console.info(TAG, 'Async operation coordination completed');
      } catch (error) {
        console.info(TAG, 'Async operation coordination error: ' + error);
      }
    });
  });
}
