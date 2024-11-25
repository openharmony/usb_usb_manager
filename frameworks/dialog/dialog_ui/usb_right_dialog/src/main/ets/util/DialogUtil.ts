/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

const TAG: string = 'UsbDialog';

function terminateDialog(): void {
  let session = AppStorage.get<UIExtensionContentSession>('session');
  if (!session) {
    console.error(TAG + 'get session failed.');
    return;
  }
  try {
    session.terminateSelf().then(() => {
      console.info(TAG + 'terminateSelf success');
    }).catch((err: object) => {
      console.error(TAG + 'terminateSelf err:', JSON.stringify(err));
    });
  } catch (err) {
    console.error(TAG + 'terminateSelf error:', JSON.stringify(err));
  }
}

export { terminateDialog };
