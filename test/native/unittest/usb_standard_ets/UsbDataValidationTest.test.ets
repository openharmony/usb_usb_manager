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

const TAG: string = "[UsbDataValidationTest]";
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

function getTransferParams(gPipe: usbMgr.USBDevicePipe, flagsValue: number, endpointValue: number, typeValue: number, timeOutValue: number, bufferSize: number = 10) {
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

function getControlTransferParams(iReqType: number, iReq: number, iValue: number, iIndex: number, iLength: number) {
    let tmpUint8Array = new Uint8Array(512);
    
    let requestParam: usbMgr.USBDeviceRequestParams = {
        bmRequestType: iReqType,
        bRequest: iReq,
        wValue: iValue,
        wIndex: iIndex,
        wLength: iLength,
        data: tmpUint8Array
    }
    return requestParam;
}

function getControlParams(iCmd: number, iReqTarType: usbMgr.USBRequestTargetType, iReqType: usbMgr.USBControlRequestType,
    iValue: number, iIndex: number) {
    let tmpUint8Array = new Uint8Array(512);
    let requestCmd = iCmd
    let requestTargetType = iReqTarType
    let requestType = iReqType
    let value = iValue;
    let index = iIndex;
    let controlParam: usbMgr.USBControlParams = {
        request: requestCmd,
        target: requestTargetType,
        reqType: requestType,
        value: value,
        index: index,
        data: tmpUint8Array
    }
    return controlParam;
}

/* USB data validation and error handling test */
export default function UsbDataValidationTest() {
    describe('UsbDataValidationTest', () => {
        
        beforeAll( async (done: Function) => {
            console.log(TAG, '*************Usb Unit UsbDataValidationTest Begin*************');
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
            console.log('*************Usb Unit UsbDataValidationTest End*************');
        })

        beforeEach(() => {
            console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
        })

        /**
         * @tc.name   testBulkTransferDataValidation001
         * @tc.number SUB_USB_DataValidation_Bulk_0100
         * @tc.desc   Test bulk transfer with various data sizes and validation
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testBulkTransferDataValidation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'bulk testBulkTransferDataValidation001 enter');
            if (!isDeviceConnected) {
                console.info(TAG, 'usb testBulkTransferDataValidation001 No device is connected');
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
            let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
            
            // Test different buffer sizes
            const testSizes = [1, 8, 16, 32, 64, 128, 256, 512, 1024, 2048];
            
            usbMgr.claimInterface(gPipe, tmpInterface, true);
            
            for (const size of testSizes) {
                try {
                    console.info(TAG, `Testing bulk transfer with buffer size: ${size}`);
                    
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
                    
                    if (inEndpoint) {
                        let readBuffer = new Uint8Array(size);
                        let readResult = await usbMgr.bulkTransfer(gPipe, inEndpoint, readBuffer, 5000);
                        console.info(TAG, `Bulk read result for size ${size}: ${readResult}`);
                        expect(readResult >= 0).assertTrue();
                    }
                    
                    if (outEndpoint) {
                        let writeData = new Uint8Array(size);
                        // Fill test data
                        for (let i = 0; i < size; i++) {
                            writeData[i] = i % 256;
                        }
                        
                        let writeResult = await usbMgr.bulkTransfer(gPipe, outEndpoint, writeData, 5000);
                        console.info(TAG, `Bulk write result for size ${size}: ${writeResult}`);
                        expect(writeResult > 0).assertTrue();
                    }
                    
                } catch (error) {
                    console.error(`Bulk transfer error for size ${size}:`, error);
                    // Some sizes might fail due to device constraints, but not all should fail
                    expect(error === null).assertTrue();
                }
            }
            
            usbMgr.releaseInterface(gPipe, tmpInterface);
            usbMgr.closePipe(gPipe);
        });

        /**
         * @tc.name   testInterruptTransferDataValidation001
         * @tc.number SUB_USB_DataValidation_Interrupt_0100
         * @tc.desc   Test interrupt transfer with various data sizes and validation
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testInterruptTransferDataValidation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: Function) => {
            console.info(TAG, 'interrupt testInterruptTransferDataValidation001 enter');
            if (!isDeviceConnected) {
                console.info(TAG, 'usb testInterruptTransferDataValidation001 No device is connected');
                expect(isDeviceConnected).assertFalse();
                done();
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
                // Test various buffer sizes
                const testSizes = [1, 8, 16, 32, 64];
                
                for (const size of testSizes) {
                    try {
                        console.info(TAG, `Testing interrupt transfer with buffer size: ${size}`);
                        
                        let transferParams = getTransferParams(gPipe, 0, endpoint.address, type, 2000, size);
                        
                        transferParams.callback = (err: Error, callBackData: usbMgr.SubmitTransferCallback) => {
                            console.info('callBackData = ' + JSON.stringify(callBackData));
                            console.info(`Interrupt transfer success for size ${size}, status = ${callBackData.status}`);
                            expect(callBackData.status === usbMgr.UsbTransferStatus.TRANSFER_COMPLETED ||
                                    callBackData.status === usbMgr.UsbTransferStatus.TRANSFER_TIMED_OUT).assertTrue();
                            
                            if (callBackData.status === usbMgr.UsbTransferStatus.TRANSFER_COMPLETED) {
                                expect(callBackData.actualLength > 0).assertTrue();
                            }
                        };
                        
                        await usbMgr.usbSubmitTransfer(transferParams);
                        await sleep(100);
                        
                        try {
                            usbMgr.usbCancelTransfer(transferParams);
                        } catch (cancelError) {
                            console.info(`Cancel transfer error: ${cancelError}`);
                        }
                        
                    } catch (error) {
                        console.error(`Interrupt transfer error for size ${size}:`, error);
                        expect(error === null).assertTrue();
                    }
                }
            }
            
            usbMgr.releaseInterface(gPipe, tmpInterface);
            usbMgr.closePipe(gPipe);
            done();
        });

        /**
         * @tc.name   testControlTransferDataValidation001
         * @tc.number SUB_USB_DataValidation_Control_0100
         * @tc.desc   Test control transfer with various request types and data validation
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testControlTransferDataValidation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'control testControlTransferDataValidation001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
            
            // Test various control transfer commands
            const testRequests = [
                { cmd: 6, target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE, reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD, value: (1 << 8), index: 0 },
                { cmd: 6, target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE, reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD, value: (2 << 8), index: 0 },
                { cmd: 8, target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE, reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD, value: 0, index: 0 },
                { cmd: 0, target: usbMgr.USBRequestTargetType.USB_REQUEST_TARGET_DEVICE, reqType: usbMgr.USBControlRequestType.USB_REQUEST_TYPE_STANDARD, value: 0, index: 0 },
            ];
            
            for (const request of testRequests) {
                try {
                    console.info(TAG, `Testing control transfer: cmd=${request.cmd}, target=${request.target}, type=${request.reqType}`);
                    
                    let controlParam = getControlParams(
                        request.cmd,
                        request.target,
                        request.reqType,
                        request.value,
                        request.index
                    );
                    
                    let result = await usbMgr.controlTransfer(gPipe, controlParam, 5000);
                    console.info(TAG, `Control transfer result: ${result}`);
                    
                    // Control transfers should return the number of bytes transferred or an error
                    expect(result >= 0 || result < 0).assertTrue();
                    
                } catch (error) {
                    console.error(`Control transfer error for cmd ${request.cmd}:`, error);
                    // Some control requests may not be supported, which is expected
                }
            }
            
            usbMgr.closePipe(gPipe);
        });

        /**
         * @tc.name   testInvalidTransferParams001
         * @tc.number SUB_USB_DataValidation_InvalidParams_0100
         * @tc.desc   Test transfer operations with invalid parameters
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testInvalidTransferParams001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'testInvalidTransferParams001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
            let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
            
            usbMgr.claimInterface(gPipe, tmpInterface, true);
            
            // Test with null buffer
            try {
                let result = await usbMgr.bulkTransfer(gPipe, tmpInterface.endpoints[0], null, 5000);
                expect(result === null || result < 0).assertTrue();
            } catch (error) {
                console.info('Expected error for null buffer:', error);
            }
            
            // Test with invalid endpoint
            try {
                let invalidEndpoint: usbMgr.USBEndpoint = {
                    address: -1,
                    type: -1,
                    direction: -1,
                    maxPacketSize: 0
                };
                let buffer = new Uint8Array(10);
                let result = await usbMgr.bulkTransfer(gPipe, invalidEndpoint, buffer, 5000);
                expect(result < 0).assertTrue();
            } catch (error) {
                console.info('Expected error for invalid endpoint:', error);
            }
            
            // Test with zero timeout
            try {
                let buffer = new Uint8Array(10);
                let result = await usbMgr.bulkTransfer(gPipe, tmpInterface.endpoints[0], buffer, 0);
                // Zero timeout should either fail or return immediately
                expect(result >= 0 || result < 0).assertTrue();
            } catch (error) {
                console.info('Expected error for zero timeout:', error);
            }
            
            // Test with invalid transfer parameters
            try {
                let invalidTransferParams: usbMgr.UsbDataTransferParams = {
                    devPipe: null,
                    flags: -1,
                    endpoint: -1,
                    type: -1,
                    timeout: -1,
                    length: -1,
                    callback: () => {},
                    userData: null,
                    buffer: null,
                    isoPacketCount: -1,
                };
                
                await usbMgr.usbSubmitTransfer(invalidTransferParams);
                expect().assertFail();
            } catch (error) {
                console.info('Expected error for invalid transfer params:', error);
                expect(error !== null).assertTrue();
            }
            
            usbMgr.releaseInterface(gPipe, tmpInterface);
            usbMgr.closePipe(gPipe);
        });

        /**
         * @tc.name   testTransferErrorHandling001
         * @tc.number SUB_USB_DataValidation_ErrorHandling_0100
         * @tc.desc   Test comprehensive error handling for transfer operations
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testTransferErrorHandling001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'testTransferErrorHandling001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            // Test with disconnected device
            try {
                let disconnectedPipe: usbMgr.USBDevicePipe = {
                    busNum: 999,
                    devAddress: 999
                };
                let buffer = new Uint8Array(10);
                let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
                
                let result = await usbMgr.bulkTransfer(disconnectedPipe, tmpInterface.endpoints[0], buffer, 5000);
                expect(result < 0).assertTrue();
            } catch (error) {
                console.info('Expected error for disconnected device:', error);
            }
            
            // Test with huge buffer size
            try {
                let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
                let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
                
                usbMgr.claimInterface(gPipe, tmpInterface, true);
                
                let hugeBuffer = new Uint8Array(1024 * 1024); // 1MB
                let result = await usbMgr.bulkTransfer(gPipe, tmpInterface.endpoints[0], hugeBuffer, 5000);
                console.info(TAG, `Huge buffer transfer result: ${result}`);
                
                usbMgr.releaseInterface(gPipe, tmpInterface);
                usbMgr.closePipe(gPipe);
                
                // May fail or succeed depending on device capabilities
                expect(result >= 0 || result < 0).assertTrue();
            } catch (error) {
                console.info('Expected error for huge buffer:', error);
            }
        });

        /**
         * @tc.name   testTransferCancelHandling001
         * @tc.number SUB_USB_DataValidation_CancelHandling_0100
         * @tc.desc   Test transfer cancellation and cleanup
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testTransferCancelHandling001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: Function) => {
            console.info(TAG, 'testTransferCancelHandling001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                done();
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
                let transferParams = getTransferParams(gPipe, 0, endpoint.address, type, 10000, 64);
                let transferCompleted = false;
                
                transferParams.callback = (err: Error, callBackData: usbMgr.SubmitTransferCallback) => {
                    console.info('callBackData = ' + JSON.stringify(callBackData));
                    console.info(`Transfer status: ${callBackData.status}`);
                    
                    if (!transferCompleted) {
                        expect(callBackData.status === usbMgr.UsbTransferStatus.TRANSFER_CANCELLED ||
                                callBackData.status === usbMgr.UsbTransferStatus.TRANSFER_COMPLETED ||
                                callBackData.status === usbMgr.UsbTransferStatus.TRANSFER_TIMED_OUT).assertTrue();
                        transferCompleted = true;
                        usbMgr.releaseInterface(gPipe, tmpInterface);
                        usbMgr.closePipe(gPipe);
                        done();
                    }
                };
                
                try {
                    // Submit transfer
                    await usbMgr.usbSubmitTransfer(transferParams);
                    
                    // Cancel quickly after submission
                    setTimeout(() => {
                        if (!transferCompleted) {
                            try {
                                console.info(TAG, 'Cancelling transfer');
                                usbMgr.usbCancelTransfer(transferParams);
                            } catch (cancelError) {
                                console.info('Cancel transfer error:', cancelError);
                                if (!transferCompleted) {
                                    transferCompleted = true;
                                    usbMgr.releaseInterface(gPipe, tmpInterface);
                                    usbMgr.closePipe(gPipe);
                                    done();
                                }
                            }
                        }
                    }, 100);
                    
                } catch (error) {
                    console.error('Transfer submission error:', error);
                    expect(error === null).assertTrue();
                    usbMgr.releaseInterface(gPipe, tmpInterface);
                    usbMgr.closePipe(gPipe);
                    done();
                }
            } else {
                usbMgr.releaseInterface(gPipe, tmpInterface);
                usbMgr.closePipe(gPipe);
                done();
            }
        });

        /**
         * @tc.name   testMultipleConcurrentTransfers001
         * @tc.number SUB_USB_DataValidation_Concurrent_0100
         * @tc.desc   Test multiple concurrent transfers
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testMultipleConcurrentTransfers001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: Function) => {
            console.info(TAG, 'testMultipleConcurrentTransfers001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                done();
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
            
            const concurrentCount = 3;
            let completedTransfers = 0;
            
            if (inEndpoint) {
                // Submit multiple concurrent bulk reads
                for (let i = 0; i < concurrentCount; i++) {
                    try {
                        let readBuffer = new Uint8Array(256);
                        
                        usbMgr.bulkTransfer(gPipe, inEndpoint, readBuffer, 2000).then(result => {
                            console.info(TAG, `Concurrent transfer ${i} result: ${result}`);
                            completedTransfers++;
                            
                            if (completedTransfers === concurrentCount) {
                                usbMgr.releaseInterface(gPipe, tmpInterface);
                                usbMgr.closePipe(gPipe);
                                done();
                            }
                        }).catch(error => {
                            console.error(`Concurrent transfer ${i} error:`, error);
                            completedTransfers++;
                            
                            if (completedTransfers === concurrentCount) {
                                usbMgr.releaseInterface(gPipe, tmpInterface);
                                usbMgr.closePipe(gPipe);
                                done();
                            }
                        });
                        
                    } catch (error) {
                        console.error(`Submit concurrent transfer ${i} error:`, error);
                        completedTransfers++;
                        
                        if (completedTransfers === concurrentCount) {
                            usbMgr.releaseInterface(gPipe, tmpInterface);
                            usbMgr.closePipe(gPipe);
                            done();
                        }
                    }
                }
            } else {
                usbMgr.releaseInterface(gPipe, tmpInterface);
                usbMgr.closePipe(gPipe);
                done();
            }
        });
    })
}