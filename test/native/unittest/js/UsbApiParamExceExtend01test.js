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

import usbManager from '@ohos.usbManager';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium';

export default function UsbApiParamExceExtend01Test() {
    describe('UsbApiParamExceExtend01Test', function () {
        const TAG = "[UsbApiParamExceExtend01Test]";
        const PARAM_NULL = null;
        const PARAM_UNDEFINED = undefined;
        const PARAM_EMPTY_STRING = "";
        let gDeviceList;
        let devices;
        let isDeviceConnected;

        function deviceConnected() {
            if (gDeviceList && gDeviceList.length > 0) {
                console.info(TAG, "Test USB device is connected");
                return true;
            }
            console.info(TAG, "Test USB device is not connected");
            return false;
        }

        beforeAll(function () {
            console.log(TAG, '*************Usb Unit UsbApiParamExceExtend01Test Begin*************');
            const Version = usbManager.getVersion();
            console.info(TAG, 'usb unit begin test getversion :' + Version);
            gDeviceList = usbManager.getDevices();
            isDeviceConnected = deviceConnected();
        })

        beforeEach(function () {
            console.info(TAG, 'beforeEach: *************Usb Unit Test CaseEx*************');
            gDeviceList = usbManager.getDevices();
            if (isDeviceConnected && gDeviceList.length > 0) {
                devices = gDeviceList[0];
            }
        })

        afterEach(function () {
            console.info(TAG, 'afterEach: *************Usb Unit Test CaseEx*************');
            devices = null;
        })

        afterAll(function () {
            console.log(TAG, '*************Usb Unit UsbApiParamExceExtend01Test End*************');
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_0100
         * @tc.name     : testHasRightParamEx003
         * @tc.desc     : Negative test: Param is null
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testHasRightParamEx003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testHasRightParamEx003 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let isHasRight = usbManager.hasRight(PARAM_NULL);
                console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testHasRightParamEx003 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_0200
         * @tc.name     : testHasRightParamEx004
         * @tc.desc     : Negative test: Param is undefined
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testHasRightParamEx004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testHasRightParamEx004 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let isHasRight = usbManager.hasRight(PARAM_UNDEFINED);
                console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testHasRightParamEx004 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_0300
         * @tc.name     : testHasRightParamEx005
         * @tc.desc     : Negative test: Param is number
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testHasRightParamEx005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testHasRightParamEx005 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let isHasRight = usbManager.hasRight(123);
                console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testHasRightParamEx005 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_0400
         * @tc.name     : testHasRightParamEx006
         * @tc.desc     : Negative test: Param is object
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testHasRightParamEx006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testHasRightParamEx006 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let isHasRight = usbManager.hasRight({});
                console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testHasRightParamEx006 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_0500
         * @tc.name     : testHasRightParamEx007
         * @tc.desc     : Negative test: Param is array
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testHasRightParamEx007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testHasRightParamEx007 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let isHasRight = usbManager.hasRight([]);
                console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testHasRightParamEx007 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_0600
         * @tc.name     : testHasRightParamEx008
         * @tc.desc     : Negative test: Param is boolean
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testHasRightParamEx008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testHasRightParamEx008 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let isHasRight = usbManager.hasRight(true);
                console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testHasRightParamEx008 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_0700
         * @tc.name     : testHasRightParamEx009
         * @tc.desc     : Negative test: Param with special characters
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testHasRightParamEx009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testHasRightParamEx009 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let specialChars = "!@#$%^&*()_+-=[]{}|;':\",./<>?";
                let isHasRight = usbManager.hasRight(specialChars);
                console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
                expect(isHasRight).assertFalse();
            } catch (err) {
                console.info(TAG, 'testHasRightParamEx009 catch err code: ', err.code, ', message: ', err.message);
                expect(err !== null).assertTrue();
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_0800
         * @tc.name     : testHasRightParamEx010
         * @tc.desc     : Negative test: Param with unicode characters
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testHasRightParamEx010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testHasRightParamEx010 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let unicodeStr = "测试中文";
                let isHasRight = usbManager.hasRight(unicodeStr);
                console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
                expect(isHasRight).assertFalse();
            } catch (err) {
                console.info(TAG, 'testHasRightParamEx010 catch err code: ', err.code, ', message: ', err.message);
                expect(err !== null).assertTrue();
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_0900
         * @tc.name     : testRequestRightParamEx003
         * @tc.desc     : Negative test: Param is null
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testRequestRightParamEx003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async function () {
            console.info(TAG, 'usb testRequestRightParamEx003 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let isHasRight = await usbManager.requestRight(PARAM_NULL);
                console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testRequestRightParamEx003 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_1000
         * @tc.name     : testRequestRightParamEx004
         * @tc.desc     : Negative test: Param is undefined
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testRequestRightParamEx004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async function () {
            console.info(TAG, 'usb testRequestRightParamEx004 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let isHasRight = await usbManager.requestRight(PARAM_UNDEFINED);
                console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testRequestRightParamEx004 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_1100
         * @tc.name     : testRequestRightParamEx005
         * @tc.desc     : Negative test: Param is number
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testRequestRightParamEx005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async function () {
            console.info(TAG, 'usb testRequestRightParamEx005 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let isHasRight = await usbManager.requestRight(123);
                console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testRequestRightParamEx005 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_1200
         * @tc.name     : testRequestRightParamEx006
         * @tc.desc     : Negative test: Param is object
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testRequestRightParamEx006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async function () {
            console.info(TAG, 'usb testRequestRightParamEx006 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let isHasRight = await usbManager.requestRight({});
                console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testRequestRightParamEx006 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_1300
         * @tc.name     : testRequestRightParamEx007
         * @tc.desc     : Negative test: Param is array
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testRequestRightParamEx007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async function () {
            console.info(TAG, 'usb testRequestRightParamEx007 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let isHasRight = await usbManager.requestRight( []);
                console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testRequestRightParamEx007 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_1400
         * @tc.name     : testRequestRightParamEx008
         * @tc.desc     : Negative test: Param is boolean
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testRequestRightParamEx008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async function () {
            console.info(TAG, 'usb testRequestRightParamEx008 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let isHasRight = await usbManager.requestRight(true);
                console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testRequestRightParamEx008 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_1500
         * @tc.name     : testRequestRightParamEx009
         * @tc.desc     : Negative test: Param with special characters
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testRequestRightParamEx009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async function () {
            console.info(TAG, 'usb testRequestRightParamEx009 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let specialChars = "!@#$%^&*()_+-=[]{}|;':\",./<>?";
                let isHasRight = await usbManager.requestRight(specialChars);
                console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
                expect(isHasRight).assertFalse();
            } catch (err) {
                console.info(TAG, 'testRequestRightParamEx009 catch err code: ', err.code, ', message: ', err.message);
                expect(err !== null).assertTrue();
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_1600
         * @tc.name     : testRequestRightParamEx010
         * @tc.desc     : Negative test: Param with unicode characters
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testRequestRightParamEx010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async function () {
            console.info(TAG, 'usb testRequestRightParamEx010 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let unicodeStr = "测试中文";
                let isHasRight = await usbManager.requestRight(unicodeStr);
                console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
                expect(isHasRight).assertFalse();
            } catch (err) {
                console.info(TAG, 'testRequestRightParamEx010 catch err code: ', err.code, ', message: ', err.message);
                expect(err !== null).assertTrue();
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_1700
         * @tc.name     : testRemoveRightParamEx005
         * @tc.desc     : Negative test: Param is null
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testRemoveRightParamEx005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testRemoveRightParamEx005 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let remRight = usbManager.removeRight(PARAM_NULL);
                console.info(TAG, 'usb case removeRight ret : ' + remRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testRemoveRightParamEx005 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_1800
         * @tc.name     : testRemoveRightParamEx006
         * @tc.desc     : Negative test: Param is undefined
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testRemoveRightParamEx006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testRemoveRightParamEx006 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let remRight = usbManager.removeRight(PARAM_UNDEFINED);
                console.info(TAG, 'usb case removeRight ret : ' + remRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testRemoveRightParamEx006 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_1900
         * @tc.name     : testRemoveRightParamEx007
         * @tc.desc     : Negative test: Param is number
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testRemoveRightParamEx007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testRemoveRightParamEx007 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let remRight = usbManager.removeRight(123);
                console.info(TAG, 'usb case removeRight ret : ' + remRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testRemoveRightParamEx007 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_2000
         * @tc.name     : testRemoveRightParamEx008
         * @tc.desc     : Negative test: Param is object
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testRemoveRightParamEx008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testRemoveRightParamEx008 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let remRight = usbManager.removeRight({});
                console.info(TAG, 'usb case removeRight ret : ' + remRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testRemoveRightParamEx008 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_2100
         * @tc.name     : testRemoveRightParamEx009
         * @tc.desc     : Negative test: Param is array
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testRemoveRightParamEx009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testRemoveRightParamEx009 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let remRight = usbManager.removeRight([]);
                console.info(TAG, 'usb case removeRight ret : ' + remRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testRemoveRightParamEx009 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_2200
         * @tc.name     : testRemoveRightParamEx010
         * @tc.desc     : Negative test: Param is boolean
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testRemoveRightParamEx010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testRemoveRightParamEx010 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let remRight = usbManager.removeRight(true);
                console.info(TAG, 'usb case removeRight ret : ' + remRight);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testRemoveRightParamEx010 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_2300
         * @tc.name     : testRemoveRightParamEx011
         * @tc.desc     : Negative test: Param with special characters
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testRemoveRightParamEx011', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testRemoveRightParamEx011 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let specialChars = "!@#$%^&*()_+-=[]{}|;':\",./<>?";
                let remRight = usbManager.removeRight(specialChars);
                console.info(TAG, 'usb case removeRight ret : ' + remRight);
                expect(remRight).assertFalse();
            } catch (err) {
                console.info(TAG, 'testRemoveRightParamEx011 catch err code: ', err.code, ', message: ', err.message);
                expect(err !== null).assertTrue();
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_2400
         * @tc.name     : testRemoveRightParamEx012
         * @tc.desc     : Negative test: Param with unicode characters
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testRemoveRightParamEx012', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testRemoveRightParamEx012 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let unicodeStr = "测试中文";
                let remRight = usbManager.removeRight(unicodeStr);
                console.info(TAG, 'usb case removeRight ret : ' + remRight);
                expect(remRight).assertFalse();
            } catch (err) {
                console.info(TAG, 'testRemoveRightParamEx012 catch err code: ', err.code, ', message: ', err.message);
                expect(err !== null).assertTrue();
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_2500
         * @tc.name     : testConnectDeviceParamEx028
         * @tc.desc     : Negative test: devices is null
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testConnectDeviceParamEx028', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testConnectDeviceParamEx028 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let gPipe = usbManager.connectDevice(PARAM_NULL);
                console.info(TAG, 'usb connectDevice ret : ', JSON).stringify(gPipe));
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testConnectDeviceParamEx028 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_2600
         * @tc.name     : testConnectDeviceParamEx029
         * @tc.desc     : Negative test: devices is undefined
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testConnectDeviceParamEx029', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testConnectDeviceParamEx029 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let gPipe = usbManager.connectDevice(PARAM_UNDEFINED);
                console.info(TAG, 'usb connectDevice ret : ', JSON.stringify(gPipe));
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testConnectDeviceParamEx029 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_2700
         * @tc.name     : testConnectDeviceParamEx030
         * @tc.desc     : Negative test: devices is empty object
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testConnectDeviceParamEx030', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testConnectDeviceParamEx030 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let gPipe = usbManager.connectDevice({});
                console.info(TAG, 'usb connectDevice ret : ', JSON.stringify(gPipe));
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testConnectDeviceParamEx030 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_2800
         * @tc.name     : testConnectDeviceParamEx031
         * @tc.desc     : Negative test: devices is array
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testConnectDeviceParamEx031', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testConnectDeviceParamEx031 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let gPipe = usbManager.connectDevice([]);
                console.info(TAG, 'usb connectDevice ret : ', JSON.stringify(gPipe));
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testConnectDeviceParamEx031 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_2900
         * @tc.name     : testConnectDeviceParamEx032
         * @tc.desc     : Negative test: devices is string
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testConnectDeviceParamEx032', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testConnectDeviceParamEx032 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let gPipe = usbManager.connectDevice("test");
                console.info(TAG, 'usb connectDevice ret : ', JSON.stringify(gPipe));
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testConnectDeviceParamEx032 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_3000
         * @tc.name     : testConnectDeviceParamEx033
         * @tc.desc     : Negative test: devices is number
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testConnectDeviceParamEx033', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb test)ConnectDeviceParamEx033 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let gPipe = usbManager.connectDevice(123);
                console.info(TAG, 'usb connectDevice ret : ', JSON.stringify(gPipe));
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testConnectDeviceParamEx033 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_3100
         * @tc.name     : testConnectDeviceParamEx034
         * @tc.desc     : Negative test: devices is boolean
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testConnectDeviceParamEx034', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testConnectDeviceParamEx034 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let gPipe = usbManager.connectDevice(true);
                console.info(TAG, 'usb connectDevice ret : ', JSON.stringify(gPipe));
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testConnectDeviceParamEx034 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })
    })
}