#include "scene_parameter.h"

SceneParameter::SceneParameter(wstring const& name) : name(name) {}
void SceneParameter::Save(FILE* f) {
	int sz = 0;
	sz = (name).size();
	fwrite(&(sz), sizeof(sz), 1, f);
	fwrite(&*(name).begin(), sizeof(*(&*(name).begin())), sz, f);
	fwrite(&(value), sizeof(value), 1, f);
}
SceneParameter::SceneParameter(FILE* f) {
	int sz = 0;
	fread(&(sz), sizeof(sz), 1, f);
	(name).resize(sz);
	fread(&*(name).begin(), sizeof(*(&*(name).begin())), sz, f);
	fread(&(value), sizeof(value), 1, f);
}


