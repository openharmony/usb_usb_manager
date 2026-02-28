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

describe("UsbAccessoryJsTestExtend", function () {
    let bundleInfo = null;
    let tokenId = 0;
    let accList = [];
    let accessoryHandle = null;

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
        if (accessoryHandle != null) {
            try {
                usbManager.closeAccessory(accessoryHandle);
            } catch (err) {
                console.error("afterAll closeAccessory error: " + JSON.stringify(err))
            }
        }
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
     * @tc.name:UsbAccessory_getAccessoryList_boundary_001
     * @tc.desc:verify getAccessoryList with empty list scenario
     * @tc.type: FUNC
     */
    it("UsbAccessory_getAccessoryList_boundary_001", 0, () => {
        console.info('----------------------UsbAccessory_getAccessoryList_boundary_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            expect(list != null).assertEqual(true);
            expect(Array.isArray(list)).assertEqual(true);
            expect(list.length >= 0).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_getAccessoryList_boundary_002
     * @tc.desc:verify getAccessoryList return type
     * @tc.type: FUNC
     */
    it("UsbAccessory_getAccessoryList_boundary_002", 0, () => {
        console.info('----------------------UsbAccessory_getAccessoryList_boundary_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            expect(typeof list).assertEqual('object');
            expect(list.constructor === Array).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_getAccessoryList_boundary_003
     * @tc.desc:verify getAccessoryList multiple calls
     * @tc.type: FUNC
     */
    it("UsbAccessory_getAccessoryList_boundary_003", 0, () => {
        console.info('----------------------UsbAccessory_getAccessoryList_boundary_003---------------------------');
        try {
            let list1 = usbManager.getAccessoryList();
            let list2 = usbManager.getAccessoryList();
            let list3 = usbManager.getAccessoryList();
            expect(list1 != null).assertEqual(true);
            expect(list2 != null).assertEqual(true);
            expect(list3 != null).assertEqual(true);
            expect(list1.length).assertEqual(list2.length);
            expect(list2.length).assertEqual(list3.length);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_requestAccessoryRight_boundary_001
     * @tc.desc:verify requestAccessoryRight with empty object
     * @tc.type: FUNC
     */
    it("UsbAccessory_requestAccessoryRight_boundary_001", 0, () => {
        console.info('----------------------UsbAccessory_requestAccessoryRight_boundary_001---------------------------');
        try {
            let emptyAccess = {};
            usbManager.requestAccessoryRight(emptyAccess);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_requestAccessoryRight_boundary_002
     * @tc.desc:verify requestAccessoryRight with partial fields
     * @tc.type: FUNC
     */
    it("UsbAccessory_requestAccessoryRight_boundary_002", 0, () => {
        console.info('----------------------UsbAccessory_requestAccessoryRight_boundary_002---------------------------');
        try {
            let partialAccess = {"manufacturer": "test"};
            usbManager.requestAccessoryRight(partialAccess);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_requestAccessoryRight_boundary_003
     * @tc.desc:verify requestAccessoryRight with all fields
     * @tc.type: FUNC
     */
    it("UsbAccessory_requestAccessoryRight_boundary_003", 0, () => {
        console.info('----------------------UsbAccessory_requestAccessoryRight_boundary_003---------------------------');
        try {
            let fullAccess = {
                "manufacturer": "testManu",
                "model": "testModel",
                "description": "testDesc",
                "version": "1.0",
                "uri": "test://uri",
                "serial": "testSerial"
            };
            usbManager.requestAccessoryRight(fullAccess);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_requestAccessoryRight_boundary_004
     * @tc.desc:verify requestAccessoryRight with string fields
     * @tc.type: FUNC
     */
    it("UsbAccessory_requestAccessoryRight_boundary_004", 0, () => {
        console.info('----------------------UsbAccessory_requestAccessoryRight_boundary_004---------------------------');
        try {
            let stringAccess = {"manufacturer": ""};
            usbManager.requestAccessoryRight(stringAccess);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_requestAccessoryRight_boundary_005
     * @tc.desc:verify requestAccessoryRight with long string
     * @tc.type: FUNC
     */
    it("UsbAccessory_requestAccessoryRight_boundary_005", 0, () => {
        console.info('----------------------UsbAccessory_requestAccessoryRight_boundary_005---------------------------');
        try {
            let longString = "a".repeat(1000);
            let longAccess = {"manufacturer": longString};
            usbManager.requestAccessoryRight(longAccess);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_requestAccessoryRight_boundary_006
     * @tc.desc:verify requestAccessoryRight with special characters
     * @tc.type: FUNC
     */
    it("UsbAccessory_requestAccessoryRight_boundary_006", 0, () => {
        console.info('----------------------UsbAccessory_requestAccessoryRight_boundary_006---------------------------');
        try {
            let specialAccess = {"manufacturer": "!@#$%^&*()_+-=[]{}|;':\",./<>?"};
            usbManager.requestAccessoryRight(specialAccess);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_requestAccessoryRight_boundary_007
     * @tc.desc:verify requestAccessoryRight with unicode characters
     * @tc.type: FUNC
     */
    it("UsbAccessory_requestAccessoryRight_boundary_007", 0, () => {
        console.info('----------------------UsbAccessory_requestAccessoryRight_boundary_007---------------------------');
        try {
            let unicodeAccess = {"manufacturer": "测试中文"};
            usbManager.requestAccessoryRight(unicodeAccess);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_requestAccessoryRight_boundary_008
     * @tc.desc:verify requestAccessoryRight with numeric string
     * @tc.type: FUNC
     */
    it("UsbAccessory_requestAccessoryRight_boundary_008", 0, () => {
        console.info('----------------------UsbAccessory_requestAccessoryRight_boundary_008---------------------------');
        try {
            let numericAccess = {"manufacturer": "123456"};
            usbManager.requestAccessoryRight(numericAccess);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_requestAccessoryRight_boundary_009
     * @tc.desc:verify requestAccessoryRight with whitespace
     * @tc.type: FUNC
     */
    it("UsbAccessory_requestAccessoryRight_boundary_009", 0, () => {
        console.info('----------------------UsbAccessory_requestAccessoryRight_boundary_009---------------------------');
        try {
            let whitespaceAccess = {"manufacturer": "   "};
            usbManager.requestAccessoryRight(whitespaceAccess);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_requestAccessoryRight_boundary_010
     * @tc.desc:verify requestAccessoryRight with newline
     * @tc.type: FUNC
     */
    it("UsbAccessory_requestAccessoryRight_boundary_010", 0, () => {
        console.info('----------------------UsbAccessory_requestAccessoryRight_boundary_010---------------------------');
        try {
            let newlineAccess = {"manufacturer": "test\nmanufacturer"};
            usbManager.requestAccessoryRight(newlineAccess);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_hasAccessoryRight_boundary_001
     * @tc.desc:verify hasAccessoryRight with empty object
     * @tc.type: FUNC
     */
    it("UsbAccessory_hasAccessoryRight_boundary_001", 0, () => {
        console.info('----------------------UsbAccessory_hasAccessoryRight_boundary_001---------------------------');
        try {
            let emptyAccess = {};
            usbManager.hasAccessoryRight(emptyAccess);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_hasAccessoryRight_boundary_002
     * @tc.desc:verify hasAccessoryRight with partial fields
     * @tc.type: FUNC
     */
    it("UsbAccessory_hasAccessoryRight_boundary_002", 0, () => {
        console.info('----------------------UsbAccessory_hasAccessoryRight_boundary_002---------------------------');
        try {
            let partialAccess = {"model": "test"};
            usbManager.hasAccessoryRight(partialAccess);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_hasAccessoryRight_boundary_003
     * @tc.desc:verify hasAccessoryRight return type
     * @tc.type: FUNC
     */
    it("UsbAccessory_hasAccessoryRight_boundary_003", 0, () => {
        console.info('----------------------UsbAccessory_hasAccessoryRight_boundary_003---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                let result = usbManager.hasAccessoryRight(list[0]);
                expect(typeof result).assertEqual('boolean');
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_hasAccessoryRight_boundary_004
     * @tc.desc:verify hasAccessoryRight multiple calls
     * @tc.type: FUNC
     */
    it("UsbAccessory_hasAccessoryRight_boundary_004", 0, () => {
        console.info('----------------------UsbAccessory_hasAccessoryRight_boundary_004---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                let result1 = usbManager.hasAccessoryRight(list[0]);
                let result2 = usbManager.hasAccessoryRight(list[0]);
                let result3 = usbManager.hasAccessoryRight(list[0]);
                expect(typeof result1).assertEqual('boolean');
                expect(typeof result2).assertEqual('boolean');
                expect(typeof result3).assertEqual('boolean');
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_cancelAccessoryRight_boundary_001
     * @tc.desc:verify cancelAccessoryRight with empty object
     * @tc.type: FUNC
     */
    it("UsbAccessory_cancelAccessoryRight_boundary_001", 0, () => {
        console.info('----------------------UsbAccessory_cancelAccessoryRight_boundary_001---------------------------');
        try {
            let emptyAccess = {};
            usbManager.cancelAccessoryRight(emptyAccess);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_cancelAccessoryRight_boundary_002
     * @tc.desc:verify cancelAccessoryRight with partial fields
     * @tc.type: FUNC
     */
    it("UsbAccessory_cancelAccessoryRight_boundary_002", 0, () => {
        console.info('----------------------UsbAccessory_cancelAccessoryRight_boundary_002---------------------------');
        try {
            let partialAccess = {"description": "test"};
            usbManager.cancelAccessoryRight(partialAccess);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_cancelAccessoryRight_boundary_003
     * @tc.desc:verify cancelAccessoryRight multiple times
     * @tc.type: FUNC
     */
    it("UsbAccessory_cancelAccessoryRight_boundary_003", 0, () => {
        console.info('----------------------UsbAccessory_cancelAccessoryRight_boundary_003---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.cancelAccessoryRight(list[0]);
                    usbManager.cancelAccessoryRight(list[0]);
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
     * @tc.name:UsbAccessory_addAccessoryRight_boundary_001
     * @tc.desc:verify addAccessoryRight with zero tokenId
     * @tc.type: FUNC
     */
    it("UsbAccessory_addAccessoryRight_boundary_001", 0, () => {
        console.info('----------------------UsbAccessory_addAccessoryRight_boundary_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                usbManager.addAccessoryRight(0, list[0]);
                expect(false).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_addAccessoryRight_boundary_002
     * @tc.desc:verify addAccessoryRight with max tokenId
     * @tc.type: FUNC
     */
    it("UsbAccessory_addAccessoryRight_boundary_002", 0, () => {
        console.info('----------------------UsbAccessory_addAccessoryRight_boundary_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                usbManager.addAccessoryRight(Number.MAX_SAFE_INTEGER, list[0]);
                expect(false).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_addAccessoryRight_boundary_003
     * @tc.desc:verify addAccessoryRight with min tokenId
     * @tc.type: FUNC
     */
    it("UsbAccessory_addAccessoryRight_boundary_003", 0, () => {
        console.info('----------------------UsbAccessory_addAccessoryRight_boundary_003---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                usbManager.addAccessoryRight(Number.MIN_SAFE_INTEGER, list[0]);
                expect(false).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_addAccessoryRight_boundary_004
     * @tc.desc:verify addAccessoryRight with float tokenId
     * @tc.type: FUNC
     */
    it("UsbAccessory_addAccessoryRight_boundary_004", 0, () => {
        console.info('----------------------UsbAccessory_addAccessoryRight_boundary_004---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                usbManager.addAccessoryRight(1.5, list[0]);
                expect(false).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_addAccessoryRight_boundary_005
     * @tc.desc:verify addAccessoryRight with string tokenId
     * @tc.type: FUNC
     */
    it("UsbAccessory_addAccessoryRight_boundary_005", 0, () => {
        console.info('----------------------UsbAccessory_addAccessoryRight_boundary_005---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                usbManager.addAccessoryRight("123", list[0]);
                expect(false).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_addAccessoryRight_boundary_006
     * @tc.desc:verify addAccessoryRight with empty accessory
     * @tc.type: FUNC
     */
    it("UsbAccessory_addAccessoryRight_boundary_006", 0, () => {
        console.info('----------------------UsbAccessory_addAccessoryRight_boundary_006---------------------------');
        try {
            usbManager.addAccessoryRight(tokenId, {});
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_addAccessoryRight_boundary_007
     * @tc.desc:verify addAccessoryRight multiple times
     * @tc.type: FUNC
     */
    it("UsbAccessory_addAccessoryRight_boundary_007", 0, () => {
        console.info('----------------------UsbAccessory_addAccessoryRight_boundary_007---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                for (let i = 0; i < 50; i++) {
                    try {
                        usbManager.addAccessoryRight(tokenId, list[0]);
                    } catch (err) {
                        expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                    }
                }
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_openAccessory_boundary_001
     * @tc.desc:verify openAccessory with empty object
     * @tc.type: FUNC
     */
    it("UsbAccessory_openAccessory_boundary_001", 0, () => {
        console.info('----------------------UsbAccessory_openAccessory_boundary_001---------------------------');
        try {
            let emptyAccess = {};
            usbManager.openAccessory(emptyAccess);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_openAccessory_boundary_002
     * @tc.desc:verify openAccessory return type
     * @tc.type: FUNC
     */
    it("UsbAccessory_openAccessory_boundary_002", 0, () => {
        console.info('----------------------UsbAccessory_openAccessory_boundary_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    let handle = usbManager.openAccessory(list[0]);
                    expect(typeof handle).assertEqual('object');
                    expect(handle != null).assertEqual(true);
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
     * @tc.name:UsbAccessory_openAccessory_boundary_003
     * @tc.desc:verify openAccessory handle structure
     * @tc.type: FUNC
     */
    it("UsbAccessory_openAccessory_boundary_003", 0, () => {
        console.info('----------------------UsbAccessory_openAccessory_boundary_003---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    let handle = usbManager.openAccessory(list[0]);
                    expect('accessFd' in handle).assertEqual(true);
                    expect(typeof handle.accessFd).assertEqual('number');
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
     * @tc.name:UsbAccessory_closeAccessory_boundary_001
     * @tc.desc:verify closeAccessory with empty object
     * @tc.type: FUNC
     */
    it("UsbAccessory_closeAccessory_boundary_001", 0, () => {
        console.info('----------------------UsbAccessory_closeAccessory_boundary_001---------------------------');
        try {
            let emptyHandle = {};
            usbManager.closeAccessory(emptyHandle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_closeAccessory_boundary_002
     * @tc.desc:verify closeAccessory with zero fd
     * @tc.type: FUNC
     */
    it("UsbAccessory_closeAccessory_boundary_002", 0, () => {
        console.info('----------------------UsbAccessory_closeAccessory_boundary_002---------------------------');
        try {
            let zeroHandle = {"accessFd": 0};
            usbManager.closeAccessory(zeroHandle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_closeAccessory_boundary_003
     * @tc.desc:verify closeAccessory with max fd
     * @tc.type: FUNC
     */
    it("UsbAccessory_closeAccessory_boundary_003", 0, () => {
        console.info('----------------------UsbAccessory_closeAccessory_boundary_003---------------------------');
        try {
            let maxHandle = {"accessFd": Number.MAX_SAFE_INTEGER};
            usbManager.closeAccessory(maxHandle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_closeAccessory_boundary_004
     * @tc.desc:verify closeAccessory with negative fd
     * @tc.type: FUNC
     */
    it("UsbAccessory_closeAccessory_boundary_004", 0, () => {
        console.info('----------------------UsbAccessory_closeAccessory_boundary_004---------------------------');
        try {
            let negHandle = {"accessFd": -100};
            usbManager.closeAccessory(negHandle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc:UsbAccessory_closeAccessory_boundary_005
     * @tc.desc:verify closeAccessory with string fd
     * @tc.type: FUNC
     */
    it("UsbAccessory_closeAccessory_boundary_005", 0, () => {
        console.info('----------------------UsbAccessory_closeAccessory_boundary_005---------------------------');
        try {
            let strHandle = {"accessFd": "123"};
            usbManager.closeAccessory(strHandle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_closeAccessory_boundary_006
     * @tc.desc:verify closeAccessory with float fd
     * @tc.type: FUNC
     */
    it("UsbAccessory_closeAccessory_boundary_006", 0, () => {
        console.info('----------------------UsbAccessory_closeAccessory_boundary_006---------------------------');
        try {
            let floatHandle = {"accessFd": 1.5};
            usbManager.closeAccessory(floatHandle);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_sequence_001
     * @tc.desc:verify request then has right sequence
     * @tc.type: FUNC
     */
    it("UsbAccessory_sequence_001", 0, () => {
        console.info('----------------------UsbAccessory_sequence_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.requestAccessoryRight(list[0]);
                    let hasRight = usbManager.hasAccessoryRight(list[0]);
                    expect(typeof hasRight).assertEqual('boolean');
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
     * @tc.name:UsbAccessory_sequence_002
     * @tc.desc:verify request then cancel sequence
     * @tc.type: FUNC
     */
    it("UsbAccessory_sequence_002", 0, () => {
        console.info('----------------------UsbAccessory_sequence_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.requestAccessoryRight(list[0]);
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
     * @tc.name:UsbAccessory_sequence_003
     * @tc.desc:verify add then open sequence
     * @tc.type: FUNC
     */
    it("UsbAccessory_sequence_003", 0, () => {
        console.info('----------------------UsbAccessory_sequence_003---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.addAccessoryRight(tokenId, list[0]);
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
     * @tc.name:UsbAccessory_sequence_004
     * @tc.desc:verify open then close sequence
     * @tc.type: FUNC
     */
    it("UsbAccessory_sequence_004", 0, () => {
        console.info('----------------------UsbAccessory_sequence_004---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.addAccessoryRight(tokenId, list[0]);
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
     * @tc.name:UsbAccessory_sequence_005
     * @tc.desc:verify full lifecycle sequence
     * @tc.type: FUNC
     */
    it("UsbAccessory_sequence_005", 0, () => {
        console.info('----------------------UsbAccessory_sequence_005---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.requestAccessoryRight(list[0]);
                    let hasRight1 = usbManager.hasAccessoryRight(list[0]);
                    usbManager.cancelAccessoryRight(list[0]);
                    let hasRight2 = usbManager.hasAccessoryRight(list[0]);
                    usbManager.addAccessoryRight(tokenId, list[0]);
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
     * @tc.name:UsbAccessory_stress_001
     * @tc.desc:verify getAccessoryList stress test
     * @tc.type: FUNC
     */
    it("UsbAccessory_stress_001", 0, () => {
        console.info('----------------------UsbAccessory_stress_001---------------------------');
        try {
            for (let i = 0; i < 100; i++) {
                let list = usbManager.getAccessoryList();
                expect(list != null).assertEqual(true);
                expect(Array.isArray(list)).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_stress_002
     * @tc.desc:verify hasAccessoryRight stress test
     * @tc.type: FUNC
     */
    it("UsbAccessory_stress_002", 0, () => {
        console.info('----------------------UsbAccessory_stress_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                for (let i = 0; i < 100; i++) {
                    let result = usbManager.hasAccessoryRight(list[0]);
                    expect(typeof result).assertEqual('boolean');
                }
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_stress_003
     * @tc.desc:verify request/cancel stress test
     * @tc.type: FUNC
     */
    it("UsbAccessory_stress_003", 0, () => {
        console.info('----------------------UsbAccessory_stress_003---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                for (let i = 0; i < 50; i++) {
                    try {
                        usbManager.requestAccessoryRight(list[0]);
                        usbManager.cancelAccessoryRight(list[0]);
                    } catch (err) {
                        expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                    }
                }
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_stress_004
     * @tc.desc:verify addAccessoryRight stress test
     * @tc.type: FUNC
     */
    it("UsbAccessory_stress_004", 0, () => {
        console.info('----------------------UsbAccessory_stress_004---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                for (let i = 0; i < 50; i++) {
                    try {
                        usbManager.addAccessoryRight(tokenId, list[0]);
                    } catch (err) {
                        expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                    }
                }
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_stress_005
     * @tc.desc:verify open/close stress test
     * @tc.type: FUNC
     */
    it("UsbAccessory_stress_005", 0, () => {
        console.info('----------------------UsbAccessory_stress_005---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.addAccessoryRight(tokenId, list[0]);
                    for (let i = 0; i < 50; i++) {
                        try {
                            let handle = usbManager.openAccessory(list[0]);
                            usbManager.closeAccessory(handle);
                        } catch (err) {
                            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                        }
                    }
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
     * @tc.name:UsbAccessory_edge_001
     * @tc.desc:verify with manufacturer only
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_001", 0, () => {
        console.info('----------------------UsbAccessory_edge_001---------------------------');
        try {
            let access = {"manufacturer": "test"};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_002
     * @tc.desc:verify with model only
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_002", 0, () => {
        console.info('----------------------UsbAccessory_edge_002---------------------------');
        try {
            let access = {"model": "test"};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_003
     * @tc.desc:verify with description only
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_003", 0, () => {
        console.info('----------------------UsbAccessory_edge_003---------------------------');
        try {
            let access = {"description": "test"};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_004
     * @tc.desc:verify with version only
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_004", 0, () => {
        console.info('----------------------UsbAccessory_edge_004---------------------------');
        try {
            let access = {"version": "1.0"};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_005
     * @tc.desc:verify with uri only
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_005", 0, () => {
        console.info('----------------------UsbAccessory_edge_005---------------------------');
        try {
            let access = {"uri": "test://uri"};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_006
     * @tc.desc:verify with serial only
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_006", 0, () => {
        console.info('----------------------UsbAccessory_edge_006---------------------------');
        try {
            let access = {"serial": "test"};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_007
     * @tc.desc:verify with manufacturer and model
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_007", 0, () => {
        console.info('----------------------UsbAccessory_edge_007---------------------------');
        try {
            let access = {"manufacturer": "test", "model": "test"};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_008
     * @tc.desc:verify with all fields except one
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_008", 0, () => {
        console.info('----------------------UsbAccessory_edge_008---------------------------');
        try {
            let access = {
                "manufacturer": "test",
                "model": "test",
                "description": "test",
                "version": "1.0",
                "uri": "test://uri"
            };
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual()SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_009
     * @tc.desc:verify with numeric version
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_009", 0, () => {
        console.info('----------------------UsbAccessory_edge_009---------------------------');
        try {
            let access = {"manufacturer": "test", "version": 123};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_010
     * @tc.desc:verify with boolean field
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_010", 0, () => {
        console.info('----------------------------UsbAccessory_edge_010---------------------------');
        try {
            let access = {"manufacturer": "test", "enabled": true};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_011
     * @tc.desc:verify with array field
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_011", 0, () => {
        console.info('----------------------UsbAccessory_edge_011---------------------------');
        try {
            let access = {"manufacturer": "test", "tags": ["tag1", "tag2"]};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_012
     * @tc.desc:verify with nested object
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_012", 0, () => {
        console.info('----------------------UsbAccessory_edge_012---------------------------');
        try {
            let access = {"manufacturer": "test", "config": {"key": "value"}};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_013
     * @tc.desc:verify with null field value
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_013", 0, () => {
        console.info('----------------------UsbAccessory_edge_013---------------------------');
        try {
            let access = {"manufacturer": null};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_014
     * @tc.desc:verify with undefined field value
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_014", 0, () => {
        console.info('----------------------UsbAccessory_edge_014---------------------------');
        try {
            let access = {"manufacturer": undefined};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_015
     * @tc.desc:verify with NaN field value
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_015", 0, () => {
        console.info('----------------------UsbAccessory_edge_015---------------------------');
        try {
            let access = {"manufacturer": NaN};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_016
     * @tc.desc:verify with Infinity field value
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_016", 0, () => {
        console.info('----------------------UsbAccessory_edge_016---------------------------');
        try {
            let access = {"manufacturer": Infinity};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_017
     * @tc.desc:verify with date object
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_017", 0, () => {
        console.info('----------------------UsbAccessory_edge_017---------------------------');
        try {
            let access = {"manufacturer": new Date()};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_018
     * @tc.desc:verify with function field
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_018", 0, () => {
        console.info('----------------------UsbAccessory_edge_018---------------------------');
        try {
            let access = {"manufacturer": "test", "callback": function() {}};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_019
     * @tc.desc:verify with symbol field
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_019", 0, () => {
        console.info('----------------------UsbAccessory_edge_019---------------------------');
        try {
            let access = {"manufacturer": "test", "symbol": Symbol("test")};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_020
     * @tc.desc:verify with regex field
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_020", 0, () => {
        console.info('----------------------UsbAccessory_edge_020---------------------------');
        try {
            let access = {"manufacturer": "test", "pattern": /test/};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_021
     * @tc.desc:verify with extra fields
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_021", 0, () => {
        console.info('----------------------UsbAccessory_edge_021---------------------------');
        try {
            let access = {
                "manufacturer": "test",
                "extra1": "value1",
                "extra2": "value2",
                "extra3": "value3"
            };
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_022
     * @tc.desc:verify with reserved field names
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_022", 0, () => {
        console.info('----------------------UsbAccessory_edge_022---------------------------');
        try {
            let access = {
                "manufacturer": "test",
                "constructor": "test",
                "prototype": "test",
                "__proto__": "test"
            };
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
            } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_023
     * @tc.desc:verify with very long field name
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_023", 0, () => {
        console.info('----------------------UsbAccessory_edge_023---------------------------');
        try {
            let longName = "a".repeat(1000);
            let access = {};
            access[longName] = "test";
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_024
     * @tc.desc:verify with special field names
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_024", 0, () => {
        console.info('----------------------UsbAccessory_edge_024---------------------------');
        try {
            let access = {
                "": "empty",
                " ": "space",
                "\t": "tab",
                "\n": "newline"
            };
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_025
     * @tc.desc:verify with number field name
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_025", 0, () => {
        console.info('----------------------UsbAccessory_edge_025---------------------------');
        try {
            let access = {};
            access[123] = "test";
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_026
     * @tc.desc:verify with frozen object
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_026", 0, () => {
        console.info('----------------------UsbAccessory_edge_026---------------------------');
        try {
            let access = {"manufacturer": "test"};
            Object.freeze(access);
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_027
     * @tc.desc:verify with sealed object
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_027", 0, () => {
        console.info('----------------------UsbAccessory_edge_027---------------------------');
        try {
            let access = {"manufacturer": "test"};
            Object.seal(access);
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_028
     * @tc.desc:verify with non-extensible object
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_028", 0, () => {
        console.info('----------------------UsbAccessory_edge_028---------------------------');
        try {
            let access = {"manufacturer": "test"};
            Object.preventExtensions(access);
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_029
     * @tc.desc:verify with object prototype
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_029", 0, () => {
        console.info('----------------------UsbAccessory_edge_029---------------------------');
        try {
            let proto = {"manufacturer": "test"};
            let access = Object.create(proto);
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_030
     * @tc.desc:verify with getter/setter
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_030", 0, () => {
        console.info('----------------------UsbAccessory_edge_030---------------------------');
        try {
            let access = {
                get manufacturer() { return "test"; },
                set manufacturer(value) {}
            };
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_031
     * @tc.desc:verify with readonly property
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_031", 0, () => {
        console.info('----------------------UsbAccessory_edge_031---------------------------');
        try {
            let access = {};
            Object.defineProperty(access, "manufacturer", {
                value: "test",
                writable: false,
                configurable: true
            });
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_032
     * @tc.desc:verify with non-enumerable property
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_032", 0, () => {
        console.info('----------------------UsbAccessory_edge_032---------------------------');
        try {
            let access = {};
            Object.defineProperty(access, "manufacturer", {
                value: "test",
                enumerable: false,
                configurable: true
            });
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_033
     * @tc.desc:verify with non-configurable property
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_033", 0, () => {
        console.info('----------------------UsbAccessory_edge_033---------------------------');
        try {
            let access = {};
            Object.defineProperty(access, "manufacturer", {
                value: "test",
                configurable: false
            });
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_edge_034
     * @tc.desc:verify with array as accessory
     * @tc.type: FUNC
     */
    it("UsbAccessory_edge_034", 0, () => {
        console.info('----------------------UsbAccessory_edge_034---------------------------');
        try {
            let access = ["manufacturer", "test"];
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(PARAM_EXCEPTION_CODE);
        }
    })
})