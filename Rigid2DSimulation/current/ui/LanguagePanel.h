#pragma once
#include "Button.h"
#include "ControlList.h"
#include "FloatPanel.h"

struct BtChi : Button {
	BtChi();
	void OnClick(App& app) override;
};
struct BtEng : Button {
	BtEng();
	void OnClick(App& app) override;
};

struct LanguagePanel : LocalizedFloatPanel {
	std::shared_ptr<BtChi> bt_chi;
	std::shared_ptr<BtEng> bt_eng;
	std::shared_ptr<VerticalControlList> cly;
	LanguagePanel(App& app);
};

