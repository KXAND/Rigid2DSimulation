#include "SceneRenderer.h"

#include "Body.h"
#include "Collision.h"
#include "Connection.h"
#include "CreationPreview.h"
#include "DrawCurve.h"
#include "DrawGeometry.h"
#include "DrawLineSegment.h"
#include "DrawString.h"
#include "DrawTriangle.h"
#include "PhysicsWorld.h"
#include "SceneCanvas.h"
#include "Shape.h"

void SceneRenderer::Render(SceneCanvas const& canvas, PhysicsWorld const& world,
                           CreationPreview const& preview, SceneRenderContext const& context) const
{
    canvas.Render(context.screen);

    for (auto const& body : world.GetBodies())
    {
        RenderBody(*body, context);
    }
    for (auto const& connection : world.GetConnections())
    {
        RenderConnection(*connection, connection->p0, connection->p1, context);
    }

    if (preview.body)
    {
        RenderBody(*preview.body, context);
    }
    if (preview.connection)
    {
        auto const& anchorBody = *preview.connection->GetBody0();
        Vector2 anchor = anchorBody.position + anchorBody.transform * preview.relativeAnchor;
        RenderConnection(*preview.connection, anchor, preview.targetPosition, context);
    }
}

// 保留原碰撞接触点调试绘制入口，正常帧流程不主动调用。
void SceneRenderer::RenderCollision(Collision const& collision, SceneRenderContext const& context) const
{
    DrawEllipse(context.screen, context.depthBuffer, 100, collision.contactPoint, 5, 5, context.viewport, Color::kYellow);
}

void SceneRenderer::RenderBody(Body const& body, SceneRenderContext const& context) const
{
    double depth = body.appearance.GetDepth(body.point);
    bool isSelectedConnection = context.selectedConnection &&
                                (context.selectedConnection->GetBody0() == &body || context.selectedConnection->GetBody1() == &body);
    Color normalColor;
    switch (display)
    {
    case kColorDisplay:
        normalColor = body.appearance.innerColor;
        break;
    case kEnergyDisplay:
    {
        double energy = body.velocity.GetLengthSquared() + body.velocityAngular * body.velocityAngular * body.inertia / body.area;
        normalColor = (Color)Vector3(energy * energyMultiplier, 0, 0);
        break;
    }
    case kChargesDisplay:
    {
        normalColor = body.chargeDensity > 0 ? (Color)Vector3(body.chargeDensity * chargeMultiplier, 0, 0) : (Color)Vector3(0, 0, -body.chargeDensity * chargeMultiplier);
        break;
    }
    default:
        break;
    }

    Color color = (context.draggedBody == &body) ? body.appearance.draggedColor
                : (context.selectedBody == &body) ? body.appearance.selectedColor
                : (context.hoveredBody == &body) ? body.appearance.hoveredColor
                : isSelectedConnection ? body.appearance.selectedConnectionColor
                : normalColor;

    if (body.point)
    {
        DrawEllipse(context.screen, context.depthBuffer, depth, body.position, body.appearance.pointRadius, body.appearance.pointRadius, context.viewport, color);
        DrawEllipseBorder(context.screen, context.depthBuffer, depth, body.position, body.appearance.pointRadius, body.appearance.pointRadius, context.viewport, body.appearance.borderColor, 20);
    }
    for (auto& shape : body.shapes)
    {
        if (shape->isCircle)
        {
            DrawEllipse(context.screen, context.depthBuffer, depth,
                        shape->position, shape->radius, shape->radius, context.viewport, color);
            DrawEllipseBorder(context.screen, context.depthBuffer, depth,
                              shape->position, shape->radius, shape->radius, context.viewport, body.appearance.borderColor, 30);

            // 显示一条半径以便于我们判断圆的旋转角度
            auto peakPoint = Vector2(cos(shape->radian), sin(shape->radian)) * shape->radius + shape->position;
            DrawLineSegment(context.screen, context.depthBuffer, shape->position, peakPoint, depth, context.viewport, body.appearance.borderColor);
            continue;
        }

        for (int i = (0), iEnd = int(shape->vertices.size()); i < iEnd; ++i)
        {
            int j = (i + 1) % shape->vertices.size();
            DrawTriangle(context.screen, context.depthBuffer, context.viewport, shape->position, shape->vertices[i], shape->vertices[j], depth, color);
        }
        for (int i = (0), iEnd = int(shape->vertices.size()); i < iEnd; ++i)
        {
            int j = (i + 1) % shape->vertices.size();
            DrawLineSegment(context.screen, context.depthBuffer, shape->vertices[i], shape->vertices[j], depth, context.viewport, body.appearance.borderColor);
        }
    }

    if (context.draggedBody == &body)
    {
        Vector2 p1 = Vector2(context.mousePosition);
        Vector2 p0 = body.position + body.transform * context.relativeDragPoint;
        DrawLineSegment(context.screen, context.depthBuffer, p0, p1, depth, context.viewport, Color(0, 255, 255));
    }

    auto track = context.tracks.find(&body);
    if (track != context.tracks.end() && !track->second.empty())
    {
        Vector2 p0 = track->second.front();
        for (auto p1 : track->second)
        {
            DrawLineSegment(context.screen, context.depthBuffer, p0, p1, 10, context.viewport, Color(255, 0, 255));
            p0 = p1;
        }
    }
}

void SceneRenderer::RenderConnection(Connection const& connection, Vector2 p0, Vector2 p1,
                                     SceneRenderContext const& context) const
{
    Color color;
    switch (connection.type)
    {
    case kRopeConnection:
        color = connection.isTight ? connection.appearance.colorRopeTight : connection.appearance.colorRope;
        break;
    case kLinkConnection:
        color = connection.appearance.colorLink;
        break;
    case kCordConnection:
        color = connection.isTight ? connection.appearance.colorCordTight : connection.appearance.colorCord;
        break;
    case kSpringConnection:
        color = connection.isTight ? connection.appearance.colorSpringStretch : connection.appearance.colorSpringCompress;
        break;
    }
    DrawLineSegment(context.screen, context.depthBuffer, p0, p1, 2, context.viewport, color);
}
