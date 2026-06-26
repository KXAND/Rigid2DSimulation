#include "fpn_global.h"
#include "ui.h"
#include "simulation_app.h"
#include "creator.h"

#define cur (*(SimulationApp*)&app)

BtSave::BtSave() : BtLan(80, L"bt_save") {}
void BtSave::OnClick(App& app) { cur.Save(cur.ui->fpn_global->tb_save->str); }
BtLoad::BtLoad() : BtLan(80, L"bt_load") {}
void BtLoad::OnClick(App& app) { cur.Load(cur.ui->fpn_global->tb_load->str); }

void RLbGlobal::Sync(App& app) {
	txt = L"";
	txt += cur.getLocalizedString(L"vol") + tw2(cur.vol) + L"\n";
	txt += cur.getLocalizedString(L"n_step") + tw(cur.stepNum) + L"\n";
	txt += cur.getLocalizedString(L"s_grid") + tw2(cur.gridSize) + L"\n";
	txt += cur.getLocalizedString(L"max_real_dt") + tw2(cur.max_real_dt) + L"\n";
	txt += cur.getLocalizedString(L"tl_scene") + tw(cur.rectScene.topLeftPosition) + L"\n";
	txt += cur.getLocalizedString(L"w_scene") + tw(cur.rectScene.w) + L"\n";
	txt += cur.getLocalizedString(L"h_scene") + tw(cur.rectScene.h) + L"\n";
	txt += cur.getLocalizedString(L"gravity") + toWstring(cur.gravity) + L"\n";
	txt += cur.getLocalizedString(L"electrostatic") + tw(cur.hasElectrostatic) + L"\n";
	txt += cur.getLocalizedString(L"coulomb") + tw2(cur.coulomb) + L"\n";

	wstring s;
	switch (cur.creator->mode) {
	case CREATE_BALL: s = cur.getLocalizedString(L"crt_ball"); break;
	case CREATE_BOX: s = cur.getLocalizedString(L"crt_box"); break;
	case CREATE_CONN: s = cur.getLocalizedString(L"crt_conn"); break;
	case CREATE_NAIL: s = cur.getLocalizedString(L"crt_nail"); break;
	case CREATE_PARTICLE: s = cur.getLocalizedString(L"crt_particle"); break;
	case CREATE_PLATE: s = cur.getLocalizedString(L"crt_plate"); break;
	case CREATE_POINT: s = cur.getLocalizedString(L"crt_point"); break;
	}
	txt += cur.getLocalizedString(L"creator_mode") + s + L"\n";
}

FPnGlobal::FPnGlobal(App& app) : FPnLan(app, 400, 600, L"fpn_global") {
	vector<Control*> tmp;
	mkp(tb_save)(270); mkp(bt_save)();
	tmp = { &*tb_save, &*bt_save };
	mkp(clx_save)(tmp);
	tmp = {};
	mkp(tb_load)(270); mkp(bt_load)();
	tmp = { &*tb_load, &*bt_load };
	mkp(clx_load)(tmp);
	tmp = {};
	mkp(spy)(400); mkp(rlb_global)();
	tmp = { &*clx_save, &*clx_load, &*spy, &*rlb_global };
	mkp(cly)(tmp);
	c = &*cly;
	Init(app);
}



