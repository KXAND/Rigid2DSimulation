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
	DVector2 textTopLeft = tl + DVector2(0, (h - app.font.h) / 2);
	DrawString(app.screen, app.depthBuffer, depth, text, textColor, app.font, textTopLeft, 0, vp);

	Color const& color =
		!enabled ? invalidColor :
		(hovered && !app.input.mouseButtonDown[0] && !app.input.mouseButtonDown[1] && !app.input.mouseButtonDown[2]) ? hoveredColor : normalColor;
	drawRectangleRaw(app.screen, GetBoxTopLeft(), boxSize, boxSize, vp, color);
	if (checked) {
		int innerOffset = (boxSize - innerBoxSize) / 2;
		DVector2 innerBoxTopLeft = GetBoxTopLeft() + DVector2(innerOffset, innerOffset);
		drawRectangleRaw(app.screen, innerBoxTopLeft, innerBoxSize, innerBoxSize, vp, innerColor);
	}
}

void CheckBox::Update(App& app) {
	hovered = (app.inputOwners.hovered == this);
	textWidth = GetStringSize(text, app.font, 0).x;
	bool click = enabled && hovered && app.input.IsMouseClickedDown(0);
	if (click) { checked = !checked; Upload(app); }
	else { Sync(app); } Render(app);
}
void CheckBox::Discard(App& app) {
	app.inputOwners.Remove(this); textWidth = 0;
	hovered = false;
}
void CheckBox::PreUpdate(App& app) {
	bool ok = app.inputOwners.hoveredDepth <= depth &&
		isInside(app.input.mousePosition, { GetBoxTopLeft(), boxSize, boxSize }) && isInside(app.input.mousePosition, vp);
	if (ok) { app.inputOwners.hoveredDepth = depth; app.inputOwners.hovered = this; }
}

LocalizedCheckBox::LocalizedCheckBox(wstring const& id, bool big) : CheckBox(big), textId(id) {}
void LocalizedCheckBox::Update(App& app) { text = app.GetLocalizedString(textId); CheckBox::Update(app); }



