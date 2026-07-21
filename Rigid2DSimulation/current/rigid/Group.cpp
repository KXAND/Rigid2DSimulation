#include "Group.h"
#include "Body.h"
#include "Connection.h"
#include "SimulationApp.h"

void Group::Warp(DRect rectangle)
{
	for (auto b : bodies)
		if (b->invMass == 0 || b->dragged) { return; }
	bool isInRect = false;

	isInRect = true;
	for (auto b : bodies)
		if (b->position.x > rectangle.GetLeft()) { isInRect = false; break; }
	if (isInRect)
		for (auto b : bodies) { b->position.x += rectangle.width; b->track.clear(); }

	isInRect = true;
	for (auto b : bodies)
		if (b->position.x < rectangle.GetRight()) { isInRect = false; break; }
	if (isInRect)
		for (auto b : bodies) { b->position.x -= rectangle.width; b->track.clear(); }

	isInRect = true;
	for (auto b : bodies)
		if (b->position.y > rectangle.GetTop()) { isInRect = false; break; }
	if (isInRect)
		for (auto b : bodies) { b->position.y += rectangle.height;  b->track.clear(); }

	isInRect = true;
	for (auto b : bodies)
		if (b->position.y < rectangle.GetBottom()) { isInRect = false; break; }
	if (isInRect)
		for (auto b : bodies) { b->position.y -= rectangle.height; b->track.clear(); }
}

void FindBodiesInGroup(vector<Body*>& bodies, Body* body)
{
	body->visited = true;
	bodies.push_back(body);
	for (auto connnection : body->connections)
	{
		auto tmp = GetOtherBody(body, *connnection);
		if (tmp && !tmp->visited)
		{
			FindBodiesInGroup(bodies, tmp);
		}
	}
}
vector<std::shared_ptr<Group>> FormGroups(vector<std::shared_ptr<Body>> const& bodies)
{
	vector<std::shared_ptr<Group>> out;
	vector<Body*> connectedBodies;
	for (auto& b : bodies) { b->visited = false; }
	for (auto& b : bodies)
	{
		if (!b->visited)
		{
			connectedBodies.clear();
			FindBodiesInGroup(connectedBodies, &*b);
			out.push_back(std::make_shared<Group>(connectedBodies));
		}
	}
	return out;
}

