#include "SceneStorage.h"
#include "Body.h"
#include "Connection.h"
#include "PhysicsWorld.h"
#include "SceneParameter.h"
#include "SceneScriptRuntime.h"
#include "Shape.h"
#include "Utils.h"

void SceneStorage::Save(std::wstring const& name, PhysicsWorld const& physicsWorld,
                        SceneScriptRuntime const& runtime)
{
    FILE* file = OpenFile(name, L"wb");
    if (!file)
    {
        return;
    }
    int size = 0;
    size = (runtime.scriptText).size();
    fwrite(&(size), sizeof(size), 1, file);
    fwrite(&*(runtime.scriptText).begin(), sizeof(*(&*(runtime.scriptText).begin())), size, file);
    SaveParameters(file, runtime);

    // 可能会保存一些即将被删除的元素，但是也无所谓。
    size = physicsWorld.bodies.size();
    fwrite(&(size), sizeof(size), 1, file);
    for (auto body : physicsWorld.bodies)
    {
        SaveBody(file, *body);
    }
    size = physicsWorld.connections.size();
    fwrite(&(size), sizeof(size), 1, file);
    for (auto connection : physicsWorld.connections)
    {
        SaveConnection(file, *connection, physicsWorld);
    }

    fclose(file);
}

void SceneStorage::Load(std::wstring const& name, PhysicsWorld& physicsWorld,
                        SceneScriptRuntime& runtime, std::function<void()> const& resetScene)
{
    FILE* file = OpenFile(name, L"rb");
    if (!file)
    {
        return;
    }
    resetScene();
    int size = 0;
    fread(&(size), sizeof(size), 1, file);
    (runtime.scriptText).resize(size);
    fread(&*(runtime.scriptText).begin(), sizeof(*(&*(runtime.scriptText).begin())), size, file);
    runtime.temporaryScriptText = runtime.scriptText;
    LoadParameters(file, runtime);

    fread(&(size), sizeof(size), 1, file);
    for (int i = (0), iEnd = int(size); i < iEnd; ++i)
    {
        auto body = LoadBody(file);
        runtime.RefreshBodyScript(*body);
        physicsWorld.bodies.push_back(body);
    }
    fread(&(size), sizeof(size), 1, file);
    for (int i = (0), iEnd = int(size); i < iEnd; ++i)
    {
        auto connection = LoadConnection(file, physicsWorld);
        runtime.RefreshConnectionScript(*connection);
        connection->SignUpToBodies();
        connection->UpdatePosition();
        physicsWorld.connections.push_back(connection);
    }
    physicsWorld.isSceneChanged = true;

    runtime.ExecuteSceneScript();
    fclose(file);
}

void SceneStorage::SaveParameters(FILE* file, SceneScriptRuntime const& runtime)
{
    int size = runtime.parameters.size();
    fwrite(&(size), sizeof(size), 1, file);
    for (auto parameter : runtime.parameters)
    {
        int nameLen = parameter->name.size();
        fwrite(&(nameLen), sizeof(nameLen), 1, file);
        fwrite(&*(parameter->name).begin(), sizeof(*(&*(parameter->name).begin())), nameLen, file);
        fwrite(&(parameter->value), sizeof(parameter->value), 1, file);
    }
}
void SceneStorage::LoadParameters(FILE* file, SceneScriptRuntime& runtime)
{
    int size = 0;
    fread(&(size), sizeof(size), 1, file);
    for (int i = (0), iEnd = int(size); i < iEnd; ++i)
    {
        auto parameter = std::make_shared<SceneParameter>();
        int nameLen = 0;
        fread(&(nameLen), sizeof(nameLen), 1, file);
        (parameter->name).resize(nameLen);
        fread(&*(parameter->name).begin(), sizeof(*(&*(parameter->name).begin())), nameLen, file);
        fread(&(parameter->value), sizeof(parameter->value), 1, file);
        runtime.parameters.push_back(parameter);
    }
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
    size = (body.scriptData.scriptText).size();
    fwrite(&(size), sizeof(size), 1, file);
    fwrite(&*(body.scriptData.scriptText).begin(), sizeof(*(&*(body.scriptData.scriptText).begin())), size, file);
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
    auto body = std::make_shared<Body>();
    int size = 0;
    fread(&(body->point), sizeof(body->point), 1, file);
    fread(&(body->fixed), sizeof(body->fixed), 1, file);
    fread(&(size), sizeof(size), 1, file);
    for (int i = (0), iEnd = int(size); i < iEnd; ++i)
    {
        body->shapes.push_back(LoadShape(file));
    }
    fread(&(body->position), sizeof(body->position), 1, file);
    fread(&(body->velocity), sizeof(body->velocity), 1, file);
    fread(&(size), sizeof(size), 1, file);
    (body->scriptData.scriptText).resize(size);
    fread(&*(body->scriptData.scriptText).begin(), sizeof(*(&*(body->scriptData.scriptText).begin())), size, file);
    body->scriptData.temporaryScriptText = body->scriptData.scriptText;
    fread(&(body->radian), sizeof(body->radian), 1, file);
    fread(&(body->velocityAngular), sizeof(body->velocityAngular), 1, file);
    fread(&(body->density), sizeof(body->density), 1, file);
    fread(&(body->elasticity), sizeof(body->elasticity), 1, file);
    fread(&(body->frictionStatic), sizeof(body->frictionStatic), 1, file);
    fread(&(body->frictionDynamic), sizeof(body->frictionDynamic), 1, file);
    fread(&(body->dampCoeff), sizeof(body->dampCoeff), 1, file);
    fread(&(body->dampCoeffAngular), sizeof(body->dampCoeffAngular), 1, file);
    fread(&(body->chargeDensity), sizeof(body->chargeDensity), 1, file);
    fread(&(body->appearance.innerColor), sizeof(body->appearance.innerColor), 1, file);
    return body;
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
    fwrite(&*(shape.verticesRelative).begin(), sizeof(*(&*(shape.verticesRelative).begin())), size, file);
}

std::shared_ptr<Shape> SceneStorage::LoadShape(FILE* file)
{
    auto shape = std::make_shared<Shape>();
    int size = 0;
    fread(&(shape->isCircle), sizeof(shape->isCircle), 1, file);
    fread(&(shape->radius), sizeof(shape->radius), 1, file);
    fread(&(shape->radian), sizeof(shape->radian), 1, file);
    fread(&(shape->relativePosition), sizeof(shape->relativePosition), 1, file);
    fread(&(size), sizeof(size), 1, file);
    (shape->verticesRelative).resize(size);
    fread(&*(shape->verticesRelative).begin(), sizeof(*(&*(shape->verticesRelative).begin())), size, file);
    shape->vertices.resize(shape->verticesRelative.size());
    return shape;
}

void SceneStorage::SaveConnection(FILE* file, Connection const& connection, PhysicsWorld const& physicsWorld)
{
    int size = 0;
    fwrite(&(connection.type), sizeof(connection.type), 1, file);
    size = (connection.scriptText).size();
    fwrite(&(size), sizeof(size), 1, file);
    fwrite(&*(connection.scriptText).begin(), sizeof(*(&*(connection.scriptText).begin())), size, file);
    auto pred0 = [&connection](std::shared_ptr<Body> b)
    {
        return &*b == connection.body0;
    };
    int bodyIndex0 = find_if(physicsWorld.bodies.begin(), physicsWorld.bodies.end(), pred0) - physicsWorld.bodies.begin();
    auto pred1 = [&connection](std::shared_ptr<Body> b)
    {
        return &*b == connection.body1;
    };
    int bodyIndex1 = find_if(physicsWorld.bodies.begin(), physicsWorld.bodies.end(), pred1) - physicsWorld.bodies.begin();
    fwrite(&(bodyIndex0), sizeof(bodyIndex0), 1, file);
    fwrite(&(bodyIndex1), sizeof(bodyIndex1), 1, file);
    fwrite(&(connection.p0Relative), sizeof(connection.p0Relative), 1, file);
    fwrite(&(connection.p1Relative), sizeof(connection.p1Relative), 1, file);
    fwrite(&(connection.length), sizeof(connection.length), 1, file);
    fwrite(&(connection.springConstant), sizeof(connection.springConstant), 1, file);
}

std::shared_ptr<Connection> SceneStorage::LoadConnection(FILE* file, PhysicsWorld const& physicsWorld)
{
    auto connection = std::make_shared<Connection>();
    int size = 0;
    fread(&(connection->type), sizeof(connection->type), 1, file);
    fread(&(size), sizeof(size), 1, file);
    (connection->scriptText).resize(size);
    fread(&*(connection->scriptText).begin(), sizeof(*(&*(connection->scriptText).begin())), size, file);
    connection->temporaryScriptText = connection->scriptText;
    int bodyIndex0 = 0, bodyIndex1 = 0;
    fread(&(bodyIndex0), sizeof(bodyIndex0), 1, file);
    fread(&(bodyIndex1), sizeof(bodyIndex1), 1, file);
    connection->body0 = &*physicsWorld.bodies[bodyIndex0];
    connection->body1 = &*physicsWorld.bodies[bodyIndex1];
    fread(&(connection->p0Relative), sizeof(connection->p0Relative), 1, file);
    fread(&(connection->p1Relative), sizeof(connection->p1Relative), 1, file);
    fread(&(connection->length), sizeof(connection->length), 1, file);
    fread(&(connection->springConstant), sizeof(connection->springConstant), 1, file);
    return connection;
}
