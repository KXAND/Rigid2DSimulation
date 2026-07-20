#include "scene_parameter.h"

SceneParameter::SceneParameter(wstring const& nm) : nm(nm) {}
void SceneParameter::Save(FILE* f) {
	int sz = 0;
	fwtv(nm); fwt(val);
}
SceneParameter::SceneParameter(FILE* f) {
	int sz = 0;
	frdv(nm); frd(val);
}


