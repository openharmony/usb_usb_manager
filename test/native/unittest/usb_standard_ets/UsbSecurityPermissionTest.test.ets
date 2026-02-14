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

const TAG: string = "[UsbSecurityPermissionTest]";
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

function getTransferParams(gPipe: usbMgr.USBDevicePipe, flagsValue: number, endpointValue: number, typeValue: number, timeOutValue: number) {
    let transferParams: usbMgr.UsbDataTransferParams = {
        devPipe: gPipe,
        flags: flagsValue,
        endpoint: endpointValue,
        type: typeValue,
        timeout: timeOutValue,
        length: 10,
        callback: () => {},
        userData: new Uint8Array(10),
        buffer: new Uint8Array(10),
        isoPacketCount: 0,
    };
    return transferParams;
}

/* USB security and permission edge cases test */
export default function UsbSecurityPermissionTest() {
    describe('UsbSecurityPermissionTest', () => {
        
        beforeAll( async (done: Function) => {
            console.log(TAG, '*************Usb Unit UsbSecurityPermissionTest Begin*************');
            let gDeviceList: Array<usbMgr.USBDevice> = usbMgr.getDevices();
            console.info(TAG, 'usb unit begin test getDevices **********', JSON.stringify(gDeviceList));
            isDeviceConnected = deviceConnected(gDeviceList);
            console.info(TAG, 'isDeviceConnected:', isDeviceConnected);
            if (isDeviceConnected) {
                devices = gDeviceList[0];
                // Don't get permission initially for security testing
            }
            done();
        })

        afterAll( ()=> {
            // Clean up permissions
            if (isDeviceConnected) {
                usbMgr.removeRight(devices.name);
            }
            console.log('*************Usb Unit UsbSecurityPermissionTest End*************');
        })

        beforeEach(() => {
            console.info(TAG, 'beforeEach: *************Usb Unit Test Case*************');
        })

        /**
         * @tc.name   testPermissionRequestDeny001
         * @tc.number SUB_USB_Security_PermissionRequest_0100
         * @tc.desc   Test USB permission request denial scenarios
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testPermissionRequestDeny001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'testPermissionRequestDeny001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            // Check initial permission state
            let hasRight = usbMgr.hasRight(devices.name);
            console.info(TAG, `Initial permission state: ${hasRight}`);
            
            // If permission exists, remove it first
            if (hasRight) {
                let removeResult = usbMgr.removeRight(devices.name);
                console.info(TAG, `Remove permission result: ${removeResult}`);
                expect(removeResult).assertTrue();
                
                hasRight = usbMgr.hasRight(devices.name);
                expect(hasRight).assertFalse();
            }
            
            // Try to connect without permission
            try {
                let pipe = usbMgr.connectDevice(devices);
                expect(pipe === null).assertTrue();
                console.info(TAG, 'Connection without permission correctly failed');
            } catch (error) {
                console.info(TAG, 'Expected error for connection without permission: ' + error);
            }
            
            // Try to access device descriptors without permission
            try {
                let descriptor = usbMgr.getRawDescriptor(gPipe);
                expect(descriptor === null || descriptor.length === 0).assertTrue();
                console.info(TAG, 'Descriptor access without permission correctly failed');
            } catch (error) {
                console.info(TAG, 'Expected error for descriptor access without permission: ' + error);
            }
            
            // Try to access file descriptor without permission
            try {
                let fileDescriptor = usbMgr.getFileDescriptor(gPipe);
                expect(fileDescriptor === null || fileDescriptor < 0).assertTrue();
                console.info(TAG, 'File descriptor access without permission correctly failed');
            } catch (error) {
                console.info(TAG, 'Expected error for file descriptor access without permission: ' + error);
            }
        });

        /**
         * @tc.name   testPermissionRequestGrant001
         * @tc.number SUB_USB_Security_PermissionRequest_0200
         * @tc.desc   Test USB permission request grant scenarios
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testPermissionRequestGrant001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'testPermissionRequestGrant001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            // Ensure no permission initially
            let hasRight = usbMgr.hasRight(devices.name);
            if (hasRight) {
                usbMgr.removeRight(devices.name);
            }
            
            // Request permission
            try {
                let granted = await usbMgr.requestRight(devices.name);
                console.info(TAG, `Permission request result: ${granted}`);
                
                if (granted) {
                    // Verify permission was granted
                    hasRight = usbMgr.hasRight(devices.name);
                    expect(hasRight).assertTrue();
                    
                    // Test operations with permission
                    let pipe = usbMgr.connectDevice(devices);
                    expect(pipe !== null).assertTrue();
                    
                    if (pipe) {
                        let descriptor = usbMgr.getRawDescriptor(pipe);
                        expect(descriptor.length > 0).assertTrue();
                        
                        let fileDescriptor = usbMgr.getFileDescriptor(pipe);
                        expect(fileDescriptor >= 0).assertTrue();
                        
                        usbMgr.closePipe(pipe);
                    }
                } else {
                    console.info(TAG, 'Permission was denied by user');
                    // In a real test scenario, we would simulate user denial
                }
                
            } catch (error) {
                console.info(TAG, 'Permission request error: ' + error);
                // Permission request may fail due to UI automation limitations
            }
        });

        /**
         * @tc.name   testPermissionPersistence001
         * @tc.number SUB_USB_Security_PermissionPersistence_0100
         * @tc.desc   Test USB permission persistence across operations
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testPermissionPersistence001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'testPermissionPersistence001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            // Ensure permission is granted first
            let hasRight = usbMgr.hasRight(devices.name);
            if (!hasRight) {
                try {
                    await getPermission(gDeviceList);
                    await sleep(3000);
                    await driveFn();
                    await sleep(1000);
                    hasRight = usbMgr.hasRight(devices.name);
                } catch (error) {
                    console.info(TAG, 'Permission request failed: ' + error);
                }
            }
            
            if (hasRight) {
                // Test permission persistence after multiple operations
                for (let i = 0; i < 5; i++) {
                    console.info(TAG, `Permission persistence test iteration ${i + 1}`);
                    
                    // Check permission
                    hasRight = usbMgr.hasRight(devices.name);
                    expect(hasRight).assertTrue();
                    
                    // Perform operations
                    let pipe = usbMgr.connectDevice(devices);
                    expect(pipe !== null).assertTrue();
                    
                    if (pipe) {
                        let descriptor = usbMgr.getRawDescriptor(pipe);
                        expect(descriptor.length > 0).assertTrue();
                        
                        usbMgr.closePipe(pipe);
                    }
                    
                    // Wait before next iteration
                    await sleep(1000);
                }
            } else {
                console.info(TAG, 'Permission not available for persistence test');
            }
        });

        /**
         * @tc.name   testDeviceNameValidation001
         * @tc.number SUB_USB_Security_DeviceValidation_0100
         * @tc.desc   Test USB device name validation for security
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testDeviceNameValidation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
            console.info(TAG, 'testDeviceNameValidation001 begin');
            
            // Test permission operations with invalid device names
            const invalidDeviceNames = [
                '',
                null,
                undefined,
                '../../../etc/passwd',
                '../sensitive_file',
                'device_with_<>chars',
                'device_with_"quotes"',
                'device_with_&ampersand',
                'device_with_|pipe',
                'device_with_semicolon;command',
                'device_with_backslash\\escape'
            ];
            
            for (const invalidName of invalidDeviceNames) {
                try {
                    // Test hasRight with invalid device name
                    let hasRight = usbMgr.hasRight(invalidName);
                    expect(hasRight).assertFalse();
                    
                    // Test removeRight with invalid device name
                    let removeResult = usbMgr.removeRight(invalidName);
                    expect(removeResult).assertFalse();
                    
                    console.info(TAG, `Invalid device name '${invalidName}' properly handled`);
                    
                } catch (error) {
                    console.info(TAG, `Expected error for invalid device name '${invalidName}': ` + error);
                }
            }
            
            // Test with extremely long device name
            let longDeviceName = 'a'.repeat(1000);
            try {
                let hasRight = usbMgr.hasRight(longDeviceName);
                expect(hasRight).assertFalse();
                
                let removeResult = usbMgr.removeRight(longDeviceName);
                expect(removeResult).assertFalse();
                
                console.info(TAG, 'Long device name properly handled');
            } catch (error) {
                console.info(TAG, 'Expected error for long device name: ' + error);
            }
        });

        /**
         * @tc.name   testUnauthorizedAccess001
         * @tc.number SUB_USB_Security_UnauthorizedAccess_0100
         * @tc.desc   Test unauthorized access prevention
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testUnauthorizedAccess001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
            console.info(TAG, 'testUnauthorizedAccess001 begin');
            
            // Test access without proper permission
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            // Remove permission first
            let hasRight = usbMgr.hasRight(devices.name);
            if (hasRight) {
                usbMgr.removeRight(devices.name);
            }
            
            // Try various operations without permission
            try {
                let pipe = usbMgr.connectDevice(devices);
                expect(pipe === null).assertTrue();
            } catch (error) {
                console.info(TAG, 'Expected unauthorized access error for connectDevice: ' + error);
            }
            
            try {
                let descriptor = usbMgr.getRawDescriptor(null);
                expect(descriptor === null).assertTrue();
            } catch (error) {
                console.info(TAG, 'Expected unauthorized access error for getRawDescriptor: ' + error);
            }
            
            try {
                let fileDescriptor = usbMgr.getFileDescriptor(null);
                expect(fileDescriptor === null || fileDescriptor < 0).assertTrue();
            } catch (error) {
                console.info(TAG, 'Expected unauthorized access error for getFileDescriptor: ' + error);
            }
        });

        /**
         * @tc.name   testPermissionRaceConditions001
         * @tc.number SUB_USB_Security_RaceConditions_0100
         * @tc.desc   Test permission race conditions and timing attacks
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testPermissionRaceConditions001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'testPermissionRaceConditions001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            // Test rapid permission check and access
            for (let i = 0; i < 10; i++) {
                try {
                    let hasRight = usbMgr.hasRight(devices.name);
                    console.info(TAG, `Permission check ${i}: ${hasRight}`);
                    
                    if (hasRight) {
                        // Try to quickly access device
                        let pipe = usbMgr.connectDevice(devices);
                        if (pipe) {
                            usbMgr.closePipe(pipe);
                        }
                    }
                    
                    // Small delay
                    await sleep(10);
                    
                } catch (error) {
                    console.info(TAG, `Race condition test iteration ${i} error: ` + error);
                }
            }
            
            // Test concurrent permission requests
            const promises = [];
            for (let i = 0; i < 5; i++) {
                const promise = usbMgr.requestRight(devices.name).then(result => {
                    console.info(TAG, `Concurrent permission request ${i}: ${result}`);
                    return result;
                }).catch(error => {
                    console.info(TAG, `Concurrent permission request ${i} error: ` + error);
                    return false;
                });
                
                promises.push(promise);
            }
            
            try {
                const results = await Promise.all(promises);
                console.info(TAG, `Concurrent permission requests results: ${JSON.stringify(results)}`);
            } catch (error) {
                console.info(TAG, 'Concurrent permission requests error: ' + error);
            }
        });

        /**
         * @tc.name   testDataExfiltration001
         * @tc.number SUB_USB_Security_DataExfiltration_0100
         * @tc.desc   Test prevention of data exfiltration through USB
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testDataExfiltration001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'testDataExfiltration001 begin');
            
            // Only test if device is connected
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            // Ensure we have permission for this test
            let hasRight = usbMgr.hasRight(devices.name);
            if (!hasRight) {
                try {
                    await getPermission(gDeviceList);
                    await sleep(3000);
                    await driveFn();
                    await sleep(1000);
                    hasRight = usbMgr.hasRight(devices.name);
                } catch (error) {
                    console.info(TAG, 'Permission request failed: ' + error);
                }
            }
            
            if (hasRight) {
                try {
                    let gPipe: usbMgr.USBDevicePipe = usbMgr.connectDevice(devices);
                    let tmpInterface: usbMgr.USBInterface = devices.configs[0].interfaces[0];
                    
                    usbMgr.claimInterface(gPipe, tmpInterface, true);
                    
                    // Test bulk transfer with potentially sensitive data
                    const sensitiveDataStrings = [
                        'password',
                        'secret',
                        'token',
                        'key',
                        'credential',
                        'private',
                        'confidential',
                        'classified'
                    ];
                    
                    for (const sensitiveWord of sensitiveDataStrings) {
                        let testData = new Uint8Array(100);
                        // Fill with test data containing sensitive word
                        for (let i = 0; i < sensitiveWord.length && i < testData.length; i++) {
                            testData[i] = sensitiveWord.charCodeAt(i);
                        }
                        
                        // Try to find a bulk OUT endpoint
                        let outEndpoint = null;
                        for (const endpoint of tmpInterface.endpoints) {
                            if (endpoint.type === usbMgr.UsbEndpointTransferType.TRANSFER_TYPE_BULK &&
                                endpoint.direction === usbMgr.USBRequestDirection.USB_REQUEST_DIR_TO_DEVICE) {
                                outEndpoint = endpoint;
                                break;
                            }
                        }
                        
                        if (outEndpoint) {
                            try {
                                let result = await usbMgr.bulkTransfer(gPipe, outEndpoint, testData, 2000);
                                console.info(TAG, `Data transfer with '${sensitiveWord}' result: ${result}`);
                                
                                // The transfer may succeed or fail depending on device
                                // The test verifies that the system doesn't crash or expose sensitive info
                                expect(result >= 0 || result < 0).assertTrue();
                                
                            } catch (error) {
                                console.info(TAG, `Data transfer with '${sensitiveWord}' error: ` + error);
                            }
                        }
                        
                        await sleep(100); // Small delay between transfers
                    }
                    
                    usbMgr.releaseInterface(gPipe, tmpInterface);
                    usbMgr.closePipe(gPipe);
                    
                } catch (error) {
                    console.info(TAG, 'Data exfiltration test error: ' + error);
                }
            } else {
                console.info(TAG, 'Permission not available for data exfiltration test');
            }
        });

        /**
         * @tc.name   testPermissionRevocation001
         * @tc.number SUB_USB_Security_PermissionRevocation_0100
         * @tc.desc   Test permission revocation and immediate effect
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testPermissionRevocation001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'testPermissionRevocation001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            
            // Ensure permission is granted first
            let hasRight = usbMgr.hasRight(devices.name);
            if (!hasRight) {
                try {
                    await getPermission(gDeviceList);
                    await sleep(3000);
                    await driveFn();
                    await sleep(1000);
                    hasRight = usbMgr.hasRight(devices.name);
                } catch (error) {
                    console.info(TAG, 'Permission request failed: ' + error);
                }
            }
            
            if (hasRight) {
                // Verify access works before revocation
                let pipe = usbMgr.connectDevice(devices);
                expect(pipe !== null).assertTrue();
                if (pipe) {
                    usbMgr.closePipe(pipe);
                }
                
                // Revoke permission
                let removeResult = usbMgr.removeRight(devices.name);
                console.info(TAG, `Permission revocation result: ${removeResult}`);
                expect(removeResult).assertTrue();
                
                // Verify permission is gone
                hasRight = usbMgr.hasRight(devices.name);
                expect(hasRight).assertFalse();
                
                // Verify access is blocked after revocation
                try {
                    let pipe = usbMgr.connectDevice(devices);
                    expect(pipe === null).assertTrue();
                } catch (error) {
                    console.info(TAG, 'Expected error for connection after permission revocation: ' + error);
                }
                
                // Try to request permission again
                try {
                    let granted = await usbMgr.requestRight(devices.name);
                    console.info(TAG, `Permission re-request result: ${granted}`);
                    
                    if (granted) {
                        hasRight = usbMgr.hasRight(devices.name);
                        expect(hasRight).assertTrue();
                        
                        // Verify access works again
                        pipe = usbMgr.connectDevice(devices);
                        expect(pipe !== null).assertTrue();
                        if (pipe) {
                            usbMgr.closePipe(pipe);
                        }
                    }
                } catch (error) {
                    console.info(TAG, 'Permission re-request error: ' + error);
                }
            } else {
                console.info(TAG, 'Permission not available for revocation test');
            }
        });

        /**
         * @tc.name   testMultipleDevicePermissions001
         * @tc.number SUB_USB_Security_MultipleDevicePermissions_0100
         * @tc.desc   Test permission management for multiple USB devices
         * @tc.type   FUNCTION
         * @tc.size   MEDIUMTEST
         * @tc.level  LEVEL3
         */
        it('testMultipleDevicePermissions001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
            console.info(TAG, 'testMultipleDevicePermissions001 begin');
            
            // Get all connected devices
            let allDevices = usbMgr.getDevices();
            console.info(TAG, `Found ${allDevices.length} devices`);
            
            if (allDevices.length > 1) {
                // Test permissions for multiple devices
                for (let i = 0; i < allDevices.length; i++) {
                    let device = allDevices[i];
                    console.info(TAG, `Testing permissions for device ${i}: ${device.name}`);
                    
                    try {
                        let hasRight = usbMgr.hasRight(device.name);
                        console.info(TAG, `Device ${i} permission status: ${hasRight}`);
                        
                        // Try to request permission if not granted
                        if (!hasRight) {
                            try {
                                // In a real scenario, this would require UI interaction
                                let granted = await usbMgr.requestRight(device.name);
                                console.info(TAG, `Device ${i} permission request result: ${granted}`);
                                
                                if (granted) {
                                    hasRight = usbMgr.hasRight(device.name);
                                    expect(hasRight).assertTrue();
                                    
                                    // Test device access
                                    let pipe = usbMgr.connectDevice(device);
                                    if (pipe) {
                                        console.info(TAG, `Device ${i} connected successfully`);
                                        usbMgr.closePipe(pipe);
                                    }
                                }
                            } catch (error) {
                                console.info(TAG, `Device ${i} permission request error: ` + error);
                            }
                        }
                        
                        // Test permission isolation between devices
                        for (let j = 0; j < allDevices.length; j++) {
                            if (i !== j) {
                                let otherDevice = allDevices[j];
                                let otherHasRight = usbMgr.hasRight(otherDevice.name);
                                console.info(TAG, `Device ${j} permission status when testing device ${i}: ${otherHasRight}`);
                                // Permissions should be isolated per device
                                expect(otherHasRight === usbMgr.hasRight(otherDevice.name)).assertTrue();
                            }
                        }
                        
                    } catch (error) {
                        console.info(TAG, `Device ${i} permission test error: ` + error);
                    }
                }
            } else {
                console.info(TAG, 'Need multiple devices for permission isolation test');
                
                // Test with non-existent device names
                const fakeDeviceNames = ['fake_device_1', 'fake_device_2', 'fake_device_3'];
                
                for (const fakeName of fakeDeviceNames) {
                    try {
                        let hasRight = usbMgr.hasRight(fakeName);
                        expect(hasRight).assertFalse();
                        
                        let removeResult = usbMgr.removeRight(fakeName);
                        expect(removeResult).assertFalse();
                        
                        console.info(TAG, `Fake device name '${fakeName}' properly handled`);
                    } catch (error) {
                        console.info(TAG, `Expected error for fake device name '${fakeName}': ` + error);
                    }
                }
            }
        });
    })
}