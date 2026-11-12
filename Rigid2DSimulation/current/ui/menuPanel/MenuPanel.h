#pragma once

#include <string>

#include "ControlContainer.h"
#include "Panel.h"

class MenuPanel : public Panel
{
public:
    MenuPanel(App& app);

private:
    Button* modeButton = nullptr;
    Button* displayButton = nullptr;

    void BuildSimulationButtons(VerticalStack& layout);
    void BuildModeButtons(VerticalStack& layout);
    void BuildScenePanelButtons(VerticalStack& layout, App& app);
    void BuildUtilityPanelButtons(VerticalStack& layout, App& app);
    static std::wstring GetInteractionModeText(App& app);
    static std::wstring GetDisplayModeText(App& app);
};
