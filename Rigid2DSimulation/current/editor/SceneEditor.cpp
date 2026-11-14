#include "SceneEditor.h"

#include "AppItem.h"
#include "Body.h"
#include "Connection.h"
#include "Creator.h"
#include "PhysicsWorld.h"
#include "SceneCanvas.h"
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
    frameDrag = {};
    tracks.clear();
}

void SceneEditor::PrepareForDeletion(InputOwners& inputOwners)
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
    if (frameDrag.body && frameDrag.body->IsMarkedForDeletion())
    {
        frameDrag = {};
    }
    creator.PrepareForDeletion(inputOwners);
}

void SceneEditor::ClearTrack(Body const& body)
{
    tracks.erase(&body);
}

void SceneEditor::ResetTools(InputOwners& inputOwners)
{
    dragTool.rotationSpeed = 2;
    pendingCreatorConfig.reset();
    creator.ApplyConfig(CreatorConfig{}, inputOwners);
}

void SceneEditor::RequestCreatorConfig(Value const& config)
{
    RequestCreatorConfig(ParseCreatorConfig(config));
}

void SceneEditor::RequestCreatorConfig(CreatorConfig const& config)
{
    pendingCreatorConfig = std::make_unique<CreatorConfig>(config);
}

CreatorConfig const& SceneEditor::GetRequestedCreatorConfig() const
{
    return pendingCreatorConfig ? *pendingCreatorConfig : creator.GetConfig();
}

void SceneEditor::SelectCreationTool(int toolMode)
{
    auto config = GetRequestedCreatorConfig();
    config.mode = toolMode;
    RequestCreatorConfig(config);
    dragTool.Cancel();
    mode = kCreateMode;
}

void SceneEditor::CycleMode(InputOwners& inputOwners)
{
    creator.Discard(inputOwners);
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
    // 配置变更只在本次拾取之前生效，不能复用旧模式的输入归属。
    if (pendingCreatorConfig)
    {
        creator.ApplyConfig(*pendingCreatorConfig, inputOwners);
        pendingCreatorConfig.reset();
    }
    if (mode == kCreateMode)
    {
        creator.PreUpdate(input, inputOwners, viewport);
    }
    for (auto& body : world.GetBodies())
    {
        if (body->IsMarkedForDeletion())
        {
            continue;
        }
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

void SceneEditor::Update(PhysicsWorld& world, AppInput& input, InputOwners& inputOwners,
                         SceneCanvas const& canvas, double realDeltaTime)
{
    // 实体删除留到场景状态阶段，但工具不能继续使用此前已标记删除的端点或拖拽对象。
    PrepareForDeletion(inputOwners);
    if (inputOwners.hovered == &canvas && input.IsMouseClickedDown(0))
    {
        ClearSelection();
    }

    hoveredBody = nullptr;
    frameDrag = {};
    for (const auto& body : world.GetBodies())
    {
        if (body->IsMarkedForDeletion())
        {
            continue;
        }
        auto bodyDrag = UpdateBody(world, *body, input, inputOwners, realDeltaTime);
        if (bodyDrag.body)
        {
            frameDrag = bodyDrag;
        }
    }
    // 工具可以新增物体或连接，必须在物体遍历结束后确认创建。
    if (mode == kCreateMode && !pendingCreatorConfig)
    {
        creator.Update(world, input, inputOwners);
    }
}

CreationPreview SceneEditor::GetPreview() const
{
    return mode == kCreateMode && !pendingCreatorConfig ? creator.GetPreview() : CreationPreview{};
}

void SceneEditor::RecordTracks(PhysicsWorld const& world, bool isTrackShown, bool isPaused)
{
    for (auto const& body : world.GetBodies())
    {
        if (body.get() == bodySelecting && isTrackShown && !isPaused)
        {
            tracks[body.get()].push_back(body->position);
        }
        else if (body.get() != bodySelecting || !isTrackShown)
        {
            ClearTrack(*body);
        }
    }
}

BodyDrag SceneEditor::UpdateBody(PhysicsWorld& world, Body& body, AppInput& input, InputOwners const& inputOwners,
                                 double realDeltaTime)
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
    auto frameDrag = dragTool.UpdateBody(body, input, inputOwners, realDeltaTime, isHovered, mode == kDragMode);

    if (isHovered && mode == kCreateMode && !pendingCreatorConfig)
    {
        if (input.IsMouseClickedDown(0))
        {
            creator.OnBodyPressed(body, (Vector2)input.mousePosition);
        }
        else if (!input.mouseButtonDown[0] && input.previousMouseButtonDown[0])
        {
            creator.OnBodyReleased(body, (Vector2)input.mousePosition);
        }
    }
    return frameDrag;
}
