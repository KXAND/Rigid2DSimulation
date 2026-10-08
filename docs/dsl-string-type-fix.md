# DSL 字符串类型标记修复

## 范围与实现

基线：d969a98。用户确认修复字符串类型标记不一致；本轮不重构 DSL，不改变脚本语法，不修复其他缺陷。状态：已实现并验证，经用户确认，代码与本文一起提交。

源码位置：Rigid2DSimulation/framework/parse/Value.cpp。

| 位置 | 修改 |
| --- | --- |
| Value(wstring const&) | 字符串值类型从 ToString 改为 str |
| EvaluateAccess | 字符串字典键的类型判断从 ToString 改为 str |
| Evaluate | 字符串表达式的类型判断从 ToString 改为 str |

Token 和 ScriptExpression 已使用 str；本轮使运行时保持一致。内建函数名称 ToString 保留，不做全局文本替换。不兼容错误的旧标签。

## 验证

- 本地测试位于忽略目录 x64/dsl-string-check/，不提交。
- 同一组 24 项检查，修复前 21 项失败，修复后全部通过。
- 覆盖原生字符串构造、字面量、空串、转义、中文、赋值、ToString 转换、字典字符串/数值读写及转换结果作键。
- 通过真实 Compile/Execute 与 SceneScriptRuntime 验证七种 SetCreator 字符串模式及半径配置；验证四种连接类型配置。
- 数值表达式和向量索引检查通过。Debug x64/Win32 构建通过，输出使用独立程序名；未进行人工 GUI 验收。
- C++ 保持 UTF-8 BOM、CRLF、空格缩进。生产代码只修改三行，不修改工程配置。

## 独立问题（本轮不修复）

测试拼接表达式 `"ball" . ToString(12)` 时发生运行库错误。TryEvaluateDefaultFunction 中 stringArg 返回 wstring const&，但引用来自 Evaluate 返回的临时 shared_ptr 所持对象；对于临时字符串值，对象可能在返回后已销毁，形成悬空引用。

该拼接用例已从本轮通过的 24 项检查中排除。因此本轮只确认字符串类型标签修复，不宣称字符串拼接或整个 DSL 已无缺陷。后续需单独修复求值结果的生命周期并补回拼接回归。
