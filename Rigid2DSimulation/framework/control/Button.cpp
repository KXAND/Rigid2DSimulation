#include "Button.h"
#include "App.h"
#include "DrawString.h"
#include "DrawBorderedRectangle.h"
#include "FloatPanel.h"


Button::Button(int w) : w(w) {
	gap = { 10, 10 }; depth = 1000;
	text = L"°´Å¥"; h = 30;

	textColor = { 255, 255, 255 };
	frameColor = { 150, 150, 150 };
	normalColor = { 80, 80, 80 };
	chosenColor = { 150, 60, 60 };
	hoveredColor = { 60, 60, 150 };
	invalidColor = { 0, 0, 0 };
}
int Button::GetW() const { return w + gap.x; }
int Button::GetH() const { return h + gap.y; }

void Button::Render(App& app) {
	Color const& color =
		!enabled ? invalidColor :
		chosen ? chosenColor :
		(hovered && !app.input.msd[0] && !app.input.msd[1] && !app.input.msd[2]) ? hoveredColor : normalColor;
	DVector2 textSize = GetStringSize(text, app.ft, 0);
	DVector2 textTopLeft = tl + DVector2(w, h) / 2 - textSize / 2;

	drawRectangleWithBorderRaw(app.scr, tl, w, h, vp, color, frameColor);
	DrawString(app.scr, app.dscr, depth, text, textColor, app.ft, textTopLeft, 0, vp);
}

void Button::Update(App& app) {
	hovered = (app.own.hovered == this);
	if (chosen) {
		if (enabled && !app.input.msd[0]) { OnClick(app); }
		chosen = enabled && hovered && app.input.msd[0];
	} else {
		chosen = enabled && hovered && app.input.IsMouseClickedDown(0);
	} Render(app);
}
void Button::Discard(App& app) { app.own.Remove(this); chosen = hovered = false; }
void Button::PreUpdate(App& app) {
	bool ok = app.own.hoveredDepth <= depth &&
		isInside(app.input.msp, { tl, w, h }) && isInside(app.input.msp, vp);
	if (ok) { app.own.hoveredDepth = depth; app.own.hovered = this; }
}

LocalizedButton::LocalizedButton(int w, wstring const& id) : Button(w), textId(id) {}
void LocalizedButton::Update(App& app) { text = app.GetLocalizedString(textId); Button::Update(app); }

ShowFloatPanelButton::ShowFloatPanelButton(FloatPanel* floatPanel) : Button(150), floatPanel(floatPanel) {}
void ShowFloatPanelButton::OnClick(App& app) { floatPanel->Show(app); }
void ShowFloatPanelButton::Update(App& app) {
	enabled = !floatPanel->shown;
	text = floatPanel->title; Button::Update(app);
}



