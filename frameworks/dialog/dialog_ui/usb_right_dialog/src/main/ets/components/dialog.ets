/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

import { BusinessError, Callback } from '@ohos.base';
import hilog from '@ohos.hilog';
import measure from '@ohos.measure';
import resourceManager from '@ohos.resourceManager';
import { CustomColors, CustomTheme, Theme } from '@ohos.arkui.theme';
import { LengthMetrics, LengthUnit } from '@ohos.arkui.node';
import common from '@ohos.app.ability.common';

export declare class ButtonOptions {
  public value: ResourceStr;
  public action?: () => void;
  public background?: ResourceColor;
  public fontColor?: ResourceColor;
  public buttonStyle?: ButtonStyleMode;
  public role?: ButtonRole;
  public defaultFocus?: boolean;
}

class CustomThemeImpl implements CustomTheme {
  public colors?: CustomColors;

  constructor(colors: CustomColors) {
    this.colors = colors;
  }
}

const TITLE_MAX_LINES: number = 2;
const HORIZON_BUTTON_MAX_COUNT: number = 2;
const VERTICAL_BUTTON_MAX_COUNT: number = 4;
const BUTTON_LAYOUT_WEIGHT: number = 1;
const LIST_MIN_HEIGHT: number = 48;
const CHECKBOX_CONTAINER_LENGTH: number = 20;
const MIN_CONTENT_HEIGHT: number = 100;
const MAX_CONTENT_HEIGHT: number = 30000;
const FIRST_ITEM_INDEX: number = 0;
const BUTTON_MIN_FONT_SIZE = 9;
const MAX_FONT_SCALE: number = 2;
// 'sys.float.alert_right_padding_horizontal'
const BUTTON_HORIZONTAL_MARGIN: number = getNumberByResourceId(125831054, 16);
// 'sys.float.padding_level8'
const BUTTON_HORIZONTAL_PADDING: number = getNumberByResourceId(125830927, 16);
// 'sys.float.alert_button_horizontal_space'
const BUTTON_HORIZONTAL_SPACE: number = getNumberByResourceId(125831051, 8);
// 'sys.float.Body_L'
const BODY_L = getNumberByResourceId(125830970, 16);
// 'sys.float.Body_M'
const BODY_M = getNumberByResourceId(125830971, 14);
// 'sys.float.Body_S'
const BODY_S = getNumberByResourceId(125830972, 12);
// 'sys.float.Title_S'
const TITLE_S = getNumberByResourceId(125830966, 20);
// 'sys.float.Subtitle_S'
const SUBTITLE_S = getNumberByResourceId(125830969, 14);
// 'sys.float.dialog_divider_show'
const DIALOG_DIVIDER_SHOW = getNumberByResourceId(125831202, 1, true);
// 'sys.float.alert_button_style'
const ALERT_BUTTON_STYLE = getNumberByResourceId(125831085, 2, true);
// 'sys.float.alert_title_alignment'
const ALERT_TITLE_ALIGNMENT = getEnumNumberByResourceId(125831126, 1);

export interface SheetInfoEx extends SheetInfo {
  subTitle?: ResourceStr;
}

@CustomDialog
export struct SelectDialog {
  controller: CustomDialogController;
  title: ResourceStr = '';
  content?: ResourceStr = '';
  confirm?: ButtonOptions | null = null;
  radioContent: Array<SheetInfoEx> = [];
  buttons?: ButtonOptions[] = [];
  contentPadding ?: Padding;
  isFocus: boolean = false;
  currentFocusIndex?: number = -1;
  radioHeight: number = 0;
  itemHeight: number = 0;
  @State selectedIndex?: number = -1;
  @BuilderParam contentBuilder: () => void = this.buildContent;
  @State fontColorWithTheme: ResourceColor = $r('sys.color.font_primary');
  @State dividerColorWithTheme: ResourceColor = $r('sys.color.comp_divider');
  theme?: Theme | CustomTheme = new CustomThemeImpl({});
  themeColorMode?: ThemeColorMode = ThemeColorMode.SYSTEM;
  // the controller of content list
  contentScroller: Scroller = new Scroller();
  @State fontSizeScale: number = 1;
  @State minContentHeight: number = MIN_CONTENT_HEIGHT;

  @Styles
  paddingContentStyle() {
    .padding({
      left: $r('sys.float.padding_level12'),
      right: $r('sys.float.padding_level12'),
      bottom: $r('sys.float.padding_level4')
    })
  }

  @Styles
  paddingStyle() {
    .padding({
      left: $r('sys.float.padding_level6'),
      right: $r('sys.float.padding_level6')
    })
  }

  @Builder
  buildContent(): void {
    Scroll(this.contentScroller) {
      Column() {
        if (this.content) {
          Row() {
            Text(this.content)
              .fontSize(`${BODY_M}fp`)
              .fontWeight(FontWeight.Regular)
              .fontColor(this.fontColorWithTheme)
              .textOverflow({ overflow: TextOverflow.Ellipsis })
          }.paddingContentStyle().width('100%')
        }
        List() {
          ForEach(this.radioContent, (item: SheetInfoEx, index: number) => {
            ListItem() {
              Column() {
                Button() {
                  Row() {
                    Flex({ direction: FlexDirection.Column }) {
                      Text(item.title)
                        .fontSize(`${BODY_L}fp`)
                        .fontWeight(FontWeight.Medium)
                        .fontColor(this.fontColorWithTheme)
                        // .layoutWeight(1)
                      Text(item.subTitle)
                        .fontSize(`${BODY_M}fp`)
                        .fontWeight(FontWeight.Medium)
                        .fontColor($r('sys.color.font_secondary'))
                        // .layoutWeight(1)
                    }.layoutWeight(1)
                    Radio({ value: 'item.title', group: 'radioGroup' })
                      .size({ width: CHECKBOX_CONTAINER_LENGTH, height: CHECKBOX_CONTAINER_LENGTH })
                      .checked(this.selectedIndex === index)
                      .hitTestBehavior(HitTestMode.None)
                      .id(String(index))
                      .focusable(false)
                      .accessibilityLevel('no')
                      .onFocus(() => {
                        this.isFocus = true;
                        this.currentFocusIndex = index;
                        if (index === FIRST_ITEM_INDEX) {
                          this.contentScroller.scrollEdge(Edge.Top);
                        } else if (index === this.radioContent.length - 1) {
                          this.contentScroller.scrollEdge(Edge.Bottom);
                        }
                      })
                      .onSizeChange((oldValue: SizeOptions, newValue: SizeOptions) => {
                        this.radioHeight = Number(newValue.height)
                      })
                  }.constraintSize({ minHeight: LIST_MIN_HEIGHT }).clip(false)
                  .padding({ top: $r('sys.float.padding_level4'), bottom: $r('sys.float.padding_level4') })
                }
                .type(ButtonType.Normal)
                .borderRadius($r('sys.float.corner_radius_level8'))
                .buttonStyle(ButtonStyleMode.TEXTUAL)
                .paddingStyle()

                if (index < this.radioContent.length - 1) {
                  Divider().color(this.dividerColorWithTheme).paddingStyle();
                }
              }
              .borderRadius($r('sys.float.corner_radius_level8'))
              .focusBox({
                margin: { value: -2, unit: LengthUnit.VP }
              })
              .accessibilityText(getAccessibilityText(item.title, this.selectedIndex === index))
              .onClick(() => {
                this.selectedIndex = index;
                item.action && item.action();
                this.controller?.close();
              })
            }
            .paddingStyle()
            .onSizeChange((oldValue: SizeOptions, newValue: SizeOptions) => {
              this.itemHeight = Number(newValue.height)
            })
          })
        }.width('100%').clip(false)
        .onFocus(() => {
          if (!this.contentScroller.isAtEnd()) {
            this.contentScroller.scrollEdge(Edge.Top);
            focusControl.requestFocus(String(FIRST_ITEM_INDEX));
          }
        })
        .defaultFocus(this.buttons?.length === 0 ? true : false)
      }
    }.scrollBar(BarState.Auto)
    .nestedScroll({ scrollForward: NestedScrollMode.PARALLEL, scrollBackward: NestedScrollMode.PARALLEL })
    .onDidScroll((xOffset: number, yOffset: number) => {
      let scrollHeight: number = (this.itemHeight - this.radioHeight) / 2
      if (this.isFocus) {
        if (this.currentFocusIndex === this.radioContent.length - 1) {
          this.contentScroller.scrollEdge(Edge.Bottom);
          this.currentFocusIndex = -1;
        } else if (this.currentFocusIndex === FIRST_ITEM_INDEX) {
          this.contentScroller.scrollEdge(Edge.Top);
          this.currentFocusIndex = -1;
        } else {
          if (yOffset > 0) {
            this.contentScroller.scrollBy(0, scrollHeight)
          } else if (yOffset < 0) {
            this.contentScroller.scrollBy(0, 0 - scrollHeight)
          }
        }
        this.isFocus = false;
      }
    })
  }

  build() {
    CustomDialogContentComponent({
      controller: this.controller,
      primaryTitle: this.title,
      contentBuilder: () => {
        this.contentBuilder();
      },
      buttons: this.buttons,
      contentAreaPadding: this.contentPadding,
      theme: this.theme,
      themeColorMode: this.themeColorMode,
      fontSizeScale: this.fontSizeScale,
      minContentHeight: this.minContentHeight,
    }).constraintSize({ maxHeight: '100%' });
  }

  aboutToAppear(): void {
    this.fontColorWithTheme = this.theme?.colors?.fontPrimary ?
    this.theme.colors.fontPrimary : $r('sys.color.font_primary');
    this.dividerColorWithTheme = this.theme?.colors?.compDivider ?
    this.theme.colors.compDivider : $r('sys.color.comp_divider');
    this.initContentPadding();
    this.initButtons();
  }

  private initContentPadding(): void {
    this.contentPadding = {
      left: $r('sys.float.padding_level0'),
      right: $r('sys.float.padding_level0')
    }

    if (!this.title && !this.confirm) {
      this.contentPadding = {
        top: $r('sys.float.padding_level12'),
        bottom: $r('sys.float.padding_level12')
      }
      return;
    }

    if (!this.title) {
      this.contentPadding = {
        top: $r('sys.float.padding_level12')
      }
    } else if (!this.confirm) {
      this.contentPadding = {
        bottom: $r('sys.float.padding_level12')
      }
    }
  }

  private initButtons(): void {
    this.buttons = [];
    if (this.confirm) {
      this.buttons.push(this.confirm);
    }
  }
}

class CustomDialogControllerExtend extends CustomDialogController {
  public arg_: CustomDialogControllerOptions;

  constructor(value: CustomDialogControllerOptions) {
    super(value);
    this.arg_ = value;
  }
}

@Component
struct CustomDialogLayout {
  @Builder
  doNothingBuilder(): void {
  };

  @Link titleHeight: number;
  @Link buttonHeight: number;
  @Link titleMinHeight: Length;
  @BuilderParam dialogBuilder: () => void = this.doNothingBuilder;
  titleIndex: number = 0;
  contentIndex: number = 1;
  buttonIndex: number = 2;

  onPlaceChildren(selfLayoutInfo: GeometryInfo, children: Array<Layoutable>,
    constraint: ConstraintSizeOptions) {
    let currentX: number = 0;
    let currentY: number = 0;
    for (let index = 0; index < children.length; index++) {
      let child = children[index];
      child.layout({ x: currentX, y: currentY });
      currentY += child.measureResult.height;
    }
  }

  onMeasureSize(selfLayoutInfo: GeometryInfo, children: Array<Measurable>,
    constraint: ConstraintSizeOptions): SizeResult {
    let sizeResult: SizeResult = { width: Number(constraint.maxWidth), height: 0 };
    let childrenSize: number = 3;
    if (children.length < childrenSize) {
      return sizeResult;
    }
    let height: number = 0;
    let titleChild: Measurable = children[this.titleIndex];
    let titleConstraint: ConstraintSizeOptions = {
      maxWidth: constraint.maxWidth,
      minHeight: this.titleMinHeight,
      maxHeight: constraint.maxHeight
    };
    let titleMeasureResult: MeasureResult = titleChild.measure(titleConstraint);
    this.titleHeight = titleMeasureResult.height;
    height += this.titleHeight;

    let buttonChild: Measurable = children[this.buttonIndex];
    let buttonMeasureResult: MeasureResult = buttonChild.measure(constraint);
    this.buttonHeight = buttonMeasureResult.height;
    height += this.buttonHeight;

    let contentChild: Measurable = children[this.contentIndex];
    let contentConstraint: ConstraintSizeOptions = {
      maxWidth: constraint.maxWidth,
      maxHeight: Number(constraint.maxHeight) - height
    };

    let contentMeasureResult: MeasureResult = contentChild.measure(contentConstraint);
    height += contentMeasureResult.height;
    sizeResult.height = height;
    return sizeResult;
  }

  build() {
    this.dialogBuilder();
  }
}

@Component
struct CustomDialogContentComponent {
  controller?: CustomDialogController;
  primaryTitle?: ResourceStr;
  secondaryTitle?: ResourceStr;
  localizedContentAreaPadding?: LocalizedPadding;
  @BuilderParam contentBuilder: () => void = this.defaultContentBuilder;
  buttons?: ButtonOptions[];
  contentAreaPadding?: Padding;
  keyIndex: number = 0;
  theme?: Theme | CustomTheme = new CustomThemeImpl({});
  themeColorMode?: ThemeColorMode = ThemeColorMode.SYSTEM;
  @Link minContentHeight: number;

  @Builder
  defaultContentBuilder(): void {
  }

  @State titleHeight: number = 0;
  @State buttonHeight: number = 0;
  @State contentMaxHeight: Length = '100%';
  @Link fontSizeScale: number;
  @State customStyle: boolean | undefined = undefined;
  @State buttonMaxFontSize: Length = `${BODY_L}fp`;
  @State buttonMinFontSize: Length = 9;
  @State primaryTitleMaxFontSize: Length = `${TITLE_S}fp`;
  @State primaryTitleMinFontSize: Length = `${BODY_L}fp`;
  @State secondaryTitleMaxFontSize: Length = `${SUBTITLE_S}fp`;
  @State secondaryTitleMinFontSize: Length = `${BODY_S}fp`;
  @State primaryTitleFontColorWithTheme: ResourceColor = $r('sys.color.font_primary');
  @State secondaryTitleFontColorWithTheme: ResourceColor = $r('sys.color.font_secondary');
  @State titleTextAlign: TextAlign = TextAlign.Center;
  @State isButtonVertical: boolean = false;
  @State titleMinHeight: Length = 0;
  isFollowingSystemFontScale: boolean = false;
  appMaxFontScale: number = 3.2;
  titleIndex: number = 0;
  contentIndex: number = 1;
  buttonIndex: number = 2;

  build() {
    WithTheme({ theme: this.theme, colorMode: this.themeColorMode }) {
      Scroll() {
        Column() {
          CustomDialogLayout({
            buttonHeight: this.buttonHeight,
            titleHeight: this.titleHeight,
            titleMinHeight: this.titleMinHeight
          }) {
            ForEach([this.titleIndex, this.contentIndex, this.buttonIndex], (index: number) => {
              if (index === this.titleIndex) {
                WithTheme({ theme: this.theme, colorMode: this.themeColorMode }) {
                  this.titleBuilder();
                }
              } else if (index === this.contentIndex) {
                Column() {
                  WithTheme({ theme: this.theme, colorMode: this.themeColorMode }) {
                    this.contentBuilder();
                  }
                }.padding(this.getContentPadding())
              } else {
                WithTheme({ theme: this.theme, colorMode: this.themeColorMode }) {
                  this.ButtonBuilder();
                }
              }
            });
          }
        }
        .constraintSize({ maxHeight: this.contentMaxHeight })
        .backgroundBlurStyle(this.customStyle ? BlurStyle.Thick : BlurStyle.NONE)
        .borderRadius(this.customStyle ? $r('sys.float.ohos_id_corner_radius_dialog') : 0)
        .margin(this.customStyle ? {
          start: LengthMetrics.resource($r('sys.float.ohos_id_dialog_margin_start')),
          end: LengthMetrics.resource($r('sys.float.ohos_id_dialog_margin_end')),
          bottom: LengthMetrics.resource($r('sys.float.ohos_id_dialog_margin_bottom')),
        } : { left: 0, right: 0, bottom: 0 })
        .backgroundColor(this.customStyle ? $r('sys.color.ohos_id_color_dialog_bg') : Color.Transparent)
      }
      .backgroundColor(this.themeColorMode === ThemeColorMode.SYSTEM || undefined ?
      Color.Transparent : $r('sys.color.comp_background_primary'))
    }
  }

  onMeasureSize(selfLayoutInfo: GeometryInfo, children: Array<Measurable>,
    constraint: ConstraintSizeOptions): SizeResult {
    let sizeResult: SizeResult = { width: selfLayoutInfo.width, height: selfLayoutInfo.height };
    let maxWidth: number = Number(constraint.maxWidth);
    let maxHeight: number = Number(constraint.maxHeight);
    this.fontSizeScale = this.updateFontScale();
    this.updateFontSize();
    this.isButtonVertical = this.isVerticalAlignButton(maxWidth - BUTTON_HORIZONTAL_MARGIN * 2);
    this.titleMinHeight = this.getTitleAreaMinHeight();
    let height: number = 0;
    children.forEach((child) => {
      this.contentMaxHeight = '100%';
      let measureResult: MeasureResult = child.measure(constraint);
      if (maxHeight - this.buttonHeight - this.titleHeight < this.minContentHeight) {
        this.contentMaxHeight = MAX_CONTENT_HEIGHT;
        measureResult = child.measure(constraint);
      }
      height += measureResult.height;
    });
    sizeResult.height = height;
    sizeResult.width = maxWidth;
    return sizeResult;
  }

  aboutToAppear(): void {
    let uiContext: UIContext = this.getUIContext();
    this.isFollowingSystemFontScale = uiContext.isFollowingSystemFontScale();
    this.appMaxFontScale = uiContext.getMaxFontScale();
    this.fontSizeScale = this.updateFontScale();
    if (this.controller && this.customStyle === undefined) {
      let customController: CustomDialogControllerExtend = this.controller as CustomDialogControllerExtend;
      if (customController.arg_ && customController.arg_.customStyle && customController.arg_.customStyle === true) {
        this.customStyle = true;
      }
    }
    if (this.customStyle === undefined) {
      this.customStyle = false;
    }
    this.primaryTitleFontColorWithTheme = this.theme?.colors?.fontPrimary ?
    this.theme.colors.fontPrimary : $r('sys.color.font_primary');
    this.secondaryTitleFontColorWithTheme = this.theme?.colors?.fontSecondary ?
    this.theme.colors.fontSecondary : $r('sys.color.font_secondary');
    this.initTitleTextAlign();
  }

  private updateFontSize(): void {
    if (this.fontSizeScale > MAX_FONT_SCALE) {
      this.buttonMaxFontSize = BODY_L * MAX_FONT_SCALE + 'vp';
      this.buttonMinFontSize = BUTTON_MIN_FONT_SIZE * MAX_FONT_SCALE + 'vp';
    } else {
      this.buttonMaxFontSize = BODY_L + 'fp';
      this.buttonMinFontSize = BUTTON_MIN_FONT_SIZE + 'fp';
    }
  }

  updateFontScale(): number {
    try {
      let uiContext: UIContext = this.getUIContext();
      let systemFontScale = (uiContext.getHostContext() as common.UIAbilityContext)?.config.fontSizeScale ?? 1;
      if (!this.isFollowingSystemFontScale) {
        return 1;
      }
      return Math.min(systemFontScale, this.appMaxFontScale);
    } catch (exception) {
      let code: number = (exception as BusinessError).code;
      let message: string = (exception as BusinessError).message;
      hilog.error(0x3900, 'Ace', `Faild to init fontsizescale info,cause, code: ${code}, message: ${message}`);
      return 1;
    }
  }

  /**
   * get dialog content padding
   *
   * @returns content padding
   */
  private getContentPadding(): Padding | LocalizedPadding {
    if (this.localizedContentAreaPadding) {
      return this.localizedContentAreaPadding;
    }
    if (this.contentAreaPadding) {
      return this.contentAreaPadding;
    }

    if ((this.primaryTitle || this.secondaryTitle) && this.buttons && this.buttons.length > 0) {
      return {
        top: 0,
        right: $r('sys.float.alert_content_default_padding'),
        bottom: 0,
        left: $r('sys.float.alert_content_default_padding'),
      };
    } else if (this.primaryTitle || this.secondaryTitle) {
      return {
        top: 0,
        right: $r('sys.float.alert_content_default_padding'),
        bottom: $r('sys.float.alert_content_default_padding'),
        left: $r('sys.float.alert_content_default_padding'),
      };
    } else if (this.buttons && this.buttons.length > 0) {
      return {
        top: $r('sys.float.alert_content_default_padding'),
        right: $r('sys.float.alert_content_default_padding'),
        bottom: 0,
        left: $r('sys.float.alert_content_default_padding'),
      };
    } else {
      return {
        top: $r('sys.float.alert_content_default_padding'),
        right: $r('sys.float.alert_content_default_padding'),
        bottom: $r('sys.float.alert_content_default_padding'),
        left: $r('sys.float.alert_content_default_padding'),
      };
    }
  }

  @Builder
  titleBuilder() {
    Column() {
      Row() {
        Text(this.primaryTitle)
          .fontWeight(FontWeight.Bold)
          .fontColor(this.primaryTitleFontColorWithTheme)
          .textAlign(this.titleTextAlign)
          .maxFontSize(this.primaryTitleMaxFontSize)
          .minFontSize(this.primaryTitleMinFontSize)
          .maxFontScale(Math.min(this.appMaxFontScale, MAX_FONT_SCALE))
          .maxLines(TITLE_MAX_LINES)
          .heightAdaptivePolicy(TextHeightAdaptivePolicy.MIN_FONT_SIZE_FIRST)
          .textOverflow({ overflow: TextOverflow.Ellipsis })
          .width('100%')
      }
      .width('100%')

      if (this.primaryTitle && this.secondaryTitle) {
        Row() {
        }.height($r('sys.float.padding_level1'))
      }

      Row() {
        Text(this.secondaryTitle)
          .fontWeight(FontWeight.Regular)
          .fontColor(this.secondaryTitleFontColorWithTheme)
          .textAlign(this.titleTextAlign)
          .maxFontSize(this.secondaryTitleMaxFontSize)
          .minFontSize(this.secondaryTitleMinFontSize)
          .maxFontScale(Math.min(this.appMaxFontScale, MAX_FONT_SCALE))
          .maxLines(TITLE_MAX_LINES)
          .heightAdaptivePolicy(TextHeightAdaptivePolicy.MIN_FONT_SIZE_FIRST)
          .textOverflow({ overflow: TextOverflow.Ellipsis })
          .width('100%')
      }
      .width('100%')
    }
    .justifyContent(FlexAlign.Center)
    .width('100%')
    .padding(this.getTitleAreaPadding())
  }

  /**
   * get title area padding
   *
   * @returns padding
   */
  private getTitleAreaPadding(): Padding {
    if (this.primaryTitle || this.secondaryTitle) {
      return {
        top: $r('sys.float.alert_title_padding_top'),
        right: $r('sys.float.alert_title_padding_right'),
        left: $r('sys.float.alert_title_padding_left'),
        bottom: $r('sys.float.alert_title_padding_bottom'),
      };
    }

    return {
      top: 0,
      right: $r('sys.float.alert_title_padding_right'),
      left: $r('sys.float.alert_title_padding_left'),
      bottom: 0,
    };
  }

  /**
   * get tile TextAlign
   * @returns TextAlign
   */
  private initTitleTextAlign(): void {
    let textAlign: number = ALERT_TITLE_ALIGNMENT;
    if (textAlign === TextAlign.Start) {
      this.titleTextAlign = TextAlign.Start;
    } else if (textAlign === TextAlign.Center) {
      this.titleTextAlign = TextAlign.Center;
    } else if (textAlign === TextAlign.End) {
      this.titleTextAlign = TextAlign.End;
    } else if (textAlign === TextAlign.JUSTIFY) {
      this.titleTextAlign = TextAlign.JUSTIFY;
    } else {
      this.titleTextAlign = TextAlign.Center;
    }
  }

  /**
   * get title area min height
   *
   * @returns min height
   */
  private getTitleAreaMinHeight(): ResourceStr | number {
    if (this.secondaryTitle) {
      return $r('sys.float.alert_title_secondary_height');
    } else if (this.primaryTitle) {
      return $r('sys.float.alert_title_primary_height');
    } else {
      return 0;
    }
  }

  @Builder
  ButtonBuilder(): void {
    Column() {
      if (this.buttons && this.buttons.length > 0) {
        if (this.isButtonVertical) {
          this.buildVerticalAlignButtons();
        } else {
          this.buildHorizontalAlignButtons();
        }
      }
    }
    .width('100%')
    .padding(this.getOperationAreaPadding());
  }

  /**
   * get operation area padding
   *
   * @returns padding
   */
  private getOperationAreaPadding(): Padding {
    if (this.isButtonVertical) {
      return {
        top: $r('sys.float.alert_button_top_padding'),
        right: $r('sys.float.alert_right_padding_vertical'),
        left: $r('sys.float.alert_left_padding_vertical'),
        bottom: $r('sys.float.alert_button_bottom_padding_vertical'),
      };
    }

    return {
      top: $r('sys.float.alert_button_top_padding'),
      right: $r('sys.float.alert_right_padding_horizontal'),
      left: $r('sys.float.alert_left_padding_horizontal'),
      bottom: $r('sys.float.alert_button_bottom_padding_horizontal'),
    };
  }

  @Builder
  buildSingleButton(buttonOptions: ButtonOptions): void {
    if (this.isNewPropertiesHighPriority(buttonOptions)) {
      Button(buttonOptions.value)
        .setButtonProperties(buttonOptions, this.buttons, this.controller)
        .role(buttonOptions.role ?? ButtonRole.NORMAL)
        .key(`advanced_dialog_button_${this.keyIndex++}`)
        .labelStyle({ maxLines: 1, maxFontSize: this.buttonMaxFontSize, minFontSize: this.buttonMinFontSize })
    } else if (buttonOptions.background !== undefined && buttonOptions.fontColor !== undefined) {
      Button(buttonOptions.value)
        .setButtonProperties(buttonOptions, this.buttons, this.controller)
        .backgroundColor(buttonOptions.background)
        .fontColor(buttonOptions.fontColor)
        .key(`advanced_dialog_button_${this.keyIndex++}`)
        .labelStyle({ maxLines: 1, maxFontSize: this.buttonMaxFontSize, minFontSize: this.buttonMinFontSize })
    } else if (buttonOptions.background !== undefined) {
      Button(buttonOptions.value)
        .setButtonProperties(buttonOptions, this.buttons, this.controller)
        .backgroundColor(buttonOptions.background)
        .key(`advanced_dialog_button_${this.keyIndex++}`)
        .labelStyle({ maxLines: 1, maxFontSize: this.buttonMaxFontSize, minFontSize: this.buttonMinFontSize })
    } else {
      Button(buttonOptions.value)
        .setButtonProperties(buttonOptions, this.buttons, this.controller)
        .fontColor(buttonOptions.fontColor)
        .key(`advanced_dialog_button_${this.keyIndex++}`)
        .labelStyle({ maxLines: 1, maxFontSize: this.buttonMaxFontSize, minFontSize: this.buttonMinFontSize })
    }
  }

  @Builder
  buildHorizontalAlignButtons(): void {
    if (this.buttons && this.buttons.length > 0) {
      Row() {
        this.buildSingleButton(this.buttons[0]);
        if (this.buttons.length === HORIZON_BUTTON_MAX_COUNT) {
          Row() {
            Divider()
              .width($r('sys.float.alert_divider_width'))
              .height($r('sys.float.alert_divider_height'))
              .color(this.getDividerColor())
              .vertical(true)
          }
          .width(BUTTON_HORIZONTAL_SPACE * 2)
          .justifyContent(FlexAlign.Center)

          this.buildSingleButton(this.buttons[HORIZON_BUTTON_MAX_COUNT - 1]);
        }
      }
    }
  }

  @Builder
  buildVerticalAlignButtons(): void {
    if (this.buttons) {
      Column() {
        ForEach(this.buttons.slice(0, VERTICAL_BUTTON_MAX_COUNT), (item: ButtonOptions, index: number) => {
          this.buildButtonWithDivider(this.buttons?.length === HORIZON_BUTTON_MAX_COUNT ?
            HORIZON_BUTTON_MAX_COUNT - index - 1 : index);
        }, (item: ButtonOptions) => item.value.toString());
      }
    }
  }

  /**
   * get divider color
   *
   * @returns divider color
   */
  private getDividerColor(): ResourceColor {
    if (!this.buttons || this.buttons.length === 0 || !DIALOG_DIVIDER_SHOW) {
      return Color.Transparent;
    }

    if (this.buttons[0].buttonStyle === ButtonStyleMode.TEXTUAL || this.buttons[0].buttonStyle === undefined) {
      if (this.buttons[HORIZON_BUTTON_MAX_COUNT - 1].buttonStyle === ButtonStyleMode.TEXTUAL ||
        this.buttons[HORIZON_BUTTON_MAX_COUNT - 1].buttonStyle === undefined) {
        return $r('sys.color.alert_divider_color');
      }
    }
    return Color.Transparent;
  }

  /**
   * is button buttonStyle and role properties high priority
   *
   * @param buttonOptions button properties
   * @returns check result
   */
  private isNewPropertiesHighPriority(buttonOptions: ButtonOptions): boolean {
    if (buttonOptions.role === ButtonRole.ERROR) {
      return true;
    }
    if (buttonOptions.buttonStyle !== undefined &&
      buttonOptions.buttonStyle !== ALERT_BUTTON_STYLE) {
      return true;
    }
    if (buttonOptions.background === undefined && buttonOptions.fontColor === undefined) {
      return true;
    }
    return false;
  }

  @Builder
  buildButtonWithDivider(index: number): void {
    if (this.buttons && this.buttons[index]) {
      Row() {
        this.buildSingleButton(this.buttons[index]);
      }

      if ((this.buttons.length === HORIZON_BUTTON_MAX_COUNT ? HORIZON_BUTTON_MAX_COUNT - index - 1 : index) <
        Math.min(this.buttons.length, VERTICAL_BUTTON_MAX_COUNT) - 1) {
        Row() {
        }
        .height($r('sys.float.alert_button_vertical_space'))
      }
    }
  }

  private isVerticalAlignButton(width: number): boolean {
    if (this.buttons) {
      if (this.buttons.length === 1) {
        return false;
      }
      if (this.buttons.length !== HORIZON_BUTTON_MAX_COUNT) {
        return true;
      }
      let isVertical: boolean = false;
      let maxButtonTextSize = vp2px(width / HORIZON_BUTTON_MAX_COUNT - BUTTON_HORIZONTAL_MARGIN -
        BUTTON_HORIZONTAL_SPACE - 2 * BUTTON_HORIZONTAL_PADDING);
      this.buttons.forEach((button) => {
        let contentSize: SizeOptions = measure.measureTextSize({
          textContent: button.value,
          fontSize: this.buttonMaxFontSize
        });
        if (Number(contentSize.width) > maxButtonTextSize) {
          isVertical = true;
        }
      });
      return isVertical;
    }
    return false;
  }
}

@Extend(Button)
function setButtonProperties(buttonOptions: ButtonOptions, buttonList?: ButtonOptions[],
  controller?: CustomDialogController) {
  .onClick(() => {
    if (buttonOptions.action) {
      buttonOptions.action();
    }
    controller?.close();
  })
  .defaultFocus(buttonOptions.defaultFocus ? true : isHasDefaultFocus(buttonList) ? false : true)
  .buttonStyle(buttonOptions.buttonStyle ?? ALERT_BUTTON_STYLE)
  .layoutWeight(BUTTON_LAYOUT_WEIGHT)
  .type(ButtonType.ROUNDED_RECTANGLE)
}

/**
 * is button list has default focus
 *
 * @param buttonList button list
 * @returns boolean
 */
function isHasDefaultFocus(buttonList?: ButtonOptions[]): boolean {
  try {
    let isHasDefaultFocus: boolean = false;
    buttonList?.forEach((button) => {
      if (button.defaultFocus) {
        isHasDefaultFocus = true;
      }
    })
    return isHasDefaultFocus;
  } catch (error) {
    let code: number = (error as BusinessError).code;
    let message: string = (error as BusinessError).message;
    hilog.error(0x3900, 'Ace', `get defaultFocus exist error, code: ${code}, message: ${message}`);
    return false;
  }
}

/**
 * get resource size
 *
 * @param resourceId resource id
 * @param defaultValue default value
 * @returns resource size
 */
function getNumberByResourceId(resourceId: number, defaultValue: number, allowZero?: boolean): number {
  try {
    let sourceValue: number = resourceManager.getSystemResourceManager().getNumber(resourceId);
    if (sourceValue > 0 || allowZero) {
      return sourceValue;
    } else {
      return defaultValue;
    }
  } catch (error) {
    let code: number = (error as BusinessError).code;
    let message: string = (error as BusinessError).message;
    hilog.error(0x3900, 'Ace', `CustomContentDialog getNumberByResourceId error, code: ${code}, message: ${message}`);
    return defaultValue;
  }
}

/**
 * get enum number
 *
 * @param resourceId resource id
 * @param defaultValue default value
 * @returns number
 */
function getEnumNumberByResourceId(resourceId: number, defaultValue: number): number {
  try {
    let sourceValue: number = getContext().resourceManager.getNumber(resourceId);
    if (sourceValue > 0) {
      return sourceValue;
    } else {
      return defaultValue;
    }
  } catch (error) {
    let code: number = (error as BusinessError).code;
    let message: string = (error as BusinessError).message;
    hilog.error(0x3900, 'Ace', `getEnumNumberByResourceId error, code: ${code}, message: ${message}`);
    return defaultValue;
  }
}

/**
 * 获取无障碍文本
 *
 * @param resource 资源
 * @param selected select state
 * @returns string
 */
function getAccessibilityText(resource: ResourceStr, selected: boolean): string {
  try {
    let selectText: string = getContext().resourceManager.getStringSync(125833934);
    let resourceString: string = '';
    if (typeof resource === 'string') {
      resourceString = resource;
    } else {
      resourceString = getContext().resourceManager.getStringSync(resource);
    }
    return selected ? `${selectText},${resourceString}` : resourceString;
  } catch (error) {
    let code: number = (error as BusinessError).code;
    let message: string = (error as BusinessError).message;
    hilog.error(0x3900, 'Ace', `getAccessibilityText error, code: ${code}, message: ${message}`);
    return '';
  }
}

export declare interface PopoverOptions extends CustomPopupOptions {}