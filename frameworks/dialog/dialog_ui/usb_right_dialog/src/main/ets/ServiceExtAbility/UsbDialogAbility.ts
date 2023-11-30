/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

import extension from '@ohos.app.ability.ServiceExtensionAbility';
import window from '@ohos.window';
import common from '@ohos.app.ability.common';
import rpc from '@ohos.rpc';
import Want from '@ohos.app.ability.Want';

class UsbDialogStub extends rpc.RemoteObject {
  constructor(des) {
    super(des);
  }
  onRemoteRequest(code, data, reply, option): boolean {
    return true;
  }
}

const BG_COLOR = '#33000000';

interface NavigationBarRect {
  left: number;
  top: number;
  width: number;
  height: number;
}

export default class UsbDialogAbility extends extension {
  private usbWin: window.Window | undefined = undefined;
  private mContext: common.ServiceExtensionContext | undefined = undefined;
  private windowNum: number = 0;
  /**
   * Lifecycle function, called back when a service extension is started for initialization.
   */
  onCreate(want: Want): void {
    console.log('onCreate want: ' + JSON.stringify(want));
    this.windowNum = 0;
  }

  onConnect(want: Want): rpc.RemoteObject {
    console.log('onConnect want: ' + JSON.stringify(want));
    let navigationBarRect: NavigationBarRect = {
        left: 0,
        top: 0,
        width: 300,
        height: 300,
      };
    let windowConfig: window.Configuration = {
      name: 'Usb Dialog',
      windowType: window.WindowType.TYPE_FLOAT,
      ctx: this.mContext
    };
      this.createWindow(windowConfig, navigationBarRect);

    return new UsbDialogStub('UsbRightDialog');
  }

  onDisconnect(want: Want): void {
    console.log('onDisconnect');
  }

  /**
   * Lifecycle function, called back when a service extension is started or recall.
   */
  onRequest(want: Want, startId: number): void {
    console.log('onRequest');
  }
  /**
   * Lifecycle function, called back before a service extension is destroyed.
   */
  onDestroy(): void {
    console.info('UsbDialogAbility onDestroy.');
  }

  private async createWindow(config: window.Configuration, rect: NavigationBarRect): Promise<void> {
    console.log('create windows execute');
    try {
      this.usbWin = await window.createWindow(config);
      await this.usbWin.moveWindowTo(rect.left, rect.top);
      await this.usbWin.resize(rect.width, rect.height);
      await this.usbWin.setUIContent('pages/UsbDialog');
      await this.usbWin.setWindowBackgroundColor(BG_COLOR)
      await this.usbWin.showWindow();
      console.log('UsbDialogAbility window create successfully');
    } catch (exception) {
      console.error('UsbDialogAbility Failed to create the window. Cause: ' + JSON.stringify(exception));
    }
  }
};

