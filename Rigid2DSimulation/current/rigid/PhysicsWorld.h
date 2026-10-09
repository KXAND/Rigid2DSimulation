#pragma once
#include <functional>
#include <memory>
#include <vector>

#include "DVector2.h"
#include "Vector2.h"

class Body;
class Collision;
class Connection;
class Group;

class PhysicsWorld
{
public:
    // 只读约束集合的增删，不限制物体自身的属性更新。
    std::vector<std::shared_ptr<Body>> const& GetBodies() const
    {
        return bodies;
    }
    std::vector<std::shared_ptr<Connection>> const& GetConnections() const
    {
        return connections;
    }

    void AddBody(std::shared_ptr<Body> const& body);
    void AddConnection(std::shared_ptr<Connection> const& connection);
    void RequestBodyDeletion(Body& body);
    void RequestConnectionDeletion(Connection& connection);

    bool isSceneChanged = false;
    Vector2 gravity;
    double coulombConstant = 0;
    bool hasElectrostatic = false;
    double simulationTime = 0;
    int stepNum = 0;
    DRect rectScene;
    double parallelEpsilon = 0;
    bool useEqualPositionCorrection = false;
    double gridSize = 0;

    void ClearObjects();
    void ResetParameters(DRect const& sceneRect);
    void RemoveDeletedObjects();
    void Prepare();
    // 同步当前姿态对应的几何和连接端点，不推进时间；暂停时也须在绘制前调用。
    void UpdateGeometry();

    // 同步过渡接口：保留每个子步内脚本预设与拖拽的原有执行位置，不保存回调。
    void Advance(double deltaTime, Body const* warpExcludedBody,
                 std::function<void(double)> const& beginStep,
                 std::function<void(Body&, double)> const& stepBody,
                 std::function<void(Body const&)> const& onBodyWarped);

private:
    std::vector<std::shared_ptr<Body>> bodies;
    std::vector<std::shared_ptr<Connection>> connections;
    std::vector<std::shared_ptr<Group>> groups;
    std::vector<std::shared_ptr<Collision>> collisions;
    int gridNumX = 0;
    int gridNumY = 0;
    std::vector<Body*> multiGridBodies;
    std::vector<std::vector<Body*>> grid;

    void RegisterBody(Body& body);
    void DetectCollisions();
};
