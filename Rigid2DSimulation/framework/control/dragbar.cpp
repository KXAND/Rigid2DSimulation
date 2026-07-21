#include "dragbar.h"
#include "App.h"
#include "draw_geo.h"
#include "draw_str.h"


DragbarX::DragbarX(int w) : bw(w) {
	gap = { 10, 10 }; depth = 1000;
	bh = 2; nw = 12; nh = 30;

	c_txt = { 255, 255, 255 };
	c_bar = { 80, 80, 80 };
	c_normal = { 80, 80, 80 };
	hoveredColor = { 20, 20, 255 };
	c_invalid = { 0, 0, 0 };
	draggedColor = { 250, 20, 20 };
}
int DragbarX::GetW() const { return bw + gap.x + w_txt; }
int DragbarX::GetH() const { return nh + gap.y; }

// 下面的表达式是正确的，不需要加一减一什么的。
double DragbarX::uval() const { return (double)nx_rel / (bw - nw); }
void DragbarX::set_nx_rel(double uv) {
	int tmp = uv * (bw - nw);
	nx_rel = clamp(tmp, 0, bw - nw);
}
DVector2 DragbarX::tl_node() const { return tl + DVector2(w_txt + nx_rel, 0); }
void DragbarX::render(App& app) {
	DVector2 tl_txt = tl + DVector2(0, (nh - app.ft.h) / 2);
	draw_str(app.scr, app.dscr, depth, txt, c_txt, app.ft, tl_txt, 0, vp);

	dColor const& c =
		!enabled ? c_invalid :
		dragged ? draggedColor :
		(hovered && !app.input.msd[0] && !app.input.msd[1] && !app.input.msd[2]) ? hoveredColor : c_normal;
	DVector2 tl_bar = tl + DVector2(w_txt, (nh - bh) / 2);
	drawRectangleRaw(app.scr, tl_bar, bw, bh, vp, c_bar);
	drawRectangleRaw(app.scr, tl_node(), nw, nh, vp, c);
}

void DragbarX::Update(App& app) {
	hovered = (app.own.hovered == this);
	w_txt = str_wh(txt, app.ft, 0).x;
	if (dragged) {
		nx_rel = clamp(app.input.msp.x - (tl.x + w_txt + nw / 2), 0, bw - nw);
		// 在刚刚抓住的那一帧没有更新，在放下的那一帧还在更新。省事。
		Upload(app);
		if (enabled && !app.input.msd[0]) { OnDone(app); }
		dragged = enabled && app.input.msd[0];
	} else {
		Sync(app);
		dragged = enabled && hovered && app.input.isMouseClickedDown(0);
	} render(app);
}
void DragbarX::Discard(App& app) { 
	app.own.remove(this); w_txt = 0;
	dragged = hovered = false; 
}
void DragbarX::PreUpdate(App& app) {
	bool ok = app.own.hoveredDepth <= depth &&
		isInside(app.input.msp, { tl_node(), nw, nh }) && isInside(app.input.msp, vp);
	if (ok) { app.own.hoveredDepth = depth; app.own.hovered = this; }
}

DbXLan::DbXLan(wstring const& id, int w) : DragbarX(w), id_txt(id) {}
void DbXLan::Update(App& app) { txt = app.getLocalizedString(id_txt); DragbarX::Update(app); }



