#include "fpn_body.h"
#include "cur.h"
#include "body.h"
#include "connection.h"

#define cur (*(Cur*)&app)
#include "my_def.h"
#define ptr_body_sel (cur.bodySelecting)
#define bsel (*ptr_body_sel)
#define pcsel (cur.connectionSelecting)
#define pcon (bsel.connections[id])

RTbBody::RTbBody() : RichTextbox(360, 400) {}
void RTbBody::Sync(App& app) { str = bsel.tmp_cmd; }
void RTbBody::Upload(App& app) const { bsel.tmp_cmd = str; }

BtBodyOK::BtBodyOK() : BtLan(70, L"bt_body_ok") {}
void BtBodyOK::OnClick(App& app) {
	bsel.cmd = bsel.tmp_cmd; bsel.refresh(cur);
}
BtBodyDel::BtBodyDel() : BtLan(70, L"bt_body_del") {}
void BtBodyDel::OnClick(App& app) { bsel.del = true; cur.isSceneChanged = true; }


LbBodyCon::LbBodyCon(int id) : id(id), Label() {
	fixed_w = true; w = 270;
}
void LbBodyCon::Sync(App& app) {
	wstring type;
	switch (pcon->type) {
	case CON_ROPE: type = getLocalizedString(L"conn_rope"); break;
	case CON_LINK: type = getLocalizedString(L"conn_link"); break;
	case CON_SPRING: type = getLocalizedString(L"conn_spring"); break;
	case CON_CORD: type = getLocalizedString(L"conn_cord"); break;
	}
	txt = getLocalizedString(L"conn") + tw(id) + L": " + type;
}

CbBodyCon::CbBodyCon(int id) : id(id), CbLan(L"cb_body_con") {}
void CbBodyCon::Sync(App& app) { checked = (pcsel == pcon); }
void CbBodyCon::Upload(App& app) const { pcsel = checked ? pcon : NULL; }

ClXBodyCon::ClXBodyCon(int id) {
	mkp(cb)(id); mkp(lb)(id);
	cs = { &*lb, &*cb };
}
void ClYBodyCons::Update(App& app) {
	rep(i, mbs.size(), bsel.connections.size()) {
		mbs.push_back(msh<ClXBodyCon>(i));
	}
	cs.clear();
	rep(i, 0, bsel.connections.size()) {
		cs.push_back(&*mbs[i]);
	}
	rep(i, bsel.connections.size(), mbs.size()) {
		mbs[i]->Discard(app);
	}
	CtrlListY::Update(app);
}

ClyBody::ClyBody() {
	vector<Control*> tmp;
	mkp(rtb)(); 
	mkp(bt_ok)(); mkp(bt_del)();
	tmp = { &*bt_ok, &*bt_del };
	mkcl(clx_bt);
	mkp(cly_cons)(); mkp(spy)(400);
}
void ClyBody::Update(App& app) {
	if (bo != ptr_body_sel) {
		bo = ptr_body_sel;
		for (auto c : cs) { c->Discard(app); }
		if (bo) { cs = { &*cly_cons, &*rtb, &*clx_bt, &*spy }; }
		else { cs = {}; }
	}
	CtrlListY::Update(app);
}
void ClyBody::Discard(App& app) {
	CtrlListY::Discard(app);
	bo = NULL; cs = {};
}

void RLbBody::Sync(App& app) {
	if (!ptr_body_sel) { txt = getLocalizedString(L"body_not_sel"); return; }
	txt.clear();
	
	txt += getLocalizedString(L"body_o") + toWstring(bsel.o) + L"\n";
	txt += getLocalizedString(L"body_v") + toWstring(bsel.velocity) + L"\n";
	txt += getLocalizedString(L"body_ang") + tw2(bsel.radian) + L"\n";
	txt += getLocalizedString(L"body_v_ang") + tw2(bsel.velocityAngular) + L"\n";
	txt += getLocalizedString(L"body_fixed") + tw(bsel.fixed) + L"\n";
	txt += getLocalizedString(L"body_density") + tw2(bsel.density) + L"\n";
	txt += getLocalizedString(L"body_charge_density") + tw2(bsel.chargeDensity) + L"\n";
	txt += getLocalizedString(L"body_e") + tw2(bsel.elasticity) + L"\n";
	txt += getLocalizedString(L"body_mu_s") + tw2(bsel.frictionStatic) + L"\n";
	txt += getLocalizedString(L"body_mu_d") + tw2(bsel.frictionDynamic) + L"\n";
	txt += getLocalizedString(L"body_damp_v") + tw2(bsel.dampCoeff) + L"\n";
	txt += getLocalizedString(L"body_damp_v_ang") + tw2(bsel.dampCoeffAngular) + L"\n";
}

FPnBody :: FPnBody(App& app) : FPnLan(app, 400, 600, L"fpn_body") {
	vector<Control*> tmp;
	mkp(cly_body)(); mkp(rlb_body)();
	tmp = { &*cly_body, &*rlb_body };
	mkcl(cly); c = &*cly; Init(app);
}
