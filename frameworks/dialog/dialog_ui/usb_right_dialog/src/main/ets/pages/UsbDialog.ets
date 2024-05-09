/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import usbMgr from '@ohos.usbManager'

// allow,notAllow button style
@Extend(Text) function controlBtn(fontColor: string) {
  .fontColor(fontColor)
  .backgroundColor(0xffffff)
  .width(100)
  .height(22)
  .fontSize(20)
  .textAlign(TextAlign.Center)
}

// content style
@Extend(Text) function controlFontSize(fontSize: number) {
  .fontSize(fontSize)
  .width('180%')
  .fontColor('rgba(10, 10, 10, 1.00)')
  .textAlign(TextAlign.Center)
  .padding({ top: 30, left: 50, right: 50 })
  .alignSelf(ItemAlign.Center)
  .margin({top: 10})
}

@CustomDialog
export default
struct ConfirmDialog {
  content: string
  notAllowFontColor: string = '#E84026'
  allowFontColor: string = '#ff1a0af7'
  controller: CustomDialogController
  // title style
  @Builder TipTextStyle(tip: string, fontSize: number) {
    Text(tip)
      .controlFontSize(fontSize)
      .visibility(tip.length > 0 ? Visibility.Visible : Visibility.None)
  }
  build() {
    Column() {
      this.TipTextStyle(this.content, 22)
      Flex({ justifyContent: FlexAlign.SpaceAround }) {
        Text($r('app.string.allowButton'))
          .controlBtn(this.allowFontColor)
          .onClick(() => {
            try {
              usbMgr.addDeviceAccessRight(globalThis.want.parameters['tokenId'], globalThis.want.parameters['deviceName'])
              this.destruction()
            } catch {
              console.info('USBRight ConfirmDialog onClick error!');
            }
          })
        Text('|')
            .fontSize(10)
        Text($r('app.string.notAllowButton'))
          .controlBtn(this.notAllowFontColor)
          .onClick(() => {
            this.destruction()
          })
      }.margin({ top: 43, bottom: 15, left: 20, right: 20 })
    }
  }
  destruction() {
    this.controller.close()
    try {
      globalThis.window.destroy()
      globalThis.extensionContext.terminateSelf()
    } catch {
      console.info('USBRight ConfirmDialog destruction error!');
    }
  }
}

@Entry
@Component
struct IndexHapComponent {
  private content: string = ''
  private allow: string = ''
  private use: string = ''
  confirmDialogController: CustomDialogController = new CustomDialogController( {
    builder: ConfirmDialog({content: this.content}),
    autoCancel: false
  })

  async aboutToAppear() {
    this.allow = await globalThis.getContext().resourceManager.getStringValue($r('app.string.allow'));
    this.use = await globalThis.getContext().resourceManager.getStringValue($r('app.string.use'));
    this.content = this.allow + globalThis.want.parameters.bundleName + this.use +
    globalThis.want.parameters.deviceName;
    this.confirmDialogController.open()
  }
  build() {}
}