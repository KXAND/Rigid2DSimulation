# 场景对象关联维护方案

## 1. 状态与范围

- 日期：2026-10-07；检查基线为 75effd6 及当前未提交的轨迹迁移代码。
- 对应 Body 整理清单第 4 项。第 4 节固定本轮实现范围；属性访问仍保留待讨论边界。
- 用户已确认关联维护和删除入口统一的实现，并授权将对应代码与本文一起提交。位置、速度和外观的属性封装不在本轮范围内。

## 2. 已确认：统一删除入口

源码路径相对 Rigid2DSimulation/current/。

| 当前入口 | 当前行为 |
| --- | --- |
| ui/bodyPanel/BodyButtons.cpp → SceneEditor::RequestBodyDeletion | 标记选中物体待删除，并设置 world.isSceneChanged |
| editor/SceneEditor.cpp 中 UpdateBody 的删除模式 | 标记被点击物体待删除，但不设置 world.isSceneChanged |
| ui/connectionPanel/ConnectionButtons.cpp → SceneEditor::RequestConnectionDeletion | 标记选中连接待删除，并设置 world.isSceneChanged |

isSceneChanged 当前用于通知 PhysicsWorld::Prepare 重建连接分组，不是重绘标记。两条物体删除路径都会在后续统一清理阶段移除物体及附属连接，但鼠标删除模式没有主动使分组失效。若没有其他操作触发重建，旧 Group 中可能残留已移除物体的裸指针。

已核对较早提交 13ba108，该差异当时已存在；没有足够证据判断原作者意图，不将其解释为有意设计。

目标：

- 面板删除和删除模式点击统一走物体删除请求入口，由该入口维护删除标记及分组失效，不让 UI 分别补写 isSceneChanged。
- 连接删除采用一致的关联维护原则；具体接口签名及封装范围留待完整方案确定。
- 保留延迟删除和现有帧内处理位置，不改为点击后立即移除。
- 保留“传播附属连接删除标记 → 编辑器清理选择、拖拽和轨迹引用 → 世界解除关联并移除对象 → 重建分组”的顺序。
- 补齐鼠标删除的分组失效属于已明确记录的行为修复，不描述为纯行为不变的重构；不借此修改物理算法或其他历史问题。

验证覆盖两种入口删除同一场景的结果、删除连接导致分组拆开、暂停时删除，以及统一清理和 Prepare 完成后的编辑器引用及分组行为，结果见第 5 节。

## 3. 待讨论：维护权与属性访问

建议保留 World 持有连接、Connection 记录端点、Body 保存反向关联索引的表示，由 World 集中维护成员增删及关联一致性；不是只允许 World 知道连接关系，也不是取消 Connection 的约束职责。

属性修改是否经过 World 应按需要维护的约束判断，不按“增删必须经过、修改一定不经过”机械区分：

- 端点修改即使只是赋值，也会影响多个物体的关联和分组，应由关联维护入口处理。
- 位置修改涉及变换、世界几何和 AABB；还需区分求解器内部逐步更新与外部重定位，不能直接认定任意位置写入均安全。
- 速度是否需要唤醒、外观是否需要缓存失效等，取决于实际机制；不为尚不存在的机制预建统一属性管理层。

位置、速度、外观的最终接口及可见性尚未确认，本记录不授权相关封装或同步时序调整。

## 4. 本轮执行映射

| 位置 | 修改 |
| --- | --- |
| rigid/PhysicsWorld | 新增 AddBody、AddConnection、RequestBodyDeletion、RequestConnectionDeletion；世界容器私有，GetBodies/GetConnections 返回 const 容器引用 |
| rigid/Body | connections 私有，仅 PhysicsWorld 增删，GetConnections 只读查询；删除标记通过 IsMarkedForDeletion 查询 |
| rigid/Connection | 移除 SignUpToBodies；端点私有，构造时传入，GetBody0/GetBody1 查询；删除标记通过 IsMarkedForDeletion 查询 |
| editor/Creator、rigid/SceneBuilders、storage/SceneStorage | 保留原属性、位置和长度计算，原加入容器位置改为 AddBody/AddConnection，移除外部登记及对应分组标记 |
| editor/SceneEditor | 面板和场景点击均转交世界删除请求；UpdateBody 显式接收世界；交互清理继续先于实际移除 |
| render、script、ui、storage、Group | 容器、端点及删除状态读取同步改为查询接口，不改变算法 |

- AddConnection 只接纳已配置对象，不计算位置、长度或执行脚本；自连接只在同一个 Body 上登记一次。空对象、重复加入、端点不属于当前世界属于接口使用错误，以 std::invalid_argument 拒绝；AddBody 同时拒绝携带已有反向关联的对象，删除请求拒绝非本世界对象。重复删除请求保持幂等。
- AddConnection 在实际插入前按需扩容三个容器，避免分配失败留下部分关联；只在容量不足时增长，不逐次精确扩容。
- Connection 允许空端点的创建预览，但这种对象不能加入世界。SceneStorage 作为受控友元在加载未入场对象时恢复端点，不新增入场后改接端点的功能。
- 保持 shared_ptr 所有权、对象地址、容器顺序、随机数调用、存档字节布局及脚本执行时点。构造不自动登记连接。
- ClearObjects 先清除反向关联及缓存，再释放世界持有的对象，外部仍持有的 Body 不应保留已移除连接的索引。重置前的编辑器清理由 App 保留。
- isSceneChanged 仍保留现有非结构配置通知用途，本轮不重写该机制；结构增删不再要求调用方额外设置它。
- 不变更 Body/Connection 隐式复制语义，不引入对象 ID、世界所有者指针或通用命令队列。禁止同时把同一个物体加入多个世界、禁止整体赋值覆盖已入场对象作为使用前提；本轮不能在类型层面阻止所有这类误用，不宣称完成所有生命周期封装。

## 5. 验证与状态

实现和验证已获用户确认，与对应代码一起提交。此前已确认的第 3 项轨迹迁移及其设计文档纳入同一提交，没有覆盖或撤销。

| 检查 | 结果 |
| --- | --- |
| 主工程 | Debug/x64 构建通过，输出临时 Rigid2DSimulationInitCheck，不覆盖用户可能正在运行的程序 |
| 关联与生命周期 | 64 组图，每组 8 个物体、16 条连接；覆盖自连接、平行连接、反向索引顺序、非法及重复登记拒绝、重复删除请求、级联删除、重置后外部持有物体的索引清理 |
| 两条物体删除路径 | 面板对应的编辑器入口/场景删除模式 × 暂停/运行，共 4 组；确认延迟删除、分组失效、选择/悬停/拖拽/轨迹清理及 shared_ptr 释放 |
| 分组 | 删除固定端点后剩余物体能够越界平移；删除连接后两端分组拆开，独立越界。使用真实 PhysicsWorld::Advance 验证，不只检查布尔标记 |
| 创建入口 | 七种 Creator 模式，另加 32 次连接/钉子创建；16 组 Boundary、Gear、Strand、Necklace、CreateConnection 组合场景的数量和关联校验 |
| 存档 | 16 组保存/加载/再次保存字节一致；加载后关联完整、自连接不重复；物体脚本仍在加入世界之前执行，场景脚本在加载完成后执行 |
| 静态及格式 | 当前工作区 17 个 C++ 文件为 UTF-8 BOM、CRLF、无 Tab/替换字符；git diff --check 通过；约束求解和物理公式未改动 |

测试文件位于忽略的 x64/association-check/，不纳入提交。首次测试编译中的 DRect 构造参数错误已在本地夹具中修正，最终重新构建并运行通过。MSBuild 的沙箱内 FileTracker 拒绝访问，已在沙箱外完成构建；未修改项目构建配置。

本轮没有人工 GUI 验收，也没有重跑此前全部历史数值/像素对照套件。存档检查是本轮往返测试，不冒充独立旧实现的二进制兼容证明；数学代码保持不变由差异检查确认。

明确的行为变化为：鼠标删除补齐分组失效；ClearObjects 清除反向关联和缓存并设置分组失效；新入口拒绝不合法登记。其余保留延迟清理、分组在 Prepare 重建以及属性计算和脚本执行时点，不顺手修复旧物理或存档问题。
