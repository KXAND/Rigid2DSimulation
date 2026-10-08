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

BodyDrag DragTool::UpdateBody(Body const& body, AppInput& input, InputOwners const& inputOwners,
                              double realDeltaTime, bool isHovered, bool enabled)
{
    if (drag.body == &body)
    {
        ReadDragInput(body, input, inputOwners, realDeltaTime);
        // 释放帧仍需应用一次原有作用，但后续子步不再拖拽。
        BodyDrag frameDrag = drag;
        if (!input.mouseButtonDown[0] || !enabled)
        {
            Cancel();
        }
        return frameDrag;
    }
    if (!drag.body && isHovered && input.IsMouseClickedDown(0) && enabled)
    {
        drag.body = &body;
        drag.relativePoint = body.transform.Inverse() * ((Vector2)input.mousePosition - body.position);
        ReadDragInput(body, input, inputOwners, realDeltaTime);
    }
    return {};
}

void DragTool::ReadDragInput(Body const& body, AppInput const& input, InputOwners const& inputOwners, double realDeltaTime)
{
    drag.targetPosition = Vector2(input.mousePosition);
    drag.targetDelta = Vector2(input.mousePosition - input.previousMousePosition);
    if (input.mouseButtonDown[2])
    {
        drag.method = DragMethod::Whole;
    }
    else if (!inputOwners.keyboardOwner && input.keyDown[L'F'])
    {
        drag.method = DragMethod::Force;
    }
    else
    {
        drag.method = DragMethod::Point;
    }

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
