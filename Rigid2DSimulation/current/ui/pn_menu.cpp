#include "pn_menu.h"
#include "ui.h"
#include "simulation_app.h"
#include "fpn_global.h"
#include "fpn_body.h"
#include "fpn_con.h"
#include "fpn_cmd.h"
#include "fpn_par.h"
#include "fpn_lang.h"
#include "fpn_about.h"
#include "creator.h"

#define mkbt(nm) mkp(bt_##nm)(&*simulationApp.ui->fpn_##nm);

BtPause::BtPause() : Button(150) {}
void BtPause::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	txt = simulationApp.isPaused ? simulationApp.getLocalizedString(L"start") : simulationApp.getLocalizedString(L"pause");
	Button::Update(app);
}
void BtPause::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.isPaused = !simulationApp.isPaused; }

BtReset::BtReset() : BtLan(150, L"bt_reset") {}
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
	mkp(bt_pause)(); mkp(bt_reset)(); mkp(bt_mute)();
	mkbt(global); mkbt(body); mkbt(con);
	mkp(bt_mode)(); mkp(bt_display)();
	mkbt(cmd); mkbt(par);
	mkbt(lang); mkbt(about);
	tmp = { &*bt_pause, &*bt_reset, &*bt_mute, &*bt_global, 
		&*bt_body, &*bt_con, &*bt_mode, &*bt_display,
		&*bt_cmd, &*bt_par, &*bt_lang, &*bt_about };
	mkp(cly)(tmp);
	c = &*cly;
	Init(app);
}



