/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import { usbServiceSwitch } from '../util/UsbServiceSwitch';
import deviceInfo from '@ohos.deviceInfo';
import common from '@ohos.app.ability.common';
import commonEventManager from '@ohos.commonEventManager';

let USBSERVICESWITCH_ID_CHARGE_ONLY: number = 0;
let USBSERVICESWITCH_ID_XFER_HDC: number = 4;
let USBSERVICESWITCH_ID_XFER_FILE: number = 8;
let USBSERVICESWITCH_ID_XFER_PIC: number = 16;
let DEBOUNCE_TIME: number = 500;

class Contact {
  public name: ResourceStr = '';
  public des: ResourceStr = '';
  public usbFunction: number;
  public checked: boolean;

  constructor(name: ResourceStr = '', des: ResourceStr = '', usbFunction: number, checked: boolean) {
    this.name = name;
    this.des = des;
    this.usbFunction = usbFunction;
    this.checked = checked;
  }
}

@Entry
@Component
struct SimpleContacts {
  @State mtpTip: ResourceStr = $r('app.string.USB_func_mtp_tip');
  @State mtpChecked: boolean = false;
  @State ptpChecked: boolean = false;
  @State chargeChecked: boolean = false;
  private curFunc: number = USBSERVICESWITCH_ID_CHARGE_ONLY;
  @State index: number = USBSERVICESWITCH_ID_CHARGE_ONLY;
  private contacts: Array<Contact> = []
  private timeout: number | undefined = 0;

  onPageHide() {
    console.info('IndexComponent onPageHide');
    usbServiceSwitch.serviceChoose(this.index);
    let context = getContext(this) as common.ServiceExtensionContext;
    context?.terminateSelf();
  }

  async aboutToAppear() {
    if (deviceInfo.deviceType === 'tablet') {
      this.mtpTip = $r('app.string.USB_func_mtp_tip2', $r('app.string.USB_my_tablet'));
    }
    this.curFunc = usbServiceSwitch.getCurrentFunctions();
    if (this.curFunc & USBSERVICESWITCH_ID_XFER_FILE) {
      this.mtpChecked = true;
      this.index = USBSERVICESWITCH_ID_XFER_FILE;
    } else if (this.curFunc & USBSERVICESWITCH_ID_XFER_PIC) {
      this.ptpChecked = true;
      this.index = USBSERVICESWITCH_ID_XFER_PIC;
    } else {
      this.chargeChecked = true;
      this.index = USBSERVICESWITCH_ID_CHARGE_ONLY;
    }

    console.info('globalThis.want=' + JSON.stringify(globalThis.want));
    let usbSupportedFunctions = USBSERVICESWITCH_ID_XFER_FILE | USBSERVICESWITCH_ID_XFER_PIC;
    if (globalThis.want?.parameters?.usbSupportedFunctions !== undefined) {
      usbSupportedFunctions = globalThis.want?.parameters?.usbSupportedFunctions;
    }
    if (usbSupportedFunctions & USBSERVICESWITCH_ID_XFER_FILE) {
      this.contacts.push(new Contact($r('app.string.USB_func_mtp'), this.mtpTip, USBSERVICESWITCH_ID_XFER_FILE,
        this.mtpChecked));
    }
    if (usbSupportedFunctions & USBSERVICESWITCH_ID_XFER_PIC) {
      this.contacts.push(new Contact($r('app.string.USB_func_ptp'), $r('app.string.USB_func_ptp_tip'),
        USBSERVICESWITCH_ID_XFER_PIC, this.ptpChecked));
    }
    this.contacts.push(new Contact($r('app.string.USB_func_charge'), '',
      USBSERVICESWITCH_ID_CHARGE_ONLY, this.chargeChecked));

    commonEventManager.createSubscriber({ events: [commonEventManager.Support.COMMON_EVENT_USB_STATE] },
      (err, subscriber) => {
        if (err) {
          console.error('createSubscriber failed:', JSON.stringify(err))
          return;
        }
        commonEventManager.subscribe(subscriber, (err, data) => {
          if (err) {
            console.error('subscribe failed:', JSON.stringify(err))
            return;
          }
          console.info(JSON.stringify(data))
          if (data.parameters?.connected === false) {
            console.info('connected false and terminateSelf');
            let context = getContext(this) as common.ServiceExtensionContext;
            context?.terminateSelf();
          } else {
            if (data.parameters?.mtp !== undefined && data.parameters?.mtp === true) {
              this.index = USBSERVICESWITCH_ID_XFER_FILE;
            } else if (data.parameters?.ptp !== undefined && data.parameters?.ptp === true) {
              this.index = USBSERVICESWITCH_ID_XFER_PIC;
            } else {
              this.index = USBSERVICESWITCH_ID_CHARGE_ONLY;
            }
          }
        })
      });
  }

  @Builder
  itemHead(text: ResourceStr = '') {
    Text(text)
      .fontSize(19)
      .width('100%')
      .padding(16)
      .fontWeight(FontWeight.Bolder)
      .margin({
        top: 0,
        left: 0,
        bottom: 15,
        right: 0
      })
  }

  @Builder
  itemFoot(num: number) {
  }

  @Builder
  overBuilder(index: number) {

  }

  build() {
    List({ space: 10 }) {
      ListItemGroup({ header: this.itemHead($r('app.string.USB_hint')), footer: this.itemFoot(3), space: 12 }) {
        ForEach(this.contacts, (item: Contact, index: number) => {
          ListItem() {
            Button({ type: ButtonType.Normal, stateEffect: true }) {
              Row() {
                Row() {
                  Column() {
                    Text(item.name)
                      .fontSize(16)
                      .width('100%')
                      .padding({
                        top: 1,
                        left: 0,
                        bottom: 1,
                        right: 0
                      })
                      .margin(0)
                      .fontWeight(FontWeight.Medium)
                    if (item.des != '') {
                      Text(item.des)
                        .fontSize(14)
                        .width('100%')
                        .margin(0)
                        .padding({
                          top: 1,
                          left: 0,
                          bottom: 1,
                          right: 0
                        })
                        .fontColor('#626262')
                    }
                  }
                  .width('92.5%')
                  .justifyContent(FlexAlign.Start)
                  .padding({
                    top: 5,
                    left: 0,
                    bottom: 5,
                    right: 0
                  })

                  Column() {
                    if (this.index === item.usbFunction) {
                      Radio({ value: 'item.name', group: 'radioGroup', indicatorType: RadioIndicatorType.TICK })
                        .radioStyle({
                          checkedBackgroundColor: Color.Blue
                        })
                        .height(20)
                        .width(20)
                        .margin(0)
                        .padding(0)
                        .checked(this.index === item.usbFunction)
                        .alignSelf(ItemAlign.End)
                    } else {
                      Column()
                        .height(20)
                        .width(20)
                        .alignSelf(ItemAlign.End)
                    }
                  }
                  .justifyContent(FlexAlign.End)
                  .width('7.5%')
                }
                .width('100%')
              }
              .padding(12)
              .width('100%')
            }
            .borderRadius(20)
            .backgroundColor($r('sys.color.comp_background_list_card'))
            .width('100%')
            .onClick((e) => {
              this.index = item.usbFunction
              console.log('this.index=' + this.index)
              this.setUsbFunction(item.usbFunction);
            })
          }
          .margin({
            top: 0,
            left: 16,
            bottom: 0,
            right: 16
          })
        }, (item: Contact) => JSON.stringify(item))
      }
    }
    .height('100%')
    .width('100%')
    .backgroundColor($r('sys.color.comp_background_gray'))
    .alignListItem(ListItemAlign.Center)
    .expandSafeArea([SafeAreaType.SYSTEM], [SafeAreaEdge.TOP, SafeAreaEdge.BOTTOM])
  }

  setUsbFunction(choose: number) {
    console.log('choose is ' + choose)
    if (this.timeout) {
      clearTimeout(this.timeout);
    }
    this.timeout = setTimeout(() => {
      console.log('serviceChoose start')
      usbServiceSwitch.serviceChoose(choose);
      console.log('serviceChoose end')
    }, DEBOUNCE_TIME)
  }
}