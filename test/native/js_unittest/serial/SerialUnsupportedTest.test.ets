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

import { hilog } from '@kit.PerformanceAnalysisKit';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from '@ohos/hypium';
import { serialManager } from '@kit.BasicServicesKit';
import { buffer } from '@kit.ArkTS';

const TAG: string = "[SerialUnsupportedTest]";

export default function serialUnsupportedTest() {
  describe('SerialUnsupportedTest', () => {
    beforeAll(() => {
      hilog.info(0x0000, TAG, 'SerialUnsupportedTest beforeAll start');
    })

    beforeEach(() => {
      hilog.info(0x0000, TAG, 'SerialUnsupportedTest beforeEach start');
    })

    afterEach(() => {
      hilog.info(0x0000, TAG, 'SerialUnsupportedTest afterEach start');
    })

    afterAll(() => {
      hilog.info(0x0000, TAG, 'SerialUnsupportedTest afterAll start');
    })

    it('testGetPortList_unsupported', 0, () => {
      hilog.info(0x0000, TAG, 'testGetPortList_unsupported start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let ports = serialManager.getPortList();
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testGetPortList_unsupported error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.getPortList).assertUndefined();
      }
    })

    it('testHasSerialRight_portId_0', 0, () => {
      hilog.info(0x0000, TAG, 'testHasSerialRight_portId_0 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let hasRight = serialManager.hasSerialRight(0);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testHasSerialRight_portId_0 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.hasSerialRight).assertUndefined();
      }
    })

    it('testHasSerialRight_portId_1', 0, () => {
      hilog.info(0x0000, TAG, 'testHasSerialRight_portId_1 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let hasRight = serialManager.hasSerialRight(1);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testHasSerialRight_portId_1 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.hasSerialRight).assertUndefined();
      }
    })

    it('testHasSerialRight_portId_100', 0, () => {
      hilog.info(0x0000, TAG, 'testHasSerialRight_portId_100 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let hasRight = serialManager.hasSerialRight(100);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testHasSerialRight_portId_100 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.hasSerialRight).assertUndefined();
      }
    })

    it('testHasSerialRight_portId_999', 0, () => {
      hilog.info(0x0000, TAG, 'testHasSerialRight_portId_999 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let hasRight = serialManager.hasSerialRight(999);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testHasSerialRight_portId_999 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.hasSerialRight).assertUndefined();
      }
    })

    it('testHasSerialRight_portId_neg1', 0, () => {
      hilog.info(0x0000, TAG, 'testHasSerialRight_portId_neg1 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let hasRight = serialManager.hasSerialRight(-1);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testHasSerialRight_portId_neg1 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.hasSerialRight).assertUndefined();
      }
    })

    it('testHasSerialRight_portId_2', 0, () => {
      hilog.info(0x0000, TAG, 'testHasSerialRight_portId_2 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let hasRight = serialManager.hasSerialRight(2);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testHasSerialRight_portId_2 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.hasSerialRight).assertUndefined();
      }
    })

    it('testHasSerialRight_portId_3', 0, () => {
      hilog.info(0x0000, TAG, 'testHasSerialRight_portId_3 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let hasRight = serialManager.hasSerialRight(3);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testHasSerialRight_portId_3 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.hasSerialRight).assertUndefined();
      }
    })

    it('testHasSerialRight_portId_4', 0, () => {
      hilog.info(0x0000, TAG, 'testHasSerialRight_portId_4 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let hasRight = serialManager.hasSerialRight(4);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testHasSerialRight_portId_4 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.hasSerialRight).assertUndefined();
      }
    })

    it('testHasSerialRight_portId_5', 0, () => {
      hilog.info(0x0000, TAG, 'testHasSerialRight_portId_5 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let hasRight = serialManager.hasSerialRight(5);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testHasSerialRight_portId_5 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.hasSerialRight).assertUndefined();
      }
    })

    it('testRequestSerialRight_portId_0', 0, async () => {
      hilog.info(0x0000, TAG, 'testRequestSerialRight_portId_0 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          await serialManager.requestSerialRight(0);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testRequestSerialRight_portId_0 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.requestSerialRight).assertUndefined();
      }
    })

    it('testRequestSerialRight_portId_1', 0, async () => {
      hilog.info(0x0000, TAG, 'testRequestSerialRight_portId_1 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          await serialManager.requestSerialRight(1);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testRequestSerialRight_portId_1 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.requestSerialRight).assertUndefined();
      }
    })

    it('testRequestSerialRight_portId_100', 0, async () => {
      hilog.info(0x0000, TAG, 'testRequestSerialRight_portId_100 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          await serialManager.requestSerialRight(100);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testRequestSerialRight_portId_100 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.requestSerialRight).assertUndefined();
      }
    })

    it('testRequestSerialRight_portId_999', 0, async () => {
      hilog.info(0x0000, TAG, 'testRequestSerialRight_portId_999 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          await serialManager.requestSerialRight(999);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testRequestSerialRight_portId_999 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.requestSerialRight).assertUndefined();
      }
    })

    it('testRequestSerialRight_portId_neg1', 0, async () => {
      hilog.info(0x0000, TAG, 'testRequestSerialRight_portId_neg1 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          await serialManager.requestSerialRight(-1);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testRequestSerialRight_portId_neg1 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.requestSerialRight).assertUndefined();
      }
    })

    it('testRequestSerialRight_portId_2', 0, async () => {
      hilog.info(0x0000, TAG, 'testRequestSerialRight_portId_2 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          await serialManager.requestSerialRight(2);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testRequestSerialRight_portId_2 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.requestSerialRight).assertUndefined();
      }
    })

    it('testRequestSerialRight_portId_3', 0, async () => {
      hilog.info(0x0000, TAG, 'testRequestSerialRight_portId_3 start');
'      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          await serialManager.requestSerialRight(3);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testRequestSerialRight_portId_3 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.requestSerialRight).assertUndefined();
      }
    })

    it('testCancelSerialRight_portId_0', 0, () => {
      hilog.info(0x0000, TAG, 'testCancelSerialRight_portId_0 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.cancelSerialRight(0);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testCancelSerialRight_portId_0 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.cancelSerialRight).assertUndefined();
      }
    })

    it('testCancelSerialRight_portId_1', 0, () => {
      hilog.info(0x0000, TAG, 'testCancelSerialRight_portId_1 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.cancelSerialRight(1);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testCancelSerialRight_portId_1 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.cancelSerialRight).assertUndefined();
      }
    })

    it('testCancelSerialRight_portId_100', 0, () => {
      hilog.info(0x0000, TAG, 'testCancelSerialRight_portId_100 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.cancelSerialRight(100);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testCancelSerialRight_portId_100 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.cancelSerialRight).assertUndefined();
      }
    })

    it('testCancelSerialRight_portId_999', 0, () => {
      hilog.info(0x0000, TAG, 'testCancelSerialRight_portId_999 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.cancelSerialRight(999);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testCancelSerialRight_portId_999 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.cancelSerialRight).assertUndefined();
      }
    })

    it('testCancelSerialRight_portId_neg1', 0, () => {
      hilog.info(0x0000, TAG, 'testCancelSerialRight_portId_neg1 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.cancelSerialRight(-1);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testCancelSerialRight_portId_neg1 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.cancelSerialRight).assertUndefined();
      }
    })

    it('testCancelSerialRight_portId_2', 0, () => {
      hilog.info(0x0000, TAG, 'testCancelSerialRight_portId_2 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.cancelSerialRight(2);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testCancelSerialRight_portId_2 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.cancelSerialRight).assertUndefined();
      }
    })

    it('testCancelSerialRight_portId_3', 0, () => {
      hilog.info(0x0000, TAG, 'testCancelSerialRight_portId_3 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.cancelSerialRight(3);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testCancelSerialRight_portId_3 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.cancelSerialRight).assertUndefined();
      }
    })

    it('testOpen_portId_0', 0, () => {
      hilog.info(0x0000, TAG, 'testOpen_portId_0 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.open(0);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testOpen_portId_0 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.open).assertUndefined();
      }
    })

    it('testOpen_portId_1', 0, () => {
      hilog.info(0x0000, TAG, 'testOpen_portId_1 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.open(1);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testOpen_portId_1 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.open).assertUndefined();
      }
    })

    it('testOpen_portId_100', 0, () => {
      hilog.info(0x0000, TAG, 'testOpen_portId_100 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.open(100);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testOpen_portId_100 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.open).assertUndefined();
      }
    })

    it('testOpen_portId_999', 0, () => {
      hilog.info(0x.x000, TAG, 'testOpen_portId_999 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.open(999);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testOpen_portId_999 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.open).assertUndefined();
      }
    })

    it('testOpen_portId_neg1', 0, () => {
      hilog.info(0x0000, TAG, 'testOpen_portId_neg1 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.open(-1);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testOpen_portId_neg1 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect'('(serialManager.open).assertUndefined();
      }
    })

    it('testOpen_portId_2', 0, () => {
      hilog.info(0x0000, TAG, 'testOpen_portId_2 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.open(2);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testOpen_portId_2 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.open).assertUndefined();
      }
    })

    it('testOpen_portId_3', 0, () => {
      hilog.info(0x0000, TAG, 'testOpen_portId_3 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.open(3);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testOpen_portId_3 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.open).assertUndefined();
      }
    })

    it('testClose_portId_0', 0, () => {
      hilog.info(0x0000, TAG, 'testClose_portId_0 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.close(0);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testClose_portId_0 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.close).assertUndefined();
      }
    })

    it('testClose_portId_1', 0, () => {
      hilog.info(0x0000, TAG, 'testClose_portId_1 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.close(1);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testClose_portId_1 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.close).assertUndefined();
      }
    })

    it('testClose_portId_100', 0, () => {
      hilog.info(0x0000, TAG, 'testClose_portId_100 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.close(100);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testClose_portId_100 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.close).assertUndefined();
      }
    })

    it('testClose_portId_999', 0, () => {
      hilog.info(0x0000, TAG, 'testClose_portId_999 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.close(999);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testClose_portId_999 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.close).assertUndefined();
      }
    })

    it('testClose_portId_neg1', 0, () => {
      hilog.info(0x0000, TAG, 'testClose_portId_neg1 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.close(-1);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testClose_portIdId_neg1 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.close).assertUndefined();
      }
    })

    it('testClose_portId_2', 0, () => {
      hilog.info(0x0000, TAG, 'testClose_portId_2 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.close(2);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testClose_portId_2 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.close).assertUndefined();
      }
    })

    it('testClose_portId_3', 0, () => {
      hilog.info(0x0000, TAG, 'testClose_portId_3 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.close(3);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testClose_portId_3 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.close).assertUndefined();
      }
    })

    it('testGetAttribute_portId_0', 0, () => {
      hilog.info(0x0000, TAG, 'testGetAttribute_portId_0 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let attribute = serialManager.getAttribute(0);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testGetAttribute_portId_0 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.getAttribute).assertUndefined();
      }
    })

    it('testGetAttribute_portId_1', 0, () => {
      hilog.info(0x0000, TAG, 'testGetAttribute_portId_1 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let attribute = serialManager.getAttribute(1);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testGetAttribute_portId_1 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.getAttribute).assertUndefined();
      }
    })

    it('testGetAttribute_portId_100', 0, () => {
)      hilog.info(0x0000, TAG, 'testGetAttribute_portId_100 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let attribute = serialManager.getAttribute(100);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testGetAttribute_portId_100 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.getAttribute).assertUndefined();
      }
    })

    it('testGetAttribute_portId_999', 0, () => {
      hilog.info(0x0000, TAG, 'testGetAttribute_portId_999 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let attribute = serialManager.getAttribute(999);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testGetAttribute_portId_999 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.getAttribute).assertUndefined();
      }
    })

    it('testGetAttribute_portId_neg1', 0, () => {
      hilog.info(0x0000, TAG, 'testGetAttribute_portId_neg1 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let attribute = serialManager.getAttribute(-1);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testGetAttribute_portId_neg1 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.getAttribute).assertUndefined();
      }
    })

    it('testGetAttribute_portId_2', 0, () => {
      hilog.info(0x0000, TAG, 'testGetAttribute_portId_2 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let attribute = serialManager.getAttribute(2);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testGetAttribute_portId_2 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.getAttribute).assertUndefined();
      }
    })

    it('testGetAttribute_portId_3', 0, () => {
      hilog.info(0x0000, TAG, 'testGetAttribute_portId_3 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let attribute = serialManager.getAttribute(3);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testGetAttribute_portId_3 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.getAttribute).assertUndefined();
      }
    })

    it('testSetAttribute_portId_0_default', 0, () => {
      hilog.info(0x0000, TAG, 'testSetAttribute_portId_0_default start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let attribute: serialManager.SerialAttribute = {
            baudRate: serialManager.BaudRates.BAUDRATE_9600,
            dataBits: serialManager.DataBits.DATABIT_8,
            parity: serialManager.Parity.PARITY_NONE,
            stopBits: serialManager.StopBits.STOPBIT_1
          };
          serialManager.setAttribute(0, attribute);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testSetAttribute_portId_0_default error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.setAttribute).assertUndefined();
      }
    })

    it('testSetAttribute_portId_1_baudrate_115200', 0, () => {
      hilog.info(0x0000, TAG, 'testSetAttribute_portId_1_baudrate_115200 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let attribute: serialManager.SerialAttribute = {
            baudRate: serialManager.BaudRates.BAUDRATE_115200,
            dataBits: serialManager.DataBits.DATABIT_8,
            parity: serialManager.Parity.PARITY_NONE,
            stopBits: serialManager.StopBits.STOPBIT_1
          };
          serialManager.setAttribute(1, attribute);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testSetAttribute_portId_1_baudrate_115200 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.setAttribute).assertUndefined();
      }
    })

    it('testSetAttribute_portId_100_databit_7', 0, () => {
      hilog.info(0x0000, TAG, 'testSetAttribute_portId_100_databit_7 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let attribute: serialManager.SerialAttribute = {
            baudRate: serialManager.BaudRates.BAUDRATE_9600,
            dataBits: serialManager.DataBits.DATABIT_7,
            parity: serialManager.Parity.PARITY_NONE,
            stopBits: serialManager.StopBits.STOPBIT_1
          };
          serialManager.setAttribute(100, attribute);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testSetAttribute_portId_100_databit_7 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.setAttribute).assertUndefined();
      }
    })

    it('testSetAttribute_portId_999_parity_odd', 0, () => {
      hilog.info(0x0000, TAG, 'testSetAttribute_portId_999_parity_odd start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let attribute: serialManager.SerialAttribute = {
            baudRate: serialManager.BaudRates.BAUDRATE_9600,
            dataBits: serialManager.DataBits.DATABIT_8,
            parity: serialManager.Parity.PARITY_ODD,
            stopBits: serialManager.StopBits.STOPBIT_1
          };
          serialManager.setAttribute(999, attribute);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testSetAttribute_portId_999_parity_odd error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.setAttribute).assertUndefined();
      }
    })

    it('testSetAttribute_portId_neg1_stopbit_2', 0, () => {
      hilog.info(0x0000, TAG, 'testSetAttribute_portId_neg1_stopbit_2 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let attribute: serialManager.SerialAttribute = {
            baudRate: serialManager.BaudRates.BAUDRATE_9600,
            dataBits: serialManager.DataBits.DATABIT_8,
            parity: serialManager.Parity.PARITY_NONE,
            stopBits: serialManager.StopBits.STOPBIT_2
          };
          serialManager.setAttribute(-1, attribute);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testSetAttribute_portId_neg1_stopbit_2 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.setAttribute).assertUndefined();
      }
    })

    it('testSetAttribute_portId_2_baudrate_19200', 0, () => {
      hilog.info(0x0000, TAG, 'testSetAttribute_portId_2_baudrate_19200 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let attribute: serialManager.SerialAttribute = {
            baudRate: serialManager.BaudRates.BAUDRATE_19200,
            dataBits: serialManager.DataBits.DATABIT_8,
            parity: serialManager.Parity.PARITY_NONE,
            stopBits: serialManager.StopBits.STOPBIT_1
          };
          serialManager.setAttribute(2, attribute);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testSetAttribute_portId_2_baudrate_19200 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.setAttribute).assertUndefined();
      }
    })

    it('testSetAttribute_portId_3_baudrate_38400', 0, () => {
      hilog.info(0x0000, TAG, 'testSetAttribute_portId_3_baudrate_38400 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let attribute: serialManager.SerialAttribute = {
            baudRate: serialManager.BaudRates.BAUDRATE_38400,
            dataBits: serialManager.DataBits.DATABIT_8,
            parity: serialManager.Parity.PARITY_NONE,
            stopBits: serialManager.StopBits.STOPBIT_1
          };
          serialManager.setAttribute(3, attribute);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testSetAttribute_portId_3_baudrate_38400 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.setAttribute).assertUndefined();
      }
    })

    it('testRead_portId_0_buffer_8_timeout_0', 0, async () => {
      hilog.info(0x0000, TAG, 'testRead_portId_0_buffer_8_timeout_0 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readBuffer: Uint8Array = new Uint8Array(8);
          await serialManager.read(0, readBuffer, 0);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testRead_portId_0_buffer_8_timeout_0 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.read).assertUndefined();
      }
    })

    it('testRead_portId_1_buffer_1024_timeout_100', 0, async () => {
      hilog.info(0x0000, TAG, 'testRead_portId_1_buffer_1024_timeout_100 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readBuffer: Uint8Array = new Uint8Array(1024);
          await serialManager.read(1, readBuffer, 100);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testRead_portId_1_buffer_1024_timeout_100 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.read).assertUndefined();
      }
    })

    it('testRead_portId_100_buffer_8192_timeout_1000', 0, async () => {
      hilog.info(0x0000, TAG, 'testRead_portId_100_buffer_8192_timeout_1000 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readBuffer: Uint8Array = new Uint8Array(8192);
          await serialManager.read(100, readBuffer, 1000);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testRead_portId_100_buffer_8192_timeout_1000 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.read).assertUndefined();
      }
    })

    it('testRead_portId_999_buffer_4096_timeout_5000', 0, async () => {
      hilog.info(0x0000, TAG, 'testRead_portId_999_buffer_4096_timeout_5000 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readBuffer: Uint8Array = new Uint8Array(4096);
          await serialManager.read(999, readBuffer, 5000);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testRead_portId_999_buffer_4096_timeout_5000 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.read).assertUndefined();
      }
    })

    it('testRead_portId_neg1_buffer_64_timeout_500', 0, async () => {
      hilog.info(0x0000, TAG, 'testRead_portId_neg1_buffer_64_timeout_500 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readBuffer: Uint8Array = new Uint8Array(64);
          await serialManager.read(-1, readBuffer, 500);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testRead_portId_neg1_buffer_64_timeout_500 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.read).assertUndefined();
      }
    })

    it('testRead_portId_2_buffer_16_timeout_0', 0, async () => {
      hilog.info(0x0000, TAG, 'testRead_portId_2_buffer_16_timeout_0 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readBuffer: Uint8Array = new Uint8Array(16);
          await serialManager.read(2, readBuffer, 0);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testRead_portId_2_buffer_16_timeout_0 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.read).assertUndefined();
      }
    })

    it('testRead_portId_3_buffer_32_timeout_100', 0, async () => {
      hilog.info(0x0000, TAG, 'testRead_portId_3_buffer_32_timeout_100 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readBuffer: Uint8Array = new Uint8Array(32);
          await serialManager.read(3, readBuffer, 100);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testRead_portId_3_buffer_32_timeout_100 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.read).assertUndefined();
      }
    })

    it('testReadSync_portId_0_buffer_8_timeout_0', 0, () => {
      hilog.info(0x0000, TAG, 'testReadSync_portId_0_buffer_8_timeout_0 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readSyncBuffer: Uint8Array = new Uint8Array(8);
          let length: number = serialManager.readSync(0, readSyncBuffer, 0);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testReadSync_portId_0_buffer_8_timeout_0 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.readSync).assertUndefined();
      }
    })

    it('testReadSync_portId_1_buffer_1024_timeout_100', 0, () => {
      hilog.info(0x0000, TAG, 'testReadSync_portId_1_buffer_1024_timeout_100 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readSyncBuffer: Uint8Array = new Uint8Array(1024);
          let length: number = serialManager.readSync(1, readSyncBuffer, 100);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testReadSync_portId_1_buffer_1024_timeout_100 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.readSync).assertUndefined();
      }
    })

    it('testReadSync_portId_100_buffer_8192_timeout_1000', 0, () => {
      hilog.info(0x0000, TAG, 'testReadSync_portId_100_buffer_8192_timeout_1000 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readSyncBuffer: Uint8Array = new Uint8Array(8192);
          let length: number = serialManager.readSync(100, readSyncBuffer, 1000);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testReadSync_portId_100_buffer_8192_timeout_1000 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.readSync).assertUndefined();
      }
    })

    it('testReadSync_portId_999_buffer_4096_timeout_5000', 0, () => {
      hilog.info(0x0000, TAG, 'testReadSync_portId_999_buffer_4096_timeout_5000 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readSyncBuffer: Uint8Array = new Uint8Array(4096);
          let length: number = serialManager.readSync(999, readSyncBuffer, 5000);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testReadSync_portId_999_buffer_4096_timeout_5000 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.readSync).assertUndefined();
      }
    })

    it('testReadSync_portId_neg1_buffer_64_timeout_500', 0, () => {
      hilog.info(0x0000, TAG, 'testReadSync_portId_neg1_buffer_64_timeout_500 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readSyncBuffer: Uint8Array = new Uint8Array(64);
          let length: number = serialManager.readSync(-1, readSyncBuffer, 500);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testReadSync_portId_neg1_buffer_64_timeout_500 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.readSync).assertUndefined();
      }
    })

    it('testReadSync_portId_2_buffer_16_timeout_0', 0, () => {
      hilog.info(0x0000, TAG, 'testReadSync_portId_2_buffer_16_timeout_0 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readSyncBuffer: Uint8Array = new Uint8Array(16);
          let length: number = serialManager.readSync(2, readSyncBuffer, 0);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testReadSync_portId_2_buffer_16_timeout_0 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.readSync).assertUndefined();
      }
    })

    it('testReadSync_portId_3_buffer_32_timeout_100', 0, () => {
      hilog.info(0x0000, TAG, 'testReadSync_portId_3_buffer_32_timeout_100 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readSyncBuffer: Uint8Array = new Uint8Array(32);
          let length: number = serialManager.readSync(3, readSyncBuffer, 100);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testReadSync_portId_3_buffer_32_timeout_100 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.readSync).assertUndefined();
      }
    })

    it('testWrite_portId_0_buffer_hello_timeout_0', 0, async () => {
      hilog.info(0x0000, TAG, 'testWrite_portId_0_buffer_hello_timeout_0 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeBuffer: Uint8Array = new Uint8Array(buffer.from('Hello', 'utf-8').buffer);
          await serialManager.write(0, writeBuffer, 0);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWrite_portId_0_buffer_hello_timeout_0 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.write).assertUndefined();
      }
    })

    it('testWrite_portId_1_buffer_helloworld_timeout_100', 0, async () => {
      hilog.info(0x0000, TAG, 'testWrite_portId_1_buffer_helloworld_timeout_100 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeBuffer: Uint8Array = new Uint8Array(buffer.from('Hello World', 'utf-8').buffer);
          await serialManager.write(1, writeBuffer, 100);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWrite_portId_1_buffer_helloworld_timeout_100 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.write).assertUndefined();
      }
    })

    it('testWrite_portId_100_buffer_1024_timeout_1000', 0, async () => {
      hilog.info(0x0000, TAG, 'testWrite_portId_100_buffer_1024_timeout_1000 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeBuffer: Uint8Array = new Uint8Array(1024);
          await serialManager.write(100, writeBuffer, 1000);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWrite_portId_100_buffer_1024_timeout_1000 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.write).assertUndefined();
      }
    })

    it('testWrite_portId_999_buffer_8192_timeout_5000', 0, async () => {
      hilog.info(0x0000, TAG, 'testWrite_portId_999_buffer_8192_timeout_5000 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeBuffer: Uint8Array = new Uint8Array(8192);
          await serialManager.write(999, writeBuffer, 5000);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWrite_portId_999_buffer_8192_timeout_5000 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.write).assertUndefined();
      }
    })

    it('testWrite_portId_neg1_buffer_64_timeout_500', 0, async () => {
      hilog.info(0x0000, TAG, 'testWrite_portId_neg1_buffer_64_timeout_500 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeBuffer: Uint8Array = new Uint8Array(64);
          await serialManager.write(-1, writeBuffer, 500);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWrite_portId_neg1_buffer_64_timeout_500 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.write).assertUndefined();
      }
    })

    it('testWrite_portId_2_buffer_test_timeout_0', 0, async () => {
      hilog.info(0x0000, TAG, 'testWrite_portId_2_buffer_test_timeout_0 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeBuffer: Uint8Array = new Uint8Array(buffer.from('Test', 'utf-8').buffer);
          await serialManager.write(2, writeBuffer, 0);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWrite_portId_2_buffer_test_timeout_0 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.write).assertUndefined();
      }
    })

    it('testWrite_portId_3_buffer_data_timeout_100', 0, async () => {
      hilog.info(0x0000, TAG, 'testWrite_portId_3_buffer_data_timeout_100 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeBuffer: Uint8Array = new Uint8Array(buffer.from('Data', 'utf-8').buffer);
          await serialManager.write(3, writeBuffer, 100);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWrite_portId_3_buffer_data_timeout_100 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.write).assertUndefined();
      }
    })

    it('testWriteSync_portId_0_buffer_hello_timeout_0', 0, () => {
      hilog.info(0x0000, TAG, 'testWriteSync_portId_0_buffer_hello_timeout_0 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeSyncBuffer: Uint8Array = new Uint8Array(buffer.from('Hello', 'utf-8').buffer);
          let length: number = serialManager.writeSync(0, writeSyncBuffer, 0);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWriteSync_portId_0_buffer_hello_timeout_0 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.writeSync).assertUndefined();
      }
    })

    it('testWriteSync_portId_1_buffer_helloworld_timeout_100', 0, () => {
      hilog.info(0x0000, TAG, 'testWriteSync_portId_1_buffer_helloworld_timeout_100 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeSyncBuffer: Uint8Array = new Uint8Array(buffer.from('Hello World', 'utf-8').buffer);
          let length: number = serialManager.writeSync(1, writeSyncBuffer, 100);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWriteSync_portId_1_buffer_helloworld_timeout_100 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.writeSync).assertUndefined();
      }
    })

    it('testWriteSync_portId_100_buffer_1024_timeout_1000', 0, () => {
      hilog.info(0x0000, TAG, 'testWriteSync_portId_100_buffer_1024_timeout_1000 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeSyncBuffer: Uint8Array = new Uint8Array(1024);
          let length: number = serialManager.writeSync(100, writeSyncBuffer, 1000);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWriteSync_portId_100_buffer_1024_timeout_1000 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.writeSync).assertUndefined();
      }
    })

    it('testWriteSync_portId_999_buffer_8192_timeout_5000', 0, () => {
      hilog.info(0x0000, TAG, 'testWriteSync_portId_999_buffer_8192_timeout_5000 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeSyncBuffer: Uint8Array = new Uint8Array(8192);
          let length: number = serialManager.writeSync(999, writeSyncBuffer, 5000);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWriteSync_portId_999_buffer_8192_timeout_5000 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.writeSync).assertUndefined();
      }
    })

    it('testWriteSync_portId_neg1_buffer_64_timeout_500', 0, () => {
      hilog.info(0x0000, TAG, 'testWriteSync_portId_neg1_buffer_64_timeout_500 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeSyncBuffer: Uint8Array = new Uint8Array(64);
          let length: number = serialManager.writeSync(-1, writeSyncBuffer, 500);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWriteSync_portId_neg1_buffer_64_timeout_500 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.writeSync).assertUndefined();
      }
    })

    it('testWriteSync_portId_2_buffer_test_timeout_0', 0, () => {
      hilog.info(0x0000, TAG, 'testWriteSync_portId_2_buffer_test_timeout_0 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeSyncBuffer: Uint8Array = new Uint8Array(buffer.from('Test', 'utf-8').buffer);
          let length: number = serialManager.writeSync(2, writeSyncBuffer, 0);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWriteSync_portId_2_buffer_test_timeout_0 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.writeSync).assertUndefined();
      }
    })

    it('testWriteSync_portId_3_buffer_data_timeout_100', 0, () => {
      hilog.info(0x0000, TAG, 'testWriteSync_portId_3_buffer_data_timeout_100 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeSyncBuffer: Uint8Array = new Uint8Array(buffer.from('Data', 'utf-8').buffer);
          let length: number = serialManager.writeSync(3, writeSyncBuffer, 100);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWriteSync_portId_3_buffer_data_timeout_100 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.writeSync).assertUndefined();
      }
    })

    it('testWriteSync_portId_4_buffer_128_timeout_500', 0, () => {
      hilog.info(0x0000, TAG, 'testWriteSync_portId_4_buffer_128_timeout_500 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeSyncBuffer: Uint8Array = new Uint8Array(128);
          let length: number = serialManager.writeSync(4, writeSyncBuffer, 500);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWriteSync_portId_4_buffer_128_timeout_500 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.writeSync).assertUndefined();
      }
    })

    it('testWriteSync_portId_5_buffer_256_timeout_1000', 0, () => {
      hilog.info(0x0000, TAG, 'testWriteSync_portId_5_buffer_256_timeout_1000 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeSyncBuffer: Uint8Array = new Uint8Array(256);
          let length: number = serialManager.writeSync(5, writeSyncBuffer, 1000);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWriteSync_portId_5_buffer_256_timeout_1000 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.writeSync).assertUndefined();
      }
    })

    it('testWriteRead_portId_4_buffer_128_timeout_500', 0, async () => {
      hilog.info(0x0000, TAG, 'testWriteRead_portId_4_buffer_128_timeout_500 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeBuffer: Uint8Array = new Uint8Array(128);
          await serialManager.write(4, writeBuffer, 500);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWriteRead_portId_4_buffer_128_timeout_500 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.write).assertUndefined();
      }
    })

    it('testWriteRead_portId_5_buffer_256_timeout_1000', 0, async () => {
      hilog.info(0x0000, TAG, 'testWriteRead_portId_5_buffer_256_timeout_1000 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let writeBuffer: Uint8Array = new Uint8Array(256);
          await serialManager.write(5, writeBuffer, 1000);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testWriteRead_portId_5_buffer_256_timeout_1000 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.write).assertUndefined();
      }
    })

    it('testReadWrite_portId_4_buffer_128_timeout_500', 0, async () => {
      hilog.info(0x0000, TAG, 'testReadWrite_portId_4_buffer_128_timeout_500 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readBuffer: Uint8Array = new Uint8Array(128);
          await serialManager.read(4, readBuffer, 500);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testReadWrite_portId_4_buffer_128_timeout_500 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.read).assertUndefined();
      }
    })

    it('testReadWrite_portId_5_buffer_256_timeout_1000', 0, async () => {
      hilog.info(0x0000, TAG, 'testReadWrite_portId_5_buffer_256_timeout_1000 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readBuffer: Uint8Array = new Uint8Array(256);
          await serialManager.read(5, readBuffer, 1000);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testReadWrite_portId_5_buffer_256_timeout_1000 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.read).assertUndefined();
      }
    })

    it('testReadWriteSync_portId_4_buffer_128_timeout_500', 0, () => {
      hilog.info(0x0000, TAG, 'testReadWriteSync_portId_4_buffer_128_timeout_500 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readSyncBuffer: Uint8Array = new Uint8Array(128);
          let length: number = serialManager.readSync(4, readSyncBuffer, 500);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testReadWriteSync_portId_4_buffer_128_timeout_500 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.readSync).assertUndefined();
      }
    })

    it('testReadWriteSync_portId_5_buffer_256_timeout_1000', 0, () => {
      hilog.info(0x0000, TAG, 'testReadWriteSync_portId_5_buffer_256_timeout_1000 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let readSyncBuffer: Uint8Array = new Uint8Array(256);
          let length: number = serialManager.readSync(5, readSyncBuffer, 1000);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testReadWriteSync_portId_5_buffer_256_timeout_1000 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.readSync).assertUndefined();
      }
    })

    it('testOpenClose_portId_4', 0, () => {
      hilog.info(0x0000, TAG, 'testOpenClose_portId_4 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.open(4);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testOpenClose_portId_4 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.open).assertUndefined();
      }
    })

    it('testOpenClose_portId_5', 0, () => {
      hilog.info(0x0000, TAG, 'testOpenClose_portId_5 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          serialManager.open(5);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testOpenClose_portId_5 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.open).assertUndefined();
      }
    })

    it('testGetSetAttribute_portId_4', 0, () => {
      hilog.info(0x0000, TAG, 'testGetSetAttribute_portId_4 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let attribute = serialManager.getAttribute(4);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testGetSetAttribute_portId_4 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.getAttribute).assertUndefined();
      }
    })

    it('testGetSetAttribute_portId_5', 0, () => {
      hilog.info(0x0000, TAG, 'testGetSetAttribute_portId_5 start');
      if (canIUse("SystemCapability.USB.USBManager.Serial")) {
        try {
          let attribute = serialManager.getAttribute(5);
          expect().assertFail();
        } catch (error) {
          hilog.info(0x0000, TAG, 'testGetSetAttribute_portId_5 error: %{public}s', JSON.stringify(error));
          expect(error != null).assertTrue();
          expect(error.code).assertEqual(14400001);
        }
      } else {
        expect(serialManager.getAttribute).assertUndefined();
      }
    })
  })
}
