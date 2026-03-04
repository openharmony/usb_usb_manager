/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

import usbManager from '@ohos.usbManager'
import bundleManager from '@ohos.bundle.bundleManager';
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe("UsbAccessoryJsTestNegative", function () {
    let bundleInfo = null;
    let tokenId = 0;
    let accList = [];

    beforeAll(function() {
        console.info('beforeAll called')
        try {
            const bundleFlags = bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION
                | bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_EXTENSION_ABILITY;
            bundleInfo = bundleManager.getBundleInfoForSelfSync(bundleFlags);
            tokenId = bundleInfo.appInfo.accessTokenId;
            accList = usbManager.getAccessoryList();
        } catch (err) {
            console.error("beforeAll error: " + JSON.stringify(err))
        }
    })

    afterAll(function() {
        console.info('afterAll called')
    })

    beforeEach(function() {
        console.info('beforeEach called')
    })

    afterEach(function() {
        console.info('afterEach called')
    })

    const SERVICE_EXCEPTION_CODE = 14400005;
    const PARAM_EXCEPTION_CODE = 401;

    /*
     * @tc.name:UsbAccessory_neg_requestAccessoryRight_001
     * @tc.desc:verify requestAccessoryRight with number param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_requestAccessoryRight_001", 0, () => {
        console.info('----------------------UsbAccessory_neg_requestAccessoryRight_001---------------------------');
        try {
            usbManager.requestAccessoryRight(123);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_requestAccessoryRight_002
     * @tc.desc:verify requestAccessoryRight with boolean param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_requestAccessoryRight_002", 0, () => {
        console.info('----------------------UsbAccessory_neg_requestAccessoryRight_002---------------------------');
        try {
            usbManager.requestAccessoryRight(true);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_requestAccessoryRight_003
     * @tc.desc:verify requestAccessoryRight with array param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_requestAccessoryRight_003", 0, () => {
        console.info('----------------------UsbAccessory_neg_requestAccessoryRight_003---------------------------');
        try {
            usbManager.requestAccessoryRight([1, 2, 3]);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_requestAccessoryRight_004
     * @tc.desc:verify requestAccessoryRight with function param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_requestAccessoryRight_004", 0, () => {
        console.info('----------------------UsbAccessory_neg_requestAccessoryRight_004---------------------------');
        try {
            usbManager.requestAccessoryRight(function() {});
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_requestAccessoryRight_005
     * @tc.desc:verify requestAccessoryRight with date param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_requestAccessoryRight_005", 0, () => {
        console.info('----------------------UsbAccessory_neg_requestAccessoryRight_005---------------------------');
        try {
            usbManager.requestAccessoryRight(new Date());
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_hasAccessoryRight_001
     * @tc.desc:verify hasAccessoryRight with number param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_hasAccessoryRight_001", 0, () => {
        console.info('----------------------UsbAccessory_neg_hasAccessoryRight_001---------------------------');
        try {
            usbManager.hasAccessoryRight(456);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_hasAccessoryRight_002
     * @tc.desc:verify hasAccessoryRight with boolean param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_hasAccessoryRight_002", 0, () => {
        console.info('----------------------UsbAccessory_neg_hasAccessoryRight_002---------------------------');
        try {
            usbManager.hasAccessoryRight(false);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_hasAccessoryRight_003
     * @tc.desc:verify hasAccessoryRight with array param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_hasAccessoryRight_003", 0, () => {
        console.info('----------------------UsbAccessory_neg_hasAccessoryRight_003---------------------------');
        try {
            usbManager.hasAccessoryRight([]);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_cancelAccessoryRight_001
     * @tc.desc:verify cancelAccessoryRight with number param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_cancelAccessoryRight_001", 0, () => {
        console.info('----------------------UsbAccessory_neg_cancelAccessoryRight_001---------------------------');
        try {
            usbManager.cancelAccessoryRight(789);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_cancelAccessoryRight_002
     * @tc.desc:verify cancelAccessoryRight with boolean param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_cancelAccessoryRight_002", 0, () => {
        console.info('----------------------UsbAccessory_neg_cancelAccessoryRight_002---------------------------');
        try {
            usbManager.cancelAccessoryRight(true);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_cancelAccessoryRight_003
     * @tc.desc:verify cancelAccessoryRight with array param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_cancelAccessoryRight_003", 0, () => {
        console.info('----------------------UsbAccessory_neg_cancelAccessoryRight_003---------------------------');
        try {
            usbManager.cancelAccessoryRight([1, 2]);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_addAccessoryRight_001
     * @tc.desc:verify addAccessoryRight with string tokenId
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_addAccessoryRight_001", 0, () => {
        console.info('----------------------UsbAccessory_neg_addAccessoryRight_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                usbManager.addAccessoryRight("token", list[0]);
                expect(false).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_addAccessoryRight_002
     * @tc.desc:verify addAccessoryRight with boolean tokenId
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_addAccessoryRight_002", 0, () => {
        console.info('----------------------UsbAccessory_neg_addAccessoryRight_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                usbManager.addAccessoryRight(true, list[0]);
                expect(false).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_addAccessoryRight_003
     * @tc.desc:verify addAccessoryRight with array tokenId
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_addAccessoryRight_003", 0, () => {
        console.info('----------------------UsbAccessory_neg_addAccessoryRight_003---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                usbManager.addAccessoryRight([1, 2], list[0]);
                expect(false).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_addAccessoryRight_004
     * @tc.desc:verify addAccessoryRight with object tokenId
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_addAccessoryRight_004", 0, () => {
        console.info('----------------------UsbAccessory_neg_addAccessoryRight_004---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                usbManager.addAccessoryRight({}, list[0]);
                expect(false).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_addAccessoryRight_005
     * @tc.desc:verify addAccessoryRight with null tokenId
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_addAccessoryRight_005", 0, () => {
        console.info('----------------------UsbAccessory_neg_addAccessoryRight_005---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                usbManager.addAccessoryRight(null, list[0]);
                expect(false).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_addAccessoryRight_006
     * @tc.desc:verify addAccessoryRight with string accessory
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_addAccessoryRight_006", 0, () => {
        console.info('----------------------UsbAccessory_neg_addAccessoryRight_006---------------------------');
        try {
            usbManager.addAccessoryRight(tokenId, "accessory");
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_addAccessoryRight_007
     * @tc.desc:verify addAccessoryRight with number accessory
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_addAccessoryRight_007", 0, () => {
        console.info('----------------------UsbAccessory_neg_addAccessoryRight_007---------------------------');
        try {
            usbManager.addAccessoryRight(tokenId, 123);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_addAccessoryRight_008
     * @tc.desc:verify addAccessoryRight with boolean accessory
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_addAccessoryRight_008", 0, () => {
        console.info('----------------------UsbAccessory_neg_addAccessoryRight_008---------------------------');
        try {
            usbManager.addAccessoryRight(tokenId, true);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_addAccessoryRight_009
     * @tc.desc:verify addAccessoryRight with array accessory
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_addAccessoryRight_009", 0, () => {
        console.info('----------------------UsbAccessory_neg_addAccessoryRight_009---------------------------');
        try {
            usbManager.addAccessoryRight(tokenId, [1, 2, 3]);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_openAccessory_001
     * @tc.desc:verify openAccessory with number param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_openAccessory_001", 0, () => {
        console.info('----------------------UsbAccessory_neg_openAccessory_001---------------------------');
        try {
            usbManager.openAccessory(123);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_openAccessory_002
     * @tc.desc:verify openAccessory with boolean param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_openAccessory_002", 0, () => {
        console.info('----------------------UsbAccessory_neg_openAccessory_002---------------------------');
        try {
            usbManager.openAccessory(false);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_openAccessory_003
     * @tc.desc:verify openAccessory with string param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_openAccessory_003", 0, () => {
        console.info('----------------------UsbAccessory_neg_openAccessory_003---------------------------');
        try {
            usbManager.openAccessory("accessory");
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_openAccessory_004
     * @tc.desc:verify openAccessory with array param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_openAccessory_004", 0, () => {
        console.info('----------------------UsbAccessory_neg_openAccessory_004---------------------------');
        try {
            usbManager.openAccessory([1, 2, 3]);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_closeAccessory_001
     * @tc.desc:verify closeAccessory with number param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_closeAccessory_001", 0, () => {
        console.info('----------------------UsbAccessory_neg_closeAccessory_001---------------------------');
        try {
            usbManager.closeAccessory(123);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_closeAccessory_002
     * @tc.desc:verify closeAccessory with boolean param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_closeAccessory_002", 0, () => {
        console.info('----------------------UsbAccessory_neg_closeAccessory_002---------------------------');
        try {
            usbManager.closeAccessory(true);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_closeAccessory_003
     * @tc.desc:verify closeAccessory with string param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_closeAccessory_003", 0, () => {
        console.info('----------------------UsbAccessory_neg_closeAccessory_003---------------------------');
        try {
            usbManager.closeAccessory("handle");
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_closeAccessory_004
     * @tc.desc:verify closeAccessory with array param
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_closeAccessory_004", 0, () => {
        console.info('----------------------UsbAccessory_neg_closeAccessory_004---------------------------');
        try {
            usbManager.closeAccessory([1, 2, 3]);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_closeAccessory_005
     * @tc.desc:verify closeAccessory with invalid fd
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_closeAccessory_005", 0, () => {
        console.info('----------------------UsbAccessory_neg_closeAccessory_005---------------------------');
        try {
            let handle = {"accessFd": 999999};
            usbManager.closeAccessory(handle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_closeAccessory_006
     * @tc.desc:verify closeAccessory with negative fd
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_closeAccessory_006", 0, () => {
        console.info('----------------------UsbAccessory_neg_closeAccessory_006---------------------------');
        try {
            let handle = {"accessFd": -999};
            usbManager.closeAccessory(handle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_handle_001
     * @tc.desc:verify handle with missing accessFd
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_handle_001", 0, () => {
        console.info('----------------------UsbAccessory_neg_handle_001---------------------------');
        try {
            let handle = {};
            usbManager.closeAccessory(handle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_handle_002
     * @tc.desc:verify handle with wrong field name
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_handle_002", 0, () => {
        console.info('----------------------UsbAccessory_neg_handle_002---------------------------');
        try {
            let handle = {"fd": 123};
            usbManager.closeAccessory(handle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_handle_003
     * @tc.desc:verify handle with extra fields
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_handle_003", 0, () => {
        console.info('----------------------UsbAccessory_neg_handle_003---------------------------');
        try {
            let handle = {"accessFd": 123, "extra": "value"};
            usbManager.closeAccessory(handle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_handle_004
     * @tc.desc:verify handle with null accessFd
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_handle_004", 0, () => {
        console.info('----------------------UsbAccessory_neg_handle_004---------------------------');
        try {
            let handle = {"accessFd": null};
            usbManager.closeAccessory(handle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_handle_005
     * @tc.desc:verify handle with undefined accessFd
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_handle_005", 0, () => {
        console.info('----------------------UsbAccessory_neg_handle_005---------------------------');
        try {
            let handle = {"accessFd": undefined};
            usbManager.closeAccessory(handle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_handle_006
     * @tc.desc:verify handle with NaN accessFd
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_handle_006", 0, () => {
        console.info('----------------------UsbAccessory_neg_handle_006---------------------------');
        try {
            let handle = {"accessFd": NaN};
            usbManager.closeAccessory(handle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_handle_007
     * @tc.desc:verify handle with Infinity accessFd
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_handle_007", 0, () => {
        console.info('----------------------UsbAccessory_neg_handle_007---------------------------');
        try {
            let handle = {"accessFd": Infinity};
            usbManager.closeAccessory(handle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_handle_008
     * @tc.desc:verify handle with float accessFd
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_handle_008", 0, () => {
        console.info('----------------------UsbAccessory_neg_handle_008---------------------------');
        try {
            let handle = {"accessFd": 1.5};
            usbManager.closeAccessory(handle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_handle_009
     * @tc.desc:verify handle with string accessFd
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_handle_009", 0, () => {
        console.info('----------------------UsbAccessory_neg_handle_009---------------------------');
        try {
            let handle = {"accessFd": "123"};
            usbManager.closeAccessory(handle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_handle_010
     * @tc.desc:verify handle with boolean accessFd
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_handle_010", 0, () => {
        console.info('----------------------UsbAccessory_neg_handle_010---------------------------');
        try {
            let handle = {"accessFd": true};
            usbManager.closeAccessory(handle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_handle_011
     * @tc.desc:verify handle with array accessFd
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_handle_011", 0, () => {
        console.info('----------------------UsbAccessory_neg_handle_011---------------------------');
        try {
            let handle = {"accessFd": [1, 2, 3]};
            usbManager.closeAccessory(handle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_handle_012
     * @tc.desc:verify handle with object accessFd
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_handle_012", 0, () => {
        console.info('----------------------UsbAccessory_neg_handle_012---------------------------');
        try {
            let handle = {"accessFd": {"fd": 123}};
            usbManager.closeAccessory(handle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_sequence_001
     * @tc.desc:verify cancel without request
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_sequence_001", 0, () => {
        console.info('----------------------UsbAccessory_neg_sequence_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.cancelAccessoryRight(list[0]);
                } catch (err) {
                    expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                }
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_sequence_002
     * @tc.desc:verify close without open
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_sequence_002", 0, () => {
        console.info('----------------------UsbAccessory_neg_sequence_002---------------------------');
        try {
            let handle = {"accessFd": 123};
            usbManager.closeAccessory(handle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_sequence_003
     * @tc.desc:verify open without add right
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_sequence_003", 0, () => {
        console.info('----------------------UsbAccessory_neg_sequence_003---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    let handle = usbManager.openAccessory(list[0]);
                    usbManager.closeAccessory(handle);
                } catch (err) {
                    expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                }
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_sequence_004
     * @tc.desc:verify double open
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_sequence_004", 0, () => {
        console.info('----------------------UsbAccessory_neg_sequence_004---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.addAccessoryRight(tokenId, list[0]);
                    let handle1 = usbManager.openAccessory(list[0]);
                    let handle2 = usbManager.openAccessory(list[0]);
                    usbManager.closeAccessory(handle1);
                    usbManager.closeAccessory(handle2);
                } catch (err) {
                    expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                }
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_neg_sequence_005
     * @tc.desc:verify double close
     * @tc.type: FUNC
     */
    it("UsbAccessory_neg_sequence_005", 0, () => {
        console.info('----------------------UsbAccessory_neg_sequence_005---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.addAccessoryRight(tokenId, list[0]);
                    let handle = usbManager.openAccessory(list[0]);
                    usbManager.closeAccessory(handle);
                    usbManager.closeAccessory(handle);
                } catch (err) {
                    expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                }
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })
})