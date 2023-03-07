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

import ServiceExtensionAbility from '@ohos.app.ability.ServiceExtensionAbility';
import window from '@ohos.window';
import display from '@ohos.display';
import rpc from '@ohos.rpc';
import Logger from '../util/Logger';
import { notificationUtil } from '../util/NotificationUtil';

const TAG: string = 'usbfunctionswitchwindow_ServiceExtensionAbility';
const WINDOW_NAME: string = 'UsbFunctionSwitchWindow';
class UsbFunctionSwitchStub extends rpc.RemoteObject {
  constructor(des) {
    super(des);
  }
  onRemoteRequest(code, data, reply, option) {
    return true;
  }
}

const BG_COLOR = '#00000000';
let usbServiceSwitchWindowFirst = undefined;

export default class UsbFunctionSwitchAbility extends ServiceExtensionAbility {

  /**
   * Lifecycle function, called back when a service extension is started for initialization.
   */
  onCreate(want): void {
    Logger.info(TAG, `onCreate want: ${JSON.stringify(want)}`);
    globalThis.extensionContext = this.context;
    globalThis.want = want;
    globalThis.windowNum = 0;
    globalThis.usbServiceSwitchWindowFirst = usbServiceSwitchWindowFirst;
    return;
  }

  /**
   * Lifecycle function, called back when a service extension is started or recall.
   */
  onRequest(want, startId): void {
    Logger.info(TAG, `onRequest want=${JSON.stringify(want)} startId=${startId}`);
    globalThis.abilityWant = want;
    try {
      let dis = display.getDefaultDisplaySync();
      let navigationBarRect = {
        left: 0,
        top: 0,
        width: dis.width,
        height: dis.height
      };
      this.createOrShowUsbWindow(WINDOW_NAME, window.WindowType.TYPE_FLOAT, navigationBarRect);
    } catch (exception) {
      Logger.error(TAG, `Failed to obtain the default display object. Code: ${JSON.stringify(exception)}`);
    };
    return;
  }

  onConnect(want): rpc.RemoteObject {
    globalThis.abilityWant = want;
    Logger.info(TAG, `onConnect want=${JSON.stringify(want)}`);
    return new UsbFunctionSwitchStub('test');
  }


  onDisconnect(want): void {
    Logger.info(TAG, `onDisconnect want=${JSON.stringify(want)}`);
    return;
  }


  /**
   * Lifecycle function, called back before a service extension is destroyed.
   */
  onDestroy(): void {
    Logger.info(TAG, 'onDestroy');
    if (globalThis.window) {
      globalThis.usbServiceSwitchWindowFirst = undefined;
      notificationUtil.cancelAllNotifications();
      globalThis.window.destroy((err, data) => {
        if (err.code) {
          Logger.error(TAG, `Failed to destroy the window. Cause: ${JSON.stringify(err)}`);
          return;
        }
        Logger.info(TAG, 'Succeeded in destroy the window');
        globalThis.window = undefined;
      });
    }
    return;
  }

  private async createOrShowUsbWindow(winName: string, windowType: number, rect): Promise<void> {
    try {
      if (globalThis.usbServiceSwitchWindowFirst !== undefined) {
        Logger.info(TAG, 'destroy first power window');
        globalThis.usbServiceSwitchWindowFirst.destroy();
        globalThis.usbServiceSwitchWindowFirst = undefined;
      }
      const usbWin = await window.create(globalThis.extensionContext, winName, windowType);
      if (globalThis.usbServiceSwitchWindowFirst === undefined) {
        Logger.info(TAG, 'usbServiceSwitchWindowFirst not init, init now');
        usbServiceSwitchWindowFirst = usbWin;
        globalThis.usbServiceSwitchWindowFirst = usbServiceSwitchWindowFirst;
      }
      globalThis.window = usbWin;
      await usbWin.moveTo(rect.left, rect.top);
      await usbWin.resetSize(rect.width, rect.height);
      await usbWin.loadContent('pages/Index');
      await usbWin.setBackgroundColor(BG_COLOR);
      await usbWin.show();
      Logger.info(TAG, 'window create success');
    } catch {
      Logger.error(TAG, 'window create failed');
    }
    return;
  }
};

