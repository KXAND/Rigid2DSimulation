#pragma once

struct Var;
struct SimulationApp;

void CreateBody(SimulationApp& cur, Var const& shs_cfg, Var const& cfg);
void CreateConnetion(SimulationApp& cur, Var const& cfg);
void Boundary(SimulationApp& cur, Var const& thk, Var const& cfg);
void Gear(SimulationApp& cur, Var const& r, Var const& n, Var const& h, Var const& cfg);
void Strand(SimulationApp& cur, Var const& _p0, Var const& _p1,
	Var const& _n, Var const& ratio, Var const& cfg_body, Var const& cfg_conn);
void Necklace(SimulationApp& cur, Var const& _o, Var const& rad,
	Var const& _n, Var const& ratio, Var const& cfg_body, Var const& cfg_conn);