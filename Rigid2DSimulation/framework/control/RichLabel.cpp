#include "RichLabel.h"
#include "App.h"
#include "DrawString.h"


RichLabel::RichLabel(int w) : w(w) {
	gap = { 10, 10 }; depth = 1000;
	fixedWidth = true;
	textColor = { 255, 255, 255 }; text = L"´ó¶ÎÎÄ±¾";
}
int RichLabel::GetW() const { return w + gap.x; }
int RichLabel::GetH() const { return h + gap.y; }

int RichLabel::GetLineWidth() const { return fixedWidth ? w : 0; }
void RichLabel::Render(App& app) {
	DrawString(app.scr, app.dscr, depth, text, textColor, app.ft, tl, GetLineWidth(), vp);
}
void RichLabel::Update(App& app) {
	Sync(app);
	DVector2 textSize = GetStringSize(text, app.ft, GetLineWidth());
	if (!fixedWidth) { w = textSize.x; }
	if (!fixedHeight) { h = textSize.y; }
	Render(app);
}

LocalizedRichLabel::LocalizedRichLabel(int w, wstring const& id) : textId(id), RichLabel(w) {}
void LocalizedRichLabel::Sync(App& app) { text = app.getLocalizedString(textId); }



