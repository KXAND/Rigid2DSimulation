# rigid 的脚本与存储职责迁移

## 1. 范围与状态

- 日期：2026-10-05；起点：`b0c4a6a`。
- 用户已批准本轮三项工作：迁出 Connection 配置解释、集中 Shape/Connection 记录读写、将 ShapeBuiltins 移入 script。
- 沿用 Body 已完成的迁移方式，不开始 Body 所有权改造、UI 回调化或 SceneBuilders 整体搬迁。
- 三项作为一个实施批次，已获用户检查确认，代码和本文一起提交，不按文件拆提交。测试仅保存在被忽略的 `x64/`。
- 下表路径相对 `Rigid2DSimulation/current/`。C++ 保持 UTF-8 BOM、CRLF、4 空格；编码遵循此前已确认的新要求，不采用旧风格文档中的 GBK 条目。

## 2. 确定的实现映射

| 原入口或位置 | 目标 | 调用点与边界 |
| --- | --- | --- |
| Connection::ReadConfig(Value const&) | `script/ConnectionConfig.h/.cpp` 的 ApplyConnectionConfig(Connection&, Value const&) | 同步 Creator、SceneBuilders、SceneScriptRuntime；不初始化、不执行脚本、不注册连接 |
| Shape::Save(FILE*) | SceneStorage 私有静态 SaveShape(FILE*, Shape const&) | SaveBody 在原位置调用 |
| Shape(FILE*) | SceneStorage 私有静态 LoadShape(FILE*)，返回 shared_ptr<Shape> | LoadBody 在原位置调用；Shape 增加默认构造入口，保留成员默认初始化，读后按原逻辑调整 vertices 大小 |
| Connection::Save(PhysicsWorld const&, FILE*) | SceneStorage 私有静态 SaveConnection(FILE*, Connection const&, PhysicsWorld const&) | Save 在原位置调用；仍按 bodies 中的顺序计算两端索引 |
| Connection(PhysicsWorld&, FILE*) | SceneStorage 私有静态 LoadConnection(FILE*, PhysicsWorld const&)，返回 shared_ptr<Connection> | 先默认构造，再读入记录并恢复 body0/body1；不注册、不执行脚本、不提前入队 |
| `rigid/ShapeBuiltins.h/.cpp` | `script/ShapeBuiltins.h/.cpp` | 保持函数名、签名、函数体和 DSL 注册名；更新工程及 filters 路径，已有 script 包含路径不新增重复配置 |

删除旧成员接口和失效的直接依赖，不保留兼容转发。Shape/Connection 的几何、物理算法和数据归属不变；Connection 仍持有外观及脚本字段，不宣称已成为纯物理类型。

## 3. 必须保留的行为

1. 配置仍依次处理 length、springConstant、type；字符串类型映射、未知字符串和缺省字段处理保持原样，不增加校验或默认分支。
2. Shape 记录依次为 isCircle、radius、radian、relativePosition、顶点数量、相对顶点数组；运行时 vertices 只按数量分配，不在读取阶段计算几何。
3. Connection 记录依次为 type、脚本文本长度和内容、两端刚体索引、p0Relative、p1Relative、length、springConstant；临时脚本仍从已读脚本复制。
4. 保留所有记录的字段类型、数量、顺序及长度表示，不保存新增字段，不直接写结构体内存块。
5. 连接加载继续按“读取记录 → RefreshConnectionScript → SignUpToBodies → UpdatePosition → 加入 connections”执行。两端刚体此前已加载；连接脚本运行时，该连接尚未注册或入队。
6. 不修改对象所有权、延迟删除、物理公式、DSL 语法和调用次数。不处理空顶点、空字符串及损坏文件的历史问题。

## 4. 验证要求

- Debug|x64 构建；检查旧接口、旧 ShapeBuiltins 路径和工程登记无遗漏。
- 配置使用迁移前的独立实现对照，覆盖缺省、部分字段、数值类型及全部字符串类型和未知字符串。
- 使用原记录编解码实现对照新实现的保存字节、读入状态及再次保存结果；覆盖不同形状、连接类型、端点排列及同体连接。
- 复用物理、编辑、渲染、脚本和 Body 本地测试；旧实现不得改成调用新存储/配置函数来充当期望值。
- 以脚本观察加载时的连接数量及关联状态，核对注册、端点更新和入队时机。
- 检查迁移之外的算法函数未变、ShapeBuiltins 函数体未变，以及编码、换行、无 Tab 和差异空白检查。
- GUI 验收与自动化验证分别记录；上述历史问题不计作通过范围。

## 5. 实施记录

三项迁移已实现，构建与下列本地验证已通过，用户已检查确认；代码和本文一起提交。后续 ShapeBuiltins 可读性整理单独实施，不混入本批职责迁移。

### 5.1 实现结果

- 新增 ConnectionConfig 两个文件，替换 Creator、SceneBuilders、SceneScriptRuntime 三个文件中的全部六处调用；Connection 的 Value/PhysicsWorld 前置声明及配置、文件接口已移除。
- Shape/Connection 的四个记录函数迁入 SceneStorage 私有静态函数，旧文件构造和 Save 已删除。Shape 默认构造保留所有已有字段默认值；读取函数不执行几何初始化或脚本。
- ShapeBuiltins 两文件移入 script，内容逐文件与基线一致；工程及 filters 已同步，未新增包含目录。
- Connection 从 SignUpToBodies 起、Shape 从 GenerateRandomPointInside 起的实现与基线一致；三个配置调用文件除包含声明及顺序外，还原本轮调用替换后与基线一致。配置函数还原成员前缀及签名后与原实现一致。

### 5.2 验证记录

验证日期：2026-10-05。

- 主工程 Debug|x64 构建通过。首次沙箱构建因 MSBuild FileTracker 访问拒绝而失败，获准在沙箱外重试后成功；不是源码编译错误。
- `x64/record-migration-check/BaselineRecords.h` 保留迁移前的 Shape/Connection 成员实现，仅更换测试类型和符号。原脚本与 Body 测试的基线改用这些旧实现，不调用新存储/配置函数生成期望值。
- 256 组 Connection 配置对照通过，覆盖缺省字段、两个数值字段的组合、四种字符串类型、未知字符串和数值类型，保持原非配置字段不变。
- 64 组完整场景的保存字节、读后形状/连接状态与再次保存字节对照通过，覆盖三角形、矩形、多形状、四种连接类型、端点排列、同体连接及中文脚本。
- 加载时序探针通过：连接脚本执行时刚体已加载、当前连接尚未入队或注册；此前连接端点已更新，同体连接只注册一次。脚本处理结果以旧实现求值结果为基准，不假设 DSL 的赋值语义。
- 原脚本测试通过：注册/场景构建/配置、10 帧双次 Update、128 组预设/512 子步、6 组二进制保存加载及打开失败时保留场景。
- 原物理测试通过：10 场景、800 子步、2 项删除检查及重置默认值。原编辑测试通过：2048 输入组合、6144 拖拽子步、选择/删除/模式及七种创建模式。
- 原渲染测试通过：960 场景的四阶段 RGB/alpha/depth 缓冲及预览端点一致。原 Body 测试通过：5 类构造、1280 组配置、拾取及程序共享语义、48 组二进制/加载/再次保存。
- 本轮 13 个现存 C++ 文件通过严格 UTF-8 BOM、CRLF、无 Tab、无替换字符检查。工程及 filters 引用路径均存在，旧接口/旧路径无残留，git diff --check 通过。
- 两项独立只读审查已完成：Spec 未发现遗漏、越界或迁移错误；Standards 指出的两处新增头文件排序问题已修正，仅整理对应包含块，没有调整函数体。
- 未做实际窗口验收；圆形空顶点存储、空字符串及损坏文件的历史问题未修复，不算作本次通过范围。所有测试仍只在被忽略的 x64 下，不提交。

既存未跟踪的 rigid 审计及 UI 回调文档未改动；本次仅新增本文，不更新其他阶段的设计。
