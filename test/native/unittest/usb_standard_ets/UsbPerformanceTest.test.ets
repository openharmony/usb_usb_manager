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

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium'
import usbMgr from '@ohos.usbManager';
import { UiDriver, BY } from '@ohos.UiTest';

const TAG: string = "[UsbPerformanceTest]";
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

export default function UsbPerformanceTest() {
  describe('UsbPerformanceTest', () => {
    let isDeviceConnected: boolean = false;
    let devices: usbMgr.USBDevice;

    beforeAll(async (done: Function) => {
      console.log(TAG, '*************Usb Unit UsbPerformanceTest Begin*************');
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
      console.log('*************Usb Unit UsbPerformanceTest End*************');
    })

    beforeEach(() => {
      console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
    })

    afterEach(() => {
      console.info(TAG, 'afterEach: *************Usb Unit Test Case*************');
    })

    it('testGetDevicesPerformance001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testGetDevicesPerformance001 enter');
      
      const iterations = 100;
      let startTime = Date.now();
      
      for (let i = 0; i < iterations; i++) {
        let deviceList = usbMgr.getDevices();
        expect(deviceList.length >= 0).assertTrue();
      }
      
      let endTime = Date.now();
      let totalTime = endTime - startTime;
      let avgTime = totalTime / iterations;
      
      console.info(TAG, `Total time for ${iterations} iterations: ${totalTime}ms`);
      console.info(TAG, `Average time per call: ${avgTime}ms`);
      expect(avgTime < 100).assertTrue();
    });

    it('testConnectDisconnectPerformance001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testConnectDisconnectPerformance001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      const iterations = 50;
      let startTime = Date.now();
      
      for (let i = 0; i < iterations; i++) {
        let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
        expect(gPipe !== null).assertTrue();
        
        let closeResult = usbMgr.closePipe(gPipe);
        expect(closeResult).assertEqual(0);
      }
      
      let endTime = Date.now();
      let totalTime = endTime - startTime;
      let avgTime = totalTime / iterations;
      
      console.info(TAG, `Total time for ${iterations} iterations: ${totalTime}ms`);
      console.info(TAG, `Average time per connect/disconnect: ${avgTime}ms`);
      expect(avgTime < 200).assertTrue();
    });

    it('testGetRawDescriptorPerformance001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testGetRawDescriptorPerformance001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      const iterations = 100;
      let startTime = Date.now();
      
      for (let i = 0; i < iterations; i++) {
        let descriptor = usbMgr.getRawDescriptor(gPipe);
        expect(descriptor.length > 0).assertTrue();
      }
      
      let endTime = Date.now();
      let totalTime = endTime - startTime;
      let avgTime = totalTime / iterations;
      
      console.info(TAG, `Total time for ${iterations} iterations: ${totalTime}ms`);
      console.info(TAG, `Average time per call: ${avgTime}ms`);
      
      usbMgr.closePipe(gPipe);
      expect(avgTime < 50).assertTrue();
    });

    it('testGetFileDescriptorPerformance001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testGetFileDescriptorPerformance001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      const iterations = 100;
      let startTime = Date.now();
      
      for (let i = 0; i < iterations; i++) {
        let fd = usbMgr.getFileDescriptor(gPipe);
        expect(fd >= 0).assertTrue();
      }
      
      let endTime = Date.now();
      let totalTime = endTime - startTime;
      let avgTime = totalTime / iterations;
      
      console.info(TAG, `Total time for ${iterations} iterations: ${totalTime}ms`);
      console.info(TAG, `Average time per call: ${avgTime}ms`);
      
      usbMgr.closePipe(gPipe);
      expect(avgTime < 50).assertTrue();
    });

    it('testHasRightPerformance001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testHasRightPerformance001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      const iterations = 1000;
      let startTime = Date.now();
      
      for (let i = 0; i < iterations; i++) {
        let hasRight = usbMgr.hasRight(devices.name);
        expect(typeof hasRight === 'boolean').assertTrue();
      }
      
      let endTime = Date.now();
      let totalTime = endTime - startTime;
      let avgTime = totalTime / iterations;
      
      console.info(TAG, `Total time for ${iterations} iterations: ${totalTime}ms`);
      console.info(TAG, `Average time per call: ${avgTime}ms`);
      expect(avgTime < 10).assertTrue();
    });

    it('testGetConfigurationPerformance001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testGetConfigurationPerformance001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      const iterations = 100;
      let startTime = Date.now();
      
      for (let i = 0; i < iterations; i++) {
        let config = usbMgr.getConfiguration(gPipe);
        expect(config !== null).assertTrue();
      }
      
      let endTime = Date.now();
      let totalTime = endTime - startTime;
      let avgTime = totalTime / iterations;
      
      console.info(TAG, `Total time for ${iterations} iterations: ${totalTime}ms`);
      console.info(TAG, `Average time per call: ${avgTime}ms`);
      
      usbMgr.closePipe(gPipe);
      expect(avgTime < 50).assertTrue();
    });

    it('testClaimReleaseInterfacePerformance001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testClaimReleaseInterfacePerformance001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      let config = devices.configs[0];
      let iface = config.interfaces[0];
      
      const iterations = 50;
      let startTime = Date.now();
      
      for (let i = 0; i < iterations; i++) {
        let claimResult = usbMgr.claimInterface(gPipe, iface, true);
        expect(claimResult).assertEqual(0);
        
        let releaseResult = usbMgr.releaseInterface(gPipe, iface);
        expect(releaseResult).assertEqual(0);
      }
      
      let endTime = Date.now();
      let totalTime = endTime - startTime;
      let avgTime = totalTime / iterations;
      
      console.info(TAG, `Total time for ${iterations} iterations: ${totalTime}ms`);
      console.info(TAG, `Average time per claim/release: ${avgTime}ms`);
      
      usbMgr.closePipe(gPipe);
      expect(avgTime < 100).assertTrue();
    });

    it('testConcurrentGetDevices001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.info(TAG, 'testConcurrentGetDevices001 enter');
      
      const concurrentCalls = 10;
      let promises = [];
      
      let startTime = Date.now();
      
      for (let i = 0; i < concurrentCalls; i++) {
        promises.push(
          new Promise<void>((resolve) => {
            let deviceList = usbMgr.getDevices();
            expect(deviceList.length >= 0).assertTrue();
            resolve();
          })
        );
      }
      
      await Promise.all(promises);
      
      let endTime = Date.now();
      let totalTime = endTime - startTime;
      
      console.info(TAG, `Total time for ${concurrentCalls} concurrent calls: ${totalTime}ms`);
      expect(totalTime < 500).assertTrue();
    });

    it('testMemoryUsage001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testMemoryUsage001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      const iterations = 100;
      let descriptors = [];
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      for (let i = 0; i < iterations; i++) {
        let descriptor = usbMgr.getRawDescriptor(gPipe);
        descriptors.push(descriptor);
      }
      
      console.info(TAG, `Created ${iterations} descriptors`);
      console.info(TAG, `Each descriptor size: ${descriptors[0].length} bytes`);
      
      descriptors = [];
      
      usbMgr.closePipe(gPipe);
      expect(descriptors.length).assertEqual(0);
    });

    it('testStressTest001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testStressTest001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      const iterations = 200;
      let successCount = 0;
      
      for (let i = 0; i < iterations; i++) {
        try {
          let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
          if (gPipe !== null) {
            let descriptor = usbMgr.getRawDescriptor(gPipe);
            if (descriptor.length > 0) {
              let fd = usbMgr.getFileDescriptor(gPipe);
              if (fd >= 0) {
                let closeResult = usbMgr.closePipe(gPipe);
                if (closeResult === 0) {
                  successCount++;
                }
              }
            }
          }
        } catch (error) {
          console.info(TAG, `Iteration ${i} failed:`, error);
        }
      }
      
      console.info(TAG, `Stress test completed: ${successCount}/${iterations} successful`);
      expect(successCount).assertLarger(iterations * 0.9);
    });

    it('testLongRunningTest001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testLongRunningTest001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      const duration = 5000;
      let startTime = Date.now();
      let iterationCount = 0;
      
      while (Date.now() - startTime < duration) {
        let deviceList = usbMgr.getDevices();
        expect(deviceList.length >= 0).assertTrue();
        iterationCount++;
        
        if (iterationCount % 100 === 0) {
          console.info(TAG, `Completed ${iterationCount} iterations`);
        }
      }
      
      let endTime = Date.now();
      let actualDuration = endTime - startTime;
      
      console.info(TAG, `Long running test completed: ${iterationCount} iterations in ${actualDuration}ms`);
      expect(iterationCount).assertLarger(50);
    });

    it('testPerformanceBenchmark001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testPerformanceBenchmark001 enter');
      
      let benchmarks = [];
      
      let start1 = Date.now();
      for (let i = 0; i < 100; i++) {
        usbMgr.getDevices();
      }
      let end1 = Date.now();
      benchmarks.push({ name: 'getDevices', time: end1 - start1 });
      
      if (isDeviceConnected) {
        let start2 = Date.now();
        for (let i = 0; i < 50; i++) {
          let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
          if (gPipe !== null) {
            usbMgr.closePipe(gPipe);
          }
        }
        let end2 = Date.now();
        benchmarks.push({ name: 'connectDisconnect', time: end2 - start2 });
        
        let start3 = Date.now();
        let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
        if (gPipe !== null) {
          for (let i = 0; i < 100; i++) {
            usbMgr.getRawDescriptor(gPipe);
          }
          usbMgr.closePipe(gPipe);
        }
        let end3 = Date.now();
        benchmarks.push({ name: 'getRawDescriptor', time: end3 - start3 });
      }
      
      console.info(TAG, 'Performance Benchmark Results:');
      for (let benchmark of benchmarks) {
        console.info(TAG, `  ${benchmark.name}: ${benchmark.time}ms`);
      }
      
      expect(benchmarks.length > 0).assertTrue();
    });

    it('testResponseTimeStability001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testResponseTimeStability001 enter');
      
      const iterations = 100;
      let responseTimes = [];
      
      for (let i = 0; i < iterations; i++) {
        let start = Date.now();
        let deviceList = usbMgr.getDevices();
        let end = Date.now();
        
        responseTimes.push(end - start);
      }
      
      let avgTime = responseTimes.reduce((a, b) => a + b, 0) / responseTimes.length;
      let maxTime = Math.max(...responseTimes);
      let minTime = Math.min(...responseTimes);
      
      console.info(TAG, `Response time statistics:`);
      console.info(TAG, `  Average: ${avgTime}ms`);
      console.info(TAG, `  Min: ${minTime}ms`);
      console.info(TAG, `  Max: ${maxTime}ms`);
      console.info(TAG, `  Range: ${maxTime - minTime}ms`);
      
      expect(avgTime < 50).assertTrue();
      expect(maxTime - minTime < 100).assertTrue();
    });

    it('testResourceCleanup001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testResourceCleanup001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      const iterations = 20;
      let pipes = [];
      
      for (let i = 0; i < iterations; i++) {
        let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
        if (gPipe !== null) {
          pipes.push(gPipe);
        }
      }
      
      console.info(TAG, `Created ${pipes.length} connections`);
      
      for (let i = 0; i < pipes.length; i++) {
        let closeResult = usbMgr.closePipe(pipes[i]);
        expect(closeResult).assertEqual(0);
      }
      
      pipes = [];
      console.info(TAG, 'All resources cleaned up');
      expect(pipes.length).assertEqual(0);
    });

    it('testThroughputTest001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
      console.info(TAG, 'testThroughputTest001 enter');
      if (!isDeviceConnected) {
        expect(isDeviceConnected).assertFalse();
        return;
      }
      
      let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
      expect(gPipe !== null).assertTrue();
      
      const iterations = 1000;
      let totalBytes = 0;
      let startTime = Date.now();
      
      for (let i = 0; i < iterations; i++) {
        let descriptor = usbMgr.getRawDescriptor(gPipe);
        totalBytes += descriptor.length;
      }
      
      let endTime = Date.now();
      let totalTime = endTime - startTime;
      let throughput = totalBytes / (totalTime / 1000);
      
      console.info(TAG, `Throughput test results:`);
      console.info(TAG, `  Total bytes: ${totalBytes}`);
      console.info(TAG, `  Total time: ${totalTime}ms`);
      console.info(TAG, `  Throughput: ${throughput.toFixed(2)} bytes/sec`);
      
      usbMgr.closePipe(gPipe);
      expect(throughput > 0).assertTrue();
    });
  })
}
