#pragma once
#include "vector3.h"
#include "tile.h"

struct Cur;
struct Bgr {
	dVector2 topLeft;
	double depth = 0;
	tile black;
	int width = 0, height = 0;

	bool hovered = false;
	bool wheeled = false;

	Bgr(Cur& cur);
	dRect viewPort() const { return { topLeft, width, height }; }
	void render(Cur& cur);

	void Update(Cur& cur);
	void PreUpdate(Cur& cur);
};
