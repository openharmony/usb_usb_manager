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
let TAG: string = 'UsbService';
let USBSERVICESWITCH_ID_DEFAULT : number = -1;
let USBSERVICESWITCH_ID_CHARGE_ONLY : number = -1;
let USBSERVICESWITCH_ID_XFER_FILE : number = 8;
let USBSERVICESWITCH_ID_XFER_PIC : number = 16;
const FIRST_ITEM_INDEX: number = 0;
const LIST_MIN_HEIGHT: number = 48;
@CustomDialog
  export default
  struct PopUpWindow {
  @State curSelectNumber: number = USBSERVICESWITCH_ID_DEFAULT;
  customDialogController: CustomDialogController;
  radioContent: Array<SheetInfo> = [];

  build() {
    Column() {
      Row() {
        Text($r('app.string.USB_hint'))
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .textAlign(TextAlign.Start)
          .textOverflow({overflow: TextOverflow.Ellipsis})
          .fontFamily('HarmonyHeiTi')
      }
        .height(56)
        .width(280)
      List({ space: 1 }) {
        ForEach(this.radioContent, (item: SheetInfo, index?: number) => {
          ListItem() {
            Column() {
              Button() {
                Row() {
                  Text(item.title)
                    .fontSize(16)
                    .fontFamily('HarmonyHeiTi')
                    .fontWeight(FontWeight.Medium)
                    .fontColor($r('sys.color.font_primary'))
                    .layoutWeight(1)
                    .width(178)
                  Radio({ value: 'item.title', group: 'radioGroup' })
                    .size({ width: 16, height: 16 })
                    .checked(this.curSelectNumber === index)
                    .hitTestBehavior(HitTestMode.None)
                    .id(String(index))
                    .onFocus(() => {
                      // this.curSelectNumber === index
                      if (index == FIRST_ITEM_INDEX) {
                      } else if (index == this.radioContent.length - 1) {
                      }
                    })
                }.constraintSize({ minHeight: LIST_MIN_HEIGHT }).clip(false)
                .padding({ top: 13, bottom: 13 })
              }
                .type(ButtonType.Normal)
                .borderRadius($r('sys.float.corner_radius_level8'))
                .buttonStyle(ButtonStyleMode.TEXTUAL)
                .height(48)
                .padding({left: 12, right: 12})
                .onClick(() => {
                  item.action && item.action();
                })
              if (index < this.radioContent.length - 1) {
                Divider()
                  .color($r('sys.color.comp_divider'))
                  .padding({left: 12, right: 12})
              }
            }
          }
        })
      }
        .width(304)
        .clip(false)
        .onFocus(() => {
        })
        .defaultFocus(true)

      Button({ type: ButtonType.Normal, stateEffect: true }) {
        Text($r('app.string.USB_hint_cancel'))
          .fontSize(16)
          .fontColor(0x0000FF)
          .fontFamily('HarmonyHeiTi')
          .fontWeight(FontWeight.Medium)
          .textAlign(TextAlign.Center)
      }
        .height(40)
        .width(328)
        .backgroundColor(0xFFFFFF)
        .padding({right: 16, left: 16, bottom: 16})
        .margin({top: 8})
        .onClick(() => {
          console.log(TAG + 'close pop up window: cancel');
          this.destruction();
        })
    }
      .width(328)
      .border({radius: 32})
      .align(Alignment.Center)
  }
  destruction() {
    console.log(TAG + 'destruction')
    let session = AppStorage.get<UIExtensionContentSession>('session');
    if (session) {
      session.terminateSelf();
    }
  }
}

@Entry
@Component
struct IndexHapComponent {
  private curChoose: number = 0;
  private content: string = ''
  private thisWant: string = ''
  customDialogController: CustomDialogController = new CustomDialogController({
    builder: PopUpWindow({ 
      curSelectNumber: this.curChoose,
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
        }
      ]
    }),
    autoCancel: true,
    cancel: this.onCancel
  });

  async aboutToAppear() {
    this.customDialogController.open();

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
