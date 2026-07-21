#include "WaveOutput.h"

WaveBlock::WaveBlock(int sampleCount) : samples(sampleCount) {
	waveHeader.lpData = (LPSTR)samples.data();
	waveHeader.dwBufferLength = 2 * sampleCount;
}
int WaveBlock::GetSampleCount() const { return samples.size(); }
bool WaveBlock::IsDone() const { return waveHeader.dwFlags & WHDR_DONE; }
bool WaveBlock::Play(HWAVEOUT waveOutputHandle) {
	auto ret0 = waveOutPrepareHeader(waveOutputHandle, &waveHeader, sizeof(waveHeader));
	auto ret1 = waveOutWrite(waveOutputHandle, &waveHeader, sizeof(waveHeader));
	return
		ret0 == MMSYSERR_NOERROR &&
		ret1 == MMSYSERR_NOERROR;
}
bool WaveBlock::Finish(HWAVEOUT waveOutputHandle) {
	return waveOutUnprepareHeader(waveOutputHandle, &waveHeader, sizeof(waveHeader)) == MMSYSERR_NOERROR;
}

WaveOutput::WaveOutput() {
	// 这里实时播放的处理我还不知道怎么做的更好。
	sampleRate = 44100; blockDuration = 5e-4;
	safeBlockCount = 120; blockCount = 512;
}
int WaveOutput::GetBlockSampleCount() const { return blockDuration * sampleRate; }
bool WaveOutput::Initialize() {
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 1;
	format.nSamplesPerSec = sampleRate;
	// 好像这里不能是其它数了，没怎么搞清楚。
	format.wBitsPerSample = 16;
	format.nBlockAlign = 2;
	format.nAvgBytesPerSec = 2 * sampleRate;

	auto ret = waveOutOpen(&waveOutputHandle, WAVE_MAPPER, &format, NULL, 0, CALLBACK_NULL);
	if (ret != MMSYSERR_NOERROR) { return false; }
	for (int i = (0), iEnd = int(blockCount); i < iEnd; ++i) {
		blocks.push_back(std::make_shared<WaveBlock>(GetBlockSampleCount()));
	} return true;
}
bool WaveOutput::Play() {
	while (playingBlockCount > 0 && blocks.front()->IsDone()) {
		bool ret = blocks.front()->Finish(waveOutputHandle);
		if (!ret) { return false; } playingBlockCount--;
		blocks.push_back(blocks.front());
		blocks.pop_front();
	} 
	
	int cur = 0;
	int n_in = max(0, safeBlockCount - playingBlockCount);
	for (auto v : inputSamples) {
		blocks[playingBlockCount]->samples[cur++] =
			clamp(int(v * 256 * 128), -128 * 256, 128 * 256 - 1);
		if (cur == GetBlockSampleCount()) {
			bool ret = blocks[playingBlockCount++]->Play(waveOutputHandle);
			if (!ret) { return false; } cur = 0;
		}
	} inputSamples = vector<double>(n_in * GetBlockSampleCount());
	return true;
}
void WaveOutput::Shutdown() {
	while (playingBlockCount > 0) {
		while (!blocks.front()->IsDone()) { Sleep(0); }
		blocks.front()->Finish(waveOutputHandle);
		blocks.pop_front(); playingBlockCount--;
	} waveOutClose(waveOutputHandle);
}

