#pragma once

struct Value;
struct SimulationApp;

void CreateBody(SimulationApp& cur, Value const& shs_cfg, Value const& cfg);
void CreateConnection(SimulationApp& cur, Value const& cfg);
void Boundary(SimulationApp& cur, Value const& thk, Value const& cfg);
void Gear(SimulationApp& cur, Value const& r, Value const& n, Value const& h, Value const& cfg);
void Strand(SimulationApp& cur, Value const& _p0, Value const& _p1,
	Value const& _n, Value const& ratio, Value const& bodyConfiguration, Value const& cfg_conn);
void Necklace(SimulationApp& cur, Value const& _o, Value const& rad,
	Value const& _n, Value const& ratio, Value const& bodyConfiguration, Value const& cfg_conn);