#include "DropList.h"
#include "App.h"
#include "DrawGeometry.h"
#include "DrawString.h"
#include "DrawBorderedRectangle.h"


wstring nm_none(int id, App &app) { return L"空"; }
DropList::DropList(int w, DpNm const& nm) : w(w), nm(nm) {
	gap = { 10, 10 }; depth = 1000;
	h_item = 25; h = 30;

	sign_r = 5;
	sign_margin = 15;
	x_txt_margin = 5;

	// 懒得区分标签和内容的颜色了。
	c_txt = { 255, 255, 255 };
	c_mark = { 255, 255, 255 };
	c_edit = { 150, 60, 60 };
	c_frame = { 150, 150, 150 };
	c_normal = { 40, 40, 40 };
	c_invalid = { 0, 0, 0 };
	hoveredColor = { 60, 60, 150 };
}
int DropList::GetW() const { return w + gap.x + w_txt; }
int DropList::GetH() const {
	return h + gap.y + (edit ? n_item * h_item : 0);
}

DVector2 DropList::tl_box() const { return tl + DVector2(w_txt, 0); }
DVector2 DropList::tl_item() const { return tl_box() + DVector2(0, h); }
void DropList::render_main(App& app) {
	DVector2 tl_txt = tl + DVector2(0, (h - app.ft.h) / 2);
	DrawString(app.scr, app.dscr, depth, txt, c_txt, app.ft, tl_txt, 0, vp);

	Color const& c =
		!enabled ? c_invalid :
		edit ? c_edit :
		(hovered && !app.input.msd[0] && !app.input.msd[1] && !app.input.msd[2]) ? hoveredColor : c_normal;
	drawRectangleWithBorder(app.scr, app.dscr, depth, tl_box(), w, h, vp, c, c_frame);

	Vector2 ct_sign = (Vector2)tl_box() + Vector2(w - sign_margin, h / 2);
	drawEllipse(app.scr, app.dscr, depth, ct_sign, sign_r, sign_r, vp, c_mark);

	DVector2 tl_item = tl + DVector2(x_txt_margin, (h - app.ft.h) / 2);
	DrawString(app.scr, app.dscr, depth, nm(picked, app), c_txt, app.ft, tl_txt, 0, vp);
}
void DropList::render_items(App& app) {
	if (edit) {
		drawRectangleWithBorder(app.scr, app.dscr, depth, 
			tl_item(), w, h_item * n_item, vp, c_normal, c_frame);
		if (item_hv != -1) {
			DVector2 tl_hovered = tl_box() + DVector2(0, h + h_item * item_hv);
			drawRectangleWithBorder(app.scr, app.dscr, depth,
				tl_hovered, w, h_item, vp, hoveredColor, c_frame);
		}
		for (int i = (0), iEnd = int(n_item); i < iEnd; ++i) {
			DVector2 tl_txt_item = tl_box() +
				DVector2(0, h + h_item * i) +
				DVector2(x_txt_margin, (h_item - app.ft.h) / 2);
			DrawString(app.scr, app.dscr, depth, nm(i, app), c_txt, app.ft, tl_txt_item, 0, vp);
		}
	}
}
void DropList::render(App& app) {
	render_main(app); render_items(app);
}

// 在内容可变的情形下怎么处理我还没想清楚的。
void DropList::Update(App& app) {
	hovered = (app.own.hovered == this);
	w_txt = str_wh(txt, app.ft, 0).x;
	if (edit) {
		if (hovered) {
			int tmp = app.input.msp.y - tl_box().y - h;
			item_hv = tmp >= 0 ? tmp / h_item : -1;
		}
		if (enabled && hovered && app.input.isMouseClickedDown(0)) {
			if (item_hv != -1) { picked = item_hv; }
			Upload(app);
		}
		if (!enabled || app.input.isMouseClickedDown(0)) { item_hv = -1; edit = false; }
	} else {
		Sync(app);
		edit = enabled && hovered && app.input.isMouseClickedDown(0);
	} render(app);
}
void DropList::Discard(App& app) {
	app.own.remove(this); w_txt = 0;
	edit = hovered = false; item_hv = -1;
}
void DropList::PreUpdate(App& app) {
	int real_h = h + (edit ? n_item * h_item : 0);
	bool ok = app.own.hoveredDepth <= depth &&
		isInside(app.input.msp, { tl_box(), w, real_h}) && isInside(app.input.msp, vp);
	if (ok) { app.own.hoveredDepth = depth; app.own.hovered = this; }
}

DpLan::DpLan(wstring const& id, int w, DpNm const& nm) : DropList(w, nm), id_txt(id) {}
void DpLan::Update(App& app) { txt = app.getLocalizedString(id_txt); DropList::Update(app); }



