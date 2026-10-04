#include "SceneEditor.h"

#include "AppItem.h"
#include "Body.h"
#include "Connection.h"
#include "Creator.h"
#include "PhysicsWorld.h"

void SceneEditor::ClearSelection()
{
    bodySelecting = NULL;
    connectionSelecting = NULL;
}

void SceneEditor::ClearDeletedSelection()
{
    if (bodySelecting && bodySelecting->isMarkedForDeletion)
    {
        bodySelecting = NULL;
    }
    if (connectionSelecting && connectionSelecting->isMarkedForDeletion)
    {
        connectionSelecting = NULL;
    }
}

void SceneEditor::ResetTools()
{
    velocityAngularDrag = 2;
    creator = std::make_shared<Creator>();
}

void SceneEditor::CycleMode(InputOwners& inputOwners)
{
    creator->Discard(inputOwners);
    mode = (mode + 1) % 4;
}

void SceneEditor::RequestBodyDeletion(PhysicsWorld& world)
{
    bodySelecting->isMarkedForDeletion = true;
    world.isSceneChanged = true;
}

void SceneEditor::RequestConnectionDeletion(PhysicsWorld& world)
{
    connectionSelecting->isMarkedForDeletion = true;
    world.isSceneChanged = true;
}

void SceneEditor::PreUpdate(PhysicsWorld const& world, AppInput const& input,
                            InputOwners& inputOwners, DRect viewport)
{
    if (mode == kCreateMode)
    {
        creator->PreUpdate(input, inputOwners, viewport);
    }
    for (auto& body : world.bodies)
    {
        bool canReceiveHover = inputOwners.hoveredDepth <= body->GetDepth() &&
                               body->IsInside((Vector2)input.mousePosition) && ::IsInside(input.mousePosition, viewport);
        if (canReceiveHover)
        {
            inputOwners.hoveredDepth = body->GetDepth();
            inputOwners.hovered = body.get();
        }
    }
}

void SceneEditor::UpdateBodies(PhysicsWorld& world, AppInput& input, InputOwners const& inputOwners,
                               double realDeltaTime, bool isTrackShown, bool isPaused)
{
    for (const auto& body : world.bodies)
    {
        UpdateBody(*body, input, inputOwners, realDeltaTime, isTrackShown, isPaused);
    }
}

void SceneEditor::ApplyDrag(Body& body, AppInput const& input, InputOwners const& inputOwners,
                            double realDeltaTime, double stepDeltaTime)
{
    if (body.dragged)
    {
        if (input.mouseButtonDown[2])
        {
            HandleDragWhole(body, input, realDeltaTime);
        }
        else if (!inputOwners.keyboardOwner && input.keyDown[L'F'])
        {
            HandleDragForce(body, input, stepDeltaTime);
        }
        else if (body.invMass == 0)
        {
            HandleDragWhole(body, input, realDeltaTime);
        }
        else
        {
            HandleDragPoint(body, input, realDeltaTime);
        }
    }
}

void SceneEditor::UpdateDragAngle(Body& body, AppInput const& input, InputOwners const& inputOwners, double realDeltaTime)
{
    body.velocityAngularDrag = 0;
    if (!inputOwners.keyboardOwner)
    {
        if (input.keyDown[L'Q'])
        {
            body.velocityAngularDrag = -velocityAngularDrag;
        }
        if (input.keyDown[L'E'])
        {
            body.velocityAngularDrag = +velocityAngularDrag;
        }
    }
    body.radianDrag = body.radian + body.velocityAngularDrag * realDeltaTime;
}

void SceneEditor::HandleDragPoint(Body& body, AppInput const& input, double realDeltaTime)
{
    // 下列命名指定o,p,m三点为body中心、被拖拽点及鼠标现在点
    Vector2 mousePos = Vector2(input.mousePosition);
    Vector2 pPos = body.position + body.transform * body.relativeDragPoint;
    Vector2 mouseVelocity = Vector2(input.mousePosition - input.previousMousePosition) / realDeltaTime;
    Vector2 pmDir = (mousePos - pPos).GetNormalized();

    Vector2 opDir = pPos - body.position;
    opDir = Vector2(-opDir.y, opDir.x);
    Vector2 pVelocity = body.velocity + body.velocityAngular * opDir;
    double rd0 = Dot(opDir, pmDir);
    double effectiveInvMass = body.invMass + rd0 * rd0 * body.invInertia;

    Vector2 impulse = Dot(mouseVelocity - pVelocity, pmDir) * pmDir / effectiveInvMass;

    body.velocity += impulse * body.invMass;
    body.velocityAngular += Dot(impulse, opDir) * body.invInertia;
    body.position += mousePos - pPos;
}

// 所有点同步被拖拽位移。
void SceneEditor::HandleDragWhole(Body& body, AppInput const& input, double realDeltaTime)
{
    Vector2 mousePos = Vector2(input.mousePosition);
    Vector2 dragPointPos = body.position + body.transform * body.relativeDragPoint;
    Vector2 newVelocity = Vector2(input.mousePosition - input.previousMousePosition) / realDeltaTime;

    body.velocity = newVelocity;
    body.radian = body.radianDrag;
    body.velocityAngular = body.velocityAngularDrag;
    body.position += mousePos - dragPointPos; // 中心平行位移
}

void SceneEditor::HandleDragForce(Body& body, AppInput const& input, double deltaTime)
{
    // 运动路径方向
    Vector2 p1 = Vector2(input.mousePosition);
    Vector2 p0 = body.position + body.transform * body.relativeDragPoint;
    Vector2 d = (p1 - p0).GetNormalized();

    // 力
    double k = 1e6;
    Vector2 impulse = k * (p1 - p0).GetLength() * deltaTime * d;

    // 刚体半径
    Vector2 r0 = p0 - body.position;
    r0 = Vector2(-r0.y, r0.x);

    body.velocity += impulse * body.invMass;
    body.velocityAngular += Dot(impulse, r0) * body.invInertia;
}

void SceneEditor::UpdateBody(Body& body, AppInput& input, InputOwners const& inputOwners,
                             double realDeltaTime, bool isTrackShown, bool isPaused)
{
    body.hovered = (inputOwners.hovered == &body);

    if (mode == kDeleteMode && body.hovered && input.IsMouseClickedDown(0))
    {
        body.isMarkedForDeletion = true;
    }
    if (mode == kSelectMode && body.hovered && input.IsMouseClickedDown(0))
    {
        bodySelecting = &body;
    }
    if (body.dragged)
    {
        // 检查拖拽模式，应用函数
        UpdateDragAngle(body, input, inputOwners, realDeltaTime);
        if (input.mouseButtonDown[2])
        {
            HandleDragWhole(body, input, realDeltaTime);
        }
        else if (!inputOwners.keyboardOwner && input.keyDown[L'F'])
        {
        }
        else if (body.invMass == 0)
        {
            HandleDragWhole(body, input, realDeltaTime);
        }
        else
        {
            HandleDragPoint(body, input, realDeltaTime);
        }
        body.dragged = input.mouseButtonDown[0] && (mode == kDragMode);
    }
    else
    {
        // 检查 draged是否判断正确，更新被拖拽角度
        body.dragged = body.hovered && input.IsMouseClickedDown(0) && (mode == kDragMode);
        if (body.dragged)
        {
            // p移到m，那么mo = 变换*原距离 = 变换*原相对位置=po，故有。
            body.relativeDragPoint = body.transform.Inverse() * ((Vector2)input.mousePosition - body.position);
            UpdateDragAngle(body, input, inputOwners, realDeltaTime);
        }
    }

    if (&body == bodySelecting && isTrackShown && !isPaused)
    {
        // 绘制运动轨迹
        body.track.push_back(body.position);
    }
    else if (&body != bodySelecting || !isTrackShown)
    {
        //不符合条件时清空轨迹信息
        body.track.clear();
    }

    // 创造连接
    bool canCreate = body.hovered && mode == kCreateMode &&
                     (creator->mode == kCreateConnection || creator->mode == kCreateNail);
    if (canCreate)
    {
        if (input.IsMouseClickedDown(0) && !creator->active)
        {
            creator->body0 = &body;
            creator->p0Relative = body.transform.Inverse() * ((Vector2)input.mousePosition - body.position);
        }
        else if (!input.mouseButtonDown[0] && input.previousMouseButtonDown[0] && creator->active)
        {
            creator->body1 = &body;
            creator->p1Relative = body.transform.Inverse() * ((Vector2)input.mousePosition - body.position);
        }
    }
}
