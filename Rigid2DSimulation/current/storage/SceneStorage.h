#pragma once
#include <cstdio>
#include <functional>
#include <string>

class PhysicsWorld;
struct SceneScriptRuntime;

struct SceneStorage
{
    static void Save(std::wstring const& name, PhysicsWorld const& physicsWorld,
                     SceneScriptRuntime const& runtime);
    static void Load(std::wstring const& name, PhysicsWorld& physicsWorld,
                     SceneScriptRuntime& runtime, std::function<void()> const& resetScene);

private:
    static void SaveParameters(FILE* file, SceneScriptRuntime const& runtime);
    static void LoadParameters(FILE* file, SceneScriptRuntime& runtime);
};
