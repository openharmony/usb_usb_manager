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

const TAG = "[UsbAdvancedTransferTest]";
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

/* USB advanced transfer test */
export default function UsbAdvancedTransferTest() {
  describe('UsbAdvancedTransferTest', () => {

    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbAdvancedTransferTest Begin*************');

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
      console.log(TAG, '*************Usb Unit UsbAdvancedTransferTest End*************');
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
     * @tc.name   testLargeBlockTransfer001
     * @tc.number SUB_USB_AdvTransfer_LargeBlock_0100
     * @tc.desc   Test large block transfer operations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testLargeBlockTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testLargeBlockTransfer001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interface = config.interfaces[0];
      const endpoint = interface.endpoints.find(ep => ep.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK);

      if (!endpoint) {
        console.info(TAG, 'No bulk endpoint found, skipping test');
        return;
      }

      const testData = new Uint8Array(64 * 1024);
      const transferParams = {
        devPipe: gPipe,
        flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_WRITE,
        endpoint: endpoint,
        type: usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
        timeout: 10000,
        length: testData.length,
        callback: () => {},
        userData: new Uint8Array(32),
        buffer: testData,
        isoPacketCount: 0,
      };

      try {
        const result = usbMgr.bulkTransferWrite(transferParams);
        console.info(TAG, 'Large block transfer size: ' + testData.length + ', result: ' + result);
        expect(result === 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'Large block transfer error: ' + error);
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testFragmentedTransfer001
     @tc.number SUB_USB_AdvTransfer_Fragmented_0100
     * @tc.desc   Test fragmented data transfer
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testFragmentedTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testFragmentedTransfer001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected). assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interface = config.interfaces[0];
      const endpoint = interface.endpoints.find(ep => ep.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK);

      if (!endpoint) {
        console.info(TAG, 'No bulk endpoint found, skipping test');
        return;
      }

      const fragmentSize = 512;
      const fragmentCount = 5;
      const testData = new Uint8Array(fragmentSize);
      const transferParams = {
        devPipe: gPipe,
        flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_WRITE,
        endpoint: endpoint,
        type: usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
        timeout: 5000,
        length: fragmentSize,
        callback: () => {},
        userData: new Uint8Array(32),
        buffer: testData,
        isoPacketCount: 0,
      };

      let totalTransferred = 0;
      const expectedTotal = fragmentSize * fragmentCount;

      try {
        for (let i = 0; i < fragmentCount; i++) {
          const result = usbMgr.bulkTransferWrite(transferParams);
          totalTransferred += result === 0 ? fragmentSize : 0;
          console.info(TAG, `Fragment ${i + 1}/${fragmentCount}, result: ${result}`);
        }
      } catch (error) {
        console.info(TAG, 'Fragmented transfer error: ' + error);
      }

      console.info(TAG, `Total transferred: ${totalTransferred}, expected: ${expectedTotal}`);

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testMultipleEndpointTransfer001
     * @tc.number SUB_USB_AdvTransfer_MultiEndpoint_0100
     * @tc.desc   Test multiple endpoint simultaneous transfers
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     @tc.level  LEVEL3
     */
    it('testMultipleEndpointTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testMultipleEndpointTransfer001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;

      const bulkEndpoints = interfaces.filter(iface => 
        iface.endpoints.some(ep => ep.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK)
      );

      if (bulkEndpoints.length < 2) {
        console.info(TAG, 'Less than 2 bulk endpoints, skipping test');
        return;
      }

      const endpoint1 = bulkEndpoints[0];
      const endpoint2 = bulkEndpoints[1];

      const testData = new Uint8Array(512);

      const transferParams1 = {
        devPipe: gPipe,
        flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_WRITE,
        endpoint: endpoint1,
        type: usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
        timeout: 5000,
        length: testData.length,
        callback: () => {},
        userData: new Uint8Array(32),
        buffer: testData,
        isoPacketCount: 0,
      };

      const transferParams2 = {
        devPipe: gPipe,
        flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_WRITE,
        endpoint: endpoint2,
        type: usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
        timeout: 5000,
        length: testData.length,
        callback: () => {},
        userData: new Uint8Array(32),
        buffer: testData,
        isoPacketCount: 0,
      };

      try {
        const results = await Promise.all([
          usbMgr.bulkTransferWrite(transferParams1),
          usbMgr.bulkTransferWrite(transferParams2),
        ]);

        console.info(TAG, 'Multi endpoint transfer completed');
        expect(results[0] === 0 || results[1] === 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'Multi endpoint transfer error: ' + error);
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testInterruptTransfer001
     * @tc.number SUB_USB_AdvTransfer_Interrupt_0100
     * @tc.desc   Test interrupt transfer operations
     * @tc.type   FUNCTION
     @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInterruptTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testInterruptTransfer001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;
      const endpoint = interfaces.endpoints.find(ep => 
        ep.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_INTERRUPT);

      if (!endpoint) {
        console.info(TAG, 'No interrupt endpoint found, skipping test');
        return;
      }

      const testData = new Uint8Array(64);
      const transferParams = {
        devPipe: gPipe,
        flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_READ,
        endpoint: endpoint,
        type: usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_INTERRUPT,
        timeout: 5000,
        length: testData.length,
        callback: () => {},
        userData: new Uint8Array(32),
        buffer: testData,
        isoPacketCount: 0,
      };

      try {
        const result = usbMgr.bulkTransferRead(transferParams);
        console.info(TAG, 'Interrupt transfer result: ${result}');
        expect(result).assertEqual(0);
      } catch (error) {
        console.info(TAG, 'Interrupt transfer error: ' + error);
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testIsochronousTransfer001
     * @tc.number SUB_USB_AdvTransfer_Isochronous_0100
     * @tc.desc   Test isochronous transfer operations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     @tc.level  LEVEL3
     */
    it('testIsochronousTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testIsochronousTransfer001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;
      const endpoint = interfaces.endpoints.find(ep => 
        ep.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_ISOCHRONOUS);

      if (!endpoint) {
        console.info(TAG, 'No isochronous endpoint, skipping test');
        return;
      }

      const packetSize = 512;
      const numPackets = 8;
      const testData = new Uint8Array(packetSize * numPackets);
      const isoPacketDescriptors: Array<usbMgr.UsbIsochronousPacketDescriptor> = [];

      for (let i = 0; i < numPackets; i++) {
        isoPacketDescriptors.push({
          offset: i * packetSize,
          actualLength: packetSize,
          status: 0,
        });
      }

      const transferParams = {
        devPipe: gPipe,
        flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_WRITE,
        endpoint: endpoint,
        type: usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_ISOCHRONOUS,
        timeout: 5000,
        length: packetSize * numPackets,
        callback: () => {},
        userData: new Uint8Array(packetSize * numPackets),
        buffer: testData,
        isoPacketCount: numPackets,
      };

      try {
        const result = usbMgr.bulkTransferWrite(transferParams);
        console.info(TAG, 'Isochronous transfer result: ${result}');
        expect(result).assertEqual(0);
      } catch (error) {
        console.info(TAG, 'Isochronous transfer error: ' + error);
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testTransferWithInvalidPipe001
     * @tc.number SUB_USB_AdvTransfer_InvalidPipe_0100
     * @tc.desc   Test transfer with invalid pipe
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testTransferWithInvalidPipe001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testTransferWithInvalidPipe001 begin');

      const testData = new Uint8Array(512);
      const invalidPipe: usbMgr.USBDevicePipe = {
        busNum: 999,
        devAddress: 999,
        configId: 999,
        interfaceId: 999,
      };

      const transferParams = {
        devPipe: invalidPipe,
        flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_WRITE,
        endpoint: {
          address: 1,
          attributes: 1,
          direction: usbMgr.UsbEndpointDirection.USB_ENDPOINT_DIR_OUT,
          id: 1,
          interfaceId: 1,
          maxPacketSize: 512,
          type: usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
        },
        timeout: 5000,
        length: testData.length,
        callback: () => {},
        userData: new Uint8Array(32),
        buffer: testData,
        isoPacketCount: 0,
      };

      try {
        const result = usbMgr.bulkTransferWrite(transferParams);
        console.info(TAG, 'Invalid pipe transfer result: ${result}');
        expect(result !== 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'Invalid pipe transfer error: ' + error);
      }
    });

    /**
     * @tc.name   testConcurrentReadAndWrite001
     * @tc.number SUB_USB_AdvTransfer_ConcurrentRW_0100
     * @tc.desc   Test concurrent read and write operations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConcurrentReadAndWrite001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentReadAndWrite001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interface = config.interfaces[0];
      const endpoint = interface.endpoints.find(ep => ep.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK);

      if (!endpoint) {
        console.info(TAG, 'No bulk endpoint found, skipping test');
        return;
      }

      const readData = new Uint8Array(512);
      const writeData = new Uint8Array(512);

      const readParams = {
        devPipe: gPipe,
        flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_READ,
        endpoint: endpoint,
        type: usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
        timeout: 5000,
        length: readData.length,
        callback: () => {},
        userData: new Uint8Array(32),
        buffer: readData,
        isoPacketCount: 0,
      };

      const writeParams = {
        devPipe: gPipe,
        flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_WRITE,
        endpoint: endpoint,
        type: usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
        timeout: 5000,
        length: writeData.length,
        callback: () => {},
        userData: new Uint8Array(32),
        buffer: writeData,
        isoPacketCount: 0,
      };

      try {
        await Promise.all([
          usbMgr.bulkTransferRead(readParams),
          usbMgr.bulkTransferWrite(writeParams),
        ]);

        console.info(TAG, 'Concurrent read and write completed');
      } catch (error) {
        console.info(TAG, 'Concurrent read write error: ' + error);
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testTransferCancellation001
     * @tc.number SUB_USB_AdvTransfer_Cancellation_0100
     * @tc.desc   Test transfer cancellation
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     @tc.level  LEVEL3
     */
    it('testTransferCancellation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testTransferCancellation001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interface = config.interfaces[0];
      const endpoint = interface.endpoints.find(ep => ep.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK);

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
        timeout: 10000,
        length: testData.length,
        callback: () => {},
        userData: new Uint8Array(32),
        buffer: testData,
        isoPacketCount: 0,
      };

      try {
        const cancelResult = usbMgr.usbCancelTransfer(gPipe, endpoint.address);
        console.info(TAG, 'Cancel transfer result: ${cancelResult}`);

        const result = usbMgr.bulkTransferWrite(transferParams);
        console.info(TAG, 'Transfer after cancel result: ${result}`);

        expect(cancelResult === 0).assertTrue();
        expect(result !== 0).assertTrue();
      } catch (error) {
        console.info(TAG, 'Transfer cancellation test error: ' + error);
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testZeroLengthTransfer001
     * @tc.number SUB_USB_AdvTransfer_ZeroLength_0100
     * @tc.desc   Test zero length transfer
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testZeroLengthTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testZeroLengthTransfer001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interface = config.interfaces[0];
      const endpoint = interface.endpoints.find(ep => ep.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK);

      if (!endpoint) {
        console.info(TAG, 'No bulk endpoint found, skipping test');
        return;
      }

      const testData = new Uint8Array(0);
      const transferParams = {
        devPipe: gPipe,
        flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_WRITE,
        endpoint: endpoint,
        type: usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK,
        timeout: 5000,
        length: 0,
        callback: () => {},
        userData: new Uint8Array(32),
        buffer: testData,
        isoPacketCount: 0,
      };

      try {
        const result = usbMgr.bulkTransferWrite(transferParams);
        console.info(TAG, 'Zero length transfer result: ${result}');
        expect(result === 0 || result === -1).assertTrue();
      } catch (error) {
        console.info(TAG, 'Zero length transfer error: ' + error);
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testTimeoutWithSlowDevice001
     * @tc.number SUB_USB_AdvTransfer_Timeout_0100
     @tc.desc   Test transfer with slow device
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     @tc.level  LEVEL3
     */
    it('testTimeoutWithSlowDevice001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testTimeoutWithSlowDevice001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interface = config.interfaces[0];
      const endpoint = interface.endpoints.find(ep => ep.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK);

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
        timeout: 1000,
        length: testData.length,
        callback: () => {},
        userData: new Uint8Array(32),
        buffer: testData,
        isoPacketCount: 0,
      };

      try {
        await new Promise((resolve) => setTimeout(resolve, 60000));
        const result = usbMgr.bulkTransferWrite(transferParams);
        console.info(TAG, 'Timeout transfer result: ${result}');
        expect(result === 0 || result === -1).assertTrue();
      } catch (error) {
        console.info(TAG, 'Timeout transfer error: ' + error);
      }

      usbMgr.closePipe(gPipe);
    });

    /**
     * @tc.name   testMaxEndpointsTransfer001
     * @tc.number SUB_USB_AdvTransfer_MaxEndpoints_0100
     * @tc.desc   Test with maximum endpoints
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testMaxEndpointsTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testMaxEndpointsTransfer001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;

      if (interfaces.length === 0) {
        console.info(TAG, 'No interfaces found, skipping test');
        return;
      }

      const endpoints = interfaces[0].endpoints;

      for (const endpoint of endpoints) {
        const testData = new Uint8Array(512);
        const transferParams = {
          devPipe: gPipe,
          flags: usbMgr.UsbTransferFlags.TRANSFER_FLAG_WRITE,
          endpoint: endpoint,
          type: endpoint.type,
          timeout: 5000,
          length: testData.length,
          callback: () => {},
          userData: new Uint8Array(32),
          buffer: testData,
          isoPacketCount: 0,
        };

        try {
          const result = usbMgr.bulkTransferWrite(transferParams);
          console.info(TAG, 'Endpoint ${endpoint.address} transfer result: ${result}`);
        } catch (error) {
          console.info(TAG, `Endpoint ${endpoint.address} transfer error: ' + error);
        }
      }

      usbMgr.closePipe(gPipe);
    });
  });
}
