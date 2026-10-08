#pragma once
#include "Vector2.h"

int constexpr kCreateBox = 0;
int constexpr kCreateBall = 1;
int constexpr kCreatePlate = 2;
int constexpr kCreateParticle = 3;
int constexpr kCreateConnection = 4;
int constexpr kCreatePoint = 5;
int constexpr kCreateNail = 6;

struct Value;
class AppInput;
struct InputOwners;
class PhysicsWorld;
class Body;
class Connection;
class Creator
{
public:
    int mode = 0;
    double bodyDepth = 0;
    double connectionDepth = 0;
    double thickness = 0;
    double radius = 0;
    std::shared_ptr<Value> bodyConfiguration, connectionConfiguration;

    Creator();
    Creator(Value const& config);
    double Depth() const;
    // 左键命中物体的通知；在物理推进前记录锚点，由 Update 在原阶段确认创建。
    void OnBodyPressed(Body& hitBody, Vector2 const& mousePosition);
    void OnBodyReleased(Body& hitBody, Vector2 const& mousePosition);
    void Update(PhysicsWorld& world, AppInput& input, InputOwners const& inputOwners);
    void Discard(InputOwners& inputOwners);
    // 删除前取消引用待删除端点的创建过程及预览。
    void PrepareForDeletion(InputOwners& inputOwners);
    void PreUpdate(AppInput const& input, InputOwners& inputOwners, DRect viewport);

    // 只借用当前预览；Update 或 Discard 后须重新获取，无预览时返回 nullptr。
    Body const* GetPreviewBody() const
    {
        return body.get();
    }
    Connection const* GetPreviewConnection() const
    {
        return body0 ? connection.get() : nullptr;
    }

private:
    DVector2 creationStartPoint;
    Body* body0 = NULL;
    Body* body1 = NULL;
    Vector2 p0Relative, p1Relative;
    std::shared_ptr<Body> body;
    std::shared_ptr<Connection> connection;
    bool active = false;
    bool hovered = false;

    void UpdateBox(PhysicsWorld& world, AppInput& input);
    void UpdateBall(PhysicsWorld& world, AppInput& input);
    void UpdatePlate(PhysicsWorld& world, AppInput& input);
    void UpdateConnection(PhysicsWorld& world, AppInput& input);
    /// <summary>
    /// 在一个空地方点击，开始创建一个新物体
    /// </summary>
    /// <param name="input"></param>
    void UpdatePoint(PhysicsWorld& world, AppInput& input);
    /// <summary>
    /// 将body0固定钉在原地
    /// </summary>
    /// <param name="input"></param>
    void UpdateNail(PhysicsWorld& world, AppInput& input);
    /// <summary>
    /// 在鼠标位置创建一个半径为 radius 的圆/粒子
    /// </summary>
    /// <param name="input"></param>
    void UpdateParticle(PhysicsWorld& world, AppInput& input);
};
