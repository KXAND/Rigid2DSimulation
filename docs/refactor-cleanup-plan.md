# 职责重构小清理收尾

## 1. 范围与状态

- 基线：e48ec89。以下五项清理已获用户确认，代码和本文一起提交。
- 目标是删除失效入口和阅读噪声，不继续拆分 Body 的数据，不改变运行时序、物理公式或场景行为。
- 后续 struct 转 class 的范围已获确认：优先转换承担行为和管理职责的类型，纯数据及数学值类型保留 struct；不能仅凭存在构造函数或运算方法决定转换。作为独立批次列明类型和访问权限后处理，不混入本次清理；不借转换新增 getter/setter 或改变继承关系。运行主线与隐含规则梳理也不在本次实施范围内。

## 2. 执行清单

源码路径相对 Rigid2DSimulation/current/。

| 项目 | 修改 | 不变部分 |
| --- | --- | --- |
| 旧越界入口 | 删除 rigid/Body.h/.cpp 中无生产调用的 Body::Warp 声明、实现及过时说明 | PhysicsWorld 调用 Group::Warp 的路径、越界规则及通知时机 |
| 不可达分支 | 删除 render/SceneRenderer.cpp 中 kColorDisplay 分支 break 后的 if (body.point) | 不移动该判断，不猜测或恢复作者意图，不修改其他显示模式 |
| 内部函数可见性 | 将 FindBodiesInGroup 放入 Group.cpp 的匿名命名空间，从 Group.h 删除声明 | DFS 实现、递归和遍历顺序、visited 字段及其使用方式 |
| 注释和局部排版 | Creator.cpp 两条重复疑问合为现有调用约束；Group.h 删除空 XML 参数说明和占位 TODO；Body.cpp 函数间连续空白收敛 | 不删除未解决的数学疑问，不全库格式化 |
| 无用依赖 | 删除 Body.cpp 的 Connection.h 包含和 Group.h 的 Connection 前置声明 | 不扩展到其他头文件依赖整理 |

只改上述 6 个源码文件。保留有明确调试用途的 RenderCollision，不因正常帧流程未调用就删除；visited 局部化、脚本/外观归属、UI 回调化和历史文档全面更新均不纳入。

## 3. 验证与实施记录

五项修改已实现并验证，用户已确认并授权提交。Creator 的端点字段及 active 依赖仍存在，已按用户要求标为 TODO；本次只明确该约束，不宣称消除了耦合。

| 检查 | 结果 |
| --- | --- |
| 主工程 | Debug/x64 构建通过，临时输出 Rigid2DSimulationInitCheck；未覆盖可能正在运行的程序 |
| 拾取与渲染 | 重新编译并运行 PickingChecks：302,424 次新旧拾取对照通过；1,024 个场景、2,048 次颜色/透明度/深度缓冲对照一致 |
| 关联与分组 | 重新编译并运行 AssociationChecks：64 组关联图、4 组物体删除入口/暂停组合、连接删除后的分组拆分、七种创建模式、32 次连接/钉子操作、16 组组合场景和16组存档往返通过 |
| 入口及差异 | Body::Warp 无生产声明或调用残留；FindBodiesInGroup 仅在 Group.cpp 匿名命名空间内使用；Group::Warp、分组遍历内容及物理公式不变 |
| 格式 | 6 个 C++ 文件为 UTF-8 BOM、CRLF、无 Tab/替换字符；git diff --check 通过 |

MSBuild 沿用此前 FileTracker 所需的沙箱外构建方式，没有修改构建配置。未做人工 GUI 验收，未重跑所有历史套件；本轮不宣称修复旧逻辑问题。

测试仍位于忽略的 x64/，ref 本地进度不纳入 Git。既有无关草案不修改或提交。
