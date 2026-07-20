#pragma once
#include "framework/basic/utils.h"

struct SceneParameter {
	wstring nm;
	double val = 0;
	bool del = false;

	SceneParameter(wstring const& nm = {});
	void Save(FILE* f);
	SceneParameter(FILE* f);
};


