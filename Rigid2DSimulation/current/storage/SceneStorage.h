#pragma once
#include <cstdio>
#include <functional>
#include <memory>
#include <string>

struct Body;
class PhysicsWorld;
struct SceneScriptRuntime;

struct SceneStorage
{
    static void Save(std::wstring const& name, PhysicsWorld const& physicsWorld,
                     SceneScriptRuntime const& runtime);
    static void Load(std::wstring const& name, PhysicsWorld& physicsWorld,
                     SceneScriptRuntime& runtime, std::function<void()> const& resetScene);

private:
    static void SaveBody(FILE* file, Body const& body);
    static std::shared_ptr<Body> LoadBody(FILE* file);
    static void SaveParameters(FILE* file, SceneScriptRuntime const& runtime);
    static void LoadParameters(FILE* file, SceneScriptRuntime& runtime);
};
