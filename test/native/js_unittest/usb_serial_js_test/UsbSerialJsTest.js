/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

import serialManager from '@ohos.usbManager.serial'
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

describe("UsbSerialJsTest", function () {
  var gPortId;
  const attribute = {
    baudRate: 9600,
    dataBits: 8,
    parity: 0,
    stopBits: 1
  };
  const BASE_COUNT = 1000;
  beforeAll(function() {
    console.info('beforeAll called')
    var serialPortList = serialManager.getPortList();
    console.log("beforeAll getPortList :" + serialPortList);
    gPortId = parseInt(serialPortList[0].portId);
    console.log("gprotId = " + gPortId);
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

  /**
   * @tc.number   : SUB_USB_HostMgr_Serial_JS_getPortList_Perf_0100
   * @tc.name     : testgetPortList001
   * @tc.desc     : getPortList successfully.
   * @tc.size     : MediumTest
   * @tc.type     : PERFORMANCE
   * @tc.level    : Level 3
   */
  it('testgetPortList001', 0, function () {
    console.info('serial testgetPortList001 begin');
    var startTime = new Date().getTime();
    console.info("getPortList start time = " + startTime);
    for (var count = 0; count < BASE_COUNT; count++) {
      try {
        serialManager.getPortList();
      } catch (error) {
      }
    }
    var endTime = new Date().getTime();
    console.info("getPortList end time = " + endTime);
    var averageTime = (endTime - startTime);
    console.info("getPortList averageTime = " + averageTime);
    expect(averageTime < 1200).assertTrue();
  })

  /**
   * @tc.number   : SUB_USB_HostMgr_Serial_JS_hasRight_Perf_0100
   * @tc.name     : testhasRiht001
   * @tc.desc     : hasRight successfully.
   * @tc.size     : MediumTest
   * @tc.type     : PERFORMANCE
   * @tc.level    : Level 3
   */
  it('testhasRiht001', 0, function () {
    console.info('serial testhasRiht001 begin');
    var startTime = new Date().getTime();
    console.info("hasSerialRight start time = " + startTime);
    for (var count = 0; count < BASE_COUNT; count++) {
      try {
        serialManager.hasSerialRight(gPortId);
      } catch (error) {
      }
    }
    var endTime = new Date().getTime();
    console.info("hasSerialRight end time = " + endTime);
    var averageTime = (endTime - startTime);
    console.info("hasSerialRight averageTime = " + averageTime);
    expect(averageTime < 380).assertTrue();
  })

  /**
   * @tc.number   : SUB_USB_HostMgr_Serial_JS_requestSerialRight_Perf_0100
   * @tc.name     : testrequestSerialRight001
   * @tc.desc     : requestSerialRight successfully.
   * @tc.size     : MediumTest
   * @tc.type     : PERFORMANCE
   * @tc.level    : Level 3
   */
  it('testrequestRiht001', 0, function () {
    console.info('serial testrequestRight001 begin');
    var startTime = new Date().getTime();
    console.info("testrequestRight001 start time = " + startTime);
    for (var count = 0; count < BASE_COUNT; count++) {
      try {
        serialManager.requestSerialRight(gPortId);
      } catch (error) {
      }
    }
    var endTime = new Date().getTime();
    console.info("requestSerialRight end time = " + endTime);
    var averageTime = (endTime - startTime);
    console.info("requestSerialRight averageTime = " + averageTime);
    expect(averageTime < 120).assertTrue();
  })

  /**
   * @tc.number   : SUB_USB_HostMgr_Serial_JS_open_Perf_0100
   * @tc.name     : testOpen001
   * @tc.desc     : open cost time test.
   * @tc.size     : MediumTest
   * @tc.type     : PERFORMANCE
   * @tc.level    : Level 3
   */
    it('testOpen001', 0, function () {
      console.info('serial testOpen001 begin');
      var startTime = new Date().getTime();
      console.info("open start time = " + startTime);
      for (var count = 0; count < BASE_COUNT; count++) {
        try {
          serialManager.open(gPortId);
        } catch (error) {
        }
      }
      var endTime = new Date().getTime();
      console.info("open end time = " + endTime);
      var averageTime = (endTime - startTime);
      console.info("open averageTime = " + averageTime);
      expect(averageTime < 1920).assertTrue();
    })

  /**
   * @tc.number   : SUB_USB_HostMgr_Serial_JS_getAttribute_Perf_0100
   * @tc.name     : testgetAttribute001
   * @tc.desc     : getAttribute successfully.
   * @tc.size     : MediumTest
   * @tc.type     : PERFORMANCE
   * @tc.level    : Level 3
   */
  it('testgetAttribute001', 0, async function () {
    console.info('serial testgetAttribute001 begin');
    await new Promise(resolve => setTimeout(resolve, 5000));
    var startTime = new Date().getTime();
    console.info("start time = " + startTime);
    for (var count = 0; count < BASE_COUNT; count++) {
      try {
        serialManager.getAttribute(gPortId);
      } catch (error) {
      }
    }
    var endTime = new Date().getTime();
    console.info("testgetAttribute end time = " + endTime);
    var averageTime = (endTime - startTime);
    console.info("getAttribute averageTime = " + averageTime);
    expect(averageTime < 5900).assertTrue();
  });

  /**
   * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_Perf_0100
   * @tc.name     : testsetAttribute001
   * @tc.desc     : setAttribute successfully.
   * @tc.size     : MediumTest
   * @tc.type     : PERFORMANCE
   * @tc.level    : Level 3
   */
  it('testsetAttribute001', 0, function () {
    console.info('serial testsetAttribute001 begin');
    var startTime = new Date().getTime();
    console.info("testsetAttribute001 start time = " + startTime);
    for (var count = 0; count < BASE_COUNT; count++) {
      try {
        serialManager.setAttribute(gPortId, attribute);
      } catch (error) {
      }
    }
    var endTime = new Date().getTime();
    console.info("testsetAttribute001 end time = " + endTime);
    var averageTime = (endTime - startTime);
    console.info("testsetAttribute001 averageTime = " + averageTime);
    expect(averageTime < 12200).assertTrue();
  })

  /**
   * @tc.number   : SUB_USB_HostMgr_Serial_JS_write_Perf_0100
   * @tc.name     : testwrite001
   * @tc.desc     : write successfully.
   * @tc.size     : MediumTest
   * @tc.type     : PERFORMANCE
   * @tc.level    : Level 3
   */
  it('testwrite001', 0, function () {
    console.info('serial testwrite001 begin');
    var startTime = new Date().getTime();
    let writeBuffer = new Uint8Array([0x31, 0x32, 0x33, 0x34, 0x35, 0x36]);
    console.info("testwrite001 start time = " + startTime);
    for (var count = 0; count < BASE_COUNT; count++) {
      try {
        serialManager.write(gPortId, writeBuffer, 100);
      } catch (error) {
      }
    }
    var endTime = new Date().getTime();
    console.info("testwrite001 end time = " + endTime);
    var averageTime = (endTime - startTime);
    console.info("testwrite001 averageTime = " + averageTime);
    expect(averageTime < 104000).assertTrue();
  })

  /**
   * @tc.number   : SUB_USB_HostMgr_Serial_JS_writeSync_Perf_0100
   * @tc.name     : testwriteSync001
   * @tc.desc     : write successfully.
   * @tc.size     : MediumTest
   * @tc.type     : PERFORMANCE
   * @tc.level    : Level 3
   */
  it('testWriteSync001', 0, function () {
    console.info('serial testWriteSync001 begin');
    var startTime = new Date().getTime();
    let writeBuffer = new Uint8Array([0x31, 0x32, 0x33, 0x34, 0x35, 0x36]);
    console.info("testwriteSync001 start time = " + startTime);
    for (var count = 0; count < BASE_COUNT; count++) {
      try {
        var n = serialManager.writeSync(gPortId, writeBuffer, 100);
      } catch (error) {
      }
    }
    var endTime = new Date().getTime();
    console.info("testwriteSync001 end time = " + endTime);
    var averageTime = (endTime - startTime);
    console.info("testwriteSync001 averageTime = " + averageTime);
    expect(averageTime < 104000).assertTrue();
  })

  /**
   * @tc.number   : SUB_USB_HostMgr_Serial_JS_readSync_Perf_0100
   * @tc.name     : testReadSync001
   * @tc.desc     : read successfully.
   * @tc.size     : MediumTest
   * @tc.type     : PERFORMANCE
   * @tc.level    : Level 3
   */
    it('testReadSync001', 0, function () {
      console.info('serial testReadSync001 begin');
      var startTime = new Date().getTime();
      let readBuffer = new Uint8Array(4096);
      console.info("testReadSync001 start time = " + startTime);
      for (var count = 0; count < BASE_COUNT; count++) {
        try {
          var n = serialManager.readSync(gPortId, readBuffer, 10);
        } catch (error) {
        }
      }
      var endTime = new Date().getTime();
      console.info("testReadSync001 end time = " + endTime);
      var averageTime = (endTime - startTime);
      console.info("testReadSync001 averageTime = " + averageTime);
      expect(averageTime < 19590).assertTrue();
    })

  /**
   * @tc.number   : SUB_USB_HostMgr_Serial_JS_read_Perf_0100
   * @tc.name     : testRead001
   * @tc.desc     : read successfully.
   * @tc.size     : MediumTest
   * @tc.type     : PERFORMANCE
   * @tc.level    : Level 3
   */
  it('testRead001', 0, function () {
    console.info('serial testRead001 begin');
    var startTime = new Date().getTime();
    let readBuffer = new Uint8Array(4096);
    console.info("testRead001 start time = " + startTime);
    for (var count = 0; count < BASE_COUNT; count++) {
      try {
        serialManager.read(gPortId, readBuffer, 10);
      } catch (error) {
      }
    }
    var endTime = new Date().getTime();
    console.info("testread001 end time = " + endTime);
    var averageTime = (endTime - startTime);
    console.info("testread001 averageTime = " + averageTime);
    expect(averageTime < 190).assertTrue();
  })

  /**
   * @tc.number   : SUB_USB_HostMgr_Serial_JS_close_Perf_0100
   * @tc.name     : testClose001
   * @tc.desc     : close cost time test.
   * @tc.size     : MediumTest
   * @tc.type     : PERFORMANCE
   * @tc.level    : Level 3
   */
    it('testClose001', 0, function () {
      console.info('serial testClose001 begin');
      var startTime = new Date().getTime();
      console.info("close start time = " + startTime);
      for (var count = 0; count < BASE_COUNT; count++) {
        try {
          serialManager.close(gPortId);
        } catch (error) {
        }
      }
      var endTime = new Date().getTime();
      console.info("close end time = " + endTime);
      var averageTime = (endTime - startTime);
      console.info("close averageTime = " + averageTime);
      expect(averageTime < 2530).assertTrue();
    })

  /**
   * @tc.number   : SUB_USB_HostMgr_Serial_JS_cancelSerialRight_Perf_0100
   * @tc.name     : testcancelSerialRigth001
   * @tc.desc     : hasRight successfully.
   * @tc.size     : MediumTest
   * @tc.type     : PERFORMANCE
   * @tc.level    : Level 3
   */
  it('testCancelSerialRigth001', 0, function () {
    console.info('serial testcancelRigth001 begin');
    var startTime = new Date().getTime();
    console.info("testcancelRigth001 start time = " + startTime);
    for (var count = 0; count < BASE_COUNT; count++) {
      try {
        serialManager.cancelSerialRight(gPortId);
      } catch (error) {
      }
    }
    var endTime = new Date().getTime();
    console.info("testcancelRigth001 end time = " + endTime);
    var averageTime = (endTime - startTime);
    console.info("cancelSerialRight averageTime = " + averageTime);
    expect(averageTime < 380).assertTrue();
  })

  /**
   * @tc.number   : SUB_USB_HostMgr_Serial_JS_addSerialRight_Perf_0100
   * @tc.name     : testAddSerialRight001
   * @tc.desc     : hasRight successfully.
   * @tc.size     : MediumTest
   * @tc.type     : PERFORMANCE
   * @tc.level    : Level 3
   */
    it('testAddSerialRight001', 0, function () {
      console.info('serial testAddSerialRight001 begin');
      var startTime = new Date().getTime();
      console.info("testAddSerialRight001 start time = " + startTime);
      for (var count = 0; count < BASE_COUNT; count++) {
        try {
          serialManager.addSerialRight(gPortId);
        } catch (error) {
        }
      }
      var endTime = new Date().getTime();
      console.info("testAddSerialRight001 end time = " + endTime);
      var averageTime = (endTime - startTime);
      console.info("testAddSerialRight001 averageTime = " + averageTime);
      expect(averageTime < 160).assertTrue();
    })
})

