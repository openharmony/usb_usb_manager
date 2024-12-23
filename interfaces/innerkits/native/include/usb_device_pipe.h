/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef USB_DEVICE_PIPE_H
#define USB_DEVICE_PIPE_H

#include "iusb_srv.h"
#include "usb_config.h"
#include "v1_2/usb_types.h"

namespace OHOS {
namespace USB {
class USBDevicePipe {
public:
    USBDevicePipe();
    ~USBDevicePipe() {}
    USBDevicePipe(uint8_t busNum, uint8_t devAddr);
    int32_t ClaimInterface(const UsbInterface &interface, bool force);
    int32_t ReleaseInterface(const UsbInterface &interface);
    int32_t BulkTransfer(const USBEndpoint &endpoint, std::vector<uint8_t> &bufferData, int32_t timeOut);
    
    int32_t UsbSubmitTransfer(HDI::Usb::V1_2::USBTransferInfo &info, const TransferCallback &cb,
        sptr<Ashmem> &ashmem);
    int32_t UsbCancelTransfer(const int32_t &endpoint);
    
    int32_t ControlTransfer(const HDI::Usb::V1_0::UsbCtrlTransfer &ctrl, std::vector<uint8_t> &bufferData);
    int32_t UsbControlTransfer(
        const HDI::Usb::V1_2::UsbCtrlTransferParams &ctrlParams, std::vector<uint8_t> &bufferData);
    int32_t SetConfiguration(const USBConfig &config);
    int32_t SetInterface(const UsbInterface &interface);
    int32_t Close();

    void SetBusNum(uint8_t busNum);
    void SetDevAddr(uint8_t devAddr);
    uint8_t GetBusNum() const;
    uint8_t GetDevAddr() const;

private:
    uint8_t busNum_ = UINT8_MAX;
    uint8_t devAddr_ = UINT8_MAX;
};
} // namespace USB
} // namespace OHOS

#endif // USB_DEVICE_PIPE_H
