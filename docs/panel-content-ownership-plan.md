# 面板内容持有与初始化依赖整理

## 范围

基于 a8be7a7，在 refactor/ui-panels（原 refactor/ui-callbacks）上实施用户确认的两点：容器命名明确表达控件类型；通用面板接管根内容，并用显示区域边界替代初始化对 App 的依赖。

保持保留模式 UI，不引入声明式框架或即时模式接口。不调整 UpdateContent 钩子的名称、业务回调、参数提交时机或物理逻辑。实现与验证完成后，用户已确认提交本轮代码与文档，并合并到 master、推送 origin。

## 命名与所有权

| 原名 | 新名 |
| --- | --- |
| ControlList | ControlContainer |
| HorizontalControlList | HorizontalStack |
| VerticalControlList | VerticalStack |
| framework/control/ControlList.h、.cpp | framework/control/ControlContainer.h、.cpp |

容器本身仍是 Control 子类，具有尺寸、布局、输入和更新行为，不是普通的指针列表。同步项目文件及全部源码引用，不保留旧名兼容层。历史方案保留当时名称。

Panel、FloatPanel 私有持有 unique_ptr<Control> 根内容。业务面板用局部 unique_ptr 组装任意方向的布局，再通过 SetContent(std::move(layout)) 移交所有权；不再持有根布局成员或设置裸 contentControl。

后续业务需要访问的输入框、编辑区、动态列表仍使用借用指针，不引入 shared_ptr 共享所有权。构造辅助函数需要根布局时明确接收其引用。滚动条只借用根内容；不保留没有生产调用方的 GetContent 接口，本地检查通过已有滚动条关联观察内容。

滚动条的 shared_ptr 暂不调整，在 Panel、FloatPanel 成员旁保留 TODO：当前仅由面板持有，后续评估改为值成员或 unique_ptr。删除疑问注释不等于解决了所有权选择问题。

SetContent 是受保护的首次安装入口，拒绝空内容和重复安装。运行中替换内容需要处理旧焦点和输入归属，不在本轮开放；移除无调用方的旧面板 SetControl，不影响 Delegate::SetControl。

## 初始化分工

旧 Init(App&) 只设置根内容的位置、裁剪范围，以及创建并配置滚动条；其中 App 只用于取得 screen.Rect()。不能直接提前到根内容创建之前，否则跳过内容设置且滚动条关联为空；可以放在根内容登记之后、子项创建之前。

新流程：

1. FloatPanel 构造接收 DRect 显示边界、宽和高，完成窗口几何与滚动条初始化；LocalizedFloatPanel 另外接收标题键。
2. 普通 Panel 的菜单和底栏工厂接收显示边界，保留原宽高及位置规则，并准备滚动条。
3. 业务构造组装根内容，不假定横向或纵向。
4. SetContent 接管内容，设置根位置和裁剪区域，并让滚动条关联根内容。内部不使用 App，也不调用派生类虚函数。

删除业务末尾的 contentControl = layout.get() 和 Init(app)。初始化工作没有消失，只是归入拥有相应对象的通用面板。

## 边界更新与行为约束

显示边界不是永久缓存。UserInterface::PreUpdate 在命中测试之前统一调用 SetDisplayBounds，刷新普通面板和浮动面板的根裁剪范围。容器 PreUpdate 将新的裁剪范围继续传给子控件，避免绘制已经裁剪但输入仍使用旧范围。

边界刷新不重建内容、不重置滚动位置或输入焦点，也不重新居中浮动面板。本轮不添加窗口自适应布局系统。浮动面板移动使用已知边界计算裁剪，不再为 SetTopLeft 传入整个 App。

控件创建顺序、参数行缓存、选择切换、滚动测量顺序、输入回调和绘制顺序保持原样。UTF-8 BOM、CRLF、空格缩进保持当前源码基线。

## 验证

复用 87 个画面哈希、面板/按钮/创建工具/脚本回归。新增初始化后立即可用、任意根内容类型、唯一所有权与拒绝替换、边界变更后的裁剪与输入检查。检查文件放在忽略的 x64/，不纳入 Git。

验证结果：

- x64、Win32 Debug 构建通过。
- 87 个面板画面哈希与 a8be7a7 基线一致。
- 面板与绑定检查 55 项、容器与内容安装检查 80 项、按钮回调检查 9,599 项、创建工具真实输入检查 58 项、脚本与配置回归 1,299 项全部通过。
- 本轮新增 29 项检查，覆盖不依赖 App 的首次安装、滚动关联、空内容与重复安装拒绝、旧焦点保留、移动与边界更新、嵌套控件裁剪及命中、根内容唯一销毁、横向容器和单个控件作为根内容。
- 所有旧容器类型、头文件和项目文件引用均已迁移；业务面板没有遗留的 Init(app) 或 contentControl = layout.get()。

状态：实现与验证完成，用户已确认。按确认范围提交代码与本文，再合并到 master 并推送 origin/master。
