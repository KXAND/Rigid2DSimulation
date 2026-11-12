#include "BottomPanel.h"

#include <memory>
#include <utility>

#include "SimulationApp.h"

BottomPanel::BottomPanel(App& app) : Panel(Panel::CreateBottomPanel(app.screen.Rect()))
{
    auto layout = std::make_unique<HorizontalStack>();
    frameRateLabel = &layout->AddLabel(450);
    timeLabel = &layout->AddLabel(200);
    mousePositionLabel = &layout->AddLabel(300);
    SetContent(std::move(layout));
}

void BottomPanel::Update(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    frameRateLabel->text = simulationApp.GetLocalizedString(L"frameTimeLabel") +
                          FormatFixed2(simulationApp.frameRate.averageFrameTime * 1000) +
                          L", FPS: " + FormatFixed2(simulationApp.frameRate.framesPerSecond);
    timeLabel->text = simulationApp.GetLocalizedString(L"timeLabel") +
                      FormatFixed2(simulationApp.physicsWorld.simulationTime);
    mousePositionLabel->text = simulationApp.GetLocalizedString(L"mousePositionLabel") +
                               ToWString(simulationApp.input.mousePosition);

    Panel::Update(app);
}
