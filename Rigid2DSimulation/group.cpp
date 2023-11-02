#include "group.h"
#include "body.h"
#include "connection.h"
#include "cur.h"

void Group::Warp(dRect rect)
{
	for (auto b : bodies)
		if (b->invMass == 0 || b->dragged) { return; }
	bool isInRect = false;

	isInRect = true;
	for (auto b : bodies)
		if (b->o.x > rect.left()) { isInRect = false; break; }
	if (isInRect)
		for (auto b : bodies) { b->o.x += rect.w; b->track.clear(); }

	isInRect = true;
	for (auto b : bodies)
		if (b->o.x < rect.right()) { isInRect = false; break; }
	if (isInRect)
		for (auto b : bodies) { b->o.x -= rect.w; b->track.clear(); }

	isInRect = true;
	for (auto b : bodies)
		if (b->o.y > rect.top()) { isInRect = false; break; }
	if (isInRect)
		for (auto b : bodies) { b->o.y += rect.h;  b->track.clear(); }

	isInRect = true;
	for (auto b : bodies)
		if (b->o.y < rect.bottom()) { isInRect = false; break; }
	if (isInRect)
		for (auto b : bodies) { b->o.y -= rect.h; b->track.clear(); }
}

void findBodiesInGroup(vector<Body*>& out, Body* body)
{
	body->visited = true;
	out.push_back(body);
	for (auto connnection : body->connections)
	{
		auto tmp = getTheOtherBody(body, *connnection);
		if (tmp && !tmp->visited)
		{
			findBodiesInGroup(out, tmp);
		}
	}
}
vector<ptr<Group>> FormGroups(vector<ptr<Body>> const& bodies)
{
	vector<ptr<Group>> out;
	vector<Body*> connectedBodies;
	for (auto& b : bodies) { b->visited = false; }
	for (auto& b : bodies)
	{
		if (!b->visited)
		{
			connectedBodies.clear();
			findBodiesInGroup(connectedBodies, &*b);
			out.push_back(msh<Group>(connectedBodies));
		}
	}
	return out;
}
