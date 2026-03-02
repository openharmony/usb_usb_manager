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

export default function UsbApiParamExceExtend03Test() {
    describe('UsbApiParamExceExtend03Test', function () {
        const TAG = "[UsbApiParamExceExtend03Test]";
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
            console.log(TAG, '*************Usb Unit UsbApiParamExceExtend03Test Begin*************');
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
            console.log(TAG, '*************Usb Unit UsbApiParamExceExtend03Test End*************');
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
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_6000
         * @tc.name     : testReleaseInterfaceParamEx001
         * @tc.desc     : Negative test: pipe is null
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testReleaseInterfaceParamEx001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testReleaseInterfaceParamEx001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.releaseInterface(PARAM_NULL, devices.configs[0].interfaces[0]);
                console.info(TAG, 'usb releaseInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testReleaseInterfaceParamEx001 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_6100
         * @tc.name     : testReleaseInterfaceParamEx002
         * @tc.desc     : Negative test: pipe is undefined
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testReleaseInterfaceParamEx002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testReleaseInterfaceParamEx002 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.releaseInterface(PARAM_UNDEFINED, devices.configs[0].interfaces[0]);
                console.info(TAG, 'usb releaseInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testReleaseInterfaceParamEx002 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_6200
         * @tc.name     : testReleaseInterfaceParamEx003
         * @tc.desc     : Negative test: pipe is empty object
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testReleaseInterfaceParamEx003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testReleaseInterfaceParamEx003 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.releaseInterface({}, devices.configs[0].interfaces[0]);
                console.info(TAG, 'usb releaseInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testReleaseInterfaceParamEx003 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_6300
         * @tc.name     : testReleaseInterfaceParamEx004
         * @tc.desc     : Negative test: pipe is string
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testReleaseInterfaceParamEx004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testReleaseInterfaceParamEx004 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.releaseInterface("test", devices.configs[0].interfaces[0]);
                console.info(TAG, 'usb releaseInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testReleaseInterfaceParamEx004 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_6400
         * @tc.name     : testReleaseInterfaceParamEx005
         * @tc.desc     : Negative test: pipe is number
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testReleaseInterfaceParamEx005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testReleaseInterfaceParamEx005 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.releaseInterface(123, devices.configs[0].interfaces[0]);
                console.info(TAG, 'usb releaseInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testReleaseInterfaceParamEx005 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_6500
         * @tc.name     : testReleaseInterfaceParamEx006
         * @tc.desc     : Negative test: pipe is array
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testReleaseInterfaceParamEx006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testReleaseInterfaceParamEx006 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.releaseInterface([], devices.configs[0].interfaces[0]);
                console.info(TAG, 'usb releaseInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testReleaseInterfaceParamEx006 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_6600
         * @tc.name     : testReleaseInterfaceParamEx007
         * @tc.desc     : Negative test: pipe is boolean
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testReleaseInterfaceParamEx007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testReleaseInterfaceParamEx007 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.releaseInterface(true, devices.configs[0].interfaces[0]);
                console.info(TAG, 'usb releaseInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testReleaseInterfaceParamEx007 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_6700
         * @tc.name     : testSetInterfaceParamEx001
         * @tc.desc     : Negative test: pipe is null
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testSetInterfaceParamEx001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testSetInterfaceParamEx001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.setInterface(PARAM_NULL, devices.configs[0].interfaces[0], true);
                console.info(TAG, 'usb setInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testSetInterfaceParamEx001 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_6800
         * @tc.name     : testSetInterfaceParamEx002
         * @tc.desc     : Negative test: pipe is undefined
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testSetInterfaceParamEx002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testSetInterfaceParamEx002 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.setInterface(PARAM_UNDEFINED, devices.configs[0].interfaces[0], true);
                console.info(TAG, 'usb setInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testSetInterfaceParamEx002 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_6900
         * @tc.name     : testSetInterfaceParamEx003
         * @tc.desc     : Negative test: pipe is empty object
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testSetInterfaceParamEx003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testSetInterfaceParamEx003 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.setInterface({}, devices.configs[0].interfaces[0], true);
                console.info(TAG, 'usb setInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testSetInterfaceParamEx003 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_7000
         * @tc.name     : testSetInterfaceParamEx004
         * @tc.desc     : Negative test: pipe is string
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testSetInterfaceParamEx004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testSetInterfaceParamEx004 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.setInterface("test", devices.configs[0].interfaces[0], true);
                console.info(TAG, 'usb setInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testSetInterfaceParamEx004 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_7100
         * @tc.name     : testSetInterfaceParamEx005
         * @tc.desc     : Negative: test: pipe is number
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testSetInterfaceParamEx005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testSetInterfaceParamEx005 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.setInterface(123, devices.configs[0].interfaces[0], true);
                console.info(TAG, 'usb setInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testSetInterfaceParamEx005 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_7200
         * @tc.name     : testSetInterfaceParamEx006
         * @tc.desc     : Negative test: pipe is array
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testSetInterfaceParamEx006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testSetInterfaceParamEx006 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.setInterface([], devices.configs[0].interfaces[0], true);
                console.info(TAG, 'usb setInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testSetInterfaceParamEx006 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_7300
         * @tc.name     : testSetInterfaceParamEx007
         * @tc.desc     : Negative test: pipe is boolean
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testSetInterfaceParamEx007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, function () {
            console.info(TAG, 'usb testSetInterfaceParamEx007 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let ret = usbManager.setInterface(true, devices.configs[0].interfaces[0], true);
                console.info(TAG, 'usb setInterface ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testSetInterfaceParamEx007 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_7400
         * @tc.name     : testBulkTransferParamEx001
         * @tc.desc     : Negative test: pipe is null
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testBulkTransferParamEx001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async function () {
            console.info(TAG, 'usb testBulkTransferParamEx001 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let data = new Uint8Array([1, 2, 3]);
                let ret = await usbManager.bulkTransfer(PARAM_NULL, devices.configs[0].interfaces[0].endpoints[0], data, 5000);
                console.info(TAG, 'usb bulkTransfer ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testBulkTransferParamEx001 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_7500
         * @tc.name     : testBulkTransferParamEx002
         * @tc.desc     : Negative test: pipe is undefined
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testBulkTransferParamEx002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async function () {
            console.info(TAG, 'usb testBulkTransferParamEx002 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let data = new Uint8Array([1, 2, 3]);
                let ret = await usbManager.bulkTransfer(PARAM_UNDEFINED, devices.configs[0].interfaces[0].endpoints[0], data, 5000);
                console.info(TAG, 'usb bulkTransfer ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testBulkTransferParamEx002 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_7600
         * @tc.name     : testBulkTransferParamEx003
         * @tc.desc     : Negative test: pipe is empty object
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testBulkTransferParamEx003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async function () {
            console.info(TAG, 'usb testBulkTransferParamEx003 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let data = new Uint8Array([1, 2, 3]);
                let ret = await usbManager.bulkTransfer({}, devices.configs[0].interfaces[0].endpoints[0], data, 5000);
                console.info(TAG, 'usb bulkTransfer ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testBulkTransferParamEx003 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_7700
         * @tc.name     : testBulkTransferParamEx004
         * @tc.desc     : Negative test: pipe is string
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testBulkTransferParamEx004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async function () {
            console.info(TAG, 'usb testBulkTransferParamEx004 begin');
            if (!isDeviceConnected) {
               ) expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let data = new Uint8Array([1, 2, 3]);
                let ret = await usbManager.bulkTransfer("test", devices.configs[0].interfaces[0].endpoints[0], data, 5000);
                console.info(TAG, 'usb bulkTransfer ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testBulkTransferParamEx004 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_7800
         * @tc.name     : testBulkTransferParamEx005
         * @tc.desc     : Negative test: pipe is number
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testBulkTransferParamEx005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async function () {
            console.info(TAG, 'usb testBulkTransferParamEx005 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let data = new Uint8Array([1, 2, 3]);
                let ret = await usbManager.bulkTransfer(123, devices.configs[0].interfaces[0].endpoints[0], data, 5000);
                console.info)TAG, 'usb bulkTransfer ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testBulkTransferParamEx005 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_7900
         * @tc.name     : testBulkTransferParamEx006
         * @tc.desc     : Negative test: pipe is array
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testBulkTransferParamEx006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async function () {
            console.info(TAG, 'usb testBulkTransferParamEx006 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let data = new Uint8Array([1, 2, 3]);
                let ret = await usbManager.bulkTransfer([], devices.configs[0].interfaces[0].endpoints[0], data, 5000);
                console.info(TAG, 'usb bulkTransfer ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testBulkTransferParamEx006 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })

        /**
         * @tc.number   : SUB_USB_HostManager_JS_ParamEx_Extend_8000
         * @tc.name     : testBulkTransferParamEx007
         * @tc.desc     : Negative test: pipe is boolean
         * @tc.size     : MediumTest
         * @tc.type     : Function
         * @tc.level    : Level 3
         */
        it('testBulkTransferParamEx007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async function () {
            console.info(TAG, 'usb testBulkTransferParamEx007 begin');
            if (!isDeviceConnected) {
                expect(isDeviceConnected).assertFalse();
                return;
            }
            try {
                let data = new Uint8Array([1, 2, 3]);
                let ret = await usbManager.bulkTransfer(true, devices.configs[0].interfaces[0].endpoints[0], data, 5000);
                console.info(TAG, 'usb bulkTransfer ret : ', ret);
                expect(false).assertTrue();
            } catch (err) {
                console.info(TAG, 'testBulkTransferParamEx007 catch err code: ', err.code, ', message: ', err.message);
                expect(err.code).assertEqual(401);
            }
        })
    })
}