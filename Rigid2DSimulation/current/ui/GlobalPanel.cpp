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
	text += simulationApp.GetLocalizedString(L"vol") + FormatFixed2(simulationApp.volume) + L"\n";
	text += simulationApp.GetLocalizedString(L"n_step") + std::to_wstring(simulationApp.stepNum) + L"\n";
	text += simulationApp.GetLocalizedString(L"s_grid") + FormatFixed2(simulationApp.gridSize) + L"\n";
	text += simulationApp.GetLocalizedString(L"max_real_dt") + FormatFixed2(simulationApp.maxRealDeltaTime) + L"\n";
	text += simulationApp.GetLocalizedString(L"tl_scene") + ToWString(simulationApp.rectScene.topLeftPosition) + L"\n";
	text += simulationApp.GetLocalizedString(L"w_scene") + std::to_wstring(simulationApp.rectScene.width) + L"\n";
	text += simulationApp.GetLocalizedString(L"h_scene") + std::to_wstring(simulationApp.rectScene.height) + L"\n";
	text += simulationApp.GetLocalizedString(L"gravity") + ToWString(simulationApp.gravity) + L"\n";
	text += simulationApp.GetLocalizedString(L"electrostatic") + std::to_wstring(simulationApp.hasElectrostatic) + L"\n";
	text += simulationApp.GetLocalizedString(L"coulomb") + FormatFixed2(simulationApp.coulombConstant) + L"\n";

	wstring s;
	switch (simulationApp.creator->mode) {
	case kCreateBall: s = simulationApp.GetLocalizedString(L"crt_ball"); break;
	case kCreateBox: s = simulationApp.GetLocalizedString(L"crt_box"); break;
	case kCreateConnection: s = simulationApp.GetLocalizedString(L"crt_conn"); break;
	case kCreateNail: s = simulationApp.GetLocalizedString(L"crt_nail"); break;
	case kCreateParticle: s = simulationApp.GetLocalizedString(L"crt_particle"); break;
	case kCreatePlate: s = simulationApp.GetLocalizedString(L"crt_plate"); break;
	case kCreatePoint: s = simulationApp.GetLocalizedString(L"crt_point"); break;
	}
	text += simulationApp.GetLocalizedString(L"creator_mode") + s + L"\n";
}

GlobalPanel::GlobalPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"fpn_global") {
	vector<Control*> tmp;
	savePathTextBox = std::make_shared<TextBox>(270); saveButton = std::make_shared<SaveButton>();
	tmp = { &*savePathTextBox, &*saveButton };
	saveControls = std::make_shared<HorizontalControlList>(tmp);
	tmp = {};
	loadPathTextBox = std::make_shared<TextBox>(270); loadButton = std::make_shared<LoadButton>();
	tmp = { &*loadPathTextBox, &*loadButton };
	loadControls = std::make_shared<HorizontalControlList>(tmp);
	tmp = {};
	separator = std::make_shared<HorizontalSeparator>(400); detailsLabel = std::make_shared<GlobalDetailsLabel>();
	tmp = { &*saveControls, &*loadControls, &*separator, &*detailsLabel };
	controls = std::make_shared<VerticalControlList>(tmp);
	contentControl = &*controls;
	Init(app);
}



