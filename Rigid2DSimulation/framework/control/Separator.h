#pragma once
#include "Tile.h"
#include "Control.h"

struct HorizontalSeparator : Control
{
    int width = 0;
    DVector2 gap;
    Color markerColor;

    HorizontalSeparator(int width = 0);
    int GetWidth() const override;
    int GetHeight() const override;

    void Render(App& app);
    void Update(App& app) override;
};
