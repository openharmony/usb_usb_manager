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

export default function UsbApiParamExceExtend02Test() {
    describe('UsbryApiParamExceExtend02Test', function () {
        const TAG = "[UsbApiParamExceExtend02Test]";
        const PARAM_NULL = null;
        const PARAM_UNDEFINED = undefined;
        let gDeviceList;
        let gPipe;
        let devices;
        let isDeviceConnected;
        let tmpPipe = {
            busNum: null,
            devAddress: null
        };

        function deviceConnected() {
            if (gDeviceList && gDeviceList.length > 0) {
                console.info(TAG, "Test USB device is connected");
                return true;
            }
            console.info(TAG, "Test USB device is not connected");
            return false;
        }

        beforeAll(function () {
            console.log(TAG, '*************Usb Unit UsbApiParamExceExtend02Test Begin*************');
            const Version = usbManager.getVersion();
            console.info(TAG, 'usb unit begin test getversion :' + Version);
            gDeviceList = usbManager.getDevices();
            isDeviceConnected = deviceConnected();
            if (isDeviceConnected) {
                tmpPipe.busNum = gDeviceList[0].busNum;
                tmpPipe.devAddress = gDeviceList[0].devAddress;
            }
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
            gPipe = null;
        })

        afterAll(function () {
            console.log(TAG, '*************Usb Unit UsbApiParamExceExtend02Test End*************');
        })

        function getPipe(testCaseName) {
            gPipe = usbManager.connectDevice(devices);
            console.info(TAG, `usb ${testCaseName} connectDevice getPipe ret: ${JSON.stringify(gPipe)}`);
            expect(gPipe !== null).assertTrue();
        }

        function toClosePipe(testCaseName) {
            let isPipClose = usbManager.closePipe(tmpPipe);
            console.info(TAG, `usb ${testCaseName} closePipe ret: ${isPipClose}`);
            expect(isPipClose).assertEqual(0);
        }

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_3200
         * @tc.name     : testClosePipeParamEx003
         * @tc.desc     : Negative test: pipe is null
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testClosePipeParamEx003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testClosePipeParamEx003 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.closePipe(PARAM_NULL);
                console.info(TAG, 'usb closePipe ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testClosePipeParamEx003 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_3300
         * @tc.name     : testClosePipeParamEx004
         * @tc.desc     : Negative test: pipe is undefined
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testClosePipeParamEx004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testClosePipeParamEx004 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.closePipe(PARAM_UNDEFINED);
                console.info(TAG, 'usb closePipe ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testClosePipeParamEx004 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamExr_Extend_3400
         * @tc.name     : testClosePipeParamEx005
         * @tc.desc     : Negative test: pipe is empty object
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testClosePipeParamEx005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testClosePipeParamEx005 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.closePipe({});
                console.info(TAG, 'usb closePipe ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testClosePipeParamEx005 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_3500
         * @tc.name     : testClosePipeParamEx006
         * @tc.desc     : Negative test: pipe is string
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testClosePipeParamEx006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testClosePipeParamEx006 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.closePipe("test");
                console.info(TAG, 'usb closePipe ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testClosePipeParamEx006 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_3600
         * @tc.name     : testClosePipeParamEx007
         * @tc.desc     : Negative test: pipe is number
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testClosePipeParamEx007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testClosePipeParamEx007 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.closePipe(123);
                console.info(TAG, 'usb closePipe ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testClosePipeParamEx007 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_3700
         * @tc.name     : testClosePipeParamEx008
         * @tc.desc     : Negative test: pipe is array
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testClosePipeParamEx008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testClosePipeParamEx008 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.closePipe([]);
                console.info(TAG, 'usb closePipe ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testClosePipeParamEx008 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS)ParamEx_Extend_3800
         * @tc.name     : testClosePipeParamEx009
         * @tc.desc     : Negative test: pipe is boolean
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testClosePipeParamEx009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testClosePipeParamEx009 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.closePipe(true);
                console.info(TAG, 'usb closePipe ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testClosePipeParamEx009 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_3900
         * @tc.name     : testGetRawDescriptorParamEx006
         * @tc.desc     : Negative test: pipe is null
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testGetRawDescriptorParamEx006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testGetRawDescriptorParamEx006 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.getRawDescriptor(PARAM_NULL);
                console.info(TAG, 'usb getRawDescriptor ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testGetRawDescriptorParamEx006 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_4000
         * @tc.name     : testGetRawDescriptorParamEx007
         * @tc.desc     : Negative test: pipe is undefined
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testGetRawDescriptorParamEx007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testGetRawDescriptorParamEx007 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.getRawDescriptor(PARAM_UNDEFINED);
                console.info(TAG, 'usb getRawDescriptor ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testGetRawDescriptorParamEx007 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_4100
         * @tc.name     : testGetRawDescriptorParamEx008
         * @tc.desc     : Negative test: pipe is empty object
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testGetRawDescriptorParamEx008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testGetRawDescriptorParamEx008 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.getRawDescriptor({});
                console.info(TAG, 'usb getRawDescriptor ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testGetRawDescriptorParamEx008 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_4200
         * @tc.name     : testGetRawDescriptorParamEx009
         * @tc.desc     : Negative test: pipe is string
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testGetRawDescriptorParamEx009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testGetRawDescriptorParamEx009 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.getRawDescriptor("test");
                console.info(TAG, 'usb getRawDescriptor ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testGetRawDescriptorParamEx009 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_4300
         * @tc.name     : testGetRawDescriptorParam)Ex010
         * @tc.desc     : Negative test: pipe is number
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testGetRawDescriptorParamEx010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testGetRawDescriptorParamEx010 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.getRawDescriptor(123);
                console.info(TAG, 'usb getRawDescriptor ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testGetRawDescriptorParamEx010 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_4400
         * @tc.name     : testGetRawDescriptorParamEx011
         * @tc.desc     : Negative test: pipe is array
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testGetRawDescriptorParamEx011', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testGetRawDescriptorParamEx011 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.getRawDescriptor([]);
                console.info(TAG, 'usb getRawDescriptor ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testGetRawDescriptorParamEx011 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_4500
         * @tc.name     : testGetRawDescriptorParamEx012
         * @tc.desc     : Negative test: pipe is boolean
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testGetRawDescriptorParamEx012', TestType.FUNCTION | Size)MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testGetRawDescriptorParamEx012 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.getRawDescriptor(true);
                console.info(TAG, 'usb getRawDescriptor ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testGetRawDescriptorParamEx012 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_4600
         * @tc.name     : testGetFileDescriptorParamEx001
         * @tc.desc     : Negative test: pipe is null
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testGetFileDescriptorParamEx001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testGetFileDescriptorParamEx001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.getFileDescriptor(PARAM_NULL);
                console.info(TAG, 'usb getFileDescriptor ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testGetFileDescriptorParamEx001 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_4700
         * @tc.name     : testGetFileDescriptorParamEx002
         * @tc.desc     : Negative test: pipe is undefined
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testGetFileDescriptorParamEx002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testGetFileDescriptorParamEx002 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.getFileDescriptor(PARAM_UNDEFINED);
                console.info(TAG, 'usb getFileDescriptor ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testGetFileDescriptorParamEx002 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_4800
         * @tc.name     : testGetFileDescriptorParamEx003
         * @tc.desc     : Negative test: pipe is empty object
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testGetFileDescriptorParamEx003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testGetFileDescriptorParamEx003 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.getFileDescriptor({});
                console.info(TAG, 'usb getFileDescriptor ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testGetFileDescriptorParamEx003 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_4900
         * @tc.name     : testGetFileDescriptorParamEx004
         * @tc.desc     : Negative test: pipe is string
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testGetFileDescriptorParamEx004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testGetFileDescriptorParamEx004 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.getFileDescriptor("test");
                console.info(TAG, 'usb getFileDescriptor ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testGetFileDescriptorParamEx004 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_5000
         * @tc.name     : testGetFileDescriptorParamEx005
         * @tc.desc     : Negative test: pipe is number
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testGetFileDescriptorParamEx005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testGetFileDescriptorParamEx005 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.getFileDescriptor(123);
                console.info(TAG, 'usb getFileDescriptor ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testGetFileDescriptorParamEx005 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_5100
         * @tc.name     : testGetFileDescriptorParamEx006
         * @tc.desc     : Negative test: pipe is array
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testGetFileDescriptorParamEx006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testGetFileDescriptorParamEx006 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.getFileDescriptor([]);
                console.info(TAG, 'usb getFileDescriptor ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testGetFileDescriptorParamEx006 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_5200
         * @tc.name     : testGetFileDescriptorParamEx007
         * @tc.desc     : Negative test: pipe is boolean
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testGetFileDescriptorParamEx007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testGetFileDescriptorParamEx007 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.getFileDescriptor(true);
                console.info(TAG, 'usb getFileDescriptor ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testGetFileDescriptorParamEx007 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_5300
         * @tc.name     : testClaimInterfaceParamEx001
         * @tc.desc     : Negative test: pipe is null
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testClaimInterfaceParamEx001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testClaimInterfaceParamEx001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.claimInterface(PARAM_NULL, devices.configs[0].interfaces[0], true);
                console.info(TAG, 'usb claimInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testClaimInterfaceParamEx001 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_5400
         * @tc.name     : testClaimInterfaceParamEx002
         * @tc.desc     : Negative test: pipe is undefined
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testClaimInterfaceParamEx002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testClaimInterfaceParamEx002 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.claimInterface(PARAM_UNDEFINED, devices.configs[0].interfaces[0], true);
                console.info(TAG, 'usb claimInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testClaimInterfaceParamEx002 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_5500
         * @tc.name     : testClaimInterfaceParamEx003
         * @tc.desc     : Negative test: pipe is empty object
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testClaimInterfaceParamEx003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testClaimInterfaceParamEx003 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.claimInterface({}, devices.configs[0].interfaces[0], true);
                console.info(TAG, 'usb claimInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testClaimInterfaceParamEx003 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_5600
         * @tc.name     : testClaimInterfaceParamEx004
         * @tc.desc     : Negative test: pipe is string
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testClaimInterfaceParamEx004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testClaimInterfaceParamEx004 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.claimInterface("test", devices.configs[0].interfaces[0], true);
                console.info(TAG, 'usb claimInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testClaimInterfaceParamEx004 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_5700
         * @tc.name     : testClaimInterfaceParamEx005
         * @tc.desc     : Negative test: pipe is number
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testClaimInterfaceParamEx005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testClaimInterfaceParamEx005 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.claimInterface(123, devices.configs[0].interfaces[0], true);
                console.info(TAG, 'usb claimInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testClaimInterfaceParamEx005 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_5800
         * @tc.name     : testClaimInterfaceParamEx006
         * @tc.desc     : Negative test: pipe is array
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testClaimInterfaceParamEx006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testClaimInterfaceParamEx006 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.claimInterface([], devices.configs[0].interfaces[0], true);
                console.info(TAG, 'usb claimInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testClaimInterfaceParamEx006 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_5900
         * @tc.name     : testClaimInterfaceParamEx007
         * @tc.desc     : Negative test: pipe is boolean
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testClaimInterfaceParamEx007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testClaimInterfaceParamEx007 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.claimInterface(true, devices.configs[0].interfaces[0], true);
                console.info(TAG, 'usb claimInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testClaimInterfaceParamEx007 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })
    })
}