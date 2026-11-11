#pragma once

#include <memory>
#include <string>

#include "ControlList.h"
#include "Panel.h"

class MenuPanel : public Panel
{
public:
    MenuPanel(App& app);

private:
    std::unique_ptr<VerticalControlList> layout;
    Button* modeButton = nullptr;
    Button* displayButton = nullptr;

    void BuildSimulationButtons();
    void BuildModeButtons();
    void BuildScenePanelButtons(App& app);
    void BuildUtilityPanelButtons(App& app);
    static std::wstring GetInteractionModeText(App& app);
    static std::wstring GetDisplayModeText(App& app);
};
