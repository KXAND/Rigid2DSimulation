# 剩余模块风格迁移设计

## 1. 状态说明

本文是除 `framework/control` 以外的模块设计总表，服从 `docs/style-migration-design-protocol.md`。表中列出的名称均已确定；未列出的标识符一律不改，若后续发现新的非豁免缩写，必须先补充本表再实施。

数学公式中局部且行业通用的 `i`、`j`、`x`、`y`、`dx`、`dy`、`p0`、`p1`、`v0`、`v1`、`phi`、矩阵元素 `a00` 等保持不变。外部 API、C/C++ 标准库、Windows SDK、DSL 语法和二进制布局字段不改。

## 2. `framework/basic`

### 已确定名称

| 旧名称 | 目标名称 | 范围 |
| --- | --- | --- |
| `Rect::tl` | `Rect::topLeft` | `Vector2.h` 与所有项目内矩形访问 |
| `Rect::w` | `Rect::width` | 同上 |
| `Rect::h` | `Rect::height` | 同上 |
| `Rect` 构造参数 `tl`、`w`、`h` | `topLeft`、`width`、`height` | 同上 |
| `DRect::w`、`DRect::h` | `width`、`height` | `DVector2.h/.cpp` 与所有项目内访问 |
| `DRect` 构造参数 `w`、`h` | `width`、`height` | 同上 |
| `Camera::vp`、`p`、`scl`、`r` | `viewport`、`position`、`scale`、`radian` | `Vector3.h/.cpp` 与直接调用点 |
| `Camera::min_d`、`max_d`、`ct`、`mt` | `minimumDistance`、`maximumDistance`、`center`、`transformMatrix` | 同上 |

本模块实施边界见第 10 节的 `refactor(basic): clarify geometry names`；`Tile`、`App` 的独立几何字段不包含在该提交。

`DVector2`、`Vector3` 和矩阵的坐标、轴、比例和矩阵元素属于数学类型，保持不变。`Camera::Save` 的序列化写入顺序和字段类型保持不变。

## 3. `framework/app`

### 已确定名称

| 旧名称 | 目标名称 | 范围 |
| --- | --- | --- |
| `LAN_CHI`、`LAN_ENG` | `kChineseLanguage`、`kEnglishLanguage` | `App.h` 及调用点 |
| `App::w`、`App::h` | `width`、`height` | `App` 与所有项目内访问 |
| `App::Run` 的 `ShowConsole` | `showConsole` | 声明与实现 |
| `App::AddLog` 的 `s` | `text` | 声明与实现 |
| `f_log` | `logFile` | `App::AddLog` |
| `f` | `file` | `App::LoadLocalizationFile` |
| `s`、`strs`、`c` | `textBuffer`、`translations`、`character` | `App::LoadLocalizationFile` |
| `wc`、`cr`、`sty`、`rw`、`rh` | `windowClass`、`clientRect`、`windowStyle`、`windowWidth`、`windowHeight` | 窗口注册与创建函数 |

应用状态和消息分发合并为第 10 节的 `refactor(app): clarify application names`。消息参数映射为：`wnd` 到 `windowHandle`、`msg` 到 `message`、`wp` 到 `wParam`、`lp` 到 `lParam`、`pcs` 到 `createStruct`、`cs` 到 `cursor`、`dt` 到 `wheelDelta`。Windows 消息常量和 API 名称保持不变。

## 4. `framework/draw`

### 已确定名称

| 旧名称 | 目标名称 | 范围 |
| --- | --- | --- |
| `DrawRectangleRaw` 的 `w`、`h` | `width`、`height` | 声明、实现和项目内调用点 |
| 椭圆参数 `a`、`b` | `horizontalRadius`、`verticalRadius` | `DrawEllipse` 声明、实现和项目内调用点 |
| `inv_aax`、`inv_aay` | `inverseHorizontalRadiusSquared`、`inverseVerticalRadiusSquared` | `DrawEllipse` |
| `tmp` | `normalizedDistanceSquared` | `DrawEllipse` |
| `DrawTriangleHalf` 的 `dest` | `targetTile` | 声明与实现 |
| `pa`、`pb`、`pc` | `firstPoint`、`middlePoint`、`lastPoint` | `DrawTriangleHalf` 及其调用点 |
| `same_y`、`y_big` | `isYAxisMajor` | 三角形扫描线方向传递 |
| `dp` | `depthIndex` | `DrawTriangleHalf` |

本模块实施边界见第 10 节的 `refactor(draw): clarify primitive geometry names`。扫描线循环的 `x`、`y`、`dx`、`dy` 与矩形裁剪边界 `a`、`b`、`c`、`d` 属于紧凑数学局部范围，保持不变。

## 5. `framework/media`

### 已确定名称

| 旧名称 | 目标名称 | 范围 |
| --- | --- | --- |
| `CHINESE_CHARS` | `chineseCharacters` | `FontCharacters.cpp` |
| `NUMBERS` | `numbers` | 同上 |
| `NORMAL_SYMBOLS` | `normalSymbols` | 同上 |
| `UPPERS`、`LOWERS` | `uppercaseLetters`、`lowercaseLetters` | 同上 |
| `ADDITIONAL_CHINESE_CHARS` | `additionalChineseCharacters` | 同上 |
| `Color::r`、`g`、`b` | `red`、`green`、`blue` | `Tile.h/.cpp` 与所有项目内访问；内存字段顺序不变 |
| `Tile::as` | `alphaValues` | `Tile.h/.cpp` 与所有项目内访问；二进制写入顺序不变 |
| `Tile` 构造参数 `w`、`h`、`c`、`a` | `width`、`height`、`color`、`alpha` | 全部构造函数与直接调用点 |
| `Tile` 缩放构造参数 `src`、`vp_src` | `sourceTile`、`sourceViewport` | 声明、实现和调用点 |
| `Tile` 加载参数 `ok` | `isLoaded` | 声明、实现和调用点 |
| `BlendColor` 参数 `bc`、`ba`、`fc`、`fa` | `backgroundColor`、`backgroundAlpha`、`foregroundColor`、`foregroundAlpha` | 声明、实现和调用点 |
| `Font::h`、`chi`、`fts` | `height`、`chineseCharacters`、`glyphs` | `Font.h/.cpp` 与直接调用点 |
| `Font` 构造参数 `h`、索引和加载参数 `c` | `height`、`character` | 声明与实现 |
| `GenerateRandomText` 的 `n`、`s` | `count`、`text` | `Font.cpp` |
| `LoadFont` 的 `ret`、`t0`、`t1` | `isLoaded`、`sourceTile`、`scaledTile` | `Font.cpp` |
| `sym`、`upp`、`cha` | `normalCharacters`、`uppercaseCharacters`、`chineseCharacterSet` | `Font::Initialize` |
| `AudioClip::Play` 的 `v` | `outputSample` | `AudioClip.cpp` |

字体、图块、颜色和音频局部命名合并为第 10 节的 `refactor(media): clarify media resource names`。`Tile` 的像素与 alpha 二进制布局保持不变；`AudioClip` 已无待迁移缩写。

## 6. `framework/parse`

当前命名迁移已完成；下一批仅允许按文件完成排版、包含顺序和直接依赖审计。若发现新的非豁免名称，必须先补充本节名称对照，不能与纯排版提交混合。

## 7. `current/common`

### 已确定名称

| 旧名称 | 目标名称 | 范围 |
| --- | --- | --- |
| `Background` 成员函数参数 `cur` | `simulationApp` | `Background.h/.cpp` |
| `Background::ViewPort` | `GetViewport` | 声明、实现和项目内调用点 |
| `FindConfigValue`、`TryReadConfigNumber` 的 `cfg` | `configuration` | `Utils.h` |
| `TryReadConfigNumber` 的 `out` | `output` | `Utils.h` |
| `SimulationApp` 保存、加载、更新循环中的 `b`、`c`、`p` | `body`、`connection`、`parameter` | `SimulationApp.cpp` |
| `tmp` | `updateFunction` | `SimulationApp::Update` 的脚本更新调用 |
| `cons0`、`cons1` | `firstBodyConnections`、`secondBodyConnections` | 删除连接时的两个所属刚体连接表 |
| `SimulationApp` 中删除谓词参数 `c`、`b`、`p` | `connection`、`body`、`parameter` | `SimulationApp.cpp` 的 `remove_if` 与作用域写入 lambda |

`SceneParameter` 的序列化局部 `size`、`file` 及 `SimulationApp` 的网格公式变量属于允许的紧凑局部范围，保持不变。`App::width/height` 与 `Rect::width/height` 分别由应用框架和基础数据主题处理，不混入本模块。

## 8. `current/rigid`

### 已确定名称

| 旧名称 | 目标名称 | 范围 |
| --- | --- | --- |
| 函数参数 `cur` | `simulationApp` | 刚体模块所有接收 `SimulationApp&` 的函数与直接调用链 |
| `ReadCfg` | `ReadConfig` | `Body`、`Connection`、`Creator`、`SceneBuilders` 与直接调用点 |
| `Connection::len` | `Connection::length` | 连接字段及项目内访问；DSL 配置键和序列化顺序不变 |
| `Creator::DepthBody`、`DepthConnection` | `bodyDepth`、`connectionDepth` | `Creator` |
| `Creator::thick`、`rad` | `thickness`、`radius` | `Creator` |
| `CREATE_*` | 对应 `kCreate*` 常量 | `Creator` 与调用点 |
| `min_wh`、`min_r`、`min_len` | `minimumSize`、`minimumRadius`、`minimumLength` | `Creator` |
| `thk` | `thickness` | `Creator::UpdatePlate` 与 `Boundary` |
| `shs_cfg`、`cfg` | `shapeConfigurations`、`configuration` | `CreateBody` |
| `bo`、`shs`、`sh`、`o_rel`、`vs` | `body`、`shapes`、`shape`、`relativePosition`、`vertices` | `CreateBody` |
| `bw`、`bh`、`bx0`、`bx1`、`by0`、`by1` | `backgroundWidth`、`backgroundHeight`、`left`、`right`、`top`、`bottom` | `Boundary` |
| `r`、`n`、`h` | `radius`、`toothCount`、`toothHeight` | `Gear` 的参数 |
| `_p0`、`_p1`、`_n`、`cfg_conn` | `point0Value`、`point1Value`、`countValue`、`connectionConfiguration` | `Strand`、`Necklace` 的参数 |
| `b0`、`b1`、`beg`、`len`、`rad`、`col`、`ball_rad` | `previousBody`、`currentBody`、`firstBody`、`segmentLength`、`bodyRadius`、`bodyColor`、`ballRadius` | `Strand`、`Necklace` |
| `_o`、`o` | `centerValue`、`center` | `Necklace` |
| `Body` 构造参数 `_o`、`sh`、`_shs`、`cur` | `position`、`shape`、`shapes`、`simulationApp` | `Body` 构造函数声明、实现和调用点 |
| `Body::ReadCfg` | `ReadConfig` | `Body` 与直接调用点 |
| `Body::Warp` 的 `rc` | `rectangle` | 声明与实现 |
| `Body` 的 `sdt` | `stepDeltaTime` | `Step`、预设跟随和拖拽辅助函数 |
| `Body::selectedConColor` | `selectedConnectionColor` | `Body` 与渲染使用点 |
| `CON_ROPE`、`CON_LINK`、`CON_SPRING`、`CON_CORD` | `kRopeConnection`、`kLinkConnection`、`kSpringConnection`、`kCordConnection` | `Connection`、创建器和场景构建调用点 |
| `Connection` 的 `cur`、`cfg`、`sdt` | `simulationApp`、`configuration`、`stepDeltaTime` | 构造、配置、模拟与渲染接口 |
| `GetOtherBody` 的 `b`、`con` | `body`、`connection` | 声明、实现和调用点 |
| `Collision::d`、`diff`、`n`、`c` | `depth`、`difference`、`normal`、`contactPoint` | `Collision` 及碰撞求解调用点 |
| `collide` | `CollideShapes` | 声明、实现和调用点 |
| 碰撞辅助函数的 `sh0`、`sh1`、`out`、`n`、`o`、`c`、`dsqr_close` | `firstShape`、`secondShape`、`collision`、`normal`、`origin`、`contactPoint`、`closestDistanceSquared` | `Collision.h/.cpp` |
| `findBodiesInGroup` 的 `out`、`b` | `FindBodiesInGroup`、`bodies`、`body` | `Group` 与调用点 |
| `Group::Warp` 的 `Rect` | `rectangle` | 声明与实现 |
| `Shape` 的 `r` | `radius` | `Shape`、刚体和构建函数的直接访问 |
| `Shape` 构造参数 `_r`、`_o_rel`、`_verticesRelative` | `radius`、`relativePosition`、`verticesRelative` | 声明与实现 |
| `GetRectVerticesByCenter` 的 `w`、`h` | `width`、`height` | 声明、实现和项目内调用点 |
| `ShapeBuiltins` 的 `r`、`w`、`h`、`s`、`t`、`b`、`thk`、`n` | `radius`、`width`、`height`、`shear`、`topWidth`、`bottomWidth`、`thickness`、`sideCount` | 声明、实现及 DSL 注册调用 |

本模块按第 10 节的“物理数据”与“创建和场景构建”两个提交实施。所有本节名称均已确定；数学局部量 `p0/p1`、`v0/v1`、`x0/y0/x1/y1`、`phi`、循环下标及 `Aabb` 的坐标边界保持不变。

## 9. `current/ui`

### 已确定名称

| 旧名称 | 目标名称 | 范围 |
| --- | --- | --- |
| 面板构造函数中的 `tmp` 控件列表 | `controls` | 所有 `current/ui` 面板构造函数 |
| `UserInterface` 的 `cur` | `simulationApp` | 构造、`Init`、`Update`、`PreUpdate` 的声明、实现及调用点 |

面板领域成员已在此前迁移。面板构造函数中继承的 `w` 由控件尺寸主题统一迁移为 `width`；资源字符串键和 DSL 文本不改。UI 命名提交只包含上述两项，排版另行按面板文件审计。

## 10. 已确定的实施提交清单

以下列表是本分支命名工作的唯一提交清单，并记录各主题完成状态。列表之外不产生“顺手提交”；纯排版不在当前命名工作中计数，须另行设计。

| 顺序 | 提交信息 | 状态 | 范围 | 不做的工作 |
| --- | --- | --- | --- | --- |
| 1 | `refactor(control): clarify control geometry names` | 已完成 | `framework/control` 的全部控件几何名称和 `current/ui` 直接访问 | 排版、布局或交互逻辑 |
| 2 | `refactor(basic): clarify geometry names` | 已完成 | `Rect`、`DRect`、`Camera` 字段、构造参数和直接访问 | `Tile`、`App` 的几何字段 |
| 3 | `refactor(app): clarify application names` | 待执行 | `App` 状态、本地化局部变量和 Win32 消息分发参数 | 消息流程、窗口创建逻辑 |
| 4 | `refactor(draw): clarify primitive geometry names` | 待执行 | 矩形、椭圆、三角形绘制接口与非数学局部变量 | 绘制算法和扫描线计算 |
| 5 | `refactor(media): clarify media resource names` | 待执行 | 字符集、`Color`、`Tile`、`Font` 与音频输出局部变量 | 图块/音频二进制布局和资源加载顺序 |
| 6 | `refactor(common): clarify scene state names` | 待执行 | `Background`、配置读取工具和 `SimulationApp` 的集合局部变量 | 场景逻辑、网格算法和 DSL 键 |
| 7 | `refactor(rigid): clarify physics data names` | 待执行 | `Body`、`Shape`、`Connection`、`Collision`、`Group` 的字段、接口、常量和直接调用点 | 物理公式、序列化布局和碰撞流程 |
| 8 | `refactor(rigid): clarify scene builder names` | 待执行 | `Creator`、`SceneBuilders`、`ShapeBuiltins` 的参数、局部变量和直接调用点 | 创建流程、DSL 语法和配置键 |
| 9 | `refactor(ui): clarify panel local names` | 待执行 | `UserInterface` 应用参数和各面板控件列表局部变量 | 面板业务逻辑、资源字符串键和排版 |
