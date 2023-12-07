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

import Ability from '@ohos.app.ability.UIAbility';
import common from '@ohos.app.ability.common';
import Logger from '../util/Logger';
import commonEvent from '@ohos.commonEventManager';
import Want from '@ohos.app.ability.Want';
import { BusinessError } from '@ohos.base';

const TAG: string = 'usbfunctionswitchwindow_MainAbility';

export default class MainAbility extends Ability {
  private extensionWin: window.windowStage | undefined = undefined;
  private mContext: common.ServiceExtensionContext | undefined = undefined;
  onCreate(want: Want, launchParam: string): void {
    console.log(TAG, 'onCreate');
    Logger.info(TAG, `onCreate want ${JSON.stringify(want)} launchParam=${JSON.stringify(launchParam)}`);
    this.mContext = this.context;
    return;
  }

  onWindowStageCreate(): void {
    // Main window is created, set main page for this ability
    Logger.info(TAG, 'onWindowStageCreate');
    this.extensionWin.loadContent('pages/Index', (err: BusinessError, data: commonEvent.Support) => {
      if (err.code) {
        Logger.error(TAG, `Failed to load the content. Cause: ${JSON.stringify(err)}`);
        return;
      }
      Logger.info(TAG, `Succeeded in loading the content. Data: ${JSON.stringify(data)}`);
    });
    return;
  }

  onForeground(): void {
    // Ability has brought to foreground
    Logger.info(TAG, 'onForeground');
    return;
  }

  onBackground(): void {
    // Ability has back to background
    Logger.info(TAG, 'onBackground');
    return;
  }

  onDestroy(): void {
    Logger.info(TAG, 'onDestroy');
    return;
  }

  onWindowStageDestroy(): void {
    // Main window is destroyed, release UI related resources
    Logger.info(TAG, 'onWindowStageDestroy');
    return;
  }

};
