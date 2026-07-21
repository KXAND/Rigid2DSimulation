#include "Label.h"
#include "App.h"
#include "DrawString.h"


Label::Label(bool big) {
	gap = { 10, 10 }; depth = 1000;
	h = big ? 30 : 20;
	textColor = { 255, 255, 255 }; text = L"ÎÄ±¾";
}
int Label::GetW() const { return w + gap.x; }
int Label::GetH() const { return h + gap.y; }

void Label::Render(App& app) {
	DVector2 textTopLeft = tl + DVector2(0, h - app.ft.h) / 2;
	DrawString(app.scr, app.dscr, depth, text, textColor, app.ft, textTopLeft, 0, vp);
}
void Label::Update(App& app) {
	Sync(app);
	if (!fixedWidth) { w = GetStringSize(text, app.ft, 0).x; }
	Render(app);
}

LocalizedLabel::LocalizedLabel(wstring const& id, bool big) : Label(big), textId(id) {}
void LocalizedLabel::Sync(App& app) { text = app.getLocalizedString(textId); }



