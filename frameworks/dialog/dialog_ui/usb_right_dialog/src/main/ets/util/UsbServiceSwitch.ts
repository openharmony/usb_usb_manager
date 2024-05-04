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

import usbManagerService from '@ohos.usbManager';

const TAG: string = 'usbfunctionswitchwindow_UsbServiceSwitch';
const USBFUNCTION_NONE: number = 0;
const USBFUNCTION_MTP: number = 8;
const USBFUNCTION_PTP: number = 16;

class UsbServiceSwitch {
  private curFunc: number = USBFUNCTION_NONE;
  private tarFunc: number = USBFUNCTION_NONE;

  async serviceChoose(chooseId: number): Promise<void> {
    this.curFunc = usbManagerService.getCurrentFunctions();
    console.log(TAG + 'choose ' + chooseId + 'current' + this.curFunc);
    if (typeof (this.curFunc) === undefined) {
      console.error(TAG + 'undefined, error');
      return;
    }
    this.tarFunc = ~(USBFUNCTION_MTP | USBFUNCTION_PTP);
    /* signed number to unsigned number */
    this.tarFunc = this.tarFunc >>> 0;
    this.tarFunc = this.tarFunc & this.curFunc;

    if (chooseId === -1) {
      console.log(TAG + 'choose: charge only');
    } else if (chooseId === USBFUNCTION_MTP) {
      this.tarFunc = this.tarFunc | USBFUNCTION_MTP;
      console.log(TAG + 'choose: xfer file(MTP)');
    } else if (chooseId === USBFUNCTION_PTP) {
      this.tarFunc = this.tarFunc | USBFUNCTION_PTP;
      console.log(TAG + 'choose: xfer pic(PTP)');
    } else {
      console.log(TAG + 'choose error');
    }

    console.log(TAG + 'setFunctions: current ' + JSON.stringify(this.curFunc) + 'target: ' + JSON.stringify(this.tarFunc));
    if (this.tarFunc != this.curFunc) {
      usbManagerService.setCurrentFunctions(this.tarFunc).then((data) => {
        console.log(TAG + 'setCurrentFunctions success: ' + JSON.stringify(data));
      }).catch((err) => {
        console.error(TAG + 'setCurrentFunctions failed: ' + JSON.stringify(err));
      });
    }
    return;
  }
}

export let usbServiceSwitch = new UsbServiceSwitch();
