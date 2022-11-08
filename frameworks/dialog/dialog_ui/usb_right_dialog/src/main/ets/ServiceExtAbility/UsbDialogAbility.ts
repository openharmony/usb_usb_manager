/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import extension from '@ohos.application.ServiceExtensionAbility'
import window from '@ohos.window';
import display from '@ohos.display';
import rpc from '@ohos.rpc'

class UsbDialogStub extends rpc.RemoteObject {
    constructor(des) {
        super(des);
    }
    onRemoteRequest(code, data, reply, option) {
        return true;
    }
}

const BG_COLOR = '#33000000'

export default class UsbDialogAbility extends extension {
    /**
     * Lifecycle function, called back when a service extension is started for initialization.
     */
    onCreate(want) {
        console.log("onCreate want: " + JSON.stringify(want))
        globalThis.extensionContext = this.context;
        globalThis.want = want;

        this.context.resourceManager.getStringValue($r('app.string.allow').id, (error, value) => {
            if (error != null) {
                console.log("error is " + error);
            } else {
                globalThis.allow = value;
            }
        });

        this.context.resourceManager.getStringValue($r('app.string.use').id, (error, value) => {
            if (error != null) {
                console.log("error is " + error);
            } else {
                globalThis.use = value;
            }
        });
        globalThis.windowNum = 0
    }

    onConnect(want) {
        console.log("onConnect want: " + JSON.stringify(want))
        display.getDefaultDisplay().then(dis => {
            let navigationBarRect = {
                left: 0,
                top: 0,
                width: dis.width,
                height: dis.height
            }
            this.createWindow("UsbDialogAbility", window.WindowType.TYPE_FLOAT, navigationBarRect)
        })
        return new UsbDialogStub("UsbRightDialog");
    }

    onDisconnect(want) {
        console.log("onDisconnect")
    }

    /**
     * Lifecycle function, called back when a service extension is started or recall.
     */
    onRequest(want, startId) {
        console.log("onRequest")
    }
    /**
     * Lifecycle function, called back before a service extension is destroyed.
     */
    onDestroy() {
        console.info("UsbDialogAbility onDestroy.");
    }

    private async createWindow(name: string, windowType: number, rect) {
        console.log("create windows execute")
        try {
            const usbWin = await window.create(globalThis.extensionContext, name, windowType)
            globalThis.window = usbWin
            await usbWin.moveTo(rect.left, rect.top)
            await usbWin.resetSize(rect.width, rect.height)
            await usbWin.loadContent('pages/UsbDialog')
            await usbWin.setBackgroundColor(BG_COLOR)
            await usbWin.show()
            console.log("UsbDialogAbility window create successfully")
        } catch {
            console.info("UsbDialogAbility window create failed")
        }
    }
};

