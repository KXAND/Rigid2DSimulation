#pragma once
#include <memory>
#include <unordered_map>
#include <vector>

#include "DVector2.h"
#include "DragTool.h"
#include "Vector2.h"

class AppInput;
class Body;
class Connection;
class Creator;
struct InputOwners;
class PhysicsWorld;

int constexpr kDragMode = 0;
int constexpr kSelectMode = 1;
int constexpr kCreateMode = 2;
int constexpr kDeleteMode = 3;

class SceneEditor
{
public:
    int mode = 0;
    std::shared_ptr<Creator> creator;
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
    void ResetTools();
    void CycleMode(InputOwners& inputOwners);
    void RequestBodyDeletion(PhysicsWorld& world);
    void RequestConnectionDeletion(PhysicsWorld& world);
    void PreUpdate(PhysicsWorld const& world, AppInput const& input, InputOwners& inputOwners, DRect viewport);
    void UpdateBodies(PhysicsWorld& world, AppInput& input, InputOwners const& inputOwners,
                      double realDeltaTime, bool isTrackShown, bool isPaused);

private:
    Body const* hoveredBody = nullptr;
    std::unordered_map<Body const*, std::vector<Vector2>> tracks;

    static bool HitTestBody(Body const& body, Vector2 mousePosition);
    void UpdateBody(PhysicsWorld& world, Body& body, AppInput& input, InputOwners const& inputOwners,
                    double realDeltaTime, bool isTrackShown, bool isPaused);
};
