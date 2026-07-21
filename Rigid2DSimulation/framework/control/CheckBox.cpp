#include "CheckBox.h"
#include "App.h"
#include "DrawGeometry.h"
#include "DrawString.h"


CheckBox::CheckBox(bool big) {
	h = big ? 30 : 20;
	gap = { 10, 10 }; depth = 1000;
	boxSize = 20; innerBoxSize = 10;

	textColor = { 255, 255, 255 };
	frameColor = { 150, 150, 150 };
	innerColor = { 0, 255, 0 };
	normalColor = { 80, 80, 80 };
	invalidColor = { 0, 0, 0 };
	hoveredColor = { 20, 20, 255 };
}
// 这里会利用到空格的宽度刚好等于 gap.x。
// 这里会利用到空格的宽度刚好等于 gap.x。
int CheckBox::GetW() const { return boxSize + gap.x + textWidth; }
int CheckBox::GetH() const { return h + gap.y; }

DVector2 CheckBox::GetBoxTopLeft() const { return tl + DVector2(textWidth, (h - boxSize) / 2); }
void CheckBox::Render(App& app) {
	DVector2 textTopLeft = tl + DVector2(0, (h - app.ft.h) / 2);
	DrawString(app.scr, app.dscr, depth, text, textColor, app.ft, textTopLeft, 0, vp);

	Color const& color =
		!enabled ? invalidColor :
		(hovered && !app.input.msd[0] && !app.input.msd[1] && !app.input.msd[2]) ? hoveredColor : normalColor;
	drawRectangleRaw(app.scr, GetBoxTopLeft(), boxSize, boxSize, vp, color);
	if (checked) {
		int innerOffset = (boxSize - innerBoxSize) / 2;
		DVector2 innerBoxTopLeft = GetBoxTopLeft() + DVector2(innerOffset, innerOffset);
		drawRectangleRaw(app.scr, innerBoxTopLeft, innerBoxSize, innerBoxSize, vp, innerColor);
	}
}

void CheckBox::Update(App& app) {
	hovered = (app.own.hovered == this);
	textWidth = GetStringSize(text, app.ft, 0).x;
	bool click = enabled && hovered && app.input.IsMouseClickedDown(0);
	if (click) { checked = !checked; Upload(app); }
	else { Sync(app); } Render(app);
}
void CheckBox::Discard(App& app) {
	app.own.Remove(this); textWidth = 0;
	hovered = false;
}
void CheckBox::PreUpdate(App& app) {
	bool ok = app.own.hoveredDepth <= depth &&
		isInside(app.input.msp, { GetBoxTopLeft(), boxSize, boxSize }) && isInside(app.input.msp, vp);
	if (ok) { app.own.hoveredDepth = depth; app.own.hovered = this; }
}

LocalizedCheckBox::LocalizedCheckBox(wstring const& id, bool big) : CheckBox(big), textId(id) {}
void LocalizedCheckBox::Update(App& app) { text = app.GetLocalizedString(textId); CheckBox::Update(app); }



