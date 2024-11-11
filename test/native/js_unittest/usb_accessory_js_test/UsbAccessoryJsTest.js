/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

describe("UsbAccessoryJsTest", function () {
    beforeAll(function() {
        console.info('beforeAll called')
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
    const bundleFlags = bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION
        | bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_EXTENSION_ABILITY;

    /*
         * @tc.name:UsbAccessory_getAccessoryList_001
         * @tc.desc:verify getAccessoryList result
         * @tc.type: FUNC
         */
    it("UsbAccessory_getAccessoryList_001", 0, () => {
        console.info('----------------------UsbAccessory_getAccessoryList_001---------------------------');
        try {
            let accList = usbManager.getAccessoryList();
            expect(accList != null).assertEqual(true);
            expect(accList.length == 1).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_requestAccessoryRight
     * @tc.desc:verify requestAccessoryRight no param result
     * @tc.type: FUNC
     */
    it("UsbAccessory_requestAccessoryRight_002", 0, () => {
        console.info('----------------------UsbAccessory_requestAccessoryRight_002---------------------------');
        try {
            let access = {"manufacturer": "accessoryTest"};
            usbManager.requestAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_requestAccessoryRight
     * @tc.desc:verify requestAccessoryRight result
     * @tc.type: FUNC
     */
    it("UsbAccessory_requestAccessoryRight_003", 0, () => {
        console.info('----------------------UsbAccessory_requestAccessoryRight_003---------------------------');
        try {
            let accList = usbManager.getAccessoryList();
            expect(accList != null).assertEqual(true);
            expect(accList.length == 1).assertEqual(true);
            try {
                usbManager.requestAccessoryRight(accList[0]);
                usbManager.cancelAccessoryRight(accList[0]);
            } catch (err) {
                expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_hasAccessoryRight
     * @tc.desc:verify hasAccessoryRight no param result
     * @tc.type: FUNC
     */
    it("UsbAccessory_hasAccessoryRight_004", 0, () => {
        console.info('----------------------UsbAccessory_hasAccessoryRight_004---------------------------');
        try {
            let access = { manufacturer: "accessoryTest" };
            usbManager.hasAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_hasAccessoryRight
     * @tc.desc:verify hasAccessoryRight result
     * @tc.type: FUNC
     */
    it("UsbAccessory_hasAccessoryRight_005", 0, () => {
        console.info('----------------------UsbAccessory_hasAccessoryRight_005---------------------------');
        try {
            let accList = usbManager.getAccessoryList();
            expect(accList != null).assertEqual(true);
            expect(accList.length == 1).assertEqual(true);
            try {
                let ret = usbManager.hasAccessoryRight(accList[0]);
                expect(ret).assertEqual(false);
            } catch (err) {
                expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_hasAccessoryRight
     * @tc.desc:verify hasAccessoryRight result
     * @tc.type: FUNC
     */
    it("UsbAccessory_hasAccessoryRight_006", 0, () => {
        console.info('----------------------UsbAccessory_hasAccessoryRight_006---------------------------');
        try {
            let accList = usbManager.getAccessoryList();
            expect(accList != null).assertEqual(true);
            expect(accList.length == 1).assertEqual(true);
            try {
                usbManager.requestAccessoryRight(accList[0]);
                usbManager.hasAccessoryRight(accList[0]);
                usbManager.cancelAccessoryRight(accList[0]);
            } catch (err) {
                expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_cancelAccessoryRight
     * @tc.desc:verify cancelAccessoryRight no param result
     * @tc.type: FUNC
     */
    it("UsbAccessory_cancelAccessoryRight_007", 0, () => {
        console.info('----------------------UsbAccessory_cancelAccessoryRight_007---------------------------');
        try {
            let access = {"manufacturer": "accessoryTest"};
            usbManager.cancelAccessoryRight(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_cancelAccessoryRight
     * @tc.desc:verify cancelAccessoryRight result
     * @tc.type: FUNC
     */
    it("UsbAccessory_cancelAccessoryRight_008", 0, () => {
        console.info('----------------------UsbAccessory_cancelAccessoryRight_008---------------------------');
        try {
            let accList = usbManager.getAccessoryList();
            expect(accList != null).assertEqual(true);
            expect(accList.length == 1).assertEqual(true);
            try {
                usbManager.cancelAccessoryRight(accList[0]);
            } catch (err) {
                expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_addAccessoryRight
     * @tc.desc:verify addAccessoryRight no param result
     * @tc.type: FUNC
     */
    it("UsbAccessory_addAccessoryRight_009", 0, () => {
        console.info('----------------------UsbAccessory_addAccessoryRight_009---------------------------');
        try {
            let accList = usbManager.getAccessoryList();
            expect(accList != null).assertEqual(true);
            expect(accList.length == 1).assertEqual(true);
            try {
                let tokenId = 0x001;
                usbManager.addAccessoryRight(tokenId, accList[0]);
            } catch (err) {
                expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })


    /*
     * @tc.name:UsbAccessory_addAccessoryRight
     * @tc.desc:verify addAccessoryRight no param result
     * @tc.type: FUNC
     */
    it("UsbAccessory_addAccessoryRight_010", 0, () => {
        console.info('----------------------UsbAccessory_addAccessoryRight_010---------------------------');
        try {
            try {
                let tokenId = 0x001;
                let access = {"manufacturer": "accessoryTest"};
                usbManager.addAccessoryRight(tokenId, access);
            } catch (err) {
                expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
            }
        } catch (err) {
            console.error("getBundleInfoForSelfSync error")
        }
    })

    /*
     * @tc.name:UsbAccessory_addAccessoryRight
     * @tc.desc:verify addAccessoryRight no param result
     * @tc.type: FUNC
     */
    it("UsbAccessory_addAccessoryRight_011", 0, () => {
        console.info('----------------------UsbAccessory_addAccessoryRight_011---------------------------');
        try {
            let bundleInfo = bundleManager.getBundleInfoForSelfSync(bundleFlags);
            let tokenId = bundleInfo.appInfo.accessTokenId;
            try {
                let accList = usbManager.getAccessoryList();
                expect(accList != null).assertEqual(true);
                expect(accList.length == 1).assertEqual(true);
                try {
                    usbManager.addAccessoryRight(tokenId, accList[0]);
                } catch (err) {
                    expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                }
            } catch (err) {
                expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
            }
        } catch (err) {
            console.error("getBundleInfoForSelfSync error")
        }
    })

    /*
     * @tc.name:UsbAccessory_openAccessory
     * @tc.desc:verify openAccessory no param result
     * @tc.type: FUNC
     */
    it("UsbAccessory_openAccessory_0012", 0, () => {
        console.info('----------------------UsbAccessory_openAccessory_0012---------------------------');
        try {
            let access = {"manufacturer": "accessoryTest"};
            usbManager.openAccessory(access);
            expect(false).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_openAccessory
     * @tc.desc:verify openAccessory no param result
     * @tc.type: FUNC
     */
    it("UsbAccessory_openAccessory_0013", 0, () => {
        console.info('----------------------UsbAccessory_openAccessory_0013---------------------------');
        try {
            let accList = usbManager.getAccessoryList();
            expect(accList != null).assertEqual(true);
            expect(accList.length == 1).assertEqual(true);
            try {
                usbManager.openAccessory(accList[0]);
            } catch (err) {
                expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_openAccessory
     * @tc.desc:verify openAccessory no param result
     * @tc.type: FUNC
     */
    it("UsbAccessory_openAccessory_0014", 0, () => {
        console.info('----------------------UsbAccessory_openAccessory_0014---------------------------');
        try {
            let bundleInfo = bundleManager.getBundleInfoForSelfSync(bundleFlags);
            let tokenId = bundleInfo.appInfo.accessTokenId;
            try {
                let accList = usbManager.getAccessoryList();
                expect(accList != null).assertEqual(true);
                expect(accList.length == 1).assertEqual(true);
                try {
                    usbManager.addAccessoryRight(tokenId, accList[0]);
                    let acchandle =usbManager.openAccessory(accList[0]);
                    usbManager.closeAccessory(acchandle);
                } catch (err) {
                    expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                }
            } catch (err) {
                expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
            }
        } catch (err) {
            console.error("getBundleInfoForSelfSync error")
        }
    })

    /*
     * @tc.name:UsbAccessory_openAccessory
     * @tc.desc:verify openAccessory no param result
     * @tc.type: FUNC
     */
    it("UsbAccessory_openAccessory_0015", 0, () => {
        console.info('----------------------UsbAccessory_openAccessory_0015---------------------------');
        try {
            let bundleInfo = bundleManager.getBundleInfoForSelfSync(bundleFlags);
            let tokenId = bundleInfo.appInfo.accessTokenId;
            try {
                let accList = usbManager.getAccessoryList();
                expect(accList != null).assertEqual(true);
                expect(accList.length == 1).assertEqual(true);
                let acchandle = {"accessFd": 0};
                try {
                    usbManager.addAccessoryRight(tokenId, accList[0]);
                    acchandle = usbManager.openAccessory(accList[0]);
                    usbManager.openAccessory(accList[0]);
                } catch (err) {
                    expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                }
                usbManager.closeAccessory(acchandle);
            } catch (err) {
                expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
            }
        } catch (err) {
            console.error("getBundleInfoForSelfSync error")
        }
    })

    /*
     * @tc.name:UsbAccessory_closeAccessory
     * @tc.desc:verify closeAccessory no param result
     * @tc.type: FUNC
     */
    it("UsbAccessory_closeccessory_0016", 0, () => {
        console.info('----------------------UsbAccessory_closeccessory_0016---------------------------');
        try {
            let bundleInfo = bundleManager.getBundleInfoForSelfSync(bundleFlags);
            let tokenId = bundleInfo.appInfo.accessTokenId;
            try {
                let accList = usbManager.getAccessoryList();
                expect(accList != null).assertEqual(true);
                expect(accList.length == 1).assertEqual(true);
                let acchandle = {"accessFd": 0};
                try {
                    usbManager.closeAccessory(acchandle);
                } catch (err) {
                    expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                }
            } catch (err) {
                expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
            }
        } catch (err) {
            console.error("getBundleInfoForSelfSync error")
        }
    })
})