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

import UIExtensionContentSession from '@ohos.app.ability.UIExtensionContentSession'
import { usbServiceSwitch } from '../util/UsbServiceSwitch';
import { terminateDialog } from '../util/DialogUtil';
import {GlobalThisProxy} from '../util/GlobalThisProxy';
import { BusinessError } from '@ohos.base';
import window from '@ohos.window';
import common from '@ohos.app.ability.common'
import Want from '@ohos.app.ability.Want';
import deviceInfo from '@ohos.deviceInfo';
import { SelectDialog } from '../components/dialog';
let TAG: string = 'UsbService';
let USBSERVICESWITCH_ID_DEFAULT : number = -1;
let USBSERVICESWITCH_ID_CHARGE_ONLY : number = -1;
let USBSERVICESWITCH_ID_XFER_FILE : number = 8;
let USBSERVICESWITCH_ID_XFER_PIC : number = 16;

@Entry
@Component
struct IndexHapComponent {
  private currentChoice: number = 2;
  private content: string = ''
  private thisWant: string = ''
  @State mtpTip: ResourceStr = $r('app.string.USB_func_mtp_tip');
  dialogControllerList: CustomDialogController = new CustomDialogController({
    builder: SelectDialog({
      title: $r('app.string.USB_hint'),
      selectedIndex: this.currentChoice,
      confirm: {
        value: $r('app.string.USB_hint_cancel'),
        action: () => {
          terminateDialog();
        },
      },
      radioContent: [
        {
          title: $r('app.string.USB_func_mtp'),
          subTitle: this.mtpTip,
          action: () => {
            let tmpChooseNumber:number = USBSERVICESWITCH_ID_XFER_FILE;
            this.setUsbFunction(tmpChooseNumber);
          }
        },
        {
          title: $r('app.string.USB_func_ptp'),
          subTitle: $r('app.string.USB_func_ptp_tip'),
          action: () => {
            let tmpChooseNumber:number = USBSERVICESWITCH_ID_XFER_PIC;
            this.setUsbFunction(tmpChooseNumber);
          }
        },
        {
          title: $r('app.string.USB_func_charge'),
          action: () => {
            let tmpChooseNumber:number = USBSERVICESWITCH_ID_CHARGE_ONLY;
            this.setUsbFunction(tmpChooseNumber);
          }
        },
      ]
    }),
    alignment: DialogAlignment.Center,
    autoCancel: true,
    cancel: this.onCancel
  });

  setUsbFunction(choose: number) {
    usbServiceSwitch.serviceChoose(choose);
    terminateDialog();
  }

  async aboutToAppear() {
    if (deviceInfo.deviceType === 'tablet') {
      this.mtpTip = $r('app.string.USB_func_mtp_tip2', $r('app.string.USB_my_tablet'));
    }

    setTimeout(() => {
      this.dialogControllerList.open();
    });
  }

  async aboutToDisappear() {
    console.log(TAG + 'aboutToDisappear');
  }

  async onPageShow() {
    console.log(TAG + 'onPageShow, currentChoice: ' + this.currentChoice);
  }

  async onPageHide() {
    console.log(TAG + 'onPageHide');
  }
  async onCancel() {
    console.log(TAG + 'onCancel');
    terminateDialog();
  }
  build() { }
}
