# 命名 Review

这份文档只讨论“应该怎么改名”，不直接改代码。原则是优先改那些跨模块传播、阅读成本高、且收益明显大于迁移成本的名字；局部数学变量、短生命周期临时变量，以及会影响脚本/序列化兼容性的名字，不建议轻易动。

## 1. 优先改名的对象

- `Cur`
  建议改成 `SimulationApp`、`RigidSimApp` 或 `SceneApp` 这一类名字。
  原因是它不是“当前对象”，而是整个程序的主状态与主调度器。
- `mBgr` / `bgr`
  建议改成 `background`。
  这是全局可见成员，缩写价值很低，阅读成本很高。
- `proc`
  建议改成 `Statement`、`ScriptStmt` 或 `ScriptStatement`。
  现在它既不像 process，也不像 procedure，语义不清。
- `expr`
  建议改成 `Expression` 或 `ScriptExpr`。
  这是脚本 AST 的核心类型，值得直白命名。
- `bra`
  建议改成 `BracketNode`、`GroupedToken` 或 `BracketGroup`。
  当前名字几乎不能从字面推断用途。
- `param`
  建议改成 `SceneParameter` 或 `NamedParameter`。
  现在它不是通用函数参数，而是场景内可编辑参数。
- `equal_repos`
  建议改成 `useEqualPositionCorrection` 或 `equalPositionResponse`。
  这是全局物理解算开关，现名很难理解。
- `dep`
  如果表示绘制深度，建议统一改成 `depth` 或 `zOrder`。
  当前在 UI 和刚体里都出现，歧义偏大。

## 2. 值得逐步改名，但不必一次做完

- `FPnGlobal`、`FPnBody`、`FPnCon`、`FPnCmd`、`FPnPar`、`FPnLang`、`FPnAbout`
  建议逐步改成 `GlobalPanel`、`BodyPanel`、`ConnectionPanel`、`CommandPanel`、`ParameterPanel`、`LanguagePanel`、`AboutPanel`。
- `PnMenu`、`PnBottom`
  建议改成 `MenuPanel`、`BottomPanel`。
- `RTb*`、`RLb*`、`Bt*`、`Cb*`、`Db*`、`ClX*`、`ClY*`
  这些 UI 缩写可以保留到真正重构 UI 时再统一处理。
  单独零碎改收益一般，但如果后续做面板/控件层整理，统一展开会很值。
- `o`
  在 `Body` / `Shape` 里建议改成 `position` 或 `center`。
  这是典型的“作者知道，读者不知道”的命名。
- `oRelative`
  建议改成 `localPosition` 或 `relativePosition`。
- `v_ang_prog`
  建议改成 `angularVelocityProgram`。
- `ang_prog`
  建议改成 `angleProgram` 或 `rotationProgram`。
- `o_prog`
  建议改成 `positionProgram`。

## 3. 更适合补注释，而不是强行改名

- `i`、`j`、`k`
  在局部循环里完全可以保留。
- `n`、`c`、`d`、`r0`、`r1`、`v0`、`v1`
  在碰撞、冲量、SAT、弹簧约束的数学推导里可以保留。
  这类名字短，但和公式对应紧，强行改长名反而会让推导更难看。
- `tl`、`vp`、`wh`
  在 UI / 绘制代码里已经比较稳定，建议在类型或函数注释里明确它们分别表示 `top-left`、`viewport`、`width-height`，不一定非得全量改名。
- `sp`
  在音频上下文里如果明确表示 sample rate / sample pointer，需要就地补注释，比全项目重命名更省成本。
- `cmd`
  如果语义是脚本文本，建议补注释说明是 scene script / body script / connection script，而不是立刻统一重命名。

## 4. 不建议现在改的名字

- 脚本 DSL 对外暴露的名字，如 `body`、`conn`、`set_cfg`、`create_body`。
  这些名字已经进入脚本输入和存档语义，改名会影响兼容性。
- 二进制序列化直接依赖的字段概念。
  只要没有同步迁移器，就不应为了“更美观”立即调整。

## 5. 推荐的命名整改顺序

1. 先改类型名：`Cur`、`proc`、`expr`、`bra`、`param`。
2. 再改跨模块成员名：`mBgr`、`o`、`oRelative`、`equal_repos`、`dep`。
3. 最后视 UI 是否重构，再统一处理 `FPn*` / `Pn*` / `RTb*` / `Bt*` 这一批缩写。

## 6. 结论

这个仓库里最值得立即改的名字，其实不多，核心是那些“类型级别、全局级别、跨模块级别”的高传播名字。`Cur` 是第一优先级；UI 缩写批次和数学局部变量，不建议为了整齐感单独大动。很多地方补一行注释，比机械重命名更划算。
