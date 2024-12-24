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
import {GlobalThisProxy} from '../util/GlobalThisProxy';
import { BusinessError } from '@ohos.base';
import window from '@ohos.window';
import common from '@ohos.app.ability.common'
import Want from '@ohos.app.ability.Want';
import { SelectDialog } from '@ohos.arkui.advanced.Dialog'
let TAG: string = 'UsbService';
let USBSERVICESWITCH_ID_DEFAULT : number = -1;
let USBSERVICESWITCH_ID_CHARGE_ONLY : number = -1;
let USBSERVICESWITCH_ID_XFER_FILE : number = 8;
let USBSERVICESWITCH_ID_XFER_PIC : number = 16;

@Entry
@Component
struct IndexHapComponent {
  private curChoose: number = 0;
  private content: string = ''
  private thisWant: string = ''
  dialogControllerList: CustomDialogController = new CustomDialogController({
    builder: SelectDialog({
      title: $r('app.string.USB_hint'),
      selectedIndex: this.curChoose,
      confirm: {
        value: $r('app.string.USB_hint_cancel'),
        action: () => {
          let session = AppStorage.get<UIExtensionContentSession>('session');
          if (session) {
            session.terminateSelf();
          }
        },
      },
      radioContent: [
        {
          title: $r('app.string.USB_func_charge'),
          action: () => {
            let tmpChooseNumber:number = USBSERVICESWITCH_ID_CHARGE_ONLY;
            usbServiceSwitch.serviceChoose(tmpChooseNumber);
            let session = AppStorage.get<UIExtensionContentSession>('session');
            if (session) {
              session.terminateSelf();
            }
          }
        },
      ]
    }),
    alignment: DialogAlignment.Center,
    autoCancel: true,
    cancel: this.onCancel
  });

  async aboutToAppear() {
    setTimeout(() => {
      this.dialogControllerList.open();
    });
  }

  async aboutToDisappear() {
    console.log(TAG + 'aboutToDisappear');
  }

  async onPageShow() {
    console.log(TAG + 'onPageShow, curChoose: ' + this.curChoose);
  }

  async onPageHide() {
    console.log(TAG + 'onPageHide');
  }
  async onCancel() {
    console.log(TAG + 'onCancel');
    let session = AppStorage.get<UIExtensionContentSession>('session');
    if (session) {
      session.terminateSelf();
    }
  }
  build() { }
}