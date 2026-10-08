# Creator 创建过程归属调整

## 1. 决策与范围

基线：9f1daec。用户已确认由 Creator 管理创建过程；SceneEditor 提供物体命中及输入事实，Renderer 只读取创建预览。本轮先实现并验证，用户检查后再提交代码与本文。

不改 SceneEditor/Creator 类名，不新增事件总线、工具继承体系或状态机类。不改物理计算、输入采集、命中竞争、DSL 语法、创建配置或既有操作行为。

## 2. 修改清单

路径相对 Rigid2DSimulation/current/。

| 文件 | 修改 |
| --- | --- |
| editor/SceneEditor.cpp | 在原物体输入处理位置，将左键按下/释放通知 Creator；删除对创建子类型、active、端点和局部锚点的访问 |
| editor/Creator.h/.cpp | 新增 OnBodyPressed(Body&, Vector2 const&) 与 OnBodyReleased(Body&, Vector2 const&)；Creator 自行判断子类型和操作阶段，计算局部锚点 |
| editor/Creator.h | creationStartPoint、body0/body1、p0Relative/p1Relative、body/connection、active/hovered 改为私有；现有创建配置仍保留公开，不进行全类封装 |
| editor/Creator.h | 新增 GetPreviewBody() const、GetPreviewConnection() const，返回 Body const*、Connection const*；无预览返回 nullptr，不转移所有权；连接 getter 保留原 Renderer 对 body0 存在的判断 |
| editor/Creator.cpp | Update 在原生成预览阶段完成连接预览端点更新；Discard 仍处理外部要求的取消和清理 |
| render/SceneRenderer.h/.cpp | RenderCreatorPreview 接收 Creator const&，通过只读接口绘制；移除写入预览端点的代码 |

OnBodyPressed/OnBodyReleased 仅表示左键在给定物体上的输入事实，不表示要求开始或确认连接。SceneEditor 只在创建模式且该物体实际获得悬停归属时通知。Creator 只在连接/钉子模式处理这些通知；按下且未 active 时记录起点，释放且 active 时记录终点，保持现有条件。

## 3. 必须保留的时序

1. 输入归属仍由画布、编辑器和 UI 在原 PreUpdate 流程中竞争。
2. 在原 SceneEditor::UpdateBody 位置记录物体及局部锚点，发生在物理推进之前；不延迟坐标转换，不在通知函数中提前修改 active 或加入世界。
3. SimulationApp 仍先推进物理并绘制现有物体，再调用 Creator::Update；Creator 在这里推进创建阶段、准备预览或登记正式对象。
4. 预览连接端点仍使用此时的起点物体姿态与当前鼠标坐标。该计算从 Renderer 移到紧邻绘制前的 Creator::Update 末尾，不改变预览对象逐帧重建及配置应用的次数。
5. 首次选起点的帧仍只进入 active；释放且存在终点时才加入连接，没有终点则清理。新对象加入世界的帧及取消/切换模式的原行为不变。
6. 钉子与其他六种创建类型的既有规则不变；不改变存档或正式 Connection 的生命周期。

## 4. 明确不包含

- DSL 的 str/ToString 类型标记不一致：独立缺陷，不在本轮顺手修复。测试通过 C++ 配置工具，不把测试通过声称为 DSL 入口可用。
- 创建期间端点被删除的引用清理：需要单独确定取消策略，本轮不修复。
- Renderer 对正式物体/连接的几何同步：不在本轮移动。
- 新建 UI 类型选择控件、UI 回调化、公开配置私有化：不包含。

## 5. 验证与状态

状态：五个源码文件已实现并验证，用户已检查确认并授权提交，代码与本文一起提交。

- 修改前记录创建全流程基线，修改后对照每帧正式对象、端点/锚点/长度、预览与颜色/透明度/深度输出；覆盖七种模式、暂停/运动、释放到物体/空白/UI、同物体两端及切换模式取消。
- 检查预览在 Creator 更新后即已准备完成，Renderer 不再修改预览对象。
- 重新构建主工程并运行关联、拾取等适用回归。旧测试中直接写 Creator 字段的部分改为从公开输入接口驱动，不为测试增加生产后门。
- C++ 保持 UTF-8 BOM、CRLF、四空格；测试保留在忽略的 x64/，不提交。未获用户确认前不提交本批代码。

### 验证结果

| 检查 | 结果 |
| --- | --- |
| 主工程 | Debug/x64 全量重建及最终增量构建通过；Debug/x86（工程 Win32）构建通过，均输出临时 Rigid2DSimulationCreatorCheck，不覆盖用户程序 |
| 创建过程 | 修改前采集 9f1daec 基线；224 组场景、1,792 帧的正式对象、锚点/端点/长度、预览和绘制缓冲累计摘要与修改后完全一致 |
| 只读预览 | Creator 更新后预览末端已等于鼠标位置；渲染前后预览对象状态摘要一致 |
| 拾取与绘制 | 302,424 次新旧拾取对照及边界检查通过；1,024 个场景各对照正式物体、矩形预览、连接预览，共 3,072 次 RGB/透明度/深度缓冲比较通过 |
| 关联与存档 | 64 组关联图、4 组删除入口/暂停组合、连接分组拆分、16 组组合场景、7 种创建模式、32 次连接/钉子操作、16 组存档往返通过 |
| 源码与格式 | 无外部直接访问 Creator 过程字段的生产调用；SimulationApp 调度、工程配置和 .vcxproj.user 未改；5 个源码文件为 UTF-8 BOM、CRLF，无 Tab/替换字符；git diff --check 通过 |

创建测试使用真实 SceneEditor 拾取/输入处理、物理推进、Creator 更新及 Renderer 绘制；UI 遮挡通过模拟输入归属覆盖。工具类型及配置由 C++ 设置，不经过已知存在字符串问题的 DSL。旧渲染基线使用本地历史预览数据记录，新实现通过公开接口驱动，不为测试放开生产私有状态。

本轮未进行人工 GUI 验收。上述回归不覆盖端点在创建中被删除的策略，不宣称 DSL 创建入口或其他历史缺陷已修复。本地基线和测试位于 x64/creator-check、x64/picking-check、x64/association-check，均不纳入 Git。
