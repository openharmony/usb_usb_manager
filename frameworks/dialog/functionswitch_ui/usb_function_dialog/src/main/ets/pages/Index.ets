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
import { BusinessError } from '@ohos.base';
import Logger from '../util/Logger';
import Want from '@ohos.app.ability.Want';

const TAG: string = 'usbfunctionswitchwindow_Index';

interface  UsbServiceSwitchId {
  default: number;
  chargeOnly: number;
  xferFile: number;
  xferPic: number;
}

let usbServiceSwitchId: UsbServiceSwitchId = {
  default: -1,
  chargeOnly: -1,
  xferFile: 8,
  xferPic: 16,
};

@CustomDialog
  export default
  struct PopUpWindow {
  @State curSelectNumber: number = usbServiceSwitchId.default;
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
            let tmpChooseNumber: number = usbServiceSwitchId.chargeOnly;
            usbServiceSwitch.serviceChoose(tmpChooseNumber);
            notificationUtil.publishChooseNotify(tmpChooseNumber);
            this.curSelectNumber = tmpChooseNumber;

            Logger.info(TAG, 'close pop up window: charge only');
            customDialogController.close();
            this.window.hide().then(() => {
              Logger.info(TAG, 'Hide window success');
            }).catch((err: BusinessError) => {
              Logger.error(TAG, `Failed to hide the Window. Cause: ${JSON.stringify(err)}`);
            })
          })
          .borderColor(Color.Red)
          .borderWidth(this.curSelectNumber === usbServiceSwitchId.chargeOnly ? 2 : 0)

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
            let tmpChooseNumber: number = usbServiceSwitchId.xferFile;
            usbServiceSwitch.serviceChoose(tmpChooseNumber);
            notificationUtil.publishChooseNotify(tmpChooseNumber);
            this.curSelectNumber = tmpChooseNumber;

            Logger.info(TAG, 'close pop up window: xfer file');
            customDialogController.close();
            this.window.hide().then(() => {
              Logger.info(TAG, 'Hide window success');
            }).catch((err: BusinessError) => {
              Logger.error(TAG, `Failed to hide the Window. Cause: ${JSON.stringify(err)}`);
            })
          })
          .borderColor(Color.Red)
          .borderWidth(this.curSelectNumber === usbServiceSwitchId.xferFile ? 2 : 0)

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
            let tmpChooseNumber: number = usbServiceSwitchId.xferPic;
            usbServiceSwitch.serviceChoose(tmpChooseNumber);
            notificationUtil.publishChooseNotify(tmpChooseNumber);
            this.curSelectNumber = tmpChooseNumber;

            Logger.info(TAG, 'close pop up window: xfer pic');
            this.window.hide().then(() => {
              Logger.info(TAG, 'Hide window success');
            }).catch((err: BusinessError) => {
              Logger.error(TAG, `Failed to hide the Window. Cause: ${JSON.stringify(err)}`);
            })
          })
          .borderColor(Color.Red)
          .borderWidth(this.curSelectNumber === usbServiceSwitchId.xferPic ? 2 : 0)
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
          ustomDialogController.close();

          this.window.hide().then(() => {
            Logger.info(TAG, 'Hide window success');
          }).catch((err: BusinessError) => {
            Logger.error(TAG, `Failed to hide the Window. Cause: ${JSON.stringify(err)}`);
          })

        })

      Blank().height(5)
    }
    .height('40%')

  }
  destruction() {
    Logger.info(TAG, 'destruction')
    customDialogController.close()
    this.window.destroy()
    this.extensionContext.terminateSelf()
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
    Logger.info(TAG, `aboutToAppear want=${JSON.stringify(this.want)}`);
    Logger.info(TAG, `  want defaultChoose=${JSON.stringify(this.want.parameters["defaultChoose"])}`);
    Logger.info(TAG, `  want funcSelect=${JSON.stringify(this.want.parameters["funcSelect"])}`);
    Logger.info(TAG, `aboutToAppear abilityWant=${JSON.stringify(this.abilityWant)}`);
    Logger.info(TAG, `  abilityWant defaultChoose=${JSON.stringify(this.abilityWant.parameters["defaultChoose"])}`);
    Logger.info(TAG, `  abilityWant funcSelect=${JSON.stringify(this.abilityWant.parameters["funcSelect"])}`);

    if (this.abilityWant.parameters["defaultChoose"]) {
      this.curChoose = this.abilityWant.parameters["defaultChoose"];
      Logger.info(TAG, `use want defaultChoose: ${this.curChoose}`);
    } else if (this.abilityWant.parameters["funcSelect"]) {
      this.curChoose = this.abilityWant.parameters["funcSelect"].value;
      Logger.info(TAG, `use want funcSelect: ${this.curChoose}`);
    }

    if (this.curChoose != usbServiceSwitchId.chargeOnly && this.curChoose != usbServiceSwitchId.xferFile
      && this.curChoose != usbServiceSwitchId.xferPic) {
      this.curChoose = usbServiceSwitchId.chargeOnly;
      Logger.info(TAG, `error, use default: ${this.curChoose}`);
    }

    notificationUtil.enableNotification();

    if (this.curChoose === usbServiceSwitchId.chargeOnly) {
      Logger.info(TAG, 'aboutToAppear: publish charge only');
      notificationUtil.publishChooseNotify(usbServiceSwitchId.chargeOnly);
    } else if (this.curChoose === usbServiceSwitchId.xferFile) {
      Logger.info(TAG, 'aboutToAppear: publish xfer file');
      notificationUtil.publishChooseNotify(usbServiceSwitchId.xferFile);
    } else if (this.curChoose === usbServiceSwitchId.xferPic) {
      Logger.info(TAG, 'aboutToAppear: publish xfer pic');
      notificationUtil.publishChooseNotify(usbServiceSwitchId.xferPic);
    } else {

    }
    if (this.abilityWant.parameters["defaultChoose"]) {
      Logger.info(TAG, 'first time show window');
    } else if (this.abilityWant.parameters["funcSelect"]) {
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
