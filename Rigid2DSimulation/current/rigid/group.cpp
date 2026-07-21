#include "group.h"
#include "body.h"
#include "connection.h"
#include "simulation_app.h"

void Group::Warp(DRect Rect)
{
	for (auto b : bodies)
		if (b->invMass == 0 || b->dragged) { return; }
	bool isInRect = false;

	isInRect = true;
	for (auto b : bodies)
		if (b->position.x > Rect.left()) { isInRect = false; break; }
	if (isInRect)
		for (auto b : bodies) { b->position.x += Rect.w; b->track.clear(); }

	isInRect = true;
	for (auto b : bodies)
		if (b->position.x < Rect.right()) { isInRect = false; break; }
	if (isInRect)
		for (auto b : bodies) { b->position.x -= Rect.w; b->track.clear(); }

	isInRect = true;
	for (auto b : bodies)
		if (b->position.y > Rect.top()) { isInRect = false; break; }
	if (isInRect)
		for (auto b : bodies) { b->position.y += Rect.h;  b->track.clear(); }

	isInRect = true;
	for (auto b : bodies)
		if (b->position.y < Rect.bottom()) { isInRect = false; break; }
	if (isInRect)
		for (auto b : bodies) { b->position.y -= Rect.h; b->track.clear(); }
}

void findBodiesInGroup(vector<Body*>& out, Body* body)
{
	body->visited = true;
	out.push_back(body);
	for (auto connnection : body->connections)
	{
		auto tmp = GetOtherBody(body, *connnection);
		if (tmp && !tmp->visited)
		{
			findBodiesInGroup(out, tmp);
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
			findBodiesInGroup(connectedBodies, &*b);
			out.push_back(std::make_shared<Group>(connectedBodies));
		}
	}
	return out;
}

