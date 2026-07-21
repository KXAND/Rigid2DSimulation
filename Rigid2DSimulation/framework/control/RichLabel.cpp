#include "RichLabel.h"
#include "App.h"
#include "DrawString.h"


RichLabel::RichLabel(int w) : w(w) {
	gap = { 10, 10 }; depth = 1000;
	fixed_w = true;
	c_txt = { 255, 255, 255 }; txt = L"´ó¶ÎÎÄ±¾";
}
int RichLabel::GetW() const { return w + gap.x; }
int RichLabel::GetH() const { return h + gap.y; }

int RichLabel::GetLineWidth() const { return fixed_w ? w : 0; }
void RichLabel::Render(App& app) {
	DrawString(app.scr, app.dscr, depth, txt, c_txt, app.ft, tl, GetLineWidth(), vp);
}
void RichLabel::Update(App& app) {
	Sync(app);
	DVector2 wh = GetStringSize(txt, app.ft, GetLineWidth());
	if (!fixed_w) { w = wh.x; }
	if (!fixed_h) { h = wh.y; }
	Render(app);
}

LocalizedRichLabel::LocalizedRichLabel(int w, wstring const& id) : id_txt(id), RichLabel(w) {}
void LocalizedRichLabel::Sync(App& app) { txt = app.getLocalizedString(id_txt); }



