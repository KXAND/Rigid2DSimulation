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


BtPause::BtPause() : Button(150) {}
void BtPause::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	txt = simulationApp.isPaused ? simulationApp.getLocalizedString(L"start") : simulationApp.getLocalizedString(L"pause");
	Button::Update(app);
}
void BtPause::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.isPaused = !simulationApp.isPaused; }

BtReset::BtReset() : LocalizedButton(150, L"bt_reset") {}
void BtReset::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.Reset(); }

BtMute::BtMute() : Button(150) {}
void BtMute::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	txt = simulationApp.isMuted ? simulationApp.getLocalizedString(L"unmute") : simulationApp.getLocalizedString(L"mute");
	Button::Update(app);
}
void BtMute::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.isMuted = !simulationApp.isMuted; }

BtMode::BtMode() : Button(150) {}
void BtMode::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	switch (simulationApp.mode) {
	case MODE_DRAG: txt = simulationApp.getLocalizedString(L"mode_drag"); break;
	case MODE_SEL: txt = simulationApp.getLocalizedString(L"mode_sel"); break;
	case MODE_CREATE: txt = simulationApp.getLocalizedString(L"mode_create"); break;
	case MODE_DEL: txt = simulationApp.getLocalizedString(L"mode_del"); break;
	}
	Button::Update(app);
}
void BtMode::OnClick(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	simulationApp.creator->Discard(simulationApp); 
	simulationApp.mode = (simulationApp.mode + 1) % 4;
}

BtDisplay::BtDisplay() : Button(150) {}
void BtDisplay::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	switch (simulationApp.display) {
	case DISPLAY_COLOR: txt = simulationApp.getLocalizedString(L"disp_col"); break;
	case DISPLAY_ENERGY: txt = simulationApp.getLocalizedString(L"disp_energy"); break;
	case DISPLAY_CHARGES: txt = simulationApp.getLocalizedString(L"disp_charge"); break;
	case DISPLAY_NONE: txt = simulationApp.getLocalizedString(L"disp_none"); break;
	}
	Button::Update(app);
}
void BtDisplay::OnClick(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	simulationApp.display = (simulationApp.display + 1) % 4;
}

PnMenu::PnMenu(App& app) : Panel(Panel::menu(app)) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	vector<Control*> tmp;
	bt_pause = std::make_shared<BtPause>(); bt_reset = std::make_shared<BtReset>(); bt_mute = std::make_shared<BtMute>();
	bt_global = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->fpn_global);
	bt_body = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->fpn_body);
	bt_con = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->fpn_con);
	bt_mode = std::make_shared<BtMode>(); bt_display = std::make_shared<BtDisplay>();
	bt_cmd = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->fpn_cmd);
	bt_par = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->fpn_par);
	bt_lang = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->fpn_lang);
	bt_about = std::make_shared<ShowFloatPanelButton>(&*simulationApp.userInterface->fpn_about);
	tmp = { &*bt_pause, &*bt_reset, &*bt_mute, &*bt_global, 
		&*bt_body, &*bt_con, &*bt_mode, &*bt_display,
		&*bt_cmd, &*bt_par, &*bt_lang, &*bt_about };
	cly = std::make_shared<VerticalControlList>(tmp);
	c = &*cly;
	Init(app);
}



