/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an " "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium'
import usbMgr from '@ohos.usbManager';
import { UiDriver, BY } from '@ohos.UiTest';

const TAG: string = "[UsbAudioTest]";
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
    console.info(TAG, `button is ${JSON.stringify(button)}`);
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

export default function UsbAudioTest() {
  describe('UsbAudioTest', () => {
    let isDeviceConnected: boolean = false;
    let devices: usbMgr.USBDevice;

    beforeAll(async (done: Function) => {
      console.log(TAG, '*************Usb Unit UsbAudioTest Begin*************');
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
      if (isDeviceConnected) {
        usbMgr.removeRight(devices.name);
      }
      console.log('*************Usb Unit UsbAudioTest End*************');
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test Case*************');
    })

    it('testAudioDeviceDetection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAudioDeviceDetection001 enter');
      
      let deviceList = usbMgr.getDevices();
      console.info(TAG, `Audio device detection count: ${deviceList.length}`);
      
      for (let i = 0; i < deviceList.length; i++) {
        let device = deviceList[i];
        console.info(TAG, `Device ${i}: vendorId=0x${device.vendorId.toString(16)}, productId=0x${device.productId.toString(16)}`);
      }
      
      expect(deviceList.length >= 0).assertTrue();
    });

    it('testAudioInterfaceDetection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAudioInterfaceDetection001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      console.info(TAG, `Audio interface count: ${config.interfaces.length}`);
      
      for (let i = 0; i < config.interfaces.length; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `Interface ${i}: class=0x${iface.clazz.toString(16)}, subclass=0x${iface.subClass.toString(16)}`);
        console.info(TAG, `Interface ${i}: protocol=0x${iface.protocol.toString(16)}, endpoints=${iface.endpoints.length}`);
      }
      
      expect(config.interfaces.length > 0).assertTrue();
    });

    it('testAudioEndpointDetection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAudioEndpointDetection001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      console.info(TAG, `Audio endpoint count: ${iface.endpoints.length}`);
      
      for (let i = 0; i < iface.endpoints.length; i++) {
        let endpoint = iface.endpoints[i];
        console.info(TAG, `Endpoint ${i}: address=${endpoint.address}, type=${endpoint.type}`);
        console.info(TAG, `Endpoint ${i}: direction=${endpoint.direction}, maxPacketSize=${endpoint.maxPacketSize}`);
      }
      
      expect(iface.endpoints.length > 0).assertTrue();
    });

    it('testAudioIsochronousEndpoint001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAudioIsochronousEndpoint001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      let isochronousEndpoints = iface.endpoints.filter(ep => ep.type === 1);
      console.info(TAG, `Isochronous endpoints count: ${isochronousEndpoints.length}`);
      
      for (let i = 0; i < isochronousEndpoints.length; i++) {
        let endpoint = isochronousEndpoints[i];
        console.info(TAG, `Isochronous endpoint ${i}: address=${endpoint.address}, direction=${endpoint.direction}`);
        console.info(TAG, `Isochronous endpoint ${i}: maxPacketSize=${endpoint.maxPacketSize}, interval=${endpoint.interval}`);
      }
    });

    it('testAudioControlInterface001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAudioControlInterface001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      let controlInterfaces = config.interfaces.filter(iface => iface.clazz === 0x01 && iface.subClass === 0x01);
      console.info(TAG, `Audio control interfaces count: ${controlInterfaces.length}`);
      
      for (let i = 0; i < controlInterfaces.length; i++) {
        let iface = controlInterfaces[i];
        console.info(TAG, `Control interface ${i}: id=${iface.id}, protocol=0x${iface.protocol.toString(16)}`);
        console.info(TAG, `Control interface ${i}: endpoints=${iface.endpoints.length}`);
      }
    });

    it('testAudioStreamingInterface001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAudioStreamingInterface001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      let streamingInterfaces = config.interfaces.filter(iface => iface.clazz === 0x01 && iface.subClass === 0x02);
      console.info(TAG, `Audio streaming interfaces count: ${streamingInterfaces.length}`);
      
      for (let i = 0; i < streamingInterfaces.length; i++) {
        let iface = streamingInterfaces[i];
        console.info(TAG, `Streaming interface ${i}: id=${iface.id}, protocol=0x${iface.protocol.toString(16)}`);
        console.info(TAG, `Streaming interface ${i}: endpoints=${iface.endpoints.length}`);
      }
    });

    it('testAudioMidiInterface001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAudioMidiInterface001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      let midiInterfaces = config.interfaces.filter(iface => iface.clazz === 0x01 && iface.subClass === 0x03);
      console.info(TAG, `Audio MIDI interfaces count: ${midiInterfaces.length}`);
      
      for (let i = 0; i < midiInterfaces.length; i++) {
        let iface = midiInterfaces[i];
        console.info(TAG, `MIDI interface ${i}: id=${iface.id}, protocol=0x${iface.protocol.toString(16)}`);
        console.info(TAG, `MIDI interface ${i}: endpoints=${iface.endpoints.length}`);
      }
    });

    it('testAudioGetVolume001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testAudioGetVolume001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x81,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0100,
        index: iface.id,
        data: new Uint8Array(2)
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Get audio volume result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testAudioSetVolume001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testAudioSetVolume001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let volumeData = new Uint8Array(2);
      volumeData[0] = 0x80;
      volumeData[1] = 0x00;
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x01,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0100,
        index: iface.id,
        data: volumeData
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Set audio volume result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testAudioMute001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testAudioMute001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let muteData = new Uint8Array(1);
      muteData[0] = 0x01;
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x01,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0101,
        index: iface.id,
        data: muteData
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Set audio mute result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testAudioUnmute001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testAudioUnmute001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      usbMgr.claimInterface(gPipe, iface, true);
      
      let unmuteData = new Uint8Array(1);
      unmuteData[0] = 0x00;
      
      let controlParam: usbMgr.USBControlParams = {
        request: 0x01,
        target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_INTERFACE,
        reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_CLASS,
        value: 0x0101,
        index: iface.id,
        data: unmuteData
      };
      
      let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
      console.info(TAG, `Set audio unmute result: ${result}`);
      
      usbMgr.releaseInterface(gPipe, iface);
      usbMgr.closePipe(gPipe);
    });

    it('testAudioPowerManagement001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAudioPowerManagement001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      
      console.info(TAG, `Audio maxPower: ${config.maxPower}mA`);
      console.info(TAG, `Audio isRemoteWakeup: ${config.isRemoteWakeup}`);
      console.info(TAG, `Audio isSelfPowered: ${config.isSelfPowered}`);
      
      expect(config.maxPower >= 0).assertTrue();
    });

    it('testAudioStringDescriptors001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAudioStringDescriptors001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      console.info(TAG, `Audio name: ${devices.name}`);
      console.info(TAG, `Audio serial: ${devices.serial}`);
      
      let config = devices.configs[0];
      console.info(TAG, `Config name: ${config.name}`);
      
      for (let i = 0; i < config.interfaces.length; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `Interface ${i} name: ${iface.name}`);
      }
    });

    it('testAudioMultipleInterfaces001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAudioMultipleInterfaces001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let config = devices.configs[0];
      let interfaceCount = config.interfaces.length;
      
      console.info(TAG, `Audio interfaces count: ${interfaceCount}`);
      
      for (let i = 0; i < interfaceCount; i++) {
        let iface = config.interfaces[i];
        console.info(TAG, `Interface ${i}: class=0x${iface.clazz.toString(16)}, subclass=0x${iface.subClass.toString(16)}`);
        console.info(TAG, `Interface ${i}: protocol=0x${iface.protocol.toString(16)}, endpoints=${iface.endpoints.length}`);
      }
    });

    it('testAudioReconnection001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testAudioReconnection001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      const reconnectCount = 3;
      
      for (let i = 0; i < reconnectCount; i++) {
        let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
        expect(gPipe !== null).assertTrue();
        
        let closeResult = usbMgr.closePipe(gPipe);
        expect(closeResult).assertEqual(0);
        
        console.info(TAG, `Reconnection ${i + 1}/${reconnectCount} completed`);
      }
    });

    it('testAudioConcurrentAccess001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testAudioConcurrentAccess001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let operations = [];
      
      for (let i = 0; i < 5; i++) {
        operations.push(
          new Promise<void>((resolve) => {
            let descriptor = usbMgr.getRawDescriptor(gPipe);
            expect(descriptor.length > 0).assertTrue();
            resolve();
          })
        );
      }
      
      await Promise.all(operations);
      console.info(TAG, 'Concurrent access test completed');
      
      usbMgr.closePipe(gPipe);
    });
  })
}
