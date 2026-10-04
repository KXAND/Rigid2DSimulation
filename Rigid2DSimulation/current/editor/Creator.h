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
struct AppInput;
struct InputOwners;
class PhysicsWorld;
struct Body;
struct Connection;
struct Creator
{
    int mode = 0;
    double bodyDepth = 0;
    double connectionDepth = 0;
    double thickness = 0;
    double radius = 0;
    DVector2 creationStartPoint;
    Body* body0 = NULL;
    Body* body1 = NULL;
    std::shared_ptr<Value> bodyConfiguration, connectionConfiguration;
    Vector2 p0Relative, p1Relative;

    std::shared_ptr<Body> body;
    std::shared_ptr<Connection> connection;
    bool active = false;
    bool hovered = false;

    Creator();
    Creator(Value const& config);
    double Depth() const;
    void Update(PhysicsWorld& world, AppInput& input, InputOwners const& inputOwners);
    void Discard(InputOwners& inputOwners);
    void PreUpdate(AppInput const& input, InputOwners& inputOwners, DRect viewport);

private:
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
