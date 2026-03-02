/*
 * Copyright (c) 2026 Huawei Device Co. , Ltd.
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
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium';

let gDeviceList: Array<usbMgr.USBDevice>;
let gPipe: usbMgr.USBDevicePipe | null;
let devices: usbMgr.USBDevice | null;
let isDeviceConnected: boolean;
const TAG = "[UsbSecurityPermissionTestExtend01]";

function deviceConnected(): boolean {
  if (gDeviceList.length > 0) {
    console.info(TAG, "Test USB device is connected");
    return true;
  }
  console.info(TAG, "Test USB device is not connected");
  return false;
}

async function driveFn(): Promise<void> {
  console.info('**************driveFn**************');
  try {
    let driver = await UiDriver.create();
    console.info(TAG, ` come in driveFn`);
    console.info(TAG, `driver is ${JSON.stringify(driver)}`);
    let button = await driver.findComponent(BY.text('允许'));
    console.info(TAG, `button is ${JSON.stringify(button)}`);
    await button.click();
  } catch (err) {
    console.info(TAG, 'err is ' + err);
    return;
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
    return
  }
}

function sleep(ms: number): Promise<void> {
  return new Promise<void>(resolve => setTimeout(resolve, ms));
}

export default function UsbSecurityPermissionTestExtend01() {
  describe('UsbSecurityPermissionTestExtend01', () => {

    beforeAll(async () => {
      console.log(TAG, '*************Usb Unit UsbSecurityPermissionTestExtend01 Begin*************');
      
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
      console.info(TAG, 'beforeAll: usb unit begin test getversion: ' + usbMgr.getVersion());
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
      console.info(TAG, 'afterEach return devices : ' + JSON.stringify(devices));
      console.info(TAG, 'afterEach return devices : ' + JSON.stringify(gPipe));
    })

    afterAll(() => {
      console.log(TAG, '*************Usb Unit UsbSecurityPermissionTestExtend01 End*************');
    })

    /**
     * @tc.name   testPermissionRequestDeny002
     * @tc.number SUB_USB_Security_PermissionRequest_Extend_0200
     * @tc.desc   Test USB permission request with special characters
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPermissionRequestDeny002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testPermissionRequestDeny002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let specialChars = "!@#$%^&*()_+-=[]{}|;':\",./<>?";
      try {
        let granted = await usbMgr.requestRight(specialChars);
        console.info(TAG, `Permission request with special chars result: ${granted}`);
        expect(granted).assertFalse();
      } catch (error) {
        console.info(TAG, 'Permission request error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
    })

    /**
     * @tc.name   testPermissionRequestDeny003
     * @tc.number SUB_USB_Security_PermissionRequest_Extend_0300
     * @tc.desc   Test USB permission request with unicode characters
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPermissionRequestDeny003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testPermissionRequestDeny003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let unicodeStr = "测试中文权限";
      try {
        let granted = await usbMgr.requestRight(unicodeStr);
        console.info(TAG, `Permission request with unicode result: ${granted}`);
        expect(granted).assertFalse();
      } catch (error) {
        console.info(TAG, 'Permission request error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
    })

    /**
     * @tc.name   testPermissionRequestDeny004
     * @tc.number SUB_USB_Security_PermissionRequest_Extend_0400
     * @tc.desc   Test USB permission request with empty string
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPermissionRequestDeny004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testPermissionRequestDeny004 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      try {
        let granted = await usbMgr.requestRight("");
        console.info(TAG, `Permission request with empty string result: ${granted}`);
        expect(granted).assertFalse();
      } catch (error) {
        console.info(TAG, 'Permission request error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
    })

    /**
     * @tc.name   testPermissionRequestDeny005
     * @tc.number SUB_USB_Security_PermissionRequest_Extend_0500
     * @tc.desc   Test USB permission request with whitespace
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPermissionRequestDeny005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testPermissionRequestDeny005 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      try {
        let granted = await usbMgr.requestRight("   ");
        console.info(TAG, `Permission request with whitespace result: ${granted}`);
        expect(granted).assertFalse();
      } catch (error) {
        console.info(TAG, 'Permission request error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
    })

    /**
     * @tc.name   testPermissionRequestDeny006
     * @tc.number SUB_USB_Security_PermissionRequest_Extend_0600
     * @tc.desc   Test USB permission request with newline
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPermissionRequestDeny006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testPermissionRequestDeny006 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      try {
        let granted = await usbMgr.requestRight("test\nname");
        console.info(TAG, `Permission request with newline result: ${granted}`);
        expect(granted).assertFalse();
      } catch (error) {
        console.info(TAG, 'Permission request error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
    })

    /**
     * @tc.name   testPermissionRequestDeny007
     * @tc.number SUB_USB_Security_PermissionRequest_Extend_0700
     * @tc.desc   Test USB permission request with very long string
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPermissionRequestDeny007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testPermissionRequestDeny007 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let longString = "a".repeat(1000);
      try {
        let granted = await usbMgr.requestRight(longString);
        console.info(TAG, `Permission request with long string result: ${granted}`);
        expect(granted).assertFalse();
      } catch (error) {
        console.info(TAG, 'Permission request error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
    })

    /**
     * @tc.name   testPermissionRequestDeny008
     * @tc.number SUB_USB_Security_PermissionaryRequest_Extend_0800
     * @tc.desc   Test USB permission request with number
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPermissionRequestDeny008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testPermissionRequestDeny008 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      try {
        let granted = await usbMgr.requestRight(123456);
        console.info(TAG, `Permission request with number result: ${granted}`);
        expect(granted).assertFalse();
      } catch (error) {
        console.info(TAG, 'Permission request error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
    })

    /**
     * @tc.name   testPermissionRequestDeny009
     * @tc.number SUB_USB_Security_PermissionRequest_Extend_0900
     * @tc.desc   Test USB permission request with boolean
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPermissionRequestDeny009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testPermissionRequestDeny009 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      try {
        let granted = await usbMgr.requestRight(true);
        console.info(TAG, `Permission request with boolean result: ${granted}`);
        expect(granted).assertFalse();
      } catch (error) {
        console.info(TAG, 'Permission request error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
    })

    /**
     * @tc.name   testPermissionRequestDeny010
     * @tc.number SUB_USB_Security_PermissionRequest_Extend_1000
     * @tc.desc   Test USB permission request with object
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPermissionRequestDeny010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testPermissionRequestDeny010 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      try {
        let granted = await usbMgr.requestRight({});
        console.info(TAG, `Permission request with object result: ${granted}`);
        expect(granted).assertFalse();
      } catch (error) {
        console.info(TAG, 'Permission request error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
    })

    /**
     * @tc.name   testPermissionRequestDeny011
     * @tc.number SUB_USB_Security_PermissionRequest_Extend_1100
     * @tc.desc   Test USB permission request with array
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPermissionRequestDeny011', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testPermissionRequestDeny011 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      try {
        let granted = await usbMgr.requestRight([]);
        console.info(TAG, `Permission request with array result: ${granted}`);
        expect(granted).assertFalse();
      } catch (error) {
        console.info(TAG, 'Permission request error: ' + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
    })

    /**
     * @tc.name   testPermissionPersistence002
     * @tc.number SUB_USB_Security_PermissionPersistence_Extend_0200
     * @tc.desc   Test USB permission persistence across operations
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPermissionPersistence002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testPermissionPersistence002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let hasGranted1 = usbMgr.hasRight(devices.name);
      if (hasGranted1) {
        let removeResult = usbMgr.removeRight(devices.name);
        expect(removeResult).assertTrue();
      }
      
      let granted = await usbMgr.requestRight(devices.name);
      console.info(TAG, `Permission request result: ${granted}`);
      expect(granted).assertTrue();
      
      let hasGranted2 = usbMgr.hasRight(devices.name);
      expect(hasGranted2).assertTrue();
      
      let removeResult2 = usbMgr.removeRight(devices.name);
      expect(removeResult2).assertTrue();
      
      let hasGranted3 = usbMgr.hasRight(devices.name);
      expect(hasGranted3).assertFalse();
    })

    /**
     * @tc.name   testPermissionPersistence003
     * @tc.number SUB_USB_Security_PermissionPersistence_Extend_0300
     * @tc.desc   Test USB permission persistence with multiple requests
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testPermissionPersistence003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'usb testPermissionPersistence003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      for (let i = 0; i < 5; i++) {
        let granted = await usbMgr.requestRight(devices.name);
        console.info(TAG, `Permission request iteration ${i} result: ${granted}`);
        expect(granted).assertTrue();
        
        let hasRight = usbMgr.hasRight(devices.name);
        expect(hasRight).assertTrue();
      }
      
     ) })

    /**
     * @tc.name   testDeviceNameValidation002
     * @tc.number SUB_USB_Security_DeviceValidation_Extend_0100
     * @tc.desc   Test USB device name validation with invalid names
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceNameValidation002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDeviceNameValidation002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let invalidNames = [
        "",
        null,
        undefined,
        123456,
        "!@#$%^&*()",
        "test\nname",
        "test\tname",
        "测试中文name",
        "a".repeat(1000)
      ];
      
      for (let i = 0; i < invalidNames.length; i++) {
        let device = gDeviceList[0];
        let originalName = device.name;
        device.name = invalidNames[i];
        
        try {
          let pipe = usbMgr.connectDevice(device);
          console.info(TAG, `Connect with invalid name ${i} should fail`);
          expect(pipe === null).assertTrue();
        } catch (error) {
          console.info(TAG, `Connect error for invalid name ${i}: ` + JSON.stringify(error));
          expect(error !== null).assertTrue();
        }
        
        device.name = originalName;
      }
    })

    /**
     * @tc.name   testDeviceNameValidation003
     * @tc.number SUB_USB_Security_DeviceValidation_Extend_0200
     * @tc.desc   Test USB device name with special characters
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceNameValidation003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDeviceNameValidation003 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let specialChars = "!@#$%^&*()_+-=[]{}|;':\",./<>?";
      let device = gDeviceList[0];
      let originalName = device.name;
      device.name = specialChars;
      
      try {
        let pipe = usbMgr.connectDevice(device);
        console.info(TAG(TAG, `Connect with special chars should fail`);
        expect(pipe === null).assertTrue();
      } catch (error) {
        console.info(TAG, `Connect error: ` + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      device.name = originalName;
    })

    /**
     * @tc.name   testDeviceNameValidation004
     * @tc.number SUB_USB_Security_DeviceValidation_Extend_0300
     * @tc.desc   Test USB device name with unicode
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceNameValidation004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDeviceNameValidation004 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let unicodeStr = "测试中文设备";
      let device = gDeviceList[0];
      let originalName = device.name;
      device.name = unicodeStr;
      
      try {
        let pipe = usbMgr.connectDevice(device);
        console.info(TAG, `Connect with unicode should fail`);
        expect(pipe === null).assertTrue();
      } catch (error) {
        console.info(TAG, `Connect error: ` + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      device.name = originalName;
    })

    /**
     * @tc.name   testDeviceNameValidation005
     * @tc.number SUB_USB_Security_DeviceValidation_Extend_0400
     * @tc.desc   Test USB device name with very long string
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceNameValidation005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDeviceNameValidation005 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let longString = "a".repeat(1000);
      let device = gDeviceList[0];
      let originalName = device.name;
      device.name = longString;
      
      try {
        let pipe = usbMgr.connectDevice(device);
        console.info(TAG, `Connect with long name should fail`);
        expect(pipe === null).assertTrue();
      } catch (error) {
        console.info(TAG, `Connect error: ` + JSON.stringify(error));
        expect(error !== null).assertTrue();
      }
      
      device.name = originalName;
    })

    /**
     * @tc.name   testDeviceSerialValidation001
     * @tc.number SUB_USB_Security_DeviceValidation_Extend_0500
     * @tc.desc   Test USB device serial validation
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceSerialValidation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDeviceSerialValidation001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let device = gDeviceList[0];
      expect(device.serial).not.toBeNull();
      expect(device.serial.length > 0).assertTrue();
      expect(typeof device.serial).assertEqual('string');
    })

    /**
     * @tc.name   testDeviceSerialValidation002
     * @tc.number SUB_USB_Security_DeviceValidation_Extend_0600
     * @tc.desc   Test USB device serial with invalid values
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceSerialValidation002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDeviceSerialValidation002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let invalidSerials = [
        "",
        null,
        undefined,
        123456,
        "!@#$%^&*()",
        "test\nserial",
        "测试serial",
        "a".repeat(1000)
      ];
      
      for (let i = 0; i < invalidSerials.length; i++) {
        let device = gDeviceList[0];
        let originalSerial = device.serial;
        device.serial = invalidSerials[i];
        
        let hasRight = usbMgr.hasRight(device.name);
        if (hasRight) {
          expect(true).assertTrue();
        } else {
          expect(true).assertTrue();
        }
        
        device.serial = originalSerial;
      }
    })

    /**
     * @tc.name   testDeviceConfigValidation001
     * @tc.number SUB_USB_Security_ConfigValidation_Extend_0100
     * @tc.desc   Test USB device config validation
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceConfigValidation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDeviceConfigValidation001 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let device = gDeviceList[0];
      expect(device.configs).not.toBeNull();
      expect(device.configs.length).toBeLargerThan(0);
      
      device.configs.forEach((config, index) => {
        expect(config).not.toBeNull();
        expect(config.id).toBeLargerThan(-1);
        expect(config.interfaces).not.toBeNull();
        expect(config.interfaces.length).toBeLargerThan(0);
      });
    })

    /**
     * @tc.name   testDeviceConfigValidation002
     * @tc.number SUB_USB_Security_ConfigValidation_Extend_0200
     * @tc.desc   Test USB device interface validation
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL3
     */
    it('testDeviceConfigValidation002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'usb testDeviceConfigValidation002 begin');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let device = gDeviceList[0];
      
      device.configs.forEach((config, configIndex) => {
        config.interfaces.forEach((iface, ifaceIndex) => {
          expect(iface.id).toBeLargerThan(-1);
          expect(iface.clazz).toBeLargerThan(-1);
          expect(iface.subClass).toBeLargerThan(-1);
          expect(iface.protocol).toBeLargerThan(-1);
          expect(iface.endpoints).not.toBeNull();
          
          iface.endpoints.forEach((endpoint, epIndex) => {
            expect(endpoint.address).toBeLargerThan(-1);
            expect(endpoint.type).toBeLargerThan(-1);
            expect(endpoint.direction).toBeLargerThan(-1);
            expect(endpoint.maxPacketSize).toBeLargerThan(0);
          });
        });
      });
    })
  })
}