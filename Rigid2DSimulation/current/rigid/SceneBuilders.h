#pragma once

struct Value;
struct SimulationApp;

void CreateBody(SimulationApp& simulationApp, Value const& shapeConfigurations, Value const& configuration);
void CreateConnection(SimulationApp& simulationApp, Value const& configuration);
void Boundary(SimulationApp& simulationApp, Value const& thickness, Value const& configuration);
void Gear(SimulationApp& simulationApp, Value const& radius, Value const& toothCount, Value const& toothHeight, Value const& configuration);
void Strand(SimulationApp& simulationApp, Value const& point0Value, Value const& point1Value,
            Value const& countValue, Value const& ratio, Value const& bodyConfiguration, Value const& connectionConfiguration);
void Necklace(SimulationApp& simulationApp, Value const& centerValue, Value const& radius,
              Value const& countValue, Value const& ratio, Value const& bodyConfiguration, Value const& connectionConfiguration);
