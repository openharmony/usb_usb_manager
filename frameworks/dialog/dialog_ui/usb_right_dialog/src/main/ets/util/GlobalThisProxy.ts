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

import type common from '@ohos.app.ability.common';
//构造单例对象
export class GlobalThisProxy {
  private constructor() {}
  private static instance: GlobalThisProxy;
  private extensionMap = new Map<string, common.ServiceExtensionContext>();
  private infoNumMap = new Map<string, number>();
  private infoStringMap = new Map<string, string>();

  public static getInstance() : GlobalThisProxy {
    if (!GlobalThisProxy.instance) {
      GlobalThisProxy.instance = new GlobalThisProxy();
    }
    return GlobalThisProxy.instance;
  }

  getContext(key:string): common.ServiceExtensionContext {
    let value = this.extensionMap.get(key);
    if (value === undefined) {
      return null;
    }
    return value;
  }

  setContext(key: string, value: common.ServiceExtensionContext): void {
    this.extensionMap.set(key, value);
  }

  getNumInfo(key: string): number {
    let value = this.infoNumMap.get(key);
    if (value === undefined) {
      return 0;
    }
    return value;
  }

  setNumInfo(key: string, value: number): void {
    this.infoNumMap.set(key, value);
  }

  getStringInfo(key: string): string {
    let value = this.infoStringMap.get(key);
    if (value === undefined) {
      return '';
    }
    return value;
  }

  setStringInfo(key: string, value: string): void {
    this.infoStringMap.set(key, value);
  }
}