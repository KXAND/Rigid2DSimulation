# 脚本本帧时长同步修正

基线 e4bdd88，分支 fix/app-logic。此前 84f1130 已将脚本 Update 改为每帧一次，本轮仅修正帧时长的发布时点及 DSL 访问键。

## 问题

- App 先执行脚本，再计算 realDeltaTime 并发布，因此作用域保存的是上次发布值，首次执行还未发布。
- current.frameRate.GetDeltaTime() 被作为完整字符串键写入，但 DSL 将点号解析为拼接运算，将括号解析为调用，不会读取这个完整键。

## 执行规则

1. App 在调用脚本前计算本帧 realDeltaTime = min(maxRealDeltaTime, frameRate.GetDeltaTime())，本帧不重复计算。
2. App 调用 SetFrameDeltaTime(realDeltaTime) 保存帧数据，再调用无参 Update()。Update 清理并同步面板参数，然后将保存的时长发布到 deltaTime，再执行一次脚本 Update。
3. 编辑器拖拽和物理推进沿用本帧相同的 realDeltaTime。脚本通过 SetConfig 修改 maxRealDeltaTime 只影响下一帧的截断；其他配置项不改变生效时点。

deltaTime 是以秒为单位、经过上限截断的帧间隔，不是物理子步时长，也不是预知下一帧所需的时间。暂停不将其设为零，暂停时仍按原规则执行脚本。真实零时长仍为零，不人为增加下限。

## 实施边界

- SimulationApp 提前计算时长，先调用 SetFrameDeltaTime，再调用无参 Update，移除原先靠后的时长计算和发布。
- SceneScriptRuntime 用私有成员 frameDeltaTime 保存帧数据。SetFrameDeltaTime 只更新成员，不立即写入 DSL 作用域；Update 同步面板参数后才发布。App 的唯一调度入口负责保证调用顺序，不引入全局 Time 对象。
- frameDeltaTime 与 DSL 的 deltaTime 在 ResetScope 时均初始化为零；每次进入脚本 Update 前由运行时覆盖 DSL 值，优先于同名面板参数。不新增 DSL 只读变量机制，脚本主动赋值仍遵循原语言规则，不会反写保存的帧数据或 App 的 realDeltaTime。
- 旧的完整字符串键不再发布，不保留兼容别名。仓库历史迁移记录不改写；当前源码未发现需同步修改的现成脚本调用。
- 保留原脚本作用域、面板参数其他规则、物理算法、模拟 time 的发布时点、调试文字及 UI 调度。不恢复 stash 中的严格参数契约与错误处理修改。
- 后续恢复 DSL 契约 stash 时，需保留本轮无参 Update、私有帧数据及 deltaTime 发布位置，再合入异常捕获逻辑。

## 验证计划

- 首帧、连续不同帧时长、零时长、重置初始化、无 Update 时的时长发布。
- 真实 DSL 读取 deltaTime；面板同名参数不能覆盖本帧输入，脚本赋值不改变 App 时长，下一次发布恢复真实值。
- 脚本修改时长上限：本帧使用原先确定的值，下一帧使用新上限；本帧脚本仍只执行一次。
- Debug x64 / Win32 构建及既有关键回归。测试放在忽略的 x64/，不提交。

## 使用示例

```text
elapsed = 0;
def Update()
{
    elapsed = elapsed + deltaTime;
}
```

普通自定义函数内如需显式读取全局时长，按现有 DSL 作用域规则使用 &deltaTime；自动 Update 仍直接在场景作用域执行。

## 验证结果

- Debug x64 / Win32 构建通过，使用独立 TargetName，不修改 .vcxproj.user。
- 87 项脚本检查通过：既有 58 项字符串、单次 Update、配置校验；新增 24 项时长与作用域检查；5 项 Parallelogram 参数回归。包含 setter 不立即发布、无参 Update 发布保存值及重置清除内部时长。
- 时长检查包含实际 DSL 读取、同名面板参数覆盖顺序、脚本写入隔离、下一帧上限生效，以及实际 PhysicsWorld::Advance 的推进时间与子步数量。
- 拖拽回归通过：216 次非法时长计算、6 次释放检查；256 组场景、1280 次帧比较、3840 次子步比较保持一致。
- 本轮三个 C++ 文件保持 UTF-8 BOM、CRLF、空格缩进，git diff 检查通过。
- 测试位于忽略的 x64/frame-time-check/ 与既有拖拽测试目录，不提交。未进行人工 GUI 验收。
- 用户已确认本轮修改，代码与本文独立提交；Creator 配置交接另行提交，DSL 契约 stash 保持不变。
