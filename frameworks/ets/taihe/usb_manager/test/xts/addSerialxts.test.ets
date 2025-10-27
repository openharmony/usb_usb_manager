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

import { describe, it, expect, TestType, Size, Level, beforeAll, afterAll} from "../../../hypium/index";
import { Driver, MatchPattern, ON } from '@ohos.UiTest'
import { BusinessError } from '@ohos.base';
import serialManager from '@ohos.usbManager.serial';
import json from '@ohos.util.json';
import bundleManager from '@ohos.bundle.bundleManager';
import Utils from './Util.test';
import hilog from '@ohos.hilog'
import buffer from '@ohos.buffer';
const TAG: string = "[SerialTest]";
let domain: number = 0x0000;

function sleep(count:int) : Promise<int> {
  return new Promise<int>((resolve, reject) => {
    setTimeout(() => {
      resolve(0)
    }, count)
  })
}

// async function driveFn() {
//   console.info(TAG, '**************driveFn**************');
//   try {
//     console.info(TAG, '**************Driver.create() start**************');
//     let driver: Driver = Driver.create();
//     await driver.delayMs(1000)
//     console.info(TAG, '**************Driver.create() end**************');
//     await sleep(1000);
//     let buttons = await driver.findComponents(ON.text('允许'));
//     if (buttons && buttons.length > 0) {
//       console.info(TAG, 'driver click');
//       await buttons[buttons.length - 1].click();
//     }
//   } catch (error) {
//     console.info(TAG, 'driveFn error: ' + error);
//   }
// }

async function driveDecFn() {
  console.info('**************driveDecFn**************');
  try {
    let driver = Driver.create();
    await sleep(1000);
    let buttons = await driver.findComponents(ON.text('不允许', MatchPattern.CONTAINS));
    console.info(TAG, 'driveDecFn button:', json.stringify(buttons));
    if (buttons && buttons.length > 0) {
      await buttons[buttons.length - 1].click();
    }
  } catch (error) {
    console.info(TAG, 'driveDecFn error: ' + error);
  }
}

function checkDevice() {
  let portList: serialManager.SerialPort[] = serialManager.getPortList();
  if (portList && portList.length > 0) {
    console.info(TAG, 'Test serial device detected');
    return true;
  } else {
    console.info(TAG, 'Test no serial device detected');
    return false;
  }
}

export default function SerialBaseTest() {
  describe("SerialBaseTest", (): void => {
    console.info(TAG, 'describe start');
    let openPortId: int = -1;
    beforeAll(()=> {
      try {
        let portList: serialManager.SerialPort[] = serialManager.getPortList();
        if (portList && portList.length > 0) {
          openPortId = portList[0].portId;
          console.info(TAG, 'openPortId: ', openPortId);
          let hasRight: boolean = serialManager.hasSerialRight(openPortId);
          if (!hasRight) {
            console.info(TAG, 'beforeEach: requestSerialRight start');
            let futureRes: Promise<boolean> = serialManager.requestSerialRight(openPortId);
            // await driveFn();
            try {
              let hasRight: boolean = await futureRes;
              console.info(TAG, 'getPermission isAgree: ', hasRight);
            } catch (error) {
              console.error(TAG, 'getPermission catch error: ', error);
            }
          }
        } else {
          console.info(TAG, 'portList size error: ', portList.length);
        }
      } catch (error) {
        console.info(TAG, 'beforeAll error: ', error);
      }
    })
    afterAll(async () => {
      console.info(TAG, 'afterAll');
      if (serialManager.hasSerialRight(openPortId)){
        serialManager.close(openPortId);
        serialManager.cancelSerialRight(openPortId);
      }
    })

    it('testHasSerialRight001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        console.info(TAG, 'testHasSerialRight002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let invalidInput: int = -1;
        let hasRight: boolean = serialManager.hasSerialRight(invalidInput);
        console.info(TAG, 'testHasSerialRight002 hasRight: ', hasRight);
        expect().assertFail();
      } catch (error) {
        const err = error as BusinessError;
        expect(err.code).assertEqual(31400001);
        done();
      }
    })

    it('testRequestSerialRight002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try{
        console.info(TAG, 'testRequestSerialRight002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let invalidInput: int = -1;
        await serialManager.requestSerialRight(invalidInput);
        // await driveFn();
        try {
          expect().assertFail();
        } catch (err) {
          console.error(TAG, 'getPermission catch err:',json.stringify(err));
          expect().assertFail();
        }
      } catch (error) {
        const err = error as BusinessError;
        expect(err.code).assertEqual(31400001);
        console.error(TAG, 'testRequestSerialRight002 catch err code: ', err.code, ', message: ', err.message);
        done();
      }
    })

    it('testRequestSerialRight003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try{
        console.info(TAG, 'testRequestSerialRight002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let invalidInput: int = -1;
        await serialManager.requestSerialRight(invalidInput);
        // await driveFn();
        try {
          expect().assertFail();
        } catch (err) {
          console.error(TAG, 'getPermission catch err:',json.stringify(err));
          expect().assertFail();
        }
      } catch (error) {
        const err = error as BusinessError;
        expect(err.code).assertEqual(14400005);
        console.error(TAG, 'testRequestSerialRight002 catch err code: ', err.code, ', message: ', err.message);
        done();
      }
    })

    it('testOpen004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        console.info(TAG, 'testOpen005 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        serialManager.cancelSerialRight(openPortId);
        serialManager.open(openPortId);
        expect().assertFail();
      } catch (error) {
        const err = error as BusinessError;
        console.error(TAG, 'testOpen005 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(14400005);
        done();
      }
    })


    it('testOpen005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        console.info(TAG, 'testOpen005 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        serialManager.cancelSerialRight(openPortId);
        serialManager.open(openPortId);
        expect().assertFail();
      } catch (error) {
        const err = error as BusinessError;
        console.error(TAG, 'testOpen005 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(31400001);
        done();
      }
    })

    it('testOpen006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        console.info(TAG, 'testOpen005 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        serialManager.cancelSerialRight(openPortId);
        serialManager.open(openPortId);
        expect().assertFail();
      } catch (error) {
        const err = error as BusinessError;
        console.error(TAG, 'testOpen005 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(31400001);
        done();
      }
    })

    it('testClose007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        console.info(TAG, 'testClose003 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let invalidPort: int = -1;
        serialManager.open(openPortId);
        serialManager.close(invalidPort);
        expect().assertFail();
      } catch (error) {
        const err = error as BusinessError;
        console.error(TAG, 'testClose003 catch err code: ', err.code, ', message: ', err.message);
        expect(err.code).assertEqual(31400001);
        serialManager.close(openPortId);
        done();
      }
    })

    it('testGetAttribute008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0,
      async (done: () => void): Promise<void> => {
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        try {
          console.info(TAG, 'testGetAttribute001 start');
          // if (await checkDevice() === false) {
          //   done();
          //   return;
          // }
          let attribute: serialManager.SerialAttribute = {
            baudRate: serialManager.BaudRates.BAUDRATE_9600,
            dataBits: serialManager.DataBits.DATABIT_8,
            parity: serialManager.Parity.PARITY_NONE,
            stopBits: serialManager.StopBits.STOPBIT_2
          };
          serialManager.open(openPortId);
          serialManager.setAttribute(openPortId, attribute);
          expect(serialManager.getAttribute(openPortId).stopBits).assertEqual(serialManager.StopBits.STOPBIT_2);
          serialManager.close(openPortId);
          done();
        } catch (error) {
          if (!(error instanceof BusinessError)) {
            throw error;
          }
          console.error(TAG, 'testGetAttribute001 catch err code: ', error.code, ', message: ', error.message);
          expect(error.code).assertEqual(31400001);
        }
      })

    it('testReadSync009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      await Utils.msSleep(1000);
      if (checkDevice() === false) {
        done();
        return Promise.resolve();
      }
      try {
        console.info(TAG, 'testReadSync002 start');
        // if (await checkDevice() === false) {
        //   done();
        //   return;
        // }
        serialManager.open(openPortId);
        let readSyncBuffer: Uint8Array = new Uint8Array(8192);
        let length: int = serialManager.readSync(openPortId, readSyncBuffer, 500);
        console.info(TAG, 'testReadSync002 length = ' + length);
        expect(length >= 0).assertTrue();
        serialManager.close(openPortId);
        done();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        console.error(TAG, 'testReadSync002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400001);
      }
    })

    it('testReadSync0010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      await Utils.msSleep(1000);
      if (checkDevice() === false) {
        done();
        return Promise.resolve();
      }
      try {
        console.info(TAG, 'testReadSync002 start');
        // if (await checkDevice() === false) {
        //   done();
        //   return;
        // }
        serialManager.open(openPortId);
        let readSyncBuffer: Uint8Array = new Uint8Array(8192);
        let length: int = serialManager.readSync(openPortId, readSyncBuffer, 500);
        console.info(TAG, 'testReadSync002 length = ' + length);
        expect(length >= 0).assertTrue();
        serialManager.close(openPortId);
        done();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        console.error(TAG, 'testReadSync002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400007);
      }
    })

    it('testReadSync011', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0,
      async (done: () => void): Promise<void> => {
        await Utils.msSleep(1000);
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        try {
          console.info(TAG, 'testReadSync002 start');
          // if (await checkDevice() === false) {
          //   done();
          //   return;
          // }
          serialManager.open(openPortId);
          let readSyncBuffer: Uint8Array = new Uint8Array(8192);
          let length: int = serialManager.readSync(openPortId, readSyncBuffer, 500);
          console.info(TAG, 'testReadSync002 length = ' + length);
          expect(length >= 0).assertTrue();
          serialManager.close(openPortId);
          done();
        } catch (error) {
          if (!(error instanceof BusinessError)) {
            throw error;
          }
          console.error(TAG, 'testReadSync002 catch err code: ', error.code, ', message: ', error.message);
          expect(error.code).assertEqual(31400007);
        }
      })

    it('testReadSync012', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0,
      async (done: () => void): Promise<void> => {
        await Utils.msSleep(1000);
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        try {
          console.info(TAG, 'testReadSync002 start');
          // if (await checkDevice() === false) {
          //   done();
          //   return;
          // }
          serialManager.open(openPortId);
          let readSyncBuffer: Uint8Array = new Uint8Array(8192);
          let length: int = serialManager.readSync(openPortId, readSyncBuffer, 500);
          console.info(TAG, 'testReadSync002 length = ' + length);
          expect(length >= 0).assertTrue();
          serialManager.close(openPortId);
          done();
        } catch (error) {
          if (!(error instanceof BusinessError)) {
            throw error;
          }
          console.error(TAG, 'testReadSync002 catch err code: ', error.code, ', message: ', error.message);
          expect(error.code).assertEqual(31400001);
        }
      })

    it('testWrite0013', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      console.info(TAG, 'testWrite001 start');
      if (checkDevice() === false) {
        done();
        return Promise.resolve();
      }
      let writeBuffer: Uint8Array = new Uint8Array(buffer.from('Hello World', 'utf8').buffer);
      //let writeBuffer: Uint8Array = new Uint8Array(8192);
      serialManager.open(openPortId);
      serialManager.write(openPortId, writeBuffer, 500)
        .then((length: int) => {
          expect(length >= 0).assertTrue();
          serialManager.close(openPortId);
          done();
        })
        .catch((error: Error) => {
          console.error(TAG, 'testWrite001 catch err name: ', error.name, ', message: ', error.message);
          expect(error.code).assertEqual(31400001);
        });
    })

    it('testWrite014', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      console.info(TAG, 'testWrite001 start');
      if (checkDevice() === false) {
        done();
        return Promise.resolve();
      }
      let writeBuffer: Uint8Array = new Uint8Array(buffer.from('Hello World', 'utf8').buffer);
      //let writeBuffer: Uint8Array = new Uint8Array(8192);
      serialManager.open(openPortId);
      serialManager.write(openPortId, writeBuffer, 500)
        .then((length: int) => {
          expect(length >= 0).assertTrue();
          serialManager.close(openPortId);
          done();
        })
        .catch((error: Error) => {
          console.error(TAG, 'testWrite001 catch err name: ', error.name, ', message: ', error.message);
          expect(error.code).assertEqual(31400006);
        });
    })

    it('testWrite015', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      console.info(TAG, 'testWrite001 start');
      if (checkDevice() === false) {
        done();
        return Promise.resolve();
      }
      let writeBuffer: Uint8Array = new Uint8Array(buffer.from('Hello World', 'utf8').buffer);
      //let writeBuffer: Uint8Array = new Uint8Array(8192);
      serialManager.open(openPortId);
      serialManager.write(openPortId, writeBuffer, 500)
        .then((length: int) => {
          expect(length >= 0).assertTrue();
          serialManager.close(openPortId);
          done();
        })
        .catch((error: Error) => {
          console.error(TAG, 'testWrite001 catch err name: ', error.name, ', message: ', error.message);
          expect(error.code).assertEqual(31400007);
        });
    })



    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute016', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      if (checkDevice() === false) {
        done();
        return Promise.resolve();
      }
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_50,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute017', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      if (checkDevice() === false) {
        done();
        return Promise.resolve();
      }
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_75,
          dataBits: serialManager.DataBits.DATABIT_7,
          parity: serialManager.Parity.PARITY_ODD,
          stopBits: serialManager.StopBits.STOPBIT_2
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute018', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      if (checkDevice() === false) {
        done();
        return Promise.resolve();
      }
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_110,
          dataBits: serialManager.DataBits.DATABIT_6,
          parity: serialManager.Parity.PARITY_EVEN,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute019', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      if (checkDevice() === false) {
        done();
        return Promise.resolve();
      }
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_134,
          dataBits: serialManager.DataBits.DATABIT_5,
          parity: serialManager.Parity.PARITY_MARK,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute020', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_150,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_SPACE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute021', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_200,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute022', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_300,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute023', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_600,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute024', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_1200,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute025', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_1800,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute026', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_2400,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute027', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_4800,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute028', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_19200,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute029', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_38400,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute030', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_57600,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute031', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_115200,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute032', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_230400,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute033', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_460800,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute034', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_500000,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute035', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_576000,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute036', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_921600,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute037', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_1000000,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute038', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_1152000,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute039', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_1500000,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute040', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_2000000,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute041', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_2500000,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute042', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_3000000,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute043', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_3500000,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_setAttribute_ErrCode_0100
     * @tc.name     : testSetAttribute002
     * @tc.desc     : not openPortId to setAttribute.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testSetAttribute044', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testSetAttribute002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let attribute: serialManager.SerialAttribute = {
          baudRate: serialManager.BaudRates.BAUDRATE_4000000,
          dataBits: serialManager.DataBits.DATABIT_8,
          parity: serialManager.Parity.PARITY_NONE,
          stopBits: serialManager.StopBits.STOPBIT_1
        };
        serialManager.setAttribute(openPortId, attribute);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testSetAttribute002 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    it('testRead045', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      console.info(TAG, 'testRead002 start');
      await Utils.msSleep(3000);
      if (checkDevice() === false) {
        done();
        return Promise.resolve();
      }
      let readBuffer: Uint8Array = new Uint8Array(8192);
      try {
        serialManager.open(openPortId);
        serialManager.read(openPortId, readBuffer, 1000)
          .then((length: int) => {
            console.info(TAG, 'testRead002 length = ' + length);
            expect(length >= 0).assertTrue();
            serialManager.close(openPortId);
          })
          .catch((error: Error) => {
            expect(error.code).assertEqual(31400001);
            console.error(TAG, 'testRead002 catch err name: ', error.name, ', message: ', error.message);
          });
      }
      catch(error){
        console.error(TAG, 'testRead002 catch err');
      }
    })

    it('testRead046', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      console.info(TAG, 'testRead002 start');
      await Utils.msSleep(3000);
      if (checkDevice() === false) {
        done();
        return Promise.resolve();
      }
      let readBuffer: Uint8Array = new Uint8Array(8192);
      try {
        serialManager.open(openPortId);
        serialManager.read(openPortId, readBuffer, 1000)
          .then((length: int) => {
            console.info(TAG, 'testRead002 length = ' + length);
            expect(length >= 0).assertTrue();
            serialManager.close(openPortId);
          })
          .catch((error: Error) => {
            expect(error.code).assertEqual(31400007);
            console.error(TAG, 'testRead002 catch err name: ', error.name, ', message: ', error.message);
          });
      }
      catch(error){
        console.error(TAG, 'testRead002 catch err');
      }
    })

    it('testWriteSync047', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      if (checkDevice() === false) {
        done();
        return Promise.resolve();
      }
      try {
        console.info(TAG, 'testWriteSync001 start');
        let writeSyncBuffer: Uint8Array = new Uint8Array(8192);
        serialManager.open(openPortId);
        let length: int = serialManager.writeSync(openPortId, writeSyncBuffer, 500);
        expect(length >= 0).assertTrue();
        serialManager.close(openPortId);
        done();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        console.error(TAG, 'testWriteSync001 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400001);
      }
    })

    it('testWriteSync048', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      if (checkDevice() === false) {
        done();
        return Promise.resolve();
      }
      try {
        console.info(TAG, 'testWriteSync001 start');
        let writeSyncBuffer: Uint8Array = new Uint8Array(8192);
        serialManager.open(openPortId);
        let length: int = serialManager.writeSync(openPortId, writeSyncBuffer, 500);
        expect(length >= 0).assertTrue();
        serialManager.close(openPortId);
        done();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        console.error(TAG, 'testWriteSync001 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400006);
      }
    })

    it('testWriteSync049', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      if (checkDevice() === false) {
        done();
        return Promise.resolve();
      }
      try {
        console.info(TAG, 'testWriteSync001 start');
        let writeSyncBuffer: Uint8Array = new Uint8Array(8192);
        serialManager.open(openPortId);
        let length: int = serialManager.writeSync(openPortId, writeSyncBuffer, 500);
        expect(length >= 0).assertTrue();
        serialManager.close(openPortId);
        done();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        console.error(TAG, 'testWriteSync001 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400007);
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_read_Func_0100
     * @tc.name     : testRead002
     * @tc.desc     : read successfully.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 0
     */
    it('testRead050', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      if (checkDevice() === false) {
        done();
        return Promise.resolve();
      }
      try {
        hilog.info(domain, TAG, 'testRead002 start');
        await Utils.msSleep(3000);
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }

        let readBuffer: Uint8Array = new Uint8Array(8192);
        serialManager.open(openPortId);
        let length = await serialManager.read(openPortId, readBuffer);
        expect(length >= 0).assertTrue();
        serialManager.close(openPortId);
        done();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testRead002 catch err name: ', error.name, ', message: ', error.message);
        expect().assertFail();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_readSync_ErrCode_0200
     * @tc.name     : testReadSync003
     * @tc.desc     : port not open.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testReadSync051', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      await Utils.msSleep(1000);
      try {
        hilog.info(domain, TAG, 'testReadSync003 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let readSyncBuffer: Uint8Array = new Uint8Array(8192);
        serialManager.readSync(openPortId, readSyncBuffer);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testReadSync003 catch err code: ', error.code, ', message: ', error.message);
        expect(error.code).assertEqual(31400005);
        done();
      }
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_write_ErrCode_0300
     * @tc.name     : testWrite004
     * @tc.desc     : not exist PortId to write.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 3
     */
    it('testWrite052', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try{
        hilog.info(domain, TAG, 'testWrite004 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let invalidport: int = 100;
        let writeBuffer: Uint8Array = new Uint8Array(100).fill(1);
        await serialManager.write(invalidport, writeBuffer);
        expect().assertFail();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testWrite004 catch err name: ', error);
        expect(error.code).assertEqual(31400003);
        done();
      };
    })

    /**
     * @tc.number   : SUB_USB_HostMgr_Serial_JS_writeSync_Func_0100
     * @tc.name     : testWriteSync001
     * @tc.desc     : writeSync successfully.
     * @tc.size     : MediumTest
     * @tc.type     : Function
     * @tc.level    : Level 0
     */
    it('testWriteSync053', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        hilog.info(domain, TAG, 'testWriteSync001 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let writeSyncBuffer: Uint8Array = new Uint8Array(100).fill(1);
        serialManager.open(openPortId);
        let length: int = serialManager.writeSync(openPortId, writeSyncBuffer);
        expect(length >= 0).assertTrue();
        serialManager.close(openPortId);
        done();
      } catch (error) {
        if (!(error instanceof BusinessError)) {
          throw error;
        }
        hilog.error(domain, TAG, 'testWriteSync001 catch err code: ', error.code, ', message: ', error.message);
        expect().assertFail();
      };
    })

    it('testHasSerialRight054', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      try {
        console.info(TAG, 'testHasSerialRight002 start');
        if (checkDevice() === false) {
          done();
          return Promise.resolve();
        }
        let invalidInput: int = -1;
        let hasRight: boolean = serialManager.hasSerialRight(invalidInput);
        console.info(TAG, 'testHasSerialRight002 hasRight: ', hasRight);
        expect().assertFail();
      } catch (error) {
        const err = error as BusinessError;
        expect(err.code).assertEqual(14400005);
        done();
      }
    })
  })
}