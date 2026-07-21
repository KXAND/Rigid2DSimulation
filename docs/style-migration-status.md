# 风格迁移执行状态

## 1. 文档定位

本文是 `docs/code-style.md` 和 `docs/naming-and-macro-refactor-plan.md` 的执行台账，仅记录当前风格迁移分支的工作状态、提交边界与验收要求。

- 长期规则以 `docs/code-style.md` 为准。
- 迁移范围与原则以 `docs/naming-and-macro-refactor-plan.md` 为准。
- 本文不定义新的代码规则，也不记录物理逻辑、架构或正确性问题。
- 状态应在每个相关提交后更新；合并该分支前，再决定是否保留本文及其他过程文档。

## 2. 已完成

### 2.1 宏与 DSL

- 项目内部别名宏、控制流宏和序列化读写宏已移除；必要的平台配置宏保留在 `framework/basic/PlatformIncludes.h`。
- 绘制算法宏已改为同文件匿名命名空间中的普通私有函数。
- 项目 DSL 的内建函数、场景构建函数、运行时作用域键和主要配置键已迁移，并同步更新项目内引用。
- `docs/dsl-naming-migration.md` 中列出的 DSL 命名迁移项已完成核对；示例 `.rigid2d` 场景未包含旧脚本文本或旧配置键。
- DSL 中仍保留的语法关键字、运算符、二进制格式标记和经确认无需改名的领域值不属于待迁移项。

### 2.2 文件与类型

- 所有 C++ 源文件和头文件已改为 PascalCase 文件名，并已同步更新项目文件与 `#include`。
- `framework/basic`、`framework/app`、`framework/media`、`framework/parse` 与核心模拟类型已完成第一批类型名迁移。
- 控件与界面类型已完成 `Checkbox`、`RichTextbox`、`Textbox`、`ScrollbarY`、`DragbarX`、`Droplist`、`ScriptExpr` 和 `UI` 到完整 PascalCase 名称的迁移；`SimulationApp` 的界面成员已命名为 `userInterface`。
- 本地化控件、横纵向控件容器、显示浮动面板按钮、下拉项名称回调和分隔线类型已迁移为表达职责的完整名称。
- 全局、刚体、连接、命令、参数、语言、关于、菜单和底部面板类型已迁移为与其职责和文件名一致的完整名称。
- 刚体、连接和参数面板中的脚本编辑器、应用/删除按钮、详情标签、连接选择行及参数编辑控件已迁移为职责名称。
- 全局、命令、底部状态、语言选择和菜单控件类型已迁移为完整职责名称。
- `UserInterface` 的面板成员已从 `fpn_*`、`pn_*` 迁移为 lower camel；资源字符串键保持原样。
- `BodyPanel` 的选择对象、脚本编辑器、连接行、动作按钮和布局成员已迁移为 lower camel 职责名称。
- `ConnectionPanel` 的选中连接、脚本编辑器、动作按钮、详情标签和布局成员已迁移为 lower camel 职责名称。
- `GlobalPanel`、`MenuPanel` 及命令、语言、底部状态、关于面板的成员已迁移为 lower camel 职责名称。
- Win32 消息分发函数和 `framework/basic` 项目工具函数已迁移为 PascalCase；标准库和 C 库接口保持原拼写。
- `RichTextBox` 与 `TextBox` 的编辑、选区、光标和渲染辅助函数已迁移为表达职责的 PascalCase 名称。
- `Delegate` 的受委派控件成员、参数和设置函数已迁移为完整 lower camel 与 PascalCase 名称。
- `RichLabel` 的行宽和渲染辅助函数已迁移为 PascalCase。
- `framework/control` 中剩余控件的渲染辅助函数已统一为 `Render`、`RenderMain` 或 `RenderItems`。
- `CheckBox`、`HorizontalDragBar` 和 `DropList` 的布局、选区和归一化值辅助函数已迁移为完整 PascalCase 名称。
- `Panel` 的静态构造方法及 `FloatPanel` 的可见性方法已迁移为 PascalCase；所有项目调用点已同步。
- `Button` 的文本、颜色、资源键、几何局部变量和浮动面板指针已迁移为表达职责的 lower camel 名称。
- `Label`、`RichLabel`、`CheckBox` 和 `HorizontalDragBar` 的文本、颜色、尺寸、资源键及相关局部变量已迁移为完整 lower camel 名称，并同步更新 UI 派生类。
- `DropList`、`HorizontalSeparator`、`VerticalScrollBar` 与横纵控件列表的字段和局部变量已迁移为完整 lower camel 名称；滚动条和控件列表的直接面板调用点已同步。
- `TextBox` 的编辑值、标签、选区、光标、边距、颜色及局部几何变量已迁移为完整 lower camel；`RichTextBox` 的独立字段保持待单独审计。
- `RichTextBox` 的编辑值、行宽、选区端点和清空编辑值标志已迁移为完整 lower camel，并同步更新脚本编辑器调用点。
- `RichTextBox` 的文本偏移、编辑与文本视区边距、光标横坐标历史及绘制颜色已迁移为完整 lower camel。
- `RichTextBox` 的滚动、闪烁和光标尺寸字段、局部文本视区、选区、光标与迭代器变量已迁移为完整 lower camel。
- `Panel` 与 `FloatPanel` 的内容控件、滚动条、内容视区、溢出状态和面板颜色字段已迁移为完整 lower camel，并同步更新各 UI 面板构造函数。
- `FloatPanel` 的标题、标题栏、关闭按钮、颜色、交互状态和排序时间字段已迁移为完整 lower camel；显示浮动面板按钮已同步读取标题字段。
- `SimulationApp` 的构造函数已按初始化阶段采用 4 空格缩进和逻辑空行；音量与模拟时间状态已迁移为 `volume`、`simulationTime`，DSL 键保持不变。
- `AudioClip` 的采样率、样本、播放位置、循环标志、文件名和加载状态已迁移为完整 lower camel；其接口已统一为 PascalCase。
- `Font` 的加载、初始化和随机文本生成接口已迁移为 `LoadFont`、`Initialize`、`GenerateRandomText`；应用初始化调用点已同步。
- `Vector3::near_zero` 已迁移为 PascalCase 的 `IsNearZero`，不改变其数值判定逻辑。
- `DVector2`、`Vector2` 及其矩形类型的长度平方、边界、包含与重叠查询接口已迁移为 PascalCase，并同步更新绘制、控件和刚体调用点。
- `Vector2` 与 `Vector3` 的长度、归一化和零值查询，以及二维、三维点积和叉积辅助函数，已迁移为 PascalCase；刚体和相机调用点已同步。
- `Matrix2`、`Matrix3` 的构造、查询和变换接口，以及 `Vector3` 的颜色通道访问器，已迁移为表达职责的 PascalCase；绘制、刚体、相机和纹理调用点已同步。
- 基础工具中的字符串数值解析与宽字符文件打开包装已迁移为 `ParseDouble` 和 `OpenFile`，不再与 C 运行库缩写混淆；文本过滤参数已统一为 lower camel。
- 解析器的字符串和变量标记工厂已迁移为 `CreateString`、`CreateVariable`，并消除其参数缩写。
- 解析器令牌、表达式、语句、括号组和运行时值的类型字段已统一为 `type`，并同步更新刚体配置读取点。
- 解析器令牌、表达式和运行时值的数值字段已统一为 `number`，并同步更新配置、刚体和形状构建调用点。
- 解析器令牌和表达式的全局作用域标志已迁移为 `isGlobal`，不改变作用域解析逻辑。
- 解析器令牌和表达式的变量标识符字段已迁移为 `variableName`，避免与运行时 `Value` 类型混淆。
- 解析器令牌、表达式和运行时值的字符串字段已迁移为 `stringValue`，并同步更新配置、刚体和调试字符串读取点。
- `Camera` 的保存、投影计算和第一人称投影计算接口已迁移为 `Save`、`Calculate`、`CalculateFirstPerson`。
- 基础数学工具的区间判断、截断、非负模和弧度归一接口已迁移为 PascalCase；数学常量已迁移为 `kPi`、`kEulerNumber`。
- `AppInput`、`FrameRate` 与 `InputOwners` 的查询、生命周期和时间访问接口已迁移为 PascalCase；`dt`、`dur` 分别明确为 `GetDeltaTime`、`GetElapsedTime`，调用点已同步。
- `AppInput` 的鼠标、键盘和文本输入状态字段，以及 `FrameRate` 的采样与时间统计字段，已迁移为表达职责的 lower camel 名称；输入事件和帧率计算逻辑保持不变。
- `App` 的画布、深度缓冲、字体、帧率、输入所有者、窗口资源和窗口类配置字段已迁移为完整 lower camel；绘制、窗口和消息处理调用点已同步。
- `SimulationApp::InitializeDefaultFunctions` 的注册 lambda 和参数已按 DSL 职责命名，不再使用序号临时变量或 `in` 缩写。
- `WaveBlock`、`WaveOutput` 及 `App::waveOutput` 的音频输出字段和接口已迁移为完整 lower camel 与 PascalCase，并同步更新应用初始化和模拟层调用点。
- `App` 的日志、国际化查询和本地化文件加载接口已迁移为 PascalCase；仅更新应用与界面调用链，解析器中的同名数据字段保持不变。
- 已完成的提交保持文件改名、类型名改名、宏替换和纯排版互相分离。

## 3. 下一批

### 3.1 UI 面板与控件缩写审计

审计 `FPn*`、`Pn*`、`Bt*`、`Lb*`、`Cb*`、`Tb*` 等 UI 面板与控件类型、成员和局部变量。每个缩写必须先依据其实际职责确定名称；不以词典式扩写替代语义判断。

每个确定的命名主题必须完成旧名称检索、`Debug|x64` 构建和差异检查后，作为独立 `refactor:` 提交。

## 4. 剩余工作

### 4.1 命名

- 按模块迁移仍未符合规则的 C++ 函数、成员、参数和局部变量为 PascalCase 或 lower camel。
- 展开 UI 面板与控件中的类型、成员和局部缩写；先确认每个缩写的领域语义，再改名，禁止机械扩写。
- 清理绘制、解析、模拟与应用层中残留的 snake_case 函数名和不明缩写。
- 对 DSL 保留项逐项核对；改变运行时字符串键或示例场景引用时，必须在同一提交中同步完成。

### 4.2 排版与直接依赖

- 按目录迁移 4 空格缩进、Allman 大括号、单语句单行、完整控制流大括号与逻辑空行。
- 按规范整理包含顺序，并在头文件中补齐完整类型所需的直接包含。
- 保持 C++ 文件 GBK 编码与 CRLF；不进行无关转码或全文件格式化。

## 5. 提交与验收

- 一个提交只处理一个目录、命名主题、宏类型或排版主题。
- 每次改动 C++ 源码前先执行 `Debug|x64` 构建；每个提交完成后再次构建。
- 改名提交需检索旧名称，宏迁移提交需检索旧宏，DSL 改名还需检索 C++ 注册点、读取点与项目内脚本。
- 提交前执行 `git diff --check`，确认无空白错误或无意的全文件差异。
- 提交信息使用 conventional commit，例如 `refactor(control): standardize control type names` 或 `style(parse): apply code layout rules`。
- 不在本分支修改物理逻辑、控制流程、资源生命周期、脚本语法或 `.rigid2d` 二进制布局。
