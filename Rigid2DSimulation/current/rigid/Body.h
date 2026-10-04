#pragma once
#include "Tile.h"
#include "Aabb.h"

struct ScriptStatement;
typedef vector<ScriptStatement> Program;

struct Value;
struct SimulationApp;
struct Shape;
struct Connection;
/// <summary>
/// 定义单个物体的物理属性
/// </summary>
struct Body
{
    bool isMarkedForDeletion = false;
    bool point = false;
    bool fixed = false;
    std::shared_ptr<Program> positionProgram, angleProgram, angularVelocityProgram;
    double inertia = 0;
    bool hasPositionPreset = false;
    bool hasAnglePreset = false;
    bool hasAngularVelocityPreset = false;
    vector<Vector2> track;
    vector<std::shared_ptr<Shape>> shapes;
    vector<Connection*> connections;
    Vector2 position, velocity;
    Aabb box;
    Matrix2 transform = Matrix2::Identity();
    wstring scriptText, temporaryScriptText;
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
    double depthShape = 0, depthPoint = 0;
    double pointRadius = 0;

    Color innerColor;
    Color borderColor;
    Color hoveredColor;
    Color draggedColor;
    Color selectedColor;
    Color selectedConnectionColor;

    Body();
    Body(Vector2 const& position);
    Body(std::shared_ptr<Shape> shape);
    Body(vector<std::shared_ptr<Shape>> const& shapes);
    Body(SimulationApp& simulationApp, FILE* file);
    void Save(FILE* file) const;

    Vector2 GenerateRandomPointInside() const;
    bool IsInside(Vector2 p) const;
    void RefreshFromScript(SimulationApp& simulationApp);
    void ReadConfig(Value const& config);
    void UpdateShapesPositionAndAABB();
    void Warp(DRect rectangle);
    double GetDepth() const;

private:
    void UpdateAABB();

public:
    void Init(bool resetPosition = false);
    void Render(SimulationApp& simulationApp) const;
    void Step(SimulationApp& simulationApp, double stepDeltaTime);

private:
    void FollowPresetPosition(SimulationApp& simulationApp, double stepDeltaTime);
    void FollowPresetAngle(SimulationApp& simulationApp, double stepDeltaTime);
    void FollowPresetAngularVelocity(SimulationApp& simulationApp);
};

void Electrostatic(Body& b0, Body& b1, double sdt, double coulombConstant);
