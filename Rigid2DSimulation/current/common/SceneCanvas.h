#pragma once
#include "DVector2.h"
#include "Tile.h"

class AppInput;
struct InputOwners;

class SceneCanvas
{
public:
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
};
