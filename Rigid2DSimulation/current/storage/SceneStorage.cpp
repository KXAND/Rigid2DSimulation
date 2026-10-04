#include "SceneStorage.h"
#include "Body.h"
#include "Connection.h"
#include "PhysicsWorld.h"
#include "SceneParameter.h"
#include "SceneScriptRuntime.h"
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
        body->Save(file);
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
        auto body = std::make_shared<Body>(file);
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
