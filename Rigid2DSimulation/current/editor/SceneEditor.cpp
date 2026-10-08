#include "SceneEditor.h"

#include "AppItem.h"
#include "Body.h"
#include "Connection.h"
#include "Creator.h"
#include "PhysicsWorld.h"
#include "Shape.h"

void SceneEditor::ClearSelection()
{
    bodySelecting = NULL;
    connectionSelecting = NULL;
}

void SceneEditor::ResetInteraction()
{
    ClearSelection();
    hoveredBody = nullptr;
    dragTool.Cancel();
    tracks.clear();
}

void SceneEditor::PrepareForDeletion()
{
    for (auto it = tracks.begin(); it != tracks.end();)
    {
        if (it->first->IsMarkedForDeletion())
        {
            it = tracks.erase(it);
        }
        else
        {
            ++it;
        }
    }
    if (bodySelecting && bodySelecting->IsMarkedForDeletion())
    {
        bodySelecting = NULL;
    }
    if (connectionSelecting && connectionSelecting->IsMarkedForDeletion())
    {
        connectionSelecting = NULL;
    }
    if (hoveredBody && hoveredBody->IsMarkedForDeletion())
    {
        hoveredBody = nullptr;
    }
    dragTool.PrepareForDeletion();
}

void SceneEditor::ClearTrack(Body const& body)
{
    tracks.erase(&body);
}

void SceneEditor::ResetTools()
{
    dragTool.rotationSpeed = 2;
    creator = std::make_shared<Creator>();
}

void SceneEditor::CycleMode(InputOwners& inputOwners)
{
    creator->Discard(inputOwners);
    mode = (mode + 1) % 4;
}

void SceneEditor::RequestBodyDeletion(PhysicsWorld& world)
{
    world.RequestBodyDeletion(*bodySelecting);
}

void SceneEditor::RequestConnectionDeletion(PhysicsWorld& world)
{
    world.RequestConnectionDeletion(*connectionSelecting);
}

bool SceneEditor::HitTestBody(Body const& body, Vector2 mousePosition)
{
    if (body.point)
    {
        return (mousePosition - body.position).GetLengthSquared() < body.appearance.pointRadius * body.appearance.pointRadius;
    }

    for (auto const& shape : body.shapes)
    {
        if (shape->IsInside(mousePosition))
        {
            return true;
        }
    }
    return false;
}

void SceneEditor::PreUpdate(PhysicsWorld const& world, AppInput const& input,
                            InputOwners& inputOwners, DRect viewport)
{
    if (mode == kCreateMode)
    {
        creator->PreUpdate(input, inputOwners, viewport);
    }
    for (auto& body : world.GetBodies())
    {
        double depth = body->appearance.GetDepth(body->point);
        bool canReceiveHover = inputOwners.hoveredDepth <= depth &&
                               HitTestBody(*body, (Vector2)input.mousePosition) && ::IsInside(input.mousePosition, viewport);
        if (canReceiveHover)
        {
            inputOwners.hoveredDepth = depth;
            inputOwners.hovered = body.get();
        }
    }
}

void SceneEditor::UpdateBodies(PhysicsWorld& world, AppInput& input, InputOwners const& inputOwners,
                               double realDeltaTime, bool isTrackShown, bool isPaused)
{
    hoveredBody = nullptr;
    for (const auto& body : world.GetBodies())
    {
        UpdateBody(world, *body, input, inputOwners, realDeltaTime, isTrackShown, isPaused);
    }
}

void SceneEditor::UpdateBody(PhysicsWorld& world, Body& body, AppInput& input, InputOwners const& inputOwners,
                             double realDeltaTime, bool isTrackShown, bool isPaused)
{
    bool isHovered = inputOwners.hovered == &body;
    if (isHovered)
    {
        hoveredBody = &body;
    }

    if (mode == kDeleteMode && isHovered && input.IsMouseClickedDown(0))
    {
        world.RequestBodyDeletion(body);
    }
    if (mode == kSelectMode && isHovered && input.IsMouseClickedDown(0))
    {
        bodySelecting = &body;
    }
    dragTool.UpdateBody(body, input, inputOwners, realDeltaTime, isHovered, mode == kDragMode);

    if (&body == bodySelecting && isTrackShown && !isPaused)
    {
        // 绘制运动轨迹
        tracks[&body].push_back(body.position);
    }
    else if (&body != bodySelecting || !isTrackShown)
    {
        //不符合条件时清空轨迹信息
        ClearTrack(body);
    }

    if (isHovered && mode == kCreateMode)
    {
        if (input.IsMouseClickedDown(0))
        {
            creator->OnBodyPressed(body, (Vector2)input.mousePosition);
        }
        else if (!input.mouseButtonDown[0] && input.previousMouseButtonDown[0])
        {
            creator->OnBodyReleased(body, (Vector2)input.mousePosition);
        }
    }
}
