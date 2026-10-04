# UI 文件职责整理：第一阶段

## 1. 范围与约束

本方案于 2026-10-04 经讨论确认，目标是让读者能定位面板入口、按钮行为和其他控件，不改变当前继承与虚函数驱动的 UI 设计。

- 范围为 `Rigid2DSimulation/current/ui/` 及项目文件、直接包含引用。
- 本次明确允许搬迁文件和将同文件中的完整类型按职责分组；不拆分类型或函数的内部实现。
- 面板子目录使用 lower camel，文件使用 PascalCase，例如 `bodyPanel/BodyButtons.h`，不使用 `Actions` 或每按钮一个文件的目录层级。
- 保留 `struct`、成员访问权限、继承关系、事件调用顺序、控件创建顺序、选择和编辑状态、所有权及销毁时机。
- 不实施第二阶段回调化，也不引入控制器、事件总线、兼容头或旧类型别名。
- 按用户最新编码要求保持 UTF-8 BOM、CRLF 和 4 空格缩进；较早文档中的 GBK 要求不适用于本轮。
- 仅改下列已确认的 C++ 类型名。函数名、成员名、本地化键、DSL、脚本和二进制格式不改。
- 代码、本文及执行台账作为同一个 `refactor(ui):` 提交；现有 `SceneParameter.cpp` 修改和未跟踪的 `docs/rigid-responsibility-audit.md` 不纳入。

## 2. 文件归属

以下路径相对 `Rigid2DSimulation/current/ui/`，每个文件组均包含 `.h` 和 `.cpp`。

| 目录 | 面板入口 | 按钮文件组 | 其他控件文件组 |
| --- | --- | --- | --- |
| `aboutPanel` | `AboutPanel` | 无 | 无，直接使用框架文本控件 |
| `bodyPanel` | `BodyPanel` | `BodyButtons` | `BodyControls` |
| `bottomPanel` | `BottomPanel` | 无 | `BottomControls` |
| `commandPanel` | `CommandPanel` | `CommandButtons` | `CommandControls` |
| `connectionPanel` | `ConnectionPanel` | `ConnectionButtons` | `ConnectionControls` |
| `globalPanel` | `GlobalPanel` | `GlobalButtons` | `GlobalControls` |
| `languagePanel` | `LanguagePanel` | `LanguageButtons` | 无 |
| `menuPanel` | `MenuPanel` | `MenuButtons` | 无 |
| `parameterPanel` | `ParameterPanel` | `ParameterButtons` | `ParameterControls` |

`UserInterface.h/.cpp` 留在 `ui/` 根目录，继续承担面板集合和更新调度职责。

### 按钮归属

- `BodyButtons`：应用刚体脚本、删除刚体按钮。
- `CommandButtons`：编译并执行场景脚本的按钮。
- `ConnectionButtons`：应用连接脚本、删除连接按钮。
- `GlobalButtons`：保存、加载场景按钮。
- `LanguageButtons`：中文、英文切换按钮。
- `MenuButtons`：暂停、重置、静音、交互模式、显示模式按钮。
- `ParameterButtons`：添加、删除参数按钮。

### 其他控件归属

- `BodyControls`：`BodyScriptEditor`、`BodyConnectionLabel`、`SelectBodyConnectionCheckBox`、`BodyConnectionControlRow`、`BodyConnectionControlList`、`BodyDetailsLabel`、`BodyEditorControls`。
- `BottomControls`：`FrameRateLabel`、`TimeLabel`、`MousePositionLabel`。
- `CommandControls`：`CommandScriptEditor`。
- `ConnectionControls`：`ConnectionScriptEditor`、`ConnectionEditorControls`、`ConnectionDetailsLabel`。
- `GlobalControls`：`GlobalDetailsLabel`。
- `ParameterControls`：`ParameterNameTextBox`、`ParameterValueDragBar`、`ParameterValueTextBox`、`ParameterControlRow`、`ParameterControlList`。

原面板入口只保留对应面板类型及构造实现。控件和按钮的声明、定义一起搬迁；不复制实现。

## 3. 已确认的类型命名

采用“对象 + 用途名词 + Button”的词序。以下为本轮完整映射，不机械扩展到其他类型或资源键。

| 旧名称 | 唯一目标名称 |
| --- | --- |
| `ApplyBodyButton` | `BodyScriptApplicationButton` |
| `DeleteBodyButton` | `BodyDeletionButton` |
| `ApplyConnectionButton` | `ConnectionScriptApplicationButton` |
| `DeleteConnectionButton` | `ConnectionDeletionButton` |
| `CompileButton` | `ScriptExecutionButton` |
| `SaveButton` | `SceneSaveButton` |
| `LoadButton` | `SceneLoadButton` |
| `PauseButton` | `SimulationPauseButton` |
| `ResetButton` | `SceneResetButton` |
| `MuteButton` | `AudioMuteButton` |
| `ModeButton` | `InteractionModeButton` |
| `DisplayButton` | `DisplayModeButton` |
| `AddParameterButton` | `ParameterAdditionButton` |
| `DeleteParameterButton` | `ParameterDeletionButton` |

`ChineseLanguageButton`、`EnglishLanguageButton` 已是用途明确的名词式名称，保持不变。选择框不是本轮按钮改名对象；框架的 `ShowFloatPanelButton` 等类型不改。

`ScriptExecutionButton` 表达原函数同时编译并执行的既有行为，不代表增加执行步骤；暂停、静音等按钮仍保持原来的切换行为。

## 4. 包含与工程文件

- 同目录使用本地头文件名，跨面板使用 `current/ui/<目录>/<文件>.h`，不添加九个全局包含目录。
- 按新的职责补齐直接依赖；不让面板构造文件为子控件实现引入整个模拟对象定义。
- 同步 `.vcxproj` 中的编译单元与头文件，以及 `.vcxproj.filters` 的九个面板目录。
- 不保留旧位置的转发头文件。

## 5. 验证

- 改动前执行 `Debug|x64` 构建，建立基线。
- 比较搬迁前后的类型定义和完整成员函数，归一化已批准的类型改名后，代码词法单元与注释内容一致。
- 检查 45 个面板相关类型和原有成员函数均只出现一次；`UserInterface` 的执行逻辑不变。
- 检查旧类型名、旧包含路径、旧项目引用没有源码残留。
- 检查工程文件引用存在且无重复，新增源文件全部参与编译。
- 检查 UTF-8 BOM、CRLF、空格缩进及 `git diff --check`。
- 改动后执行 `Debug|x64` 构建和差异审查；运行时交互验证若未执行，必须在交付中明确说明。

## 6. 实施状态

- 改动前 `Debug|x64` 构建已通过。
- 已完成九个面板目录的整理：原 18 个面板源文件和头文件迁移为 44 个职责文件，`UserInterface.h/.cpp` 保留在根目录。
- 已完成表中 14 个按钮类型的改名，旧名称没有源码残留。
- 已直接对照 Git 基线验证 45 个类型定义、87 个成员函数：归一化已批准类型改名后，代码词法单元与注释一致；`UserInterface.cpp` 仅更新包含路径。
- 已核对全部 46 个 UI 源文件和头文件在工程及筛选器中注册，引用存在、无重复，未添加面板子目录到全局包含路径。
- 改动后 `Debug|x64` 构建、UTF-8 BOM/CRLF/空格缩进检查与差异检查均通过。
- 本轮未执行窗口交互测试，不据此宣称运行时行为已全面验收。第二阶段回调化尚未开始。
