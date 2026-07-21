#include "label.h"
#include "App.h"
#include "draw_str.h"


Label::Label(bool big) {
	gap = { 10, 10 }; depth = 1000;
	h = big ? 30 : 20;
	c_txt = { 255, 255, 255 }; txt = L"ÎÄ±¾";
}
int Label::GetW() const { return w + gap.x; }
int Label::GetH() const { return h + gap.y; }

void Label::render(App& app) {
	DVector2 tl_txt = tl + DVector2(0, h - app.ft.h) / 2;
	draw_str(app.scr, app.dscr, depth, txt, c_txt, app.ft, tl_txt, 0, vp);
}
void Label::Update(App& app) {
	Sync(app);
	if (!fixed_w) { w = str_wh(txt, app.ft, 0).x; }
	render(app);
}

LbLan::LbLan(wstring const& id, bool big) : Label(big), id_txt(id) {}
void LbLan::Sync(App& app) { txt = app.getLocalizedString(id_txt); }



