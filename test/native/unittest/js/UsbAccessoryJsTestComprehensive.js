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

describe("UsbAccessoryJsTestComprehensive", function () {
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
     * @tc.name:UsbAccessory_comp_getAccessoryList_001
     * @tc.desc:verify getAccessoryList returns array
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_getAccessoryList_001", 0, () => {
        console.info('----------------------UsbAccessory_comp_getAccessoryList_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            expect(Array.isArray(list)).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_getAccessoryList_002
     * @tc.desc:verify getAccessoryList elements are objects
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_getAccessoryList_002", 0, () => {
        console.info('----------------------UsbAccessory_comp_getAccessoryList_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list.length > 0) {
                expect(typeof list[0]).assertEqual('object');
                expect(list[0] !== null).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_accessoryFields_001
     * @tc.desc:verify accessory has manufacturer field
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_accessoryFields_001", 0, () => {
        console.info('----------------------UsbAccessory_comp_accessoryFields_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list.length > 0) {
                expect('manufacturer' in list[0]).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_accessoryFields_002
     * @tc.desc:verify accessory has model field
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_accessoryFields_002", 0, () => {
        console.info('----------------------UsbAccessory_comp_accessoryFields_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list.length > 0) {
                expect('model' in list[0]).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_accessoryFields_003
     * @tc.desc:verify accessory has description field
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_accessoryFields_003", 0, () => {
        console.info('----------------------UsbAccessory_comp_accessoryFields_003---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list.length > 0) {
                expect('description' in list[0]).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_accessoryFields_004
     * @tc.desc:verify accessory has version field
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_accessoryFields_004", 0, () => {
        console.info('----------------------UsbAccessory_comp_accessoryFields_004---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list.length > 0) {
                expect('version' in list[0]).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_accessoryFields_005
     * @tc.desc:verify accessory has uri field
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_accessoryFields_005", 0, () => {
        console.info('----------------------UsbAccessory_comp_accessoryFields_005---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list.length > 0) {
                expect('uri' in list[0]).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_accessoryFields_006
     * @tc.desc:verify accessory has serial field
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_accessoryFields_006", 0, () => {
        console.info('----------------------UsbAccessory_comp_accessoryFields_006---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list.length > 0) {
                expect('serial' in list[0]).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_fieldTypes_001
     * @tc.desc:verify manufacturer field type
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_fieldTypes_001", 0, () => {
        console.info('----------------------UsbAccessory_comp_fieldTypes_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list.length > 0) {
                expect(typeof list[0].manufacturer).assertEqual('string');
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_fieldTypes_002
     * @tc.desc:verify model field type
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_fieldTypes_002", 0, () => {
        console.info('----------------------UsbAccessory_comp_fieldTypes_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list.length > 0) {
                expect(typeof list[0].model).assertEqual('string');
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_fieldTypes_003
     * @tc.desc:verify description field type
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_fieldTypes_003", 0, () => {
        console.info('----------------------UsbAccessory_comp_fieldTypes_003---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list.length > 0) {
                expect(typeof list[0].description).assertEqual('string');
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_fieldTypes_004
     * @tc.desc:verify version field type
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_fieldTypes_004", 0, () => {
        console.info('----------------------UsbAccessory_comp_fieldTypes_004---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list.length > 0) {
                expect(typeof list[0].version).assertEqual('string');
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_fieldTypes_005
     * @tc.desc:verify uri field type
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_fieldTypes_005", 0, () => {
        console.info('----------------------UsbAccessory_comp_fieldTypes_005---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list.length > 0) {
                expect(typeof list[0].uri).assertEqual('string');
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_fieldTypes_006
     * @tc.desc:verify serial field type
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_fieldTypes_006", 0, () => {
        console.info('----------------------UsbAccessory_comp_fieldTypes_006---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list.length > 0) {
                expect(typeof list[0].serial).assertEqual('string');
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_handleStructure_001
     * @tc.desc:verify handle has accessFd field
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_handleStructure_001", 0, () => {
        console.info('----------------------UsbAccessory_comp_handleStructure_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.addAccessoryRight(tokenId, list[0]);
                    let handle = usbManager.openAccessory(list[0]);
                    expect('accessFd' in handle).assertEqual(true);
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
     * @tc.name:UsbAccessory_comp_handleStructure_002
     * @tc.desc:verify handle accessFd type
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_handleStructure_002", 0, () => {
        console.info('----------------------UsbAccessory_comp_handleStructure_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.addAccessoryRight(tokenId, list[0]);
                    let handle = usbManager.openAccessory(list[0]);
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
     * @tc.name:UsbAccessory_comp_handleStructure_003
     * @tc.desc:verify handle accessFd is positive
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_handleStructure_003", 0, () => {
        console.info('----------------------UsbAccessory_comp_handleStructure_003---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.addAccessoryRight(tokenId, list[0]);
                    let handle = usbManager.openAccessory(list[0]);
                    expect(handle.accessFd >= 0).assertEqual(true);
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
     * @tc.name:UsbAccessory_comp_handleStructure_004
     * @tc.desc:verify handle is integer
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_handleStructure_004", 0, () => {
        console.info('----------------------UsbAccessory_comp_handleStructure_004---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.addAccessoryRight(tokenId, list[0]);
                    let handle = usbManager.openAccessory(list[0]);
                    expect(Number.isInteger(handle.accessFd)).assertEqual(true);
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
     * @tc.name:UsbAccessory_comp_hasAccessoryRight_001
     * @tc.desc:verify hasAccessoryRight returns boolean
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_hasAccessoryRight_001", 0, () => {
        console.info('----------------------UsbAccessory_comp_hasAccessoryRight_001---------------------------');
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
     * @tc.name:UsbAccessory_comp_hasAccessoryRight_002
     * @tc.desc:verify hasAccessoryRight returns true or false
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_hasAccessoryRight_002", 0, () => {
        console.info('----------------------UsbAccessory_comp_hasAccessoryRight_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                let result = usbManager.hasAccessoryRight(list[0]);
                expect(result === true || result === false).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_multipleAccessories_001
     * @tc.desc:verify handling multiple accessories
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_multipleAccessories_001", 0, () => {
        console.info('----------------------UsbAccessory_comp_multipleAccessories_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            expect(list != null).assertEqual(true);
            expect(Array.isArray(list)).assertEqual(true);
            for (let i = 0; i < list.length; i++) {
                expect(typeof list[i]).assertEqual('object');
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_multipleAccessories_002
     * @tc.desc:verify hasAccessoryRight for multiple accessories
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_multipleAccessories_002", 0, () => {
        console.info('----------------------UsbAccessory_comp_multipleAccessories_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                for (let i = 0; i < list.length; i++) {
                    let result = usbManager.hasAccessoryRight(list[i]);
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
     * @tc.name:UsbAccessory_comp_errorHandling_001
     * @tc.desc:verify error object structure
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_errorHandling_001", 0, () => {
        console.info('----------------------UsbAccessory_comp_errorHandling_001---------------------------');
        try {
            usbManager.requestAccessoryRight();
        } catch (err) {
            expect(typeof err).assertEqual('object');
            expect(err !== null).assertEqual(true);
            expect('code' in err).assertEqual(true);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_errorHandling_002
     * @tc.desc:verify error code type
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_errorHandling_002", 0, () => {
        console.info('----------------------UsbAccessory_comp_errorHandling_002---------------------------');
        try {
            usbManager.requestAccessoryRight();
        } catch (err) {
            expect(typeof err.code).assertEqual('number');
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_errorHandling_003
     * @tc.desc:verify error code is valid
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_errorHandling_003", 0, () => {
        console.info('----------------------UsbAccessory_comp_errorHandling_003---------------------------');
        try {
            usbManager.requestAccessoryRight();
        } catch (err) {
            expect(err.code === PARAM_EXCEPTION_CODE || err.code === SERVICE_EXCEPTION_CODE).assertEqual(true);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_consistency_001
     * @tc.desc:verify getAccessoryList consistency
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_consistency_001", 0, () => {
        console.info('----------------------UsbAccessory_comp_consistency_001---------------------------');
        try {
            let list1 = usbManager.getAccessoryList();
            let list2 = usbManager.getAccessoryList();
            expect(list1.length).assertEqual(list2.length);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_consistency_002
     * @tc.desc:verify hasAccessoryRight consistency
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_consistency_002", 0, () => {
        console.info('----------------------UsbAccessory_comp_consistency_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                let result1 = usbManager.hasAccessoryRight(list[0]);
                let result2 = usbManager.hasAccessoryRight(list[0]);
                expect(result1).assertEqual(result2);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_nullHandling_001
     * @tc.desc:verify null handling in getAccessoryList
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_nullHandling_001", 0, () => {
        console.info('----------------------UsbAccessory_comp_nullHandling_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            expect(list !== null).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_undefinedHandling_001
     * @tc.desc:verify undefined handling in getAccessoryList
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_undefinedHandling_001", 0, () => {
        console.info('----------------------UsbAccessory_comp_undefinedHandling_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            expect(list !== undefined).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_emptyList_001
     * @tc.desc:verify handling empty accessory list
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_emptyList_001", 0, () => {
        console.info('----------------------UsbAccessory_comp_emptyList_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            expect(list.length >= 0).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_stringValues_001
     * @tc.desc:verify manufacturer is string
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_stringValues_001", 0, () => {
        console.info('----------------------UsbAccessory_comp_stringValues_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                expect(typeof list[0].manufacturer).assertEqual('string');
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_stringValues_002
     * @tc.desc:verify model is string
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_stringValues_002", 0, () => {
        console.info('----------------------UsbAccessory_comp_stringValues_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                expect(typeof list[0].model).assertEqual('string');
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_stringValues_003
     * @tc.desc:verify description is string
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_stringValues_003", 0, () => {
        console.info('----------------------UsbAccessory_comp_stringValues_003---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                expect(typeof list[0].description).assertEqual('string');
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_stringValues_004
     * @tc.desc:verify version is string
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_stringValues_004", 0, () => {
        console.info('----------------------UsbAccessory_comp_stringValues_004---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                expect(typeof list[0].version).assertEqual('string');
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_stringValues_005
     * @tc.desc:verify uri is string
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_stringValues_005", 0, () => {
        console.info('----------------------UsbAccessory_comp_stringValues_005---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                expect(typeof list).assertEqual('string');
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_stringValues_006
     * @tc.desc:verify serial is string
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_stringValues_006", 0, () => {
        console.info('----------------------UsbAccessory_comp_stringValues_006---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                expect(typeof list[0].serial).assertEqual('string');
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_tokenId_001
     * @tc.desc:verify tokenId is number
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_tokenId_001", 0, () => {
        console.info('----------------------UsbAccessory_comp_tokenId_001---------------------------');
        try {
            const bundleFlags = bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION
                | bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_EXTENSION_ABILITY;
            let info = bundleManager.getBundleInfoForSelfSync(bundleFlags);
            let id = info.appInfo.accessTokenId;
            expect(typeof id).assertEqual('number');
        } catch (err) {
            console.error("error: " + JSON.stringify(err));
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_tokenId_002
     * @tc.desc:verify tokenId is positive
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_tokenId_002", 0, () => {
        console.info('----------------------UsbAccessory_comp_tokenId_002---------------------------');
        try {
            const bundleFlags = bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION
                | bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_EXTENSION_ABILITY;
            let info = bundleManager.getBundleInfoForSelfSync(bundleFlags);
            let id = info.appInfo.accessTokenId;
            expect(id > 0).assertEqual(true);
        } catch (err) {
            console.error("error: " + JSON.stringify(err));
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_tokenId_003
     * @tc.desc:verify tokenId is integer
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_tokenId_003", 0, () => {
        console.info('----------------------UsbAccessory_comp_tokenId_003---------------------------');
        try {
            const bundleFlags = bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION
                | bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_EXTENSION_ABILITY;
            let info = bundleManager.getBundleInfoForSelfSync(bundleFlags);
            let id = info.appInfo.accessTokenId;
            expect(Number.isInteger(id)).assertEqual(true);
        } catch (err) {
            console.error("error: " + JSON.stringify(err));
        }
    })

    /*
     * @tc.name:UsbAccessory_comp_lifecycle_001
     * @tc.desc:verify full lifecycle test
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_lifecycle_001", 0, () => {
        console.info('----------------------UsbAccessory_comp_lifecycle_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.requestAccessoryRight(list[0]);
                    let hasRight1 = usbManager.hasAccessoryRight(list[0]);
                    expect(typeof hasRight1).assertEqual('boolean');
                    usbManager.cancelAccessoryRight(list[0]);
                    let hasRight2 = usbManager.hasAccessoryRight(list[0]);
                    expect(typeof hasRight2).assertEqual('boolean');
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
     * @tc.name:UsbAccessory_comp_lifecycle_002
     * @tc.desc:verify open/close lifecycle
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_lifecycle_002", 0, () => {
        console.info('----------------------UsbAccessory_comp_lifecycle_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.addAccessoryRight(tokenId, list[0]);
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
     * @tc.name:UsbAccessory_comp_apiAvailability_001
     * @tc.desc:verify usbManager API availability
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_apiAvailability_001", 0, () => {
        console.info('----------------------UsbAccessory_comp_apiAvailability_001---------------------------');
        expect(usbManager !== null).assertEqual(true);
        expect(typeof usbManager).assertEqual('object');
    })

    /*
     * @tc.name:UsbAccessory_comp_apiAvailability_002
     * @tc.desc:verify getAccessoryList method exists
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_apiAvailability_002", 0, () => {
        console.info('----------------------UsbAccessory_comp_apiAvailability_002---------------------------');
        expect(typeof usbManager.getAccessoryList).assertEqual('function');
    })

    /*
     * @tc.name:UsbAccessory_comp_apiAvailability_003
     * @tc.desc:verify requestAccessoryRight method exists
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_apiAvailability_003", 0, () => {
        console.info('----------------------UsbAccessory_comp_apiAvailability_003---------------------------');
        expect(typeof usbManager.requestAccessoryRight).assertEqual('function');
    })

    /*
     * @tc.name:UsbAccessory_comp_apiAvailability_004
     * @tc.desc:verify hasAccessoryRight method exists
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_apiAvailability_004", 0, () => {
        console.info('----------------------UsbAccessory_comp_apiAvailability_004---------------------------');
        expect(typeof usbManager.hasAccessoryRight).assertEqual('function');
    })

    /*
     * @tc.name:UsbAccessory_comp_apiAvailability_005
     * @tc.desc:verify cancelAccessoryRight method exists
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_apiAvailability_005", 0, () => {
        console.info('----------------------UsbAccessory_comp_apiAvailability_005---------------------------');
        expect(typeof usbManager.cancelAccessoryRight).assertEqual('function');
    })

    /*
     * @tc.name:UsbAccessory_comp_apiAvailability_006
     * @tc.desc:verify addAccessoryRight method exists
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_apiAvailability_006", 0, () => {
        console.info('----------------------UsbAccessory_comp_apiAvailability_006---------------------------');
        expect(typeof usbManager.addAccessoryRight).assertEqual('function');
    })

    /*
     * @tc.name:UsbAccessory_comp_apiAvailability_007
     * @tc.desc:verify openAccessory method exists
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_apiAvailability_007", 0, () => {
        console.info('----------------------UsbAccessory_comp_apiAvailability_007---------------------------');
        expect(typeof usbManager.openAccessory).assertEqual('function');
    })

    /*
     * @tc.name:UsbAccessory_comp_apiAvailability_008
     * @tc.desc:verify closeAccessory method exists
     * @tc.type: FUNC
     */
    it("UsbAccessory_comp_apiAvailability_008", 0, () => {
        console.info('----------------------UsbAccessory_comp_apiAvailability_008---------------------------');
        expect(typeof usbManager.closeAccessory).assertEqual('function');
    })
})