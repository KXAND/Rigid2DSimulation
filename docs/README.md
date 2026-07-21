# Rigid2DSimulation 文档索引

这组文档只分析当前仓库状态，不修改现有代码，也不改写 git 历史。

## 本次分析覆盖

- 项目架构与主运行链路
- 当前代码的主要耦合点与重构方向
- 每个代码文件承担的职责
- 学习这个项目的大致时间成本

## 提交历史观察

- 当前 `HEAD` 提交是 `4c90e06`，提交者是 `KXand <71508678+KXAND@users.noreply.github.com>`，提交时间是 `2023-11-15 22:51:51 +0800`，主题是“完成background的基本修改”。
- 根提交是 `27e6f58`，提交者是 `zikun <2570880393@qq.com>`，提交时间是 `2023-09-19 01:51:47 +0800`，主题是“添加 .gitattributes、.gitignore 和 README.md。”。
- 因此需要注意两点：当前仓库最近一次提交虽然是 `KXand`，但时间并不新；仓库最初提交者也不是 `KXand`，而是 `zikun`。

## 文档列表

- [architecture.md](./architecture.md): 项目结构、运行流程、耦合点、重构方向、学习时间评估
- [file-map.md](./file-map.md): 代码文件职责总表
- [naming-review.md](./naming-review.md): 命名 review，说明哪些名字值得改、哪些更适合补注释
- [code-style.md](./code-style.md): 项目的长期代码风格规范
- [style-migration-design-protocol.md](./style-migration-design-protocol.md): 存量风格迁移在修改代码前必须满足的设计条件
- [control-geometry-migration-plan.md](./control-geometry-migration-plan.md): `framework/control` 几何命名的名称对照与提交边界
- [remaining-module-style-design.md](./remaining-module-style-design.md): 其他源码模块已确定的命名主题与实施范围
