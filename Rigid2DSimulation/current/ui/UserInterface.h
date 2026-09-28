#pragma once
#include "Utils.h"

struct SimulationApp;
struct FloatPanel;
struct GlobalPanel;
struct BodyPanel;
struct ConnectionPanel;
struct CommandPanel;
struct ParameterPanel;
struct LanguagePanel;
struct AboutPanel;
struct MenuPanel;
struct BottomPanel;
struct UserInterface
{
    vector<FloatPanel*> floatPanels;
    std::shared_ptr<GlobalPanel> globalPanel;
    std::shared_ptr<BodyPanel> bodyPanel;
    std::shared_ptr<ConnectionPanel> connectionPanel;
    std::shared_ptr<CommandPanel> commandPanel;
    std::shared_ptr<ParameterPanel> parameterPanel;
    std::shared_ptr<LanguagePanel> languagePanel;
    std::shared_ptr<AboutPanel> aboutPanel;
    std::shared_ptr<MenuPanel> menuPanel;
    std::shared_ptr<BottomPanel> bottomPanel;

    UserInterface(SimulationApp& simulationApp) {};
    void SortFloatPanels();

    // 这里用 init 的原因是控件会用 simulationApp->ui 来访问 ui, 但这个时候 ui 尚未初始化。
    // 这也是为什么主程序不需要 Init。这应该是一个普遍的问题。
    // 好像上面这些话已经过时了，不过还是保留这个设计以防万一。
    void Init(SimulationApp& simulationApp);
    void Update(SimulationApp& simulationApp);
    void PreUpdate(SimulationApp& simulationApp);
};
