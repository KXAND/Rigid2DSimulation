#include "Separator.h"
#include "App.h"
#include "DrawGeometry.h"


HorizontalSeparator::HorizontalSeparator(int w) : w(w) {
	gap = { 10, 10 };
	markerColor = { 150, 150, 150 };
}
int HorizontalSeparator::GetW() const { return w + gap.x; }
int HorizontalSeparator::GetH() const { return 1 + gap.y; }

void HorizontalSeparator::Render(App& app) {
	// 虽然 dep 没什么用，但出于习惯还是保留了。
	drawRectangleRaw(app.screen, tl, w, 1, vp, markerColor);
}
void HorizontalSeparator::Update(App& app) { Render(app); }



