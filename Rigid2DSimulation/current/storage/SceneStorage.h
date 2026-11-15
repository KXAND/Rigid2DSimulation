#pragma once
#include <cstdio>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class Body;
class Connection;
class PhysicsWorld;
class Shape;

class SceneStorage
{
public:
    static void Save(std::wstring const& name, PhysicsWorld const& physicsWorld);
    static bool Load(std::wstring const& name, PhysicsWorld& physicsWorld,
                     std::function<void()> const& resetScene);

private:
    static void SaveShape(FILE* file, Shape const& shape);
    static std::shared_ptr<Shape> LoadShape(FILE* file);
    static void SaveConnection(FILE* file, Connection const& connection, PhysicsWorld const& physicsWorld);
    static std::shared_ptr<Connection> LoadConnection(FILE* file, std::vector<std::shared_ptr<Body>> const& bodies);
    static void SaveBody(FILE* file, Body const& body);
    static std::shared_ptr<Body> LoadBody(FILE* file);
};
