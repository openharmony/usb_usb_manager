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
import window from '@ohos.window';
import { BusinessError } from '@ohos.base';

const TAG: string = 'usbfunctionswitchwindow_UsbServiceSwitch';

interface UsbfunctionId {
  mtp: number;
  ptp: number;
  none: number;
}

let usbfunctionId: UsbfunctionId = {
  mtp: 8,
  ptp: 16,
  none: 0,
}

class UsbServiceSwitch {
  private curFunc: number = usbfunctionId.none;
  private tarFunc: number = usbfunctionId.none;

  async serviceChoose(chooseId: number): Promise<void> {
    this.curFunc = usbManager.getCurrentFunctions();
    Logger.info(TAG, `choose ${chooseId}, current ${this.curFunc}`);
    if (typeof (this.curFunc) == undefined) {
      Logger.error(TAG, 'undefined, error');
      return;
    }
    this.tarFunc = ~(usbfunctionId.mtp | usbfunctionId.ptp);
    /* signed number to unsigned number */
    this.tarFunc = this.tarFunc >>> 0;
    this.tarFunc = this.tarFunc & this.curFunc;

    if (chooseId === -1) {
      Logger.info(TAG, 'choose: charge only');
    } else if (chooseId === 8) {
      this.tarFunc = this.tarFunc | usbfunctionId.MTP;
      Logger.info(TAG, 'choose: xfer file(mtp)');
    } else if (chooseId === 16) {
      this.tarFunc = this.tarFunc | usbfunctionId.ptp;
      Logger.info(TAG, 'choose: xfer pic(ptp)');
    } else {
      Logger.info(TAG, 'choose error');
    }

    Logger.info(TAG, `setFunctions: current ${JSON.stringify(this.curFunc)}, target ${JSON.stringify(this.tarFunc)}`);
    usbManager.setCurrentFunctions(this.tarFunc).then((data: window.WindowEventType) => {
      Logger.info(TAG, `setCurrentFunctions success: ${JSON.stringify(data)}`);
    }).catch((err: BusinessError) => {
      Logger.error(TAG, `setCurrentFunctions failed:${JSON.stringify(err)}`);
    })
    return;
  }
}

export let usbServiceSwitch = new UsbServiceSwitch();
