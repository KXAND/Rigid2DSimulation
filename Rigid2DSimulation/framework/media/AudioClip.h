#pragma once
#include "Utils.h"

struct AudioClip {
	int sampleRate = 0;
	int sampleIndex = 0;
	bool repeat = false;
	vector<double> samples;

	AudioClip(wstring const& fileName, bool* loaded = NULL);
	AudioClip(int sampleRate = 0);
	int GetSampleCount() const { return samples.size(); }
	void Play(vector<double>& output, double volume = 1);
	void Save(wstring const& fileName) const;
};

