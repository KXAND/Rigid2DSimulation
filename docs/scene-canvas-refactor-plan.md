# Background 职责收敛为场景画布

## 已确认的范围

本轮只调整原 `Background` 的职责，不继续拆分 UI，不改目录分类，不重构物理、DSL 或选择状态的所有权。

| 原职责 | 实施位置 |
| --- | --- |
| 位置、尺寸、裁剪矩形、背景图 | `current/common/SceneCanvas.h/.cpp`，替换原文件和类型 |
| 鼠标悬停及滚轮输入优先级登记 | `SceneCanvas::PreUpdate` |
| 空白区域点击判定 | `SceneCanvas::IsEmptyAreaClicked`，只报告结果 |
| 清空刚体、连接选择 | `SimulationApp::ClearSelection` |
| 调试文字绘制 | `SimulationApp::Update` 中原来的绘制位置 |
| 根据显示范围初始化物理范围 | 保留在 `SimulationApp::Reset` |

表中源码路径相对 `Rigid2DSimulation/`。`SimulationApp::background` 更名为 `sceneCanvas`；所有裁剪范围、场景构建调用和 Visual Studio 工程引用同步迁移，不保留旧类型兼容层。

## 接口与行为约束

- 画布构造不再接收未使用的 `SimulationApp&`；仍由应用持有共享指针，不改变创建时机和生命周期。
- `Render(Tile&) const` 只绘制底图，不读应用状态。
- `PreUpdate(AppInput const&, InputOwners&)` 保留原有悬停、滚轮深度比较和登记行为。
- `IsEmptyAreaClicked(AppInput&, InputOwners const&) const` 检查最终悬停归属及左键按下边沿。输入参数暂用非 const 引用，以使用现有非 const 的 `IsMouseClickedDown`，不扩展修改框架接口。
- 原 `hovered`、`wheeled` 缓存没有外部读取者，移除；点击判定直接读取最终输入归属。
- 必须先完成画布、创建工具、刚体、UI 的 `PreUpdate`，再查询空白点击。不能把区域内点击直接当作空白点击。
- 保留取消选择发生在对象清理之后、刚体更新之前的顺序，不增加模式限制。
- 底图和调试文字仍在原来的顺序及位置绘制，保留文字颜色、深度、坐标、宽度和裁剪参数。
- 默认位置 `(170, 0)`、尺寸 `1630 × 860`、深度 `-100000` 和黑色背景不变。
- `backgroundLeft`、`backgroundTop`、`backgroundWidth`、`backgroundHeight` 脚本键保持不变。`rectScene` 仍独立存在，边界构建仍取画布尺寸。
- 不新增选择管理器、控制器或事件总线；不改物理算法、存储格式、脚本语法。
- C++ 保持 UTF-8 BOM、CRLF、4 空格缩进。现有无关源码修改及此前未提交文档不变。

## 验证记录

- 修改前 `Debug|x64` 构建通过；沙箱内 MSBuild 文件跟踪器报访问拒绝，经授权在沙箱外构建成功。
- 修改后 `Debug|x64` 构建通过，编译仍报告既有数值转换位置的 C4244/C4267 警告，本轮未处理。
- 本地定向验证通过：1024 组坐标、悬停深度、滚轮深度、按键边沿组合；随后其他对象接管输入；重复查询不消费点击；默认裁剪范围和底图像素与原绘制调用一致。
- 验证程序直接编译生产 `SceneCanvas.cpp`，位于被 Git 忽略的 `x64/canvas-check/`，仅用于本轮检查，不作为正式测试工程纳入源码。
- 对 `rigid` 的五个受影响源文件逐个比较，除画布类型、成员和对应局部名称替换外，内容一致；`SimulationApp` 的脚本作用域键列表保持一致。
- 十个修改后的 C++ 文件通过 UTF-8 BOM、CRLF 和空格缩进检查；`git diff --check` 通过。
- 未执行桌面 GUI 交互回归，不能将定向检查视为完整人工验收。
