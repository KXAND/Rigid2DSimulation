# 行为类型 struct → class 迁移

## 1. 目标与边界

基线：5b2c803。用户已确认：承担行为和管理职责的类型使用 class，纯数据及数学值类型保留 struct。本批先实现、验证并供用户检查，确认后与本文一起提交。

本次是类型角色的表达整理，不是封装或架构重设计。保留全部现有 public/private 权限、成员顺序、继承关系、虚函数、构造方式、字段默认值及函数实现。不新增 getter/setter，不改物理公式、调度时序、UI 回调方式、资源生命周期及序列化格式。Creator 的共享端点字段与更新顺序依赖只保留此前的 TODO，不在此解决。

struct 与 class 都能承载行为；不是因为有成员函数就必须转换。判据是类型的主要职责：场景对象、控件、执行服务、资源管理对象使用 class；配置、引用集合、交互记录、数学值和语法数据保留 struct。

## 2. 转换清单

以下路径相对 Rigid2DSimulation/。共 89 个类型；SceneEditor、SceneRenderer、PhysicsWorld 已是 class，不重复转换。

| 文件 | 转为 class 的类型 |
| --- | --- |
| `current/common/SceneCanvas.h` | `SceneCanvas` |
| `current/common/SimulationApp.h` | `SimulationApp` |
| `current/editor/Creator.h` | `Creator` |
| `current/rigid/Body.h` | `Body` |
| `current/rigid/Collision.h` | `Collision` |
| `current/rigid/Connection.h` | `Connection` |
| `current/rigid/Group.h` | `Group` |
| `current/rigid/Shape.h` | `Shape` |
| `current/script/SceneScriptRuntime.h` | `SceneScriptRuntime` |
| `current/storage/SceneStorage.h` | `SceneStorage` |
| `current/ui/aboutPanel/AboutPanel.h` | `AboutPanel` |
| `current/ui/bodyPanel/BodyButtons.h` | `BodyScriptApplicationButton`、`BodyDeletionButton` |
| `current/ui/bodyPanel/BodyControls.h` | `BodyScriptEditor`、`BodyConnectionLabel`、`SelectBodyConnectionCheckBox`、`BodyConnectionControlRow`、`BodyConnectionControlList`、`BodyDetailsLabel`、`BodyEditorControls` |
| `current/ui/bodyPanel/BodyPanel.h` | `BodyPanel` |
| `current/ui/bottomPanel/BottomControls.h` | `FrameRateLabel`、`TimeLabel`、`MousePositionLabel` |
| `current/ui/bottomPanel/BottomPanel.h` | `BottomPanel` |
| `current/ui/commandPanel/CommandButtons.h` | `ScriptExecutionButton` |
| `current/ui/commandPanel/CommandControls.h` | `CommandScriptEditor` |
| `current/ui/commandPanel/CommandPanel.h` | `CommandPanel` |
| `current/ui/connectionPanel/ConnectionButtons.h` | `ConnectionScriptApplicationButton`、`ConnectionDeletionButton` |
| `current/ui/connectionPanel/ConnectionControls.h` | `ConnectionScriptEditor`、`ConnectionEditorControls`、`ConnectionDetailsLabel` |
| `current/ui/connectionPanel/ConnectionPanel.h` | `ConnectionPanel` |
| `current/ui/globalPanel/GlobalButtons.h` | `SceneSaveButton`、`SceneLoadButton` |
| `current/ui/globalPanel/GlobalControls.h` | `GlobalDetailsLabel` |
| `current/ui/globalPanel/GlobalPanel.h` | `GlobalPanel` |
| `current/ui/languagePanel/LanguageButtons.h` | `ChineseLanguageButton`、`EnglishLanguageButton` |
| `current/ui/languagePanel/LanguagePanel.h` | `LanguagePanel` |
| `current/ui/menuPanel/MenuButtons.h` | `SimulationPauseButton`、`SceneResetButton`、`AudioMuteButton`、`InteractionModeButton`、`DisplayModeButton` |
| `current/ui/menuPanel/MenuPanel.h` | `MenuPanel` |
| `current/ui/parameterPanel/ParameterButtons.h` | `ParameterDeletionButton`、`ParameterAdditionButton` |
| `current/ui/parameterPanel/ParameterControls.h` | `ParameterNameTextBox`、`ParameterValueDragBar`、`ParameterValueTextBox`、`ParameterControlRow`、`ParameterControlList` |
| `current/ui/parameterPanel/ParameterPanel.h` | `ParameterPanel` |
| `current/ui/UserInterface.h` | `UserInterface` |
| `framework/app/App.h` | `App` |
| `framework/app/AppItem.h` | `AppInput`、`FrameRate` |
| `framework/app/WaveOutput.h` | `WaveBlock`、`WaveOutput` |
| `framework/control/Button.h` | `Button`、`LocalizedButton`、`ShowFloatPanelButton` |
| `framework/control/CheckBox.h` | `CheckBox`、`LocalizedCheckBox` |
| `framework/control/Control.h` | `Control`、`Delegate` |
| `framework/control/ControlList.h` | `HorizontalControlList`、`VerticalControlList` |
| `framework/control/DragBar.h` | `HorizontalDragBar`、`LocalizedHorizontalDragBar` |
| `framework/control/DropList.h` | `DropList`、`LocalizedDropList` |
| `framework/control/FloatPanel.h` | `FloatPanel`、`LocalizedFloatPanel` |
| `framework/control/Label.h` | `Label`、`LocalizedLabel` |
| `framework/control/Panel.h` | `Panel` |
| `framework/control/RichLabel.h` | `RichLabel`、`LocalizedRichLabel` |
| `framework/control/RichTextBox.h` | `RichTextBox` |
| `framework/control/ScrollBar.h` | `VerticalScrollBar` |
| `framework/control/Separator.h` | `HorizontalSeparator` |
| `framework/control/TextBox.h` | `TextBox`、`LocalizedTextBox` |
| `framework/media/AudioClip.h` | `AudioClip` |
| `framework/media/Font.h` | `Font` |
| `framework/media/Tile.h` | `Tile` |

Shape 承担形状维护与几何查询，Collision 承担碰撞响应，Group 承担分组越界行为，故列入行为类型。AppInput 负责平台输入跟踪与同步，FrameRate 负责跨帧计时。AudioClip、Font、Tile、WaveBlock、WaveOutput 管理播放或资源操作，均列入本批；不借此改变其现有公开字段。

## 3. 保留清单

共 26 个类型，保留原 struct 定义。

| 文件 | 保留 struct 的类型 |
| --- | --- |
| `current/common/SceneParameter.h` | `SceneParameter` |
| `current/editor/SceneEditor.h` | `BodyDrag` |
| `current/render/SceneRenderer.h` | `SceneRenderContext` |
| `current/rigid/Aabb.h` | `Aabb` |
| `current/rigid/Body.h` | `BodyProperties`、`BodyAppearance`、`BodyScriptData` |
| `current/rigid/Connection.h` | `ConnectionAppearance` |
| `current/script/SceneScriptRuntime.h` | `SceneScriptBindings` |
| `framework/app/AppItem.h` | `InputOwners` |
| `framework/basic/DVector2.h` | `DVector2`、`DRect` |
| `framework/basic/ValuePtr.h` | `ValuePtr` |
| `framework/basic/Vector2.h` | `Vector2`、`Rect`、`Matrix2` |
| `framework/basic/Vector3.h` | `Vector3`、`Matrix3`、`Camera` |
| `framework/media/Tile.h` | `Color` |
| `framework/parse/BracketGroup.h` | `BracketGroup` |
| `framework/parse/ScriptExpression.h` | `ScriptExpression`、`PreExpression` |
| `framework/parse/ScriptStatement.h` | `ScriptStatement` |
| `framework/parse/Token.h` | `Token` |
| `framework/parse/Value.h` | `Value` |

- BodyAppearance 的深度计算只是数据辅助方法，不因此改为 class。
- InputOwners 是共享输入归属记录；Reset、Remove、ReleaseKeyboard 是记录维护，不负责平台输入采集或事件调度。
- Camera 当前是坐标/投影计算值，不是管理场景的相机对象；与其他数学类型一起保留。
- ValuePtr 是值语义包装，DSL 的 Token、Value、表达式和语句是数据表示。构造、复制、运算或静态工厂本身不是转换依据。

## 4. 修改规则

1. 按上述白名单替换定义中的 struct，并同步前置声明、友元和其他类型关键字引用，避免声明与定义不一致。
2. 原隐式 public 成员区显式补 public:；已有 private/public 分区保持原样。Shape 首个分区已经是 private，不插入无用 public。
3. 原隐式 public 继承显式写为 public 继承，不改变可转换关系。
4. 不修改函数体、数据布局或头文件包含关系，不重命名或拆分文件。
5. C++ 文件保持本轮已采用的 UTF-8 BOM、CRLF、四空格缩进。code-style.md 的旧 GBK 条款与用户后续明确要求不一致，本批以用户后续要求为准，不顺手重写历史规范。
6. MSVC 对 class/struct 的修饰符号可能不同，必须全量重建，不与旧对象文件混链；不宣称二进制 ABI 兼容。

## 5. 验证与实施记录

状态：89 个类型已完成转换，共修改 57 个头文件；用户已确认，代码与本文一起提交。

| 检查 | 结果 |
| --- | --- |
| 类型特征 | 118 个类型的大小、对齐、标准布局、平凡复制、聚合、多态、虚析构以及默认/复制/移动构造特征前后完全一致；67 处公共基类转换静态检查通过 |
| 主工程 | Debug/x64 全量重建通过；临时输出 Rigid2DSimulationInitCheck，未启动 GUI；无 C4099 声明关键字不一致警告 |
| 拾取与渲染 | PickingChecks 重新构建并运行：302,424 次拾取对照、边界及重叠检查、1,024 个场景的 2,048 次 RGB/透明度/深度缓冲对照通过 |
| 关联与存档 | AssociationChecks 重新构建并运行：64 组关联图、4 组删除入口/暂停组合、连接删除分组、16 组组合场景、7 种 Creator 模式、32 次连接/钉子操作、16 组存档往返通过 |
| 源码范围 | 对照修改前全部 162 个 C++ 源文件与头文件，只有 57 个头文件发生清单内关键字及 public 修改；函数体、字段和已有权限不变 |
| 格式 | 57 个头文件为 UTF-8 BOM、CRLF，无 Tab 或替换字符；git diff --check 通过 |
| 资源回归 | 以 Debug/Win32 分别编译 Git 基线 5b2c803 和当前字体、绘制及音频代码。在资源目录中均加载成功，文字像素及音频采样对照一致；在缺少资源的 Debug/ 目录中均加载失败 |
| 用户运行确认 | 用户反馈重启 VS 后文字和声音恢复，授权提交；没有修改 .vcxproj.user 或资源加载代码。未确定 VS 此前启动环境异常的具体原因，不将重启恢复表述为代码修复 |

测试留在被忽略的 x64/，不提交测试或无关草案。构建仍有数值转换等既有代码警告，本批不处理；代理未进行人工 GUI 验收，用户运行确认如上，不宣称修复任何历史行为问题。
