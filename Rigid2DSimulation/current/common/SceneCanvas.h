#pragma once
#include "DVector2.h"
#include "Tile.h"

struct AppInput;
struct InputOwners;

struct SceneCanvas
{
    DVector2 topLeft;
    double depth = 0;
    Tile backgroundTile;
    int width = 0, height = 0;

    SceneCanvas();
    DRect GetViewport() const
    {
        return {topLeft, width, height};
    }

    void Render(Tile& targetTile) const;
    void PreUpdate(AppInput const& input, InputOwners& inputOwners);

    // 必须在所有对象完成 PreUpdate 后查询，避免将物体或 UI 点击视为空白点击。
    bool IsEmptyAreaClicked(AppInput& input, InputOwners const& inputOwners) const;
};
