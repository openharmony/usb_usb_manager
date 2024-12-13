/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
import { UiDriver, BY } from '@ohos.UiTest';
import CheckEmptyUtils from './CheckEmptyUtils.js';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from '@ohos/hypium'


export default function UsbApiParamErrJsunitTest() {
describe('UsbApiParamErrJsunitTest', function () {

    const TAG = "[UsbApiParamErrJsunitTest]";
    const PARAM_NULL = null;
    const PARAM_UNDEFINED = undefined;
    const PARAM_NULLSTRING = "";
    const PARAM_NUMBERTYPE = 123;
    const PARAM_ERRCODE = 401;
    let gDeviceList;
    let devices;
    let gPipe = {
        busNum: null,
        devAddress: null
    };
    let requestparam;
    let isDeviceConnected;
    function deviceConnected() {
        if (gDeviceList.length > 0) {
            console.info(TAG, "Test USB device is connected");
            return true;
        }
        console.info(TAG, "Test USB device is not connected");
        return false;
    }

    beforeAll(async function () {
        console.log(TAG, '*************Usb Unit UsbApiParamErrJsunitTest Begin*************');
        const Version = usbManager.getVersion();
        console.info(TAG, 'usb unit begin test getversion :' + Version);

        // version > 17  host currentMode = 2 device currentMode = 1
        gDeviceList = usbManager.getDevices();
        console.info(TAG, 'usb unit begin test getDevices **********', JSON.stringify(gDeviceList));
        isDeviceConnected = deviceConnected();
        if (isDeviceConnected) {
            let hasRight = usbManager.hasRight(gDeviceList[0].name);
            if (!hasRight) {
                console.info(TAG, `beforeAll: usb requestRight start`);
                await getPermission();
                CheckEmptyUtils.sleep(1000);
                await driveFn();
                CheckEmptyUtils.sleep(1000);
            }

            requestparam = getControlTransferParam(0x80, 0x06, (0x01 << 8 | 0), 0, 18);
        }
    })

    beforeEach(function () {
        console.info(TAG, 'beforeEach: *************Usb Unit Test CaseEx*************');
        gDeviceList = usbManager.getDevices();
        if (isDeviceConnected) {
            devices = gDeviceList[0];
            console.info(TAG, 'beforeEach return devices : ' + JSON.stringify(devices));
            gPipe.busNum = devices.busNum;
            gPipe.devAddress = devices.devAddress;
            console.info(TAG, 'beforeEach return devices : ' + JSON.stringify(gPipe));
        }
    })

    afterEach(function () {
        console.info(TAG, 'afterEach: *************Usb Unit Test CaseEx*************');
        devices = null;
        console.info(TAG, 'beforeEach return devices : ' + JSON.stringify(devices));
    })

    afterAll(function () {
        console.log(TAG, '*************Usb Unit UsbApiParamErrJsunitTest End*************');
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
            return;
        }
    }

    async function getPermission() {
        console.info('**************getPermission**************');
        try {
            usbManager.requestRight(gDeviceList[0].name).then(hasRight => {
                console.info(TAG, `usb requestRight success, hasRight: ${hasRight}`);
            })
        } catch (err) {
            console.info(TAG, `usb getPermission to requestRight hasRight fail: `, err);
            return
        }
    }

    function getControlTransferParam(iReqType, iReq, iValue, iIndex, iLength) {
        let tmpUint8Array = new Uint8Array(512);

        let requestparam = {
            bmRequestType: iReqType,
            bRequest: iReq,
            wValue: iValue,
            wIndex: iIndex,
            wLength: iLength,
            data: tmpUint8Array
        }
        return requestparam;
    }

    function getPipe(testCaseName) {
        gPipe = usbManager.connectDevice(devices);
        console.info(TAG, `usb ${testCaseName} connectDevice getPipe ret: ${JSON.stringify(gPipe)}`);
        expect(gPipe !== null).assertTrue();
    }

    function toReleaseInterface(testCaseName, tInterface) {
        let ret = usbManager.releaseInterface(gPipe, tInterface);
        console.info(TAG, `usb ${testCaseName} releaseInterface ret: ${ret}`);
        expect(ret).assertEqual(0);
    }

    function toClosePipe(testCaseName) {
        let isPipClose = usbManager.closePipe(gPipe);
        console.info(TAG, `usb ${testCaseName} closePipe ret: ${isPipClose}`);
        expect(isPipClose).assertEqual(0);
    }

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_0100
     * @tc.name     : testGetDevicesParamErr001
     * @tc.desc     : Negative test: Param is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetDevicesParamErr001', 0, function () {
        console.info(TAG, 'usb testGetDevicesParamErr001 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gDeviceList = usbManager.getDevices(PARAM_NULL);
            console.info(TAG, 'usb case getDevices ret length: ' + gDeviceList.length);
            expect(gDeviceList === null).assertTrue();
        } catch (err) {
            console.info(TAG, 'testGetDevicesParamErr001 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_0200
     * @tc.name     : testGetDevicesParamErr002
     * @tc.desc     : Negative test: Param is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetDevicesParamErr002', 0, function () {
        console.info(TAG, 'usb testGetDevicesParamErr002 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gDeviceList = usbManager.getDevices(PARAM_UNDEFINED);
            console.info(TAG, 'usb case getDevices ret length: ' + gDeviceList.length);
            expect(gDeviceList === null).assertTrue();
        } catch (err) {
            console.info(TAG, 'testGetDevicesParamErr002 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_0300
     * @tc.name     : testGetDevicesParamErr003
     * @tc.desc     : Negative test: Param is null string
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetDevicesParamErr003', 0, function () {
        console.info(TAG, 'usb testGetDevicesParamErr003 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gDeviceList = usbManager.getDevices(PARAM_NULLSTRING);
            console.info(TAG, 'usb case getDevices ret length: ' + gDeviceList.length);
            expect(gDeviceList === null).assertTrue();
        } catch (err) {
            console.info(TAG, 'testGetDevicesParamErr003 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_0400
     * @tc.name     : testHasRightParamErr001
     * @tc.desc     : Negative test: Param is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testHasRightParamErr001', 0, function () {
        console.info(TAG, 'usb testHasRightParamErr001 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let isHasRight = usbManager.hasRight(PARAM_NULL);
            console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
            expect(isHasRight === null).assertTrue();
        } catch (err) {
            console.info(TAG, 'testHasRightParamErr001 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_0500
     * @tc.name     : testHasRightParamErr002
     * @tc.desc     : Negative test: Param is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testHasRightParamErr002', 0, function () {
        console.info(TAG, 'usb testHasRightParamErr002 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let isHasRight = usbManager.hasRight(PARAM_UNDEFINED);
            console.info(TAG, 'usb case hasRight ret : ' + isHasRight);
            expect(isHasRight === null).assertTrue();
        } catch (err) {
            console.info(TAG, 'testHasRightParamErr002 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_0600
     * @tc.name     : testRequestRightParamErr001
     * @tc.desc     : Negative test: Param is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRequestRightParamErr001', 0, async function () {
        console.info(TAG, 'usb testRequestRightParamErr001 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let isHasRight = await usbManager.requestRight(PARAM_NULL);
            console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
            expect(isHasRight !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testRequestRightParamErr001 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_0700
     * @tc.name     : testRequestRightParamErr002
     * @tc.desc     : Negative test: Param is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRequestRightParamErr002', 0, async function () {
        console.info(TAG, 'usb testRequestRightParamErr002 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let isHasRight = await usbManager.requestRight(PARAM_UNDEFINED);
            console.info(TAG, 'usb case requestRight ret : ' + isHasRight);
            expect(isHasRight !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testRequestRightParamErr002 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_0800
     * @tc.name     : testRequestRightParamErr003
     * @tc.desc     : Negative test: Enter two parameters
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRequestRightParamErr003', 0, async function () {
        console.info(TAG, 'usb testRequestRightParamErr003 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            for (var i = 0; i < gDeviceList.length; i++) {
                let deviceName = gDeviceList[i].name;
                let isHasRight = await usbManager.requestRight(deviceName, deviceName);
                console.info(TAG, 'usb [', deviceName, '] requestRight ret : ' + isHasRight);
                expect(isHasRight).assertTrue();
            }
        } catch (err) {
            console.info(TAG, 'testRequestRightParamErr003 catch err : ', err);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_0900
     * @tc.name     : testRemoveRightParamErr001
     * @tc.desc     : Negative test: Param is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamErr001', 0, function () {
        console.info(TAG, 'usb testRemoveRightParamErr001 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let remRight = usbManager.removeRight(PARAM_NULL);
            console.info(TAG, 'usb case removeRight ret : ' + remRight);
            expect(remRight !== true).assertTrue();
        } catch (err) {
            console.info(TAG, 'testRemoveRightParamErr001 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_1000
     * @tc.name     : testRemoveRightParamErr002
     * @tc.desc     : Negative test: Param is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamErr002', 0, function () {
        console.info(TAG, 'usb testRemoveRightParamErr002 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let remRight = usbManager.removeRight(PARAM_UNDEFINED);
            console.info(TAG, 'usb case removeRight ret : ' + remRight);
            expect(remRight !== true).assertTrue();
        } catch (err) {
            console.info(TAG, 'testRemoveRightParamErr002 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_1100
     * @tc.name     : testRemoveRightParamErr003
     * @tc.desc     : Negative test: Enter two parameters
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testRemoveRightParamErr003', 0, async function () {
        console.info(TAG, 'usb testRemoveRightParamErr003 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            for (var i = 0; i < gDeviceList.length; i++) {
                let deviceName = gDeviceList[i].name;
                let remRight = usbManager.removeRight(deviceName, deviceName);
                console.info(TAG, 'usb [', deviceName, '] removeRight ret : ' + remRight);
                expect(remRight).assertTrue();
            }
        } catch (err) {
            console.info(TAG, 'testRemoveRightParamErr003 catch err : ', err);
            expect(err !== null).assertFalse();
        }
        await getPermission();
        CheckEmptyUtils.sleep(1000);
        await driveFn();
        CheckEmptyUtils.sleep(1000);
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_1200
     * @tc.name     : testConnectDeviceParamErr001
     * @tc.desc     : Negative test: Param is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr001', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr001 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let ret = usbManager.connectDevice(PARAM_NULL);
            console.info(TAG, 'usb case connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr001 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_1300
     * @tc.name     : testConnectDeviceParamErr002
     * @tc.desc     : Negative test: Param is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr002', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr002 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let ret = usbManager.connectDevice(PARAM_UNDEFINED);
            console.info(TAG, 'usb case connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr002 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_1400
     * @tc.name     : testConnectDeviceParamErr003
     * @tc.desc     : Negative test: Param is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr003', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr003 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let ret = usbManager.connectDevice(PARAM_NULLSTRING);
            console.info(TAG, 'usb case connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr003 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_1500
     * @tc.name     : testConnectDeviceParamErr004
     * @tc.desc     : Negative test: Enter two parameters
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr004', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr004 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe = usbManager.connectDevice(devices, devices);
            console.info(TAG, 'usb [', devices.name, '] connectDevice ret : ', JSON.stringify(gPipe));
            expect(CheckEmptyUtils.isEmpty(gPipe)).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr004 catch err : ', err);
            expect(err !== null).assertFalse();
        }
        toClosePipe('testConnectDeviceParamErr004');
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_1600
     * @tc.name     : testConnectDeviceParamErr005
     * @tc.desc     : Negative test: devices name is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr005', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr005 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.name = PARAM_NULL;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.name, '] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr005 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_1700
     * @tc.name     : testConnectDeviceParamErr006
     * @tc.desc     : Negative test: devices name is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr006', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr006 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.name = PARAM_UNDEFINED;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.name, '] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr006 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_1800
     * @tc.name     : testConnectDeviceParamErr007
     * @tc.desc     : Negative test: devices name is number 123
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr007', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr007 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.name = PARAM_NUMBERTYPE;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [', devices.name, '] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr007 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_1900
     * @tc.name     : testConnectDeviceParamErr008
     * @tc.desc     : Negative test: devices busNum is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr008', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr008 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.busNum = PARAM_NULL;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [busNum:null] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr008 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2000
     * @tc.name     : testConnectDeviceParamErr009
     * @tc.desc     : Negative test: devices busNum is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr009', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr009 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.busNum = PARAM_UNDEFINED;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [busNum:undefined] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr009 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2100
     * @tc.name     : testConnectDeviceParamErr010
     * @tc.desc     : Negative test: devices busNum null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr010', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr010 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.busNum = PARAM_NULLSTRING;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [busNum:null string] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr010 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2200
     * @tc.name     : testConnectDeviceParamErr011
     * @tc.desc     : Negative test: devices devAddress is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr011', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr011 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.devAddress = PARAM_NULL;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [devAddress:null] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr011 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2300
     * @tc.name     : testConnectDeviceParamErr012
     * @tc.desc     : Negative test: devices devAddress is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr012', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr012 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.devAddress = PARAM_UNDEFINED;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [devAddress:undefined] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr012 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2400
     * @tc.name     : testConnectDeviceParamErr013
     * @tc.desc     : Negative test: devices devAddress is null string
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr013', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr013 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.devAddress = PARAM_NULLSTRING;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [devAddress:null string] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr013 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2500
     * @tc.name     : testConnectDeviceParamErr014
     * @tc.desc     : Negative test: devices serial is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr014', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr014 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.serial = PARAM_NULL;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [serial:null] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr014 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2600
     * @tc.name     : testConnectDeviceParamErr015
     * @tc.desc     : Negative test: devices serial is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr015', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr015 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.serial = PARAM_UNDEFINED;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [serial:undefined] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr015 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2700
     * @tc.name     : testConnectDeviceParamErr016
     * @tc.desc     : Negative test: devices serial is number 123
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr016', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr016 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.serial = PARAM_NUMBERTYPE;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [serial:123] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr016 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2800
     * @tc.name     : testConnectDeviceParamErr017
     * @tc.desc     : Negative test: devices manufacturerName is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr017', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr017 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.manufacturerName = PARAM_NULL;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [manufacturerName:null] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr017 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_2900
     * @tc.name     : testConnectDeviceParamErr018
     * @tc.desc     : Negative test: devices manufacturerName is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr018', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr018 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.manufacturerName = PARAM_UNDEFINED;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [manufacturerName:undefined] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr018 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_3000
     * @tc.name     : testConnectDeviceParamErr019
     * @tc.desc     : Negative test: devices manufacturerName is number 123
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr019', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr019 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.manufacturerName = PARAM_NUMBERTYPE;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [manufacturerName:123] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr019 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_3100
     * @tc.name     : testConnectDeviceParamErr020
     * @tc.desc     : Negative test: devices productName is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr020', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr020 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.productName = PARAM_NULL;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [productName:null] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr020 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_3200
     * @tc.name     : testConnectDeviceParamErr021
     * @tc.desc     : Negative test: devices productName is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr021', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr021 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.productName = PARAM_UNDEFINED;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [productName:undefined] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr021 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_3300
     * @tc.name     : testConnectDeviceParamErr022
     * @tc.desc     : Negative test: devices productName is number 123
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr022', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr022 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.productName = PARAM_NUMBERTYPE;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [productName:123] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr022 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_3400
     * @tc.name     : testConnectDeviceParamErr023
     * @tc.desc     : Negative test: devices version is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr023', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr023 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.version = PARAM_NULL;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [version:null] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr023 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_3500
     * @tc.name     : testConnectDeviceParamErr024
     * @tc.desc     : Negative test: devices version is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr024', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr024 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.version = PARAM_UNDEFINED;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [version:undefined] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr024 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_3600
     * @tc.name     : testConnectDeviceParamErr025
     * @tc.desc     : Negative test: devices vendorId is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr025', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr025 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.vendorId = PARAM_NULL;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [vendorId:null] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr025 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_3700
     * @tc.name     : testConnectDeviceParamErr026
     * @tc.desc     : Negative test: devices vendorId is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr026', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr026 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.vendorId = PARAM_UNDEFINED;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [vendorId:undefined] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr026 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_3800
     * @tc.name     : testConnectDeviceParamErr027
     * @tc.desc     : Negative test: devices vendorId is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr027', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr027 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.vendorId = PARAM_NULLSTRING;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [vendorId:""] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr027 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_3900
     * @tc.name     : testConnectDeviceParamErr028
     * @tc.desc     : Negative test: devices productId is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr028', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr028 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.productId = PARAM_NULL;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [productId:null] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr028 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_4000
     * @tc.name     : testConnectDeviceParamErr029
     * @tc.desc     : Negative test: devices productId is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr029', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr029 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.productId = PARAM_UNDEFINED;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [productId:undefined] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr029 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_4100
     * @tc.name     : testConnectDeviceParamErr030
     * @tc.desc     : Negative test: devices productId is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr030', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr030 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.productId = PARAM_NULLSTRING;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [productId:" "] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr030 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_4200
     * @tc.name     : testConnectDeviceParamErr031
     * @tc.desc     : Negative test: devices clazz is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr031', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr031 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.clazz = PARAM_NULL;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [clazz:null] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr031 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_4300
     * @tc.name     : testConnectDeviceParamErr032
     * @tc.desc     : Negative test: devices clazz is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr032', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr032 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.clazz = PARAM_UNDEFINED;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [clazz:undefined] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr032 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_4400
     * @tc.name     : testConnectDeviceParamErr033
     * @tc.desc     : Negative test: devices clazz is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr033', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr033 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.clazz = PARAM_NULLSTRING;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [clazz:""] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr033 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_4500
     * @tc.name     : testConnectDeviceParamErr034
     * @tc.desc     : Negative test: devices subClass is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr034', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr034 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.subClass = PARAM_NULL;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [subClass:null] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr034 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_4600
     * @tc.name     : testConnectDeviceParamErr035
     * @tc.desc     : Negative test: devices subClass is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr035', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr035 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.subClass = PARAM_UNDEFINED;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [subClass:undefined] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr035 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_4700
     * @tc.name     : testConnectDeviceParamErr036
     * @tc.desc     : Negative test: devices subClass is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr036', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr036 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.subClass = PARAM_NULLSTRING;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [subClass:""] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr036 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_4800
     * @tc.name     : testConnectDeviceParamErr037
     * @tc.desc     : Negative test: devices protocol is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr037', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr037 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.protocol = PARAM_NULL;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [protocol:null] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr037 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_4900
     * @tc.name     : testConnectDeviceParamErr038
     * @tc.desc     : Negative test: devices protocol is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr038', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr038 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.protocol = PARAM_UNDEFINED;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [protocol:undefined] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr038 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_5000
     * @tc.name     : testConnectDeviceParamErr039
     * @tc.desc     : Negative test: devices protocol is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr039', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr039 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.protocol = PARAM_NULLSTRING;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [protocol:""] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr039 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_5100
     * @tc.name     : testConnectDeviceParamErr040
     * @tc.desc     : Negative test: devices configs is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr040', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr040 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.configs = PARAM_NULL;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [configs:null] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr040 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_5200
     * @tc.name     : testConnectDeviceParamErr041
     * @tc.desc     : Negative test: devices configs is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr041', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr041 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.configs = PARAM_UNDEFINED;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [configs:undefined] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr041 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_5300
     * @tc.name     : testConnectDeviceParamErr042
     * @tc.desc     : Negative test: devices configs is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr042', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr042 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.configs = PARAM_NULLSTRING;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [configs:""] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr042 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_5400
     * @tc.name     : testConnectDeviceParamErr043
     * @tc.desc     : Negative test: devices configs is number 123
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testConnectDeviceParamErr043', 0, function () {
        console.info(TAG, 'usb testConnectDeviceParamErr043 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            devices.configs = PARAM_NULLSTRING;
            let ret = usbManager.connectDevice(devices);
            console.info(TAG, 'usb [configs:123] connectDevice ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testConnectDeviceParamErr043 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_5500
     * @tc.name     : testClosePipeParamErr001
     * @tc.desc     : Negative test: Enter two parameters
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClosePipeParamErr001', 0, function () {
        console.info(TAG, 'usb testClosePipeParamErr001 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe('testClosePipeParamErr001');
        try {
            let ret = usbManager.closePipe(gPipe, gPipe);
            console.info(TAG, 'usb Enter two parameters closePipe ret : ', ret);
            expect(ret).assertEqual(0);
        } catch (err) {
            console.info(TAG, 'testClosePipeParamErr001 catch err : ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_5600
     * @tc.name     : testClosePipeParamErr002
     * @tc.desc     : Negative test: pipe busNum is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClosePipeParamErr002', 0, function () {
        console.info(TAG, 'usb testClosePipeParamErr002 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = PARAM_NULL;
            let ret = usbManager.closePipe(gPipe);
            console.info(TAG, 'usb [busNum:null] closePipe ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testClosePipeParamErr002 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_5700
     * @tc.name     : testClosePipeParamErr003
     * @tc.desc     : Negative test: pipe busNum is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClosePipeParamErr003', 0, function () {
        console.info(TAG, 'usb testClosePipeParamErr003 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = PARAM_UNDEFINED;
            let ret = usbManager.closePipe(gPipe);
            console.info(TAG, 'usb [busNum:undefined] closePipe ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testClosePipeParamErr003 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_5800
     * @tc.name     : testClosePipeParamErr004
     * @tc.desc     : Negative test: pipe busNum is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClosePipeParamErr004', 0, function () {
        console.info(TAG, 'usb testClosePipeParamErr004 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = PARAM_NULLSTRING;
            let ret = usbManager.closePipe(gPipe);
            console.info(TAG, 'usb [busNum:""] closePipe ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testClosePipeParamErr004 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_5900
     * @tc.name     : testClosePipeParamErr005
     * @tc.desc     : Negative test: pipe devAddress is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClosePipeParamErr005', 0, function () {
        console.info(TAG, 'usb testClosePipeParamErr005 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.devAddress = PARAM_NULL;
            let ret = usbManager.closePipe(gPipe);
            console.info(TAG, 'usb [devAddress:null] closePipe ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testClosePipeParamErr005 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_6000
     * @tc.name     : testClosePipeParamErr006
     * @tc.desc     : Negative test: pipe devAddress is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClosePipeParamErr006', 0, function () {
        console.info(TAG, 'usb testClosePipeParamErr006 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.devAddress = PARAM_UNDEFINED;
            let ret = usbManager.closePipe(gPipe);
            console.info(TAG, 'usb [devAddress:undefined] closePipe ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testClosePipeParamErr006 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_6100
     * @tc.name     : testClosePipeParamErr007
     * @tc.desc     : Negative test: devices devAddress is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClosePipeParamErr007', 0, function () {
        console.info(TAG, 'usb testClosePipeParamErr007 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.devAddress = PARAM_NULLSTRING;
            let ret = usbManager.closePipe(gPipe);
            console.info(TAG, 'usb [devAddress:""] closePipe ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testClosePipeParamErr007 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_6200
     * @tc.name     : testClosePipeParamErr008
     * @tc.desc     : Negative test: Param is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClosePipeParamErr008', 0, function () {
        console.info(TAG, 'usb testClosePipeParamErr008 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let ret = usbManager.closePipe(PARAM_NULL);
            console.info(TAG, 'usb [param:null] closePipe ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testClosePipeParamErr008 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_6300
     * @tc.name     : testClosePipeParamErr009
     * @tc.desc     : Negative test: Param is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClosePipeParamErr009', 0, function () {
        console.info(TAG, 'usb testClosePipeParamErr009 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let ret = usbManager.closePipe(PARAM_UNDEFINED);
            console.info(TAG, 'usb [param:undefined] closePipe ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testClosePipeParamErr009 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_6400
     * @tc.name     : testClosePipeParamErr010
     * @tc.desc     : Negative test: Param is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testClosePipeParamErr010', 0, function () {
        console.info(TAG, 'usb testClosePipeParamErr010 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let ret = usbManager.closePipe(PARAM_NULLSTRING);
            console.info(TAG, 'usb [param:""] closePipe ret : ', ret);
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testClosePipeParamErr010 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_6500
     * @tc.name     : testGetRawDescriptorParamErr001
     * @tc.desc     : Negative test: Enter two parameters
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetRawDescriptorParamErr001', 0, function () {
        console.info(TAG, 'usb testGetRawDescriptorParamErr001 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        getPipe('testGetRawDescriptorParamErr001');
        try {
            let ret = usbManager.getRawDescriptor(gPipe, gPipe);
            console.info(TAG, 'usb Enter two parameters getRawDescriptor ret : ', JSON.stringify(ret));
            expect(ret.length >= 0).assertTrue();
        } catch (err) {
            console.info(TAG, 'testGetRawDescriptorParamErr001 catch err code: ', err.code, ', message: ', err.message);
            expect(err !== null).assertFalse();
        }
        toClosePipe('testGetRawDescriptorParamErr001');
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_6600
     * @tc.name     : testGetRawDescriptorParamErr002
     * @tc.desc     : Negative test: Param is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetRawDescriptorParamErr002', 0, function () {
        console.info(TAG, 'usb testGetRawDescriptorParamErr002 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let ret = usbManager.getRawDescriptor(PARAM_NULL);
            console.info(TAG, 'usb [param:null] getRawDescriptor ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testGetRawDescriptorParamErr002 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_6700
     * @tc.name     : testGetRawDescriptorParamErr003
     * @tc.desc     : Negative test: Param is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetRawDescriptorParamErr003', 0, function () {
        console.info(TAG, 'usb testGetRawDescriptorParamErr003 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let ret = usbManager.getRawDescriptor(PARAM_UNDEFINED);
            console.info(TAG, 'usb [param:undefined] getRawDescriptor ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testGetRawDescriptorParamErr003 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_6800
     * @tc.name     : testGetRawDescriptorParamErr004
     * @tc.desc     : Negative test: Param is null string ""
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetRawDescriptorParamErr004', 0, function () {
        console.info(TAG, 'usb testGetRawDescriptorParamErr004 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            let ret = usbManager.getRawDescriptor(PARAM_NULLSTRING);
            console.info(TAG, 'usb [param:""] getRawDescriptor ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testGetRawDescriptorParamErr004 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_6900
     * @tc.name     : testGetRawDescriptorParamErr005
     * @tc.desc     : Negative test: pipe busNum is null
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetRawDescriptorParamErr005', 0, function () {
        console.info(TAG, 'usb testGetRawDescriptorParamErr005 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = PARAM_NULL;
            let ret = usbManager.getRawDescriptor(gPipe);
            console.info(TAG, 'usb [busNum:null] getRawDescriptor ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testGetRawDescriptorParamErr005 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

    /**
     * @tc.number   : SUB_USB_HostManager_JS_ParamErr_7000
     * @tc.name     : testGetRawDescriptorParamErr006
     * @tc.desc     : Negative test: pipe busNum is undefined
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testGetRawDescriptorParamErr006', 0, function () {
        console.info(TAG, 'usb testGetRawDescriptorParamErr006 begin');
        if (!isDeviceConnected) {
            expect(isDeviceConnected).assertFalse();
            return
        }
        try {
            gPipe.busNum = PARAM_UNDEFINED;
            let ret = usbManager.getRawDescriptor(gPipe);
            console.info(TAG, 'usb [busNum:undefined] getRawDescriptor ret : ', JSON.stringify(ret));
            expect(ret !== null).assertFalse();
        } catch (err) {
            console.info(TAG, 'testGetRawDescriptorParamErr006 catch err code: ', err.code, ', message: ', err.message);
            expect(err.code).assertEqual(PARAM_ERRCODE);
        }
    })

})
}