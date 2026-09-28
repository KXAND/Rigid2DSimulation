#include "Separator.h"
#include "App.h"
#include "DrawGeometry.h"

HorizontalSeparator::HorizontalSeparator(int width) : width(width)
{
    gap = {10, 10};
    markerColor = {150, 150, 150};
}
int HorizontalSeparator::GetWidth() const
{
    return width + gap.x;
}
int HorizontalSeparator::GetHeight() const
{
    return 1 + gap.y;
}

void HorizontalSeparator::Render(App& app)
{
    // 虽然 dep 没什么用，但出于习惯还是保留了。
    DrawRectangleRaw(app.screen, topLeft, width, 1, viewport, markerColor);
}
void HorizontalSeparator::Update(App& app)
{
    Render(app);
}
