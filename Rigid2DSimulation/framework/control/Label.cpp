#include "Label.h"
#include "App.h"
#include "DrawString.h"


Label::Label(bool big) {
	gap = { 10, 10 }; depth = 1000;
	height = big ? 30 : 20;
	textColor = { 255, 255, 255 }; text = L"ÎÄ±¾";
}
int Label::GetWidth() const { return width + gap.x; }
int Label::GetHeight() const { return height + gap.y; }

void Label::Render(App& app) {
	DVector2 textTopLeft = topLeft + DVector2(0, height - app.font.height) / 2;
	DrawString(app.screen, app.depthBuffer, depth, text, textColor, app.font, textTopLeft, 0, viewport);
}
void Label::Update(App& app) {
	Sync(app);
	if (!fixedWidth) { width = GetStringSize(text, app.font, 0).x; }
	Render(app);
}

LocalizedLabel::LocalizedLabel(wstring const& id, bool big) : Label(big), textId(id) {}
void LocalizedLabel::Sync(App& app) { text = app.GetLocalizedString(textId); }



