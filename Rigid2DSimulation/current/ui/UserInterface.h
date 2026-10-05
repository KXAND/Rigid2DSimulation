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

    // 先将实例赋给 SimulationApp::userInterface，再初始化需要通过它访问浮动面板的菜单。
    void Init(SimulationApp& simulationApp);
    // 控件仍在更新过程中按原顺序绘制，不表示全局更新/渲染已分阶段。
    void UpdateAndRender(SimulationApp& simulationApp);
    void PreUpdate(SimulationApp& simulationApp);
};
