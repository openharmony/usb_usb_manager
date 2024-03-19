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

import { notificationUtil } from '../util/NotificationUtil';
import { usbServiceSwitch } from '../util/UsbServiceSwitch';
import {GlobalThisProxy} from '../util/GlobalThisProxy';
import { BusinessError } from '@ohos.base';
import window from '@ohos.window';
import common from '@ohos.app.ability.common'
import Want from '@ohos.app.ability.Want';
let TAG: string = 'usbfunctionswitchwindow_Index';
let USBSERVICESWITCH_ID_DEFAULT : number = -1;
let USBSERVICESWITCH_ID_CHARGE_ONLY : number = -1;
let USBSERVICESWITCH_ID_XFER_FILE : number = 8;
let USBSERVICESWITCH_ID_XFER_PIC : number = 16;

@CustomDialog
  export default
  struct PopUpWindow {
  @State curSelectNumber: number = USBSERVICESWITCH_ID_DEFAULT;
  customDialogController: CustomDialogController;
  build() {
    Column() {
      Blank().height(5)
      Text($r('app.string.USB_hint'))
        .width('100%')
        .height('15%')
        .textAlign(TextAlign.Center)
        .fontSize(20)

      Blank()

      Row({ space: 5 }){
        Button({ type: ButtonType.Normal, stateEffect: true }) {
          Column() {
            Text($r('app.string.USB_func_charge'))
              .fontSize(20)
              .fontColor(0xffffff)
              .textAlign(TextAlign.Center)
              .width('100%')
          }
        }
        .height('100%')
          .width('30%')
          .borderRadius(10)
          .onClick(() => {
            let windowClass: window.Window = window.findWindow('UsbFunctionSwitchWindow');
            let tmpChooseNumber: number = USBSERVICESWITCH_ID_CHARGE_ONLY;
            usbServiceSwitch.serviceChoose(tmpChooseNumber);
            notificationUtil.publishChooseNotify(tmpChooseNumber);
            this.curSelectNumber = tmpChooseNumber;

            console.log(TAG + 'close pop up window: charge only');
            this.customDialogController.close();
            windowClass.hide((err: BusinessError) => {
              const errCode: number = err.code;
              if (errCode) {
                console.error('Failed to hide the window. Cause: ' + JSON.stringify(err));
                return;
              }
              console.info('Succeeded in hiding the window.');
            });
          })
          .borderColor(Color.Red)
          .borderWidth(this.curSelectNumber === USBSERVICESWITCH_ID_CHARGE_ONLY ? 2 : 0)

        Button({ type: ButtonType.Normal, stateEffect: true }) {
          Column() {
            Text($r('app.string.USB_func_mtp'))
              .fontSize(20)
              .fontColor(0xffffff)
              .textAlign(TextAlign.Center)
          }
        }
        .height('100%')
          .width('30%')
          .borderRadius(10)
          .onClick(() => {
            let windowClass: window.Window = window.findWindow('UsbFunctionSwitchWindow');
            let tmpChooseNumber: number = USBSERVICESWITCH_ID_XFER_FILE;
            usbServiceSwitch.serviceChoose(tmpChooseNumber);
            notificationUtil.publishChooseNotify(tmpChooseNumber);
            this.curSelectNumber = tmpChooseNumber;

            console.log(TAG + 'close pop up window: xfer file');
            this.customDialogController.close();
            windowClass.hide((err: BusinessError) => {
              const errCode: number = err.code;
              if (errCode) {
                console.error('Failed to hide the window. Cause: ' + JSON.stringify(err));
                return;
              }
              console.info('Succeeded in hiding the window.');
            });
          })
          .borderColor(Color.Red)
          .borderWidth(this.curSelectNumber === USBSERVICESWITCH_ID_XFER_FILE ? 2 : 0)

        Button({ type: ButtonType.Normal, stateEffect: true }) {
          Column() {
            Text($r('app.string.USB_func_ptp'))
              .fontSize(20)
              .fontColor(0xffffff)
              .textAlign(TextAlign.Center)
          }
        }
        .height('100%')
          .width('30%')
          .borderRadius(10)
          .onClick(() => {
            let windowClass: window.Window = window.findWindow('UsbFunctionSwitchWindow');
            let tmpChooseNumber: number = USBSERVICESWITCH_ID_XFER_PIC;
            usbServiceSwitch.serviceChoose(tmpChooseNumber);
            notificationUtil.publishChooseNotify(tmpChooseNumber);
            this.curSelectNumber = tmpChooseNumber;

            console.log(TAG + 'close pop up window: xfer pic');
            windowClass.hide((err: BusinessError) => {
              const errCode: number = err.code;
              if (errCode) {
                console.error('Failed to hide the window. Cause: ' + JSON.stringify(err));
                return;
              }
              console.info('Succeeded in hiding the window.');
            });
          })
          .borderColor(Color.Red)
          .borderWidth(this.curSelectNumber === USBSERVICESWITCH_ID_XFER_PIC ? 2 : 0)
      }
      .height('60%')

      Divider().height('5%')

      Button({ type: ButtonType.Capsule, stateEffect: true }) {
        Text($r("app.string.USB_hint_cancel"))
          .fontSize(20)
          .height('100%')
          .width('100%')
          .textAlign(TextAlign.Center)
      }
      .height('15%')
        .width('90%')
        .backgroundColor('#ff9d6b6b')
        .onClick(() => {
          console.log(TAG + 'close pop up window: cancel');
          let windowClass: window.Window = window.findWindow('UsbFunctionSwitchWindow');
          notificationUtil.publishChooseNotify(this.curSelectNumber);
          this.customDialogController.close();

          windowClass.hide((err: BusinessError) => {
            const errCode: number = err.code;
            if (errCode) {
              console.error('Failed to hide the window. Cause: ' + JSON.stringify(err));
              return;
            }
            console.info('Succeeded in hiding the window.');
          });

        })

      Blank().height(5)
    }
    .height('40%')

  }
  destruction() {
    let windowClass: window.Window = window.findWindow('UsbFunctionSwitchWindow');
    console.log(TAG + 'destruction')
    this.customDialogController.close()
    windowClass.destroyWindow()
    let extensionContextCtr: common.ServiceExtensionContext = GlobalThisProxy.getInstance().getContext('extensionAbility')
    extensionContextCtr.terminateSelf()
  }
}

@Entry
@Component
struct IndexHapComponent {
  private curChoose: number = -1;
  private content: string = ''
  private thisWant: string = ''
  customDialogController: CustomDialogController = new CustomDialogController({
    builder: PopUpWindow({ curSelectNumber: this.curChoose }),
    autoCancel: true,
    cancel: this.onCancel
  });

  async aboutToAppear() {
    let defaultChoose: number = GlobalThisProxy.getInstance().getNumInfo("workWant_defaultChoose");
    let funcSelect: number = GlobalThisProxy.getInstance().getNumInfo("workWant_funcSelect");
    if (defaultChoose) {
      this.curChoose = defaultChoose;
      console.log(TAG + 'use want defaultChoose: ' + this.curChoose);
    } else if (funcSelect) {
      this.curChoose = funcSelect;
      console.log(TAG + 'use want funcSelect: ' + this.curChoose);
    }

    if (this.curChoose != USBSERVICESWITCH_ID_CHARGE_ONLY && this.curChoose != USBSERVICESWITCH_ID_XFER_FILE 
      && this.curChoose != USBSERVICESWITCH_ID_XFER_PIC) {
      this.curChoose = USBSERVICESWITCH_ID_CHARGE_ONLY;
      console.log(TAG + 'error, use default: ' + this.curChoose);
    }

    notificationUtil.enableNotification();

    if (this.curChoose === USBSERVICESWITCH_ID_CHARGE_ONLY) {
      console.log(TAG + 'aboutToAppear: publish charge only');
      notificationUtil.publishChooseNotify(USBSERVICESWITCH_ID_CHARGE_ONLY);
    } else if (this.curChoose === USBSERVICESWITCH_ID_XFER_FILE) {
      console.log(TAG + 'aboutToAppear: publish xfer file');
      notificationUtil.publishChooseNotify(USBSERVICESWITCH_ID_XFER_FILE);
    } else if (this.curChoose === USBSERVICESWITCH_ID_XFER_PIC) {
      console.log(TAG + 'aboutToAppear: publish xfer pic');
      notificationUtil.publishChooseNotify(USBSERVICESWITCH_ID_XFER_PIC);
    } else {
      console.log(TAG + 'this.curChoose: ' + JSON.stringify(this.curChoose));
    }
    if (defaultChoose) {
      console.log(TAG + 'first time show window');
    } else if (funcSelect) {
      console.log(TAG + 'second time show window');
    }
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
    let windowClass: window.Window = window.findWindow('UsbFunctionSwitchWindow');
    this.customDialogController.close();

    windowClass.hide((err: BusinessError) => {
      const errCode: number = err.code;
      if (errCode) {
        console.error('TAG + Failed to hide the window. Cause: ' + JSON.stringify(err));
        return;
      }
      console.info('TAG + Succeeded in hiding the window.');
    });
  }
  build() { }
}
