#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Aabb.h"
#include "Tile.h"

struct ScriptStatement;
typedef vector<ScriptStatement> Program;

struct Shape;
struct Connection;
struct BodyAppearance
{
    double depthShape = 0, depthPoint = 1;
    double pointRadius = 8;

    Color innerColor;
    Color borderColor = {255, 255, 255};
    Color hoveredColor = {0, 0, 255};
    Color draggedColor = {255, 0, 0};
    Color selectedColor = {255, 0, 255};
    Color selectedConnectionColor = {255, 255, 0};
};

struct BodyScriptData
{
    std::wstring scriptText, temporaryScriptText;
    std::shared_ptr<Program> positionProgram, angleProgram, angularVelocityProgram;
    bool hasPositionPreset = false;
    bool hasAnglePreset = false;
    bool hasAngularVelocityPreset = false;
};

/// <summary>
/// 定义单个物体的物理属性
/// </summary>
struct Body
{
    bool isMarkedForDeletion = false;
    bool point = false;
    bool fixed = false;
    double inertia = 0;
    vector<Vector2> track;
    vector<std::shared_ptr<Shape>> shapes;
    vector<Connection*> connections;
    Vector2 position, velocity;
    Aabb box;
    Matrix2 transform = Matrix2::Identity();
    double radian = 0, velocityAngular = 0;
    double invMass = 0;    // 质量倒数，为0表示此物体不参与运动计算
    double invInertia = 0; // 转动惯量倒数
    double area = 0, density = 0;
    double elasticity = 0; // 弹性系数
    double frictionStatic = 0, frictionDynamic = 0;
    double dampCoeff = 0, dampCoeffAngular = 0;
    double chargeDensity = 0, charge = 0;

    bool visited = false;
    bool hovered = false;
    bool dragged = false;
    double radianDrag = 0, velocityAngularDrag = 0;
    Vector2 relativeDragPoint;
    BodyAppearance appearance;
    BodyScriptData scriptData;

    Body();
    Body(Vector2 const& position);
    Body(std::shared_ptr<Shape> shape);
    Body(vector<std::shared_ptr<Shape>> const& shapes);

    Vector2 GenerateRandomPointInside() const;
    bool IsInside(Vector2 p) const;
    void UpdateShapesPositionAndAABB();
    void Warp(DRect rectangle);
    double GetDepth() const;

private:
    void UpdateAABB();

public:
    void Init(bool resetPosition = false);
    void Step(Vector2 const& gravity, double stepDeltaTime, std::function<void(bool)> const& applyPresets);
};

void Electrostatic(Body& b0, Body& b1, double sdt, double coulombConstant);
