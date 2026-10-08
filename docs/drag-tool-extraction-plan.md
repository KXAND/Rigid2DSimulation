# 拖拽工具职责拆分

## 1. 决策与范围

基线：c257dba。先拆出拖拽工具，再处理 DSL；本轮不修改 DSL 字符串标记。用户确认实现并验收后，代码与本文一起提交，本轮不自动提交或推进下一项。

新增 current/editor/DragTool.h/.cpp，类型 class DragTool。SceneEditor 按值持有 dragTool，与现有 Creator 并列；选择、删除请求、命中竞争、悬停及轨迹仍由 SceneEditor 负责。不引入统一工具接口、继承体系或事件总线，不改 SceneEditor/Creator 名称。

## 2. 迁移映射

源码路径相对 Rigid2DSimulation/。

| 原位置 | 新位置与约定 |
| --- | --- |
| SceneEditor.h 中 BodyDrag | 原字段和名称搬至 DragTool.h，仍是数据 struct；非拥有 Body const* 为空表示无拖拽 |
| SceneEditor::drag | DragTool 私有 drag；GetDrag() 返回 const 引用，不提供修改状态的 setter |
| SceneEditor::velocityAngularDrag | DragTool::rotationSpeed，表示 Q/E 旋转速度；默认仍为 0，SceneEditor::ResetTools 原位置设为 2 |
| UpdateBody 中开始/保持/结束拖拽的整个分支 | DragTool::UpdateBody，参数为 Body&、AppInput&、InputOwners const&、realDeltaTime、isHovered、enabled；最后两个参数是编辑器提供的命中结果和模式许可，不依赖 SceneEditor 的模式常量 |
| SceneEditor::ApplyDrag | DragTool::ApplySubstep；SimulationApp 在原 Body::Step 之后调用，不保留旧转发接口 |
| UpdateDragAngle、HandleDragPoint、HandleDragWhole、HandleDragForce | 原算法整体迁至 DragTool 同名私有函数，仅替换所属类及旋转速度配置名 |
| SceneEditor::ResetInteraction 中 drag 清空 | 调用 DragTool::Cancel() |
| SceneEditor::PrepareForDeletion 中拖拽引用清理 | 调用 DragTool::PrepareForDeletion()；工具检查当前物体删除标记并取消，不负责删除物体 |
| SimulationApp 中 editor.GetDrag() | editor.dragTool.GetDrag()；Warp 排除对象及渲染上下文继续读取同一状态 |
| 工程及 filters | 注册 DragTool.h/.cpp 到 current/editor；不改包含路径或 .vcxproj.user |

SceneEditor 不再包含拖拽数学公式，也不判断 Q/E/F、右键和刚体逆质量对应的拖拽策略。DragTool 不做物体拾取、不依赖 PhysicsWorld、不处理选择/轨迹、不拥有 Body。

## 3. 保留的执行规则

1. UpdateBody 仍在原逐物体遍历的位置调用，选择/删除判断之后、轨迹采样及创建通知之前；不将拖拽提到独立循环。
2. 首次按下只记录目标、局部锚点和旋转信息，不提前应用原本下一阶段才应用的位移。
3. 已有拖拽在每帧先更新旋转信息并应用原分支，再判断释放或 disabled 后 Cancel；不能因为 enabled=false 就直接返回或提前清空。
4. 每帧与物理子步均保留原右键 > F（未被键盘控件占用）> 固定物体 > 点拖拽的分支顺序。F 的力拖拽仍只在子步中应用，每帧分支仍为空；同时按 Q/E 时 E 后写入，保持原优先级。
5. 暂停只跳过物理推进，不跳过每帧拖拽；点/整体拖拽用 realDeltaTime，力拖拽用 stepDeltaTime。位置、速度、角度、冲量公式及求值顺序不变，不修复零时间步等旧问题。
6. CycleMode 不提前取消拖拽；沿用下一次逐物体更新先应用再取消的时机。ClearSelection 不影响拖拽；ResetInteraction 清空拖拽；ResetTools 只重置旋转速度和 Creator，不额外清空交互。
7. 删除前仍先清理交互引用再移除世界对象；被拖拽物体所属组的 Warp 排除规则、Renderer 读取拖拽锚点的时机不变。

## 4. 验证与状态

状态：职责迁移已实现并经用户确认，代码与本文一起提交；DSL 修改另行处理。

- 拖拽基线：512 组场景、4096 帧，逐帧及子步物体状态、拖拽状态、轨迹和 RGB/alpha/depth 缓冲摘要与 c257dba 完全一致。覆盖动态/固定多边形、点、圆、暂停、Q/E/F、右键、键盘占用、UI 悬停、释放和模式切换。
- 清理检查通过：选择清空不取消拖拽、ResetTools 保持当前拖拽且重置旋转速度、ResetInteraction 清空状态、只取消待删除物体的拖拽、不延长物体生命周期、拖拽排除整组 Warp 且取消后恢复。
- 创建回归：224 组场景、1792 帧，与已有基线一致。
- 关联与存储回归：64 组关联图、4 组删除/暂停入口、连接拆组、16 个内建场景、7 种创建模式及 32 组连接/钉子操作、16 次存储往返通过。
- 拾取与绘制回归：302424 次拾取对照、点边界和深度竞争检查、1024 个场景的 3072 次 RGB/alpha/depth 缓冲对照通过。
- Debug/x64 完整重建和 Debug/x86（Win32）构建通过。使用独立输出名，不覆盖用户运行的程序。测试位于忽略的 x64/，不提交；尚未进行真人界面操作验收。
- 四个算法函数逐项核对，仅替换所属类和旋转速度字段名，公式和求值顺序不变。UpdateBody 沿用 AppInput&，因为现有 IsMouseClickedDown 不是 const 接口；不顺手修改框架输入接口。
- 本轮五个 C++ 文件均通过严格 UTF-8 检查，使用 BOM、CRLF、空格，无 Tab 或替换字符；.vcxproj.user 未改动。未修改历史决策文档或无关草案。
