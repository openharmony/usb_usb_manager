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
import display from '@ohos.display';
import rpc from '@ohos.rpc';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl';
import type { Permissions } from '@ohos.abilityAccessCtrl';

class UsbDialogStub extends rpc.RemoteObject {
  constructor(des) {
    super(des);
  }
  onRemoteRequest(code, data, reply, option): boolean {  
    return true;
  }
}

const BG_COLOR = '#33000000';
const ENTERPRISE_MANAGE_USB = 'ohos.permission.ENTERPRISE_MANAGE_USB';
const ACCESS_TYPE_MASK = 0b11;
const SHIFT_DIGIT = 27;
const TOKEN_NATIVE = 1;

export default class UsbDialogAbility extends extension {
  /**
   * Lifecycle function, called back when a service extension is started for initialization.
   */
  onCreate(want): void {
    console.log('onCreate want: ' + JSON.stringify(want));
    globalThis.extensionContext = this.context;
    globalThis.want = want;
    globalThis.windowNum = 0;
  }

  onConnect(want): rpc.RemoteObject {
    console.log('onConnect want: ' + JSON.stringify(want));
    let callingTokenId: number = rpc.IPCSkeleton.getCallingTokenId();
    if (!this.isSystemAbility(callingTokenId) && !this.checkPermission(callingTokenId, ENTERPRISE_MANAGE_USB)) {
      console.error('check Permission fail');
      return new UsbDialogStub('UsbRightDialog');
    }
    if (!want.parameters.bundleName || !want.parameters.deviceName || !want.parameters.tokenId) {
      console.error('onConnect code:1 failed. bundleName|deviceName|tokenId');
      return new UsbDialogStub('UsbRightDialog');
    }
    display.getDefaultDisplay().then(dis => {
      let navigationBarRect = {
        left: 0,
        top: 0,
        width: dis.width,
        height: dis.height
      };
      this.createWindow('UsbDialogAbility', window.WindowType.TYPE_FLOAT, navigationBarRect);
    });
    return new UsbDialogStub('UsbRightDialog');
  }

  onDisconnect(want): void {
    console.log('onDisconnect');
  }

  /**
   * Lifecycle function, called back when a service extension is started or recall.
   */
  onRequest(want, startId): void {
    console.log('onRequest');
  }
  /**
   * Lifecycle function, called back before a service extension is destroyed.
   */
  onDestroy(): void {
    console.info('UsbDialogAbility onDestroy.');
  }

  private async createWindow(name: string, windowType: number, rect): Promise<void> {
    console.log('create windows execute');
    try {
      let config = {name: name, windowType: windowType, ctx: globalThis.extensionContext};
      const usbWin = await window.createWindow(config);
      console.log('createWindow execute');
      globalThis.window = usbWin;
      //let token: UsbDialogStub = new UsbDialogStub('UsbRightDialog');
      //await usbWin.bindDialogTarget(token);
      //console.log('createWindow bindDialogTarget');

      await usbWin.moveTo(rect.left, rect.top);
      await usbWin.resetSize(rect.width, rect.height);
      await usbWin.loadContent('pages/UsbDialog');
      console.log('loadContent execute');
      await usbWin.setBackgroundColor(BG_COLOR);
      await usbWin.showWindow();
      console.log('show execute');
      let shouldHide = true;
      usbWin.hideNonSystemFloatingWindows(shouldHide, (err) => {
        if (err.code) {
          console.error('Failed to hide the non-system floating windows. Cause: ' + JSON.stringify(err));
          return;
        }
        console.info('Succeeded in hiding the non-system floating windows.');
      });
      console.log('UsbDialogAbility window create successfully');
    } catch {
      console.info('UsbDialogAbility window create failed');
    }
  }

  private isSystemAbility(callingTokenId: number): boolean {
    let type: number = ACCESS_TYPE_MASK & (callingTokenId >> SHIFT_DIGIT);
    console.info('isSystemAbility, type:' + type);
    return type === TOKEN_NATIVE;
  }

  private checkPermission(tokenID: number, permissionName: Permissions): boolean {
    let aac = abilityAccessCtrl.createAtManager();
    try {
      let grantStatus = aac.verifyAccessTokenSync(tokenID, permissionName);
      if (grantStatus === abilityAccessCtrl.GrantStatus.PERMISSION_DENIED) {
        console.error(`verify ${permissionName} fail`);
      }
    } catch (error) {
      console.error(`verify ${permissionName}, ${error}`);
    }
    console.info(`verify ${permissionName}, success`);
    return true;
  }
};

