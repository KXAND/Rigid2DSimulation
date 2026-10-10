# 场景网格配置校验

## 范围与问题

只修复 gridSize、sceneWidth、sceneHeight 与派生网格数量的合法性，不调整 stepCount、音频、碰撞公式、子步时序或网格算法。

此前 SetConfig 直接写入这些值。PhysicsWorld::Prepare 随后将 ceil(场景尺寸 / gridSize) 转成 int；Advance 用两个 int 的乘积分配网格。零、负数、非有限值、过大的商或乘积可能造成无效转换、整数溢出或索引越界。场景宽高本身也是 int，必须先校验 DSL 的 double，再转换。

## 输入规则

- gridSize 必须是 num 类型、有限且大于零。不通过人为最小值改变网格精度。
- sceneWidth / sceneHeight 必须是 num 类型、有限且在 [1, INT_MAX] 内。合法小数仍按已有行为向零截断，不新增严格整数契约。
- 未提供的字段沿用原值。三个字段先组成候选配置，再共同校验；任何一个无效时均保留原来的三个值，不进行部分应用。
- 拒绝网格组合不撤销同一次 SetConfig 的其他独立字段。使用与 maxRealDeltaTime 校验相同的 wcerr 提示，不为非法配置新增暂停或脚本异常规则。

## 计算边界

- PhysicsWorld::IsGridConfigValid 直接完成检查，不使用带输出参数的计算辅助函数。两个 ceil 商先保留为 double，避免校验前进行危险的整数转换。
- 用户确认总格数最多为 65,536 格，包含恰好等于上限的情况。这是限制分配与遍历成本的工程上限，不代表物理精度指标；超限时拒绝候选组合，不自动调整 gridSize。
- 上限放在 PhysicsWorld 的私有常量 kMaxGridCells 中，与网格成员归组。先检查每个轴不超过上限（同时拒绝商为无穷大），再以 double 乘法检查总格数；通过后 int 转换和索引乘积均安全。删除冗余的容器 max_size 检查；格数限制不保证永不发生内存不足，不新增通用 bad_alloc 恢复机制。
- SceneScriptRuntime 在应用候选组合前调用 IsGridConfigValid。PhysicsWorld::Prepare 先调用同一校验，再计算并存储整数格数，以免 C++ 写入非法值后绕过 DSL 校验。DSL 宽高转换上限直接使用 <climits> 的 INT_MAX。
- 对于绕过 DSL 的非法 C++ 配置，Prepare 在转换、分配及模拟前抛出 invalid_argument；这是调用方违反前置条件，不是偷偷修正世界参数。调用者仍须在 Advance 前调用 Prepare。
- 本轮不修改 sceneLeft / sceneTop、任意刚体的非有限坐标或存档校验，不声称所有几何和数值输入已安全。

## 验证

- 修复前复现四项失败：gridSize 为零或负数、场景宽为零、高为负数均被错误接受；不运行危险网格分配。
- 修复后覆盖非数值类型、NaN、正负无穷、转换范围、商溢出、乘积溢出、整组拒绝、遗漏字段、合法小数截断及真实 DSL 调用。
- 直接 C++ 非法配置须在 Prepare 拒绝；合法配置须完成真实物理推进。既有脚本和 Creator 回归不应受影响。
- 本地测试放在忽略的 x64/frame-time-check/，不提交。代码与本文经用户确认后一起提交。

## 验证结果

- Debug x64 / Win32 构建通过，输出使用独立 TargetName，不修改 .vcxproj.user。
- 663 项脚本检查通过，其中 60 项为本次网格检查；原有 603 项脚本与运动预设回归全部通过。
- 覆盖恰好 65,536 格、任一轴或总数超限、极小网格尺寸、类型与数值异常、候选组合整体应用、直接 C++ 绕过 DSL 的防护。合法网格实际完成分配、遍历和物体运动计算。
- 26 项 Creator 配置交接回归通过。
- 修改的 C++ 文件通过严格 UTF-8 BOM、CRLF、空格缩进检查；git diff --check 通过。
- 未进行人工 GUI 验收。未恢复或修改 DSL 契约 stash；本轮提交范围仅为三个源码文件及本文。
