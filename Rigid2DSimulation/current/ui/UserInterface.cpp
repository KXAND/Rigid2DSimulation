#include "UserInterface.h"
#include "SimulationApp.h"
#include "FloatPanel.h"
#include "GlobalPanel.h"
#include "BodyPanel.h"
#include "ConnectionPanel.h"
#include "ParameterPanel.h"
#include "CommandPanel.h"
#include "LanguagePanel.h"
#include "AboutPanel.h"
#include "MenuPanel.h"
#include "BottomPanel.h"


void UserInterface::SortFloatPanels() { sort(fpns.begin(), fpns.end(), less_fpn); }
void UserInterface::Init(SimulationApp& cur) {
	globalPanel = std::make_shared<GlobalPanel>(cur); fpns.push_back(&*globalPanel);
	bodyPanel = std::make_shared<BodyPanel>(cur); fpns.push_back(&*bodyPanel);
	connectionPanel = std::make_shared<ConnectionPanel>(cur); fpns.push_back(&*connectionPanel);
	commandPanel = std::make_shared<CommandPanel>(cur); fpns.push_back(&*commandPanel);
	parameterPanel = std::make_shared<ParameterPanel>(cur); fpns.push_back(&*parameterPanel);
	languagePanel = std::make_shared<LanguagePanel>(cur); fpns.push_back(&*languagePanel);
	aboutPanel = std::make_shared<AboutPanel>(cur); fpns.push_back(&*aboutPanel);

	// menuPanel 放在后面是因为要等 float_panel 先加载完。
	// menuPanel 放在后面是因为要等 float_panel 先加载完。
	// 这主要是 BtFPn 的原因。
	menuPanel = std::make_shared<MenuPanel>(cur);
	bottomPanel = std::make_shared<BottomPanel>(cur);
}
void UserInterface::Update(SimulationApp& cur) {
	menuPanel->Update(cur);
	bottomPanel->Update(cur);
	for (auto fp : fpns) { fp->Update(cur); }
}
void UserInterface::PreUpdate(SimulationApp& cur) {
	menuPanel->PreUpdate(cur);
	bottomPanel->PreUpdate(cur);
	SortFloatPanels();
	for (auto fp : fpns) { fp->PreUpdate(cur); }
}



