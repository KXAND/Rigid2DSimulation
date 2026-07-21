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
	text += simulationApp.getLocalizedString(L"vol") + FormatFixed2(simulationApp.vol) + L"\n";
	text += simulationApp.getLocalizedString(L"n_step") + std::to_wstring(simulationApp.stepNum) + L"\n";
	text += simulationApp.getLocalizedString(L"s_grid") + FormatFixed2(simulationApp.gridSize) + L"\n";
	text += simulationApp.getLocalizedString(L"max_real_dt") + FormatFixed2(simulationApp.maxRealDeltaTime) + L"\n";
	text += simulationApp.getLocalizedString(L"tl_scene") + ToWString(simulationApp.rectScene.topLeftPosition) + L"\n";
	text += simulationApp.getLocalizedString(L"w_scene") + std::to_wstring(simulationApp.rectScene.w) + L"\n";
	text += simulationApp.getLocalizedString(L"h_scene") + std::to_wstring(simulationApp.rectScene.h) + L"\n";
	text += simulationApp.getLocalizedString(L"gravity") + ToWString(simulationApp.gravity) + L"\n";
	text += simulationApp.getLocalizedString(L"electrostatic") + std::to_wstring(simulationApp.hasElectrostatic) + L"\n";
	text += simulationApp.getLocalizedString(L"coulomb") + FormatFixed2(simulationApp.coulomb) + L"\n";

	wstring s;
	switch (simulationApp.creator->mode) {
	case CREATE_BALL: s = simulationApp.getLocalizedString(L"crt_ball"); break;
	case CREATE_BOX: s = simulationApp.getLocalizedString(L"crt_box"); break;
	case CREATE_CONN: s = simulationApp.getLocalizedString(L"crt_conn"); break;
	case CREATE_NAIL: s = simulationApp.getLocalizedString(L"crt_nail"); break;
	case CREATE_PARTICLE: s = simulationApp.getLocalizedString(L"crt_particle"); break;
	case CREATE_PLATE: s = simulationApp.getLocalizedString(L"crt_plate"); break;
	case CREATE_POINT: s = simulationApp.getLocalizedString(L"crt_point"); break;
	}
	text += simulationApp.getLocalizedString(L"creator_mode") + s + L"\n";
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



