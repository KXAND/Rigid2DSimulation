#pragma once
#include "tile.h"
#include "control.h"

struct DragbarX : Control {
	dVector2 gap;
	double dep = 0;
	wstring txt;
	int bw = 0, bh = 0;
	int nw = 0, nh = 0;
	dColor c_txt, c_bar, c_normal, hoveredColor, c_invalid, draggedColor;

	int w_txt = 0;
	int nx_rel = 0;
	bool dragged = false;
	bool enabled = true;
	bool hovered = false;

	DragbarX(int w = 0);
	int GetW() const override;
	int GetH() const override;

	double uval() const;
	void set_nx_rel(double uv);
	dVector2 tl_node() const;

	void render(App& app);
	virtual void Sync(App& app) {}
	virtual void OnDone(App& app) const {}
	virtual void Upload(App& app) const {}

	void Update(App& app) override;
	void Discard(App& app) override;
	void PreUpdate(App& app) override;
};
struct DbXLan : DragbarX {
	wstring id_txt;
	DbXLan() = default;
	DbXLan(wstring const& id, int w);
	void Update(App& app) override;
};
