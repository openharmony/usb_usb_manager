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

#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#include <functional>
#include <string>
#include <uv.h>
#include <linux/usb/functionfs.h>

class UsbDevice {
public:
    UsbDevice(int ep0, int ep1, int ep2);
    UsbDevice();
    void StartRead(uv_loop_t *loop);

private:
    void LoopOnce();
    static void HandleLoop(uv_timer_t *handle);
    bool ReadEp0Event(struct usb_functionfs_event &event);
    bool ReceiveZLP(int epFd);
    // 控制传输回显、读、写
    void ControlTransferEcho(struct usb_functionfs_event &event);
    void ControlTransferRead(struct usb_functionfs_event &event);
    void ControlTransferWrite(struct usb_functionfs_event &event);
    // 批量传输回显、读、写
    void BulkTransferEcho(struct usb_functionfs_event &event);
    void BulkTransferRead(struct usb_functionfs_event &event);
    void BulkTransferWrite(struct usb_functionfs_event &event);
    int ep0fd_ = -1;
    int ep1fd_ = -1;
    int ep2fd_ = -1;
    uv_timer_t timer_;
};
#endif