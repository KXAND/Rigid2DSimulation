#include "DragTool.h"

#include "AppItem.h"
#include "Body.h"

void DragTool::Cancel()
{
    drag = {};
}

void DragTool::PrepareForDeletion()
{
    if (drag.body && drag.body->IsMarkedForDeletion())
    {
        Cancel();
    }
}

void DragTool::UpdateBody(Body& body, AppInput& input, InputOwners const& inputOwners,
                          double realDeltaTime, bool isHovered, bool enabled)
{
    if (drag.body == &body)
    {
        UpdateDragAngle(body, input, inputOwners, realDeltaTime);
        if (input.mouseButtonDown[2])
        {
            HandleDragWhole(body, input, realDeltaTime);
        }
        else if (!inputOwners.keyboardOwner && input.keyDown[L'F'])
        {
            // 力拖拽只在物理子步中应用。
        }
        else if (body.invMass == 0)
        {
            HandleDragWhole(body, input, realDeltaTime);
        }
        else
        {
            HandleDragPoint(body, input, realDeltaTime);
        }
        // 保留释放帧先应用拖拽、再结束拖拽的顺序。
        if (!input.mouseButtonDown[0] || !enabled)
        {
            Cancel();
        }
    }
    else if (!drag.body && isHovered && input.IsMouseClickedDown(0) && enabled)
    {
        drag.body = &body;
        drag.relativePoint = body.transform.Inverse() * ((Vector2)input.mousePosition - body.position);
        UpdateDragAngle(body, input, inputOwners, realDeltaTime);
    }
}

void DragTool::ApplySubstep(Body& body, AppInput const& input, InputOwners const& inputOwners,
                            double realDeltaTime, double stepDeltaTime)
{
    if (drag.body == &body)
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

void DragTool::UpdateDragAngle(Body& body, AppInput const& input, InputOwners const& inputOwners, double realDeltaTime)
{
    drag.velocityAngular = 0;
    if (!inputOwners.keyboardOwner)
    {
        if (input.keyDown[L'Q'])
        {
            drag.velocityAngular = -rotationSpeed;
        }
        if (input.keyDown[L'E'])
        {
            drag.velocityAngular = +rotationSpeed;
        }
    }
    drag.radian = body.radian + drag.velocityAngular * realDeltaTime;
}

void DragTool::HandleDragPoint(Body& body, AppInput const& input, double realDeltaTime)
{
    // 下列命名指定o,p,m三点为body中心、被拖拽点及鼠标现在点
    Vector2 mousePos = Vector2(input.mousePosition);
    Vector2 pPos = body.position + body.transform * drag.relativePoint;
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
void DragTool::HandleDragWhole(Body& body, AppInput const& input, double realDeltaTime)
{
    Vector2 mousePos = Vector2(input.mousePosition);
    Vector2 dragPointPos = body.position + body.transform * drag.relativePoint;
    Vector2 newVelocity = Vector2(input.mousePosition - input.previousMousePosition) / realDeltaTime;

    body.velocity = newVelocity;
    body.radian = drag.radian;
    body.velocityAngular = drag.velocityAngular;
    body.position += mousePos - dragPointPos; // 中心平行位移
}

void DragTool::HandleDragForce(Body& body, AppInput const& input, double deltaTime)
{
    // 运动路径方向
    Vector2 p1 = Vector2(input.mousePosition);
    Vector2 p0 = body.position + body.transform * drag.relativePoint;
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
