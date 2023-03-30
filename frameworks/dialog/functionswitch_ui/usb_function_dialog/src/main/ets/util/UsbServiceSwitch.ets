/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import Logger from '../util/Logger';
import usbManager from '@ohos.usbManager';

const TAG: string = 'usbfunctionswitchwindow_UsbServiceSwitch';

const USBFUNCTION_ID = {
  MTP: 8,
  PTP: 16,
  NONE: 0,
}

class UsbServiceSwitch {
  private curFunc: number = USBFUNCTION_ID.NONE;
  private tarFunc: number = USBFUNCTION_ID.NONE;

  async serviceChoose(chooseId: number): Promise<void> {
    this.curFunc = usbManager.getCurrentFunctions();
    Logger.info(TAG, `choose ${chooseId}, current ${this.curFunc}`);
    if (typeof (this.curFunc) == undefined) {
      Logger.error(TAG, 'undefined, error');
      return;
    }
    this.tarFunc = ~(USBFUNCTION_ID.MTP | USBFUNCTION_ID.PTP);
    /* signed number to unsigned number */
    this.tarFunc = this.tarFunc >>> 0;
    this.tarFunc = this.tarFunc & this.curFunc;

    if (chooseId === -1) {
      Logger.info(TAG, 'choose: charge only');
    } else if (chooseId === 8) {
      this.tarFunc = this.tarFunc | USBFUNCTION_ID.MTP;
      Logger.info(TAG, 'choose: xfer file(MTP)');
    } else if (chooseId === 16) {
      this.tarFunc = this.tarFunc | USBFUNCTION_ID.PTP;
      Logger.info(TAG, 'choose: xfer pic(PTP)');
    } else {
      Logger.info(TAG, 'choose error');
    }

    Logger.info(TAG, `setFunctions: current ${JSON.stringify(this.curFunc)}, target ${JSON.stringify(this.tarFunc)}`);
    usbManager.setCurrentFunctions(this.tarFunc).then((data) => {
      Logger.info(TAG, `setCurrentFunctions success: ${JSON.stringify(data)}`);
    }).catch((err) => {
      Logger.error(TAG, `setCurrentFunctions failed:${JSON.stringify(err)}`);
    })
    return;
  }
}

export let usbServiceSwitch = new UsbServiceSwitch();
