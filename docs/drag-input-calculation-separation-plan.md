# 拖拽输入与计算分离

## 范围

基线 be3598b。本文件记录第二步中的拖拽数据拆分；完整输入调度方案见 [第二步：统一编辑输入阶段](editor-input-phase-plan.md)。保留普通点拖拽、整体拖拽和 F 力拖拽三种算法，保留已有子步机制和拖拽计算相对物理积分的顺序。只分离输入解释和刚体计算，不选择新的拖拽模型，不新增阻尼、消息队列、通用约束体系或 Editor 前后两次 Update。

## 数据与调用

源码路径相对 Rigid2DSimulation/。

- 新增 rigid/BodyDrag.h/.cpp。BodyDrag 从 DragTool.h 搬入此模块，保留非拥有 body 指针、relativePoint、radian、velocityAngular；增加 method、targetPosition、targetDelta。空 body 表示没有作用。数据不包含 AppInput、InputOwners、按键或控件。
- DragMethod 使用 Point、Whole、Force。DragTool 把右键解释为 Whole，未被控件占用的 F 解释为 Force，其余为 Point。Point 对逆质量为零物体的 Whole 回退仍由计算模块在执行时判断，保留脚本预设修改物体属性后的原行为。
- targetDelta 保存本帧目标位移，不提前除以时间；计算时继续使用原 realDeltaTime，力拖拽使用子步时长。角度目标与 Q/E 旋转速度在原逐物体输入处理位置更新，E 覆盖 Q 的优先级不变。
- DragTool::UpdateBody 接收 Body const&，只更新自身数据，不修改物体；返回本次每帧作用的数据快照。首次按下只建立持续拖拽数据，返回空快照；已有拖拽返回有效快照，即使本次释放/禁用后内部已经 Cancel，也让计算阶段完成原释放帧作用。没有拖拽时返回空快照。
- DragTool 返回的每帧作用快照经 SceneEditor::Update 返回 App；最终由 SimulationApp::UpdateSceneState 调用 ApplyBodyDragFrame，再调用 Editor.RecordTracks 采样轨迹。Editor 不执行刚体计算。普通/整体仍应用一次，Force 不在这里施加。
- SimulationApp::AdvanceSimulation 接收 Editor.GetDrag() 的持续 BodyDrag 值快照，原 Body::Step 后调用 ApplyBodyDragSubstep(body, drag, realDeltaTime, stepDeltaTime)，再刷新几何；删除 DragTool::ApplySubstep。物理循环不再回调拖拽工具，也不传鼠标和输入归属。现有 PhysicsWorld 回调装配接口与步数不变。
- BodyDrag.cpp 中保留三种原计算，只以 targetPosition、targetDelta 替代从 AppInput 读取的数据。函数在 body 与数据目标不一致时无操作，不缓存指针。Warp 排除及渲染继续读取同一拖拽目标和局部锚点。

本轮保留暂停时的每帧拖拽、释放帧作用、模式切换时机和首次按下行为。DragTool 内部 Cancel 在返回快照前发生，但公式只读返回值、不调用外部回调，完整输入阶段后的计算结果保持原拖拽规则；调度及轨迹采样的具体变化见统一输入阶段方案。数据只供当前阶段同步使用，不排队、不延后一帧、不延长 Body 生命周期。

## 验证与后续

状态：数据拆分已实现，随后已补全统一输入阶段；两部分已获用户确认，与对应文档一起提交。以下旧创建摘要相等的记录仅对应数据拆分中间版本，最终创建时序和验收结果以 editor-input-phase-plan.md 为准。

- 现有 512 组拖拽场景、4096 帧的状态与 RGB/alpha/depth 绘制摘要与拆分前基线完全一致；包含暂停、模式切换、释放、Q/E/F/右键、键盘占用和固定/动态物体。
- 新增数据边界测试通过：256 组场景、1280 次输入/每帧对照、3840 次子步对照；与 be3598b 的旧 DragTool 独立实现比较。验证输入解释不改 Body、输入对象被改写不影响已生成数据、释放快照、首次按下、每子步逆质量切换后的回退，以及空目标/非目标物体和非拥有生命周期。
- 创建 224 组场景、1792 帧与旧基线一致；删除规则、64 组关联图、4 组删除/暂停路径、连接拆组、16 个内建场景、7 种创建模式及 32 次连接/钉子操作、16 次存储往返回归通过。
- 三个计算函数经静态对照，仅替换输入数据来源及目标相关局部名称，公式与运算顺序一致。
- Debug x64/Win32 构建通过。六个源码文件为严格 UTF-8 BOM、CRLF、空格缩进；git diff --check 通过。测试在忽略的 x64/，不提交；未执行人工 GUI 验收。
- 后续手感与模型选择记录于 ref/todo/拖拽方案选择.md。ref 已被 .gitignore 忽略，不修改忽略规则、不将备忘录纳入提交。
- Creator 调度已在统一输入阶段方案中补全；输入系统整体改造、UI 更新与绘制拆分、旧 DSL 拼接缺陷仍不在本步范围内。
