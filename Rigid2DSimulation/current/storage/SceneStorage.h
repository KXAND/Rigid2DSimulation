#pragma once
#include <cstdio>
#include <functional>
#include <memory>
#include <string>

struct Body;
struct Connection;
class PhysicsWorld;
struct SceneScriptRuntime;
struct Shape;

struct SceneStorage
{
    static void Save(std::wstring const& name, PhysicsWorld const& physicsWorld,
                     SceneScriptRuntime const& runtime);
    static void Load(std::wstring const& name, PhysicsWorld& physicsWorld,
                     SceneScriptRuntime& runtime, std::function<void()> const& resetScene);

private:
    static void SaveShape(FILE* file, Shape const& shape);
    static std::shared_ptr<Shape> LoadShape(FILE* file);
    static void SaveConnection(FILE* file, Connection const& connection, PhysicsWorld const& physicsWorld);
    static std::shared_ptr<Connection> LoadConnection(FILE* file, PhysicsWorld const& physicsWorld);
    static void SaveBody(FILE* file, Body const& body);
    static std::shared_ptr<Body> LoadBody(FILE* file);
    static void SaveParameters(FILE* file, SceneScriptRuntime const& runtime);
    static void LoadParameters(FILE* file, SceneScriptRuntime& runtime);
};
