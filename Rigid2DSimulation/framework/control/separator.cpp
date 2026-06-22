#include "separator.h"
#include "app.h"
#include "draw_geo.h"


SeparatorY::SeparatorY(int w) : w(w) {
	gap = { 10, 10 };
	c_mark = { 150, 150, 150 };
}
int SeparatorY::GetW() const { return w + gap.x; }
int SeparatorY::GetH() const { return 1 + gap.y; }

void SeparatorY::render(App& app) {
	// ��Ȼ dep ûʲô�ã�������ϰ�߻��Ǳ����ˡ�
	drawRectangleRaw(app.scr, tl, w, 1, vp, c_mark);
}
void SeparatorY::Update(App& app) { render(app); }


