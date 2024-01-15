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


import notificationManager from '@ohos.notificationManager';
import wantAgent from '@ohos.app.ability.wantAgent';
import type context from '@ohos.app.ability.common';

type Context = context.Context;
type WantAgent = object;
const TAG: string = 'usbfunctionswitchwindow_NotificationUtil';

const REQUEST_CODE: number = 0;
const USBFUNCTION_MTP: number = 8;
const USBFUNCTION_PTP: number = 16;

class NotificationUtil {
  private usbFuncBundleName: string = 'com.usb.right';
  private usbFuncAbilityName: string = 'UsbFunctionSwitchExtAbility';

  async createWantAgentForStartAbility(bundleName: string, abilityName: string, currentChoose: number) : Promise<WantAgent> {
    let wantAgentInfo = {
      wants: [
        {
          'deviceId': '', /* An empty deviceId indicates this device */
          'bundleName': bundleName,
          'abilityName': abilityName,
          'parameters': {
            'funcSelect': { 'type': 'number', 'value': currentChoose },
            'action': { 'type': 'string', 'value': 'show' },
          }
        }
      ],
      operationType: wantAgent.OperationType.START_ABILITY,
      requestCode: REQUEST_CODE
    };
    return await wantAgent.getWantAgent(wantAgentInfo);
  }

  /**
   * enable notification
   */
  async enableNotification(): Promise<void> {
    try {
      await notificationManager.requestEnableNotification();
      console.log(TAG + 'enableNotification success');
    } catch (err) {
      console.error(TAG + 'enableNotification err: ' + JSON.stringify(err));
    }
  }

  /**
   *
   * @param notificationRequest
   * @param id, Support specifying notification id when publishing notifications
   */
  async publishNotification(notificationRequest: notificationManager.NotificationRequest, id?: number): Promise<void> {
    if (id > 0) {
      notificationRequest.id = id;
    }
    notificationManager.publish(notificationRequest).then((data) => {
      console.log(TAG + `Succeeded in publish. Data ${JSON.stringify(data)}`);
    }).catch((err) => {
      console.error(TAG + 'Failed to publish. Cause: ' + JSON.stringify(err));
    });
  }

  async publishChooseNotify(currentChoose: number, notifyId?: number): Promise<void> {
    console.log(TAG + 'current notify: ' + currentChoose);
    this.enableNotification();

    let context: Context = getContext(this) as Context;
    let applicationContext = context.getApplicationContext();

    try {
      let notificationWantAgent = await this.createWantAgentForStartAbility(
        this.usbFuncBundleName, this.usbFuncAbilityName, currentChoose);
      let notificationRequest: notificationManager.NotificationRequest = {
        id: 1,
        // type: notificationManager.SlotType.SERVICE_INFORMATION,
        isRemoveAllowed: false,
        isUnremovable: true,
        content: {
          notificationContentType: notificationManager.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
          normal: {
            title: 'test_title',
            text: 'test_text',
            additionalText: 'test_additionalText'
          }
        },
        wantAgent: notificationWantAgent
      };
      if (currentChoose === USBFUNCTION_MTP) {
        notificationRequest.content.normal = {
          title: applicationContext.resourceManager.getStringSync($r('app.string.USB_notify_mtp_title')),
          text: applicationContext.resourceManager.getStringSync($r('app.string.USB_notify_text')),
          additionalText: 'Media Transfer Protocol(MTP)'
        };
      } else if (currentChoose === USBFUNCTION_PTP) {
        notificationRequest.content.normal = {
          title: applicationContext.resourceManager.getStringSync($r('app.string.USB_notify_ptp_title')),
          text: applicationContext.resourceManager.getStringSync($r('app.string.USB_notify_text')),
          additionalText: 'Picture Transfer Protocol(PTP)'
        };
      } else if (currentChoose === -1) {
        notificationRequest.content.normal = {
          title: applicationContext.resourceManager.getStringSync($r('app.string.USB_notify_charge_title')),
          text: applicationContext.resourceManager.getStringSync($r('app.string.USB_notify_text')),
          additionalText: 'Charge only'
        };
      }

      this.publishNotification(notificationRequest, notifyId);
      console.log(TAG + 'publishNotificationWithWantAgent done: ' + notifyId);
    } catch (error) {
      console.error(TAG + 'publishNotificationWithWantAgent error =  ' + JSON.stringify(error));
    }
  }

  /**
   * cancel notification by id
   */
  async cancelNotificationById(id: number): Promise<void> {
    try {
      await notificationManager.cancel(id);
      console.log(TAG + 'cancel notification success');
    } catch (err) {
      if (err) {
        console.error(TAG + '=cancel notification err: ' + JSON.stringify(err));
      }
    }
  }

  /**
   * cancel all notification
   */
  async cancelAllNotifications(): Promise<void> {
    try {
      await notificationManager.cancelAll();
      console.log(TAG + 'cancel all notification success');
    } catch (err) {
      if (err) {
        console.error(TAG + 'cancel all notification err ' + JSON.stringify(err));
      }
    }
  }
}

export let notificationUtil = new NotificationUtil();
