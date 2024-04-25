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
import UIExtensionAbility from '@ohos.app.ability.UIExtensionAbility'
import UIExtensionContentSession from '@ohos.app.ability.UIExtensionContentSession'
import rpc from '@ohos.rpc';

let TAG: string = 'UsbService';
const ACCESS_TYPE_MASK = 0b11;
const SHIFT_DIGIT = 27;
const TOKEN_NATIVE = 1;
const TOKEN_HAP = 0;
const BG_COLOR = '#00000000';

export default class UsbFunctionSwitchAbility extends UIExtensionAbility {
  onSessionCreate(want, session): void {
    console.log(TAG + 'UsbFunctionSwitchAbility onSessionCreate');
    let callingTokenId: number = rpc.IPCSkeleton.getCallingTokenId();
    if (!this.isSystemAbility(callingTokenId)) {
      console.error('check permission fail');
      return;
    }
    AppStorage.setOrCreate("session", session);
    (session as UIExtensionContentSession)?.loadContent('pages/Index');
    try {
      (session as UIExtensionContentSession)?.setWindowBackgroundColor(BG_COLOR);
    } catch (error) {
      console.error(TAG + 'UsbFunctionSwitchAbility onSessionCreate error: ' + error?.code);
      (session as UIExtensionContentSession)?.terminateSelf();
    }
    return;
  }

  onSessionDestroy(session): void {
    console.log(TAG + 'UsbFunctionSwitchAbility onSessionDestroy');
  }

  private isSystemAbility(callingTokenId: number): boolean {
    let type: number = ACCESS_TYPE_MASK & (callingTokenId >> SHIFT_DIGIT);
    console.info('isSystemAbility, type:' + type);
    return type === TOKEN_NATIVE || type === TOKEN_HAP;
  }
};

