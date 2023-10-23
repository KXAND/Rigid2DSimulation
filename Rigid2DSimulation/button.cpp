#include "button.h"
#include "app.h"
#include "draw_str.h"
#include "draw_comp.h"
#include "float_panel.h"

#include "ctrl_def.h"

Button::Button(int w) : w(w) {
	gap = { 10, 10 }; dep = 1000;
	txt = L"°´Å¥"; h = 30;

	c_txt = { 255, 255, 255 };
	c_frame = { 150, 150, 150 };
	c_normal = { 80, 80, 80 };
	c_chosen = { 150, 60, 60 };
	c_hovered = { 60, 60, 150 };
	c_invalid = { 0, 0, 0 };
}
int Button::GetW() const { return w + gap.x; }
int Button::GetH() const { return h + gap.y; }

void Button::render(App& app) {
	dColor const& c =
		!enabled ? c_invalid :
		chosen ? c_chosen :
		rhvd ? c_hovered : c_normal;
	dVector2 wh = str_wh(txt, ft, 0);
	dVector2 tl_txt = tl + dVector2(w, h) / 2 - wh / 2;

	drawRectangleWithBorderRaw(scr, tl, w, h, vp, c, c_frame);
	draw_str(scr, dscr, dep, txt, c_txt, ft, tl_txt, 0, vp);
}

void Button::Update(App& app) {
	hovered = (hvd == this);
	if (chosen) {
		if (enabled && !msd[0]) { OnClick(app); }
		chosen = enabled && hovered && msd[0];
	} else {
		chosen = enabled && hovered && msc(0);
	} render(app);
}
void Button::Discard(App& app) { rmv; chosen = hovered = false; }
void Button::PreUpdate(App& app) {
	bool ok = dhv <= dep &&
		isInside(msp, { tl, w, h }) && isInside(msp, vp);
	if (ok) { dhv = dep; hvd = this; }
}

BtLan::BtLan(int w, wstring const& id) : Button(w), id_txt(id) {}
void BtLan::Update(App& app) { txt = loc(id_txt); Button::Update(app); }

BtFPn::BtFPn(FloatPanel* fpn) : Button(150), fpn(fpn) {}
void BtFPn::OnClick(App& app) { fpn->show(app); }
void BtFPn::Update(App& app) {
	enabled = !fpn->shown;
	txt = fpn->txt; Button::Update(app);
}
