#pragma once
#include "Panel.h"
#include "Button.h"
#include "ControlList.h"

struct PauseButton : Button {
	PauseButton();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct ResetButton : LocalizedButton {
	ResetButton();
	void OnClick(App& app) override;
};
struct MuteButton : Button {
	MuteButton();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct ModeButton : Button {
	ModeButton();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct DisplayButton : Button {
	DisplayButton();
	void Update(App& app) override;
	void OnClick(App& app) override;
};

struct MenuPanel : Panel {
	std::shared_ptr<PauseButton> bt_pause;
	std::shared_ptr<ResetButton> bt_reset;
	std::shared_ptr<MuteButton> bt_mute;
	std::shared_ptr<ShowFloatPanelButton> bt_global;
	std::shared_ptr<ShowFloatPanelButton> bt_body;
	std::shared_ptr<ShowFloatPanelButton> bt_con;
	std::shared_ptr<ModeButton> bt_mode;
	std::shared_ptr<DisplayButton> bt_display;
	std::shared_ptr<ShowFloatPanelButton> bt_cmd;
	std::shared_ptr<ShowFloatPanelButton> bt_par;
	std::shared_ptr<ShowFloatPanelButton> bt_lang;
	std::shared_ptr<ShowFloatPanelButton> bt_about;
	std::shared_ptr<VerticalControlList> cly;
	MenuPanel(App& app);
};

