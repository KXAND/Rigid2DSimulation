# 代码组织与规范 Review

本文记录当前代码在目录分类、命名、函数与变量规范、宏定义、以及更宽泛代码组织上的问题。这里不讨论文件拆分、类拆分、函数拆分这类结构性重构。

## 2. 文件命名

当前源码文件命名整体比之前清楚。

剩余不一致点：

- `framework` 中仍有一些缩写名，例如 `dvec`、`vptr`、`app_wv`、`draw_comp`、`ft_constant`。这些名字可以保留，但需要文档或注释支撑。
- `current/ui` 仍大量使用 `fpn_*`、`pn_*`。如果暂时不做 UI 重命名，应明确 `fpn` 表示 floating panel，`pn` 表示 fixed panel。
- `draw_comp` 和旧 `comp` 容易产生语义残影。它位于 `framework/draw`，语义是 composite drawing，不是刚体组合构建。
- `docs/file-map.md` 和 `docs/architecture.md` 仍有 `current/rigid/comp.*` 旧引用，这是文档和代码不一致的问题。

## 3. 类型命名

类型命名目前是混合风格：

- 业务核心类型多为 PascalCase，例如 `SimulationApp`、`Body`、`Connection`、`Creator`、`SceneParameter`、`ScriptExpr`、`ScriptStatement`。
- 基础类型仍有小写，例如 `vector2`、`vector3`、`matrix2`、`rect`、`tile`、`font`、`clip`、`token`。
- 控件类型多为 PascalCase，例如 `Textbox`、`Button`、`Panel`、`FloatPanel`。

建议规范：

- 新业务类型优先使用 PascalCase。
- 历史基础类型可以保持小写，不再扩大这类命名。
- 同一模块内新增类型应跟随该模块现有风格，不做跨项目机械统一。

## 4. 函数命名

函数命名混合更明显：

- PascalCase: `Init`、`Save`、`Load`、`Update`、`Render`、`Execute`、`Compile`。
- lower/camel/snake 混合: `read_cfg`、`set_cfg`、`find_config_value`、`try_read_config_number`、`generateRadomInt`、`getRectVerticesByCenter`。
- 脚本 DSL 暴露名: `create_body`、`set_cfg` 等。

建议规范：

- 生命周期函数和 UI 虚函数保持现状，例如 `Update`、`Render`、`PreUpdate`。
- 脚本和配置辅助函数可以继续使用 `snake_case`。
- 同一个文件或同一个小模块内应保持一致。
- 脚本 DSL 和存档语义相关名字不应为了 C++ 命名统一而轻易修改。

明确问题：

- `generateRadomInt`、`generateRadomFloat` 拼写应为 `Random`。它们传播面较广，建议记录为后续单独改名事项。

## 5. 变量命名

变量命名最大问题是作者上下文过强。

可以保留的短名：

- `i`、`j`、`k`: 局部循环变量。
- `p0`、`p1`、`v0`、`v1`、`r0`、`r1`: 几何和物理公式中的局部变量。
- `tl`、`vp`、`wh`: UI/绘制代码中的稳定缩写，但应在文档或注释中说明分别表示 top-left、viewport、width-height。

建议逐步改善的名字：

- `o`: 在 `Body` / `Shape` 中表示位置或中心，新读者不直观。
- `cmd`: 多数情况下表示脚本文本，语义偏宽。
- `ang_prog`、`v_ang_prog`、`o_prog`: 缩写较重，建议分别记录为 angle/rotation program、angular velocity program、position program。
- `cur`: 在 UI 文件中常由宏引入，隐藏了 `App -> SimulationApp` 的下转型。

建议规范：

- 数学局部变量允许短名。
- 成员变量、跨函数变量、脚本配置相关变量应优先表达领域含义。
- 新增代码避免继续扩大 `o`、`cmd`、`cur` 这类需要上下文猜测的名字。

## 6. 宏定义

宏是当前最值得继续治理的组织问题。

当前宏大致分三类：

- 全局基础宏：`ptr`、`msh`、`mkp`、`rep`、`frd/fwt/frdv/fwtv`，位于 `framework/basic/utils.h`。
- 文件局部上下文宏：例如 `current/ui/*.cpp` 中的 `#define cur (*(SimulationApp*)&app)`、`bsel`、`pcsel`、`par`。
- 函数或算法展开宏：例如 `framework/parse/var.cpp` 中的 `d0/d1/s0/s1`，以及绘制文件中的复杂宏。

建议规范：

- 全局宏只能放在明确的基础层，例如 `framework/basic/utils.h`。
- 业务文件里不再新增上下文别名宏。
- 文件局部宏如果只是为了少写几个字符，应优先改成局部引用或 lambda。
- 算法展开宏可以最后处理，因为改动风险通常高于 UI 上下文别名宏。

优先清理对象：

- `current/ui/*.cpp` 中的 `cur`、`bsel`、`pcsel`、`par`。
- `framework/parse/var.cpp` 中的 `d0/d1/s0/s1`。

## 7. Include 与依赖边界

当前 include 主要依赖工程 include path，因此很多文件使用短 include，例如 `"body.h"`、`"utils.h"`。这在小项目里可接受，但同名文件增多后会产生歧义。

明确风险：

- `current/common/utils.h` 与 `framework/basic/utils.h` 名字相同，语义接近。现在部分地方已经通过显式路径规避，但后续新增 include 时需要谨慎。
- `framework` 层不应包含 `current` 层头文件。当前应把这个作为新增代码约束。
- `current` 层可以包含 `framework`，也可以在业务模块间包含，但应避免通过 include path 产生难以判断的同名命中。

建议规范：

- 如果 include 目标可能与其他目录同名，使用带目录的 include。
- `framework` 不依赖 `current`。
- `current/ui` 依赖 `current/common` 和 `current/rigid` 是当前设计的一部分，应明确承认。

## 8. 更宽泛的代码组织问题

当前项目是单体桌面应用，不是严格分层引擎。组织规范应建立在这个事实之上。

主要特点：

- `SimulationApp` 是总调度中心，管理 UI、背景、作用域、参数、音频、输入模式、场景、碰撞、连接、空间网格和全局更新流程。
- `current/ui` 是业务 UI，不是通用控件层。
- `framework/control` 是可复用控件框架，应尽量保持业务无关。
- `framework/parse` 是重要基础设施，但 `Var` 同时承担动态值、作用域、执行入口等多重职责。
- `current/rigid` 是物理与场景构建的主要领域层，但其中既有物理对象，也有脚本构建入口。

这些问题不要求立刻拆分，但要求新增代码保持边界意识。

## 9. 建议落地顺序

在不做文件、类、函数拆分的前提下，建议按以下顺序处理：

1. 更新文档里的旧文件名，特别是 `comp.*`。
2. 明确并写入一份简短命名规范，覆盖文件、类型、函数、变量、宏。
3. 清理 `current/ui` 里的上下文别名宏，优先处理 `cur`、`bsel`、`pcsel`、`par`。
4. 清理 `framework/parse/var.cpp` 的求值宏。
5. 检查构建输出目录和旧 `.obj` 残留是否会干扰搜索或版本控制。

## 10. 结论

当前项目已经有清楚的目录雏形，也完成了一批高收益命名调整。后续重点不应是机械追求统一，而是减少隐藏上下文、明确 `framework/current` 边界、限制宏扩散、修正文档滞后，并为新增代码建立可执行的命名规则。
