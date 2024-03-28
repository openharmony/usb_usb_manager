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
import { notificationUtil } from '../util/NotificationUtil';
import {GlobalThisProxy} from '../util/GlobalThisProxy';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl';
import type { Permissions } from '@ohos.abilityAccessCtrl';

let TAG: string = 'usbfunctionswitchwindow_ServiceExtensionAbility';
let WINDOW_NAME: string = 'UsbFunctionSwitchWindow';
class UsbFunctionSwitchStub extends rpc.RemoteObject {
  constructor(des) {
    super(des);
  }
  onRemoteRequest(code, data, reply, option): boolean {
    return true;
  }
}

const BG_COLOR = '#00000000';
let usbServiceSwitchWindowFirst = undefined;
const ENTERPRISE_MANAGE_USB = 'ohos.permission.ENTERPRISE_MANAGE_USB';
const ACCESS_TYPE_MASK = 0b11;
const SHIFT_DIGIT = 27;
const TOKEN_NATIVE = 1;

export default class UsbFunctionSwitchAbility extends ServiceExtensionAbility {

  /**
   * Lifecycle function, called back when a service extension is started for initialization.
   */
  onCreate(want): void {
    console.log('onCreate want: ' + JSON.stringify(want));
    GlobalThisProxy.getInstance().setContext('extensionAbility', this.context);
    globalThis.extensionContext = this.context;
    globalThis.want = want;
    globalThis.windowNum = 0;
    globalThis.usbServiceSwitchWindowFirst = usbServiceSwitchWindowFirst;
  }

  /**
   * Lifecycle function, called back when a service extension is started or recall.
   */
  onRequest(want, startId): void {
    console.log('onRequest want: ' + JSON.stringify(want));
    GlobalThisProxy.getInstance().setNumInfo('workWant_defaultChoose', want.parameters.defaultChoose);
    GlobalThisProxy.getInstance().setNumInfo('workWant_funcSelect', want.parameters.funcSelect);
    globalThis.abilityWant = want;
    let displayClass: display.Display | null = null;
    try {
      displayClass = display.getDefaultDisplaySync();
    } catch {
      console.info('display.getDefaultDisplaySync failed');
    }
  
    let navigationBarRect = {
      left: 0,
      top: 0,
      width: displayClass.width,
      height: displayClass.height
    };
    
    this.createOrShowUsbWindow(WINDOW_NAME, window.WindowType.TYPE_FLOAT, navigationBarRect);
  }

  onConnect(want): rpc.RemoteObject {
    globalThis.abilityWant = want;
    console.log('onConnect want: ' + JSON.stringify(want));
    let callingTokenId: number = rpc.IPCSkeleton.getCallingTokenId();
    if (!this.isSystemAbility(callingTokenId) && !this.checkPermission(callingTokenId, ENTERPRISE_MANAGE_USB)) {
      console.error('check permission fail');
      return null;
    }
    if (!want.parameters.bundleName || !want.parameters.deviceName || !want.parameters.tokenId) {
      console.error('onConnect code:1 failed. bundleName|deviceName|tokenId');
      return null;
    }
    return new UsbFunctionSwitchStub('test');
  }


  onDisconnect(want): void {
    console.log('onDisconnect');
    return;
  }


  /**
   * Lifecycle function, called back before a service extension is destroyed.
   */
  onDestroy(): void {
    console.log(TAG + 'onDestroy');
    if (globalThis.window) {
      globalThis.usbServiceSwitchWindowFirst = undefined;
      notificationUtil.cancelAllNotifications();
      globalThis.window.destroy((err, data) => {
        if (err.code) {
          console.error(TAG + 'Failed to destroy the window. Cause: ' + JSON.stringify(err));
          return;
        }
        console.log(TAG + 'Succeeded in destroy the window');
        globalThis.window = undefined;
      });
    }
  }

  private async createOrShowUsbWindow(winName: string, windowType: number, rect): Promise<void> {
    try {
      if (globalThis.usbServiceSwitchWindowFirst !== undefined) {
        console.log(TAG + 'destroy first power window');
        globalThis.usbServiceSwitchWindowFirst.destroy();
        globalThis.usbServiceSwitchWindowFirst = undefined;
      }

      let config = {name: winName, windowType: windowType, ctx: globalThis.extensionContext};
      const usbWin = await window.createWindow(config);
      if (globalThis.usbServiceSwitchWindowFirst === undefined) {
        console.log(TAG + 'usbServiceSwitchWindowFirst not init, init now');
        usbServiceSwitchWindowFirst = usbWin;
        globalThis.usbServiceSwitchWindowFirst = usbServiceSwitchWindowFirst;
      }
      globalThis.window = usbWin;
      await usbWin.moveWindowTo(rect.left, rect.top);
      await usbWin.resize(rect.width, rect.height);
      await usbWin.setUIContent('pages/Index');
      await usbWin.setWindowBackgroundColor(BG_COLOR);
      await usbWin.showWindow();
      console.log('UsbFunctionSwitchAbility window create successfully');
    } catch {
      console.info('UsbFunctionSwitchAbility window create failed');
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

