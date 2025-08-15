import usb from "@ohos.usbManager";
import promptAction from '@ohos.promptAction';
import router from '@ohos.router';
import fs, { ReadOptions } from '@ohos.file.fs';
import common from '@ohos.app.ability.common';
import buffer from '@ohos.buffer';

class Logger {
  static logFilePath: string;

  static initialize(context: common.UIAbilityContext): void {
    Logger.logFilePath = `${context.filesDir}/usb_info.log`;
  }

  static clearLog(): void {
    try {
      // Open the file with write flag to truncate it
      const file = fs.openSync(Logger.logFilePath, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE | fs.OpenMode.TRUNC);
      fs.writeSync(file.fd, '');
      fs.closeSync(file);
      console.log('Log file cleared.');
    } catch (error) {
      console.error(`Failed to clear log: ${error.message}`);
    }
  }

  static log(message: string): void {
    const timestamp = new Date().toISOString();
    const logMessage = `[${timestamp}] ${message}\n`;

    try {
      // Clear the log before writing a new message

      const file = fs.openSync(Logger.logFilePath, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE | fs.OpenMode.APPEND);
      fs.writeSync(file.fd, logMessage);
      fs.closeSync(file);
    } catch (error) {
      console.error(`Failed to write log: ${error.message}`);
    }
  }

  static readLog(): string {
    try {
      const file = fs.openSync(Logger.logFilePath, fs.OpenMode.READ_ONLY);
      const arrayBuffer = new ArrayBuffer(1024);
      const readOptions: ReadOptions = {
        offset: 0,
        length: arrayBuffer.byteLength
      };
      const readLen = fs.readSync(file.fd, arrayBuffer, readOptions);
      const buf = buffer.from(arrayBuffer, 0, readLen);
      const content = buf.toString();
      fs.closeSync(file);
      return content;
    } catch (error) {
      console.error(`Failed to read log: ${error.message}`);
      return '';
    }
  }
}

@Entry
@Component
struct UsbInfoPage {
  @State devices: Array<usb.USBDevice> = [];
  @State selectedDevice: usb.USBDevice | null = null;
  @State deviceRights: Map<string, boolean> = new Map();

  aboutToAppear() {
    const context = getContext(this) as common.UIAbilityContext;
    Logger.initialize(context);
    Logger.clearLog();
    this.refreshDevices();
  }

  async refreshDevices() {
    let devicesList: Array<usb.USBDevice> = usb.getDevices();
    if (devicesList.length == 0) {
      console.warn("No device is connected");
      Logger.log('没有找到USB设备');
      promptAction.showToast({ message: '没有找到USB设备' });
      this.devices = [];
    } else {
      this.devices = devicesList;
      this.updateDeviceRights();
      this.selectedDevice = this.devices[0];
      Logger.log(`测试USB为: ${this.selectedDevice.productName}`);
      this.removeRight(this.selectedDevice.name);
      this.updateDeviceRights();
      await this.getRight();
    }
  }

  async getRight() {
    try {
      if (this.selectedDevice == null) return;
      let res = await usb.requestRight(this.selectedDevice.name);
      if (res) {
        promptAction.showToast({ message: `权限添加成功` });
        Logger.log(`权限添加测试  成功`);
      } else {
        promptAction.showToast({ message: `权限添加失败` });
        Logger.log(`权限添加测试  失败`);
      }
      this.updateDeviceRights();
    } catch (error) {
      promptAction.showToast({ message: `权限添加失败: ${error.message}` });
      Logger.log(`权限添加测试 失败: ${error.message}`);
      console.log(`权限添加失败: ${error.message}`);
    }
  }

  updateDeviceRights() {
    this.deviceRights.clear();
    this.devices.forEach(device => {
      let right: boolean = usb.hasRight(device.name);
      this.deviceRights.set(device.name, right);
    });
  }

  removeRight(deviceName: string): boolean {
    try {
      usb.removeRight(deviceName);
      Logger.log(`权限移除测试 成功`);
      return true;
    } catch (error) {
      promptAction.showToast({ message: `权限移除测试 失败: ${error.message}` });
      Logger.log(`权限移除测试 失败: ${error.message}`);
      return false;
    }
  }

  build() {
    Column() {

      Text('USB设备信息')
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .margin({ bottom: 20 });

      Scroll() {
        Column() {
          ForEach(this.devices, (device: usb.USBDevice, index) => {
            ListItem() {
              Column() {
                Text(`设备名称: ${device.name}`);
                Text(`制造商: ${device.manufacturerName}`);
                Text(`产品名: ${device.productName}`);
                Text(`版本: ${device.version}`);
                Text(`总线号: ${device.busNum}`);
                Text(`设备地址: ${device.devAddress}`);
                Text(`序列号: ${device.serial}`);
                Text(`厂商ID: ${device.vendorId}`);
                Text(`产品ID: ${device.productId}`);
                Text(`类: ${device.clazz}`);
                Text(`子类: ${device.subClass}`);
                Text(`协议: ${device.protocol}`);
                Text(`是否有权限访问: ${this.deviceRights.get(device.name) ? '是' : '否'}`);
              }
              .padding(10);
            }
            .width('100%');
          });
        }
        .width('100%');
      }
      .width('100%')
      .height('70%');

    }
    .width('100%')
    .height('100%')
    .padding(20);
  }
}