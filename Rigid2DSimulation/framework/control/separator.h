#pragma once
#include "Tile.h"
#include "control.h"

struct SeparatorY : Control {
	int w = 0;
	DVector2 gap;
	dColor c_mark;

	SeparatorY(int w = 0);
	int GetW() const override;
	int GetH() const override;

	void render(App& app);
	void Update(App& app) override;
};

