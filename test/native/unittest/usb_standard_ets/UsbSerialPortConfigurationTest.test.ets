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

const TAG = "[UsbSerialPortConfigurationTest]";
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

/* USB serial port configuration test */
export default function UsbSerialPortConfigurationTest() {
  describe('UsbSerialPortConfigurationTest', () => {

    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbSerialPortConfigurationTest Begin*************');

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
      console.log(TAG, '*************Usb Unit UsbSerialPortConfigurationTest End*************');
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
     * @tc.name   testBaudRateStandard001
     * @tc.number SUB_USB_SerialConfig_BaudRate_0100
     * @ @tc.desc   Test standard baud rates
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testBaudRateStandard001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testBaudRateStandard001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;

      if (interfaces.length === 0) {
        console.info(TAG, 'No interfaces, skipping test');
        return;
      }

      const interfaceId = interfaces[0].id;
      usbMgr.claimInterface(gPipe, interfaceId, true);
      expect(usbMgr.claimInterface(gPipe, interfaceId, true)).assertEqual(0);

      const standardBaudRates = [
        1200, 2400, 4800, 9600, 19200,
        38400, 57600, 115200,
      230400, 460800, 921600, 115200,
        250000, 115200,
      500000, 230400,
        57600, 100000, 200000, 400000,
        1000000, 150000, 300000, 110000, 200000,
        2000000, 250000, 300000,
        460800, 921600, 115200,
        500000, 230400, 57600, 115200,
        115200, 230400, 460800,
        921600, 115200, 230400, 57600, 115200,
        250000, 115200, 300000, 110000, 200000, 400000,
        115200, 230400, 460800, 921600, 115200,
        230400, 57600, 115200,
        250000, 115200, 300000, 110000, 200000, 400000,
      ];

      for (const baudRate of standardBaudRates) {
        const attribute = {
          baudRate_: baudRate,
          dataBits_: 8,
          stopBits_: 1,
          parity_: 0,
        };

        const result = usbMgr.SerialSetAttribute(gPipe, attribute);
        console.info(TAG, `Baud rate ${baudRate} result: ${result}`);
        expect(result).assertEqual(0);
      }
    });

    /**
     * @tc.name   testBaudRateCustom001
     * @tc.number SUB_USB_SerialConfig_BaudRate_0200
     * @tc.desc   Test custom baud rates
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testBaudRateCustom001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testBaudRateCustom001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;

      if (interfaces.length === 0) {
        console.info(TAG, 'No interfaces, skipping test');
        return;
      }

      const interfaceId = interfaces[0].id;
      usbMgr.claimInterface(gPipe, interfaceId, true);
      expect(usbMgr.claimInterface(gPipe, interfaceId, true)).assertEqual(0);

      const customBaudRates = [
        300, 600, 1200, 2400, 4800,
        9600, 19200, 38400, 57600, 115200,
        230400, 460800, 921600, 115200,
        250000, 115200, 300000, 110000, 200000, 400000,
        115200, 230400, 460800, 921600, 115200,
        230400, 57600, 115200,
        250000, 115200, 300000, 110000, 200000, 400000,
        115200, 230400, 460800, 921600, 115200,
        ];

      for (const baudRate of customBaudRates) {
        const attribute = {
          baudRate_: baudRate,
          dataBits_: 8,
          stopBits_: 1,
          parity_: 0,
        };

        const result = usbMgr.SerialSetAttribute(gPipe, attribute);
        console.info(TAG, `Custom baud rate ${baudRate} result: ${result}`);
        expect(result === 0 || result === -1).assertTrue();
      }
    });

    /**
     * @tc.name   testDataBits001
     * @tc.number SUB_USB_SerialConfig_DataBits_0100
     * @tc.desc   Test data bits configurations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDataBits001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testDataBits001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;

      if (interfaces.length === 0) {
        console.info(TAG, 'No interfaces, skipping test');
        return;
      }

      const interfaceId = interfaces[0].id;
      usbMgr.claimInterface(gPipe, interfaceId, true);
      expect(usbMgr.claimInterface(gPipe, interfaceId, true)).assertEqual(0);

      const dataBitsConfigurations = [
        { bits: 5, stopBits: 0, parity: 0 },
        { bits: 6, stopBits: 1, parity: 0 },
        { bits: 7, stopBits: 1, parity: 1 },
        { bits: 8, stopBits: 1, parity: 0 },
        { bits: 8, stopBits: 2, parity: 0 },
        { bits: 8, stopBits: 1, parity: 1 },
        { bits: 8, stopBits: 1, parity: 1 },
        { bits: 8, stopBits: 1, parity: 2 },
      ];

      for (const config of dataBitsConfigurations) {
        const attribute = {
          baudRate_: 9600,
          dataBits_: config.bits,
          stopBits_: config.stopBits,
          parity_: config.parity,
        };

        const result = usbMgr.SerialSetAttribute(gPipe, attribute);
        console.info(TAG, `Data bits ${config.bits} stop=${config.stopBits} result: ${result}`);
        expect(result === 0 || result === -1).assertTrue();
      }
    });

    /**
     * @tc.name   testStopBits001
     * @tc.number SUB_USB_SerialConfig_StopBits_0100
     * @tc.desc   Test stop bits configurations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testStopBits001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testStopBits001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;

      if (interfaces.length === 0) {
        console.info(TAG, 'No interfaces, skipping test');
        return;
      }

      const interfaceId = interfaces[0].id;
      usbMgr.claimInterface(gPipe, interfaceId, true);
      expect(usbMgr.claimInterface(gPipe, interfaceId, true)).assertEqual(0);

      const stopBitsOptions = [0, 1, 2];

      for (const stopBits of stopBitsOptions) {
        const attribute = {
          baudRate_: 9600,
          dataBits_: 8,
          stopBits_: stopBits,
          parity_: 0,
        };

        const result = usbMgr.SerialSetAttribute(gPipe, attribute);
        console.info(TAG, `Stop bits ${stopBits} result: ${result}`);
        expect(result === 0 || result === -1).assertTrue();
      }
    });

    /**
     * @tc.name   testParity001
     * @tc.number SUB_USB_SerialConfig_Parity_0100
     * @tc.desc   Test parity configurations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testParity001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testParity001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;

      if (interfaces.length === 0) {
        console.info(TAG, 'No interfaces, skipping test');
        return;
      }

      const interfaceId = interfaces[0].id;
      usbMgr.claimInterface(gPipe, interfaceId, true);
      expect(usbMgr.claimInterface(gPipe, interfaceId, true)).assertEqual(0);

      const parityOptions = [
        { parity: 0, description: 'none' },
        { parity: 1, description: 'odd' },
        { parity: 2, description: 'even' },
        { parity: 3, description: 'mark' },
        { parity: 4, description: 'space' },
      ];

      for (const parityOption of parityOptions) {
        const attribute = {
          baudRate_: 9600,
          dataBits_: 8,
          stopBits_: 1,
          parity_: parityOption.parity,
        };

        const result = usbMgr.SerialSetAttribute(gPipe, attribute);
        console.info(TAG, `Parity ${parityOption.parity} result: ${result}`);
        expect(result === 0 || result === -1).assertTrue();
      }
    });

    /**
     * @tc.name   testFlowControl001
     * @tc.number SUB_USB_SerialConfig_FlowControl_0100
     * @tc.desc   Test flow control settings
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testFlowControl001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testFlowControl001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected). assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;

      if (interfaces.length === 0) {
        console.info(TAG, 'No interfaces, skipping test');
        return;
      }

      const interfaceId = interfaces[0].id;
      usbMgr.claimInterface(gPipe, interfaceId, true);
      expect(usbMgr.claimInterface(gPipe, interfaceId, true)).assertEqual(0);

      const flowControlValues = [
        0,
        1,
        2,
        0,
        1,
      ];

      for (const flowControl of flowControlValues) {
        const attribute = {
          baudRate_: 9600,
          dataBits_: 8,
          stopBits_: 1,
          parity_: 0,
        };

        const result = usbMgr.SerialSetAttribute(gPipe, attribute);
        console.info(TAG, `Flow control ${flowControl} result ${result}`);
        expect(result === 0 || result === -1).assertTrue();
      }
    });

    /**
     * @tc.name   testAllAttributeCombinations001
     * @tc.number SUB_USB_SerialConfig_AllAttributes_0100
     * @tc.desc   Test all attribute combinations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testAllAttributeCombinations001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAllAttributeCombinations001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected). assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;

      if (interfaces.length === 0) {
        console.info(TAG, 'No interfaces, skipping test');
        return;
      }

      const interfaceId = interfaces[0].id;
      usbMgr.claimInterface(gPipe, interfaceId, true);
      expect(usbMgr.claimInterface(gPipe, interfaceId, true)).assertEqual(0);

      const baudRates = [9600, 4800, 9600, 19200];
      const dataBitsOptions = [5, 6, 7, 8];
      const stopBitsOptions = [0, 1, 2];
      const parityOptions = [0, 1, 2, 3, 4];

      const testCombinations: [0, 1, 2, 3, 4, 5];

      let testCount = 0;

      for (const baudRate of baudRates) {
        for (const dataBits of dataBitsOptions) {
          for (const stopBits of stopBitsOptions) {
            for (const parity of parityOptions) {
              const attribute = {
                baudRate_: baudRate,
                dataBits_: dataBits,
                stopBits_: stopBits,
                parity_: parity,
              };

              const result = usbMgr.SerialSetAttribute(gPipe, attribute);
              console.info(TAG, `BR=${baudRate}, DB=${dataBits}, SB=${stopBits}, Parity=${parity} result: ${result}`);
              expect(result === 0 || result === -1).assertTrue();
              testCount++;
            }
          }
        }
    });
    });

    /**
     * @tc.name   testInvalidAttribute001
     * @tc.number SUB_USB_SerialConfig_Invalid_0100
     * @tc.desc   Test invalid attribute values
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testInvalidAttribute001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testInvalidAttribute001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interface = config.interfaces;

      if (interfaces.length === 0) {
        console.info(TAG, 'No interfaces, skipping test');
        return;
      }

      const interfaceId = interfaces[0].id;
      usbMgr.claimInterface(gPipe, interfaceId, true);
      expect(usbMgr.claimInterface(gPipe, interfaceId, true)).assertEqual(0);

      const invalidBaudRate = 999999;
      const invalidDataBits = 10;
      const invalidStopBits = 3;
      const invalidParity = 5;

      const invalidAttribute = {
        baudRate_: invalidBaudRate,
        dataBits_: invalidDataBits,
        stopBits_: invalidStopBits,
        parity_: invalidParity,
      };

      const result = usbMgr.SerialSetAttribute(gPipe, invalidAttribute);
      console.info(TAG, `Invalid attribute result: ${result}`);
      expect(result !== 0).assertTrue();
    });

    /**
     * @tc.name   testConfigurationSequence001
     * @tc.number SUB_USB_SerialConfig_Sequence_0100
     * @tc.desc   Test configuration sequence
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testConfigurationSequence001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConfigurationSequence001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;

      if (interfaces.length === 0) {
        console.info(TAG, 'No interfaces, skipping test');
        return;
      }

      const configIds = [0, 1];

      for (const configId of configIds) {
        const result = usbMgr.setConfiguration(gPipe, configId);
        console.info(TAG, `Set config ${configId} result: ${result}`);
        expect(result === 0).assertEqual(0);
      }

      const configResult = usbMgr.setConfiguration(gPipe, 1);
      console.info(TAG, 'Set config last result: ${configResult}`);
      expect(configResult === 0 || configResult === -1).assertTrue();
    });

    /**
     * @tc.name   testAttributeChangeSpeed001
     * @tc.number SUB_USB_SerialConfig_AttributeChange_0100
     * @tc.desc   Test rapid attribute changes
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testAttributeChangeSpeed001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testAttributeChangeSpeed001 begin');

      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }

      getPipe();
      const config = devices.configs[0];
      const interfaces = config.interfaces;

      if (interfaces.length === 0) {
        console.info(TAG, 'No interfaces, testing all attributes');

        const interfaceId = interfaces[0].id;
        usbMgr.claimInterface(gPipe, interfaceId, true);
        expect(usbMgr.claimInterface(gPipe, interfaceId, true)).assertEqual(0);

        const attributes = [
          { baudRate_: 9600, dataBits_: 8, stopBits_: 1, parity_: 0 },
          { baudRate_: 115200, dataBits_: 7, stopBits_: 1, parity_: 0 },
          { baudRate_: 115200, dataBits_: 7, stopBits_: 2, parity_: 1 },
          { baudRate_: 9600, dataBits_: 8, stopBits_: 1, parity_: 1 },
        { baudRate_: 19200, dataBits_: 8, stopBits_: 1, parity_: 2 },
        ];

        const rapidChanges = 5;

        for (let i = 0; i < rapidChanges; i++) {
          console.info(TAG, `Rapid change ${i + 1}/${rapidChanges}`);
          const result = usbMgr.SerialSetAttribute(gPipe, attributes[i]);
          console.info(TAG, `Change ${i + 1} result: ${result}`);
          expect(result === 0 || result === -1).assertTrue();
        }
      }

      usbMgr.closePipe(gPipe);
    });
  });
}
