#include "clip.h"

clip::clip(wstring const& nm, bool *ok) {
	FILE* f = wfopen(nm, L"rb");
	if (!f) { 
		if (ok) { *ok = false; }
		return; 
	}
	int sz = 0;
	fread(&(sp), sizeof(sp), 1, f);
	fread(&(replay), sizeof(replay), 1, f);
	fread(&(sz), sizeof(sz), 1, f);
	(vs).resize(sz);
	fread(&*(vs).begin(), sizeof(*(&*(vs).begin())), sz, f);
	fclose(f); 
	if (ok) { *ok = true; }
}
clip::clip(int sp) : sp(sp) {}
void clip::play(vector<double>& in, double vol) {
	for (auto& v : in) {
		if (csp >= n()) { return; }
		v += vs[csp] * vol;
		if (++csp == n() && replay) { csp = 0; }
	}
}

void clip::save(wstring const& nm) const {
	FILE* f = wfopen(nm, L"wb");
	if (!f) { return; } 
	int sz = 0;
	fwrite(&(sp), sizeof(sp), 1, f);
	fwrite(&(replay), sizeof(replay), 1, f);
	sz = (vs).size();
	fwrite(&(sz), sizeof(sz), 1, f);
	fwrite(&*(vs).begin(), sizeof(*(&*(vs).begin())), sz, f);
	fclose(f);
}

