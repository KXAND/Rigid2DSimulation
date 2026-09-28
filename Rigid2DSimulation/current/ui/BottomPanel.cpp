#include "BottomPanel.h"
#include "SimulationApp.h"

FrameRateLabel::FrameRateLabel()
{
    width = 450;
    fixedWidth = true;
}
void FrameRateLabel::Sync(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    text = simulationApp.GetLocalizedString(L"frameTimeLabel") + FormatFixed2(simulationApp.frameRate.averageFrameTime * 1000) +
           L", FPS: " + FormatFixed2(simulationApp.frameRate.framesPerSecond);
}

TimeLabel::TimeLabel()
{
    width = 200;
    fixedWidth = true;
}
void TimeLabel::Sync(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    text = simulationApp.GetLocalizedString(L"timeLabel") + FormatFixed2(simulationApp.simulationTime);
}
MousePositionLabel::MousePositionLabel()
{
    width = 300;
    fixedWidth = true;
}
void MousePositionLabel::Sync(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    text = simulationApp.GetLocalizedString(L"mousePositionLabel") + ToWString(simulationApp.input.mousePosition);
}

BottomPanel::BottomPanel(App& app) : Panel(Panel::CreateBottomPanel(app))
{
    vector<Control*> controls;
    frameRateLabel = std::make_shared<FrameRateLabel>();
    timeLabel = std::make_shared<TimeLabel>();
    mousePositionLabel = std::make_shared<MousePositionLabel>();
    controls = {&*frameRateLabel, &*timeLabel, &*mousePositionLabel};
    this->controls = std::make_shared<HorizontalControlList>(controls);
    contentControl = &*this->controls;
    Init(app);
}
