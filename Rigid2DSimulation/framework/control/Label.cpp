#include "Label.h"
#include "App.h"
#include "DrawString.h"


Label::Label(bool big) {
	gap = { 10, 10 }; depth = 1000;
	h = big ? 30 : 20;
	c_txt = { 255, 255, 255 }; txt = L"ÎÄ±¾";
}
int Label::GetW() const { return w + gap.x; }
int Label::GetH() const { return h + gap.y; }

void Label::Render(App& app) {
	DVector2 tl_txt = tl + DVector2(0, h - app.ft.h) / 2;
	DrawString(app.scr, app.dscr, depth, txt, c_txt, app.ft, tl_txt, 0, vp);
}
void Label::Update(App& app) {
	Sync(app);
	if (!fixed_w) { w = GetStringSize(txt, app.ft, 0).x; }
	Render(app);
}

LocalizedLabel::LocalizedLabel(wstring const& id, bool big) : Label(big), id_txt(id) {}
void LocalizedLabel::Sync(App& app) { txt = app.getLocalizedString(id_txt); }



