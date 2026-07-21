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
	DVector2 textTopLeft = tl + DVector2(0, h - app.font.h) / 2;
	DrawString(app.screen, app.depthBuffer, depth, text, textColor, app.font, textTopLeft, 0, vp);
}
void Label::Update(App& app) {
	Sync(app);
	if (!fixedWidth) { w = GetStringSize(text, app.font, 0).x; }
	Render(app);
}

LocalizedLabel::LocalizedLabel(wstring const& id, bool big) : Label(big), textId(id) {}
void LocalizedLabel::Sync(App& app) { text = app.GetLocalizedString(textId); }



