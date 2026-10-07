#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "DVector2.h"
#include "Tile.h"
#include "Vector2.h"

struct Body;
struct Collision;
struct Connection;
struct Creator;
struct Font;
struct SceneCanvas;
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

    void RenderBackground(SceneCanvas const& canvas, std::wstring const& debugString,
                          SceneRenderContext const& context) const;
    // 保留原时点的几何更新；本入口不是对世界的纯只读操作。
    void RenderObjects(PhysicsWorld& world, SceneRenderContext const& context) const;
    void RenderCreatorPreview(Creator& creator, SceneRenderContext const& context) const;
    void RenderCollision(Collision const& collision, SceneRenderContext const& context) const;

private:
    void RenderBody(Body const& body, SceneRenderContext const& context) const;
    void RenderConnection(Connection const& connection, SceneRenderContext const& context) const;
};
