#include "MenuPanel.h"
#include "UserInterface.h"
#include "SimulationApp.h"
#include "GlobalPanel.h"
#include "BodyPanel.h"
#include "ConnectionPanel.h"
#include "CommandPanel.h"
#include "ParameterPanel.h"
#include "LanguagePanel.h"
#include "AboutPanel.h"
#include "Creator.h"


PauseButton::PauseButton() : Button(150) {}
void PauseButton::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	txt = simulationApp.isPaused ? simulationApp.getLocalizedString(L"start") : simulationApp.getLocalizedString(L"pause");
	Button::Update(app);
}
void PauseButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.isPaused = !simulationApp.isPaused; }

ResetButton::ResetButton() : LocalizedButton(150, L"resetButton") {}
void ResetButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.Reset(); }

MuteButton::MuteButton() : Button(150) {}
void MuteButton::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	txt = simulationApp.isMuted ? simulationApp.getLocalizedString(L"unmute") : simulationApp.getLocalizedString(L"mute");
	Button::Update(app);
}
void MuteButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.isMuted = !simulationApp.isMuted; }

ModeButton::ModeButton() : Button(150) {}
void ModeButton::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	switch (simulationApp.mode) {
	case MODE_DRAG: txt = simulationApp.getLocalizedString(L"mode_drag"); break;
	case MODE_SEL: txt = simulationApp.getLocalizedString(L"mode_sel"); break;
	case MODE_CREATE: txt = simulationApp.getLocalizedString(L"mode_create"); break;
	case MODE_DEL: txt = simulationApp.getLocalizedString(L"mode_del"); break;
	}
	Button::Update(app);
}
void ModeButton::OnClick(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	simulationApp.creator->Discard(simulationApp);
	simulationApp.mode = (simulationApp.mode + 1) % 4;
}

DisplayButton::DisplayButton() : Button(150) {}
void DisplayButton::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	switch (simulationApp.display) {
	case DISPLAY_COLOR: txt = simulationApp.getLocalizedString(L"disp_col"); break;
	case DISPLAY_ENERGY: txt = simulationApp.getLocalizedString(L"disp_energy"); break;
	case DISPLAY_CHARGES: txt = simulationApp.getLocalizedString(L"disp_charge"); break;
	case DISPLAY_NONE: txt = simulationApp.getLocalizedString(L"disp_none"); break;
	}
	Button::Update(app);
}
void DisplayButton::OnClick(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	simulationApp.display = (simulationApp.display + 1) % 4;
}

MenuPanel::MenuPanel(App& app) : Panel(Panel::menu(app)) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	vector<Control*> tmp;
	pauseButton = std::make_shared<PauseButton>(); resetButton = std::make_shared<ResetButton>(); muteButton = std::make_shared<MuteButton>();
	globalPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->globalPanel);
	bodyPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->bodyPanel);
	connectionPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->connectionPanel);
	modeButton = std::make_shared<ModeButton>(); displayButton = std::make_shared<DisplayButton>();
	commandPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->commandPanel);
	parameterPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->parameterPanel);
	languagePanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->languagePanel);
	aboutPanelButton = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->aboutPanel);
	tmp = { &*pauseButton, &*resetButton, &*muteButton, &*globalPanelButton,
		&*bodyPanelButton, &*connectionPanelButton, &*modeButton, &*displayButton,
		&*commandPanelButton, &*parameterPanelButton, &*languagePanelButton, &*aboutPanelButton };
	controls = std::make_shared<VerticalControlList>(tmp);
	c = &*controls;
	Init(app);
}



