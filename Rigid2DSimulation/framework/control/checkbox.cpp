#include "checkbox.h"
#include "app.h"
#include "draw_geo.h"
#include "draw_str.h"


Checkbox::Checkbox(bool big) {
	h = big ? 30 : 20;
	gap = { 10, 10 }; dep = 1000;
	s_box = 20; s_box_in = 10;

	c_txt = { 255, 255, 255 };
	c_frame = { 150, 150, 150 };
	innerColor = { 0, 255, 0 };
	c_normal = { 80, 80, 80 };
	c_invalid = { 0, 0, 0 };
	hoveredColor = { 20, 20, 255 };
}
// ��������õ��ո�Ŀ��ȸպõ��� gap.x��
// ����Ļ�Ӧ�ö� txt �Ƿ�Ϊ�������ۣ��������鷳�����ˡ�
int Checkbox::GetW() const { return s_box + gap.x + w_txt; }
int Checkbox::GetH() const { return h + gap.y; }

dVector2 Checkbox::tl_box() const { return tl + dVector2(w_txt, (h - s_box) / 2); }
void Checkbox::render(App& app) {
	dVector2 tl_txt = tl + dVector2(0, (h - app.ft.h) / 2);
	draw_str(app.scr, app.dscr, dep, txt, c_txt, app.ft, tl_txt, 0, vp);

	dColor const& c =
		!enabled ? c_invalid :
		(hovered && !app.input.msd[0] && !app.input.msd[1] && !app.input.msd[2]) ? hoveredColor : c_normal;
	drawRectangleRaw(app.scr, tl_box(), s_box, s_box, vp, c);
	if (checked) {
		int hs = (s_box - s_box_in) / 2;
		dVector2 tl_box_in = tl_box() + dVector2(hs, hs);
		drawRectangleRaw(app.scr, tl_box_in, s_box_in, s_box_in, vp, innerColor);
	}
}

void Checkbox::Update(App& app) {
	hovered = (app.own.hovered == this);
	w_txt = str_wh(txt, app.ft, 0).x;
	bool click = enabled && hovered && app.input.isMouseClickedDown(0);
	if (click) { checked = !checked; Upload(app); }
	else { Sync(app); } render(app);
}
void Checkbox::Discard(App& app) { 
	app.own.remove(this); w_txt = 0;
	hovered = false; 
}
void Checkbox::PreUpdate(App& app) {
	bool ok = app.own.hoveredDepth <= dep &&
		isInside(app.input.msp, { tl_box(), s_box, s_box }) && isInside(app.input.msp, vp);
	if (ok) { app.own.hoveredDepth = dep; app.own.hovered = this; }
}

CbLan::CbLan(wstring const& id, bool big) : Checkbox(big), id_txt(id) {}
void CbLan::Update(App& app) { txt = app.getLocalizedString(id_txt); Checkbox::Update(app); }


