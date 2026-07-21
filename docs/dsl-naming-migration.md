# DSL 命名迁移对照表

## 1. 目的与边界

本文记录项目自定义 DSL 的名称迁移，执行时以 `docs/code-style.md` 为准。DSL 可调用函数使用 PascalCase；作用域变量、配置键和常量使用 lower camel 或 `kPascalCase`。

不改变词法与语法：关键字、运算符、分隔符和访问语法保持不变。`.rigid2d` 的二进制布局保持不变，但其中保存的脚本文本和配置名称必须同步更新。项目中没有独立文本脚本文件；`belt.rigid2d`、`car.rigid2d`、`gear.rigid2d`、`pulley.rigid2d` 和 `saved.rigid2d` 是需要重新验证的示例场景。

状态说明：

- `待迁移`：名称和目标名称已明确。
- `待确认`：名称含义不足以安全确定目标名称，实施前必须先确认。
- `不迁移`：脚本语法或外部格式标记，不属于项目命名规范。

## 2. 脚本语法与解析器内建

| 旧名称 | 目标名称 | 分类 | 注册或读取位置 | 状态 |
| --- | --- | --- | --- | --- |
| `def`、`return`、`if`、`else`、`while` | 保持 | 关键字 | `framework/parse/token.cpp` | 不迁移 |
| `null` | 保持 | 语言字面量 | `framework/parse/*` | 不迁移 |
| `+`、`-`、`*`、`/`、`==`、`&&` 等 | 保持 | 运算符 | `framework/parse/*` | 不迁移 |
| `sin` | `Sin` | 内建函数 | `framework/parse/var.cpp` | 待迁移 |
| `cos` | `Cos` | 内建函数 | `framework/parse/var.cpp` | 待迁移 |
| `abs` | `Abs` | 内建函数 | `framework/parse/var.cpp` | 待迁移 |
| `pow` | `Pow` | 内建函数 | `framework/parse/var.cpp` | 待迁移 |
| `rand` | `Random` | 内建函数 | `framework/parse/var.cpp` | 待迁移 |
| `rand_int` | `RandomInt` | 内建函数 | `framework/parse/var.cpp` | 待迁移 |
| `str` | `ToString` | 内建函数 | `framework/parse/var.cpp` | 待迁移 |
| `PI` | `kPi` | 内建常量 | `framework/parse/var.cpp` | 待迁移 |

## 3. 应用 DSL 可调用函数

这些名称由 `SimulationApp::InitializeDefaultFunctions` 注册。目标名称与对应 C++ 入口保持一致，避免 DSL 和 C++ 使用两套缩写。

| 旧名称 | 目标名称 | C++ 入口 | 状态 |
| --- | --- | --- | --- |
| `set_creator` | `SetCreator` | 创建器赋值 lambda | 待迁移 |
| `create_body` | `CreateBody` | `CreateBody` | 待迁移 |
| `box` | `Box` | `Box` | 待迁移 |
| `ball` | `Ball` | `Ball` | 待迁移 |
| `parallelogram` | `Parallelogram` | `Parallelogram` | 待迁移 |
| `trapezoid` | `Trapezoid` | `Trapezoid` | 待迁移 |
| `regular_poly` | `RegularPolygon` | `RegularPoly` | 待迁移 |
| `cross` | `Cross` | `Cross` | 待迁移 |
| `n_bodies` | `GetBodyCount` | 返回 `bodies.size()` 的 lambda | 待迁移 |
| `create_conn` | `CreateConnection` | `CreateConnection` | 待迁移 |
| `set_cfg` | `SetConfig` | `SetCfg` | 待迁移 |
| `boundary` | `Boundary` | `Boundary` | 待迁移 |
| `framed_box` | `FramedBox` | `FramedBox` | 待迁移 |
| `gear` | `Gear` | `Gear` | 待迁移 |
| `strand` | `Strand` | `Strand` | 待迁移 |
| `necklace` | `Necklace` | `Necklace` | 待迁移 |
| `update` | `Update` | `SimulationApp::Update` 脚本回调 | 待迁移 |

## 4. 应用作用域变量

| 旧名称 | 目标名称 | 来源或读取位置 | 状态 |
| --- | --- | --- | --- |
| `body` | `body` | `Body::Execute` | 不迁移 |
| `conn` | `connection` | `Connection::Execute` | 待迁移 |
| `t` | `time` | `Body` 脚本作用域、`SimulationApp::Update` | 已迁移 |
| `left_bgr` | `backgroundLeft` | `SimulationApp::UpdateBasicState` | 待迁移 |
| `top_bgr` | `backgroundTop` | `SimulationApp::UpdateBasicState` | 待迁移 |
| `w_bgr` | `backgroundWidth` | `SimulationApp::UpdateBasicState` | 待迁移 |
| `h_bgr` | `backgroundHeight` | `SimulationApp::UpdateBasicState` | 待迁移 |
| `cur.fps.dt()` | 待确认 | `SimulationApp::Update` | 待确认 |

`cur.fps.dt()` 含有访问和调用语法。实施前必须确认它是完整字符串键，还是解析器访问表达式的一部分；不得按普通标识符机械替换。

## 5. 场景与刚体配置键

### 5.1 全局场景配置

| 旧名称 | 目标名称 | 读取位置 | 状态 |
| --- | --- | --- | --- |
| `vol` | `volume` | `SimulationApp::SetConfig` | 已迁移 |
| `energy_mul` | `energyMultiplier` | `SimulationApp::SetConfig` | 待迁移 |
| `chargeMultiplier` | `chargeMultiplier` | `SimulationApp::SetConfig` | 不迁移 |
| `eps_paralell` | `parallelEpsilon` | `SimulationApp::SetConfig` | 待迁移 |
| `max_real_dt` | `maxRealDeltaTime` | `SimulationApp::SetConfig` | 待迁移 |
| `hasElectrostatic` | `hasElectrostatic` | `SimulationApp::SetConfig` | 不迁移 |
| `coulomb` | 待确认 | `SimulationApp::SetConfig` | 待确认 |
| `stepNum` | `stepCount` | `SimulationApp::SetConfig` | 待迁移 |
| `gridSize` | `gridSize` | `SimulationApp::SetConfig` | 不迁移 |
| `useEqualPositionCorrection` | `useEqualPositionCorrection` | `SimulationApp::SetConfig` | 不迁移 |
| `t` | `time` | `SimulationApp::SetConfig` | 已迁移 |
| `isTrackShown` | `isTrackShown` | `SimulationApp::SetConfig` | 不迁移 |
| `left_scene` | `sceneLeft` | `SimulationApp::SetConfig` | 待迁移 |
| `top_scene` | `sceneTop` | `SimulationApp::SetConfig` | 待迁移 |
| `w_scene` | `sceneWidth` | `SimulationApp::SetConfig` | 待迁移 |
| `h_scene` | `sceneHeight` | `SimulationApp::SetConfig` | 待迁移 |
| `gravity` | `gravity` | `SimulationApp::SetConfig` | 不迁移 |

### 5.2 刚体与连接配置

| 旧名称 | 目标名称 | 读取位置 | 状态 |
| --- | --- | --- | --- |
| `preset_o` | `hasPositionPreset` | `Body::Execute` | 待迁移 |
| `preset_ang` | `hasAnglePreset` | `Body::Execute` | 待迁移 |
| `preset_v_ang` | `hasAngularVelocityPreset` | `Body::Execute` | 待迁移 |
| `o_prog` | `positionProgram` | `Body::Execute` | 待迁移 |
| `ang_prog` | `angleProgram` | `Body::Execute` | 待迁移 |
| `v_ang_prog` | `angularVelocityProgram` | `Body::Execute` | 待迁移 |
| `o` | `position` | `Body::ReadConfig` | 待迁移 |
| `v` | `velocity` | `Body::ReadConfig` | 待迁移 |
| `col` | `innerColor` | `Body::ReadConfig` | 待迁移 |
| `radian` | `radian` | `Body::ReadConfig` | 不迁移 |
| `velocityAngular` | `velocityAngular` | `Body::ReadConfig` | 不迁移 |
| `elasticity` | `elasticity` | `Body::ReadConfig` | 不迁移 |
| `frictionStatic` | `frictionStatic` | `Body::ReadConfig` | 不迁移 |
| `frictionDynamic` | `frictionDynamic` | `Body::ReadConfig` | 不迁移 |
| `dampCoeff` | `dampCoeff` | `Body::ReadConfig` | 不迁移 |
| `dampCoeffAngular` | `dampCoeffAngular` | `Body::ReadConfig` | 不迁移 |
| `fixed` | `fixed` | `Body::ReadConfig` | 不迁移 |
| `density` | `density` | `Body::ReadConfig` | 不迁移 |
| `chargeDensity` | `chargeDensity` | `Body::ReadConfig` | 不迁移 |
| `len` | `length` | `Connection::ReadConfig` | 待迁移 |
| `hooke` | 待确认 | `Connection::ReadConfig` | 待确认 |
| `type` | `type` | `Connection::ReadConfig` | 不迁移 |

连接类型值 `rope`、`link`、`spring`、`cord` 是类型选择值而非函数或变量标识符；除非领域术语另行调整，否则保持不变。

### 5.3 创建器与场景构造配置

| 旧名称 | 目标名称 | 读取位置 | 状态 |
| --- | --- | --- | --- |
| `mode` | `mode` | `Creator::ReadConfig` | 不迁移 |
| `thick` | `thickness` | `Creator::ReadConfig` | 待迁移 |
| `rad` | `radius` | `Creator::ReadConfig` | 待迁移 |
| `cfg_body` | `bodyConfiguration` | `Creator::ReadConfig` | 待迁移 |
| `cfg_conn` | `connectionConfiguration` | `Creator::ReadConfig` | 待迁移 |
| `point` | `point` | `scene_builders.cpp` | 不迁移 |
| `repos_o` | 待确认 | `scene_builders.cpp` | 待确认 |
| `idx0` | `index0` | `scene_builders.cpp` | 待迁移 |
| `idx1` | `index1` | `scene_builders.cpp` | 待迁移 |
| `absolute` | `absolute` | `scene_builders.cpp` | 不迁移 |
| `p0` | `point0` | `scene_builders.cpp` | 待迁移 |
| `p1` | `point1` | `scene_builders.cpp` | 待迁移 |

创建模式值 `box`、`ball`、`plate`、`conn`、`point`、`nail`、`particle` 是枚举式数据值。`conn` 应在迁移时改为 `connection`；其余名称在语义不变时可保持 lower camel。

## 6. 执行步骤与验收

1. 先完成所有 `待确认` 名称的领域含义确认，再修改代码或示例场景。
2. 每个命名批次在同一提交中更新 C++ 注册点、读取点和受影响的 `.rigid2d` 示例场景。
3. 不保留旧键、旧函数名或兼容分支。
4. 使用全仓搜索确认旧名称已不再出现在 C++、项目脚本和示例场景文本中；对二进制场景通过加载验证确认更新生效。
5. 验证 `.rigid2d` 的字段顺序、类型和二进制布局未被代码修改，并执行 `Debug|x64` 构建。
