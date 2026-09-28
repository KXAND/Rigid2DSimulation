#include "Background.h"
#include "SimulationApp.h"
#include "DrawString.h"
#include "DrawTile.h"

Background::Background(SimulationApp& simulationApp)
{
    width = 1630;
    height = 860;
    topLeft = {170, 0}; // tile的原点坐标位置，170的余量用于左侧UI显示
    depth = -100000;
    backgroundTile = Tile(width, height, Color::kBlack, 255);
}

void Background::Render(SimulationApp& simulationApp)
{
    DrawTileRaw((simulationApp.screen), topLeft, backgroundTile);
    DrawString((simulationApp.screen), (simulationApp.depthBuffer), 999, (simulationApp.debugString),
               Color(110), (simulationApp.font), topLeft + DVector2(10, 10), width - 20, simulationApp.background->GetViewport());
}

void Background::Update(SimulationApp& simulationApp)
{
    hovered = ((simulationApp.inputOwners.hovered) == this);
    wheeled = ((simulationApp.inputOwners.wheeled) == this);

    if (hovered && (simulationApp.input.IsMouseClickedDown)(0))
    {
        simulationApp.bodySelecting = NULL;
        simulationApp.connectionSelecting = NULL;
    }
}
void Background::PreUpdate(SimulationApp& simulationApp)
{
    bool haveDeeperValue = ((simulationApp.inputOwners).hoveredDepth) <= depth && IsInside(simulationApp.input.mousePosition, GetViewport());
    if (haveDeeperValue)
    {
        ((simulationApp.inputOwners).hoveredDepth) = depth;
        ((simulationApp.inputOwners).hovered) = this;
    }

    haveDeeperValue = (simulationApp.inputOwners.wheeledDepth) <= depth && IsInside(simulationApp.input.mousePosition, GetViewport());
    if (haveDeeperValue)
    {
        (simulationApp.inputOwners.wheeledDepth) = depth;
        (simulationApp.inputOwners.wheeled) = this;
    }
}
