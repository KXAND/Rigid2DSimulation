#pragma once
#include <memory>

#include "DVector2.h"
#include "Vector2.h"

struct AppInput;
struct Body;
struct Connection;
struct Creator;
struct InputOwners;
class PhysicsWorld;

int constexpr kDragMode = 0;
int constexpr kSelectMode = 1;
int constexpr kCreateMode = 2;
int constexpr kDeleteMode = 3;

struct BodyDrag
{
    // 空指针表示没有拖拽物体，其余字段仅在拖拽期间有效。
    Body const* body = nullptr;
    Vector2 relativePoint;
    double radian = 0;
    double velocityAngular = 0;
};

class SceneEditor
{
public:
    int mode = 0;
    std::shared_ptr<Creator> creator;
    Body* bodySelecting = NULL;
    Connection* connectionSelecting = NULL;
    double velocityAngularDrag = 0;

    void ClearSelection();
    // 场景重置时清空选择、悬停和拖拽，不改变工具或模式。
    void ResetInteraction();
    // 删除前仅清理指向待删除对象的引用，不标记或删除对象。
    void PrepareForDeletion();
    Body const* GetHoveredBody() const
    {
        return hoveredBody;
    }
    BodyDrag const& GetDrag() const
    {
        return drag;
    }
    void ResetTools();
    void CycleMode(InputOwners& inputOwners);
    void RequestBodyDeletion(PhysicsWorld& world);
    void RequestConnectionDeletion(PhysicsWorld& world);
    void PreUpdate(PhysicsWorld const& world, AppInput const& input, InputOwners& inputOwners, DRect viewport);
    void UpdateBodies(PhysicsWorld& world, AppInput& input, InputOwners const& inputOwners,
                      double realDeltaTime, bool isTrackShown, bool isPaused);
    void ApplyDrag(Body& body, AppInput const& input, InputOwners const& inputOwners,
                   double realDeltaTime, double stepDeltaTime);

private:
    Body const* hoveredBody = nullptr;
    BodyDrag drag;

    void UpdateBody(Body& body, AppInput& input, InputOwners const& inputOwners,
                    double realDeltaTime, bool isTrackShown, bool isPaused);
    void UpdateDragAngle(Body& body, AppInput const& input, InputOwners const& inputOwners, double realDeltaTime);
    void HandleDragWhole(Body& body, AppInput const& input, double realDeltaTime);
    void HandleDragPoint(Body& body, AppInput const& input, double realDeltaTime);
    void HandleDragForce(Body& body, AppInput const& input, double deltaTime);
};
