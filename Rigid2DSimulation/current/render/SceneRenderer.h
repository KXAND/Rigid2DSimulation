#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "DVector2.h"
#include "Tile.h"
#include "Vector2.h"

class Body;
class Collision;
class Connection;
struct CreationPreview;
class Font;
class SceneCanvas;
class PhysicsWorld;

int constexpr kColorDisplay = 0;
int constexpr kEnergyDisplay = 1;
int constexpr kChargesDisplay = 2;
int constexpr kNoDisplay = 3;

// 每个绘制阶段临时组装，不保存 App 或跨帧缓存选择指针。
struct SceneRenderContext
{
    Tile& screen;
    DepthBuffer& depthBuffer;
    Font const& font;
    DRect viewport;
    DVector2 mousePosition;
    Body const* selectedBody;
    Connection const* selectedConnection;
    Body const* hoveredBody;
    Body const* draggedBody;
    Vector2 relativeDragPoint;
    std::unordered_map<Body const*, std::vector<Vector2>> const& tracks;
};

class SceneRenderer
{
public:
    int display = 0;
    double energyMultiplier = 0;
    double chargeMultiplier = 0;

    // 调用方先同步世界几何；绘制不修改物体或连接。
    void Render(SceneCanvas const& canvas, PhysicsWorld const& world,
                CreationPreview const& preview, SceneRenderContext const& context) const;
    void RenderCollision(Collision const& collision, SceneRenderContext const& context) const;

private:
    void RenderBody(Body const& body, SceneRenderContext const& context) const;
    void RenderConnection(Connection const& connection, Vector2 p0, Vector2 p1,
                          SceneRenderContext const& context) const;
};
