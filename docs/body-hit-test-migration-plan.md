# Body 拾取职责迁移方案

## 1. 范围与状态

- 基线：cb80d0b；对应 Body 整理清单第 5 项。实现与验证已获用户确认，代码与本文一起提交。
- 目标是区分形状几何查询与编辑器鼠标命中规则，不修改几何算法、物理公式、拾取结果或绘制输出。
- 不迁出 appearance 数据，不引入 SceneBody、拾取管理器、新生产目录或暂时没有调用者的 Body::ContainsPoint。

## 2. 接口与调用点

源码路径相对 Rigid2DSimulation/current/。

| 位置 | 修改 |
| --- | --- |
| rigid/Body.h | BodyAppearance 新增 GetDepth(bool isPoint) const，返回现有 depthPoint 或 depthShape；移除 Body::GetDepth 和 Body::IsInside 声明 |
| rigid/Body.cpp | 删除上述两个 Body 成员实现，其余代码不变 |
| editor/SceneEditor.h/.cpp | 新增私有 static bool HitTestBody(Body const& body, Vector2 mousePosition)，迁入点物体半径判断和普通物体形状遍历；实现显式包含 Shape.h |
| editor/SceneEditor.cpp | PreUpdate 使用 appearance.GetDepth(body.point) 和 HitTestBody，仍负责视口限制、深度比较及 inputOwners 更新 |
| render/SceneRenderer.cpp | RenderBody 从 appearance 获取显示深度，供原有绘制调用使用，不改变绘制顺序和参数 |
| rigid/Shape.h/.cpp | 不修改；IsInside 仍负责形状几何包含测试 |

HitTestBody 不读输入全局状态，不更新几何，也不决定多个物体之间谁获得鼠标。GetDepth 只按 isPoint 选择已有外观字段；编辑器与渲染共享此规则，不各自实现条件判断。

## 3. 必须保持的行为

1. 点物体命中仍为距离平方严格小于 pointRadius 的平方；圆形显示区域不变成物理几何。保留零或负半径的原判断，不修正非法参数。
2. 普通物体仍按 shapes 的原顺序查询，只要一个 Shape::IsInside 为真就命中；不新增 AABB 提前排除或其他快速路径。
3. 悬停深度仍使用 <= 比较，同深度时后遍历的命中物体覆盖前者；不排序、不改为 <、不额外过滤待删除物体。
4. 保留“创建工具 PreUpdate → 按世界容器顺序遍历物体”的顺序。物体仍按深度、命中、视口的顺序短路判断，不改输入所有权机制。
5. 拾取读取原时点已有的 shape 世界坐标，不因迁移而提前更新几何。Renderer 原有绘制前同步几何行为保留，不在本轮修复历史时序问题。
6. 渲染中点、圆、多边形、边框和拖拽线使用原深度；轨迹和连接的固定深度不改。原有 pointRadius 同时用于点物体显示和命中。

## 4. 验证与实施记录

实现与验证已获用户确认，代码与本文一起提交。只修改第 2 节列出的 5 个源码文件，无工程文件或其他生产模块修改。

独立期望端保留旧 Body::IsInside、GetDepth 和 PreUpdate 规则，不调用新的 HitTestBody 或 BodyAppearance::GetDepth。Shape 算法和底层绘图函数未修改，可由两端共享。测试仅放在忽略的 x64/，本文随对应代码提交，ref 进度继续忽略。

| 检查 | 结果 |
| --- | --- |
| 主工程 | Debug/x64 构建通过，临时输出 Rigid2DSimulationInitCheck，不覆盖可能正在运行的程序 |
| 拾取对照 | 302,424 次新旧 PreUpdate 对照通过；覆盖点、圆、多边形、复合形状、空形状集合、旋转、视口内外及边界、重叠、初始 UI 占用、创建工具竞争和待删除物体 |
| 边界与深度 | 明确检查点物体半径边界排除、同深度后者胜出及较高深度优先；对照零/负半径和 NaN/无穷深度的原有规则，不新增修复 |
| 几何时序 | 40 组拾取场景前后几何快照一致；包含直接改变 Body 位置而未同步 Shape 的样本，确认拾取没有新增几何刷新 |
| 像素对照 | 1,024 个场景各比较世界绘制、创建预览两个阶段，共 2,048 次颜色/透明度/深度缓冲完全一致；覆盖四种显示模式、四类物体、裁剪、遮挡、选择/悬停/拖拽、连接和轨迹 |
| 关联维护回归 | 重新编译并运行上一轮 AssociationChecks，通过 64 组关联图、两种删除入口和暂停组合、分组拆分、七种创建模式、32 次连接/钉子创建、16 组组合场景及 16 组存档往返 |
| 静态及格式 | Shape 算法未修改，生产代码无 Body::IsInside/GetDepth 残留；5 个 C++ 文件保持 UTF-8 BOM、CRLF、空格缩进；git diff --check 通过 |

新测试位于 x64/picking-check/。对照 Renderer 从 cb80d0b 提取，仅更换测试类型名，并将旧 Body::GetDepth 调用替换成保留原条件表达式的测试函数；两端分别构建物体和几何，不共享可变形状。对照端的拾取规则同样来自该基线，不以新接口输出作为期望值。

MSBuild 沿用此前 FileTracker 所需的沙箱外构建方式，没有修改构建配置。未做人工 GUI 验收，也未重跑所有更早的历史套件；以上有限样本和静态差异检查不等于证明全部输入等价。
