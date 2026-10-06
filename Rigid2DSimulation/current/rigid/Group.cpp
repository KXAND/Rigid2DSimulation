#include "Group.h"
#include "Body.h"
#include "Connection.h"

void Group::Warp(DRect rectangle, Body const* warpExcludedBody)
{
    for (auto body : bodies)
    {
        if (body->invMass == 0 || body == warpExcludedBody)
        {
            return;
        }
    }
    bool isInRect = false;

    isInRect = true;
    for (auto body : bodies)
    {
        if (body->position.x > rectangle.GetLeft())
        {
            isInRect = false;
            break;
        }
    }
    if (isInRect)
    {
        for (auto body : bodies)
        {
            body->position.x += rectangle.width;
            body->track.clear();
        }
    }

    isInRect = true;
    for (auto body : bodies)
    {
        if (body->position.x < rectangle.GetRight())
        {
            isInRect = false;
            break;
        }
    }
    if (isInRect)
    {
        for (auto body : bodies)
        {
            body->position.x -= rectangle.width;
            body->track.clear();
        }
    }

    isInRect = true;
    for (auto body : bodies)
    {
        if (body->position.y > rectangle.GetTop())
        {
            isInRect = false;
            break;
        }
    }
    if (isInRect)
    {
        for (auto body : bodies)
        {
            body->position.y += rectangle.height;
            body->track.clear();
        }
    }

    isInRect = true;
    for (auto body : bodies)
    {
        if (body->position.y < rectangle.GetBottom())
        {
            isInRect = false;
            break;
        }
    }
    if (isInRect)
    {
        for (auto body : bodies)
        {
            body->position.y -= rectangle.height;
            body->track.clear();
        }
    }
}

void FindBodiesInGroup(vector<Body*>& bodies, Body* body)
{
    body->visited = true;
    bodies.push_back(body);
    for (auto connection : body->connections)
    {
        auto tmp = GetOtherBody(body, *connection);
        if (tmp && !tmp->visited)
        {
            FindBodiesInGroup(bodies, tmp);
        }
    }
}
vector<std::shared_ptr<Group>> FormGroups(vector<std::shared_ptr<Body>> const& bodies)
{
    vector<std::shared_ptr<Group>> groups;
    vector<Body*> connectedBodies;
    for (auto& body : bodies)
    {
        body->visited = false;
    }
    for (auto& body : bodies)
    {
        if (!body->visited)
        {
            connectedBodies.clear();
            FindBodiesInGroup(connectedBodies, &*body);
            groups.push_back(std::make_shared<Group>(connectedBodies));
        }
    }
    return groups;
}
