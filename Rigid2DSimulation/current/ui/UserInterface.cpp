#include "UserInterface.h"
#include "SimulationApp.h"
#include "FloatPanel.h"
#include "GlobalPanel.h"
#include "BodyPanel.h"
#include "ConnectionPanel.h"
#include "ParameterPanel.h"
#include "CommandPanel.h"
#include "LanguagePanel.h"
#include "AboutPanel.h"
#include "MenuPanel.h"
#include "BottomPanel.h"

void UserInterface::SortFloatPanels()
{
    sort(floatPanels.begin(), floatPanels.end(), CompareFloatPanels);
}
void UserInterface::Init(SimulationApp& simulationApp)
{
    globalPanel = std::make_shared<GlobalPanel>(simulationApp);
    floatPanels.push_back(&*globalPanel);
    bodyPanel = std::make_shared<BodyPanel>(simulationApp);
    floatPanels.push_back(&*bodyPanel);
    connectionPanel = std::make_shared<ConnectionPanel>(simulationApp);
    floatPanels.push_back(&*connectionPanel);
    commandPanel = std::make_shared<CommandPanel>(simulationApp);
    floatPanels.push_back(&*commandPanel);
    parameterPanel = std::make_shared<ParameterPanel>(simulationApp);
    floatPanels.push_back(&*parameterPanel);
    languagePanel = std::make_shared<LanguagePanel>(simulationApp);
    floatPanels.push_back(&*languagePanel);
    aboutPanel = std::make_shared<AboutPanel>(simulationApp);
    floatPanels.push_back(&*aboutPanel);

    // menuPanel 放在后面是因为要等 float_panel 先加载完。
    // menuPanel 放在后面是因为要等 float_panel 先加载完。
    // 这主要是 BtFPn 的原因。
    menuPanel = std::make_shared<MenuPanel>(simulationApp);
    bottomPanel = std::make_shared<BottomPanel>(simulationApp);
}
void UserInterface::Update(SimulationApp& simulationApp)
{
    menuPanel->Update(simulationApp);
    bottomPanel->Update(simulationApp);
    for (auto floatPanel : floatPanels)
    {
        floatPanel->Update(simulationApp);
    }
}
void UserInterface::PreUpdate(SimulationApp& simulationApp)
{
    menuPanel->PreUpdate(simulationApp);
    bottomPanel->PreUpdate(simulationApp);
    SortFloatPanels();
    for (auto floatPanel : floatPanels)
    {
        floatPanel->PreUpdate(simulationApp);
    }
}
