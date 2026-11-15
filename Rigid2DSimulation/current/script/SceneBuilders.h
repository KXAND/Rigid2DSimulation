#pragma once

struct Value;
class PhysicsWorld;
struct DRect;

void CreateBody(PhysicsWorld& physicsWorld, Value const& shapeConfigurations, Value const& configuration);
void CreateConnection(PhysicsWorld& physicsWorld, Value const& configuration);
void Boundary(PhysicsWorld& physicsWorld, DRect canvasRect, Value const& thickness, Value const& configuration);
void Gear(PhysicsWorld& physicsWorld, Value const& radius, Value const& toothCount, Value const& toothHeight, Value const& configuration);
void Strand(PhysicsWorld& physicsWorld, Value const& point0Value, Value const& point1Value,
            Value const& countValue, Value const& ratio, Value const& bodyConfiguration, Value const& connectionConfiguration);
void Necklace(PhysicsWorld& physicsWorld, Value const& centerValue, Value const& radius,
              Value const& countValue, Value const& ratio, Value const& bodyConfiguration, Value const& connectionConfiguration);
