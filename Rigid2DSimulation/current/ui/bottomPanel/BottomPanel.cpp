#include "BottomPanel.h"

#include <memory>
#include <vector>

#include "SimulationApp.h"

BottomPanel::BottomPanel(App& app) : Panel(Panel::CreateBottomPanel(app))
{
    frameRateLabel = std::make_shared<Label>();
    frameRateLabel->width = 450;
    frameRateLabel->fixedWidth = true;

    timeLabel = std::make_shared<Label>();
    timeLabel->width = 200;
    timeLabel->fixedWidth = true;

    mousePositionLabel = std::make_shared<Label>();
    mousePositionLabel->width = 300;
    mousePositionLabel->fixedWidth = true;

    layout = std::make_shared<HorizontalControlList>(
        std::vector<Control*>{frameRateLabel.get(), timeLabel.get(), mousePositionLabel.get()});
    contentControl = layout.get();
    Init(app);
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
