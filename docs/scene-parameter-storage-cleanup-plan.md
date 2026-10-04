# SceneParameter 存储迁移与无效 App 依赖清理

## 范围与映射

- 基线：`969f194`。用户仅批准以下两项，现已检查确认，代码与本文一起提交，不推进其他整理。
- `Rigid2DSimulation/current/common/SceneParameter.cpp/.h`：删除 Save(FILE*) 和文件构造；保留名称构造、name、value、isMarkedForDeletion，数据所有权不变。
- `Rigid2DSimulation/current/storage/SceneStorage.cpp`：将上述读写直接放入现有 SaveParameters/LoadParameters 循环，不增加存储框架、模板或转发接口。
- `Rigid2DSimulation/current/rigid/Group.cpp`：移除无用的 SimulationApp.h 包含；Group.h、Shape.h 各移除无用的 SimulationApp 前置声明。算法、其余依赖和排版不动。

## 不变项

- 文件顺序仍为参数数量，以及每个参数的名称长度、名称内容、double value。数量与名称长度均保持 int，不改变 wchar_t 和 double 的存储表示。
- 参数循环数量 size 与单条记录的 nameLen 分开，不能让名称读取覆盖循环上界。
- 加载时先默认构造 SceneParameter，再读取名称和值，完成后加入 runtime.parameters；isMarkedForDeletion 仍为 false，不写入文件。
- 保存仍包含尚未清理的待删除参数，不去重、不排序、不改变同名参数及原列表顺序。
- 不改变场景重置、脚本执行和参数同步时机，不修复空字符串或损坏文件处理；不调整 Connection 外观、UI 或物理逻辑。
- C++ 保持 UTF-8 BOM、CRLF、4 空格。本批源码和本文经用户检查确认后一起提交；测试仅放入被忽略的 x64。

## 验证

- Debug|x64 构建及既有物理、脚本/存储回归。
- 保留旧参数记录实现为独立本地基线，对照完整保存字节、重载参数和再次保存字节，覆盖零/多个参数、中文及不同长度名称、重复名称、不同数值及删除标记默认值。
- 检查旧文件接口和 rigid 中的 SimulationApp 引用没有残留；核对 Group/Shape 除指定包含或声明外未变。
- 检查编码、换行和 git diff --check；明确未做 GUI 验收及无效输入边界测试。

## 进度

两项已完成，验证通过，用户已检查确认；代码与本文一起提交。

- SceneParameter 的记录读写已移入现有循环，旧 Save/文件构造及调用点无残留，未新增接口或修改工程登记。
- Group.cpp、Group.h、Shape.h 与基线逐文件比较，仅删除指定的一个包含和两个前置声明；rigid 代码中不再有 SimulationApp 引用，算法实现未变。
- Debug|x64 主工程构建通过。
- `x64/parameter-storage-check/BaselineParameter.h` 保留旧成员读写实现，不调用新 SceneStorage 生成期望值。48 组参数场景的完整保存字节、加载状态及再次保存字节一致，覆盖 0/1/2/3/7/32 个参数、不同长度中文/同名名称、正负零及不同量级数值、待删除参数仍保存且加载后标记恢复 false。
- 原脚本回归通过：注册/构建/配置、10 帧双次 Update、128 组预设/512 子步、6 组存储及打开失败状态保留。此前记录迁移的 256 组连接配置、64 组场景二进制对照及加载时序检查通过。
- 原物理回归通过：10 场景、800 子步、2 项删除与重置检查。原 Body 回归通过：5 类构造、1280 组配置、拾取、程序共享及 48 组存储对照。
- 六个修改的 C++ 文件通过严格 UTF-8 BOM、CRLF、无 Tab/替换字符检查；git diff --check 通过。
- 没有实际窗口验收；空名称和损坏文件不在测试范围，相关历史问题未改。测试继续仅保存在忽略的 x64；既存两份未跟踪文档未动。
