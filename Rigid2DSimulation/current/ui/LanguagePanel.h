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
	std::shared_ptr<ChineseLanguageButton> chineseButton;
	std::shared_ptr<EnglishLanguageButton> englishButton;
	std::shared_ptr<VerticalControlList> controls;
	LanguagePanel(App& app);
};

