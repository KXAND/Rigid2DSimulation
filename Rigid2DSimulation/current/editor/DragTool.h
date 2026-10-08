#pragma once
#include "Vector2.h"

class AppInput;
class Body;
struct InputOwners;

struct BodyDrag
{
    // 空指针表示没有拖拽物体，其余字段仅在拖拽期间有效。
    Body const* body = nullptr;
    Vector2 relativePoint;
    double radian = 0;
    double velocityAngular = 0;
};

class DragTool
{
public:
    double rotationSpeed = 0;

    BodyDrag const& GetDrag() const
    {
        return drag;
    }
    // 每帧接收命中结果和模式许可；已有拖拽在应用后才判断是否结束。
    void UpdateBody(Body& body, AppInput& input, InputOwners const& inputOwners,
                    double realDeltaTime, bool isHovered, bool enabled);
    // 在 Body::Step 后应用拖拽，不改变开始或结束状态。
    void ApplySubstep(Body& body, AppInput const& input, InputOwners const& inputOwners,
                      double realDeltaTime, double stepDeltaTime);
    void Cancel();
    // 删除前取消对待删除物体的引用，不负责删除物体。
    void PrepareForDeletion();

private:
    BodyDrag drag;

    void UpdateDragAngle(Body& body, AppInput const& input, InputOwners const& inputOwners, double realDeltaTime);
    void HandleDragWhole(Body& body, AppInput const& input, double realDeltaTime);
    void HandleDragPoint(Body& body, AppInput const& input, double realDeltaTime);
    void HandleDragForce(Body& body, AppInput const& input, double deltaTime);
};
