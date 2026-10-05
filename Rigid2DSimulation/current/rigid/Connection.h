#pragma once
#include "Tile.h"

int constexpr kRopeConnection = 0;
int constexpr kLinkConnection = 1;
int constexpr kSpringConnection = 3;
int constexpr kCordConnection = 4;

struct Body;

struct ConnectionAppearance
{
    Color colorRopeTight = Color::kRed;
    Color colorRope = Color::kWhite;
    Color colorLink = Color::kBlue;
    Color colorCordTight = Color::kMagenta;
    Color colorCord = Color::kCyan;
    Color colorSpringStretch = Color::kYellow;
    Color colorSpringCompress = Color::kGreen;
};

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

    ConnectionAppearance appearance;

    Connection() = default;

    void SignUpToBodies();
    void UpdatePosition();
    void SimulateRopeOrLink(bool useEqualPositionCorrection);
    void SimulateCordOrSpring(double stepDeltaTime);
    void Simulate(double stepDeltaTime, bool useEqualPositionCorrection);
};

Body* GetOtherBody(Body* body, Connection const& connection);
