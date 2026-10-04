# SimulationApp 职责整理与分步实施

## 1. 目标和执行约定

目标是让 `SimulationApp` 主要负责应用组装、帧阶段调度及跨模块协调，不再直接承担物理计算、编辑交互、场景绘制、DSL 和存储的全部实现。

本方案于 2026-10-05 经讨论确定五步方向。第一步的函数迁移映射已确定；后续各步在开始代码修改前，必须补齐该步的成员、函数、文件和调用点迁移表，不能把方向性描述当作任意拆分的授权。

每步执行同一流程：设计落地 → 实施和验证 → 停下来供用户检查 → 用户明确确认 → 提交该步代码及对应文档 → 开始下一步。用户提出问题时先处理，不跳过检查，也不自动提交当前步骤。最后一步确认后提交并结束，不再自行扩展任务。

- 每步按完整职责组织提交，使用 Conventional Commit，不按每个小函数拆提交。
- 不改变物理公式、时间步机制、脚本语法、本地化键和存储格式；行为修复必须单独说明并获得授权。
- 所有路径相对仓库根目录。C++ 保持 UTF-8 BOM、CRLF、4 空格缩进；函数采用 PascalCase。
- 本次开始前已有 `SceneParameter.cpp` 修改、三角形绘制修复及其测试、rigid 审计和 UI 回调方案等未提交内容，不并入本步骤提交，不覆盖或回退。

## 2. 五步工作划分

| 步骤 | 具体方向 | 完成后 SimulationApp 的角色 |
| --- | --- | --- |
| 1. 整理帧流程 | 在原类中提取六个私有函数，保留成员和执行顺序 | 主流程可读，但尚未解耦 |
| 2. 提取物理世界 | 迁移刚体、连接、分组、碰撞、物理参数、时间和网格，以及模拟推进、对象关联维护和删除操作；同步 UI、脚本等访问点 | 决定何时推进、暂停或重置模拟，不直接展开求解循环 |
| 3. 提取编辑交互 | 迁移选择、交互模式、创建工具及选择、拖拽、创建、删除请求处理 | 协调应用操作，不亲自管理编辑交互细节 |
| 4. 整理渲染职责 | 场景渲染模块承接背景、物体、连接、预览及调试显示；逐步分离 Creator、UI 中的绘制 | 安排渲染阶段，不直接组织像素绘制细节 |
| 5. 移出脚本和存储 | 分别整理 DSL 注册、执行、参数同步及场景文件读写 | 发起并协调脚本执行、加载、保存和重置 |

第二步的物理世界负责遍历需要相互作用的刚体并调用 `Electrostatic()`；静电公式继续由具体算法函数实现，不为静电单独新增管理器。

物理对象删除和物理关联清理归物理世界；删除后的选择清理由应用暂时协调，第三步迁入编辑交互。新模块不得通过持有整个 `SimulationApp&` 继续任意操作原成员来伪装职责迁移。应用和模块之间的必要接口须在对应步骤设计中明确。

第四步先迁移职责并保留时序。是否集中到帧末绘制，须另行明确 UI 操作生效、创建删除时机和遮挡规则；不能在搬迁时暗中重排。画布保留范围、底图及区域输入职责，物理所需几何/AABB 更新不因绘制拆分而停用或延后。

第五步不为缩短类而强行拆出音乐管理器；音乐逻辑是否保留在应用中按实际复杂度判断。各步具体新类型名、文件位置和接口，在该步详细设计中固定，不由本表预先假定。

## 3. 第一步：文件和函数映射

### 3.1 修改范围

- `Rigid2DSimulation/current/common/SimulationApp.h`：在类型末尾新增 `private:` 及六个 `void` 无参成员函数声明，现有成员和函数访问权限不变。
- `Rigid2DSimulation/current/common/SimulationApp.cpp`：把 `Update()` 中下列六段完整实现提取为成员函数，在原位置调用；新函数定义放在 `Update()` 后、`SetConfig()` 前。
- 本文件记录方案和执行结果；不新增业务类，不拆出额外源码文件，不修改工程配置。

| 新私有函数 | 从 Update 搬入的完整代码段 |
| --- | --- |
| `ResolveInputOwnership()` | 原 `pre update` 段：画布 PreUpdate、创建模式下 Creator PreUpdate、遍历 Body PreUpdate、UI PreUpdate；到 `UpdateBasicState()` 之前为止 |
| `UpdateMusic()` | 原音乐段：检查两个音频、播放结束时重置采样并切换曲目、未静音时输出样本；不包含暂停快捷键判断 |
| `RemoveDeletedObjects()` | 将待删除刚体的连接标记删除、清除失效选择、移除连接两端的引用、依次从容器移除连接及刚体；不包含空白点击判断 |
| `PrepareSimulation()` | 计算 realDeltaTime、写入原脚本键、计算 gridNumX/gridNumY、场景变化时重新生成 groups；不包含 Body::Update 循环 |
| `AdvanceSimulation()` | 原物理模拟段，包括暂停/零时间判断、子步时间计算、整个子步循环 |
| `RenderSceneObjects()` | 原刚体和连接的两个绘制循环，保留每个对象在绘制前的几何/AABB或端点更新；不包含 Creator 和 UI 更新 |

`AdvanceSimulation()` 保持内部顺序：写入脚本 time → Body::Step → Group::Warp → 静电 → 重建网格并登记刚体 → 碰撞检测及求解 → 连接求解 → 累加 simulationTime。第一步不继续拆解内部算法。

`RenderSceneObjects()` 是过渡阶段入口，仍包含原有几何更新，不宣称它已成为纯只读渲染函数；尤其不改成先更新所有物体、再绘制所有物体。

### 3.2 提取后的主流程

```text
SimulationApp::Update()
    ResolveInputOwnership()
    UpdateBasicState()
    原作用域 Update 脚本判断与执行
    UpdateMusic()
    原空格键暂停判断
    RemoveDeletedObjects()
    原空白点击判断和 ClearSelection()
    sceneCanvas->Render(screen)
    原 DrawString 调试文字绘制
    PrepareSimulation()
    原 Body::Update 循环
    AdvanceSimulation()
    RenderSceneObjects()
    原创建模式判断和 Creator::Update()
    userInterface->Update()
```

暂停、空白点击、Body 更新和模块调用已经简短清楚，保留在主流程，不为每几行再增加包装函数。

### 3.3 明确不改的内容

- 构造函数、Save/Load/Reset、ClearSelection、SetConfig、DetectCollisions、参数存储、InitializeDefaultFunctions 和 UpdateBasicState 的实现不变。
- 不改 Body、Creator、UI、SceneCanvas、App::Run 或底层绘图函数。
- 数据成员、所有权、公开接口和已有访问权限不变；六个新函数非虚、私有、无参，不引入继承或模板。
- 物理暂停条件、子步数、变量初始化位置、条件和循环边界不变。
- `UpdateBasicState()` 内部和外层均存在脚本 Update 检查与调用，第一步保留两处及其间的参数同步时序，不合并或修复。
- 保留提取代码的现有注释和控制流结构，不夹带全文件格式清理。

## 4. 第一步验证与检查入口

自动检查：

1. 修改前后执行主工程 `Debug|x64` 构建。
2. 逐个比较六个新函数与原代码段；将六个调用展开回函数体后，与原 `Update()` 对比，检查代码和注释顺序保持一致。
3. 检查类的原成员和公开声明保持不变，六个声明仅位于末尾 private 区域；其他成员函数实现不变。
4. 运行已有绘制回归测试作为补充，不把它当作帧调度的行为验证。
5. 检查 UTF-8 BOM、CRLF、空格缩进和 `git diff --check`；核对既有未提交内容的哈希未变。

供用户检查的运行场景：暂停/恢复、选择/空白取消选择、拖拽和创建、删除带连接的刚体、显示及调试文字、静音/音乐切换、脚本场景。不能把源码展开对比和构建通过等同于完整 GUI 验收。

## 5. 第二步：提取 PhysicsWorld

### 5.1 类型、成员和文件

新增 `Rigid2DSimulation/current/rigid/PhysicsWorld.h/.cpp`，类型为 `class PhysicsWorld`。`SimulationApp` 按值持有公开成员 `PhysicsWorld physicsWorld`，不保留旧成员别名。工程及 filters 文件登记新文件。

| 原 SimulationApp 成员 | 新归属 | 访问方式 |
| --- | --- | --- |
| `bodies`、`connections` | PhysicsWorld 公开成员 | 现有创建、UI、脚本和存储代码改为 `simulationApp.physicsWorld.bodies/connections`，暂不同时引入对象句柄或命令系统 |
| `gravity`、`coulombConstant`、`hasElectrostatic`、`simulationTime`、`stepNum`、`rectScene`、`parallelEpsilon`、`useEqualPositionCorrection`、`gridSize`、`isSceneChanged` | PhysicsWorld 公开成员 | 配置、创建工具及 UI 的现有读写点直接迁移，名称和赋值条件不变 |
| `groups`、`collisions`、`gridNumX`、`gridNumY`、`multiGridBodies`、`grid` | PhysicsWorld 私有成员 | 仅物理世界实现使用，不向 App/UI 暴露缓存 |
| `maxRealDeltaTime`、`realDeltaTime`、`isPaused` | 保留 SimulationApp | 应用决定本帧时间和是否推进 |
| `velocityAngularDrag`、选择、模式、Creator、显示参数、音乐和 DSL 成员 | 保留 SimulationApp | 分别属于后续编辑、渲染、脚本步骤，不误当物理世界参数 |

### 5.2 函数迁移与接口

| 新接口 | 搬入内容与调用位置 |
| --- | --- |
| `ClearObjects()` | 原 Reset 中依次清空 bodies、groups、collisions、connections；仍在清空 debugString 前调用 |
| `ResetParameters(DRect const& sceneRect)` | 原 Reset 的物理参数默认值和场景范围赋值；在 ClearSelection 后调用。保留未重置网格缓存的原行为，不顺手修复 |
| `MarkDeletedConnections()` | 原 RemoveDeletedObjects 中给被删除刚体的连接打标记的循环 |
| `RemoveDeletedObjects()` | 原连接两端引用清理、先移除连接再移除刚体的代码 |
| `Prepare()` | 原 PrepareSimulation 中网格尺寸计算、isSceneChanged 清除和 FormGroups 调用；仍在逐 Body::Update 前运行，暂停时也运行 |
| `Advance(double deltaTime, std::function<void(double)> const& beginStep, std::function<void(Body&, double)> const& stepBody)` | 原 AdvanceSimulation 的步长、整个子步循环、分组越界、静电、网格、碰撞与连接求解、时间累加；暂停和零时间判断留在 App |
| 私有 `RegisterBody(Body& body)` | 原 Body::RegisterGrid，转为物理世界维护自己的网格；Body 移除该声明和定义 |
| 私有 `DetectCollisions()` | 原 SimulationApp::DetectCollisions，算法及邻格遍历顺序不变；App 移除该声明和定义 |

清理选择仍由 `SimulationApp::RemoveDeletedObjects()` 协调：调用 `MarkDeletedConnections()` → 原两个选择指针检查与清空 → 调用世界的 `RemoveDeletedObjects()`。不在世界中保存选择指针，也不延后到对象销毁后才解引用检查。

### 5.3 逐子步的过渡接口

现有 `Body::Step()` 在运动积分内部调用脚本预设，随后处理拖拽并更新 AABB。第二步不提前实施第三、第五步，不改变这些调用的次数和位置。因此两个回调是针对现存耦合的同步过渡接口，不是新事件系统：

- `beginStep(time)`：每个子步开始时由 App 写入原 `currentScope[L"time"]`。
- `stepBody(body, stepDeltaTime)`：物理世界按原 bodies 顺序调用；App 回调只调用原 `body.Step(*this, stepDeltaTime)`，Body 的物理参数读取改指向 physicsWorld。
- 回调只在 Advance 返回前执行，不保存、不异步、不允许空回调；不复制刚体列表或预先计算脚本结果。
- PhysicsWorld 的声明和实现均不依赖 SimulationApp、输入、UI 或 DSL 类型，不接收或存储整个 App 引用。世界实际持有物理状态并实现多体求解，不能只把原 App 函数包一层。
- 本步并不宣称 Body 已成为纯物理对象。第三步移出 Body 的拖拽处理，第五步移出预设脚本执行时，应一并收敛这个整段 Body::Step 回调；不得将过渡接口误认为最终模型。

App 的 AdvanceSimulation 最终只有原暂停/零时间条件和一次 `physicsWorld.Advance(...)` 调用。子步顺序保持：beginStep → 逐 Body stepBody → Group::Warp → Electrostatic → 网格重建 → 碰撞检测/求解 → 连接求解 → simulationTime 累加。

### 5.4 其他调用点与不变项

- `SimulationApp.cpp/.h`：迁移成员、函数，更新 Save/Load、Reset、配置、遍历、DSL GetBodyCount 的访问点。存储仍留在 App，二进制顺序、DSL 字符串键不变。
- `Body.cpp/.h`：移除 RegisterGrid；重力与预设脚本时间改读 physicsWorld，其余运动、脚本和拖拽实现不变。
- `Connection.cpp`：保存/加载时刚体索引改从 physicsWorld.bodies 查找，索引算法和文件布局不变。
- `SceneBuilders.cpp`、`Creator.cpp`：容器与 isSceneChanged 访问点迁移；不新增缺失的脏标记，不改变创建时机。
- `ui/bodyPanel/BodyButtons.cpp`、`ui/connectionPanel/ConnectionButtons.cpp`：isSceneChanged 访问点迁移。
- `ui/globalPanel/GlobalControls.cpp`、`ui/bottomPanel/BottomControls.cpp`：显示值从物理世界读取，本地化键不变。
- 不修改其他物理算法、绘制函数或现有未提交文件。不清理已发现的旧边界问题，例如空容器的旧循环边界、删除后分组刷新的旧触发条件。

### 5.5 验证范围

1. 主工程 Debug|x64 构建；核对所有迁移成员访问点，无旧公开入口或兼容别名。
2. 逐段核对新旧求解、网格登记、删除逻辑；只允许归属/限定名和上述同步回调替换，保持公式、条件和调用顺序。
3. 本地补充物理世界回归检查，覆盖子步次数/时间、静电、碰撞、连接、分组以及关联删除；测试文件不提交，不引入生产测试专用 API。
4. 保留既有绘制回归检查；核对编码、CRLF、diff 和原有未提交文件未变。
5. GUI 拖拽、脚本预设、创建删除、暂停/恢复和存储往返仍需用户检查；不得用构建或局部测试冒充完整交互验收。

## 6. 第三步：提取 SceneEditor

### 6.1 状态和目录

新增 `Rigid2DSimulation/current/editor/SceneEditor.h/.cpp`，类型为 `class SceneEditor`；`SimulationApp` 按值持有 `SceneEditor editor`。将 `current/rigid/Creator.h/.cpp` 整体迁至 `current/editor/`，工程四种配置增加该目录的包含路径，filters 同步设置。旧路径不保留兼容文件。

| 原位置 | 新位置/本步处理 |
| --- | --- |
| App 的 `mode`、`creator`、`bodySelecting`、`connectionSelecting`、`velocityAngularDrag` | SceneEditor 同名公开成员；所有调用点迁为 `simulationApp.editor.*` |
| App.h 中四个 `k*Mode` 交互常量 | SceneEditor.h；显示模式常量仍在 App.h |
| Body 的悬停、选择、删除、拖拽及创建连接端点处理 | SceneEditor 实现，不再由 Body 读取输入或修改选择 |
| Body 的 `hovered`、`dragged`、`radianDrag`、`velocityAngularDrag`、`relativeDragPoint` | 暂保留为逐刚体交互数据，由 SceneEditor 更新；原渲染和 Group::Warp 仍读取这些值，不在本步引入外部状态表、对象句柄或改变物理依赖 |
| Body 的轨迹记录 | 随原逐刚体 Update 迁入 SceneEditor，仍在原先时点记录；轨迹容器暂留 Body 供绘制使用 |
| Creator 的参数、活动状态及预览对象 | 仍由 Creator 持有，所有者改为 SceneEditor；不拆成多个工具子类 |

### 6.2 函数与接口迁移

SceneEditor 不持有、不接收 `SimulationApp&`，仅接收明确的物理世界、输入及必要帧参数。不引入事件总线、统一上下文或延迟命令队列。

| 新接口 | 来源和行为 |
| --- | --- |
| `ClearSelection()` | 原 App::ClearSelection，移除旧入口；空白点击判断仍由 App/SceneCanvas 协调 |
| `ClearDeletedSelection()` | 原 App::RemoveDeletedObjects 的两个选择指针检查，仍位于世界标记关联连接与销毁对象之间 |
| `ResetTools()` | 设置原拖拽角速度默认值 2 并重新创建 Creator；不重置 mode，保持原 Reset 行为；选择清空仍在 Reset 原位置调用 |
| `CycleMode(InputOwners&)` | 原菜单按钮的 Creator::Discard 和 `(mode + 1) % 4`，保持先取消工具再切模式 |
| `RequestBodyDeletion(PhysicsWorld&)`、`RequestConnectionDeletion(PhysicsWorld&)` | 原两个删除按钮的选中对象删除标记及 isSceneChanged 设置；不增添空指针容错或改变触发条件 |
| `PreUpdate(PhysicsWorld const&, AppInput const&, InputOwners&, DRect)` | 原创建模式 Creator::PreUpdate，随后原逐 Body::PreUpdate；保持深度平局规则和顺序，UI 仍最后争取输入所有权 |
| `UpdateBodies(PhysicsWorld&, AppInput&, InputOwners const&, double realDeltaTime, bool isTrackShown, bool isPaused)` | 原逐 Body::Update，完整保留暂停时交互、拖拽释放帧处理、轨迹和创建端点拾取顺序 |
| `ApplyDrag(Body&, AppInput const&, InputOwners const&, double realDeltaTime, double stepDeltaTime)` | 原 Body::Step 内拖拽段，在每个刚体的运动/预设之后、AABB 更新之前执行 |
| 私有 `UpdateBody`、`UpdateDragAngle`、`HandleDragWhole`、`HandleDragPoint`、`HandleDragForce` | 从 Body 搬入，显式接收 Body 和所需输入/时间；物理公式、Q/E 优先级和 F 分支不变 |

`Body::GetDepth()` 改为公开 const 查询，供悬停判断复用，避免复制深度算法。移除 Body 的 Update、PreUpdate 和四个拖拽函数声明/定义。`Body::Step()` 仅保留原运动及脚本预设段；其后拖拽和几何更新由 App 的原 stepBody 回调依次调用：

```text
body.Step(app, stepDeltaTime)
editor.ApplyDrag(body, input, inputOwners, realDeltaTime, stepDeltaTime)
body.UpdateShapesPositionAndAABB()
```

这仍是同步过渡回调，不声称 Body 已完全与 App 解耦：预设脚本与存储、渲染依赖分别留待第五、第四步。不能把逐子步拖拽降为每帧一次。

### 6.3 Creator 的依赖与预览

- `Creator::Update(PhysicsWorld&, AppInput&, InputOwners const&)` 只处理创建逻辑；七个私有 Update* 函数改收 `PhysicsWorld&`、`AppInput&`。状态、配置读取、随机颜色生成、插入容器与脏标记的原顺序不变。
- `Creator::PreUpdate(AppInput const&, InputOwners&, DRect)` 和 `Discard(InputOwners&)` 不再接收 App。
- 原 Update 末尾的绘制段原样提为 `RenderPreview(SimulationApp&)`，暂允许这个明确的渲染入口依赖 App。App 在原创建模式分支内依次调用 Creator::Update 和 RenderPreview；两者之间不插入其他工作。
- 此次预览提取仅为创建输入/世界操作摆脱 App 所必需，不迁移其他渲染、不重排绘制，也不提前改 UI 的回调模型。连接预览端点更新仍在原绘制前位置。

### 6.4 App、UI 与其他调用点

- App 的输入阶段改为画布 PreUpdate → editor.PreUpdate → UI PreUpdate；物理推进前的逐 Body::Update 循环改为 editor.UpdateBodies。
- 删除流程仍为 world.MarkDeletedConnections → editor.ClearDeletedSelection → world.RemoveDeletedObjects；App 只协调跨模块顺序。
- Reset 的选择清空、工具重置分别在原位置调用编辑器；DSL SetCreator 注册不移走，只把赋值目标迁到 editor.creator，脚本键不变。
- Body::Render、UI 的 BodyControls、ConnectionControls、BodyButtons、ConnectionButtons、GlobalControls、MenuButtons 读取新选择/模式/工具路径；删除按钮、模式按钮委托上述编辑器接口。UI 布局、脚本应用按钮、物理标记规则不变。
- 不顺手修复创建中删除目标、旧裸指针生命周期、拖拽除零、删除模式未设置场景变化标记等旧问题。渲染用数据留 Body 是本步明确过渡，不用“已完全解耦”描述结果。

### 6.5 验证与停止点

1. 对照 `13ba108` 检查每段迁移实现及中文注释，除对象限定名、显式参数和上述入口拆分外不改变逻辑。
2. Debug|x64 构建；检查旧交互成员和 Body 输入入口无遗留、Creator 工程路径全部更新。
3. 在已忽略的 x64 目录补充本地无窗口交互检查，覆盖深度拾取、选择清除、删除请求、模式取消、拖拽分支与子步顺序、创建工具。测试不提交，不重新创建已按用户要求删除的 tests 文件。
4. 检查 UTF-8 BOM、CRLF、注释无损、diff 和原有未提交文件哈希；GUI 拖拽、脚本预设、创建预览等仍列入用户验收，不用局部测试冒充完整验收。
5. 完成后停止，等待用户检查；确认后再提交本步代码与文档并开始第四步。

## 7. 第四步：集中场景渲染，保留交错时序

### 7.1 类型和状态

新增 `Rigid2DSimulation/current/render/SceneRenderer.h/.cpp`，类型为 `class SceneRenderer`；App 按值持有 `SceneRenderer sceneRenderer`。工程全部配置增加 current/render 包含目录，filters 登记该目录。

- App 的 `display`、`energyMultiplier`、`chargeMultiplier` 迁为 SceneRenderer 同名公开成员；四个 `k*Display` 常量迁至其头文件。Reset 仍只重置两个倍率，不重置显示模式；DSL 配置键和菜单按钮行为不变。
- 同头文件定义 `SceneRenderContext`：`Tile& screen`、`DepthBuffer& depthBuffer`、`Font const& font`、`DRect viewport`、`DVector2 mousePosition`、`Body const* selectedBody`、`Connection const* selectedConnection`。只表示一次绘制所需输出目标和只读显示输入，不含 App、物理参数、DSL 或完整输入对象。
- App 私有 `GetSceneRenderContext()` 在每个绘制阶段调用前生成该对象，不跨帧保存，也不在帧首缓存选择，以免交互后还使用旧选择。Renderer 不保存 context 或对象指针，不依赖 SimulationApp 类型。
- `isTrackShown` 留在 App 供轨迹采集使用，不把轨迹采集改成绘制时执行。Body/Connection 的颜色、深度、几何和轨迹数据暂留原类型，不扩展成新的外观对象系统，也不改存储布局。

### 7.2 函数映射

以下 SceneRenderer 成员均为 const 函数：

| 新接口 | 搬入内容 |
| --- | --- |
| `RenderBackground(SceneCanvas const&, std::wstring const& debugString, SceneRenderContext const&)` | 原 App 的画布 Render 及紧随的调试文字 DrawString；Canvas 保留自己的底图 Render，不搬走输入区域职责 |
| `RenderObjects(PhysicsWorld&, SceneRenderContext const&)` | 原 App::RenderSceneObjects 的逐刚体几何更新/绘制，然后逐连接端点更新/绘制；删除旧 App 包装函数 |
| `RenderCreatorPreview(Creator&, SceneRenderContext const&)` | 原 Creator::RenderPreview；保留连接预览端点更新，再绘制，不重新调用创建逻辑 |
| `RenderCollision(Collision const&, SceneRenderContext const&)` | 原 Collision::Render 的接触点绘制；只迁移入口，不启用原本没有调用的调试显示 |
| 私有 `RenderBody(Body const&, SceneRenderContext const&)` | 原 Body::Render 的颜色选择、轮廓、填充、拖拽线和轨迹绘制，规则和优先级不变 |
| 私有 `RenderConnection(Connection const&, SceneRenderContext const&)` | 原 Connection::Render 的颜色规则与线段绘制 |

移除 Body、Connection、Collision 的 Render 声明/定义和不再需要的绘图包含；Creator 删除 RenderPreview 及 SimulationApp 依赖，并直接包含创建逻辑需要的 AppItem/PhysicsWorld 等头文件。Body/Connection 仍因脚本或存储依赖 App，不宣称全库完全解耦。

RenderObjects 与 RenderCreatorPreview 明确接收可变世界/工具，因为本步仍在原位置更新几何/端点。仅 RenderBody/RenderConnection 只读对象。物理子步内的 AABB 更新不删除，不把所有几何更新集中提前，也不强称整个 Renderer 无状态副作用。

### 7.3 UI 的实际边界

审查发现基础控件已经有 Render/RenderMain/RenderCursor 等独立绘制函数：按钮在回调后立即绘制，Panel 先画底板再更新子控件，FloatPanel 按自身状态决定是否继续绘制。为了保留现有可见行为，不能简单改成“更新所有控件，再统一绘制”。

本步不再次拆 UI 文件，不改 framework/control 内已有函数；`UserInterface::Update` 重命名为 `UpdateAndRender`，明确当前仍是交错入口，其内部调用顺序和实现不变。场景 Renderer 不接管 UI 控件树。UI 全局更新/绘制解交错需另行确定回调生效和布局规则，不是此步骤完成的承诺；本步完成的是场景渲染职责迁移及显式标明 UI 的保留边界。

### 7.4 帧流程和改动范围

```text
原输入、脚本、音乐、暂停与删除/选择阶段
sceneRenderer.RenderBackground(..., GetSceneRenderContext())
原 PrepareSimulation、editor.UpdateBodies、AdvanceSimulation
sceneRenderer.RenderObjects(physicsWorld, GetSceneRenderContext())
若创建模式：creator.Update(...) → sceneRenderer.RenderCreatorPreview(..., GetSceneRenderContext())
userInterface.UpdateAndRender(app)
```

修改 App、Body、Connection、Collision、Creator 的 .h/.cpp，UserInterface 的 .h/.cpp，MenuButtons.cpp、工程和本方案；新增 SceneRenderer 两文件。SceneCanvas、framework 绘图算法、物理解算、编辑交互实现、存储、脚本字符串及原有未提交文件均不修改。不修复 Body 原显示分支中的不可达代码或既有物理注释乱码。

### 7.5 验证及停止点

1. 对照第三步提交，逐段核对搬迁实现、调用时序和参数替换；检查没有遗留旧 Render 入口或 App 显示成员访问。
2. Debug|x64 构建；在忽略的 x64 目录建立无窗口像素对照测试，以原渲染实现为参考，对照颜色、透明度和深度缓冲。覆盖点、圆、多边形、显示模式、选择/悬停/拖拽、轨迹、各连接类型、创建预览及碰撞标记；字体/背景另设检查。
3. 重新链接交互与物理本地测试，确认几何和编辑行为不回退；不恢复 tests 目录文件，不提交本地测试。
4. 检查 UTF-8 BOM、CRLF、注释、diff 及原有未提交文件哈希，独立审查规范和方案一致性。测试不代替 GUI/音乐/脚本场景验收。
5. 完成后停下等待用户确认，不提交第四步、不开始第五步。

## 8. 进度

| 步骤 | 状态 |
| --- | --- |
| 1. 整理帧流程 | 用户已于 2026-10-05 确认，已提交 `362956d` |
| 2. 提取物理世界 | 用户已于 2026-10-05 确认，已提交 `13ba108` |
| 3. 提取编辑交互 | 用户已于 2026-10-05 确认，已提交 `6542dde` |
| 4. 整理渲染职责 | 用户已确认，代码与本方案一起提交 |
| 5. 移出脚本和存储 | 未开始 |

第一步验证记录（2026-10-05）：

- 修改前后主工程 `Debug|x64` 构建均通过，已有 C4244/C4267 数值转换警告未在本步处理。
- 六个新函数与原代码段逐段一致；展开六个调用后，整个 `.cpp` 与修改前一致，仅忽略空行，包含原注释和代码顺序。
- 头文件仅增加末尾 private 区域及六个声明，原成员和访问权限不变。
- 已有 12 项三角形绘制回归检查通过；这是补充检查，不替代帧交互验收。
- UTF-8 BOM、CRLF、空格及 `git diff --check` 检查通过。原有未提交文件和测试经 SHA256 核对均未变化。
- 两项独立只读审查已完成：Standards 未发现本次引入的规范问题；Spec 未发现迁移映射、作用域、生命周期或时序偏差。
- 代理未执行 GUI 人工回归；用户已明确确认第一步代码和方案，不额外推断其具体人工测试范围。

第一步已按用户确认提交两个源码文件和本文件（`362956d`）。第二步也已获用户确认，代码与本文更新一起提交；下一步为第三步编辑交互职责迁移。

第二步验证记录（2026-10-05）：

- 主工程 `Debug|x64` 构建通过。首次构建前新文件未补 BOM，造成默认 GBK 解码错误；已补齐 UTF-8 BOM 并重新构建通过。原有 C4244/C4267 转换警告不在本步修复。
- 8 个外部调用点源码文件与修改前逐文件比较，除物理成员访问路径迁移及 Body::RegisterGrid 搬出外一致。6 段迁移实现核对通过，差异限于明确的归属/回调替换、为旧无括号循环补齐括号及删除两行失效的注释代码；子步和邻格遍历顺序不变。
- 本地 `x64/physics-world-check/` 保存来自第一步基线的原求解流程对照。10 个场景各运行 20 帧、每帧 4 子步，共 800 个子步；逐帧刚体位置、速度、角度、角速度、AABB、连接端点及状态与对照完全一致，回调顺序和次数一致。
- 场景包含空世界、单点、静电、圆形/矩形碰撞、刚性连接、弹簧、分组越界、多格物体及等位置校正。另有连接单独删除、带连接刚体删除及重置默认值检查，全部通过。
- 上述测试链接实际生产物理算法，但 Body 推进回调使用普通运动积分，不构造 GUI App；不覆盖脚本预设、输入拖拽或完整场景存储往返。这些仍列入用户运行检查，不宣称全功能等价已获动态验证。
- 原有 12 项三角形绘制回归检查通过。所有本次修改的 13 个 C++ 文件通过 UTF-8 BOM、CRLF 和无 Tab 检查；`git diff --check` 通过。原有未提交源码、两份文档及 tests 文件 SHA256 均未变化。
- 独立 Standards、Spec 只读审查均未发现新增问题；它们不替代运行验收。新增本地测试位于已忽略的 x64 目录，不提交，也未修改原 tests。
- 用户确认第二步后，按其要求删除未跟踪的 `tests/DrawTriangleTests.cpp` 和 `tests/DrawTriangleTests.vcxproj`；上述测试记录为删除前的验证结果，不将测试文件纳入提交。其他原有未提交内容不并入本次提交。

第三步验证记录（2026-10-05）：

- 主工程 `Debug|x64` 构建通过，保留原有数值转换警告；构建因包含目录调整重新编译了全工程，不改变其他源码。
- 本地 `x64/editor-check/` 对照 `13ba108` 的原 Body 交互实现，2048 组组合输入在帧交互后及各自 3 次拖拽子步后结果一致，共 6144 次子步。比较位置、速度、角度、角速度、拖拽状态、选择、删除标记、轨迹及创建端点；覆盖模式、悬停、按下/释放、左右鼠标键、F、Q/E 同时按下、键盘占用和固定/动态物体。
- 同深度拾取顺序、视口限制、带连接删除及选择清空、模式切换取消创建、工具重置不改模式、连接删除请求和七种 Creator 创建模式检查通过。
- 上一阶段物理世界对照测试已重新链接最新生产对象：10 个场景、800 个子步、两类关联删除和重置检查继续通过。这不替代完整帧调度与 GUI 验证。
- `Body::Step → ApplyDrag → UpdateShapesPositionAndAABB` 和 `Creator::Update → RenderPreview` 的原位置相邻调用经静态核对；脚本预设、实际窗口输入和预览像素结果未做完整动态验收，仍需用户运行检查。
- 14 个新增/迁移/修改的 C++ 文件通过 UTF-8 BOM、CRLF、无 Tab 检查；Body 中文注释逐项保留在 Body 或 SceneEditor，未出现乱码。旧 App 交互成员访问、Body 输入入口及 rigid/Creator 工程引用无残留；`git diff --check` 通过。
- Standards 与 Spec 两项独立只读审查均未发现本次新增问题。原有 SceneParameter.cpp、rigid 审计和 UI 回调文档的 SHA256 未变化；没有恢复已删除的 tests 文件，本地测试保持忽略、不提交。

第三步已获用户确认，代码与本文一起提交；随后在同轮开始第四步的详细设计与实施，第四步完成后再次等待用户检查。

第四步验证记录（2026-10-05）：

- 第三步已提交为 `6542dde`。第四步主工程 `Debug|x64` 构建通过；首次构建发现迁移时漏带 DrawCurve.h，导致 DrawEllipseBorder 未声明，已补齐并重建通过。未修改底层绘图算法。
- 本地 `x64/renderer-check/` 保留第三步基线的原绘制实现作对照。960 组场景分别在背景/调试文字、物体/连接、创建预览、碰撞标记四个阶段比较全部颜色、透明度和深度缓冲，均完全一致，预览端点也一致。覆盖四种显示模式、点/圆/旋转矩形、16 组悬停/拖拽/选择组合、五个位置及视口裁剪、轨迹、四类连接和紧绷状态。
- 字体对照使用本地构造的小型字形，包括缺字和换行，不依赖窗口或字体资源加载；点物体能量测试使用有效面积/惯量，未将原有无效数值路径当作本次修复或验收范围。没有做完整 GUI/脚本场景验收。
- 本地交互与物理测试已重新链接最新生产对象：2048 组输入、6144 次拖拽子步、选择/删除/模式及七种创建工具检查通过；10 个物理场景、800 个子步、关联删除和重置检查通过。
- Body、Connection、Collision、Creator 除绘制搬出及直接包含调整外，其余实现与第三步基线一致；UserInterface 除入口重命名外实现完全一致。旧 Render 入口、App 显示成员访问及旧 UI 总入口没有残留。
- Standards 审查指出 SceneRenderContext 缺少 DVector2.h 的直接包含，已修正并检查；Spec 审查未发现新增时序或行为偏差。UI 全局分阶段调度并未实现，仍按第7.3节保留交错语义。
- 15 个相关 C++ 文件通过 UTF-8 BOM、CRLF、无 Tab 检查，`git diff --check` 通过。原有 SceneParameter.cpp、rigid 审计和 UI 回调文档的 SHA256 未变化。测试仍位于已忽略的 x64 下，不提交，不恢复 tests 目录文件。

第四步已获用户确认，代码与本文一起提交；随后开始第五步的详细设计与实施，完成后再次等待用户检查。
