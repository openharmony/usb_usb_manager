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

#include "usb_device.h"
#include "cstdio"
#include "fcntl.h"
#include "parameter.h"
#include "unistd.h"
#include "usb_common_test.h"
#include <bitset>
#include <cerrno>
#include <iostream>
#include <signal.h>
#include <string>

using namespace std;

#define BUFFER_SIZE 409600

UsbDevice::UsbDevice()
{
    cout << "new empty UsbDevice" << endl;
}

UsbDevice::UsbDevice(int ep0, int ep1, int ep2) : ep0fd_(ep0), ep1fd_(ep1), ep2fd_(ep2)
{
    cout << "new UsbDevice ep0: " << ep0fd_ << " , ep1: " << ep1fd_ << " , ep2: " << ep2fd_ << endl;
}

void UsbDevice::StartRead(uv_loop_t *loop)
{
    int iuv = uv_timer_init(loop, &timer_);
    cout << "uv_timer_init ret:" << iuv << endl;
    timer_.data = this;
    uv_timer_start(&timer_, UsbDevice::HandleLoop, 0, 1000);
}

void UsbDevice::HandleLoop(uv_timer_t *handle)
{
    cout << "------------------Loop start" << endl;
    UsbDevice *dev = (UsbDevice *)handle->data;
    dev->LoopOnce();
    cout << "------------------Loop end" << endl;
}

void UsbDevice::LoopOnce()
{
    struct usb_functionfs_event event;
    if (!ReadEp0Event(event)) {
        return;
    }

    switch (event.u.setup.bRequest) {
        case 0xA1:
            if (event.u.setup.wIndex == 0x01) {
                ControlTransferEcho(event);
            } else if (event.u.setup.wIndex == 0x02) {
                ControlTransferRead(event);
            } else if (event.u.setup.wIndex == 0x03) {
                ControlTransferWrite(event);
            } else {
                cout << "unsupported command" << endl;
            }
            break;
        case 0xA2:
            ReceiveZLP(ep0fd_); // length of control transfer data is 0
            if (event.u.setup.wIndex == 0x01) {
                BulkTransferEcho(event);
            } else if (event.u.setup.wIndex == 0x02 || event.u.setup.wIndex == 0x04) {
                BulkTransferRead(event);
            } else if (event.u.setup.wIndex == 0x03) {
                BulkTransferWrite(event);
            } else {
                cout << "unsupported command" << endl;
            }
            break;
        default:
            cout << "unsupported command" << endl;
    }
}

bool UsbDevice::ReadEp0Event(struct usb_functionfs_event &event)
{
    int ret = read(ep0fd_, &event, sizeof(event));
    if (ret != sizeof(event)) {
        cout << "read event failed: " << ret << " , err:" << errno << endl;
        return false;
    }

    cout << "Read event success: " << ret << "/" << sizeof(event) << endl;
    cout << "--Ep0 event: \nType :" << std::hex << (int)event.type << endl;
    cout << "--bRequestType:" << bitset<8>(event.u.setup.bRequestType) << endl;
    cout << "--bRequest:" << std::hex << (int)event.u.setup.bRequest << endl;
    cout << "--wValue:" << std::hex << (int)event.u.setup.wValue << endl;
    cout << "--wIndex:" << std::hex << (int)event.u.setup.wIndex << endl;
    cout << "--wLength:" << std::dec << (int)event.u.setup.wLength << endl;
    return true;
}

bool UsbDevice::ReceiveZLP(int epFd)
{
    char buffer[0];
    int ret = read(epFd, buffer, 0);
    if (ret != 0) {
        cout << "receive zlp failed : fd=" << epFd << " ,ret= " << ret << " ,errno= " << errno << endl;
        return false;
    }
    return true;
}

void UsbDevice::ControlTransferEcho(struct usb_functionfs_event &event)
{
    cout << "-----" << __FUNCTION__ << "---Enter " << endl;
    char buffer[BUFFER_SIZE] {0};
    int ret = read(ep0fd_, buffer, event.u.setup.wLength);
    if (ret != event.u.setup.wLength) {
        cout << "receive data failed: " << ret << " : " << errno << endl;
        return;
    }
    cout << "received data: " << ret << endl;

    struct usb_functionfs_event evt;
    if (!ReadEp0Event(evt)) {
        return;
    }
    ret = write(ep0fd_, buffer, evt.u.setup.wLength);
    if (ret != evt.u.setup.wLength) {
        cout << "send data failed: " << ret << " : " << errno << endl;
        return;
    }
    cout << "sent data: " << ret << endl;

    cout << "-----" << __FUNCTION__ << "---End " << endl;
}

static void inline Delay(struct usb_functionfs_event &event)
{
    int delayMs = event.u.setup.wValue;
    if (delayMs > 0) {
        cout << "Delaying " << delayMs << " ms" << endl;
        usleep(delayMs * 1000);
        cout << "Delay end" << endl;
    }
}

void UsbDevice::ControlTransferRead(struct usb_functionfs_event &event)
{
    cout << "-----" << __FUNCTION__ << "---Enter " << endl;
    char buffer[BUFFER_SIZE] {0};
    for (int i = 0; i < event.u.setup.wLength; i++) {
        buffer[i] = (i % 10) + '0';
    }

    Delay(event);

    int ret = write(ep0fd_, buffer, event.u.setup.wLength);
    cout << "sent data size:" << ret << endl;
    cout << "-----" << __FUNCTION__ << "---End " << endl;
}

void UsbDevice::ControlTransferWrite(struct usb_functionfs_event &event)
{
    cout << "-----" << __FUNCTION__ << "---Enter " << endl;
    Delay(event);
    char buffer[BUFFER_SIZE] {0};
    int ret = read(ep0fd_, buffer, event.u.setup.wLength);
    cout << "received data: " << endl;
    cout << buffer << endl;
    cout << "received data size:" << ret << endl;
    cout << "-----" << __FUNCTION__ << "---End " << endl;
}

void UsbDevice::BulkTransferEcho(struct usb_functionfs_event &event)
{
    cout << "-----" << __FUNCTION__ << "---Enter " << endl;
    char buffer[BUFFER_SIZE] {0};
    int ret = read(ep1fd_, buffer, BUFFER_SIZE);
    if (ret < 0) {
        cout << "receive data failed: " << ret << " : " << errno << endl;
        return;
    }
    cout << "bulk received bytes: " << ret << endl;
    int ret2 = write(ep2fd_, buffer, ret);
    if (ret2 != ret) {
        cout << "send data failed: " << ret2 << " : " << errno << endl;
        return;
    }
    cout << "bulk sent bytes: " << ret2 << endl;
    cout << "-----" << __FUNCTION__ << "---End " << endl;
}

void UsbDevice::BulkTransferRead(struct usb_functionfs_event &event)
{
    cout << "-----" << __FUNCTION__ << "---Enter " << endl;
    int length = 10;
    if (event.u.setup.wIndex == 0x04) {
        Delay(event);
    } else {
        length = event.u.setup.wValue;
    }
    char buffer[BUFFER_SIZE] {0};
    for (int i = 0; i < length; i++) {
        buffer[i] = (i % 10) + '0';
    }
    int ret = write(ep2fd_, buffer, length);
    cout << "sent data size:" << ret << endl;
    cout << "-----" << __FUNCTION__ << "---End " << endl;
}

void UsbDevice::BulkTransferWrite(struct usb_functionfs_event &event)
{
    cout << "-----" << __FUNCTION__ << "---Enter " << endl;
    Delay(event);
    char buffer[BUFFER_SIZE] {0};
    int ret = read(ep1fd_, buffer, BUFFER_SIZE);
    if (ret < 0) {
        cout << "receive data failed: " << ret << " : " << errno << endl;
        return;
    }
    cout << "received data: " << endl;
    cout << buffer << endl;
    cout << "received data size:" << ret << endl;
    cout << "-----" << __FUNCTION__ << "---End " << endl;
}