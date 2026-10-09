# 应用层渲染几何同步

## 范围

接续 a227e48 的编辑输入重构。只把 SceneRenderer 中的几何更新归还 PhysicsWorld，不改 framework，不拆 UI 更新/绘制，不改物理公式、子步或删除规则。

源码路径相对 Rigid2DSimulation/。

## 设计与实现

- rigid/PhysicsWorld 新增 UpdateGeometry()：先逐物体调用 UpdateShapesPositionAndAABB，再逐连接调用 UpdatePosition。不推进时间，不改编辑状态，不执行物理子步。
- common/SimulationApp::Update 在 AdvanceSimulation 返回之后、获取绘制上下文之前调用 UpdateGeometry。调用位于暂停判断之外，覆盖暂停拖拽、创建物体、属性修改以及零帧时长。
- render/SceneRenderer::RenderObjects 删除上述更新，只遍历并绘制；参数改为 PhysicsWorld const&。绘制顺序仍为物体、连接，预览仍在其后绘制。
- 保留物理子步中原有的几何/端点更新；这不是把物理计算所需的几何同步全部推迟到帧末。

原先是“更新一个物体并绘制，再处理下一个物体，最后更新并绘制连接”；现在先同步所有几何，再统一绘制。RenderBody/RenderConnection 只读取数据，不触发业务或物理回调，因此这一迁移不改变最终场景数据和画面。

PhysicsWorld const& 不代表深层不可变：GetBodies/GetConnections 仍返回 shared_ptr 集合，可间接修改对象。本次只明确 Renderer 的只读契约，不顺手重构全库容器接口；通过实现检查和回归验证不发生写入。

## 验证与提交

实现已完成。代码与本文作为第一项单独提交；第二项 docs/app-ui-boundary.md 留在工作区等待检查，不混入本提交。

- Debug x64 / Win32 构建通过；使用独立目标名，不覆盖常用程序，不修改 .vcxproj.user。
- 16 组几何归属检查通过：覆盖点、圆、多边形等形状及不同连接；直接修改姿态但不推进模拟，验证暂停/零模拟时间下的同步、同步幂等性、空场景，以及新旧几何数据完全相同。
- Renderer 在几何尚未同步和已经同步两种情况下都不改变被检查的物体姿态、速度、变换、形状顶点、AABB、连接端点及约束状态。
- 302424 次拾取对照、原有 1024 个场景的 3072 次绘制对照，加上 16 次新旧几何同步后的绘制对照，共 3088 次 RGB/alpha/depth 对照通过。
- 创建回归通过：224 组场景、1792 帧；42 组创建时序检查，以及输入边界、暂停拖拽、只读预览和新物体当帧运动/显示检查通过。
- 完整拖拽摘要回归通过：512 组场景、4096 帧的状态和画面摘要与 c257dba 基线完全一致，删除清理、非拥有生命周期及连接组 Warp 检查通过。
- 五个修改的 C++ 文件为严格 UTF-8 BOM、CRLF、空格缩进；git diff 检查通过。测试仅位于忽略的 x64/，不提交；未新增人工 GUI 验收记录。
