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
        connection->Save(physicsWorld, file);
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
        auto connection = std::make_shared<Connection>(physicsWorld, file);
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
        parameter->Save(file);
    }
}
void SceneStorage::LoadParameters(FILE* file, SceneScriptRuntime& runtime)
{
    int size = 0;
    fread(&(size), sizeof(size), 1, file);
    for (int i = (0), iEnd = int(size); i < iEnd; ++i)
    {
        runtime.parameters.push_back(std::make_shared<SceneParameter>(file));
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
        shape->Save(file);
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
        body->shapes.push_back(std::make_shared<Shape>(file));
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
