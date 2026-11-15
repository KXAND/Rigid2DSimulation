#include "BodyDrag.h"

#include <cmath>

#include "Body.h"

namespace
{
    void ApplyPointDrag(Body& body, BodyDrag const& drag, double realDeltaTime)
    {
        // 下列命名指定o,p,m三点为body中心、被拖拽点及拖拽目标点
        Vector2 targetPos = drag.targetPosition;
        Vector2 pPos = body.position + body.transform * drag.relativePoint;
        Vector2 targetVelocity = drag.targetDelta / realDeltaTime;
        Vector2 pmDir = (targetPos - pPos).GetNormalized();

        Vector2 opDir = pPos - body.position;
        opDir = Vector2(-opDir.y, opDir.x);
        Vector2 pVelocity = body.velocity + body.velocityAngular * opDir;
        double rd0 = Dot(opDir, pmDir);
        double effectiveInvMass = body.invMass + rd0 * rd0 * body.invInertia;

        Vector2 impulse = Dot(targetVelocity - pVelocity, pmDir) * pmDir / effectiveInvMass;

        body.velocity += impulse * body.invMass;
        body.velocityAngular += Dot(impulse, opDir) * body.invInertia;
        body.position += targetPos - pPos;
    }

    // 所有点同步被拖拽位移。
    void ApplyWholeDrag(Body& body, BodyDrag const& drag, double realDeltaTime)
    {
        Vector2 targetPos = drag.targetPosition;
        Vector2 dragPointPos = body.position + body.transform * drag.relativePoint;
        Vector2 newVelocity = drag.targetDelta / realDeltaTime;

        body.velocity = newVelocity;
        body.radian = drag.radian;
        body.velocityAngular = drag.velocityAngular;
        body.position += targetPos - dragPointPos; // 中心平行位移
    }

    void ApplyForceDrag(Body& body, BodyDrag const& drag, double deltaTime)
    {
        // 运动路径方向
        Vector2 p1 = drag.targetPosition;
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

    void ApplyDrag(Body& body, BodyDrag const& drag, double realDeltaTime, double stepDeltaTime)
    {
        if (drag.body != &body)
        {
            return;
        }
        // 不伪造时间，也不在无效时长下修改物体；输入释放仍由 DragTool 处理。
        if (!std::isfinite(realDeltaTime) || realDeltaTime <= 0 ||
            !std::isfinite(stepDeltaTime) || stepDeltaTime <= 0)
        {
            return;
        }

        if (drag.method == DragMethod::Whole)
        {
            ApplyWholeDrag(body, drag, realDeltaTime);
        }
        else if (drag.method == DragMethod::Force)
        {
            ApplyForceDrag(body, drag, stepDeltaTime);
        }
        else if (body.invMass == 0)
        {
            // 按计算时的物体状态选择回退。
            ApplyWholeDrag(body, drag, realDeltaTime);
        }
        else
        {
            ApplyPointDrag(body, drag, realDeltaTime);
        }
    }
}

void ApplyBodyDragFrame(Body& body, BodyDrag const& drag, double realDeltaTime)
{
    if (drag.method != DragMethod::Force)
    {
        ApplyDrag(body, drag, realDeltaTime, realDeltaTime);
    }
}

void ApplyBodyDragSubstep(Body& body, BodyDrag const& drag, double realDeltaTime, double stepDeltaTime)
{
    ApplyDrag(body, drag, realDeltaTime, stepDeltaTime);
}
