# 面板组织整理：底栏与全局面板

## 目标与范围

在按钮回调化的基础上，整理 `BottomPanel` 和 `GlobalPanel`，让构造代码直接呈现界面组成，避免只为显示一段文字而增加专用控件类型和文件。

基线：`refactor/ui-callbacks` 的 `da18339`。本轮不继续整理其他面板，不改变控件所有权、输入归属、保存格式或框架更新/渲染流程。

## 具体变更

| 原实现 | 新实现 |
| --- | --- |
| `BottomControls.h/.cpp` 中的 `FrameRateLabel`、`TimeLabel`、`MousePositionLabel` | 三个普通 `Label`，由 `BottomPanel::Update()` 集中设置文字 |
| `BottomPanel::controls` | `layout`，直接排列三个标签，不使用与成员同名的临时容器 |
| `GlobalControls.h/.cpp` 中的 `GlobalDetailsLabel` | 普通 `RichLabel`，使用 `GlobalPanel::GetDetailsText()` 提供文字 |
| 全局面板构造中重复填充、清空临时容器 | `BuildSaveRow()`、`BuildLoadRow()` 各自建立一行，构造函数只组合主要区域 |
| `saveControls`、`loadControls`、`controls` | `saveRow`、`loadRow`、`layout`，名称直接说明布局角色 |

删除两组 `*Controls` 文件及项目引用。仍保留两个 Panel 类：它们负责各自界面的组成、数据展示与动作绑定，不以取消 Panel 为目标。

整理后的结构：

```text
BottomPanel
  layout（横向）
    frameRateLabel
    timeLabel
    mousePositionLabel

GlobalPanel
  layout（纵向）
    saveRow：savePathTextBox + saveButton
    loadRow：loadPathTextBox + loadButton
    separator
    detailsLabel
```

## 文字刷新规则

底栏的三个标签只读应用状态，尺寸固定，标签之间没有修改应用状态的动作。因此在 `BottomPanel::Update()` 开头集中设置文字，再执行原 `Panel::Update()`，保持本帧显示内容和布局不变。

全局面板不能照搬这个做法：保存、加载按钮先更新，详情标签后更新。将详情刷新提前到面板更新开头，会让加载结果晚一帧显示。

因此仅对 `RichLabel` 增加可选的 `textProvider`，类型为 `std::function<std::wstring(App&)>`。默认 `Sync()` 在原有的测量和绘制之前读取该回调；未设置时不修改文字。现有派生类覆盖 `Sync()` 的行为保持不变，不额外执行第二套回调。

全局详情使用不捕获对象的静态格式化函数，仍在原控件遍历位置读取数据。面板隐藏时不读取详情。原文字、语言键、宽高策略、滚动计算时点和保存/加载行为不变。

## 验证与状态

- 修改前已建立本地面板检查基线，覆盖布局、双语文字、同帧按钮后刷新、隐藏时不刷新，共 20 项通过。
- 修改后 x64、Win32 Debug 构建通过。
- 面板检查 25 项通过，包含未设置文字回调、测量前刷新和既有派生类行为；测试画面的像素数据与修改前完全一致。
- 既有按钮与业务回调检查 9,599 项、配置与脚本等回归检查 1,299 项、真实输入帧创建工具检查 58 项均通过。
- 本地检查程序保存在已忽略的 `x64/`，不纳入提交。
- 代码和本文留在工作区，等待用户检查；本轮不自动提交。

## 后续

样例整理方向已获用户认可，并扩展到全部面板。后续范围、参数行缺陷修复及验证结果见 `docs/panel-organization-plan.md`；以上保留为样例阶段记录。
