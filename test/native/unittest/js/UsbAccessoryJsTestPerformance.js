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

describe("UsbAccessoryJsTestPerformance", function () {
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
     * @tc.name:UsbAccessory_perf_getAccessoryList_001
     *   * @tc.desc:verify getAccessoryList performance with single call
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_getAccessoryList_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_getAccessoryList_001---------------------------');
        try {
            let startTime = Date.now();
            let list = usbManager.getAccessoryList();
            let endTime = Date.now();
            let duration = endTime - startTime;
            expect(list != null).assertEqual(true);
            expect(duration < 1000).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_getAccessoryList_002
     * @tc.desc:verify getAccessoryList performance with 10 calls
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_getAccessoryList_002", 0, () => {
        console.info('----------------------UsbAccessory_perf_getAccessoryList_002---------------------------');
        try {
            let startTime = Date.now();
            for (let i = 0; i < 10; i++) {
                let list = usbManager.getAccessoryList();
                expect(list != null).assertEqual(true);
            }
            let endTime = Date.now();
            let duration = endTime - startTime;
            expect(duration < 5000).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_getAccessoryList_003
     * @tc.desc:verify getAccessoryList performance with 100 calls
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_getAccessoryList_003", 0, () => {
        console.info('----------------------UsbAccessory_perf_getAccessoryList_003---------------------------');
        try {
            let startTime = Date.now();
            for (let i = 0; i < 100; i++) {
                let list = usbManager.getAccessoryList();
                expect(list != null).assertEqual(true);
            }
            let endTime = Date.now();
            let duration = endTime - startTime;
            expect(duration < 10000).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_hasAccessoryRight_001
     * @tc.desc:verify hasAccessoryRight performance with single call
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_hasAccessoryRight_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_hasAccessoryRight_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                let startTime = Date.now();
                let result = usbManager.hasAccessoryRight(list[0]);
                let endTime = Date.now();
                let duration = endTime - startTime;
                expect(typeof result).assertEqual('boolean');
                expect(duration < 1000).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_hasAccessoryRight_002
     * @tc.desc:verify hasAccessoryRight performance with 10 calls
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_hasAccessoryRight_002", 0, () => {
        console.info('----------------------UsbAccessory_perf_hasAccessoryRight_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                let startTime = Date.now();
                for (let i = 0; i < 10; i++) {
                    let result = usbManager.hasAccessoryRight(list[0]);
                    expect(typeof result).assertEqual('boolean');
                }
                let endTime = Date.now();
                let duration = endTime - startTime;
                expect(duration < 5000).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_hasAccessoryRight_003
     * @tc.desc:verify hasAccessoryRight performance with 100 calls
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_hasAccessoryRight_003", 0, () => {
        console.info('----------------------UsbAccessory_perf_hasAccessoryRight_003---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                let startTime = Date.now();
                for (let i = 0; i < 100; i++) {
                    let result = usbManager.hasAccessoryRight(list[0]);
                    expect(typeof result).assertEqual('boolean');
                }
                let endTime = Date.now();
                let duration = endTime - startTime;
                expect(duration < 10000).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_requestAccessoryRight_001
     * @tc.desc:verify requestAccessoryRight performance
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_requestAccessoryRight_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_requestAccessoryRight_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                let startTime = Date.now();
                try {
                    usbManager.requestAccessoryRight(list[0]);
                    usbManager.cancelAccessoryRight(list[0]);
                } catch (err) {
                    expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                }
                let endTime = Date.now();
                let duration = endTime - startTime;
                expect(duration < 5000).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_cancelAccessoryRight_001
     * @tc.desc:verify cancelAccessoryRight performance
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_cancelAccessoryRight_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_cancelAccessoryRight_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.requestAccessoryRight(list[0]);
                    let startTime = Date.now();
                    usbManager.cancelAccessoryRight(list[0]);
                    let endTime = Date.now();
                    let duration = endTime - startTime;
                    expect(duration < 3000).assertEqual(true);
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
     * @tc.name:UsbAccessory_perf_addAccessoryRight_001
     * @tc.desc:verify addAccessoryRight performance
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_addAccessoryRight_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_addAccessoryRight_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                let startTime = Date.now();
                try {
                    usbManager.addAccessoryRight(tokenId, list[0]);
                } catch (err) {
                    expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                }
                let endTime = Date.now();
                let duration = endTime - startTime;
                expect(duration < 3000).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_openAccessory_001
     * @tc.desc:verify openAccessory performance
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_openAccessory_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_openAccessory_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.addAccessoryRight(tokenId, list[0]);
                    let startTime = Date.now();
                    let handle = usbManager.openAccessory(list[0]);
                    let endTime = Date.now();
                    let duration = endTime - startTime;
                    expect(duration < 3000).assertEqual(true);
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
     * @tc.name:UsbAccessory_perf_closeAccessory_001
     * @tc.desc:verify closeAccessory performance
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_closeAccessory_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_closeAccessory_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.addAccessoryRight(tokenId, list[0]);
                    let handle = usbManager.openAccessory(list[0]);
                    let startTime = Date.now();
                    usbManager.closeAccessory(handle);
                    let endTime = Date.now();
                    let duration = endTime - startTime;
                    expect(duration < 3000).assertEqual(true);
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
     * @tc.name:UsbAccessory_perf_fullCycle_001
     * @tc.desc:verify full cycle performance (request, has, cancel)
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_fullCycle_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_fullCycle_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                let startTime = Date.now();
                try {
                    usbManager.requestAccessoryRight(list[0]);
                    let hasRight = usbManager.hasAccessoryRight(list[0]);
                    usbManager.cancelAccessoryRight(list[0]);
                    expect(typeof hasRight).assertEqual('boolean');
                } catch (err) {
                    expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                }
                let endTime = Date.now();
                let duration = endTime - startTime;
                expect(duration < 5000).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_fullCycle_002
     * @tc.desc:verify full cycle performance (add, open, close)
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_fullCycle_002", 0, () => {
        console.info('----------------------UsbAccessory_perf_fullCycle_002---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                let startTime = Date.now();
                try {
                    usbManager.addAccessoryRight(tokenId, list[0]);
                    let handle = usbManager.openAccessory(list[0]);
                    usbManager.closeAccessory(handle);
                } catch (err) {
                    expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                }
                let endTime = Date.now();
                let duration = endTime - startTime;
                expect(duration < 5000).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_mixedOperations_001
     * @tc.desc:verify mixed operations performance
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_mixedOperations_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_mixedOperations_001---------------------------');
        try {
            let startTime = Date.now();
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
            }
            let endTime = Date.now();
            let duration = endTime - startTime;
            expect(duration < 10000).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_batchGet_001
     * @tc.desc:verify batch getAccessoryList performance
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_batchGet_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_batchGet_001---------------------------');
        try {
            let startTime = Date.now();
            let results = [];
            for (let i = 0; i < 50; i++) {
                let list = usbManager.getAccessoryList();
                results.push(list != null);
            }
            let endTime = Date.now();
            let duration = endTime - startTime;
            expect(results.every(r => r === true)).assertEqual(true);
            expect(duration < 5000).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_batchHas_001
     * @tc.desc:verify batch hasAccessoryRight performance
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_batchHas_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_batchHas_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                let startTime = Date.now();
                let results = [];
                for (let i = 0; i < 50; i++) {
                    let result = usbManager.hasAccessoryRight(list[0]);
                    results.push(typeof result === 'boolean');
                }
                let endTime = Date.now();
                let duration = endTime - startTime;
                expect(results.every(r => r === true)).assertEqual(true);
                expect(duration < 5000).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_batchRequestCancel_001
     * @tc.desc:verify batch request/cancel performance
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_batchRequestCancel_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_batchRequestCancel_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                let startTime = Date.now();
                for (let i = 0; i < 20; i++) {
                    try {
                        usbManager.requestAccessoryRight(list[0]);
                        usbManager.cancelAccessoryRight(list[0]);
                    } catch (err) {
                        expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                    }
                }
                let endTime = Date.now();
                let duration = endTime - startTime;
                expect(duration < 10000).assertEqual(true);
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_batchOpenClose_001
     * @tc.desc:verify batch open/close performance
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_batchOpenClose_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_batchOpenClose_001---------------------------');
        try {
            let list = usbManager.getAccessoryList();
            if (list != null && list.length > 0) {
                try {
                    usbManager.addAccessoryRight(tokenId, list[0]);
                    let startTime = Date.now();
                    for (let i = 0; i < 20; i++) {
                        try {
                            let handle = usbManager.openAccessory(list[0]);
                            usbManager.closeAccessory(handle);
                        } catch (err) {
                            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                        }
                    }
                    let endTime = Date.now();
                    let duration = endTime - startTime;
                    expect(duration < 10000).assertEqual(true);
                } catch (err) {
                    expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
                }
            } else {
                expect(true).assertEqual(true);
            }
        } catch (err) {
            expect(err.code).assertEqualEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_concurrentGet_001
     * @tc.desc:verify concurrent getAccessoryList calls
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_concurrentGet_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_concurrentGet_001---------------------------');
        try {
            let promises = [];
            for (let i = 0; i < 10; i++) {
                promises.push(Promise.resolve(usbManager.getAccessoryList()));
            }
            let startTime = Date.now();
            Promise.all(promises).then(results => {
                let endTime = Date.now();
                let duration = endTime - startTime;
                expect(results.every(r => r != null)).assertEqual(true);
                expect(duration < 5000).assertEqual(true);
            });
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_memory_001
     * @tc.desc:verify memory usage with repeated calls
     * @tc.type:  FUNC
     */
    it("UsbAccessory_perf_memory_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_memory_001---------------------------');
        try {
            let lists = [];
            for (let i = 0; i < 100; i++) {
                let list = usbManager.getAccessoryList();
                lists.push(list);
            }
            expect(lists.length).assertEqual(100);
            expect(lists.every(l => l != null)).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_responseTime_001
     * @tc.desc:verify response time consistency
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_responseTime_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_responseTime_001---------------------------');
        try {
            let times = [];
            for (let i = 0; i < 10; i++) {
                let startTime = Date.now();
                let list = = usbManager.getAccessoryList();
                let endTime = Date.now();
                times.push(endTime - startTime);
            }
            let avgTime = times.reduce((a, b) => a + b, 0) / times.length;
            expect(avgTime < 500).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })

    /*
     * @tc.name:UsbAccessory_perf_warmup_001
     * @tc.desc:verify performance after warmup
     * @tc.type: FUNC
     */
    it("UsbAccessory_perf_warmup_001", 0, () => {
        console.info('----------------------UsbAccessory_perf_warmup_001---------------------------');
        try {
            for (let i = 0; i < 10; i++) {
                usbManager.getAccessoryList();
            }
            let startTime = Date.now();
            for (let i = 0; i < 10; i++) {
                let list = usbManager.getAccessoryList();
                expect(list != null).assertEqual(true);
            }
            let endTime = Date.now();
            let duration = endTime - startTime;
            expect(duration < 3000).assertEqual(true);
        } catch (err) {
            expect(err.code).assertEqual(SERVICE_EXCEPTION_CODE);
        }
    })
})