#pragma once
#include "vector3.h"
#include "tile.h"

struct Cur;
struct Bgr {
	dVector2 tl;
	double dep = 0;
	tile black;
	int w = 0, h = 0;

	bool hovered = false;
	bool wheeled = false;

	Bgr(Cur& cur);
	dRect vp() const { return { tl, w, h }; }
	void render(Cur& cur);

	void Update(Cur& cur);
	void PreUpdate(Cur& cur);
};
