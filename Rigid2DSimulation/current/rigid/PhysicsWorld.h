#pragma once
#include <functional>
#include <memory>
#include <vector>

#include "DVector2.h"
#include "Vector2.h"

struct Body;
struct Collision;
struct Connection;
struct Group;

class PhysicsWorld
{
public:
    std::vector<std::shared_ptr<Body>> bodies;
    std::vector<std::shared_ptr<Connection>> connections;

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
    void MarkDeletedConnections();
    void RemoveDeletedObjects();
    void Prepare();

    // 同步过渡接口：保留每个子步内脚本预设与拖拽的原有执行位置，不保存回调。
    void Advance(double deltaTime,
                 std::function<void(double)> const& beginStep,
                 std::function<void(Body&, double)> const& stepBody);

private:
    std::vector<std::shared_ptr<Group>> groups;
    std::vector<std::shared_ptr<Collision>> collisions;
    int gridNumX = 0;
    int gridNumY = 0;
    std::vector<Body*> multiGridBodies;
    std::vector<std::vector<Body*>> grid;

    void RegisterBody(Body& body);
    void DetectCollisions();
};
