#include "fpn_global.h"
#include "ui.h"
#include "cur.h"
#include "creator.h"

#define cur (*(Cur*)&app)
#include "my_def.h"

BtSave::BtSave() : BtLan(80, L"bt_save") {}
void BtSave::OnClick(App& app) { cur.Save(ui.fpn_global->tb_save->str); }
BtLoad::BtLoad() : BtLan(80, L"bt_load") {}
void BtLoad::OnClick(App& app) { cur.Load(ui.fpn_global->tb_load->str); }

void RLbGlobal::Sync(App& app) {
	txt = L"";
	txt += getLocalizedString(L"vol") + tw2(cur.vol) + L"\n";
	txt += getLocalizedString(L"n_step") + tw(cur.stepNum) + L"\n";
	txt += getLocalizedString(L"s_grid") + tw2(cur.gridSize) + L"\n";
	txt += getLocalizedString(L"max_real_dt") + tw2(cur.max_real_dt) + L"\n";
	txt += getLocalizedString(L"tl_scene") + tw(cur.rectScene.topLeftPosition) + L"\n";
	txt += getLocalizedString(L"w_scene") + tw(cur.rectScene.w) + L"\n";
	txt += getLocalizedString(L"h_scene") + tw(cur.rectScene.h) + L"\n";
	txt += getLocalizedString(L"gravity") + toWstring(cur.gravity) + L"\n";
	txt += getLocalizedString(L"electrostatic") + tw(cur.hasElectrostatic) + L"\n";
	txt += getLocalizedString(L"coulomb") + tw2(cur.coulomb) + L"\n";

	wstring s;
	switch (cur.creator->mode) {
	case CREATE_BALL: s = getLocalizedString(L"crt_ball"); break;
	case CREATE_BOX: s = getLocalizedString(L"crt_box"); break;
	case CREATE_CONN: s = getLocalizedString(L"crt_conn"); break;
	case CREATE_NAIL: s = getLocalizedString(L"crt_nail"); break;
	case CREATE_PARTICLE: s = getLocalizedString(L"crt_particle"); break;
	case CREATE_PLATE: s = getLocalizedString(L"crt_plate"); break;
	case CREATE_POINT: s = getLocalizedString(L"crt_point"); break;
	}
	txt += getLocalizedString(L"creator_mode") + s + L"\n";
}

FPnGlobal::FPnGlobal(App& app) : FPnLan(app, 400, 600, L"fpn_global") {
	vector<Control*> tmp;
	mkp(tb_save)(270); mkp(bt_save)();
	tmp = { &*tb_save, &*bt_save }; mkcl(clx_save);
	mkp(tb_load)(270); mkp(bt_load)();
	tmp = { &*tb_load, &*bt_load }; mkcl(clx_load);
	mkp(spy)(400); mkp(rlb_global)();
	tmp = { &*clx_save, &*clx_load, &*spy, &*rlb_global };
	mkcl(cly); c = &*cly; Init(app);
}
