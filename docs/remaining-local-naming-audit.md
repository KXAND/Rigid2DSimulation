# 剩余局部命名审计

## 1. 目的与范围

此前的命名迁移设计表覆盖了已选定的主题，但没有穷尽 `current` 与 `framework` 中的局部变量、参数和成员缩写。因此，当前代码库不能宣称已完全符合 `docs/code-style.md`。

本文件是下一轮局部命名整改的唯一设计表，覆盖全项目 C++ 源文件中可直接确定语义的变量、参数和成员，以及 `localization.txt` 的项目资源键；不涉及文件、类型、函数、DSL 键、序列化字段、控制流或排版。计数以声明绑定为单位；同一参数在头文件声明和实现定义中只计一次。

当前已确定 **199 个 C++ 声明绑定**和 **65 个本地化资源键**的映射。此前待确认的 C++ 候选已全部完成审阅；未采用的名称及其原因记录在第 4 节，不得在实施时顺手更改。

## 2. 已确定映射

| 模块 | 文件 | 旧名称 | 绑定数 | 目标名称 | 说明 |
| --- | --- | --- | ---: | --- | --- |
| AABB | `current/rigid/Aabb.h/.cpp` | 构造函数的 `b0`、`b1` | 2 | `firstBox`、`secondBox` | 合并两个包围盒 |
| AABB | `current/rigid/Aabb.h/.cpp` | `Expand` 的 `v` | 1 | `point` | 扩展包围盒的点 |
| AABB | `current/rigid/Aabb.h` | `IsOverlap` 的 `b0`、`b1` | 2 | `box0`、`box1` | 与实现中的参数名一致 |
| AABB | `current/rigid/Aabb.cpp` | `IsOverlap` 的 `bx`、`by` | 2 | `hasXOverlap`、`hasYOverlap` | 两个坐标轴的重叠状态 |
| 场景模拟 | `current/common/SimulationApp.cpp` | 遍历刚体的 `b` | 6 | `body` | 刚体集合元素 |
| 场景模拟 | `current/common/SimulationApp.cpp` | 遍历连接的 `c` | 2 | `connection` | 连接集合元素 |
| 场景模拟 | `current/common/SimulationApp.cpp` | 遍历分组的 `g` | 1 | `group` | 分组集合元素 |
| 场景模拟 | `current/common/SimulationApp.cpp` | 碰撞网格遍历的 `pb1` | 5 | `otherBody` | 与当前刚体碰撞的另一刚体 |
| 场景模拟 | `current/common/SimulationApp.cpp` | `sdt` | 1 | `stepDeltaTime` | 单次物理子步时长 |
| 场景模拟 | `current/common/SimulationApp.cpp` | `connnection` | 1 | `connection` | 修正拼写错误 |
| 场景模拟 | `current/common/SimulationApp.cpp` | `bo` | 1 | `body` | 碰撞遍历中的刚体 |
| 场景模拟 | `current/common/SimulationApp.h/.cpp` | `SetConfig` 的 `v` | 1 | `config` | 场景配置值 |
| 刚体 | `current/rigid/Body.cpp` | `sh` | 3 | `shape` | 形状遍历元素 |
| 刚体 | `current/rigid/Body.cpp` | `body_cfg` | 1 | `bodyConfig` | 脚本求值后的刚体配置 |
| 刚体 | `current/rigid/Body.h/.cpp` | `ReadConfig` 的 `cfg` | 1 | `config` | 刚体配置值 |
| 刚体 | `current/rigid/Body.cpp` | `top_left` | 1 | `topLeft` | 场景左上角 |
| 刚体 | `current/rigid/Body.cpp` | `sub_center` | 1 | `shapeCenter` | 单个形状的面积中心 |
| 刚体 | `current/rigid/Body.cpp` | `tri_area` | 1 | `triangleArea` | 三角形面积 |
| 刚体 | `current/rigid/Body.cpp` | `tri_inertia` | 1 | `triangleInertia` | 三角形转动惯量贡献 |
| 刚体 | `current/rigid/Body.cpp` | 顶点遍历的 `v` | 1 | `vertex` | 形状局部顶点 |
| 刚体 | `current/rigid/Body.cpp` | `ret` | 3 | `hasReturned` | DSL 执行的返回状态 |
| 刚体 | `current/rigid/Body.cpp` | `sc` | 1 | `scope` | DSL 局部作用域 |
| 刚体 | `current/rigid/Body.cpp` | `stepDt` | 3 | `stepDeltaTime` | 刚体更新及预设跟随的子步时长 |
| 刚体 | `current/rigid/Body.cpp` | `PreUpdate` 的 `ok` | 1 | `canReceiveHover` | 当前刚体可成为鼠标悬停对象 |
| 刚体 | `current/rigid/Body.h/.cpp` | `Electrostatic` 的 `b0`、`b1`、`sdt` | 3 | `firstBody`、`secondBody`、`stepDeltaTime` | 静电计算的两个刚体和子步时长 |
| 连接 | `current/rigid/Connection.cpp` | `md` | 1 | `typeValue` | 连接类型配置值 |
| 连接 | `current/rigid/Connection.cpp` | `s` | 1 | `typeName` | 连接类型字符串 |
| 连接 | `current/rigid/Connection.cpp` | 保存和加载的 `ind0`、`ind1` | 4 | `bodyIndex0`、`bodyIndex1` | 两个刚体在 `bodies` 中的序列化索引；不改变二进制布局 |
| 连接 | `current/rigid/Connection.cpp` | `currLenth` | 2 | `currentLen` | 当前连接长度；同时修正拼写 |
| 连接 | `current/rigid/Connection.cpp` | `torgue0`、`torgue1` | 2 | `torque0`、`torque1` | 只修正拼写；保持现有未使用状态和计算语句不变 |
| 创建器 | `current/rigid/Creator.h/.cpp` | 构造函数的 `v` | 1 | `config` | 创建器配置值 |
| 创建器 | `current/rigid/Creator.cpp` | `md` | 1 | `modeValue` | 创建模式配置值 |
| 创建器 | `current/rigid/Creator.cpp` | `s` | 1 | `modeName` | 创建模式字符串 |
| 创建器 | `current/rigid/Creator.cpp` | `r` | 1 | `dragRadius` | 创建球体时的拖拽半径 |
| 创建器 | `current/rigid/Creator.cpp` | `v` | 1 | `dragVector` | 创建板时的拖拽向量 |
| 创建器 | `current/rigid/Creator.cpp` | `len` | 1 | `dragLen` | 创建板时的拖拽长度 |
| 分组 | `current/rigid/Group.cpp` | 遍历刚体的 `b` | 10 | `body` | 分组中的刚体元素 |
| 分组 | `current/rigid/Group.cpp` | `connnection` | 1 | `connection` | 修正拼写错误 |
| 分组 | `current/rigid/Group.cpp` | `out` | 1 | `groups` | 分组构建结果 |
| 碰撞 | `current/rigid/Collision.h/.cpp` | 碰撞深度辅助函数的 `sh` | 2 | `shape` | 形状参数 |
| 碰撞 | `current/rigid/Collision.cpp` | 顶点遍历的 `v` | 2 | `vertex` | 被检查的顶点 |
| 碰撞 | `current/rigid/Collision.cpp` | `d` | 3 | `depth` | 碰撞深度 |
| 碰撞 | `current/rigid/Collision.cpp` | `o10` | 1 | `centerOffset` | 两个圆心的位置差 |
| 碰撞 | `current/rigid/Collision.cpp` | `out` | 1 | `minimumProjection` | 多边形顶点在法线方向上的最小投影 |
| 场景构建 | `current/rigid/SceneBuilders.cpp` | 形状配置遍历的 `v` | 1 | `shapeConfig` | 单个形状配置 |
| 场景构建 | `current/rigid/SceneBuilders.cpp` | 顶点配置遍历的 `w` | 1 | `vertexValue` | 单个顶点的 DSL 值 |
| 场景构建 | `current/rigid/SceneBuilders.cpp` | 齿尖的 `v` | 1 | `toothTip` | 当前齿形三角形的外侧顶点 |
| 场景构建 | `current/rigid/SceneBuilders.cpp` | `n` | 2 | `bodyCount` | `Strand`、`Necklace` 创建的刚体数量 |
| 形状 | `current/rigid/Shape.cpp` | 包围盒遍历的 `v` | 1 | `vertex` | 形状顶点 |
| 形状 | `current/rigid/Shape.h/.cpp` | `IsInsideTriangle` 的 `v` | 1 | `point` | 待判断的点 |
| 形状内建 | `current/rigid/ShapeBuiltins.cpp` | `vs` | 7 | `shapeConfigs` | 返回的形状配置集合 |
| 形状内建 | `current/rigid/ShapeBuiltins.cpp` | `v` | 7 | `shapeConfig` | 单个形状配置 |
| 形状内建 | `current/rigid/ShapeBuiltins.cpp` | `m` | 1 | `polygonSideCount` | 归一化后的正多边形边数 |
| 刚体 | `current/rigid/Body.cpp` | `inv_i0` | 1 | `effectiveInvMass` | 拖拽点冲量计算的有效逆质量 |
| 碰撞 | `current/rigid/Collision.cpp` | `inverse_i0`、`inverse_i1` | 2 | `normalInvMass0`、`normalInvMass1` | 法线方向的有效逆质量 |
| 碰撞 | `current/rigid/Collision.cpp` | `inverse_it0`、`inverse_it1` | 2 | `tangentInvMass0`、`tangentInvMass1` | 切线方向的有效逆质量 |
| 全局面板 | `current/ui/GlobalPanel.cpp` | `s` | 1 | `modeName` | 当前创建模式的本地化名称 |
| 文本框 | `framework/control/TextBox.h/.cpp` | `cursor_change` | 1 | `isCursorChanged` | 光标位置是否在当前帧发生变化 |
| 文本框 | `framework/control/TextBox.cpp` | `show_cursor` | 1 | `isCursorVisible` | 当前帧是否绘制光标 |
| 文本框 | `framework/control/TextBox.cpp` | 文本范围的 `a`、`b` | 10 | `rangeBegin`、`rangeEnd` | 左侧、选中、右侧文本及删除范围的迭代器 |
| 文本框 | `framework/control/TextBox.cpp` | `val` | 1 | `cursorOverflow` | 鼠标相对编辑边界的溢出量 |
| 文本框 | `framework/control/TextBox.cpp` | `ok` | 1 | `canReceivePointer` | 文本框是否可取得鼠标悬停所有权 |
| 控件 | `framework/control/Button.cpp`、`CheckBox.cpp`、`DragBar.cpp`、`DropList.cpp`、`ScrollBar.cpp` | `ok` | 5 | `canReceivePointer` | 控件是否可取得鼠标悬停所有权 |
| 富文本框 | `framework/control/RichTextBox.cpp` | 两处 `ok` | 2 | `canReceivePointer`、`canReceiveWheel` | 分别判断悬停和滚轮所有权 |
| 绘制 | `framework/draw/DrawGeometry.cpp` | 三组 `a`、`b`、`c`、`d` | 12 | `left`、`right`、`top`、`bottom` | 矩形或变换矩形的裁剪边界 |
| 绘制 | `framework/draw/DrawGeometry.cpp` | `v` | 1 | `offset` | 椭圆中心到当前像素的偏移量 |
| 绘制 | `framework/draw/DrawTile.cpp` | 两组 `l`、`r`、`t`、`b` | 8 | `left`、`right`、`top`、`bottom` | 图块裁剪边界 |
| 绘制 | `framework/draw/DrawCurve.h/.cpp` | `axisX`、`axisY`、`a`、`b`、`n` | 3 | `horizontalRadius`、`verticalRadius`、`segmentCount` | 椭圆边框的两条半径和分段数 |
| 绘制 | `framework/draw/DrawGeometry.h` | `axisX`、`axisY` | 2 | `horizontalRadius`、`verticalRadius` | 与 `DrawEllipse` 实现的参数名一致 |
| 绘制 | `framework/draw/DrawTriangle.cpp` | `xb`、`xc` | 2 | `middleEdgeX`、`lastEdgeX` | 扫描线与两条三角形边的交点横坐标 |
| 绘制 | `framework/draw` 的公开函数及三角形裁剪辅助函数 | `viewPort` | 13 | `viewport` | 统一为单词 `viewport`；同步头文件、实现和调用点 |
| 绘制 | `framework/draw/DrawTriangle.h` | `DrawTriangleHalf` 的 `d` | 1 | `depth` | 与实现和调用点的参数名一致 |
| 媒体 | `framework/media/Font.cpp` | `s` | 1 | `text` | 随机生成的文本 |
| 媒体 | `framework/media/Tile.h/.cpp` | `Color` 构造函数的 `s`、`i` | 2 | `text`、`position` | 颜色文本及其读取位置 |
| 媒体 | `framework/media/Tile.h/.cpp` 及调用点 | `Color::RED` 等八个全大写具名常量 | 8 | `Color::kRed` 至 `Color::kBlack` | 与具名常量的 `kPascalCase` 规则一致 |
| 用户界面 | `current/ui/UserInterface.h/.cpp` | `fpns`、`fp` | 16 | `floatPanels`、`floatPanel` | 浮动面板集合及其遍历元素 |
| 刚体 | `current/rigid/Body.cpp` | `Body::Init` 的 `shapes` | 1 | `shape` | 形状集合中的单个元素，避免与容器同名 |
| 用户界面 | `current/ui/BodyPanel.cpp`、`ConnectionPanel.cpp` | 遍历控件的 `c` | 2 | `control` | 控件集合中的单个元素 |
| 形状内建 | `current/rigid/ShapeBuiltins.cpp` | `a0` 至 `a3` | 4 | `topOuterRight`、`topOuterLeft`、`topInnerLeft`、`topInnerRight` | 框架盒上边多边形的四个顶点 |
| 形状内建 | `current/rigid/ShapeBuiltins.cpp` | `b0` 至 `b3` | 4 | `bottomInnerRight`、`bottomInnerLeft`、`bottomOuterLeft`、`bottomOuterRight` | 框架盒下边多边形的四个顶点 |
| 形状内建 | `current/rigid/ShapeBuiltins.cpp` | `c0` 至 `c3` | 4 | `rightOuterTop`、`rightOuterBottom`、`rightInnerBottom`、`rightInnerTop` | 框架盒右边多边形的四个顶点 |
| 形状内建 | `current/rigid/ShapeBuiltins.cpp` | `d0` 至 `d3` | 4 | `leftInnerTop`、`leftInnerBottom`、`leftOuterBottom`、`leftOuterTop` | 框架盒左边多边形的四个顶点 |
| 碰撞 | `current/rigid/Collision.cpp` | `tanC1`、`tanC2` | 2 | `contactOffsetPerpendicular0`、`contactOffsetPerpendicular1` | 两个刚体从质心到接触点的偏移向量旋转 90 度后的结果 |
| 碰撞 | `current/rigid/Collision.cpp` | `tanC0normal`、`tanC1normal` | 2 | `normalMomentArm0`、`normalMomentArm1` | 接触偏移在法线方向上的有符号力矩臂 |
| 碰撞 | `current/rigid/Collision.cpp` | `tanCtan0`、`tanCtan1` | 2 | `tangentMomentArm0`、`tangentMomentArm1` | 接触偏移在切线方向上的有符号力矩臂 |

## 3. 本地化资源键映射

本地化键是项目内部资源标识符，统一采用 lower camel。以下 65 个键必须在 `localization.txt` 与全部 `GetLocalizedString`、`Localized*` 调用点中同时修改；不保留旧键。键的迁移必须保持每个键现有的翻译文本和载入顺序不变。`about_txt` 是多行值的起始键，迁移时只改其首行键名，不改后续文本内容或分隔格式。

当前 C++ 已部分使用目标键 `saveButton`、`loadButton`、`timeLabel`、`mousePositionLabel`、`resetButton`，但 `localization.txt` 仍保留旧键；实施时必须在同一提交中补齐资源文件，不能保留这种不匹配状态。`addButton` 也已在调用点出现，但它不是最终名称，必须按下表改为 `addParameterButton`。

```text
frm_tm -> frameTimeLabel
bt_save -> saveButton
bt_load -> loadButton
fpn_global -> globalPanelTitle
fpn_cmd -> commandPanelTitle
unnamed_par -> unnamedParameter
bt_del_par -> deleteParameterButton
bt_add_par -> addParameterButton
fpn_par -> parameterPanelTitle
fpn_lang -> languagePanelTitle
fpn_about -> aboutPanelTitle
bt_body_ok -> applyBodyButton
fpn_body -> bodyPanelTitle
cb_body_con -> selectBodyConnection
bt_con_ok -> applyConnectionButton
fpn_con -> connectionPanelTitle
mode_drag -> dragMode
mode_sel -> selectMode
mode_create -> createMode
mode_del -> deleteMode
disp_col -> colorDisplay
disp_energy -> energyDisplay
disp_charge -> chargeDisplay
disp_none -> noDisplay
body_not_sel -> noBodySelected
body_fixed -> bodyFixed
body_o -> bodyPosition
body_v -> bodyVelocity
body_ang -> bodyAngle
body_v_ang -> bodyAngularVelocity
body_density -> bodyDensity
body_charge_density -> bodyChargeDensity
body_e -> bodyElasticity
body_mu_s -> bodyStaticFriction
body_mu_d -> bodyDynamicFriction
body_damp_v -> bodyVelocityDamping
body_damp_v_ang -> bodyAngularVelocityDamping
conn_not_sel -> noConnectionSelected
conn_type -> connectionType
conn_rope -> ropeConnection
conn_link -> linkConnection
conn_spring -> springConnection
conn_cord -> cordConnection
conn_len -> connectionLength
conn_hooke -> springConstant
n_step -> stepCount
s_grid -> gridSize
max_real_dt -> maxRealDeltaTime
tl_scene -> sceneTopLeft
w_scene -> sceneWidth
h_scene -> sceneHeight
creator_mode -> creatorMode
crt_ball -> createBall
crt_box -> createBox
crt_conn -> createConnection
crt_nail -> createNail
crt_particle -> createParticle
crt_plate -> createPlate
crt_point -> createPoint
bt_body_del -> deleteBodyButton
bt_con_del -> deleteConnectionButton
lb_time -> timeLabel
lb_mouse -> mousePositionLabel
bt_reset -> resetButton
about_txt -> aboutText
```

## 4. 明确不在本轮修改的名称

下列名称已核对，不得在实施时顺手更改：

- 数学坐标、循环下标和公式中的局部量：`x`、`y`、`i`、`j`、`p`、`q`、`r0`、`r1`、`v0`、`v1`、`x0`、`y0` 等。它们只在紧凑的几何或物理公式中使用，符合 `docs/code-style.md` 的数学例外。此规则包括 `Body::IsInside`、`Shape::IsInside` 的 `p`，`Body` 拖拽和轨迹代码的 `p0`、`p1`、`pPos`、`pmDir`、`opDir`、`rd0`、`d`、`k`、`r0`，`Electrostatic` 的 `dsqr`、`d`、`je`，`Connection` 绳/连杆模拟的 `d`，以及 `Collision` 位置修正的 `sum`。
- `Connection` 的 `p0`、`p1`、`p0Relative`、`p1Relative`：与 `body0`、`body1` 成对表示两个连接端点及其相对位置，并直接参与连接约束计算；改名收益不足，不纳入本轮。
- `tmp`：按当前约定允许保留。本轮不将其替换为模板函数、宏或通用工具函数。
- `a`、`b`：`SimulationApp::DetectCollisions` 中仅表示网格横、纵坐标，属于数学坐标例外，不等同于表中的刚体遍历变量 `b`。
- `torgue0`、`torgue1`：只改正标识符拼写，不删除、不补用，也不重新推导力矩公式。
- 紧邻 `find` 调用的 `it`：`current/common/Utils.h` 和 `framework/media/Font.cpp` 中均直接以该迭代器完成 `end` 比较或取值，属于 C++ 容器查找的惯用局部名。
- DSL 函数名、配置键、作用域键、`.rigid2d` 二进制读写顺序和平台 API：均不是 C++ 局部命名整改对象。

## 5. 候选审阅结论

此前待确认的 34 个绑定均已审阅：13 个已移入第 2 节，另 21 个因属于公式局部量或成对端点记号而移入第 4 节。本次审阅还核对了 `Shape::IsInside` 的 `p` 以及 `Connection` 的 `p0Relative`、`p1Relative`，两者同样属于第 4 节的明确豁免范围。后续复查发现的 `Color` 具名常量、浮动面板集合和循环元素名已补入第 2 节；当前没有未确认的 C++ 命名候选。

## 6. 实施边界与验收

1. 仅实施第 2、3 节的已确认映射，引用点随声明同步修改；不得加入未列出的改名。
2. 不修改物理计算、条件、循环边界、求值次数、资源生命周期、调用顺序、DSL 语法、配置键或二进制布局。
3. 本文、`docs/code-style.md` 的相关规则、`localization.txt` 和对应 C++ 更名必须进入同一个 `refactor:` 提交；本轮只产生一个提交。
4. 改动前后均构建 `Debug|x64`；确认 C++ 文件保持 GBK 和 CRLF，文档保持 UTF-8 和 CRLF；最后执行 `git diff --check`。
5. 提交信息使用 `refactor: clarify remaining local variable names`。
