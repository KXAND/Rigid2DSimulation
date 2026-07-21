#include "AudioClip.h"

AudioClip::AudioClip(wstring const& nm, bool *ok) {
	FILE* file = wfopen(nm, L"rb");
	if (!file) {
		if (ok) { *ok = false; }
		return;
	}
	int size = 0;
	fread(&(sp), sizeof(sp), 1, file);
	fread(&(replay), sizeof(replay), 1, file);
	fread(&(size), sizeof(size), 1, file);
	(vs).resize(size);
	fread(&*(vs).begin(), sizeof(*(&*(vs).begin())), size, file);
	fclose(file);
	if (ok) { *ok = true; }
}
AudioClip::AudioClip(int sp) : sp(sp) {}
void AudioClip::play(vector<double>& in, double vol) {
	for (auto& v : in) {
		if (csp >= n()) { return; }
		v += vs[csp] * vol;
		if (++csp == n() && replay) { csp = 0; }
	}
}

void AudioClip::save(wstring const& nm) const {
	FILE* file = wfopen(nm, L"wb");
	if (!file) { return; }
	int size = 0;
	fwrite(&(sp), sizeof(sp), 1, file);
	fwrite(&(replay), sizeof(replay), 1, file);
	size = (vs).size();
	fwrite(&(size), sizeof(size), 1, file);
	fwrite(&*(vs).begin(), sizeof(*(&*(vs).begin())), size, file);
	fclose(file);
}

