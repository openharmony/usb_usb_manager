import router from '@ohos.router';

@Entry
@Component
struct Index {
  @State message: string = 'USB测试';
  @State versionInfo: string = 'version 1.0'

  navigateToAutomation() {
    router.pushUrl({ url: 'pages/automation' });
  }

  navigateToUsbInfo() {
    router.pushUrl({ url: 'pages/usb_info' });
  }

  navigateToUsbListening() {
    router.pushUrl({ url: 'pages/usb_listening' });
  }

  navigateToSplitTest() {
    router.pushUrl({ url: 'pages/split_test' });
  }

  navigateToManualTest() {
    router.pushUrl({ url: 'pages/manual_test' });
  }

  build() {
    Column() {
      // Header Section with background image and Huawei icon
      Column() {
        // Row to place icon and title side by side
        Row() {
          // Huawei Icon
          Image($r('app.media.huawei'))  // Use the resource reference for the local icon
            .width(60)
            .height(60)
            .margin({ right: '30px' })  // Add margin to the right of the icon

          // Title
          Text(this.message)
            .fontSize(36)
            .fontWeight(FontWeight.Bold)
            .textAlign(TextAlign.Start)  // Ensure text starts from the left
            .fontColor('#ff000000')
        }
        .alignItems(VerticalAlign.Center)  // Center align items in the row
        .margin({ top: '200px', bottom: '20px' })

        Text(this.versionInfo)
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .textAlign(TextAlign.Center)
          .fontColor('#ff000000')
          .margin({ bottom: '40px' })
      }
      .height('30%')
      .justifyContent(FlexAlign.Center)
      .padding({ top: '20px', bottom: '20px' })

      // Content Section with buttons
      Column() {
        Button('USB设备信息及权限获取')
          .onClick(this.navigateToUsbInfo)
          .backgroundColor('#2196F3')
          .fontColor('#FFFFFF')
          .fontSize(18)
          .borderRadius(8)
          .width('80%')
          .height('50px')
          .margin({ bottom: '20px' })
          .shadow({ color: '#000000', radius: 5, offsetX: 0, offsetY: 2})
          .alignSelf(ItemAlign.Center)

        Button('API自动化测试')
          .onClick(this.navigateToAutomation)
          .backgroundColor('#2196F3')
          .fontColor('#FFFFFF')
          .fontSize(18)
          .borderRadius(8)
          .width('80%')
          .height('50px')
          .margin({ bottom: '20px' })
          .shadow({ color: '#000000', radius: 5, offsetX: 0, offsetY: 2})
          .alignSelf(ItemAlign.Center)

        Button('插拔事件监听服务')
          .onClick(this.navigateToUsbListening)
          .backgroundColor('#2196F3')
          .fontColor('#FFFFFF')
          .fontSize(18)
          .borderRadius(8)
          .width('80%')
          .height('50px')
          .margin({ bottom: '20px' })
          .shadow({ color: '#000000', radius: 5, offsetX: 0, offsetY: 2})
          .alignSelf(ItemAlign.Center)

        Button('接口分离测试')
          .onClick(this.navigateToSplitTest)
          .backgroundColor('#2196F3')
          .fontColor('#FFFFFF')
          .fontSize(18)
          .borderRadius(8)
          .width('80%')
          .height('50px')
          .margin({ bottom: '20px' })
          .shadow({ color: '#000000', radius: 5, offsetX: 0, offsetY: 2})
          .alignSelf(ItemAlign.Center)

        Button('端口信息获取及设备主机切换')
          .onClick(this.navigateToManualTest)
          .backgroundColor('#2196F3')
          .fontColor('#FFFFFF')
          .fontSize(18)
          .borderRadius(8)
          .width('80%')
          .height('50px')
          .margin({ bottom: '20px' })
          .shadow({ color: '#000000', radius: 5, offsetX: 0, offsetY: 2})
          .alignSelf(ItemAlign.Center)
      }
      .height('70%')
      .justifyContent(FlexAlign.Center)
      .padding({ top: '20px', bottom: '20px' })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#ffffffff')
  }
}