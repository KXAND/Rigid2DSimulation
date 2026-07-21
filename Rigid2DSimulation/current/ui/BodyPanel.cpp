#include "BodyPanel.h"
#include "SimulationApp.h"
#include "Body.h"
#include "Connection.h"

BodyScriptEditor::BodyScriptEditor() : RichTextBox(360, 400) {}
void BodyScriptEditor::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); str = simulationApp.bodySelecting->temporaryScriptText; }
void BodyScriptEditor::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.bodySelecting->temporaryScriptText = str; }

ApplyBodyButton::ApplyBodyButton() : LocalizedButton(70, L"bt_body_ok") {}
void ApplyBodyButton::OnClick(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto& selectedBody = *simulationApp.bodySelecting;
	selectedBody.scriptText = selectedBody.temporaryScriptText;
	selectedBody.RefreshFromScript(simulationApp);
}
DeleteBodyButton::DeleteBodyButton() : LocalizedButton(70, L"bt_body_del") {}
void DeleteBodyButton::OnClick(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.bodySelecting->isMarkedForDeletion = true; simulationApp.isSceneChanged = true; }


BodyConnectionLabel::BodyConnectionLabel(int id) : id(id), Label() {
	fixed_w = true; w = 270;
}
void BodyConnectionLabel::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto* connection = simulationApp.bodySelecting->connections[id];
	wstring type;
	switch (connection->type) {
	case CON_ROPE: type = simulationApp.getLocalizedString(L"conn_rope"); break;
	case CON_LINK: type = simulationApp.getLocalizedString(L"conn_link"); break;
	case CON_SPRING: type = simulationApp.getLocalizedString(L"conn_spring"); break;
	case CON_CORD: type = simulationApp.getLocalizedString(L"conn_cord"); break;
	}
	txt = simulationApp.getLocalizedString(L"conn") + std::to_wstring(id) + L": " + type;
}

SelectBodyConnectionCheckBox::SelectBodyConnectionCheckBox(int id) : id(id), LocalizedCheckBox(L"cb_body_con") {}
void SelectBodyConnectionCheckBox::Sync(App& app) { auto& simulationApp = static_cast<SimulationApp&>(app); checked = (simulationApp.connectionSelecting == simulationApp.bodySelecting->connections[id]); }
void SelectBodyConnectionCheckBox::Upload(App& app) const { auto& simulationApp = static_cast<SimulationApp&>(app); simulationApp.connectionSelecting = checked ? simulationApp.bodySelecting->connections[id] : NULL; }

BodyConnectionControlRow::BodyConnectionControlRow(int id) {
	cb = std::make_shared<SelectBodyConnectionCheckBox>(id); lb = std::make_shared<BodyConnectionLabel>(id);
	cs = { &*lb, &*cb };
}
void BodyConnectionControlList::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto& selectedBody = *simulationApp.bodySelecting;
	for (int i = (mbs.size()), iEnd = int(selectedBody.connections.size()); i < iEnd; ++i) {
		mbs.push_back(std::make_shared<BodyConnectionControlRow>(i));
	}
	cs.clear();
	for (int i = (0), iEnd = int(selectedBody.connections.size()); i < iEnd; ++i) {
		cs.push_back(&*mbs[i]);
	}
	for (int i = (selectedBody.connections.size()), iEnd = int(mbs.size()); i < iEnd; ++i) {
		mbs[i]->Discard(app);
	}
	VerticalControlList::Update(app);
}

BodyEditorControls::BodyEditorControls() {
	vector<Control*> tmp;
	rtb = std::make_shared<BodyScriptEditor>();
	bt_ok = std::make_shared<ApplyBodyButton>(); bt_del = std::make_shared<DeleteBodyButton>();
	tmp = { &*bt_ok, &*bt_del };
	clx_bt = std::make_shared<HorizontalControlList>(tmp);
	tmp = {};
	cly_cons = std::make_shared<BodyConnectionControlList>(); spy = std::make_shared<HorizontalSeparator>(400);
}
void BodyEditorControls::Update(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto* selectedBody = simulationApp.bodySelecting;
	if (bo != selectedBody) {
		bo = selectedBody;
		for (auto c : cs) { c->Discard(app); }
		if (bo) { cs = { &*cly_cons, &*rtb, &*clx_bt, &*spy }; }
		else { cs = {}; }
	}
	VerticalControlList::Update(app);
}
void BodyEditorControls::Discard(App& app) {
	VerticalControlList::Discard(app);
	bo = NULL; cs = {};
}

void BodyDetailsLabel::Sync(App& app) {
	auto& simulationApp = static_cast<SimulationApp&>(app);
	auto* selectedBody = simulationApp.bodySelecting;
	if (!selectedBody) { txt = simulationApp.getLocalizedString(L"body_not_sel"); return; }
	auto& body = *selectedBody;
	txt.clear();

	txt += simulationApp.getLocalizedString(L"body_o") + ToWString(body.position) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_v") + ToWString(body.velocity) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_ang") + FormatFixed2(body.radian) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_v_ang") + FormatFixed2(body.velocityAngular) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_fixed") + std::to_wstring(body.fixed) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_density") + FormatFixed2(body.density) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_charge_density") + FormatFixed2(body.chargeDensity) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_e") + FormatFixed2(body.elasticity) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_mu_s") + FormatFixed2(body.frictionStatic) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_mu_d") + FormatFixed2(body.frictionDynamic) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_damp_v") + FormatFixed2(body.dampCoeff) + L"\n";
	txt += simulationApp.getLocalizedString(L"body_damp_v_ang") + FormatFixed2(body.dampCoeffAngular) + L"\n";
}

BodyPanel :: BodyPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"fpn_body") {
	vector<Control*> tmp;
	cly_body = std::make_shared<BodyEditorControls>(); rlb_body = std::make_shared<BodyDetailsLabel>();
	tmp = { &*cly_body, &*rlb_body };
	cly = std::make_shared<VerticalControlList>(tmp);
	c = &*cly;
	Init(app);
}



