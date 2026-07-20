#include "scene_parameter.h"

SceneParameter::SceneParameter(wstring const& name) : name(name) {}
void SceneParameter::Save(FILE* f) {
	int sz = 0;
	fwtv(name); fwt(value);
}
SceneParameter::SceneParameter(FILE* f) {
	int sz = 0;
	frdv(name); frd(value);
}


