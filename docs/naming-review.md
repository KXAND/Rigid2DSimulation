# 命名 Review

这份文档记录当前仓库里的命名整改情况。原则仍然是优先处理那些跨模块传播、阅读成本高、且收益明显大于迁移成本的名字；局部数学变量、短生命周期临时变量，以及会影响脚本/序列化兼容性的名字，不建议轻易动。

## 1. 已完成的优先改名

- `Cur -> SimulationApp`
  现在能直观看出这是整个模拟程序的主状态与主调度器。
- `mBgr` / `bgr -> background`
  背景对象改成了完整单词，跨模块引用时不再需要猜缩写。
- `proc -> ScriptStatement`
  脚本语句 AST 的语义现在是直白的。
- `expr -> ScriptExpr`
  脚本表达式 AST 类型不再和普通“expression”口语化缩写混在一起。
- `bra -> BracketGroup`
  这个类型现在直接表达“括号分组”的用途。
- `param -> SceneParameter`
  场景参数和普通函数参数区分开了。
- `equal_repos -> useEqualPositionCorrection`
  全局位置修正开关的含义已经清楚。
- `dep -> depth`
  绘制深度相关字段和参数已经统一成 `depth`；刚体和创建器里对应改成了 `depthShape`、`depthPoint`、`depthBody`、`depthConnection`。

补充：

- `current/common/simulation_app.cpp` 里的局部宏别名已经移除，核心调度代码不再依赖 `#define cur (*this)` 这类隐式上下文。

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
- `uiy`
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

1. 第一优先级已经完成：`Cur`、`proc`、`expr`、`bra`、`param`、`mBgr`、`equal_repos`、`dep` 都已落地改名。
2. 下一批如果继续推进，优先看 `o`、`oRelative`、`o_prog`、`ang_prog`、`v_ang_prog`。
3. 最后视 UI 是否重构，再统一处理 `FPn*` / `Pn*` / `RTb*` / `Bt*` 这一批缩写。

## 6. 结论

这个仓库里最值得优先处理的，仍然是那些“类型级别、全局级别、跨模块级别”的高传播名字；这一批现在已经完成。后续如果继续整理，重点应该转向局部高频但仍有歧义的领域名，而不是为了整齐感去机械展开所有 UI 缩写或数学临时变量。
