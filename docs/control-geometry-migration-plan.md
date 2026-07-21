# 控件几何命名迁移计划

## 1. 目的与范围

本文规定 `framework/control` 及其直接调用链中几何状态命名的迁移边界和执行顺序。它是 `docs/code-style.md` 在控件几何领域的执行计划，不定义新的长期命名规则。

迁移只处理名称和排版，不改变控件布局、命中检测、绘制参数、滚动计算、调用顺序或对象生命周期。

## 2. 统一术语

| 旧名称 | 新名称 | 适用语义 |
| --- | --- | --- |
| `tl` | `topLeft` | 控件、面板或绘制区域的左上角坐标 |
| `vp` | `viewport` | 控件可见或裁剪的 `DRect` 区域 |
| `w` | `width` | 控件、面板或绘制区域的宽度 |
| `h` | `height` | 控件、面板或绘制区域的高度 |

以下情况不在本计划中机械替换：

- 局部数学坐标、循环下标和已有完整职责名称。
- `DRect`、`Tile`、`App` 等非控件类型的字段；这些类型另行按自身边界迁移。
- 外部 API、标准库和二进制序列化的字段顺序、类型及读写顺序。

## 3. 名称对照与单一提交

本计划不单独提交。本文及 `docs/style-migration-status.md` 必须与下面对应的代码改动一并提交；未在本节列出的名称不得顺手改动。

| 主题 | 旧名称 | 目标名称 | 适用位置 |
| --- | --- | --- | --- |
| 位置 | `tl` | `topLeft` | `Control` 继承字段，以及 `Panel`、`FloatPanel`、`VerticalScrollBar` 自身的位置字段和全部直接访问 |
| 视区 | `vp` | `viewport` | `Control` 继承字段和全部派生控件的直接访问 |
| 尺寸接口 | `GetW`、`GetH` | `GetWidth`、`GetHeight` | `Control`、`Delegate`、全部派生控件和全部调用点 |
| 控件尺寸 | `w`、`h` | `width`、`height` | `Button`、`Label`、`RichLabel`、`CheckBox`、`DropList`、`TextBox`、`RichTextBox`、`Panel`、`FloatPanel`、`VerticalScrollBar` 的字段、构造参数和同一函数内表示控件尺寸的局部变量 |
| 单轴尺寸 | `w` | `width` | `HorizontalSeparator` 的字段、构造参数和局部尺寸变量；`HorizontalDragBar` 的构造参数 |
| 布局累计值 | `w`、`h` | `totalWidth`、`maxHeight`、`maxWidth`、`totalHeight` | `HorizontalControlList::GetWidth` 与 `GetHeight`、`VerticalControlList::GetWidth` 与 `GetHeight` 内的累计或最大值局部变量；按实际聚合方式选择目标名 |

`barWidth`、`nodeWidth`、`boxSize`、`itemHeight`、`visibleHeight` 等已有完整语义的名称保持不变。构造函数的无名实参、`DRect`、`Tile`、`App` 等非控件类型的 `w`、`h` 字段、数学局部变量及其他模块同名标识符均不属于本计划。

本模块只有一个命名提交：`refactor(control): clarify control geometry names`。该提交完整且仅完成以下工作：

1. `Control::tl`、`Panel::tl`、`FloatPanel::tl`、`VerticalScrollBar::tl` 及其直接访问改为 `topLeft`。
2. `Control::vp` 及所有派生控件访问改为 `viewport`。
3. 控件的 `w/h` 字段、构造参数和表示控件尺寸的局部变量改为 `width/height`；`ControlList` 的累计局部变量按本节对照改为 `totalWidth`、`maxHeight`、`maxWidth`、`totalHeight`。
4. `GetW/GetH` 及所有调用点改为 `GetWidth/GetHeight`。
5. 同步 `current/ui` 中直接访问上述控件尺寸字段的语句。

该提交不调整缩进、大括号、单行多语句、空行、注释、布局计算或调用顺序。控件排版将在未来具有完整文件级审计结论时另行设计，当前不计入本计划或本分支的剩余命名提交。

## 4. 每批约束与验收

- 改动 C++ 前先执行 `Debug|x64` 构建，改动后再次执行同一构建。
- C++ 文件保持 GBK 和 CRLF；文档保持 UTF-8 和 CRLF。
- 提交前检索本批旧名称，并按本节的类型和调用链限定结果；不以全局替换触及本计划豁免项。
- 提交前执行 `git diff --check`；每个代码提交同时更新 `docs/style-migration-status.md` 和本文的完成状态。
- 提交使用 conventional commit，按上述批次拆分；不保留旧名称兼容层。

## 5. 完成状态

- [x] 控件几何命名：字段、接口、参数、局部变量和直接 UI 调用点。
