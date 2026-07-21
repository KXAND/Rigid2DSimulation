#include "AudioClip.h"

AudioClip::AudioClip(wstring const& fileName, bool *loaded) {
	FILE* file = wfopen(fileName, L"rb");
	if (!file) {
		if (loaded) { *loaded = false; }
		return;
	}
	int size = 0;
	fread(&(sampleRate), sizeof(sampleRate), 1, file);
	fread(&(repeat), sizeof(repeat), 1, file);
	fread(&(size), sizeof(size), 1, file);
	(samples).resize(size);
	fread(&*(samples).begin(), sizeof(*(&*(samples).begin())), size, file);
	fclose(file);
	if (loaded) { *loaded = true; }
}
AudioClip::AudioClip(int sampleRate) : sampleRate(sampleRate) {}
void AudioClip::Play(vector<double>& output, double volume) {
	for (auto& v : output) {
		if (sampleIndex >= GetSampleCount()) { return; }
		v += samples[sampleIndex] * volume;
		if (++sampleIndex == GetSampleCount() && repeat) { sampleIndex = 0; }
	}
}

void AudioClip::Save(wstring const& fileName) const {
	FILE* file = wfopen(fileName, L"wb");
	if (!file) { return; }
	int size = 0;
	fwrite(&(sampleRate), sizeof(sampleRate), 1, file);
	fwrite(&(repeat), sizeof(repeat), 1, file);
	size = (samples).size();
	fwrite(&(size), sizeof(size), 1, file);
	fwrite(&*(samples).begin(), sizeof(*(&*(samples).begin())), size, file);
	fclose(file);
}

