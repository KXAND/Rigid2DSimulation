#pragma once
#include <memory>
#include <unordered_map>
#include <vector>

#include "CreationPreview.h"
#include "Creator.h"
#include "DVector2.h"
#include "DragTool.h"
#include "Vector2.h"

class AppInput;
class Body;
class Connection;
struct InputOwners;
class PhysicsWorld;
class SceneCanvas;

int constexpr kDragMode = 0;
int constexpr kSelectMode = 1;
int constexpr kCreateMode = 2;
int constexpr kDeleteMode = 3;

class SceneEditor
{
public:
    int mode = 0;
    Body* bodySelecting = NULL;
    Connection* connectionSelecting = NULL;
    DragTool dragTool;

    void ClearSelection();
    // 场景重置时清空选择、悬停、拖拽和轨迹，不改变工具或模式。
    void ResetInteraction();
    // 删除前清理待删除对象的交互引用和轨迹，不标记或删除对象。
    void PrepareForDeletion(InputOwners& inputOwners);
    void ClearTrack(Body const& body);
    std::unordered_map<Body const*, std::vector<Vector2>> const& GetTracks() const
    {
        return tracks;
    }
    Body const* GetHoveredBody() const
    {
        return hoveredBody;
    }
    void ResetTools(InputOwners& inputOwners);
    // 请求不会替换工具；下一次 PreUpdate 在拾取前取消旧操作并应用配置。
    void RequestCreatorConfig(Value const& config);
    void RequestCreatorConfig(CreatorConfig const& config);
    // UI 修改配置时读取最新请求，避免同帧的多项修改互相覆盖。
    CreatorConfig const& GetRequestedCreatorConfig() const;
    void SelectCreationTool(int toolMode);
    Creator const& GetCreator() const { return creator; }
    void CycleMode(InputOwners& inputOwners);
    void RequestBodyDeletion(PhysicsWorld& world);
    void RequestConnectionDeletion(PhysicsWorld& world);
    void PreUpdate(PhysicsWorld const& world, AppInput const& input, InputOwners& inputOwners, DRect viewport);
    // 每帧只处理一次编辑输入，并保存本帧拖拽快照。
    void Update(PhysicsWorld& world, AppInput& input, InputOwners& inputOwners,
                SceneCanvas const& canvas, double realDeltaTime);
    // 释放帧仍保留一次每帧作用；GetDrag 则仅表示后续子步中仍持续的拖拽。
    BodyDrag const& GetFrameDrag() const { return frameDrag; }
    BodyDrag const& GetDrag() const
    {
        return dragTool.GetDrag();
    }
    CreationPreview GetPreview() const;
    // 每帧拖拽作用后、物理推进前采样，不处理输入。
    void RecordTracks(PhysicsWorld const& world, bool isTrackShown, bool isPaused);

private:
    Creator creator;
    std::unique_ptr<CreatorConfig> pendingCreatorConfig;
    Body const* hoveredBody = nullptr;
    BodyDrag frameDrag;
    std::unordered_map<Body const*, std::vector<Vector2>> tracks;

    static bool HitTestBody(Body const& body, Vector2 mousePosition);
    BodyDrag UpdateBody(PhysicsWorld& world, Body& body, AppInput& input, InputOwners const& inputOwners,
                        double realDeltaTime);
};
