#pragma once
#include "Button.h"
#include "ControlList.h"
#include "FloatPanel.h"

struct ChineseLanguageButton : Button {
	ChineseLanguageButton();
	void OnClick(App& app) override;
};
struct EnglishLanguageButton : Button {
	EnglishLanguageButton();
	void OnClick(App& app) override;
};

struct LanguagePanel : LocalizedFloatPanel {
	std::shared_ptr<ChineseLanguageButton> bt_chi;
	std::shared_ptr<EnglishLanguageButton> bt_eng;
	std::shared_ptr<VerticalControlList> cly;
	LanguagePanel(App& app);
};

