# Creator 稳定实例与配置交接

## 范围

与脚本帧时长同步修复分开提交。只处理 Creator 生命周期与配置交接，不限制运动预设的全局函数访问，不恢复 DSL 契约 stash。

## 决策

- SceneEditor 私有持有一个 Creator 值成员，随 Editor 创建和销毁，不使用全局 Singleton。外部通过只读访问获取工具配置，不能替换工具。
- DSL SetCreator 名称及配置语法保持不变，含义改为请求配置当前工具，不再创建新工具实例。
- CreatorConfig 为配置数据；请求时解析 mode、radius、thickness 等，缺省值沿用此前新建 Creator 的默认值。bodyConfiguration / connectionConfiguration 保留原有 Value 拷贝方式，不额外扩展 DSL 深拷贝语义。
- 同一输入边界前多次请求以最后一次为准。请求后，当前帧不再处理 Creator 的后续输入，预览不再展示；旧工具实例仍有效，不会留下销毁对象的地址。
- 下一次 SceneEditor::PreUpdate 开始时，先 Discard 清理旧输入归属、创建中的端点和预览，再应用待处理配置，然后按新模式及深度参与本帧拾取。该处理不依赖是否处于创建模式。
- ResetTools 显式接收 InputOwners，取消工具操作、恢复默认配置、清除待处理请求；不重建 Creator。保留原模式和拖拽工具的既有重置规则。
- 持续每帧调用 SetCreator 仍代表持续重新配置并取消操作，不新增配置去重或自动保留进行中操作的规则。

## 实现与验证计划

- Creator.h / Creator.cpp：配置数据、解析及应用；创建算法不变。
- SceneEditor.h / SceneEditor.cpp：稳定所有权、请求入口和下一次拾取前的应用边界。
- SceneScriptRuntime.cpp：SetCreator 转交 Editor；GlobalControls.cpp 只读获取工具模式；SimulationApp.cpp 传入重置所需的 InputOwners。
- 验证地址在配置与重置后不变、七种模式、缺省字段、最后请求生效、旧配置不再接收本帧输入、跨模式拾取深度、创建中取消、重置清除待处理请求、输入归属释放。
- 本地测试与构建结果见下文；不提交忽略目录中的测试。

## 提交边界

1. 帧时长同步修复：script-frame-delta-time-fix.md，SimulationApp 的帧时长调度，以及 SceneScriptRuntime 的时长保存与发布。
2. Creator 配置交接：本文，Creator / SceneEditor 的改动，SceneScriptRuntime 的 SetCreator 调用点，SimulationApp 的 ResetTools 调用点，以及 GlobalControls 的只读配置访问。

两个提交共享文件时按上述代码块分别提交，不把整份 SimulationApp.cpp 或 SceneScriptRuntime.cpp 混入其中一个提交。帧时长修复已独立提交为 f70badb；Creator 修改经用户确认，与本文独立提交。

## 验证结果

- Debug x64 / Win32 构建通过，使用独立 TargetName，不修改 .vcxproj.user。
- 26 项 Creator 交接测试通过：身份稳定、配置延迟与最后请求生效、基本字段请求时取值、缺省值、旧工具点击阻断、方框和连接预览取消、释放不提交旧操作、端点清理、新操作正常创建、跨模式深度拾取、重置及无关输入归属保留。
- 87 项既有脚本/帧时长回归通过，其中七种 Creator 配置模式在下一次 PreUpdate 后验证。
- 既有拖拽回归通过：216 次非法时长计算、6 次释放；256 组场景、1280 次帧比较、3840 次子步比较一致。
- C++ 文件保持 UTF-8 BOM、CRLF、空格缩进；git diff 检查通过。
- 测试位于忽略的 x64/creator-config-check/ 及既有测试目录，不提交。未进行人工 GUI 验收。
- 严格 DSL 契约的 stash 保持不变。后续恢复时，SetCreator 仍应转交 RequestCreatorConfig，而非恢复指针替换。
