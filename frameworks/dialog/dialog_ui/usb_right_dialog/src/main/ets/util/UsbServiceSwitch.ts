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
const USBFUNCTION_HDC: number = 4;
const USBFUNCTION_MTP: number = 8;
const USBFUNCTION_PTP: number = 16;
const USBFUNCTION_STORAGE: number = 512;
const USBFUNCTION_DEVMODE_AUTH: number = 4096;
const supportedCompositeFuncs: Array<number> = [
  USBFUNCTION_HDC
];

class UsbServiceSwitch {
  private curFunc: number = USBFUNCTION_NONE;
  private tarFunc: number = USBFUNCTION_NONE;

  getCurrentFunctions(): number {
    this.curFunc = usbManagerService.getCurrentFunctions();
    console.log(TAG + 'current' + this.curFunc);
    return this.curFunc;
  }

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
      return;
    } else if (chooseId === USBFUNCTION_NONE) {
      this.tarFunc = (this.tarFunc | USBFUNCTION_NONE) & (~USBFUNCTION_STORAGE);
      if (this.tarFunc === USBFUNCTION_NONE) {
        this.tarFunc = USBFUNCTION_STORAGE;
      }
      console.log(TAG + 'choose: xfer file(NONE)');
    } else if (chooseId === USBFUNCTION_MTP) {
      this.tarFunc = supportedCompositeFuncs.includes(this.tarFunc) ? (this.tarFunc | USBFUNCTION_MTP) : USBFUNCTION_MTP;
      console.log(TAG + 'choose: xfer file(MTP)');
    } else if (chooseId === USBFUNCTION_PTP) {
      this.tarFunc = supportedCompositeFuncs.includes(this.tarFunc) ? (this.tarFunc | USBFUNCTION_PTP) : USBFUNCTION_PTP;
      console.log(TAG + 'choose: xfer pic(PTP)');
    } else {
      console.log(TAG + 'choose error');
      return;
    }

    this.tarFunc = this.tarFunc & (~USBFUNCTION_DEVMODE_AUTH); // remove dev auth
    console.log(TAG + 'setFunctions: current ' + JSON.stringify(this.curFunc) + 'target: ' + JSON.stringify(this.tarFunc));
    if (this.tarFunc !== this.curFunc) {
      usbManagerService.setCurrentFunctions(this.tarFunc).then((data) => {
        console.log(TAG + 'setCurrentFunctions success: ' + JSON.stringify(data));
      }).catch((err) => {
        console.error(TAG + 'setCurrentFunctions failed: ' + JSON.stringify(err));
      });
    }
    return;
  }

  setPortRole(portId: number, powerRole: number, dataRole: number): Promise<void> {
    console.log(TAG + 'setPortRoles portId ' + portId + ' powerRole ' + powerRole + ' dataRole ' + dataRole);
    usbManagerService.setPortRoles(portId, powerRole, dataRole).then(() => {
      console.log(TAG + 'setPortRoles success: ');
      return;
    }).catch((err) => {
      console.error(TAG + 'setPortRoles failed: ' + JSON.stringify(err));
      return;
    });
  }
  getPorts(): Array<usbManagerService.USBPort> {
    return usbManagerService.getPorts();
  }
}
export let usbServiceSwitch = new UsbServiceSwitch();
