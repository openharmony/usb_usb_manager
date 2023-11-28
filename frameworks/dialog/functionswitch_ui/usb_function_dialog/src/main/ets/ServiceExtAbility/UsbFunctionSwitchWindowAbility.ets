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
import common from '@ohos.app.ability.common';
import window from '@ohos.window';
import display from '@ohos.display';
import rpc from '@ohos.rpc';
import Logger from '../util/Logger';
import { notificationUtil } from '../util/NotificationUtil';
import common from '@ohos.app.ability.common';
import commonEvent from '@ohos.commonEventManager';
import window from '@ohos.window';
import Want from '@ohos.app.ability.Want';
import { BusinessError } from '@ohos.base';

const TAG: string = 'usbfunctionswitchwindow_ServiceExtensionAbility';
const WINDOW_NAME: string = 'UsbFunctionSwitchWindow';

class UsbFunctionSwitchStub extends rpc.RemoteObject {
  constructor(des: string) {
    super(des);
  }
  onRemoteRequest(code: number, data: MessageParcel, reply: MessageParcel, options: MessageOption) {
    return true;
  }
}

interface NavigationBarRect {
  left: number;
  top: number;
  width: number;
  height: number;
}

export default class UsbFunctionSwitchAbility extends ServiceExtensionAbility {
  private extensionWin: window.Window | undefined = undefined;
  private mContext: common.ServiceExtensionContext | undefined = undefined;
  private windowNum: number = 0;

  /**
   * Lifecycle function, called back when a service extension is started for initialization.
   */
  onCreate(want: Want): void {
    Logger.info(TAG, `onCreate want: ${JSON.stringify(want)}`);
    this.mContext = getContext(this) as common.ServiceExtensionContext;
    this.windowNum = 0;
    return;
  }

  /**
   * Lifecycle function, called back when a service extension is started or recall.
   */
  onRequest(want: Want, startId: number): void {
    Logger.info(TAG, `onRequest want=${JSON.stringify(want)} startId=${startId}`);
    try {
      let dis = display.getDefaultDisplaySync();
      let navigationBarRect: NavigationBarRect = {
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

  onConnect(want: Want): rpc.RemoteObject {
    Logger.info(TAG, `onConnect want=${JSON.stringify(want)}`);
    return new UsbFunctionSwitchStub('test');
  }


  onDisconnect(want: Want): void {
    Logger.info(TAG, `onDisconnect want=${JSON.stringify(want)}`);
    return;
  }


  /**
   * Lifecycle function, called back before a service extension is destroyed.
   */
  onDestroy(): void {
    Logger.info(TAG, 'onDestroy');
    if (this.window) {
      this.extensionWin = undefined;
      notificationUtil.cancelAllNotifications();
      this.window.destroy((err: BusinessError, commonEventData: commonEvent.CommonEventData) => {
        if (err.code) {
          Logger.error(TAG, `Failed to destroy the window. Cause: ${JSON.stringify(err)}`);
          return;
        }
        Logger.info(TAG, 'Succeeded in destroy the window');
        this.window = undefined;
      });
    }
    return;
  }

  private async createOrShowUsbWindow(winName: string, windowType: number, rect: NavigationBarRect): Promise<void> {
    try {
      if (this.extensionWin !== undefined) {
        Logger.info(TAG, 'destroy first power window');
        this.extensionWin.destroy();
        this.extensionWin = undefined;
      }
      const usbWin = await window.create(this.mContext, winName, windowType);
      if (this.extensionWin === undefined) {
        Logger.info(TAG, 'extensionWin not init, init now');
        extensionWin = usbWin;
        this.extensionWin = extensionWin;
      }
      this.extensionWin = usbWin;
      await this.extensionWin.moveWindowTo(rect.left, rect.top);
      await this.extensionWin.resize(rect.width, rect.height);
      await this.extensionWin.setUIContent('pages/index');
      await this.extensionWin.showWindow();
      this.windowNum++;
      console.log(TAG + 'window create successfully');
      Logger.info(TAG, 'window create success');
    } catch {
      Logger.error(TAG, 'window create failed');
    }
    return;
  }
};

