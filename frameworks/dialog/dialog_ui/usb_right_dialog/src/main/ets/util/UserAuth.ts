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

import cryptoFramwork from '@ohos.security.cryptoFramework';
import userAuth from '@ohos.userIAM.userAuth';
import { BusinessError } from '@ohos.base';

export function getUserAuth(prompt: ResourceStr, onSuccess: () => void, onFailure?: () => void) {
  try {
    const rand = cryptoFramwork.createRandom();
    const len: number = 16; // Generate a 16-byte random number
    let randData: Uint8Array | null = null;
    for (let retryCount = 0; retryCount < 3 && !randData; retryCount++) {
      randData = rand?.generateRandomSync(len)?.data ?? null;
    }
    if (!randData) {
      console.error(`usb_service userAuth: failed to generate randData`);
      onFailure?.()
      return;
    }
    const authParam: userAuth.AuthParam = {
      challange: randData,
      authType: [userAuth.UserAuthType.PIN, userAuth.UserAuthType.FACE, userAuth.UserAuthType.FINGERPRINT],
      authTrustLevel: userAuth.AuthTrustLevel.ATL3,
    };
    let promptString: string = '';
    if (typeof prompt === 'string') {
      promptString = prompt;
    } else {
      promptString = getContext().resourceManager.getStringSync(prompt);
    }
    const widgetParam: userAuth.WidgetParam = {
      title: promptString,
    };
    // get authorization instance
    const userAuthInstance = userAuth.getUserAuthInstance(authParam, widgetParam);
    console.info('usb_service get userAuth instance success');
    // subscribe authorization result
    userAuthInstance.on('result', {
      onResult(result) {
        console.info(`usb_service userAuth callback result=${JSON.stringify(result)}`);
        // unsubscrite after result received
        userAuthInstance.off('result');
        if (result.result == userAuth.UserAuthResultCode.SUCCESS ||
          result.result == userAuth.UserAuthResultCode.NOT_ENROLLED) {
          onSuccess();
        } else {
          console.error(`usb_service authorization failed, result is ${result.result}`);
          onFailure?.();
        }
      }
    });
    userAuthInstance.start();
    console.info('usb_service userAuth start success');
  } catch (error) {
    const err: BusinessError = error as BusinessError;
    console.error(`usb_service userAuth error. Code=${err?.code}, message=${err?.message}`);
    onFailure?.();
  }
}