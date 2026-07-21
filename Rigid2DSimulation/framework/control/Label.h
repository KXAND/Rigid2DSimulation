#pragma once
#include "Font.h"
#include "Control.h"

struct Label : Control {
	DVector2 gap;
	double depth = 0;
	int w = 0, h = 0;
	bool fixed_w = false;

	Color c_txt;
	wstring txt;

	Label(bool big = false);
	int GetW() const override;
	int GetH() const override;

	void render(App& app);
	virtual void Sync(App& app) {}
	void Update(App& app) override;
};
struct LocalizedLabel : Label {
	wstring id_txt;
	LocalizedLabel() = default;
	LocalizedLabel(wstring const& id, bool big = false);
	void Sync(App& app) override;
};


