# Body 悬停与拖拽状态迁移

## 1. 本步范围与工作流

- 日期：2026-10-06；起点：`aaacaab`。
- 本文先确定实现映射，再修改代码和验证；完成后等待用户检查，确认后将对应代码和本文一起提交。不自动开始下一项。
- 只处理 Body 的悬停、拖拽过程状态，以及绘制、越界和生命周期必需的调用调整。
- 不迁移 track、appearance、scriptData，不引入 SceneBody，不改变拾取规则、物理公式、脚本、存储格式或 UI 架构。
- 源码使用 UTF-8 BOM、CRLF、4 空格。验证代码仅放在被忽略的 `x64/`。

## 2. 已核实的行为

- InputOwners 每帧仅有一个 hovered；Body 仅在该目标上且 IsMouseClickedDown(0) 时启动拖拽。
- IsMouseClickedDown 是当前按下且上一帧未按下。App 每帧调用 Update，随后通过 AppInput::Reset 保存上一帧按键；暂停不会跳过 SceneEditor::UpdateBodies。
- 因而正常输入序列最多一个拖拽物体：前一帧仍按下则本帧不会出现新按下沿；前一帧松开则旧拖拽已经结束。没有脚本设置拖拽标志或复制活动 Body 的生产入口。
- 保持单鼠标语义，不引入按物体映射或多选拖拽功能。
- 开始拖拽的帧只建立锚点及目标角度；持续拖拽帧先应用帧级拖拽，再判断是否松开或退出模式。释放帧仍会执行最后一次帧级拖拽。
- F 力拖拽仅在物理子步施加；右键整体拖拽优先于 F，固定物体整体拖拽优先于普通点拖拽。Q/E 与键盘输入占用规则保持原样。
- PreUpdate 只是拾取竞争，UI 随后还可覆盖 hovered；最终悬停目标在 UpdateBodies 时确认。

## 3. 确定的映射

以下路径相对 `Rigid2DSimulation/current/`。

| 原状态或入口 | 本步目标 |
| --- | --- |
| Body::hovered | SceneEditor 私有 hoveredBody，GetHoveredBody() 只读查询 |
| Body::dragged | SceneEditor 私有 BodyDrag::body；空指针表示无拖拽 |
| Body::relativeDragPoint | BodyDrag::relativePoint |
| Body::radianDrag | BodyDrag::radian |
| Body::velocityAngularDrag | BodyDrag::velocityAngular |
| SceneEditor::velocityAngularDrag | 保留，仍是编辑器的旋转速度设置，不是当前拖拽状态 |
| SceneEditor::ClearDeletedSelection | 改为 PrepareForDeletion，删除前同时清理选中、悬停及拖拽引用 |
| 重置场景时 ClearSelection | 改为在 ClearObjects 前调用 ResetInteraction，清理全部上述引用；普通 ClearSelection 仍只取消选择 |
| SceneRenderContext | 添加 hoveredBody、draggedBody、relativeDragPoint，由 App 读取编辑器状态组装 |
| PhysicsWorld::Advance | 显式增加 Body const* warpExcludedBody 输入，向 Group 传递，不持有编辑器或交互状态 |
| Group::Warp | 接收同一排除物体，组内包含它时不平移；固定判断和 track 清理保持原样 |
| Body::Warp | 旧无调用入口暂保留，改为显式 bool isWarpBlocked 参数，消除其对已删除字段的依赖；不顺手清理旧接口 |

BodyDrag 定义在 SceneEditor.h，成员名为 drag，包含只供交互过程使用的目标指针、局部锚点、角度和角速度；SceneEditor 通过 const 引用 GetDrag() 提供读取，不允许外部直接修改。drag.body == nullptr 表示没有拖拽物体，结构体本身仍存在，其余字段仅在拖拽期间有效。目标使用非拥有的 Body const*，实际运动修改继续通过 UpdateBodies/ApplyDrag 传入的 Body& 完成。

ResetInteraction 表达场景重置时清空交互的意图，不改变工具或模式；PrepareForDeletion 表达删除对象前清理编辑器引用的意图，不负责标记或删除对象。两者均在声明处注明边界，不再用 Targets 统称两种操作。

所有数学计算只替换成员来源，不重写公式。结束拖拽后重置角度和锚点；旧 Body 中未激活时残留的这些值没有生产读取用途，也不保存到场景文件。

## 4. 生命周期和跨模块约束

1. 删除流程仍是标记关联连接、编辑器清理已删除目标、世界实际删除，不能提前释放对象。
2. 新状态非拥有，不延长 Body 生命周期。重置/加载所调用的 Reset 在释放世界对象前清理编辑器目标。
3. 普通空白区域点击只清选择，不顺手取消拖拽。切换模式仍由下次 UpdateBodies 按旧顺序结束拖拽。
4. UpdateBodies 从存活物体中匹配最终输入所有者，不将 UI 的 void* 直接转换为 Body*。
5. PhysicsWorld 和 Group 不包含 SceneEditor，排除物体只作为本次 Advance 的同步输入；不改变越界平移、轨迹采样和清理的位置。
6. Renderer 不读取编辑器对象，不在 Body 上保留高亮状态副本；外观颜色仍由 Body 持有。
7. 创建预览、实际世界对象、选择和悬停保持各自含义，不因拆出状态而自动让预览成为交互目标。

## 5. 验证要求

- Debug|x64 构建，检查生产源码旧字段引用和接口替换完整性。
- 独立旧交互实现与新实现对照：按下、持续、松开、模式切换、鼠标移动、Q/E、右键、F、固定物体、键盘占用、暂停及物理子步。
- 多物体连续帧验证单拖拽关系，拾取同深度顺序、UI 覆盖、视区限制、删除和重置不残留交互引用；取消选择不能取消拖拽。
- 旧渲染实现保留自身旧字段，比较相同目标状态下的像素、alpha、深度及预览；不能修改旧算法使其调用新状态计算来生成期望值。
- Group 的旧判断和新排除输入比较，覆盖被拖物体所在组、其他组、固定物体、边界与轨迹清理。
- 复用物理、脚本/存储、Body 和场景构建测试，区分本次运行结果与历史验证记录。未做 GUI 验收则明确记录。

## 6. 当前进度

本步实现已获用户检查确认；按反馈调整命名和注释后复验，将本文与对应代码一起提交，不自动开始下一项代码修改。

### 6.1 实现结果

- Body 的五个交互字段已移除。SceneEditor 拥有私有悬停目标及单个 BodyDrag，只读查询不暴露可修改的拖拽数据。
- 新旧交互状态转换、帧级拖拽和子步拖拽保留原执行位置；清理目标在删除对象前完成，Reset 在 ClearObjects 前清理选择和交互目标。
- Renderer 通过本次绘制的 context 接收悬停目标、拖拽目标与局部锚点，不包含 SceneEditor，不保存跨帧状态。
- PhysicsWorld 将显式排除物体传给 Group::Warp，不理解鼠标、拖拽模式或编辑器。Body 的旧 Warp 保留，仅改为显式限制输入。
- 未迁移轨迹、外观或脚本数据，未改物理公式、初始化流程、几何更新时间和二进制记录。

### 6.2 本次验证

验证日期：2026-10-06。

- 主工程 Debug|x64 构建通过；本地测试重新构建并链接本次生产对象。
- 交互：2048 组输入组合、6144 次拖拽子步、选择/删除/模式及七种创建模式通过。旧实现保留自身旧交互字段，新实现通过按下事件建立初始拖拽，不增设测试专用状态写入接口。
- 连续输入：256 场景、每场景 24 帧，共 6144 帧双物体新旧对照通过，覆盖鼠标跨物体、UI 输入覆盖、右键、F、Q/E、键盘占用、固定物体、暂停、模式变化及释放；旧实现在这些连续序列中也始终最多一个拖拽目标。
- 生命周期：选择清理不取消交互；删除拖拽目标不误清另一个悬停/选中目标；删除悬停目标、清空场景与空世界更新通过。弱引用检查确认编辑器不延长物体生命。App 的 Reset/删除编排同时作静态核对，未以这些无窗口测试冒充实际 App 窗口验收。
- 越界：2304 组独立旧算法对照及空组检查通过，覆盖无排除、排除组内任一物体、排除其他物体、固定物体及边界组合，位置和 track 清理一致。额外经过真实 PhysicsWorld::Advance 验证，只跳过排除目标所在连通组，解除排除后恢复平移。
- 渲染：960 场景、每场景五阶段 RGB/alpha/depth 对照通过，覆盖背景、物体、原有预览、独立预览和碰撞调试绘制；独立预览不继承相同几何世界物体的悬停/拖拽身份，连接预览端点一致。
- 其他回归：物理 10 场景/800 子步、删除和默认重置；脚本 10 帧双次 Update、128 组预设/512 子步；Body 5 类构造/1280 配置/48 存储场景；Connection 256 配置/64 存储场景及加载登记顺序；SceneParameter 48 存储场景；1835 个形状 Value 树及 288 个场景构建对照，全部通过。
- 静态差异核对：Body/Group 实现仅改变 Warp 的输入及原拖拽判断来源；四个角度/拖拽计算函数还原字段来源后与基线逐字一致。rigid 无旧交互字段访问和 SceneEditor 依赖，保留的 hoveredColor/draggedColor 是外观配置，不是交互状态。
- 本次 11 个 C++ 文件及本文使用严格 UTF-8 BOM、CRLF，无 Tab 或替换字符；git diff --check 通过。原有未跟踪 context、rigid 审计/复核及 UI 回调方案未改动；本地测试仍在被忽略的 x64 下，不纳入提交。

未进行实际窗口交互验收。既存数值边界、存储健壮性和删除/分组规则的正确性问题不在本步修复范围；测试一致表示本次职责迁移的行为对照通过，不代表整个项目不存在历史问题。

### 6.3 用户反馈及提交前复验

- 用户已确认提交本步，并要求改用 ResetInteraction、PrepareForDeletion、BodyDrag、drag、GetDrag。源码、调用点、本地测试及本文已同步，不保留旧接口；声明处补充清理范围和无拖拽时的含义。
- 本轮仅调整上述命名和注释，不改变第 6.2 节已验证的行为。七组本地测试重新构建和运行，交互、渲染、越界、物理、脚本/存储、Body、形状及场景构建检查全部通过。
- 原 Debug EXE 正在运行，常规构建链接时出现 LNK1168，不能覆盖该文件。未关闭进程，改用 TargetName=Rigid2DSimulationInteractionCheck，完整构建链接通过，输出仍在被忽略的 x64/Debug 下。正在运行的旧进程不会自动加载此次改动。
- 提交包含本步 11 个 C++ 文件及本文，不包含本地测试或此前其他未跟踪文档。后续初始化/派生状态工作另行设计，不纳入本次提交。
