/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
import CheckEmptyUtils from './CheckEmptyUtils.js';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium';


export default function UsbAccessoryJsTest() {
describe("UsbAccessoryJsTest", function () {

    const TAG = '[UsbAccessoryJsTest]';

    let accList;
    let isDevAccessoryFunc;
    let isSupport = true;

    function devAccessoryFunc() {
        if (accList.length > 0) {
            console.info(TAG, "This USB device is accessory function");
            return true;
        }
        console.info(TAG, "This USB device is accessory function");
        return false;
    }

    beforeAll(function() {
        console.info(TAG, 'beforeAll called');
        try {
            accList = usbMgr.getAccessoryList();
            console.info(TAG, 'beforeAll ret : ', JSON.stringify(accList));
        } catch (err) {
            console.info(TAG, 'beforeAll err : ', err);
            if (err.code == 801) {
                isSupport = false;
            }
        }
        isDevAccessoryFunc = devAccessoryFunc();
    })

    afterAll(function() {
        console.info(TAG, 'afterAll called');
    })

    beforeEach(function() {
        console.info(TAG, 'beforeEach called');
    })

    afterEach(function() {
        console.info(TAG, 'afterEach called');
    })

    async function driveFn() {
        console.info('**************driveFn**************');
        try {
            let driver = await UiDriver.create();
            console.info(TAG, ` come in driveFn`);
            console.info(TAG, `driver is ${JSON.stringify(driver)}`);
            CheckEmptyUtils.sleep(1000);
            let button = await driver.findComponent(BY.text('允许'));
            console.info(TAG, `button is ${JSON.stringify(button)}`);
            CheckEmptyUtils.sleep(1000);
            await button.click();
        } catch (err) {
            console.info(TAG, 'err is ' + err);
            return
        }
    }
    
    async function getAccPermission() {
        console.info('**************getAccPermission**************');
        try {
            usbMgr.requestAccessoryRight(accList[0]).then(hasAccRight => {
                console.info(TAG, `usb requestAccessoryRight success, hasAccRight: ${hasAccRight}`);
            })
        } catch (err) {
            console.info(TAG, `usb getPermission to requestAccessoryRight hasAccRight fail: `, err);
            return
        }
    }
    
    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_Accessory_0100
     * @tc.name       : testGetAccessoryList001
     * @tc.desc       : verify getAccessoryList result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testGetAccessoryList001", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
        console.info(TAG, '----------------------testGetAccessoryList001---------------------------');
        if (!isDevAccessoryFunc) {
            expect(isDevAccessoryFunc).assertFalse();
            return
        }
        try {
            accList = usbMgr.getAccessoryList();
            console.info(TAG, 'testGetAccessoryList001 ret : ', JSON.stringify(accList));
            expect(accList != null).assertEqual(true);
            expect(accList.length == 1).assertEqual(true);
            expect(typeof(accList[0].manufacturer)).assertEqual('string');
            expect(typeof(accList[0].product)).assertEqual('string');
            expect(typeof(accList[0].description)).assertEqual('string');
            expect(typeof(accList[0].version)).assertEqual('string');
            expect(typeof(accList[0].serialNumber)).assertEqual('string');
        } catch (err) {
            console.info(TAG, 'testGetAccessoryList001 err : ', err);
            expect(err.code).assertEqual(14400005);
        }
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_Accessory_0200
     * @tc.name       : testRequestAccessoryRight001
     * @tc.desc       : verify requestAccessoryRight no param result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testRequestAccessoryRight001", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
        console.info(TAG, '----------------------testRequestAccessoryRight001---------------------------');
        if (!isDevAccessoryFunc) {
            expect(isDevAccessoryFunc).assertFalse();
            return
        }
        try {
            let access = {"manufacturer": "accessoryTest"};
            await usbMgr.requestAccessoryRight(access).then(data => {
                console.info(TAG, 'testRequestAccessoryRight001 ret : ', JSON.stringify(data));
                expect(data !== null).assertFalse();
            });
        } catch (err) {
            console.info(TAG, 'testRequestAccessoryRight001 err : ', err);
            expect(err.code).assertEqual(14400005);
        }
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_Accessory_0300
     * @tc.name       : testRequestAccessoryRight002
     * @tc.desc       : verify requestAccessoryRight result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testRequestAccessoryRight002", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
        console.info(TAG, '----------------------testRequestAccessoryRight002---------------------------');
        try {
            await usbMgr.requestAccessoryRight(accList[0]).then(data => {
                console.info(TAG, 'testRequestAccessoryRight002 ret : ', JSON.stringify(data));
                expect(data).assertTrue();
            });
            await driveFn();
            CheckEmptyUtils.sleep(1000);
            usbMgr.cancelAccessoryRight(accList[0]);
        } catch (err) {
            console.info(TAG, 'testRequestAccessoryRight002 err : ', err);
            expect(isDevAccessoryFunc).assertFalse();
            if (!isDevAccessoryFunc) {
                expect(err.code).assertEqual(401);
            } else {
                expect(err.code).assertEqual(14401001);
            }
        }
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_Accessory_0400
     * @tc.name       : testHasAccessoryRight001
     * @tc.desc       : erify hasAccessoryRight no param result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testHasAccessoryRight001", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
        console.info(TAG, '----------------------testHasAccessoryRight001---------------------------');
        if (!isDevAccessoryFunc) {
            expect(isDevAccessoryFunc).assertFalse();
            return
        }
        try {
            let access = { manufacturer: "accessoryTest" };
            let ret = usbMgr.hasAccessoryRight(access);
            console.info(TAG, 'testHasAccessoryRight001 ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testHasAccessoryRight001 err : ', err);
            expect(err.code).assertEqual(14400005);
        }
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_Accessory_0500
     * @tc.name       : testHasAccessoryRight002
     * @tc.desc       : verify hasAccessoryRight result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testHasAccessoryRight002", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
        console.info(TAG, '----------------------testHasAccessoryRight002---------------------------');
        try {
            let ret = usbMgr.hasAccessoryRight(accList[0]);
            console.info(TAG, 'testHasAccessoryRight002 ret : ', ret);
            expect(ret).assertFalse();
        } catch (err) {
            console.info(TAG, 'testHasAccessoryRight002 err : ', err);
            if (!isDevAccessoryFunc) {
                expect(err.code).assertEqual(401);
            } else {
                expect(err.code).assertEqual(14401001);
            }
        }
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_Accessory_0600
     * @tc.name       : testHasAccessoryRight003
     * @tc.desc       : verify hasAccessoryRight result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testHasAccessoryRight003", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
        console.info(TAG, '----------------------testHasAccessoryRight003---------------------------');
        await getAccPermission();
        CheckEmptyUtils.sleep(1000);
        await driveFn();
        CheckEmptyUtils.sleep(1000);
        try {
            let ret = usbMgr.hasAccessoryRight(accList[0]);
            console.info(TAG, 'testHasAccessoryRight003 ret : ', ret);
            expect(ret).assertTrue();
            usbMgr.cancelAccessoryRight(accList[0]);
            ret = usbMgr.hasAccessoryRight(accList[0]);
            expect(ret).assertFalse();
        } catch (err) {
            console.info(TAG, 'testHasAccessoryRight003 err : ', err);
            if (!isDevAccessoryFunc) {
                expect(err.code).assertEqual(401);
            } else {
                expect(err.code).assertEqual(14401001);
            }
        }
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_Accessory_0700
     * @tc.name       : testCancelAccessoryRight001
     * @tc.desc       : verify cancelAccessoryRight no param result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testCancelAccessoryRight001", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
        console.info(TAG, '----------------------testCancelAccessoryRight001---------------------------');
        if (!isDevAccessoryFunc) {
            expect(isDevAccessoryFunc).assertFalse();
            return
        }
        try {
            let access = {"manufacturer": "accessoryTest"};
            usbMgr.cancelAccessoryRight(access);
            let ret = usbMgr.hasAccessoryRight(access);
            console.info(TAG, 'testCancelAccessoryRight001 ret : ', ret);
            expect(ret).assertFalse();
        } catch (err) {
            console.info(TAG, 'testCancelAccessoryRight001 err : ', err);
            expect(err.code).assertEqual(14400005);
        }
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_Accessory_0800
     * @tc.name       : testCancelAccessoryRight002
     * @tc.desc       : verify cancelAccessoryRight result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testCancelAccessoryRight002", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
        console.info(TAG, '----------------------testCancelAccessoryRight002---------------------------');
        await getAccPermission();
        CheckEmptyUtils.sleep(1000);
        await driveFn();
        CheckEmptyUtils.sleep(1000);
        try {
            usbMgr.cancelAccessoryRight(accList[0]);
            let ret = usbMgr.hasAccessoryRight(access);
            console.info(TAG, 'testCancelAccessoryRight002 ret : ', ret);
            expect(ret).assertFalse();
        } catch (err) {
            console.info(TAG, 'testCancelAccessoryRight002 err : ', err);
            if (!isDevAccessoryFunc) {
                expect(err.code).assertEqual(401);
            } else {
                expect(err.code).assertEqual(14401001);
            }
        }
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_Accessory_0900
     * @tc.name       : testOpenAccessory001
     * @tc.desc       : verify openAccessory no param result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testOpenAccessory001", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
        console.info(TAG, '----------------------testOpenAccessory001---------------------------');
        if (!isDevAccessoryFunc) {
            expect(isDevAccessoryFunc).assertFalse();
            return
        }
        try {
            let access = {"manufacturer": "accessoryTest"};
            let ret = usbMgr.openAccessory(access);
            console.info(TAG, 'testOpenAccessory001 ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testOpenAccessory001 err : ', err);
            expect(err.code).assertEqual(14400005);
        }
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_Accessory_1000
     * @tc.name       : testOpenAccessory002
     * @tc.desc       : verify openAccessory result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testOpenAccessory002", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done) => {
        console.info(TAG, '----------------------testOpenAccessory002---------------------------');
        await getAccPermission();
        CheckEmptyUtils.sleep(1000);
        await driveFn();
        CheckEmptyUtils.sleep(1000);
        try {
            let accHandle = usbMgr.openAccessory(accList[0]);
            console.info(TAG, 'testOpenAccessory002 ret : ', accHandle);
            expect(accHandle !== null).assertTrue();
            let accFd = accHandle.accessoryFd;
            expect(accFd > 0).assertTrue();
            usbMgr.closeAccessory(accHandle);
        } catch (err) {
            console.info(TAG, 'testOpenAccessory002 err : ', err);
            if (!isDevAccessoryFunc) {
                expect(err.code).assertEqual(401);
                done();
            } else {
                expect(err.code).assertEqual(14401001);
            }
        }
        usbMgr.cancelAccessoryRight(accList[0]);
        done();
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_Accessory_1100
     * @tc.name       : testOpenAccessory003
     * @tc.desc       : verify openAccessory reopen
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testOpenAccessory003", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done) => {
        console.info(TAG, '----------------------testOpenAccessory003---------------------------');
        await getAccPermission();
        CheckEmptyUtils.sleep(1000);
        await driveFn();
        CheckEmptyUtils.sleep(1000);
        try {
            let accHandle = usbMgr.openAccessory(accList[0]);
            console.info(TAG, 'testOpenAccessory003 accHandle : ', accHandle);
            expect(accHandle !== null).assertTrue();
            accHandle = usbMgr.openAccessory(accList[0]);
            expect(accHandle !== null).assertFalse();
            usbMgr.closeAccessory(accHandle);
        } catch (err) {
            console.info(TAG, 'testOpenAccessory003 err : ', err);
            if (!isDevAccessoryFunc) {
                expect(err.code).assertEqual(401);
                done();
            } else {
                console.info(TAG, 'testOpenAccessory003 reopen err : ', err);
                expect(err.code).assertEqual(14401003);
            }
        }
        usbMgr.cancelAccessoryRight(accList[0]);
        done();
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_Accessory_1200
     * @tc.name       : testOpenAccessory004
     * @tc.desc       : verify openAccessory no permission
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testOpenAccessory004", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
        console.info(TAG, '----------------------testOpenAccessory004---------------------------');
        try {
            let acchandle = usbMgr.openAccessory(accList[0]);
            expect(acchandle !== null).assertTrue();
        } catch (err) {
            console.info(TAG, 'testOpenAccessory004 reopen err : ', err);
            if (!isDevAccessoryFunc) {
                expect(err.code).assertEqual(401);
            } else {
                expect(err.code).assertEqual(14400001);
            }
        }
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_Accessory_1300
     * @tc.name       : testCloseAccessory001
     * @tc.desc       : verify closeAccessory result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testCloseAccessory001", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done) => {
        console.info(TAG, '----------------------testCloseAccessory001---------------------------');
        await getAccPermission();
        CheckEmptyUtils.sleep(1000);
        await driveFn();
        CheckEmptyUtils.sleep(1000);
        let acchandle;
        try {
            acchandle = usbMgr.openAccessory(accList[0]);
            expect(acchandle !== null).assertTrue();
            usbMgr.closeAccessory(acchandle);
        } catch (err) {
            console.info(TAG, 'testCloseAccessory001 err : ', err);
            if (!isDevAccessoryFunc) {
                expect(err.code).assertEqual(401);
            } else {
                expect(err.code).assertEqual(14401001);
            }
        }
        try {
            usbMgr.closeAccessory(acchandle);
        } catch (err) {
            if (!isDevAccessoryFunc) {
                expect(err.code).assertEqual(401);
                done();
            } else {
                expect(err.code).assertEqual(14400004);
            }
        }
        usbMgr.cancelAccessoryRight(accList[0]);
        done();
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_Accessory_1400
     * @tc.name       : testCloseAccessory002
     * @tc.desc       : verify openAccessory no param result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testCloseAccessory002", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
        console.info(TAG, '----------------------testCloseAccessory002---------------------------');
        if (!isDevAccessoryFunc) {
            expect(isDevAccessoryFunc).assertFalse();
            return
        }
        let acchandle = {"accessFd": 0};
        try {
            usbMgr.closeAccessory(acchandle);
        } catch (err) {
            console.info(TAG, 'testCloseAccessory002 err : ', err);
            expect(err.code).assertEqual(14400005);
        }
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_AccessoryErr_0600
     * @tc.name       : testHasAccessoryRight801Err003
     * @tc.desc       : verify HasAccessoryRight result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testHasAccessoryRight801Err003", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
        console.info(TAG, '----------------------testHasAccessoryRight801Err003---------------------------');
        await getAccPermission();
        CheckEmptyUtils.sleep(1000);
        await driveFn();
        CheckEmptyUtils.sleep(1000);
        try {
            let ret = usbMgr.hasAccessoryRight(accList[0]);
            console.info(TAG, 'testHasAccessoryRight801Err003 ret : ', ret);
            expect(ret).assertTrue();
            usbMgr.cancelAccessoryRight(accList[0]);
            ret = usbMgr.hasAccessoryRight(accList[0]);
            expect(ret).assertFalse();
        } catch (err) {
            console.info(TAG, 'testHasAccessoryRight801Err003 err : ', err);
            if (!isSupport) {
                expect(err.code).assertEqual(801);
            } else if (!isDevAccessoryFunc) {
                expect(err.code).assertEqual(401);
            } else {
                expect(err.code).assertEqual(14401001);
            }
        }
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_AccessoryErr_0200
     * @tc.name       : testRequestAccessoryRight801Err001
     * @tc.desc       : verify requestAccessoryRight no param result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testRequestAccessoryRight801Err001", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
        console.info(TAG, '----------------------testRequestAccessoryRight001---------------------------');
        if (!isDevAccessoryFunc) {
            expect(isDevAccessoryFunc).assertFalse();
            return
        }
        try {
            let access = {"manufacturer": "accessoryTest"};
            await usbMgr.requestAccessoryRight(access).then(data => {
                console.info(TAG, 'testRequestAccessoryRight801Err001 ret : ', JSON.stringify(data));
                expect(data !== null).assertFalse();
            });
        } catch (err) {
            console.info(TAG, 'testRequestAccessoryRight801Err001 err : ', err);
            if(err){
            expect(err.code).assertEqual(14400005);
            }else{
            expect(err.code).assertEqual(801);            
            }
        }
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_AccessoryErr_0700
     * @tc.name       : testCancelAccessoryRight801Err001
     * @tc.desc       : verify cancelAccessoryRight no param result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testCancelAccessoryRight801Err001", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
        console.info(TAG, '----------------------testCancelAccessoryRight801Err001---------------------------');
        if (!isDevAccessoryFunc) {
            expect(isDevAccessoryFunc).assertFalse();
            return
        }
        try {
            let access = {"manufacturer": "accessoryTest"};
            usbMgr.cancelAccessoryRight(access);
            let ret = usbMgr.hasAccessoryRight(access);
            console.info(TAG, 'testCancelAccessoryRight801Err001 ret : ', ret);
            expect(ret).assertFalse();
        } catch (err) {
            console.info(TAG, 'testCancelAccessoryRight801Err001 err : ', err);
            if(err){
                expect(err.code).assertEqual(14400005);
            }else{
                expect(err.code).assertEqual(801);  
            }
        }
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_AccessoryErr_0100
     * @tc.name       : testGetAccessoryList801Err001
     * @tc.desc       : verify getAccessoryList result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testGetAccessoryList801Err001", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
        console.info(TAG, '----------------------testGetAccessoryList801Err001---------------------------');
        if (!isDevAccessoryFunc) {
            expect(isDevAccessoryFunc).assertFalse();
            return
        }
        try {
            accList = usbMgr.getAccessoryList();
            console.info(TAG, 'testGetAccessoryList801Err001 ret : ', JSON.stringify(accList));
            expect(accList != null).assertEqual(true);
            expect(accList.length == 1).assertEqual(true);
            expect(typeof(accList[0].manufacturer)).assertEqual('string');
            expect(typeof(accList[0].product)).assertEqual('string');
            expect(typeof(accList[0].description)).assertEqual('string');
            expect(typeof(accList[0].version)).assertEqual('string');
            expect(typeof(accList[0].serialNumber)).assertEqual('string');
        } catch (err) {
            console.info(TAG, 'testGetAccessoryList801Err001 err : ', err);
            if(err){
                expect(err.code).assertEqual(14400005);
            }else{
                expect(err.code).assertEqual(801);  
            }
        }
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_AccessoryErr_0900
     * @tc.name       : testOpenAccessory001
     * @tc.desc       : verify openAccessory no param result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testOpenAccessory801Err001", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
        console.info(TAG, '----------------------testOpenAccessory801Err001---------------------------');
        if (!isDevAccessoryFunc) {
            expect(isDevAccessoryFunc).assertFalse();
            return
        }
        try {
            let access = {"manufacturer": "accessoryTest"};
            let ret = usbMgr.openAccessory(access);
            console.info(TAG, 'testOpenAccessory801Err001 ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testOpenAccessory801Err001 err : ', err);
            if(err){
                expect(err.code).assertEqual(14400005);
            }else{
                expect(err.code).assertEqual(801);  
            }
        }
    })

    /**
     * @tc.number     : SUB_USB_DeviceManager_JS_AccessoryErr_1400
     * @tc.name       : testCloseAccessory801Err002
     * @tc.desc       : verify openAccessory no param result
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it("testCloseAccessory801Err002", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, () => {
        console.info(TAG, '----------------------testCloseAccessory801Err002---------------------------');
        if (!isDevAccessoryFunc) {
            expect(isDevAccessoryFunc).assertFalse();
            return
        }
        let acchandle = {"accessFd": 0};
        try {
            usbMgr.closeAccessory(acchandle);
        } catch (err) {
            console.info(TAG, 'testCloseAccessory801Err002 err : ', err);
            if(err){
                expect(err.code).assertEqual(14400005);
            }else{
                expect(err.code).assertEqual(801);  
            }
        }
    })

    
    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2010
     * @tc.name     : testBulkTransferParamErr041
     * @tc.desc     : Negative test: endpoint interfaceId is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
     it('testBulkTransferParamErr041', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testBulkTransferParamErr041 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpEndpoints = devices.configs[0].interfaces[0].endpoints[0];
            let buffer =  new Uint8Array(128);
            tmpEndpoints.interfaceId = PARAM_UNDEFINED;
            let ret = usbManager.bulkTransfer(gPipe, tmpEndpoints, buffer);
            console.info(TAG, 'usb [endpoint.interfaceId:undefined] bulkTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testBulkTransferParamErr041 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2020
     * @tc.name     : testBulkTransferParamErr042
     * @tc.desc     : Negative test: endpoint interfaceId is ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testBulkTransferParamErr042', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testBulkTransferParamErr042 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpEndpoints = devices.configs[0].interfaces[0].endpoints[0];
            let buffer =  new Uint8Array(128);
            tmpEndpoints.interfaceId = PARAM_NULLSTRING;
            let ret = usbManager.bulkTransfer(gPipe, tmpEndpoints, buffer);
            console.info(TAG, 'usb [endpoint.interfaceId:""] bulkTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testBulkTransferParamErr042 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2030
     * @tc.name     : testUsbControlTransferParamErr001
     * @tc.desc     : Negative test: Param is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr001 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let ret = usbManager.usbControlTransfer(PARAM_NULL);
            console.info(TAG, 'usb [param:null] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr001 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2040
     * @tc.name     : testUsbControlTransferParamErr002
     * @tc.desc     : Negative test: Param is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr002 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let ret = usbManager.usbControlTransfer(PARAM_UNDEFINED);
            console.info(TAG, 'usb [param:undefined] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr002 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2050
     * @tc.name     : testUsbControlTransferParamErr003
     * @tc.desc     : Negative test: Param is ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr003 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let ret = usbManager.usbControlTransfer(PARAM_NULLSTRING);
            console.info(TAG, 'usb [param:""] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr003 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2060
     * @tc.name     : testUsbControlTransferParamErr004
     * @tc.desc     : Negative test: pipe is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr004 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let timeout = 5000;
            let ret = usbManager.usbControlTransfer(PARAM_NULL, requestparam, timeout);
            console.info(TAG, 'usb [pipe:null] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr004 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2070
     * @tc.name     : testUsbControlTransferParamErr005
     * @tc.desc     : Negative test: pipe is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr005 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let timeout = 5000;
            let ret = usbManager.usbControlTransfer(PARAM_UNDEFINED, requestparam, timeout);
            console.info(TAG, 'usb [pipe:undefined] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr005 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2080
     * @tc.name     : testUsbControlTransferParamErr006
     * @tc.desc     : Negative test: pipe is ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr006 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let timeout = 5000;
            let ret = usbManager.usbControlTransfer(PARAM_NULLSTRING, requestparam, timeout);
            console.info(TAG, 'usb [pipe:""] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr006 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2090
     * @tc.name     : testUsbControlTransferParamErr007
     * @tc.desc     : Negative test: requestparam is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr007 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let timeout = 5000;
            let ret = usbManager.usbControlTransfer(gPipe, PARAM_NULL, timeout);
            console.info(TAG, 'usb [requestparam:null] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr007 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2110
     * @tc.name     : testUsbControlTransferParamErr008
     * @tc.desc     : Negative test: requestparam is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr008 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let timeout = 5000;
            let ret = usbManager.usbControlTransfer(gPipe, PARAM_UNDEFINED, timeout);
            console.info(TAG, 'usb [requestparam:undefined] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr008 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2120
     * @tc.name     : testUsbControlTransferParamErr009
     * @tc.desc     : Negative test: requestparam is ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr009 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let timeout = 5000;
            let ret = usbManager.usbControlTransfer(gPipe, PARAM_NULLSTRING, timeout);
            console.info(TAG, 'usb [requestparam:""] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr009 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2130
     * @tc.name     : testUsbControlTransferParamErr010
     * @tc.desc     : Negative test: pipe busNum is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr010 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = PARAM_NULL;
            gPipe.devAddress = devices.devAddress;
            let timeout = 5000;
            let ret = usbManager.usbControlTransfer(gPipe, requestparam, timeout);
            console.info(TAG, 'usb [busNum:null] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr010 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2140
     * @tc.name     : testUsbControlTransferParamErr011
     * @tc.desc     : Negative test: pipe busNum is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr011', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr011 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = PARAM_UNDEFINED;
            gPipe.devAddress = devices.devAddress;
            let timeout = 5000;
            let ret = usbManager.usbControlTransfer(gPipe, requestparam, timeout);
            console.info(TAG, 'usb [busNum:undefined] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr011 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2150
     * @tc.name     : testUsbControlTransferParamErr012
     * @tc.desc     : Negative test: pipe busNum is ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr012', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr012 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = PARAM_NULLSTRING;
            gPipe.devAddress = devices.devAddress;
            let timeout = 5000;
            let ret = usbManager.usbControlTransfer(gPipe, requestparam, timeout);
            console.info(TAG, 'usb [busNum:""] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr012 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2160
     * @tc.name     : testUsbControlTransferParamErr013
     * @tc.desc     : Negative test: pipe devAddress is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr013', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr013 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = PARAM_NULL;
            let timeout = 5000;
            let ret = usbManager.usbControlTransfer(gPipe, requestparam, timeout);
            console.info(TAG, 'usb [devAddress:null] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr013 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2170
     * @tc.name     : testUsbControlTransferParamErr014
     * @tc.desc     : Negative test: pipe devAddress is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr014', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr014 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = PARAM_UNDEFINED;
            let timeout = 5000;
            let ret = usbManager.usbControlTransfer(gPipe, requestparam, timeout);
            console.info(TAG, 'usb [devAddress:undefined] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr014 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2180
     * @tc.name     : testUsbControlTransferParamErr015
     * @tc.desc     : Negative test: pipe devAddress is ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr015', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr015 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = PARAM_NULLSTRING;
            let timeout = 5000;
            let ret = usbManager.usbControlTransfer(gPipe, requestparam, timeout);
            console.info(TAG, 'usb [devAddress:""] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr015 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2190
     * @tc.name     : testUsbControlTransferParamErr016
     * @tc.desc     : Negative test: requestparam bmRequestType is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr016', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr016 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(PARAM_NULL, 0x06, (0x01 << 8 | 0), 0, 18);
            let timeout = 5000;

            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.bmRequestType:null] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr016 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2210
     * @tc.name     : testUsbControlTransferParamErr017
     * @tc.desc     : Negative test: requestparam bmRequestType is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr017', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr017 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(PARAM_UNDEFINED, 0x06, (0x01 << 8 | 0), 0, 18);
            let timeout = 5000;

            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.bmRequestType:undefined] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr017 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2220
     * @tc.name     : testUsbControlTransferParamErr018
     * @tc.desc     : Negative test: requestparam bmRequestType is ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr018', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr018 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(PARAM_NULLSTRING, 0x06, (0x01 << 8 | 0), 0, 18);
            let timeout = 5000;

            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.bmRequestType:""] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr018 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2230
     * @tc.name     : testUsbControlTransferParamErr019
     * @tc.desc     : Negative test: requestparam bRequest is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr019', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr019 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(0x80, PARAM_NULL, (0x01 << 8 | 0), 0, 18);
            let timeout = 5000;

            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.bRequest:null] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr019 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2240
     * @tc.name     : testUsbControlTransferParamErr020
     * @tc.desc     : Negative test: requestparam bRequest is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr020', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr020 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(0x80, PARAM_UNDEFINED, (0x01 << 8 | 0), 0, 18);
            let timeout = 5000;

            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.bRequest:undefined] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr020 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2250
     * @tc.name     : testUsbControlTransferParamErr021
     * @tc.desc     : Negative test: requestparam bRequest is ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr021', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr021 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(0x80, PARAM_NULLSTRING, (0x01 << 8 | 0), 0, 18);
            let timeout = 5000;

            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.bRequest:""] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr021 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2260
     * @tc.name     : testUsbControlTransferParamErr022
     * @tc.desc     : Negative test: requestparam wValue is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr022', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr022 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(0x80, 0x60, PARAM_NULL, 0, 18);
            let timeout = 5000;

            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.wValue:null] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr022 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2270
     * @tc.name     : testUsbControlTransferParamErr023
     * @tc.desc     : Negative test: requestparam wValue is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr023', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr023 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(0x80, 0x60, PARAM_UNDEFINED, 0, 18);
            let timeout = 5000;

            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.wValue:undefined] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr023 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2280
     * @tc.name     : testUsbControlTransferParamErr024
     * @tc.desc     : Negative test: requestparam wValue is ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr024', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr024 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(0x80, 0x60, PARAM_NULLSTRING, 0, 18);
            let timeout = 5000;

            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.wValue:""] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr024 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2290
     * @tc.name     : testUsbControlTransferParamErr025
     * @tc.desc     : Negative test: requestparam wIndex is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr025', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr025 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(0x80, 0x60, (0x01 << 8 | 0), PARAM_NULL, 0);
            let timeout = 5000;

            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.wIndex:null] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr025 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2310
     * @tc.name     : testUsbControlTransferParamErr026
     * @tc.desc     : Negative test: requestparam wIndex is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr026', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr026 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(0x80, 0x60, (0x01 << 8 | 0), PARAM_UNDEFINED, 0);
            let timeout = 5000;

            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.wIndex:undefined] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr026 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2320
     * @tc.name     : testUsbControlTransferParamErr027
     * @tc.desc     : Negative test: requestparam wIndex is ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr027', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr027 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(0x80, 0x60, (0x01 << 8 | 0), PARAM_NULLSTRING, 0);
            let timeout = 5000;

            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.wIndex:""] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr027 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2330
     * @tc.name     : testUsbControlTransferParamErr028
     * @tc.desc     : Negative test: requestparam wLength is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr028', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr028 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(0x80, 0x60, (0x01 << 8 | 0), 0, PARAM_NULL);
            let timeout = 5000;

            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.wLength:null] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr028 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2340
     * @tc.name     : testUsbControlTransferParamErr029
     * @tc.desc     : Negative test: requestparam wLength is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr029', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr029 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(0x80, 0x60, (0x01 << 8 | 0), 0, PARAM_UNDEFINED);
            let timeout = 5000;

            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.wLength:undefined] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr029 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2350
     * @tc.name     : testUsbControlTransferParamErr030
     * @tc.desc     : Negative test: requestparam wLength is ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr030', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr030 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(0x80, 0x60, (0x01 << 8 | 0), 0, PARAM_NULLSTRING);
            let timeout = 5000;

            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.wLength:""] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr030 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2360
     * @tc.name     : testUsbControlTransferParamErr031
     * @tc.desc     : Negative test: requestparam data is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr031', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr031 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(0x80, 0x60, (0x01 << 8 | 0), 0, 18);
            let timeout = 5000;

            tmpRequestparam.data = PARAM_NULL;
            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.data:null] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr031 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2370
     * @tc.name     : testUsbControlTransferParamErr032
     * @tc.desc     : Negative test: requestparam data is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr032', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr032 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(0x80, 0x60, (0x01 << 8 | 0), 0, 18);
            let timeout = 5000;

            tmpRequestparam.data = PARAM_UNDEFINED;
            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.data:undefined] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr032 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2380
     * @tc.name     : testUsbControlTransferParamErr033
     * @tc.desc     : Negative test: requestparam data is ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testUsbControlTransferParamErr033', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
        console.info(TAG, 'usb testUsbControlTransferParamErr033 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            let tmpRequestparam = getControlTransferParam(0x80, 0x60, (0x01 << 8 | 0), 0, 18);
            let timeout = 5000;

            tmpRequestparam.data = PARAM_NULLSTRING;
            let ret = usbManager.usbControlTransfer(gPipe, tmpRequestparam, timeout);
            console.info(TAG, 'usb [requestparam.data:""] usbControlTransfer ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testUsbControlTransferParamErr033 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(401);
        }
    })
})
}
