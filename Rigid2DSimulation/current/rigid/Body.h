#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Aabb.h"
#include "Tile.h"

class Shape;
class Connection;
class SceneStorage;

// 创建输入或刷新快照；派生量仍由 Body 计算。
struct BodyProperties
{
    Vector2 position, velocity;
    double radian = 0, velocityAngular = 0;
    bool fixed = false;
    double density = 1;
    double elasticity = 0.7;
    double frictionStatic = 0.5, frictionDynamic = 0.3;
    double dampCoeff = 0.1, dampCoeffAngular = 0.1;
    double chargeDensity = 0;
    Color innerColor;
};

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

    double GetDepth(bool isPoint) const
    {
        return isPoint ? depthPoint : depthShape;
    }
};

/// <summary>
/// 定义单个物体的物理属性
/// </summary>
class Body
{
public:
    bool point = false;
    bool fixed = false;
    double inertia = 0;
    vector<std::shared_ptr<Shape>> shapes;
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
    BodyAppearance appearance;

    Body(std::shared_ptr<Shape> shape, BodyProperties const& properties = {});
    Body(vector<std::shared_ptr<Shape>> const& shapes, BodyProperties const& properties = {});
    // 使用输入形状的中心作为位置，覆盖 properties.position。
    static std::shared_ptr<Body> CreateAtShapeCenter(vector<std::shared_ptr<Shape>> const& shapes,
                                                   BodyProperties const& properties = {});
    static std::shared_ptr<Body> CreatePoint(Vector2 const& position);
    static std::shared_ptr<Body> CreatePoint(BodyProperties const& properties);

    BodyProperties GetProperties() const;
    void ApplyProperties(BodyProperties const& properties);

    vector<Connection*> const& GetConnections() const
    {
        return connections;
    }
    bool IsMarkedForDeletion() const
    {
        return isMarkedForDeletion;
    }

    Vector2 GenerateRandomPointInside() const;
    void UpdateShapesPositionAndAABB();

    void Step(Vector2 const& gravity, double stepDeltaTime);

private:
    friend class SceneStorage;
    friend class PhysicsWorld;

    vector<Connection*> connections;
    bool isMarkedForDeletion = false;

    // 供内部工厂分阶段组装；外部使用完整构造或 CreatePoint。
    Body();
    void AssignProperties(BodyProperties const& properties);
    // 重算普通物体的派生量和局部几何，返回输入形状中心；不设置位置或同步世界几何。
    Vector2 Recalculate();
    void UpdateAABB();
};

void Electrostatic(Body& b0, Body& b1, double sdt, double coulombConstant);
