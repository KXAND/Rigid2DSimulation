#include "RichLabel.h"
#include "App.h"
#include "DrawString.h"


RichLabel::RichLabel(int width) : width(width) {
	gap = { 10, 10 }; depth = 1000;
	fixedWidth = true;
	textColor = { 255, 255, 255 }; text = L"´ó¶ÎÎÄ±¾";
}
int RichLabel::GetWidth() const { return width + gap.x; }
int RichLabel::GetHeight() const { return height + gap.y; }

int RichLabel::GetLineWidth() const { return fixedWidth ? width : 0; }
void RichLabel::Render(App& app) {
	DrawString(app.screen, app.depthBuffer, depth, text, textColor, app.font, topLeft, GetLineWidth(), viewport);
}
void RichLabel::Update(App& app) {
	Sync(app);
	DVector2 textSize = GetStringSize(text, app.font, GetLineWidth());
	if (!fixedWidth) { width = textSize.x; }
	if (!fixedHeight) { height = textSize.y; }
	Render(app);
}

LocalizedRichLabel::LocalizedRichLabel(int width, wstring const& id) : textId(id), RichLabel(width) {}
void LocalizedRichLabel::Sync(App& app) { text = app.GetLocalizedString(textId); }



