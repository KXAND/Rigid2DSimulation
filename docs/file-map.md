# 代码文件职责表

说明：下面只列代码与构建相关文件，不展开字体 tile、音频 clip、示例 `.rigid2d` 场景等资源文件。

## 1. 工程与入口

- `Rigid2DSimulation.sln`: Visual Studio 解决方案入口。
- `Rigid2DSimulation/Rigid2DSimulation.vcxproj`: C++ 工程文件，列出编译单元、平台配置、链接选项。
- `Rigid2DSimulation/Rigid2DSimulation.vcxproj.filters`: VS 工程过滤器定义，只影响 IDE 展示。
- `Rigid2DSimulation/framework/*` 与 `Rigid2DSimulation/current/*`: 当前源码目录与解决方案过滤器已经对齐，前者偏基础框架，后者偏当前业务代码。
- `Rigid2DSimulation/main.cpp`: 程序入口，创建 `Cur` 并启动主循环。
- `Rigid2DSimulation/framework/basic/include.h`: 标准库与 Win32 头集中入口，顺带导出常用 `using`。
- `Rigid2DSimulation/framework/basic/vptr.h`: 一个带深拷贝语义的轻量指针包装。

## 2. 应用壳层与系统接口

- `Rigid2DSimulation/framework/app/app.h` / `Rigid2DSimulation/framework/app/app.cpp`: 应用框架主体，管理窗口、DIB 显示、字体、本地化、音频初始化和主循环。
- `Rigid2DSimulation/framework/app/app_msg.cpp`: Win32 消息处理，把键鼠事件写入 `AppInput`。
- `Rigid2DSimulation/framework/app/app_item.h` / `Rigid2DSimulation/framework/app/app_item.cpp`: 输入状态、帧率统计、hover/keyboard owner 管理。
- `Rigid2DSimulation/framework/app/app_wv.h` / `Rigid2DSimulation/framework/app/app_wv.cpp`: `waveOut` 音频输出缓冲管理。
- `Rigid2DSimulation/framework/media/clip.h` / `Rigid2DSimulation/framework/media/clip.cpp`: 自定义 `.clip` 音频资源读写与混音。
- `Rigid2DSimulation/framework/basic/utils.h` / `Rigid2DSimulation/framework/basic/utils.cpp`: 随机数、字符串转换、文件打开、控制台、剪贴板、数值解析等通用工具。

## 3. 数学、基础数据与像素缓冲

- `Rigid2DSimulation/framework/basic/dvec.h` / `Rigid2DSimulation/framework/basic/dvec.cpp`: 整数二维向量与整数矩形，主要给像素/UI 坐标使用。
- `Rigid2DSimulation/framework/basic/vector2.h` / `Rigid2DSimulation/framework/basic/vector2.cpp`: 双精度二维向量、二维矩阵、贝塞尔与点线距离工具。
- `Rigid2DSimulation/framework/basic/vector3.h` / `Rigid2DSimulation/framework/basic/vector3.cpp`: 三维向量、三维矩阵、相机工具，主要给颜色和少量几何计算使用。
- `Rigid2DSimulation/current/rigid/aabb.h` / `Rigid2DSimulation/current/rigid/aabb.cpp`: 轴对齐包围盒与重叠判断。
- `Rigid2DSimulation/framework/media/tile.h` / `Rigid2DSimulation/framework/media/tile.cpp`: 像素 tile、alpha 缓冲和 tile 文件读写。
- `Rigid2DSimulation/framework/media/font.h` / `Rigid2DSimulation/framework/media/font.cpp`: 基于 tile 文件的位图字体加载与字符查询。
- `Rigid2DSimulation/framework/media/ft_constant.cpp`: 字体字符集常量定义，包含可加载的数字、英文、中文字符表。

## 4. 自绘渲染模块

- `Rigid2DSimulation/framework/draw/draw_geo.h` / `Rigid2DSimulation/framework/draw/draw_geo.cpp`: 矩形、旋转矩形、椭圆等基础图元绘制。
- `Rigid2DSimulation/framework/draw/draw_comp.h` / `Rigid2DSimulation/framework/draw/draw_comp.cpp`: 带边框矩形等组合控件图元绘制。
- `Rigid2DSimulation/framework/draw/draw_curve.h` / `Rigid2DSimulation/framework/draw/draw_curve.cpp`: 椭圆边框曲线绘制。
- `Rigid2DSimulation/framework/draw/draw_px_seg.h` / `Rigid2DSimulation/framework/draw/draw_px_seg.cpp`: 像素线段绘制与裁剪。
- `Rigid2DSimulation/framework/draw/draw_str.h` / `Rigid2DSimulation/framework/draw/draw_str.cpp`: 基于位图字体的字符串排版与点击定位。
- `Rigid2DSimulation/framework/draw/draw_tile.h` / `Rigid2DSimulation/framework/draw/draw_tile.cpp`: tile 拷贝、带深度 tile 绘制、tile 命中检测。
- `Rigid2DSimulation/framework/draw/draw_tri.h` / `Rigid2DSimulation/framework/draw/draw_tri.cpp`: 三角形填充，是多边形渲染基础。

## 5. 脚本语言：词法、语法、执行

- `Rigid2DSimulation/framework/parse/token.h` / `Rigid2DSimulation/framework/parse/token.cpp`: 词法分析，把源码切成 token。
- `Rigid2DSimulation/framework/parse/bracket_group.h` / `Rigid2DSimulation/framework/parse/bracket_group.cpp`: 括号层级分析，把 token 序列组织成嵌套结构。
- `Rigid2DSimulation/framework/parse/script_expr.h` / `Rigid2DSimulation/framework/parse/script_expr.cpp`: 表达式 AST 与解析。
- `Rigid2DSimulation/framework/parse/script_statement.h` / `Rigid2DSimulation/framework/parse/script_statement.cpp`: 语句 AST 与解析，覆盖赋值、函数定义、`if`、`while`、`return`。
- `Rigid2DSimulation/framework/parse/var.h` / `Rigid2DSimulation/framework/parse/var.cpp`: 动态值类型、作用域查找、脚本执行器和 `Compile` 入口。
- `Rigid2DSimulation/current/common/scene_parameter.h` / `Rigid2DSimulation/current/common/scene_parameter.cpp`: 用户参数对象，支持保存/加载。

## 6. 场景、物理与创建逻辑

- `Rigid2DSimulation/current/common/simulation_app.h` / `Rigid2DSimulation/current/common/simulation_app.cpp`: 全项目核心状态与主调度器，统筹脚本、物理、UI、音频、序列化、输入模式。
- `Rigid2DSimulation/current/common/background.h` / `Rigid2DSimulation/current/common/background.cpp`: 背景区域、视口、背景 tile 与背景 hover/wheel owner。
- `Rigid2DSimulation/current/rigid/shape.h` / `Rigid2DSimulation/current/rigid/shape.cpp`: 圆与凸多边形几何表示、点内测试、随机点生成、包围盒计算。
- `Rigid2DSimulation/current/rigid/body.h` / `Rigid2DSimulation/current/rigid/body.cpp`: 刚体核心对象，负责质量属性、积分、拖拽、渲染、脚本配置刷新、序列化。
- `Rigid2DSimulation/current/rigid/collision.h` / `Rigid2DSimulation/current/rigid/collision.cpp`: 碰撞检测结果与碰撞冲量/摩擦求解。
- `Rigid2DSimulation/current/rigid/connection.h` / `Rigid2DSimulation/current/rigid/connection.cpp`: 连接件对象，支持 rope/link/spring/cord 四种约束。
- `Rigid2DSimulation/current/rigid/group.h` / `Rigid2DSimulation/current/rigid/group.cpp`: 基于连接关系形成 body group，用于整体穿越场景边界。
- `Rigid2DSimulation/current/rigid/creator.h` / `Rigid2DSimulation/current/rigid/creator.cpp`: 鼠标交互式创建 box/ball/plate/particle/point/conn/nail。
- `Rigid2DSimulation/current/rigid/comp.h` / `Rigid2DSimulation/current/rigid/comp.cpp`: 脚本侧组合构建器，提供 `box`、`ball`、`gear`、`strand`、`necklace` 等构造能力。

## 7. UI 框架基础

- `Rigid2DSimulation/framework/control/control.h` / `Rigid2DSimulation/framework/control/control.cpp`: 控件基类与 `Delegate` 包装。
- `Rigid2DSimulation/framework/control/ctrl_list.h` / `Rigid2DSimulation/framework/control/ctrl_list.cpp`: 横向/纵向控件线性布局容器。
- `Rigid2DSimulation/framework/control/scrollbar.h` / `Rigid2DSimulation/framework/control/scrollbar.cpp`: 纵向滚动条，不继承 `Control`，由 `Panel/FloatPanel` 组合使用。
- `Rigid2DSimulation/framework/control/panel.h` / `Rigid2DSimulation/framework/control/panel.cpp`: 固定面板容器，用于左侧菜单和底栏。
- `Rigid2DSimulation/framework/control/float_panel.h` / `Rigid2DSimulation/framework/control/float_panel.cpp`: 可显示/隐藏、可拖动的浮动面板容器。
- `Rigid2DSimulation/framework/control/label.h` / `Rigid2DSimulation/framework/control/label.cpp`: 单行文本标签。
- `Rigid2DSimulation/framework/control/rich_label.h` / `Rigid2DSimulation/framework/control/rich_label.cpp`: 多行文本标签。
- `Rigid2DSimulation/framework/control/textbox.h` / `Rigid2DSimulation/framework/control/textbox.cpp`: 单行文本框，支持编辑、选择、复制粘贴。
- `Rigid2DSimulation/framework/control/rich_textbox.h` / `Rigid2DSimulation/framework/control/rich_textbox.cpp`: 多行文本框，服务脚本编辑和多行内容输入。
- `Rigid2DSimulation/framework/control/button.h` / `Rigid2DSimulation/framework/control/button.cpp`: 按钮基类、本地化按钮、浮窗开关按钮。
- `Rigid2DSimulation/framework/control/checkbox.h` / `Rigid2DSimulation/framework/control/checkbox.cpp`: 复选框控件。
- `Rigid2DSimulation/framework/control/dragbar.h` / `Rigid2DSimulation/framework/control/dragbar.cpp`: 横向拖条控件。
- `Rigid2DSimulation/framework/control/droplist.h` / `Rigid2DSimulation/framework/control/droplist.cpp`: 下拉列表控件。
- `Rigid2DSimulation/framework/control/separator.h` / `Rigid2DSimulation/framework/control/separator.cpp`: 分隔线控件。

## 8. UI 页面装配

- `Rigid2DSimulation/current/ui/ui.h` / `Rigid2DSimulation/current/ui/ui.cpp`: UI 总装配器，创建并调度所有浮动面板和固定面板。
- `Rigid2DSimulation/current/ui/fpn_global.h` / `Rigid2DSimulation/current/ui/fpn_global.cpp`: 全局面板，负责存档、读档和全局状态信息展示。
- `Rigid2DSimulation/current/ui/fpn_body.h` / `Rigid2DSimulation/current/ui/fpn_body.cpp`: body 详情面板，支持查看、编辑脚本、删除和选择关联 connection。
- `Rigid2DSimulation/current/ui/fpn_con.h` / `Rigid2DSimulation/current/ui/fpn_con.cpp`: connection 详情面板，支持查看、编辑脚本和删除。
- `Rigid2DSimulation/current/ui/fpn_cmd.h` / `Rigid2DSimulation/current/ui/fpn_cmd.cpp`: 场景脚本编辑面板，负责编译/应用全局命令文本。
- `Rigid2DSimulation/current/ui/fpn_par.h` / `Rigid2DSimulation/current/ui/fpn_par.cpp`: 参数面板，编辑 `param` 列表。
- `Rigid2DSimulation/current/ui/fpn_lang.h` / `Rigid2DSimulation/current/ui/fpn_lang.cpp`: 语言切换面板。
- `Rigid2DSimulation/current/ui/fpn_about.h` / `Rigid2DSimulation/current/ui/fpn_about.cpp`: 关于面板，展示说明文本。
- `Rigid2DSimulation/current/ui/pn_menu.h` / `Rigid2DSimulation/current/ui/pn_menu.cpp`: 左侧主菜单，聚合暂停、重置、静音、模式切换、显示模式切换和各浮窗入口。
- `Rigid2DSimulation/current/ui/pn_bottom.h` / `Rigid2DSimulation/current/ui/pn_bottom.cpp`: 底部状态栏，显示 FPS、模拟时间、鼠标坐标等。

## 9. 需要单独注意的文件

- `Rigid2DSimulation/current/common/simulation_app.cpp`: 核心调度最集中，也是后续重构最先应该拆的文件。
- `Rigid2DSimulation/current/rigid/body.cpp`: 体量最大，物理、交互、渲染、脚本混在一起。
- `Rigid2DSimulation/current/rigid/comp.cpp`: 组合构建函数很多，是脚本能力的主要“标准库”。
- `Rigid2DSimulation/framework/control/rich_textbox.cpp`: 多行编辑器逻辑集中，UI 复杂度最高的控件之一。
- `Rigid2DSimulation/localization.txt`: 不是代码，但几乎所有 UI 文本都依赖它。

