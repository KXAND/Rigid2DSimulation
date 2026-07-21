#include "button.h"
#include "app.h"
#include "draw_str.h"
#include "draw_comp.h"
#include "float_panel.h"


Button::Button(int w) : w(w) {
	gap = { 10, 10 }; depth = 1000;
	txt = L"°´Å¥"; h = 30;

	c_txt = { 255, 255, 255 };
	c_frame = { 150, 150, 150 };
	c_normal = { 80, 80, 80 };
	c_chosen = { 150, 60, 60 };
	hoveredColor = { 60, 60, 150 };
	c_invalid = { 0, 0, 0 };
}
int Button::GetW() const { return w + gap.x; }
int Button::GetH() const { return h + gap.y; }

void Button::render(App& app) {
	dColor const& c =
		!enabled ? c_invalid :
		chosen ? c_chosen :
		(hovered && !app.input.msd[0] && !app.input.msd[1] && !app.input.msd[2]) ? hoveredColor : c_normal;
	DVector2 wh = str_wh(txt, app.ft, 0);
	DVector2 tl_txt = tl + DVector2(w, h) / 2 - wh / 2;

	drawRectangleWithBorderRaw(app.scr, tl, w, h, vp, c, c_frame);
	draw_str(app.scr, app.dscr, depth, txt, c_txt, app.ft, tl_txt, 0, vp);
}

void Button::Update(App& app) {
	hovered = (app.own.hovered == this);
	if (chosen) {
		if (enabled && !app.input.msd[0]) { OnClick(app); }
		chosen = enabled && hovered && app.input.msd[0];
	} else {
		chosen = enabled && hovered && app.input.isMouseClickedDown(0);
	} render(app);
}
void Button::Discard(App& app) { app.own.remove(this); chosen = hovered = false; }
void Button::PreUpdate(App& app) {
	bool ok = app.own.hoveredDepth <= depth &&
		isInside(app.input.msp, { tl, w, h }) && isInside(app.input.msp, vp);
	if (ok) { app.own.hoveredDepth = depth; app.own.hovered = this; }
}

BtLan::BtLan(int w, wstring const& id) : Button(w), id_txt(id) {}
void BtLan::Update(App& app) { txt = app.getLocalizedString(id_txt); Button::Update(app); }

BtFPn::BtFPn(FloatPanel* fpn) : Button(150), fpn(fpn) {}
void BtFPn::OnClick(App& app) { fpn->show(app); }
void BtFPn::Update(App& app) {
	enabled = !fpn->shown;
	txt = fpn->txt; Button::Update(app);
}



