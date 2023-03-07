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

import featureAbility from '@ohos.ability.featureAbility';
import { notificationUtil } from '../util/NotificationUtil';
import { usbServiceSwitch } from '../util/UsbServiceSwitch';
import Logger from '../util/Logger';

const TAG: string = 'usbfunctionswitchwindow_Index';

const USBSERVICESWITCH_ID = {
  DEFAULT: -1,
  CHARGE_ONLY: -1,
  XFER_FILE: 8,
  XFER_PIC: 16,
}

@CustomDialog
  export default
  struct PopUpWindow {
  @State curSelectNumber: number = USBSERVICESWITCH_ID.DEFAULT;
  private customDialogController ?: CustomDialogController;
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
            let tmpChooseNumber: number = USBSERVICESWITCH_ID.CHARGE_ONLY;
            usbServiceSwitch.serviceChoose(tmpChooseNumber);
            notificationUtil.publishChooseNotify(tmpChooseNumber);
            this.curSelectNumber = tmpChooseNumber;

            Logger.info(TAG, 'close pop up window: charge only');
            this.customDialogController.close();
            globalThis.window.hide().then(() => {
              Logger.info(TAG, 'Hide window success');
            }).catch((err) => {
              Logger.error(TAG, `Failed to hide the Window. Cause: ${JSON.stringify(err)}`);
            })
          })
          .borderColor(Color.Red)
          .borderWidth(this.curSelectNumber === USBSERVICESWITCH_ID.CHARGE_ONLY ? 2 : 0)

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
            let tmpChooseNumber: number = USBSERVICESWITCH_ID.XFER_FILE;
            usbServiceSwitch.serviceChoose(tmpChooseNumber);
            notificationUtil.publishChooseNotify(tmpChooseNumber);
            this.curSelectNumber = tmpChooseNumber;

            Logger.info(TAG, 'close pop up window: xfer file');
            this.customDialogController.close();
            globalThis.window.hide().then(() => {
              Logger.info(TAG, 'Hide window success');
            }).catch((err) => {
              Logger.error(TAG, `Failed to hide the Window. Cause: ${JSON.stringify(err)}`);
            })
          })
          .borderColor(Color.Red)
          .borderWidth(this.curSelectNumber === USBSERVICESWITCH_ID.XFER_FILE ? 2 : 0)

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
            let tmpChooseNumber: number = USBSERVICESWITCH_ID.XFER_PIC;
            usbServiceSwitch.serviceChoose(tmpChooseNumber);
            notificationUtil.publishChooseNotify(tmpChooseNumber);
            this.curSelectNumber = tmpChooseNumber;

            Logger.info(TAG, 'close pop up window: xfer pic');
            globalThis.window.hide().then(() => {
              Logger.info(TAG, 'Hide window success');
            }).catch((err) => {
              Logger.error(TAG, `Failed to hide the Window. Cause: ${JSON.stringify(err)}`);
            })
          })
          .borderColor(Color.Red)
          .borderWidth(this.curSelectNumber === USBSERVICESWITCH_ID.XFER_PIC ? 2 : 0)
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
          Logger.info(TAG, 'close pop up window: cancel');
          notificationUtil.publishChooseNotify(this.curSelectNumber);
          this.customDialogController.close();

          globalThis.window.hide().then(() => {
            Logger.info(TAG, 'Hide window success');
          }).catch((err) => {
            Logger.error(TAG, `Failed to hide the Window. Cause: ${JSON.stringify(err)}`);
          })

        })

      Blank().height(5)
    }
    .height('40%')

  }
  destruction() {
    Logger.info(TAG, 'destruction')
    this.customDialogController.close()
    globalThis.window.destroy()
    globalThis.extensionContext.terminateSelf()
    featureAbility.terminateSelf();
  }
}

@Entry
@Component
struct IndexHapComponent {
  private curChoose: number = -1;
  customDialogController: CustomDialogController = new CustomDialogController({
    builder: PopUpWindow({ curSelectNumber: this.curChoose }),
    autoCancel: true
  });

  async aboutToAppear() {
    Logger.info(TAG, `aboutToAppear want=${JSON.stringify(globalThis.want)}`);
    Logger.info(TAG, `  want defaultChoose=${JSON.stringify(globalThis.want.parameters["defaultChoose"])}`);
    Logger.info(TAG, `  want funcSelect=${JSON.stringify(globalThis.want.parameters["funcSelect"])}`);
    Logger.info(TAG, `aboutToAppear abilityWant=${JSON.stringify(globalThis.abilityWant)}`);
    Logger.info(TAG, `  abilityWant defaultChoose=${JSON.stringify(globalThis.abilityWant.parameters["defaultChoose"])}`);
    Logger.info(TAG, `  abilityWant funcSelect=${JSON.stringify(globalThis.abilityWant.parameters["funcSelect"])}`);
    let workWant = globalThis.abilityWant;

    if (workWant.parameters["defaultChoose"]) {
      this.curChoose = workWant.parameters["defaultChoose"];
      Logger.info(TAG, `use want defaultChoose: ${this.curChoose}`);
    } else if (workWant.parameters["funcSelect"]) {
      this.curChoose = workWant.parameters["funcSelect"].value;
      Logger.info(TAG, `use want funcSelect: ${this.curChoose}`);
    }

    if (this.curChoose != USBSERVICESWITCH_ID.CHARGE_ONLY && this.curChoose != USBSERVICESWITCH_ID.XFER_FILE 
      && this.curChoose != USBSERVICESWITCH_ID.XFER_PIC) {
      this.curChoose = USBSERVICESWITCH_ID.CHARGE_ONLY;
      Logger.info(TAG, `error, use default: ${this.curChoose}`);
    }

    notificationUtil.enableNotification();

    if (this.curChoose === USBSERVICESWITCH_ID.CHARGE_ONLY) {
      Logger.info(TAG, 'aboutToAppear: publish charge only');
      notificationUtil.publishChooseNotify(USBSERVICESWITCH_ID.CHARGE_ONLY);
    } else if (this.curChoose === USBSERVICESWITCH_ID.XFER_FILE) {
      Logger.info(TAG, 'aboutToAppear: publish xfer file');
      notificationUtil.publishChooseNotify(USBSERVICESWITCH_ID.XFER_FILE);
    } else if (this.curChoose === USBSERVICESWITCH_ID.XFER_PIC) {
      Logger.info(TAG, 'aboutToAppear: publish xfer pic');
      notificationUtil.publishChooseNotify(USBSERVICESWITCH_ID.XFER_PIC);
    } else {

    }
    if (workWant.parameters["defaultChoose"]) {
      Logger.info(TAG, 'first time show window');
    } else if (workWant.parameters["funcSelect"]) {
      Logger.info(TAG, 'second time show window');
    }
    this.customDialogController.open();

  }

  async aboutToDisappear() {
    Logger.info(TAG, 'aboutToDisappear');
  }

  async onPageShow() {
    Logger.info(TAG, `onPageShow, curChoose: ${this.curChoose}`);

  }

  async onPageHide() {
    Logger.info(TAG, 'onPageHide');
  }

  build() { }
}
