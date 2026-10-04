# Body 数据职责整理方案

## 1. 定位与当前状态

- 日期：2026-10-05；源码基线：`8b143e7`。
- 本文承接 `docs/simulation-app-refactor-plan.md` 已完成的五步工作，记录下一阶段的设计方向和可先行工作，不重新打开已完成步骤。
- 用户已认可“区分物理刚体与场景物体，以普通组合逐步分离数据职责”的方向，并已授权执行下文第一步三项工作。完成后等待用户检查，确认后再将代码和本文一起提交。
- 下文的第一步实施映射已获执行确认。第二步所有权方案仍须另行讨论，不将第一步授权视为第二步细节也已确认。
- 本文是可更新的重构方案，不是将待定设计记为历史决策的 ADR。保留此前审计文档，不覆盖其历史记录；当前事实以本基线源码为准。
- 所有源码路径均相对仓库根目录；不修改原有无关工作区内容。

## 2. 问题与目标

App 重构已分离物理推进、编辑交互、绘制、脚本执行及场景存储调度，但 `Rigid2DSimulation/current/rigid/Body.h` 仍混合这些模块的数据。行为迁出不等于数据归属已清晰。

| 数据类别 | 当前字段 | 判断 |
| --- | --- | --- |
| 物理状态 | position、velocity、radian、velocityAngular、fixed、质量/惯量、阻尼、摩擦、弹性、电荷等 | 保留在物理 Body，不为缩短类型再次拆成大量小对象 |
| 几何与物理关联 | shapes、box、transform、connections、point | 第一阶段保留；渲染可以读取物理几何，不复制一份位置或形状 |
| 外观及拾取参数 | innerColor、borderColor、hoveredColor、draggedColor、selectedColor、selectedConnectionColor、depthShape、depthPoint、pointRadius | 可以先归为具名外观数据，暂不改变拥有者及拾取规则 |
| 脚本数据 | scriptText、temporaryScriptText、positionProgram、angleProgram、angularVelocityProgram、三个 has*Preset 开关 | 可以先归组；编辑文本和运行程序的生命周期暂不进一步拆开 |
| 交互过程 | hovered、dragged、radianDrag、velocityAngularDrag、relativeDragPoint | 长期由编辑职责管理；迁出前必须确认现有交互语义和对象清理规则 |
| 轨迹 | track | 显示辅助数据，采样及清理有明确时序，不能仅按类型机械搬迁 |
| 算法临时状态 | visited | 当前用于分组遍历，不是外观/交互状态；第一阶段不修改遍历算法或容器 |
| 删除状态 | isMarkedForDeletion | 涉及延迟删除、连接和选择清理，第一阶段保持现状 |

Body 还通过 ReadConfig(Value const&) 理解 DSL，通过文件构造函数和 Save 理解二进制格式。处理这些实现可以早于完整场景对象建模。

长期目标是让 Body 表示物理刚体，场景层组合物理、外观和脚本数据；编辑器管理操作过程。采用普通组合，不为此引入 ECS、组件注册框架、通用事件总线或额外管理器。

```text
场景物体（SceneBody 为候选名）
    Body：物理状态、几何、物理计算
    Appearance：场景物体的外观参数
    ScriptData：物体脚本与预设数据

SceneEditor：交互过程和轨迹记录
SceneRenderer：读取物理几何、外观及必要的交互状态
SceneStorage：场景记录读写
```

这是目标职责示意，不是第一步必须建立的类树。处理数据的模块不必拥有数据，例如 Renderer 不应仅因为读取颜色，就被规定为所有物体外观数据的所有者。

## 3. 必须保留的跨模块规则

1. SceneEditor 写入 dragged，SceneRenderer 据此选择颜色并绘制拖拽线；Group::Warp 还据此禁止整组越界平移。
2. SceneEditor 依照选择、轨迹开关和暂停状态记录/清空 track；Group::Warp 在平移后也会清空轨迹。保留记录位置与清理位置，不改为帧末统一处理。
3. pointRadius 同时用于点物体显示和拾取，depthShape/depthPoint 同时影响显示及悬停竞争。字段归组不能悄悄分离这两套参数。
4. 运动预设在物理子步内执行。保留分支选择、调用次数、动态/固定分支的位置，以及对当前世界时间的逐次读取。
5. 物体读取顺序仍是读取记录、执行物体脚本、加入世界。脚本执行时该新物体尚未加入世界。
6. 物体记录同时保存物理数据、脚本文本和 innerColor。拆分 C++ 字段不等于修改文件格式，也不自动把新增分组里的全部字段保存下来。

## 4. 第一步：不调整所有权的准备工作

本步不引入 SceneBody、不新增对象 ID、不更改 PhysicsWorld 的对象容器、不改变 Body 的地址和现有引用关系。以下三项已获用户确认，可以按同一个已确定方向实施，不必先解决第二步的所有设计问题。

### 4.1 外观与脚本数据归组

在 `Rigid2DSimulation/current/rigid/Body.h` 中定义两个具名数据结构，再由 Body 按值持有。不为过渡阶段提前新增目录；这是暂时的数据分组，不是最终分层。

| 新类型 / Body 成员 | 搬入字段 | 调用点变化 |
| --- | --- | --- |
| BodyAppearance / appearance | 六个颜色字段、depthShape、depthPoint、pointRadius | body.innerColor 等变为 body.appearance.innerColor；字段原名保持不变 |
| BodyScriptData / scriptData | 两个文本、三个程序指针、三个预设开关 | body.scriptText 等变为 body.scriptData.scriptText；不复制程序或改变 shared_ptr 语义 |

- 外观默认值以原 Body 构造完成后的值为准，例如 depthShape=0、depthPoint=1、pointRadius=8；不是机械采用原成员声明中的零值。没有显式赋值的字段保持其原默认构造语义。
- 同步 Body 实现、SceneRenderer、SceneScriptRuntime、Creator、SceneBuilders、BodyPanel 相关按钮/控件及存储访问点；不依靠兼容别名维持旧路径。
- hovered、dragged、relativeDragPoint 等交互状态不放进 appearance；track 也不放进外观配置。
- GetDepth、IsInside 暂留原位置，只改内部成员访问；不重写拾取算法。
- 归组后 Body 仍持有非物理数据，仍依赖 Color 和 Program。不能将这一步描述为“纯物理 Body 已完成”。

### 4.2 将 DSL 配置解释移出 Body

| 原实现 | 建议新位置和接口 |
| --- | --- |
| Body::ReadConfig(Value const& config) | 新增 `Rigid2DSimulation/current/script/BodyConfig.h/.cpp`，普通函数 `ApplyBodyConfig(Body& body, Value const& config)` |

- 不要求持有 SceneScriptRuntime 或 SimulationApp；此函数只把已有 DSL 配置应用到指定物体。
- 更新 SceneScriptRuntime、Creator、SceneBuilders 三处源码文件中的所有调用点；删除旧 Body 成员函数及不再需要的 Value 前置声明。
- 保留配置键、缺省字段处理、字段写入顺序和颜色转换方式，不新增中间配置格式。
- 不将 Init、RefreshBodyScript 或运动预设求值塞入此函数；调用方原先何时初始化、是否 resetPosition，继续保持。
- 本项只迁移 Body 的配置解释；不顺手调整 Connection::ReadConfig、ShapeBuiltins 或全部场景构建文件的归属。

这项有明确归属且无需新所有权模型，属于设计决策少、调用点替换较多的工作。

### 4.3 将 Body 记录编解码移入现有 SceneStorage

源码中 Body 的文件构造和 Save 当前均由 SceneStorage 调用，可以在不新增存储框架的情况下继续集中。

| 原入口 | 建议替代入口 |
| --- | --- |
| Body::Save(FILE*) const | SceneStorage 私有静态函数 SaveBody(FILE*, Body const&) |
| Body(FILE*) | SceneStorage 私有静态函数 LoadBody(FILE*)，返回 std::shared_ptr<Body> |

- LoadBody 先按原默认构造路径创建 Body，再按原顺序读入字段，不在其中执行脚本、加入世界或额外 Init。
- SceneStorage::Load 继续在 LoadBody 返回后调用 RefreshBodyScript，然后 push_back；不改变对象脚本可观察到的世界状态。
- 保留所有字段的写入顺序、类型、数量、字符串长度表示以及形状读写顺序；外观分组仅改变 innerColor 的 C++ 访问路径。
- 不把结构体直接按内存块写盘；继续使用当前逐字段编解码，不引入序列化宏或模板包装。
- 暂不迁移 Shape/Connection 的记录编解码，不宣称全部持久化已经从 rigid 移出。
- 不同时修复空字符串、圆形空顶点数组或损坏文件处理。相关已知限制须在验证结果中明确保留。

这项设计选择少，但二进制和加载时序验证要求高；不能因为是搬迁就省略对照测试。

### 4.4 建议执行顺序与范围

先做字段归组，随后迁出配置解释和 Body 记录编解码。三项共同构成“第一步：Body 数据职责准备”，不按每个字段、每个调用点拆提交。本轮完成后等待用户检查，确认后将三项代码及本文一起提交，不自行进入第二步。

第一步不需要提前决定 SceneBody 的最终目录、唯一拥有者、对象 ID、注册接口或跨模块通知机制。实现时若发现必须触碰这些问题，应停下来讨论，不借此扩展范围。

## 5. 第二步：需要深入设计后才能开始的工作

| 待定问题 | 为什么不能直接搬字段 |
| --- | --- |
| 场景物体与物理 Body 的身份关系 | UI、连接、分组、创建预览目前持有 Body 指针，需要明确新场景对象如何关联，不能随意引入两套身份 |
| 唯一所有者与注册关系 | 若由场景拥有 Body，PhysicsWorld 的登记和引用有效期需要调整；若保留物理世界拥有权，场景附属数据的关联与清理也须明确，不能两边各自释放 |
| 创建、删除、加载、重置入口 | Creator、DSL 构建和 Storage 均会创建对象；必须保证物理对象及附属数据同步建立、同步清理，保持延迟删除时机 |
| 拖拽状态的数量及存放方式 | 先确认现有交互语义，再决定单个拖拽会话还是按物体保存，不能未经验证假设永远只有一个 dragged 对象 |
| 越界平移与交互/轨迹的协调 | Group::Warp 同时读取 dragged、清 track，需要明确输入和结果由谁传递，不应反过来让物理模块依赖整个编辑器 |
| 外观参数与编辑高亮样式 | 是否将所有物体的高亮颜色统一成编辑器样式属于另外的行为决策，第一步保留逐物体配置 |
| 脚本草稿、有效文本、程序的生命周期 | 当前它们与物体共存；迁出后必须确认应用脚本、取消编辑、删除和重载的处理，不先引入全局指针映射 |

上述决定确定后，才建立实际的场景组合类型并迁移所有权。无需为了本方案改成 ECS，也不因数据分组就立刻给每个结构体增加一套管理器。

## 6. 验证与完成标准

- 第一阶段代码遵循 UTF-8 BOM、CRLF、4 空格及现有命名规范；数学变量与物理公式不更名、不修改。
- 比较归组前后的构造默认值、渲染输出、拾取优先级、拖拽及轨迹行为，确认成员路径变化未改变规则。
- 配置迁移比较相同 Value 输入后的物体状态，覆盖缺省字段和颜色；原 Init 时机保持不变。
- 存储迁移比较保存字节、重载状态和再次保存字节，检查执行物体脚本时的新物体尚未入队。已知无效输入/空顶点断言不能伪装成验证通过。
- 复用既有物理、交互、渲染、脚本/存储本地对照测试，测试仍不纳入提交；GUI 验收与无窗口测试分别记录。
- 第一阶段完成表示分组清晰，Body 不再直接解释 Value 或读写 FILE；不表示其已经不依赖外观和脚本数据。
- 第二阶段完成才讨论物理 Body 是否已真正不依赖编辑、显示和语言运行时。判断依据是类型依赖、数据所有权和生命周期规则，而不是 Body.h 的行数。

## 7. 进度

| 项目 | 状态 |
| --- | --- |
| 长期方向 | 用户已认可普通组合、区分物理刚体与场景物体的方向 |
| 本文 | 随第一步代码一起提交 |
| 第一步三项准备工作 | 已实现并完成下述验证，用户已确认；代码与本文一起提交 |
| 第二步数据所有权迁移 | 待设计，未开始 |

## 8. 第一步验证记录

验证日期：2026-10-05。

- 9 个外观字段和 8 个脚本字段已按第4节归组；配置函数及全部调用点已迁移；Body 的文件构造和 Save 已删除，SceneStorage 在原调用位置使用私有 LoadBody/SaveBody。新 BodyConfig 两文件已登记到工程和 filters。
- 主工程 Debug|x64 构建通过。没有修改物理公式、DSL 键、所有权、连接脚本数据、拖拽、分组或越界处理；现有数值转换警告不在本轮处理。
- 本地 `x64/body-data-check/` 保存修改前的完整平铺 Body 类型和实现作为独立基线，仅更换测试类型/符号名，未把基线字段一同改成分组结构。5 类构造、1280 组配置输入、点物体拾取及程序 shared_ptr 复制语义检查通过。配置测试覆盖缺省/部分/完整字段和颜色，并验证没有隐式调用 Init。
- 48 组场景比较旧 Body::Save 与新 SceneStorage 保存字节，再比较旧文件构造/原初始化流程和新加载结果，最后比较再次保存字节，全部一致。覆盖点、矩形和多形状物体、中文脚本注释、固定状态、外观默认值，以及原本不保存的外观参数/预设开关不会因归组被额外保存；物体脚本观察到入队前数量，场景脚本观察到入队后数量。
- 原有本地测试已适配成员路径并重新链接实际生产对象。物理 10 场景、800 子步和删除/重置检查；交互 2048 组输入、6144 拖拽子步及七种创建模式；渲染 960 场景四阶段缓冲与预览端点；脚本 10 帧双次 Update、128 组预设/512 子步、6 组保存加载及打开失败状态保留检查，全部通过。
- 原脚本对照中的旧 Body 配置/保存实现保留在本地基线中，不改为调用新 ApplyBodyConfig 或新 SaveBody 作为期望值；另有独立平铺类型测试覆盖本次数据布局调整。
- Body 从 GenerateRandomPointInside 起的几何、惯量、积分和静电实现逐段保持一致。Renderer、Runtime、Creator、SceneBuilders 还原本次成员访问路径及配置函数替换后，与基线逐文件一致；没有改变执行顺序和求值次数。
- Standards 独立只读审查：硬规范 0 项，新增判断性风险 0 项。Spec 独立只读审查：未发现遗漏、超范围或迁移错误。两项审查不代替运行验证。
- 本次 12 个 C++ 文件通过 UTF-8 BOM、CRLF、无 Tab 和无替换字符检查，git diff --check 通过。原有 SceneParameter.cpp、rigid 审计和 UI 回调方案 SHA256 未变化；暂存区为空。
- 所有本地测试继续放在被忽略的 x64 下，不提交测试。未做实际窗口验收；圆形空顶点存储、空字符串及损坏文件处理的历史问题未修复，也不算作已通过的测试范围。

本轮三项实现已获用户确认，代码与本文一起提交。Body 仍按值拥有 appearance/scriptData，并保留原交互状态；第二步所有权迁移未开始，需另行设计和确认。
