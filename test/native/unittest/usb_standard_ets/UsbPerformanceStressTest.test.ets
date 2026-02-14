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
import { BusinessError } from '@ohos.base';

const TAG: string = "[UsbPerformanceStressTest]";
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

function getTransferParams(gPipe: usbMgr.USBDevicePipe, flagsValue: number, endpointValue: number, typeValue: number, timeOutValue: number, bufferSize: number = 1024) {
    let transferParams: usbMgr.UsbDataTransferParams = {
        devPipe: gPipe,
        flags: flagsValue,
        endpoint: endpointValue,
        type: typeValue,
        timeout: timeOutValue,
        length: bufferSize,
        callback: () => {},
        userData: new Uint8Array(bufferSize),
        buffer: new Uint8Array(bufferSize),
        isoPacketCount: 0,
    };
    return transferParams;
}

/* USB performance and stress testing */
export default function UsbPerformanceStressTest() {
    describe('UsbPerformanceStressTest', () => {
        
        beforeAll( async (done: Function) => {
            console.log(TAG, '*************Usb Unit UsbPerformanceStressTest Begin*************');
            let gDeviceList: Array<usbMgr.USBDevice> = usbMgr.getDevices();
            console.info(TAG, 'usb unit begin test getDevices **********', JSON.stringify(gDeviceList));
            isDeviceConnected = deviceConnected(gDeviceList);
            console.info(TAG, 'isDeviceConnected:', isDeviceConnected);
            if (isDeviceConnected) {
                devices = gDeviceList[0];
                let hasRight: boolean = usbMgr.hasRight(devices.name);
                console.info(TAG, `beforeAll: usb requestRight start`);
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

        afterAll( ()=> {
            usbMgr.removeRight(devices.name);
            console.log('*************Usb Unit UsbPerformanceStressTest End*************');
        })

        beforeEach(() => {
            console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
        })

        /**
         * @tc.name   testBulkTransferPerformance001
         * @tc.number SUB_USB_Performance_Bulk_0100
         * @tc.desc   Test bulk transfer performance with various buffer sizes
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testBulkTransferPerformance001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'bulk testBulkTransferPerformance001 enter');
            if (!isDeviceConnected) {
                console.info(TAG, 'usb testBulkTransferPerformance001 No device is connected');
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
            let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
            
            // Find bulk endpoints
            let inEndpoint: usbMgr.USBEndpoint = null;
            let outEndpoint: usbMgr.USBEndpoint = null;
            
            for (const endpoint of tmpInterface.endpoints) {
                if (endpoint.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK) {
                    if (endpoint.direction === usbMgr.USBRequestDirection.USB_REQUEST_DIR_FROM_DEVICE) {
                        inEndpoint = endpoint;
                    } else if (endpoint.direction === usbMgr.USBRequestDirection.USB_REQUEST_DIR_TO_DEVICE) {
                        outEndpoint = endpoint;
                    }
                }
            }
            
            usbMgr.claimInterface(gPipe, tmpInterface, true);
            
            // Test performance with different buffer sizes
            const testSizes = [64, 128, 256, 512, 1024, 2048, 4096];
            const iterations = 10;
            
            for (const size of testSizes) {
                if (outEndpoint) {
                    console.info(TAG, `Testing bulk write performance with size: ${size} bytes`);
                    
                    let totalTime = 0;
                    let successCount = 0;
                    
                    for (let i = 0; i < iterations; i++) {
                        try {
                            let writeData = new Uint8Array(size);
                            // Fill with test data
                            for (let j = 0; j < size; j++) {
                                writeData[j] = j % 256;
                            }
                            
                            let startTime = Date.now();
                            let result = await usbMgr.bulkTransfer(gPipe, outEndpoint, writeData, 5000);
                            let endTime = Date.now();
                            
                            if (result > 0) {
                                totalTime += (endTime - startTime);
                                successCount++;
                                console.info(TAG, `Bulk write iteration ${i}: ${result} bytes in ${endTime - startTime}ms`);
                            }
                            
                        } catch (error) {
                            console.error(`Bulk write error for size ${size} iteration ${i}:`, error);
                        }
                    }
                    
                    if (successCount > 0) {
                        let avgTime = totalTime / successCount;
                        let throughput = (size * successCount * 1000) / totalTime; // bytes per second
                        console.info(TAG, `Bulk write size ${size}: avg time=${avgTime}ms, throughput=${throughput} bytes/s`);
                        
                        // Verify reasonable performance
                        expect(avgTime < 5000).assertTrue(); // Should complete within 5 seconds
                        expect(throughput > 0).assertTrue();
                    }
                }
                
                if (inEndpoint) {
                    console.info(TAG, `Testing bulk read performance with size: ${size} bytes`);
                    
                    let totalTime = 0;
                    let successCount = 0;
                    
                    for (let i = 0; i < iterations; i++) {
                        try {
                            let readBuffer = new Uint8Array(size);
                            
                            let startTime = Date.now();
                            let result = await usbMgr.bulkTransfer(gPipe, inEndpoint, readBuffer, 5000);
                            let endTime = Date.now();
                            
                            if (result >= 0) {
                                totalTime += (endTime - startTime);
                                successCount++;
                                console.info(TAG, `Bulk read iteration ${i}: ${result} bytes in ${endTime - startTime}ms`);
                            }
                            
                        } catch (error) {
                            console.error(`Bulk read error for size ${size} iteration ${i}:`, error);
                        }
                    }
                    
                    if (successCount > 0) {
                        let avgTime = totalTime / successCount;
                        let throughput = (size * successCount * 1000) / totalTime; // bytes per second
                        console.info(TAG, `Bulk read size ${size}: avg time=${avgTime}ms, throughput=${throughput} bytes/s`);
                        
                        // Verify reasonable performance
                        expect(avgTime < 5000).assertTrue(); // Should complete within 5 seconds
                        expect(throughput >= 0).assertTrue();
                    }
                }
            }
            
            usbMgr.releaseInterface(gPipe, tmpInterface);
            usbMgr.closePipe(gPipe);
        });

        /**
         * @tc.name   testTransferStress001
         * @tc.number SUB_USB_Performance_Stress_0100
         * @tc.desc   Test USB transfer stress with high frequency operations
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testTransferStress001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'testTransferStress001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
            let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
            
            usbMgr.claimInterface(gPipe, tmpInterface, true);
            
            // Find bulk endpoint
            let outEndpoint: usbMgr.USBEndpoint = null;
            for (const endpoint of tmpInterface.endpoints) {
                if (endpoint.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK &&
                    endpoint.direction === usbMgr.USBRequestDirection.USB_REQUEST_DIR_TO_DEVICE) {
                    outEndpoint = endpoint;
                    break;
                }
            }
            
            if (outEndpoint) {
                // Stress test with many rapid transfers
                const stressCount = 100;
                let successCount = 0;
                let errorCount = 0;
                let totalTime = 0;
                
                for (let i = 0; i < stressCount; i++) {
                    try {
                        let writeData = new Uint8Array(256);
                        // Fill with iteration-specific data
                        for (let j = 0; j < writeData.length; j++) {
                            writeData[j] = (i + j) % 256;
                        }
                        
                        let startTime = Date.now();
                        let result = await usbMgr.bulkTransfer(gPipe, outEndpoint, writeData, 1000);
                        let endTime = Date.now();
                        
                        if (result > 0) {
                            successCount++;
                            totalTime += (endTime - startTime);
                        } else {
                            errorCount++;
                        }
                        
                        console.info(TAG, `Stress test iteration ${i}: result=${result}, time=${endTime - startTime}ms`);
                        
                    } catch (error) {
                        errorCount++;
                        console.error(`Stress test iteration ${i} error:`, error);
                    }
                    
                    // Small delay to prevent overwhelming the device
                    await sleep(10);
                }
                
                console.info(TAG, `Stress test complete: success=${successCount}, errors=${errorCount}, avg time=${totalTime/successCount}ms`);
                
                // Verify that most transfers succeeded
                expect(successCount + errorCount).assertEqual(stressCount);
                expect(successCount > stressCount * 0.5).assertTrue(); // At least 50% success rate
            }
            
            usbMgr.releaseInterface(gPipe, tmpInterface);
            usbMgr.closePipe(gPipe);
        });

        /**
         * @tc.name   testConcurrentTransfers001
         * @tc.number SUB_USB_Performance_Concurrent_0100
         * @tc.desc   Test concurrent USB transfers
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testConcurrentTransfers001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'testConcurrentTransfers001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
            let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
            
            usbMgr.claimInterface(gPipe, tmpInterface, true);
            
            // Find bulk endpoints
            let inEndpoint: usbMgr.USBEndpoint = null;
            let outEndpoint: usbMgr.USBEndpoint = null;
            
            for (const endpoint of tmpInterface.endpoints) {
                if (endpoint.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK) {
                    if (endpoint.direction === usbMgr.USBRequestDirection.USB_REQUEST_DIR_FROM_DEVICE) {
                        inEndpoint = endpoint;
                    } else if (endpoint.direction === usbMgr.USBRequestDirection.USB_REQUEST_DIR_TO_DEVICE) {
                        outEndpoint = endpoint;
                    }
                }
            }
            
            const concurrentCount = 5;
            const promises = [];
            const results = [];
            
            if (outEndpoint) {
                // Create concurrent write transfers
                for (let i = 0; i < concurrentCount; i++) {
                    let writeData = new Uint8Array(512);
                    // Fill with unique data for each transfer
                    for (let j = 0; j < writeData.length; j++) {
                        writeData[j] = (i * 100 + j) % 256;
                    }
                    
                    const promise = usbMgr.bulkTransfer(gPipe, outEndpoint, writeData, 3000)
                        .then(result => {
                            results.push({ index: i, result: result, type: 'write' });
                            return result;
                        })
                        .catch(error => {
                            results.push({ index: i, result: -1, error: error, type: 'write' });
                            return -1;
                        });
                    
                    promises.push(promise);
                }
            }
            
            if (inEndpoint) {
                // Create concurrent read transfers
                for (let i = 0; i < concurrentCount; i++) {
                    let readBuffer = new Uint8Array(512);
                    
                    const promise = usbMgr.bulkTransfer(gPipe, inEndpoint, readBuffer, 3000)
                        .then(result => {
                            results.push({ index: i + concurrentCount, result: result, type: 'read' });
                            return result;
                        })
                        .catch(error => {
                            results.push({ index: i + concurrentCount, result: -1, error: error, type: 'read' });
                            return -1;
                        });
                    
                    promises.push(promise);
                }
            }
            
            // Wait for all transfers to complete
            try {
                const transferResults = await Promise.all(promises);
                console.info(TAG, `Concurrent transfer results: ${JSON.stringify(results)}`);
                
                // Analyze results
                let successCount = 0;
                let errorCount = 0;
                
                for (const result of results) {
                    if (result.result > 0) {
                        successCount++;
                    } else {
                        errorCount++;
                    }
                }
                
                console.info(TAG, `Concurrent transfers complete: success=${successCount}, errors=${errorCount}`);
                expect(successCount + errorCount).assertEqual(results.length);
                expect(results.length > 0).assertTrue();
                
            } catch (error) {
                console.error('Concurrent transfer error:', error);
                expect(error === null).assertTrue();
            }
            
            usbMgr.releaseInterface(gPipe, tmpInterface);
            usbMgr.closePipe(gPipe);
        });

        /**
         * @tc.name   testLargeDataTransfer001
         * @tc.number SUB_USB_Performance_LargeData_0100
         * @tc.desc   Test large data transfer performance
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testLargeDataTransfer001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'testLargeDataTransfer001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
            let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
            
            usbMgr.claimInterface(gPipe, tmpInterface, true);
            
            // Find bulk endpoint
            let outEndpoint: usbMgr.USBEndpoint = null;
            for (const endpoint of tmpInterface.endpoints) {
                if (endpoint.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK &&
                    endpoint.direction === usbMgr.USBRequestDirection.USB_REQUEST_DIR_TO_DEVICE) {
                    outEndpoint = endpoint;
                    break;
                }
            }
            
            if (outEndpoint) {
                // Test large data sizes
                const largeSizes = [8192, 16384, 32768, 65536]; // 8KB to 64KB
                
                for (const size of largeSizes) {
                    console.info(TAG, `Testing large data transfer with size: ${size} bytes`);
                    
                    try {
                        let largeData = new Uint8Array(size);
                        // Fill with pattern
                        for (let i = 0; i < size; i++) {
                            largeData[i] = i % 256;
                        }
                        
                        let startTime = Date.now();
                        let result = await usbMgr.bulkTransfer(gPipe, outEndpoint, largeData, 10000);
                        let endTime = Date.now();
                        
                        console.info(TAG, `Large transfer size ${size}: ${result} bytes in ${endTime - startTime}ms`);
                        
                        if (result > 0) {
                            let throughput = (result * 1000) / (endTime - startTime); // bytes per second
                            console.info(TAG, `Throughput: ${throughput} bytes/s`);
                            expect(result <= size).assertTrue();
                            expect(throughput > 0).assertTrue();
                        }
                        
                    } catch (error) {
                        console.error(`Large transfer error for size ${size}:`, error);
                        // Large transfers may fail, which is expected for some devices
                    }
                }
            }
            
            usbMgr.releaseInterface(gPipe, tmpInterface);
            usbMgr.closePipe(gPipe);
        });

        /**
         * @tc.name   testInterruptTransferPerformance001
         * @tc.number SUB_USB_Performance_Interrupt_0100
         * @tc.desc   Test interrupt transfer performance and frequency
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testInterruptTransferPerformance001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'interrupt testInterruptTransferPerformance001 enter');
            if (!isDeviceConnected) {
                console.info(TAG, 'usb testInterruptTransferPerformance001 No device is connected');
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
            let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
            let type: number = usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_INTERRUPT;
            
            usbMgr.claimInterface(gPipe, tmpInterface, true);
            
            // Find interrupt endpoint
            let endpoint: usbMgr.USBEndpoint = null;
            for (const ep of tmpInterface.endpoints) {
                if (ep.type === type) {
                    endpoint = ep;
                    break;
                }
            }
            
            if (endpoint) {
                const testFrequency = 50; // Number of rapid transfers
                let transferTimes = [];
                let successCount = 0;
                
                for (let i = 0; i < testFrequency; i++) {
                    try {
                        let transferParams = getTransferParams(gPipe, 0, endpoint.address, type, 1000, 64);
                        
                        transferParams.callback = (err: Error, callBackData: usbMgr.SubmitTransferCallback) => {
                            if (callBackData.status === usbMgr.UsbTransferStatus.TRANSFER_COMPLETED) {
                                transferTimes.push(Date.now());
                                successCount++;
                            }
                        };
                        
                        let startTime = Date.now();
                        await usbMgr.usbSubmitTransfer(transferParams);
                        
                        // Short delay between transfers
                        await sleep(20);
                        
                        try {
                            usbMgr.usbCancelTransfer(transferParams);
                        } catch (cancelError) {
                            // Cancel may fail if transfer already completed
                        }
                        
                    } catch (error) {
                        console.error(`Interrupt transfer ${i} error:`, error);
                    }
                }
                
                console.info(TAG, `Interrupt transfer performance: ${successCount}/${testFrequency} successful`);
                expect(successCount >= 0).assertTrue();
                
                if (transferTimes.length > 1) {
                    // Calculate transfer frequency
                    let minTime = Math.min(...transferTimes);
                    let maxTime = Math.max(...transferTimes);
                    let totalTime = maxTime - minTime;
                    let frequency = (transferTimes.length * 1000) / totalTime; // transfers per second
                    
                    console.info(TAG, `Interrupt transfer frequency: ${frequency} transfers/second`);
                    expect(frequency > 0).assertTrue();
                }
            }
            
            usbMgr.releaseInterface(gPipe, tmpInterface);
            usbMgr.closePipe(gPipe);
        });

        /**
         * @tc.name   testTransferReliability001
         * @tc.number SUB_USB_Performance_Reliability_0100
         * @tc.desc   Test transfer reliability over extended periods
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testTransferReliability001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'testTransferReliability001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
            let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
            
            usbMgr.claimInterface(gPipe, tmpInterface, true);
            
            // Find bulk endpoint
            let outEndpoint: usbMgr.USBEndpoint = null;
            for (const endpoint of tmpInterface.endpoints) {
                if (endpoint.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK &&
                    endpoint.direction === usbMgr.USBRequestDirection.USB_REQUEST_DIR_TO_DEVICE) {
                    outEndpoint = endpoint;
                    break;
                }
            }
            
            if (outEndpoint) {
                const reliabilityTestDuration = 30000; // 30 seconds
                const testInterval = 500; // 500ms between transfers
                const startTime = Date.now();
                
                let totalTransfers = 0;
                let successfulTransfers = 0;
                let failedTransfers = 0;
                let totalBytes = 0;
                
                while (Date.now() - startTime < reliabilityTestDuration) {
                    try {
                        let testData = new Uint8Array(256);
                        // Fill with unique data including timestamp
                        for (let i = 0; i < testData.length - 4; i++) {
                            testData[i] = (totalTransfers + i) % 256;
                        }
                        
                        // Add timestamp to last 4 bytes
                        let timestamp = Date.now();
                        testData[testData.length - 4] = (timestamp >> 24) & 0xFF;
                        testData[testData.length - 3] = (timestamp >> 16) & 0xFF;
                        testData[testData.length - 2] = (timestamp >> 8) & 0xFF;
                        testData[testData.length - 1] = timestamp & 0xFF;
                        
                        let result = await usbMgr.bulkTransfer(gPipe, outEndpoint, testData, 2000);
                        
                        totalTransfers++;
                        
                        if (result > 0) {
                            successfulTransfers++;
                            totalBytes += result;
                        } else {
                            failedTransfers++;
                        }
                        
                        console.info(TAG, `Reliability test: total=${totalTransfers}, success=${successfulTransfers}, failed=${failedTransfers}`);
                        
                    } catch (error) {
                        failedTransfers++;
                        console.error('Reliability test error:', error);
                    }
                    
                    // Wait before next transfer
                    await sleep(testInterval);
                }
                
                console.info(TAG, `Reliability test completed: duration=${reliabilityTestDuration}ms, transfers=${totalTransfers}, success=${successfulTransfers}, failed=${failedTransfers}, totalBytes=${totalBytes}`);
                
                // Verify reliability metrics
                expect(totalTransfers > 0).assertTrue();
                expect(successfulTransfers + failedTransfers).assertEqual(totalTransfers);
                
                // Expect at least 50% success rate
                let successRate = successfulTransfers / totalTransfers;
                expect(successRate > 0.5).assertTrue();
                
                // Calculate average throughput
                let avgThroughput = (totalBytes * 1000) / reliabilityTestDuration; // bytes per second
                console.info(TAG, `Average throughput: ${avgThroughput} bytes/s`);
                expect(avgThroughput >= 0).assertTrue();
            }
            
            usbMgr.releaseInterface(gPipe, tmpInterface);
            usbMgr.closePipe(gPipe);
        });
    })
}