#pragma once
#include "panel.h"
#include "button.h"
#include "ctrl_list.h"

struct BtPause : Button {
	BtPause();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtReset : BtLan {
	BtReset();
	void OnClick(App& app) override;
};
struct BtMute : Button {
	BtMute();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtMode : Button {
	BtMode();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtDisplay : Button {
	BtDisplay();
	void Update(App& app) override;
	void OnClick(App& app) override;
};

struct PnMenu : Panel {
	std::shared_ptr<BtPause> bt_pause;
	std::shared_ptr<BtReset> bt_reset;
	std::shared_ptr<BtMute> bt_mute;
	std::shared_ptr<BtFPn> bt_global;
	std::shared_ptr<BtFPn> bt_body;
	std::shared_ptr<BtFPn> bt_con;
	std::shared_ptr<BtMode> bt_mode;
	std::shared_ptr<BtDisplay> bt_display;
	std::shared_ptr<BtFPn> bt_cmd;
	std::shared_ptr<BtFPn> bt_par;
	std::shared_ptr<BtFPn> bt_lang;
	std::shared_ptr<BtFPn> bt_about;
	std::shared_ptr<CtrlListY> cly;
	PnMenu(App& app);
};

