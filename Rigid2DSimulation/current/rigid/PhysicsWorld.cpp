#include "PhysicsWorld.h"

#include <algorithm>
#include <cmath>

#include "Body.h"
#include "Collision.h"
#include "Connection.h"
#include "Group.h"
#include "Utils.h"

void PhysicsWorld::ClearObjects()
{
    bodies.clear();
    groups.clear();
    collisions.clear();
    connections.clear();
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

void PhysicsWorld::MarkDeletedConnections()
{
    // 删除被标记为删除的图形和其上的连接
    for (auto body : bodies)
    {
        if (body->isMarkedForDeletion)
        {
            for (auto connection : body->connections)
            {
                connection->isMarkedForDeletion = true;
            }
        }
    }
}

void PhysicsWorld::RemoveDeletedObjects()
{
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
    connections.erase(
        remove_if(connections.begin(), connections.end(),
                  [](std::shared_ptr<Connection> connection)
                  {
                      return connection->isMarkedForDeletion;
                  }),
        connections.end());
    bodies.erase(
        remove_if(bodies.begin(), bodies.end(),
                  [](std::shared_ptr<Body> body)
                  {
                      return body->isMarkedForDeletion;
                  }),
        bodies.end());
}

void PhysicsWorld::Prepare()
{
    gridNumX = ceil(rectScene.width / gridSize);
    gridNumY = ceil(rectScene.height / gridSize);
    if (isSceneChanged)
    {
        isSceneChanged = false;
        groups = FormGroups(bodies);
    }
}

void PhysicsWorld::Advance(double deltaTime, Body const* warpExcludedBody,
                           std::function<void(double)> const& beginStep,
                           std::function<void(Body&, double)> const& stepBody)
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
            group->Warp(rectScene, warpExcludedBody);
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
