#include "SceneParameter.h"

SceneParameter::SceneParameter(wstring const& name) : name(name) {}
void SceneParameter::Save(FILE* file) {
	int size = 0;
	size = (name).size();
	fwrite(&(size), sizeof(size), 1, file);
	fwrite(&*(name).begin(), sizeof(*(&*(name).begin())), size, file);
	fwrite(&(value), sizeof(value), 1, file);
}
SceneParameter::SceneParameter(FILE* file) {
	int size = 0;
	fread(&(size), sizeof(size), 1, file);
	(name).resize(size);
	fread(&*(name).begin(), sizeof(*(&*(name).begin())), size, file);
	fread(&(value), sizeof(value), 1, file);
}


