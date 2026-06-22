# 项目架构分析

## 1. 项目定位

这是一个基于原生 Win32 窗口和 C++ 的 2D 刚体模拟程序，当前代码不是“引擎 + 游戏逻辑”的分层结构，而是一个单体桌面应用。它把下面几类职责都放在同一个程序里：

- 应用壳层：窗口、输入、帧率、音频输出
- 场景与物理：刚体、碰撞、连接件、分组、空间网格
- 渲染：软件栅格化、字体、文字、几何图元
- UI：侧边面板、底部面板、按钮、文本框、拖条等自绘控件
- 脚本系统：自定义 tokenizer、括号分析、表达式/语句解析、运行时执行
- 资源与序列化：`.rigid2d` 场景文件、字体 tile、音频 clip、本地化文本

## 2. 主运行链路

当前主链路可以概括为：

1. `main.cpp`
   `main()` 只做一件事：构造 `Cur` 并调用 `Run()`。
2. `Cur : App`
   `Cur` 继承 `App`，在构造函数里初始化窗口、字体、UI、背景、默认脚本函数和音频资源。
3. `App::Run()`
   进入 Win32 消息循环。空闲帧时执行：
   - 更新帧率
   - 同步输入
   - 调用 `Cur::Update()`
   - 推送音频
   - 把软件缓冲区显示到窗口
4. `Cur::Update()`
   这是全项目最核心的总调度函数，依次负责：
   - 背景、创建器、刚体、UI 的 `PreUpdate`
   - 基础状态刷新和全局脚本执行
   - 音乐切换、暂停处理、删除清理
   - 背景渲染
   - `stepNum` 次物理子步进
   - 刚体与连接件渲染
   - 创建器和 UI 的最终更新
5. 物理子步进
   每个子步进内部做：
   - `Body::Step`
   - `Group::Warp`
   - 静电作用
   - 建立空间网格
   - `DetectCollisions`
   - `Collision::simulate`
   - `Connection::Simulate`

## 3. 模块划分

### 3.1 应用平台层

- `App` 管窗口生命周期、DIB 显示、字体初始化、本地化加载、音频系统初始化。
- `app_msg.cpp` 负责 Win32 消息到输入状态的转译。
- `app_item.*` 把输入、FPS、owner/hover 焦点状态抽出来。
- `app_wv.*` 和 `clip.*` 是自定义音频播放链路。

这层已经能独立承担“一个自绘桌面应用壳”，但仍然和上层业务强耦合，因为控件系统、背景、渲染缓冲都直接复用 `App` 成员。

### 3.2 场景与物理层

- `Body` 是刚体主体，包含形状、速度、角速度、材质、拖拽状态、轨迹和脚本配置。
- `Shape` 负责圆与凸多边形几何。
- `Collision` 负责碰撞检测结果与冲量求解。
- `Connection` 负责 rope/link/spring/cord 四类连接件。
- `Group` 通过连接关系把刚体划组，用于整体穿越场景边界。
- `Creator` 负责交互式创建物体与连接件。
- `comp.cpp` 负责脚本侧几何模板和组合场景构建。

这一层从功能上是完整的，但职责边界并不清晰。`Body` 同时负责数据、物理、输入拖拽、脚本刷新、渲染；`Cur` 又进一步统筹所有调度。

### 3.3 脚本系统

编译/执行链路是：

`tokenize -> bra_analysis -> parsing_expr/parsing_procs -> Compile -> Execute`

对应文件：

- `token.*`: 词法切分
- `bra.*`: 圆括号/花括号/方括号层级分析
- `expr.*`: 表达式 AST 与解析
- `proc.*`: 语句 AST 与解析
- `var.*`: 动态值、作用域、执行器

脚本系统是这个仓库的重要扩展点。场景配置、body/connection 配置、creator 配置、全局 update 回调都依赖它。

### 3.4 渲染与资源层

- `tile.*` 是像素缓冲与 alpha 数据结构。
- `draw_*` 文件是软件渲染图元。
- `font.*` + `ft_constant.cpp` + `fonts/*.tile` 组成字体系统。
- `localization.txt` 提供文本资源。
- `sound/*.clip` 提供背景音乐数据。

这套渲染并没有依赖图形 API，而是完全自己做软件绘制，再由 `SetDIBitsToDevice` 显示。

### 3.5 UI 层

UI 层本质上是一个小型自绘控件框架：

- 基类：`Control`
- 容器：`CtrlListX/Y`、`Panel`、`FloatPanel`
- 基础控件：`Button`、`Label`、`Textbox`、`RichTextbox`、`Checkbox`、`DragbarX`、`Droplist`、`ScrollbarY`
- 页面装配：`UI`、`FPn*`、`PnMenu`、`PnBottom`

特点是“轻量直接”，代价是大量代码通过宏和 `(*(Cur*)&app)` 这种方式向下转型，控件与业务状态是强绑定的。

## 4. 当前架构的核心特点

## 优点

- 功能集中，改动路径短，适合个人项目快速迭代。
- 自定义脚本让场景描述能力明显强于纯手工 UI 配置。
- 软件渲染、自定义字体、自定义控件让项目在外部依赖上很轻。
- 物理、UI、脚本、资源全部在同一工程里，便于从源码直接追踪问题。

## 成本

- `Cur` 是明显的 God Object。
  它同时管理 UI、背景、作用域、参数、音频、输入模式、场景、碰撞、连接、空间网格和全局更新流程。
- `Body` 也承担过多职责。
  它既是数据对象，又处理物理积分、拖拽交互、渲染、脚本刷新和序列化。
- 宏别名很多。
  `my_def.h` 和 `ctrl_def.h` 把大量上下文隐藏掉，阅读局部代码时经常看不出变量来源。
- 指针模型偏脆弱。
  容器里存 `shared_ptr`，横向引用却大量使用裸指针，删除时依赖每帧清理顺序维持有效性。
- UI 直接读写核心状态。
  例如 `FPnBody`、`FPnCon`、`FPnPar` 都直接操作 `Cur`、`Body`、`Connection` 内部字段，没有中间层。
- 脚本系统是动态类型，灵活但缺少约束。
  调错成本偏高，静态检查能力弱。

## 5. 推荐的重构方向

这里给的是“方向”，不是建议你现在就大改。按收益/风险排序，优先级如下。

### 方向 1：先拆 `Cur`

把 `Cur` 至少拆成 4 个明确子对象：

- `SceneState`: bodies、connections、groups、selection、scene config
- `SimulationSystem`: step、collision、grid、electrostatic、group warp
- `ScriptSystem`: scope、Compile/Execute 封装、默认函数注册
- `EditorState/UIBridge`: 当前模式、creator、参数面板交互状态

这是收益最高的一步，因为现在多数耦合都汇到 `Cur`。

### 方向 2：把 `Body` 从“全能对象”收缩为领域对象

建议把下面职责挪出去：

- 拖拽交互：单独 `BodyInteractor`
- 渲染：单独 `BodyRenderer`
- 脚本刷新：单独 `BodyScriptBinder`

保留在 `Body` 本体里的最好是：

- 形状与质量属性
- 当前位姿和速度
- 与连接件的关系
- 少量序列化逻辑

### 方向 3：减少宏和 C 风格转型

目前很多 UI 文件都有：

- `#define cur (*(Cur*)&app)`
- `#define bgr (*cur.mBgr)`
- `#define getLocalizedString (cur.getLocalizedString)`

建议逐步改成显式函数参数或引用成员，不要一口气全删，但新增代码不要继续扩大这种模式。

### 方向 4：引入更稳定的对象引用方式

现在的 `Body*`、`Connection*` 在删除、重建、切换选中对象时都依赖调用顺序。

更稳妥的方向是：

- 场景容器内部维护稳定 ID
- 选中态保存 ID 或 handle
- 连接关系保存弱引用或 ID

这样能显著降低 UI 和物理更新之间的时序风险。

### 方向 5：脚本系统单独封边界

不是先重写脚本，而是先把接口收窄：

- `Compile(code) -> Program`
- `RunProgram(scope, program)`
- `ApplyBodyConfig(body, program_or_var)`
- `ApplyConnectionConfig(connection, program_or_var)`

先把“调用点”集中，后面再考虑语法、类型系统、错误报告。

### 方向 6：渲染和更新分离

当前很多 UI/控件 `Update()` 同时做状态更新和绘制。后续如果要提升可维护性，建议逐渐改成：

- `PreUpdate`
- `Update`
- `Render`

这样更容易定位输入 bug、布局 bug 和绘制 bug。

## 6. 学习时间评估

下面的时间假设是“通过阅读源码并能做小改动”，不是“完全重写”。

### 情况 A：你已经熟悉 C++、二维数学、基础物理

- 读懂整体结构：`2-4` 天
- 能改一个局部功能或修一个明显 bug：`1-2` 周
- 能比较稳地做模块级重构：`3-4` 周

### 情况 B：你会 C++，但不熟 Win32、自绘 UI、脚本解析

- 读懂整体结构：`1-2` 周
- 能做中小型修改：`3-5` 周
- 能独立规划重构：`5-8` 周

### 为什么时间不会更短

- 这个项目不是标准框架项目，很多基础设施是自写的。
- 需要同时理解物理、UI、自绘渲染、脚本和 Win32 事件循环。
- 宏和隐式上下文较多，初读时跳转成本高。

### 推荐学习顺序

1. `main.cpp -> cur.h/cpp -> app.h/cpp -> app_msg.cpp`
2. `body / shape / collision / connection / group`
3. `creator / comp / par`
4. `token / bra / expr / proc / var`
5. `ui / panel / float_panel / textbox / rich_textbox / fpn_*`
6. `draw_* / tile / font / localization`

如果你的目标只是“能加一个新物体或新连接件”，学到第 3 步通常就够了；如果目标是“系统性重构”，第 4、5 步必须补齐。
