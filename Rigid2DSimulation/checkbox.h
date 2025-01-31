#pragma once
#include "tile.h"
#include "control.h"

struct Checkbox : Control {
	int h = 0;
	dVector2 gap;
	double dep = 0;
	wstring txt;
	int s_box = 0, s_box_in = 0;
	dColor c_txt, c_frame, innerColor, c_normal, c_invalid, hoveredColor;

	int w_txt = 0;
	bool checked = false;
	bool enabled = true;
	bool hovered = false;

	Checkbox(bool big = false);
	int GetW() const override;
	int GetH() const override;

	dVector2 tl_box() const;
	void render(App& app);
	virtual void Sync(App& app) {}
	virtual void Upload(App& app) const {}

	void Update(App& app) override;
	void Discard(App& app) override;
	void PreUpdate(App& app) override;
};
struct CbLan : Checkbox {
	wstring id_txt;
	CbLan() = default;
	CbLan(wstring const& id, bool big = false);
	void Update(App& app) override;
};
