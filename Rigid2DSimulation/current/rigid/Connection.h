#pragma once
#include "Tile.h"

int constexpr kRopeConnection = 0;
int constexpr kLinkConnection = 1;
int constexpr kSpringConnection = 3;
int constexpr kCordConnection = 4;

struct Value;
class PhysicsWorld;
struct Body;
/// <summary>
/// 定义一个两两连接关系，根据连接物理材质的不同，更新位置
/// </summary>
struct Connection
{
    bool isMarkedForDeletion = false;
    int type = 0;
    wstring scriptText, temporaryScriptText;
    Body* body0 = NULL;
    Body* body1 = NULL;
    Vector2 p0Relative, p1Relative;
    Vector2 p0, p1;
    double length = 0;
    double springConstant = 0;
    bool isTight = false;

    // @todo:应该根据变connection的类型决定这几个量的出现的
    Color colorRopeTight, colorRope;
    Color colorLink;
    Color colorCordTight, colorCord;
    Color colorSpringStretch, colorSpringCompress;

    Connection();
    Connection(PhysicsWorld& physicsWorld, FILE* file);
    void Save(PhysicsWorld const& physicsWorld, FILE* file) const;

    void ReadConfig(Value const& configuration);
    void SignUpToBodies();
    void UpdatePosition();
    void SimulateRopeOrLink(bool useEqualPositionCorrection);
    void SimulateCordOrSpring(double stepDeltaTime);
    void Simulate(double stepDeltaTime, bool useEqualPositionCorrection);
};

Body* GetOtherBody(Body* body, Connection const& connection);
