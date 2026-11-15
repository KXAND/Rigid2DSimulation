#include "SceneStorage.h"

#include <cstring>
#include <stdexcept>

#include "Body.h"
#include "Connection.h"
#include "PhysicsWorld.h"
#include "Shape.h"
#include "Utils.h"

namespace
{
    // 纯数据格式；不同标识的旧存档在重置当前场景之前拒绝。
    constexpr char kSceneHeader[] = {'R', '2', 'D', 'S', 2, 0, 0, 0};

    void ReadBytes(FILE* file, void* output, size_t size)
    {
        if (size && fread(output, 1, size, file) != size)
        {
            throw std::runtime_error("Incomplete scene data");
        }
    }

    int ReadCount(FILE* file, size_t minItemSize)
    {
        int size = 0;
        ReadBytes(file, &size, sizeof(size));
        auto position = _ftelli64(file);
        if (position < 0 || _fseeki64(file, 0, SEEK_END) != 0)
        {
            throw std::runtime_error("Cannot inspect scene data");
        }
        auto end = _ftelli64(file);
        if (_fseeki64(file, position, SEEK_SET) != 0 || end < position ||
            size < 0 || static_cast<unsigned long long>(size) > (end - position) / minItemSize)
        {
            throw std::runtime_error("Invalid scene item count");
        }
        return size;
    }
}

void SceneStorage::Save(std::wstring const& name, PhysicsWorld const& physicsWorld)
{
    std::unique_ptr<FILE, decltype(&fclose)> file(OpenFile(name, L"wb"), fclose);
    if (!file)
    {
        return;
    }
    FILE* output = file.get();
    fwrite(kSceneHeader, 1, sizeof(kSceneHeader), output);
    fwrite(&physicsWorld.gravity, sizeof(physicsWorld.gravity), 1, output);
    fwrite(&physicsWorld.coulombConstant, sizeof(physicsWorld.coulombConstant), 1, output);
    fwrite(&physicsWorld.hasElectrostatic, sizeof(physicsWorld.hasElectrostatic), 1, output);
    fwrite(&physicsWorld.simulationTime, sizeof(physicsWorld.simulationTime), 1, output);
    fwrite(&physicsWorld.stepNum, sizeof(physicsWorld.stepNum), 1, output);
    fwrite(&physicsWorld.rectScene, sizeof(physicsWorld.rectScene), 1, output);
    fwrite(&physicsWorld.parallelEpsilon, sizeof(physicsWorld.parallelEpsilon), 1, output);
    fwrite(&physicsWorld.useEqualPositionCorrection, sizeof(physicsWorld.useEqualPositionCorrection), 1, output);
    fwrite(&physicsWorld.gridSize, sizeof(physicsWorld.gridSize), 1, output);

    int size = physicsWorld.GetBodies().size();
    fwrite(&size, sizeof(size), 1, output);
    for (auto const& body : physicsWorld.GetBodies())
    {
        SaveBody(output, *body);
    }
    size = physicsWorld.GetConnections().size();
    fwrite(&size, sizeof(size), 1, output);
    for (auto const& connection : physicsWorld.GetConnections())
    {
        SaveConnection(output, *connection, physicsWorld);
    }
}

bool SceneStorage::Load(std::wstring const& name, PhysicsWorld& physicsWorld,
                        std::function<void()> const& resetScene)
{
    std::unique_ptr<FILE, decltype(&fclose)> file(OpenFile(name, L"rb"), fclose);
    if (!file)
    {
        return false;
    }
    PhysicsWorld settings;
    std::vector<std::shared_ptr<Body>> bodies;
    std::vector<std::shared_ptr<Connection>> connections;
    try
    {
        char header[sizeof(kSceneHeader)];
        ReadBytes(file.get(), header, sizeof(header));
        if (memcmp(header, kSceneHeader, sizeof(header)) != 0)
        {
            return false;
        }
        ReadBytes(file.get(), &settings.gravity, sizeof(settings.gravity));
        ReadBytes(file.get(), &settings.coulombConstant, sizeof(settings.coulombConstant));
        ReadBytes(file.get(), &settings.hasElectrostatic, sizeof(settings.hasElectrostatic));
        ReadBytes(file.get(), &settings.simulationTime, sizeof(settings.simulationTime));
        ReadBytes(file.get(), &settings.stepNum, sizeof(settings.stepNum));
        ReadBytes(file.get(), &settings.rectScene, sizeof(settings.rectScene));
        ReadBytes(file.get(), &settings.parallelEpsilon, sizeof(settings.parallelEpsilon));
        ReadBytes(file.get(), &settings.useEqualPositionCorrection, sizeof(settings.useEqualPositionCorrection));
        ReadBytes(file.get(), &settings.gridSize, sizeof(settings.gridSize));
        if (settings.stepNum < 1 || !PhysicsWorld::IsGridConfigValid(
                settings.gridSize, settings.rectScene.width, settings.rectScene.height))
        {
            return false;
        }

        int size = ReadCount(file.get(), 1);
        for (int i = 0; i < size; ++i)
        {
            bodies.push_back(LoadBody(file.get()));
        }
        size = ReadCount(file.get(), 1);
        for (int i = 0; i < size; ++i)
        {
            connections.push_back(LoadConnection(file.get(), bodies));
        }
    }
    catch (std::exception const&)
    {
        return false;
    }

    // 数据已读完且物体已完整构造，再替换当前场景；注册关联仍只通过 World。
    resetScene();
    physicsWorld.gravity = settings.gravity;
    physicsWorld.coulombConstant = settings.coulombConstant;
    physicsWorld.hasElectrostatic = settings.hasElectrostatic;
    physicsWorld.simulationTime = settings.simulationTime;
    physicsWorld.stepNum = settings.stepNum;
    physicsWorld.rectScene = settings.rectScene;
    physicsWorld.parallelEpsilon = settings.parallelEpsilon;
    physicsWorld.useEqualPositionCorrection = settings.useEqualPositionCorrection;
    physicsWorld.gridSize = settings.gridSize;
    for (auto const& body : bodies)
    {
        physicsWorld.AddBody(body);
    }
    for (auto const& connection : connections)
    {
        physicsWorld.AddConnection(connection);
    }
    physicsWorld.isSceneChanged = true;
    return true;
}

void SceneStorage::SaveBody(FILE* file, Body const& body)
{
    int size = 0;
    fwrite(&(body.point), sizeof(body.point), 1, file);
    fwrite(&(body.fixed), sizeof(body.fixed), 1, file);
    size = body.shapes.size();
    fwrite(&(size), sizeof(size), 1, file);
    for (auto shape : body.shapes)
    {
        SaveShape(file, *shape);
    }
    fwrite(&(body.position), sizeof(body.position), 1, file);
    fwrite(&(body.velocity), sizeof(body.velocity), 1, file);
    fwrite(&(body.radian), sizeof(body.radian), 1, file);
    fwrite(&(body.velocityAngular), sizeof(body.velocityAngular), 1, file);
    fwrite(&(body.density), sizeof(body.density), 1, file);
    fwrite(&(body.elasticity), sizeof(body.elasticity), 1, file);
    fwrite(&(body.frictionStatic), sizeof(body.frictionStatic), 1, file);
    fwrite(&(body.frictionDynamic), sizeof(body.frictionDynamic), 1, file);
    fwrite(&(body.dampCoeff), sizeof(body.dampCoeff), 1, file);
    fwrite(&(body.dampCoeffAngular), sizeof(body.dampCoeffAngular), 1, file);
    fwrite(&(body.chargeDensity), sizeof(body.chargeDensity), 1, file);
    fwrite(&(body.appearance.innerColor), sizeof(body.appearance.innerColor), 1, file);
}

std::shared_ptr<Body> SceneStorage::LoadBody(FILE* file)
{
    bool point = false;
    BodyProperties properties;
    ReadBytes(file, &point, sizeof(point));
    ReadBytes(file, &properties.fixed, sizeof(properties.fixed));
    int size = ReadCount(file, 1);
    std::vector<std::shared_ptr<Shape>> shapes;
    for (int i = 0; i < size; ++i)
    {
        shapes.push_back(LoadShape(file));
    }
    ReadBytes(file, &properties.position, sizeof(properties.position));
    ReadBytes(file, &properties.velocity, sizeof(properties.velocity));
    ReadBytes(file, &properties.radian, sizeof(properties.radian));
    ReadBytes(file, &properties.velocityAngular, sizeof(properties.velocityAngular));
    ReadBytes(file, &properties.density, sizeof(properties.density));
    ReadBytes(file, &properties.elasticity, sizeof(properties.elasticity));
    ReadBytes(file, &properties.frictionStatic, sizeof(properties.frictionStatic));
    ReadBytes(file, &properties.frictionDynamic, sizeof(properties.frictionDynamic));
    ReadBytes(file, &properties.dampCoeff, sizeof(properties.dampCoeff));
    ReadBytes(file, &properties.dampCoeffAngular, sizeof(properties.dampCoeffAngular));
    ReadBytes(file, &properties.chargeDensity, sizeof(properties.chargeDensity));
    ReadBytes(file, &properties.innerColor, sizeof(properties.innerColor));

    // 读取的是创建输入，而非半初始化的 Body；构造函数负责全部派生量计算。
    if (point)
    {
        if (!shapes.empty())
        {
            throw std::runtime_error("Point body must not contain shapes");
        }
        return Body::CreatePoint(properties);
    }
    if (shapes.empty())
    {
        throw std::runtime_error("Body requires shapes");
    }
    return std::make_shared<Body>(shapes, properties);
}

void SceneStorage::SaveShape(FILE* file, Shape const& shape)
{
    int size = 0;
    fwrite(&(shape.isCircle), sizeof(shape.isCircle), 1, file);
    fwrite(&(shape.radius), sizeof(shape.radius), 1, file);
    fwrite(&(shape.radian), sizeof(shape.radian), 1, file);
    fwrite(&(shape.relativePosition), sizeof(shape.relativePosition), 1, file);
    size = (shape.verticesRelative).size();
    fwrite(&(size), sizeof(size), 1, file);
    fwrite(shape.verticesRelative.data(), sizeof(Vector2), size, file);
}

std::shared_ptr<Shape> SceneStorage::LoadShape(FILE* file)
{
    auto shape = std::make_shared<Shape>();
    int size = 0;
    ReadBytes(file, &(shape->isCircle), sizeof(shape->isCircle));
    ReadBytes(file, &(shape->radius), sizeof(shape->radius));
    ReadBytes(file, &(shape->radian), sizeof(shape->radian));
    ReadBytes(file, &(shape->relativePosition), sizeof(shape->relativePosition));
    size = ReadCount(file, sizeof(Vector2));
    (shape->verticesRelative).resize(size);
    ReadBytes(file, shape->verticesRelative.data(), sizeof(Vector2) * size);
    if (!shape->isCircle && size < 3)
    {
        throw std::runtime_error("Polygon requires at least three vertices");
    }
    shape->vertices.resize(shape->verticesRelative.size());
    return shape;
}

void SceneStorage::SaveConnection(FILE* file, Connection const& connection, PhysicsWorld const& physicsWorld)
{
    fwrite(&(connection.type), sizeof(connection.type), 1, file);
    auto pred0 = [&connection](std::shared_ptr<Body> b)
    {
        return &*b == connection.body0;
    };
    int bodyIndex0 = find_if(physicsWorld.GetBodies().begin(), physicsWorld.GetBodies().end(), pred0) - physicsWorld.GetBodies().begin();
    auto pred1 = [&connection](std::shared_ptr<Body> b)
    {
        return &*b == connection.body1;
    };
    int bodyIndex1 = find_if(physicsWorld.GetBodies().begin(), physicsWorld.GetBodies().end(), pred1) - physicsWorld.GetBodies().begin();
    fwrite(&(bodyIndex0), sizeof(bodyIndex0), 1, file);
    fwrite(&(bodyIndex1), sizeof(bodyIndex1), 1, file);
    fwrite(&(connection.p0Relative), sizeof(connection.p0Relative), 1, file);
    fwrite(&(connection.p1Relative), sizeof(connection.p1Relative), 1, file);
    fwrite(&(connection.length), sizeof(connection.length), 1, file);
    fwrite(&(connection.springConstant), sizeof(connection.springConstant), 1, file);
}

std::shared_ptr<Connection> SceneStorage::LoadConnection(FILE* file, std::vector<std::shared_ptr<Body>> const& bodies)
{
    auto connection = std::make_shared<Connection>();
    ReadBytes(file, &(connection->type), sizeof(connection->type));
    int bodyIndex0 = 0, bodyIndex1 = 0;
    ReadBytes(file, &(bodyIndex0), sizeof(bodyIndex0));
    ReadBytes(file, &(bodyIndex1), sizeof(bodyIndex1));
    if (bodyIndex0 < 0 || bodyIndex1 < 0 ||
        static_cast<size_t>(bodyIndex0) >= bodies.size() || static_cast<size_t>(bodyIndex1) >= bodies.size())
    {
        throw std::runtime_error("Invalid connection endpoint");
    }
    connection->body0 = bodies[bodyIndex0].get();
    connection->body1 = bodies[bodyIndex1].get();
    ReadBytes(file, &(connection->p0Relative), sizeof(connection->p0Relative));
    ReadBytes(file, &(connection->p1Relative), sizeof(connection->p1Relative));
    ReadBytes(file, &(connection->length), sizeof(connection->length));
    ReadBytes(file, &(connection->springConstant), sizeof(connection->springConstant));
    connection->UpdatePosition();
    return connection;
}
