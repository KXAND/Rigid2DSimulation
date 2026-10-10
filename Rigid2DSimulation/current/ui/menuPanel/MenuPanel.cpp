#include "MenuPanel.h"

#include <memory>
#include <vector>

#include "SimulationApp.h"
#include "UserInterface.h"
#include "current/ui/aboutPanel/AboutPanel.h"
#include "current/ui/bodyPanel/BodyPanel.h"
#include "current/ui/commandPanel/CommandPanel.h"
#include "current/ui/connectionPanel/ConnectionPanel.h"
#include "current/ui/creationPanel/CreationPanel.h"
#include "current/ui/globalPanel/GlobalPanel.h"
#include "current/ui/languagePanel/LanguagePanel.h"
#include "current/ui/parameterPanel/ParameterPanel.h"

MenuPanel::MenuPanel(App& app) : Panel(Panel::CreateMenuPanel(app))
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    vector<Control*> controls;
    pauseButton = std::make_shared<SimulationPauseButton>();
    resetButton = std::make_shared<SceneResetButton>();
    muteButton = std::make_shared<AudioMuteButton>();
    globalPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->globalPanel);
    creationPanelButton = std::make_shared<ShowFloatPanelButton>(simulationApp.userInterface->creationPanel.get());
    bodyPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->bodyPanel);
    connectionPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->connectionPanel);
    modeButton = std::make_shared<InteractionModeButton>();
    displayButton = std::make_shared<DisplayModeButton>();
    commandPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->commandPanel);
    parameterPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->parameterPanel);
    languagePanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->languagePanel);
    aboutPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->aboutPanel);
    controls = {&*pauseButton, &*resetButton, &*muteButton, &*globalPanelButton, creationPanelButton.get(),
                &*bodyPanelButton, &*connectionPanelButton, &*modeButton, &*displayButton,
                &*commandPanelButton, &*parameterPanelButton, &*languagePanelButton, &*aboutPanelButton};
    this->controls = std::make_shared<VerticalControlList>(controls);
    contentControl = &*this->controls;
    Init(app);
}
