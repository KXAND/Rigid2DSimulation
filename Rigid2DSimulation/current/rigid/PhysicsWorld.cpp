#include "PhysicsWorld.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "Body.h"
#include "Collision.h"
#include "Connection.h"
#include "Group.h"
#include "Utils.h"

bool PhysicsWorld::IsGridConfigValid(double gridSize, int sceneWidth, int sceneHeight)
{
    if (!std::isfinite(gridSize) || gridSize <= 0 || sceneWidth <= 0 || sceneHeight <= 0)
    {
        return false;
    }

    // 暂留为 double，避免极小 gridSize 产生过大的商后直接转成 int。
    double columns = std::ceil(sceneWidth / gridSize);
    double rows = std::ceil(sceneHeight / gridSize);
    // 先限制单轴（也拒绝无穷大），再检查总格数；通过后 int 转换及索引乘积均安全。
    return columns <= kMaxGridCells && rows <= kMaxGridCells && columns * rows <= kMaxGridCells;
}

void PhysicsWorld::AddBody(std::shared_ptr<Body> const& body)
{
    if (!body || body->isMarkedForDeletion || std::find(bodies.begin(), bodies.end(), body) != bodies.end() ||
        !body->connections.empty())
    {
        throw std::invalid_argument("Body must be non-null, unregistered, not pending deletion and have no connections.");
    }
    bodies.push_back(body);
    isSceneChanged = true;
}

void PhysicsWorld::AddConnection(std::shared_ptr<Connection> const& connection)
{
    auto containsBody = [this](Body const* body)
    {
        return std::any_of(bodies.begin(), bodies.end(), [body](std::shared_ptr<Body> const& item)
        {
            return item.get() == body;
        });
    };
    if (!connection || connection->isMarkedForDeletion ||
        std::find(connections.begin(), connections.end(), connection) != connections.end() ||
        !containsBody(connection->body0) || !containsBody(connection->body1) ||
        connection->body0->isMarkedForDeletion || connection->body1->isMarkedForDeletion)
    {
        throw std::invalid_argument("Connection must be unregistered, both endpoints must belong to this world, and none may be pending deletion.");
    }

    auto& firstBodyConnections = connection->body0->connections;
    auto& secondBodyConnections = connection->body1->connections;
    // 在修改三个容器前准备容量，避免分配失败留下不完整的关联。
    if (connections.size() == connections.capacity())
    {
        connections.reserve(connections.size() * 2 + 1);
    }
    if (firstBodyConnections.size() == firstBodyConnections.capacity())
    {
        firstBodyConnections.reserve(firstBodyConnections.size() * 2 + 1);
    }
    if (secondBodyConnections.size() == secondBodyConnections.capacity())
    {
        secondBodyConnections.reserve(secondBodyConnections.size() * 2 + 1);
    }
    connections.push_back(connection);
    firstBodyConnections.push_back(connection.get());
    if (connection->body0 != connection->body1)
    {
        secondBodyConnections.push_back(connection.get());
    }
    isSceneChanged = true;
}

void PhysicsWorld::RequestBodyDeletion(Body& body)
{
    auto found = std::find_if(bodies.begin(), bodies.end(), [&body](std::shared_ptr<Body> const& item)
    {
        return item.get() == &body;
    });
    if (found == bodies.end())
    {
        throw std::invalid_argument("Body must belong to this world.");
    }
    body.isMarkedForDeletion = true;
    for (auto connection : body.connections)
    {
        connection->isMarkedForDeletion = true;
    }
    isSceneChanged = true;
}

void PhysicsWorld::RequestConnectionDeletion(Connection& connection)
{
    auto found = std::find_if(connections.begin(), connections.end(), [&connection](std::shared_ptr<Connection> const& item)
    {
        return item.get() == &connection;
    });
    if (found == connections.end())
    {
        throw std::invalid_argument("Connection must belong to this world.");
    }
    connection.isMarkedForDeletion = true;
    isSceneChanged = true;
}

void PhysicsWorld::ClearObjects()
{
    for (auto const& body : bodies)
    {
        body->connections.clear();
    }
    groups.clear();
    collisions.clear();
    grid.clear();
    multiGridBodies.clear();
    connections.clear();
    bodies.clear();
    isSceneChanged = true;
}

void PhysicsWorld::ResetParameters(DRect const& sceneRect)
{
    isSceneChanged = false;
    gravity = Vector2();
    simulationTime = 0;
    stepNum = 60;
    rectScene = sceneRect;
    gridSize = 50;
    hasElectrostatic = false;
    coulombConstant = 1e5;
    parallelEpsilon = 1e-3;
    useEqualPositionCorrection = false;
}

void PhysicsWorld::RemoveDeletedObjects()
{
    // 先解除连接的两端关联，物体在此阶段仍然有效。
    for (auto& connection : connections)
    {
        if (connection->isMarkedForDeletion)
        {
            //将该连接从其两端的物体的连接列表中移除。
            auto& firstBodyConnections = connection->body0->connections;
            firstBodyConnections.erase(remove(firstBodyConnections.begin(), firstBodyConnections.end(), &*connection), firstBodyConnections.end());
            auto& secondBodyConnections = connection->body1->connections;
            secondBodyConnections.erase(remove(secondBodyConnections.begin(), secondBodyConnections.end(), &*connection), secondBodyConnections.end());
        }
    }
    auto connectionsEnd = std::remove_if(connections.begin(), connections.end(),
        [](std::shared_ptr<Connection> const& connection)
        {
            return connection->isMarkedForDeletion;
        });
    if (connectionsEnd != connections.end())
    {
        isSceneChanged = true;
        connections.erase(connectionsEnd, connections.end());
    }

    // 连接清理完成后再移除物体；移除时重新使分组失效。
    auto bodiesEnd = std::remove_if(bodies.begin(), bodies.end(),
        [](std::shared_ptr<Body> const& body)
        {
            return body->isMarkedForDeletion;
        });
    if (bodiesEnd != bodies.end())
    {
        isSceneChanged = true;
        bodies.erase(bodiesEnd, bodies.end());
    }
}

void PhysicsWorld::Prepare()
{
    if (!IsGridConfigValid(gridSize, rectScene.width, rectScene.height))
    {
        throw std::invalid_argument("Invalid scene grid configuration.");
    }
    gridNumX = static_cast<int>(std::ceil(rectScene.width / gridSize));
    gridNumY = static_cast<int>(std::ceil(rectScene.height / gridSize));
    if (isSceneChanged)
    {
        isSceneChanged = false;
        groups = FormGroups(bodies);
    }
}

void PhysicsWorld::UpdateGeometry()
{
    for (auto const& body : bodies)
    {
        body->UpdateShapesPositionAndAABB();
    }
    for (auto const& connection : connections)
    {
        connection->UpdatePosition();
    }
}

void PhysicsWorld::Advance(double deltaTime, Body const* warpExcludedBody,
                           std::function<void(double)> const& beginStep,
                           std::function<void(Body&, double)> const& stepBody,
                           std::function<void(Body const&)> const& onBodyWarped)
{
    // 标准时长
    double stepDeltaTime = deltaTime / stepNum;

    for (int i = (0), iEnd = int(stepNum); i < iEnd; ++i)
    {
        beginStep(simulationTime);
        for (const auto& body : bodies)
        {
            stepBody(*body, stepDeltaTime);
        }
        for (auto& group : groups)
        {
            group->Warp(rectScene, warpExcludedBody, onBodyWarped);
        }

        //计算静电
        if (hasElectrostatic)
        {
            for (int i = (0), iEnd = int(bodies.size() - 1); i < iEnd; ++i)
            {
                for (int j = (i + 1), jEnd = int(bodies.size()); j < jEnd; ++j)
                {
                    Electrostatic(*bodies[i], *bodies[j], stepDeltaTime, coulombConstant);
                }
            }
        }

        // 重新检查物体在grid中的情况
        multiGridBodies.clear();
        grid.clear();
        grid.resize(gridNumX * gridNumY);
        for (auto& body : bodies)
        {
            RegisterBody(*body);
        }

        // 碰撞
        collisions.clear();
        DetectCollisions();
        for (auto& collision : collisions)
        {
            collision->Simulate(useEqualPositionCorrection);
        }

        // 连接
        for (auto& connection : connections)
        {
            connection->Simulate(stepDeltaTime, useEqualPositionCorrection);
        }

        simulationTime += stepDeltaTime;
    }
}

void PhysicsWorld::RegisterBody(Body& body)
{
    if (body.point)
    {
        return;
    }

    auto const& topLeft = rectScene.topLeftPosition;
    int x0 = floor((body.box.x0 - topLeft.x) / gridSize);
    int y0 = floor((body.box.y0 - topLeft.y) / gridSize);
    int x1 = floor((body.box.x1 - topLeft.x) / gridSize);
    int y1 = floor((body.box.y1 - topLeft.y) / gridSize);
    x0 = Clamp(x0, 0, gridNumX - 1);
    x1 = Clamp(x1, 0, gridNumX - 1);
    y0 = Clamp(y0, 0, gridNumY - 1);
    y1 = Clamp(y1, 0, gridNumY - 1);
    if (x1 > x0 + 1 || y1 > y0 + 1)
    {
        // 是多个跨越多个grid的物体
        multiGridBodies.push_back(&body);
        return;
    }
    grid[y0 * gridNumX + x0].push_back(&body);
}

void PhysicsWorld::DetectCollisions()
{
    for (int i = (0), iEnd = int(gridNumX); i < iEnd; ++i)
    {
        for (int j = (0), jEnd = int(gridNumY); j < jEnd; ++j)
        {
            auto& bodies = grid[j * gridNumX + i];
            for (int k = (0), kEnd = int(bodies.size()); k < kEnd; ++k)
            {
                auto& body = *bodies[k];

                // 检查同grid和跨grid的情况，输出到collisions中
                for (int m = (k + 1), mEnd = int(bodies.size()); m < mEnd; ++m)
                {
                    Collide(body, *bodies[m], collisions, parallelEpsilon);
                }
                for (auto otherBody : multiGridBodies)
                {
                    Collide(body, *otherBody, collisions, parallelEpsilon);
                }

                // 检查右邻
                if (i + 1 < gridNumX)
                {
                    int a = i + 1, b = j;
                    for (auto otherBody : grid[b * gridNumX + a])
                    {
                        Collide(body, *otherBody, collisions, parallelEpsilon);
                    }
                }
                if (j + 1 >= gridNumY)
                {
                    continue;
                } // 检查下邻，没有下邻就无法进行下列操作了

                //检查左下
                if (i - 1 >= 0)
                {
                    int a = i - 1, b = j + 1;
                    for (auto otherBody : grid[b * gridNumX + a])
                    {
                        Collide(body, *otherBody, collisions, parallelEpsilon);
                    }
                }
                // 检查右下
                if (i + 1 < gridNumX)
                {
                    int a = i + 1, b = j + 1;
                    for (auto otherBody : grid[b * gridNumX + a])
                    {
                        Collide(body, *otherBody, collisions, parallelEpsilon);
                    }
                }
                // 检查正下
                int a = i, b = j + 1;
                for (auto otherBody : grid[b * gridNumX + a])
                {
                    Collide(body, *otherBody, collisions, parallelEpsilon);
                }
            }
        }
    }

    // 检查多gird和多gird本身的碰撞
    for (int i = (0), iEnd = int(multiGridBodies.size() - 1); i < iEnd; ++i)
    {
        for (int j = (i + 1), jEnd = int(multiGridBodies.size()); j < jEnd; ++j)
        {
            Collide(*multiGridBodies[i], *multiGridBodies[j], collisions, parallelEpsilon);
        }
    }
}
