#include "GlobalPanel.h"
#include "UserInterface.h"
#include "SimulationApp.h"
#include "Creator.h"


SaveButton::SaveButton() : LocalizedButton(80, L"saveButton") {}
void SaveButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.Save(simulationApp.userInterface->globalPanel->savePathTextBox->value); }
LoadButton::LoadButton() : LocalizedButton(80, L"loadButton") {}
void LoadButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.Load(simulationApp.userInterface->globalPanel->loadPathTextBox->value); }

void GlobalDetailsLabel::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	text = L"";
	text += simulationApp.GetLocalizedString(L"volume") + FormatFixed2(simulationApp.volume) + L"\n";
	text += simulationApp.GetLocalizedString(L"stepCount") + std::to_wstring(simulationApp.stepNum) + L"\n";
	text += simulationApp.GetLocalizedString(L"gridSize") + FormatFixed2(simulationApp.gridSize) + L"\n";
	text += simulationApp.GetLocalizedString(L"maxRealDeltaTime") + FormatFixed2(simulationApp.maxRealDeltaTime) + L"\n";
	text += simulationApp.GetLocalizedString(L"sceneTopLeft") + ToWString(simulationApp.rectScene.topLeftPosition) + L"\n";
	text += simulationApp.GetLocalizedString(L"sceneWidth") + std::to_wstring(simulationApp.rectScene.width) + L"\n";
	text += simulationApp.GetLocalizedString(L"sceneHeight") + std::to_wstring(simulationApp.rectScene.height) + L"\n";
	text += simulationApp.GetLocalizedString(L"gravity") + ToWString(simulationApp.gravity) + L"\n";
	text += simulationApp.GetLocalizedString(L"electrostatic") + std::to_wstring(simulationApp.hasElectrostatic) + L"\n";
	text += simulationApp.GetLocalizedString(L"coulomb") + FormatFixed2(simulationApp.coulombConstant) + L"\n";

	wstring modeName;
	switch (simulationApp.creator->mode) {
	case kCreateBall: modeName = simulationApp.GetLocalizedString(L"createBall"); break;
	case kCreateBox: modeName = simulationApp.GetLocalizedString(L"createBox"); break;
	case kCreateConnection: modeName = simulationApp.GetLocalizedString(L"createConnection"); break;
	case kCreateNail: modeName = simulationApp.GetLocalizedString(L"createNail"); break;
	case kCreateParticle: modeName = simulationApp.GetLocalizedString(L"createParticle"); break;
	case kCreatePlate: modeName = simulationApp.GetLocalizedString(L"createPlate"); break;
	case kCreatePoint: modeName = simulationApp.GetLocalizedString(L"createPoint"); break;
	}
	text += simulationApp.GetLocalizedString(L"creatorMode") + modeName + L"\n";
}

GlobalPanel::GlobalPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"globalPanelTitle") {
	vector<Control*> controls;
	savePathTextBox = std::make_shared<TextBox>(270); saveButton = std::make_shared<SaveButton>();
	controls = { &*savePathTextBox, &*saveButton };
	saveControls = std::make_shared<HorizontalControlList>(controls);
	controls = {};
	loadPathTextBox = std::make_shared<TextBox>(270); loadButton = std::make_shared<LoadButton>();
	controls = { &*loadPathTextBox, &*loadButton };
	loadControls = std::make_shared<HorizontalControlList>(controls);
	controls = {};
	separator = std::make_shared<HorizontalSeparator>(400); detailsLabel = std::make_shared<GlobalDetailsLabel>();
	controls = { &*saveControls, &*loadControls, &*separator, &*detailsLabel };
	this->controls = std::make_shared<VerticalControlList>(controls);
	contentControl = &*this->controls;
	Init(app);
}



