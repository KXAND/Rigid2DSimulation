#pragma once
#include "Tile.h"
#include "Control.h"

struct HorizontalSeparator : Control {
	int w = 0;
	DVector2 gap;
	Color c_mark;

	HorizontalSeparator(int w = 0);
	int GetW() const override;
	int GetH() const override;

	void Render(App& app);
	void Update(App& app) override;
};

