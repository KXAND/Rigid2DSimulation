# 全部面板组织整理

## 范围与原则

延续已确认的 `docs/panel-organization-sample-plan.md`，覆盖应用层全部十个面板。保留 Panel 作为界面组成与动作绑定的入口；简单页面直接排列控件，复杂页面按有意义的区域建立布局，不为每个控件抽一个函数。

上一版只消除了纯文字展示子类，仍将编辑器和动态列表放在专用类中。用户进一步明确：这些仅服务于单个面板的绑定与管理职责应归回 Panel，而不只是改文件名。本文以下方案取代上一版该项边界。

Panel 负责布局、数据绑定、业务校验、行集合及选择切换；通用控件负责键鼠输入、编辑状态、焦点和绘制。仅为数据绑定不再定义派生控件；行使用 Panel 内部的纯数据分组，不是新的管理类。

本轮不调整物理、DSL、场景生命周期或全局输入/渲染调度。框架只增加必要的通用绑定回调、可选 Enter 提交和内容更新扩展点，保持原有控件尺寸、顺序、语言键及业务动作。代码与本文一起留在工作区供检查，不自动提交。

## 各面板方案

| 面板 | 整理后的组成与实现 |
| --- | --- |
| BottomPanel | 已完成样例：三个普通 Label，面板集中刷新文字 |
| GlobalPanel | 已完成样例：保存行、加载行、分隔线、详情；详情在按钮后读取 |
| AboutPanel | 直接持有 LocalizedRichLabel 作为内容，不再套单元素列表 |
| LanguagePanel | 中文按钮、英文按钮、纵向 layout；两组动作绑定分段书写 |
| CommandPanel | 普通 RichTextBox、执行按钮、纵向 layout；面板绑定编辑完成时上传草稿 |
| ParameterPanel | 自己持有参数行集合，SyncParameterRows 负责补充、复用及清理；每项由名称行、数值行、分隔线组成 |
| BodyPanel | 自己管理选择变化、连接行及编辑区；使用普通脚本框、复选框和标签绑定当前选择 |
| ConnectionPanel | 自己管理选择变化和编辑区；使用普通脚本框及详情标签 |
| CreationPanel | 普通 LocalizedTextBox 通过回调读写尺寸；ApplySize 负责校验，输入框负责 Enter 提交 |
| MenuPanel | 构建模拟按钮、面板入口按钮、模式按钮，最后按原显示顺序排列；四个动态标题按钮使用普通 Button |

根布局统一命名为 `layout`。参数项内部使用 `nameRow`、`valueRow`；物体和连接面板使用 `editorLayout` 表达可隐藏的编辑区域。普通 ControlList 只持有布局，不再派生场景专用列表管理类。

## 专用控件文件

| 原文件组 | 新文件组或去向 |
| --- | --- |
| MenuButtons | 删除；四个只更新标题的类归回 MenuPanel 的文字提供函数 |
| BodyControls / BodyEditorControls | 删除；绑定、连接行管理、选择同步和详情全部归回 BodyPanel |
| ConnectionControls / ConnectionEditorControls | 删除；绑定、选择同步和详情全部归回 ConnectionPanel |
| CommandControls / CommandScriptEditor | 删除；草稿读取与提交直接绑定在 CommandPanel |
| CreationControls / CreationSizeTextBox | 删除；尺寸校验归回 CreationPanel，Enter 提交成为 TextBox 的可选能力 |
| ParameterControls / ParameterControlList | 删除；行集合、绑定和列表同步全部归回 ParameterPanel |

以上文件组均指 `.h/.cpp`，同步项目与筛选器引用。最终每个面板目录只保留对应的 Panel 头文件和实现文件。`ParameterRow`、`ConnectionRow` 是 Panel 内声明的控件指针分组，无独立更新方法和业务行为。

## 通用控件接口

| 控件 | 回调或扩展点 |
| --- | --- |
| TextBox、RichTextBox | valueProvider 读取显示值；onEditing 在原 Upload 位置逐编辑帧回写；onEditFinished 在原 OnDone 位置提交 |
| TextBox | submitOnEnter 默认关闭，仅尺寸输入框开启；保持原绘制后提交并 Discard 的顺序 |
| CheckBox | checkedProvider 读取勾选状态；onCheckedChanged 在原 Upload 位置回写切换结果 |
| HorizontalDragBar | valueProvider 读取数值；onDragging 在原 Upload 位置连续回写，包括释放帧 |
| FloatPanel | 受保护的 UpdateContent 在原 contentControl->Update 位置执行，默认仍遍历内容；ParameterPanel、BodyPanel、ConnectionPanel 在此同步各自内容 |

不新增通用 onUpdate 回调，不给 Panel 添加逐键监听，不复制 TextBox 的光标或焦点算法。回调只捕获字段或行索引，运行时读取传入 App 的当前数据，不捕获旧 Body/Connection 或 Panel 自身指针。

## 必须保持的时序

- 文字回调在原控件位置执行，不统一提前到面板更新开头。
- 菜单动态标题在点击处理前读取，所以点击切换后的新标题仍在下一次更新显示；较早按钮对较晚按钮产生的状态影响仍可同帧读取。
- Body、Connection 的详情在应用/删除动作之后读取；选中对象变化和面板关闭时仍走原 Discard 清理编辑状态及输入归属。
- Creation 帮助在工具按钮与尺寸输入之后读取最新请求配置。
- 参数列表先更新，添加按钮后更新；新增项仍在下次列表更新出现。删除仍是标记后由原运行时入口清理，列表缩小时继续 Discard 多余行。
- Label 的默认 Sync 使用可选 textProvider；覆盖 Sync 的既有控件保持原行为。Button 的可选 textProvider 在原点击处理前执行。不引入通用 onUpdate 或第二套调度。
- 列表与选择同步在原内容更新阶段执行：滚动计算、标题栏处理、面板背景绘制之后；隐藏或当帧关闭的面板不执行同步。不能简单放到整个 Update 的开头。
- BodyPanel、ConnectionPanel 的 Discard 先走原子控件清理，再清空缓存选择和 editorLayout。丢弃编辑状态不等于提交草稿。
- Body/Connection 的脚本草稿仍逐编辑帧回写，Command 草稿仍在编辑完成时回写，不统一成一种提交规则。

## 单独批准的缺陷修复

`ParameterControlRow` 构造函数原本声明局部 `vector<Control*> controls`，遮蔽继承成员。最终拼出的名称行、数值行、分隔线未写入实际控件列表，导致参数项不显示。

用户已确认修复；本版由 ParameterPanel 建立每项的普通纵向 layout，并正确填入三个区域，继续保留该修复。这是有意的显示行为修复，不宣称相对分支基线全部画面零变化。

## 验证

修改前已记录全部浮动面板的中英文、无选择/有选择和连续更新画面，以及菜单四种模式、创建工具七种模式，共 87 个画面基线。参数非空的画面应因上述修复发生变化，其余基线应保持一致。

修改后验证构建、既有按钮与脚本/配置回归、真实输入创建工具检查，并补充参数项布局、编辑和删除后的行复用检查。本地检查程序保存在被忽略的 `x64/`。

上一版验证结果：

- x64、Win32 Debug 构建通过。
- 87 个画面基线中 86 个像素摘要一致；唯一差异为获准修复的非空参数列表。
- 新增面板与参数项行为检查 25 项通过，覆盖实际布局、名称/数值/滑块同步、添加时点、删除标记、行复用、编辑状态清理、详情同帧刷新和菜单标题时序。
- 上轮底栏与全局面板检查 25 项、已有按钮与业务回调检查 9,599 项、脚本/配置等回归检查 1,299 项、创建工具真实输入检查 58 项均通过。
- 滑块仍按原有整数像素位置量化，不改变数值精度规则。

本版重新以已修复参数显示的上一版为基线，要求 87 个画面全部保持一致，并复核通用回调、Enter 提交、选择切换、关闭清理、列表增长/缩减与脚本提交时序。

本版验证结果：

- x64、Win32 Debug 构建通过。
- 相对上一版（已恢复参数显示）的 87 个画面像素摘要全部一致。
- 面板与绑定生命周期检查扩充至 55 项并全部通过，包含无变化编辑帧回写、失焦前回写、Discard 不提交、Enter 默认关闭及开启后的单次提交、滑块释放帧、复选框切换、隐藏/当帧关闭跳过内容同步、重新打开后同步。
- 既有按钮与业务回调 9,599 项、脚本/配置回归 1,299 项、创建工具真实输入 58 项、底栏与全局面板 25 项检查全部通过。
- 测试按新 Panel 接口更新；本地测试程序继续留在被忽略的 `x64/`，未纳入版本管理。

状态：按新边界实现并完成验证，用户已确认；本文随对应代码一起提交。
