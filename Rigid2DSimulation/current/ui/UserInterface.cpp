#include "UserInterface.h"
#include "SimulationApp.h"
#include "FloatPanel.h"
#include "current/ui/globalPanel/GlobalPanel.h"
#include "current/ui/creationPanel/CreationPanel.h"
#include "current/ui/bodyPanel/BodyPanel.h"
#include "current/ui/connectionPanel/ConnectionPanel.h"
#include "current/ui/languagePanel/LanguagePanel.h"
#include "current/ui/aboutPanel/AboutPanel.h"
#include "current/ui/menuPanel/MenuPanel.h"
#include "current/ui/bottomPanel/BottomPanel.h"

void UserInterface::SortFloatPanels()
{
    sort(floatPanels.begin(), floatPanels.end(), CompareFloatPanels);
}
void UserInterface::Init(SimulationApp& simulationApp)
{
    globalPanel = std::make_shared<GlobalPanel>(simulationApp);
    floatPanels.push_back(&*globalPanel);
    creationPanel = std::make_shared<CreationPanel>(simulationApp);
    floatPanels.push_back(creationPanel.get());
    bodyPanel = std::make_shared<BodyPanel>(simulationApp);
    floatPanels.push_back(&*bodyPanel);
    connectionPanel = std::make_shared<ConnectionPanel>(simulationApp);
    floatPanels.push_back(&*connectionPanel);
    languagePanel = std::make_shared<LanguagePanel>(simulationApp);
    floatPanels.push_back(&*languagePanel);
    aboutPanel = std::make_shared<AboutPanel>(simulationApp);
    floatPanels.push_back(&*aboutPanel);

    // MenuPanel 的 ShowFloatPanelButton 保存浮动面板指针，因此须在这些面板创建后构造菜单。
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
void UserInterface::Render(SimulationApp& simulationApp)
{
    menuPanel->Render(simulationApp);
    bottomPanel->Render(simulationApp);
    for (auto floatPanel : floatPanels)
    {
        floatPanel->Render(simulationApp);
    }
}
void UserInterface::PreUpdate(SimulationApp& simulationApp)
{
    // 在命中测试之前同步裁剪边界，不让业务面板缓存过期的屏幕尺寸。
    auto bounds = simulationApp.screen.Rect();
    menuPanel->SetDisplayBounds(bounds);
    bottomPanel->SetDisplayBounds(bounds);
    for (auto floatPanel : floatPanels)
    {
        floatPanel->SetDisplayBounds(bounds);
    }

    menuPanel->PreUpdate(simulationApp);
    bottomPanel->PreUpdate(simulationApp);
    SortFloatPanels();
    for (auto floatPanel : floatPanels)
    {
        floatPanel->PreUpdate(simulationApp);
    }
}
