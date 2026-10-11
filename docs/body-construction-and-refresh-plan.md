# Body 创建与属性刷新方案

## 1. 文档地位与当前授权

归档说明：下文提及的 `docs/body-initialization-plan.md` 是未实施的历史草案，现已移至本地忽略目录 `ref/archive/body-initialization-plan.md`，不随仓库分发。旧方案的关键区别已在第 2 节保留，阅读或实施本方案不依赖该本地文件；下文历史决策不作改写。

- 日期：2026-10-07；源码基线：`9667691`。
- 本文承接最新讨论，代替 `docs/body-initialization-plan.md` 作为本轮方案入口。旧文档保留为此前草案，不再按其中的 Init 改名及函数提取清单直接实施。
- 新方向是明确创建入口、创建时完成必要计算、已有物体修改配置时刷新；不是只将 Init 改名。
- 用户已经明确：本轮保证旧计算结果不变，不要求消除原项目 bug；正确性修复留到后续独立处理。既有计算的重复调用副作用也属于需要保留的行为。
- 本轮实现及后续接口调整已获用户确认。下文第 5 节固定最终接口与调用点映射，第 6 节记录实施和验证状态；本文与对应代码一起提交。
- 后续继续按“方案确定 → 实现及验证 → 用户检查 → 文档与对应代码一起提交”执行。本地测试只放在被忽略的 x64/ 下。

## 2. 新旧方向的区别

| 内容 | 旧草案 | 当前方向 |
| --- | --- | --- |
| 创建后是否可直接使用 | 保留先构造、填字段、手动重建 | 先准备最终创建参数，再由创建入口完成原有初始化计算 |
| 点物体入口 | 保留 Body(position) | 使用能明确表达点物体的入口，如 CreatePoint，不让普通位置参数暗示对象种类 |
| 修改已有物体 | 继续通过重命名后的 Init | 明确属性修改/刷新入口，但在本轮仍执行旧完整计算 |
| bool resetPosition | 原样保留在重建入口 | 区分创建时的放置选择与属性刷新；不能让“刷新属性”含糊地承担重新放置物体的含义 |
| 内部数学实现 | 拆出原有步骤 | 可以复用旧计算，但不能因入口变清晰就删掉重复计算或修正结果 |

“一次性”只表示调用者的创建流程完成一次，不表示旧局部几何计算从此不再执行。实测证明，在当前计算不变的要求下，这两个概念必须区分。

## 3. 改动前的 C++ 行为基线

### 3.1 方法与边界

- 新增本地测试 `x64/body-init-check/BodyInitializationChecks.cpp` 及同名 vcxproj，链接当前生产 Body、Shape、配置及脚本等对象文件，实际调用 Body::Init 和 SceneScriptRuntime::RefreshBodyScript。
- 对照实现采用既有 `x64/body-data-check/BaselineBody.cpp`，不是让旧实现调用新 Body::Init。运行前逐段核对其 Init、UpdateShapesPositionAndAABB、UpdateAABB 与当前生产源码在替换类型名后完全一致。
- 新旧物体分别创建 Shape，避免共享可变几何导致相互污染；同时检查各自内部的 Shape 引用共享关系一致。
- 比较浮点位模式，不放宽误差：覆盖物理参数、面积、inertia、invMass、invInertia、电荷、位置/速度/角度、transform、box，以及每个 Shape 的局部/世界位置和顶点。
- 不使用理想质心惯量作为通过条件，不改成标准库的其他圆周率值，不修复历史公式。
- 主工程 Debug|x64 构建通过，使用 TargetName=Rigid2DSimulationInitCheck 输出临时 EXE，避免覆盖可能正在运行的程序；没有修改工程配置文件或启动新窗口。

### 3.2 覆盖结果

| 检查 | 实际结果 |
| --- | --- |
| 8 类几何 × 2 种 resetPosition × 2 种 fixed × 3 个密度 × 3 个角度 × 2 个电荷密度 | 576 组序列通过 |
| 每组依次首次初始化、原样重复、密度翻倍、切换 fixed、修改电荷密度 | 共 2880 次实际 Init 与独立旧实现逐位一致 |
| 点物体 × resetPosition/fixed 组合 | 4 组通过，Init 保持完整的提前返回，不重新计算质量或世界几何 |
| 原有字面脚本 body.density=2; | 8 组实测未写入密度，但随后仍执行 Init；保留该基线行为，不计作密度修改成功 |
| 脚本调用本地测试配置提供函数，再走真实 RefreshBodyScript | 24 次刷新通过；确认配置确实写入 density=2，结果与旧 ReadConfig+Init 逐位一致 |

8 类几何为居中矩形、偏移矩形、非对称三角形、居中圆、偏移圆、两个分离圆、圆与矩形组合，以及两次引用同一 Shape 的组合。

另外复跑既有 Body 数据和脚本/存储测试通过，包括 1280 组 Body 配置、48 组 Body 存储、运动预设、Shape/Connection 记录及加载顺序、参数记录和重新保存。它们作为原有行为的补充回归，不代替上述新建的初始化序列测试。

脚本测试第一次运行因“字面脚本应写入密度”的假设失败。随后把该基线现象与数值刷新测试分开：本地 SupplyDensityForCheck 函数只提供确定的 body 字典，实际编译/执行、ApplyBodyConfig、Init 仍由生产 RefreshBodyScript 完成。没有修改 DSL 实现，也没有将这个注入测试描述为普通字典赋值语法已通过。

### 3.3 关键数值记录

关键样例的 17 位精度数值记录位于被忽略的 `x64/body-init-check/results.csv`。以下取边长 2、输入顶点中心为 (10, 0) 的矩形，初始 density=1、fixed=false：

| 操作 | inertia | invInertia |
| --- | ---: | ---: |
| 第一次实际 Init | 402.66666666666663 | 0.0024834437086092716 |
| 不修改参数，再次 Init | 2.6666666666666665 | 0.375 |
| 独立物体首次 Init 后，density 改为 2，再按旧流程 Init | 2.6666666666666665 | 0.1875 |
| 另一独立物体首次 Init 后，density 改为 2，只利用缓存刷新质量/电荷 | 402.66666666666663 | 0.0012417218543046358 |

最后一行是本地测试中的候选简化操作，不是生产改动。它与旧结果不同，故本轮不得采用。

其他观测：

- 居中矩形在重复 Init 后结果相同；只测这类输入会漏掉上述差异。
- 偏移圆及两个分离圆在所测重复调用中 inertia 不变，保留其当前结果，不依据平行轴定理补项。
- 同一偏移矩形连续两次 Init(true)，position 先变为 (10, 0)，再变为 (0, 0)。这再次说明 resetPosition 不是通用的“刷新后保持位置”开关。

上述是当前程序的实际行为记录，不代表数学正确性认证。测试通过只建立这批输入和调用序列的回归基线，不构成对所有输入的等价性证明。

## 4. 据此约束新方案

### 4.1 创建入口

可以把原来调用方执行的“准备形状和最终参数 → Init”收进构造或具名创建入口，但必须在同样的参数已经准备完毕后执行旧计算一次。

不能直接在现有 Body(shape) 内先执行 Init，然后继续沿用调用者写参数并再次 Init 的流程；那会多执行一次，会改变数值。对象脚本、随机数、预览和入队的先后也必须逐个保持。

Body(position) 改成明确的点物体创建入口仍是推荐方向。点物体原有质量及提前返回规则不改，不顺手把它变成新的有质量粒子模型。

### 4.2 属性修改与刷新

公开接口可以表达“修改已有物体参数”，不再让 UI 调用名为初始化的操作。但本轮内部仍需执行原来的完整重算，包括局部坐标处理和世界几何同步，不能仅利用首次 area/inertia 缓存更新三个派生字段。

无实际参数变化时，旧 RefreshBodyScript 也可能继续调用 Init。不能新增“配置未变化则跳过刷新”的优化；旧代码提前返回的分支也不得擅自补算。

创建与刷新可以共用一份私有计算实现，不复制两套公式，也不保留旧名称的兼容包装。这里保留的是程序结果，不是历史接口。

### 4.3 放置选择与几何更新

resetPosition 的两个分支仍按原行为执行：false 保留现有 position，true 用原计算得到的 center 覆盖 position；两者都可能修改局部几何。创建参数应明确表达这个选择，不把它解释成新的坐标转换规则。

生产属性刷新原本调用 Init()，即 false；新刷新入口应保持同样行为。普通位置/姿态更新与渲染前同步不在本轮重新调度，Warp、碰撞、连接求解之后也不额外插入刷新。

## 5. 本次实施设计

### 5.1 接口

| 接口 | 职责 |
| --- | --- |
| `BodyProperties` | 创建输入或已有属性快照，不是第二份长期状态；包含配置可写的姿态、速度、物理参数和 innerColor，不包含形状、派生量或脚本程序 |
| `Body(shape, properties = {})` | 单形状普通物体，写入最终参数后执行原计算一次 |
| `Body(shapes, properties = {})` | 多形状普通物体，同上；保留输入 Shape 的共享关系 |
| `Body::CreateAtShapeCenter(shapes, properties = {})` | 明确采用输入形状中心作为位置，返回 shared_ptr；写入属性后执行原计算一次，覆盖 properties.position |
| `Body::CreatePoint(position)` / `Body::CreatePoint(properties)` | 明确创建点物体，返回 shared_ptr；不计算面积、质量或世界几何，保留原点物体行为 |
| `Body::GetProperties() const` | 返回已有配置属性的快照，用于保留脚本未指定的字段 |
| `Body::ApplyProperties(properties)` | 写入属性并执行旧完整重算；保持指定位置，不提供重新放置开关 |
| 私有 `AssignProperties(properties)` | 仅赋值，供创建与刷新共用 |
| 私有 `Vector2 Recalculate()` | 重算普通物体的派生量和局部几何，返回输入形状中心；不决定 position，不同步世界几何；保留旧公式及重复计算行为 |
| `ParseBodyProperties(config, defaults = {})` | 按值接收默认属性，以配置中指定的字段覆盖后返回 BodyProperties；不修改调用者参数，不持有 Body，不触发计算 |

普通构造只保留 properties.position，不再公开放置选项；特殊创建使用 CreateAtShapeCenter，对应旧 Init(true)。BodyPlacement 和私有三参数构造均删除；放置决策由入口直接表达，不再传递枚举或布尔值。DSL 的 resetPosition 键保持原样，在 CreateBody 边界选择普通构造或具名工厂；point 分支仍优先，不因 resetPosition 改变点物体行为。

BodyProperties 默认值沿用旧 Body 默认构造：density=1、elasticity=0.7、frictionStatic=0.5、frictionDynamic=0.3、dampCoeff/dampCoeffAngular=0.1；其余数值和向量为零、fixed=false、innerColor 沿用 Color 默认值。innerColor 暂时随参数传递仅为保持现有配置功能，不扩展本轮为外观职责重构。Body 自身仍持有原字段。

公开的无参构造、位置构造和 Init 移除，不保留兼容包装。复制构造行为及 Shape 的共享所有权不在本轮重设计。

### 5.2 调用点映射

下表源码路径相对于 `Rigid2DSimulation/current/`。

| 文件/调用点 | 修改后的顺序 |
| --- | --- |
| editor/Creator.cpp：Box、Ball、Plate、Particle | 准备形状 → 默认属性加位置/角度/随机颜色 → 读取配置覆盖 → 普通构造一次 → 原预览/入队流程 |
| editor/Creator.cpp：Point | 准备位置/随机颜色/配置 → CreatePoint(properties) → 入队 |
| editor/Creator.cpp：Nail | CreatePoint(原锚点世界位置) → 入队 → 原连接创建 |
| rigid/SceneBuilders.cpp：CreateBody | 原形状解析 → 随机颜色及配置 → 读取 resetPosition → 按 point/resetPosition 选择点工厂、形状中心工厂或普通构造 → 入队 |
| rigid/SceneBuilders.cpp：Boundary、Gear、Strand、Necklace | 每个物体使用独立参数对象；按原顺序应用默认值和配置，再构造；连接顺序、随机调用次数不变 |
| script/SceneScriptRuntime.cpp：RefreshBodyScript | 原脚本执行及提前返回 → 获取属性快照并读取配置 → 原预设读取 → ApplyProperties(properties) |
| storage/SceneStorage.cpp：LoadBody/Load | 私有原始构造 → 读取原二进制字段 → 原 RefreshBodyScript → 入队，不新增计算 |

加载是普通构造之外的明确例外：保留一个私有 `Body()`，只允许 Body 内部和友元 SceneStorage 使用。LoadBody 使用 `shared_ptr<Body>(new Body())`，因为 make_shared 无法访问私有构造。该对象仅在加载内部暂存原始记录，不新增公开的“未初始化物体”入口，不把 FILE 或脚本依赖引入 Body。

加载时依然由对象脚本刷新触发计算。若脚本删除 body 作用域项，仍按旧行为提前返回，不能为了新接口看似完整而补算。普通脚本刷新把数值先读到快照，预设读取后统一写回；正常执行结果及计算次数保持不变，不承诺内存分配失败时的部分写入状态一致。

### 5.3 验收边界

- 检查所有生产 Body 创建及旧 Init 调用，避免“构造先算、调用者再次刷新”的双算。
- 初次构造与独立旧实现“准备字段 + Init(放置选项)”逐位对比；后续 ApplyProperties 与旧 Init(false) 对比。旧测试中重复 Init(true) 仅保留为历史观察，不要求新属性刷新提供该语义。
- 点创建/刷新、缺省配置、实际脚本刷新、创建工具、场景构建、存储及渲染回归按新接口适配后运行。对照端保留旧计算，不能也改成新构造而形成自我比较。
- 保留历史异常行为，不声称证明所有输入等价；本地测试放在忽略的 x64/，不提交。
- 修改的 C++ 文件统一 UTF-8 BOM、CRLF、空格缩进；不改存档字段或 DSL。

## 6. 实施与验收记录

### 6.1 已实现

- 已修改 8 个生产文件：Body.h/.cpp、BodyConfig.h/.cpp、Creator.cpp、SceneBuilders.cpp、SceneScriptRuntime.cpp、SceneStorage.cpp，具体职责见第 5 节。
- 普通构造完成原首次计算；点物体仅由 CreatePoint 明确创建；已有物体配置通过属性快照、ParseBodyProperties、ApplyProperties 更新。
- 公开 Body()、Body(position)、Body::Init 和 ApplyBodyConfig 已移除，生产源码无旧调用残留。加载保留私有原始构造，不增加计算。
- Recalculate 保留旧 Init 中派生量及局部几何计算的公式和算术顺序；点物体提前返回移至 ApplyProperties，放置及世界几何同步移至入口。重复重算的数值行为保留。
- 不调整物理步进、几何同步调度、UI、DSL 语法或二进制记录布局。既有未追踪的其他文档未修改。
- 8 个 C++ 文件已检查为严格 UTF-8 BOM、CRLF，无制表符和替换字符；git diff --check 通过。
- 最终增量链接曾报 SceneStorage.obj 的 LNK1163（COMDAT 无效）；未修改源码，完整 Rebuild 后通过。因此判断为中间构建产物问题，而非通过代码变通绕过。随后将回归测试重新链接到完整重建产物验证。

### 6.2 本次回归结果

测试均位于忽略的 x64/，不纳入提交。对照端保留旧计算：核心数值用独立 BaselineBody；场景构建和创建工具用本地冻结的旧构造/Init/配置读取。旧测试中依赖公开无参构造和手动 Init 的夹具已按测试目的适配，未为测试恢复生产兼容入口。

| 检查 | 本次结果 |
| --- | --- |
| 主工程 Debug/x64 | 构建通过；输出临时 Rigid2DSimulationInitCheck，未覆盖可能正在运行的主程序 |
| 8 类几何、两种初次放置、固定/密度/角度/电荷组合 | 576 组、2880 个创建/刷新阶段与独立旧实现逐位一致 |
| 点物体刷新 | 4 组通过，保留原计算的提前返回 |
| 实际对象脚本刷新 | 8 组字面脚本保留原行为，24 次显式提供配置的真实脚本刷新与旧实现一致 |
| 场景构建 | 288 组通过，参数覆盖、物体/连接登记顺序、Shape 独立性及随机消耗一致 |
| 创建工具 | 112 组新旧对比通过，覆盖 7 种模式、8 组配置、预览/释放及随机消耗 |
| Body 默认值、配置及存储 | 5 类构造、1280 组配置刷新、程序共享、48 组二进制/加载/再保存通过 |
| 原始记录加载时序 | 6 组点/单多边形/组合多边形记录通过；脚本提供配置时只算一次，删除 body 配置时仍不计算 |
| 脚本及关联存储回归 | 10 个双更新帧、128 组预设/512 步、6 组场景存储及打开失败保留通过；256 组连接配置、64 组 Shape/Connection 存储、48 组参数记录及加载顺序通过 |
| 编辑器回归 | 2048 组输入/6144 拖拽子步、6144 个双物体连续帧，以及删除、清理、创建和 Warp 排除检查通过 |
| 渲染及外观/Group 回归 | 960 个场景各 5 阶段的 RGB/alpha/depth 缓冲一致；外观默认值/复制及 2304 组 Group Warp 对比通过 |

第 3 节旧测试曾重复调用 Init(true)；本次序列改为首次按放置选项构造，之后均 ApplyProperties 对比旧 Init(false)，符合实际公共接口及生产调用。不能把两轮 2880 的统计理解为接口序列完全相同。

### 6.3 保留问题与验证边界

- 保留偏移多边形首次/再次计算的 inertia 差异及旧字面脚本密度赋值未生效现象，不改为缓存刷新或修复 DSL。
- 新增加载测试最初触发旧记录处理的空字符串和圆形空顶点迭代器断言。改用非空场景脚本，并将新增原始加载时序测试限定为点/多边形；未修改生产存储逻辑，未宣称圆形存档已通过该测试。
- 验证范围为上述样本的程序结果，不是物理正确性证明；未进行人工交互验收，不涵盖任意畸形存档、分配失败或并发访问。
- 第 3 节为改动前基线。被忽略的 results.csv 现由新接口测试重新生成，当前记录的末两项为首次采用形状中心、后续刷新保持位置；历史重复 Init(true) 的数值仍保留在本文第 3.3 节。

### 6.4 本次接口修订（2026-10-07）

- 用户确认：Refresh 改名为 ApplyProperties，明确表示写入属性并重算；普通构造移除公开 placement，新增 CreateAtShapeCenter；当时将枚举及三参数构造保留为私有实现，随后按第 6.6 节进一步移除。
- Recalculate 的公式和执行顺序不变，特殊工厂不能先走普通构造再重算。
- 后续讨论否决了 ReadBodyProperties(properties, config) 的输出参数形式。用户确认改为返回值式 ParseBodyProperties(config, defaults = {})，仍位于脚本层，不把 Value 依赖引入 Body，也不新增读取类。
- 本次接口修订已通过 Debug/x64 构建及上表六组本地测试程序的重新构建/运行；2880 个阶段仍逐位一致。场景构建测试补充 resetPosition 与 point 的组合，保持点物体优先分支。

### 6.5 返回值式配置解析

- 签名：`BodyProperties ParseBodyProperties(Value const& config, BodyProperties defaults = {});`。
- 配置缺省字段保留传入的默认值；未传 defaults 时使用 BodyProperties 的默认属性。defaults 按值传入，调用者对象不被修改。
- Creator 的可选配置及 SceneBuilders 先准备位置、角度、随机颜色等默认属性，再接收解析结果；配置覆盖顺序和随机调用次数不变。
- RefreshBodyScript 改为 `auto properties = ParseBodyProperties(bodyConfig, body.GetProperties());`，预设处理后仍调用 ApplyProperties，计算时点不变。
- 移除 ReadBodyConfig，不保留兼容包装。BodyConfig.h 包含 Body.h 以支持完整返回类型及默认参数；Body 不反向依赖脚本层。
- 本次解析接口调整已通过 Debug/x64 构建和六组本地回归：2880 个数值阶段仍逐位一致，创建工具/场景构建、脚本、编辑器、渲染及存储检查通过。1280 组配置用例同时检查传入默认值不被修改，补充省略 defaults 的默认属性检查。
- 本轮改动的 C++ 文件保持 UTF-8 BOM、CRLF、空格缩进；git diff --check 通过。测试仍仅位于忽略的 x64/，不纳入提交。

### 6.6 移除内部放置选项

- 删除 BodyPlacement 及私有三参数构造；不替换为另一个开关。
- 普通构造保存 shapes 后调用 ApplyProperties：写入属性 → Recalculate → 同步世界几何，保留指定 position。
- ApplyProperties 对点物体在写入属性后直接返回，不计算派生量，也不更新 transform、Shape 或 AABB。
- CreateAtShapeCenter 在内部通过私有 Body() 准备对象：设置 shapes → 写入属性 → position = Recalculate() → 同步世界几何 → 返回。不能先调用会自动计算的普通构造。
- Recalculate 仅重算派生量及局部几何，返回原 center；其所有调用点均为普通物体。原放置赋值从局部几何循环前移至返回后，但局部计算不依赖 position，世界几何仍在最终位置确定后只同步一次。
- 保持零面积等输入的旧行为，不新增校验、缓存或物理公式修复；加载入口和计算时点不变。
- 本次调整通过 Debug/x64 构建和六组本地回归：2880 个创建/属性修改阶段仍与独立旧实现逐位一致；点物体、两种创建位置、配置覆盖、脚本及加载提前返回、编辑器与渲染检查均通过。静态检查确认生产代码无 BodyPlacement 残留，每条普通物体入口只重算和同步一次。
- Body.h/.cpp 保持 UTF-8 BOM、CRLF、空格缩进，git diff --check 通过；文档与对应代码一起提交。

### 6.7 当前状态

实现与本地验证完成，用户已确认。本文与第 6.1 节所列的 8 个源码文件作为同一提交，不单独提交文档，不包含本地测试、context.md 或其他未追踪草案。本轮不继续开展额外重构。
