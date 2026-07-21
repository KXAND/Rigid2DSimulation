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

ResetButton::ResetButton() : LocalizedButton(150, L"bt_reset") {}
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
	bt_pause = std::make_shared<PauseButton>(); bt_reset = std::make_shared<ResetButton>(); bt_mute = std::make_shared<MuteButton>();
	bt_global = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->globalPanel);
	bt_body = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->bodyPanel);
	bt_con = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->connectionPanel);
	bt_mode = std::make_shared<ModeButton>(); bt_display = std::make_shared<DisplayButton>();
	bt_cmd = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->commandPanel);
	bt_par = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->parameterPanel);
	bt_lang = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->languagePanel);
	bt_about = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->aboutPanel);
	tmp = { &*bt_pause, &*bt_reset, &*bt_mute, &*bt_global, 
		&*bt_body, &*bt_con, &*bt_mode, &*bt_display,
		&*bt_cmd, &*bt_par, &*bt_lang, &*bt_about };
	cly = std::make_shared<VerticalControlList>(tmp);
	c = &*cly;
	Init(app);
}



