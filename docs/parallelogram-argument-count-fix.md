# Parallelogram 参数越界最小修复

基线：74dfe70，分支 fix/app-logic。

SceneScriptRuntime 注册的 Parallelogram 检查至少两个参数，却读取第三个参数。两参数调用会越界。

本轮仅将条件改为至少三个参数。不改变缺参时返回默认 Value、多参时忽略多余参数的既有规则；三个参数对应 width、height、shear，形状计算不变。

统一严格参数契约和错误传播的完整修改另存 Git stash，留待本分支完成后在 refactor/dsl-call-contracts 上恢复。与本修复重叠的校验位置，以届时的统一调用层校验为准。

验证：Debug x64 构建通过；独立本地测试覆盖 0、1、2、3、4 个参数，以及合法调用的四个顶点，零失败。源码保持 UTF-8 BOM、CRLF、空格缩进。测试放在忽略的 x64/，不纳入提交。
