#include "WaveOutput.h"

WaveBlock::WaveBlock(int sampleCount) : samples(sampleCount) {
	waveHeader.lpData = (LPSTR)samples.data();
	waveHeader.dwBufferLength = 2 * sampleCount;
}
int WaveBlock::GetSampleCount() const { return samples.size(); }
bool WaveBlock::IsDone() const { return waveHeader.dwFlags & WHDR_DONE; }
bool WaveBlock::Play(HWAVEOUT waveOutputHandle) {
	auto prepareResult = waveOutPrepareHeader(waveOutputHandle, &waveHeader, sizeof(waveHeader));
	auto writeResult = waveOutWrite(waveOutputHandle, &waveHeader, sizeof(waveHeader));
	return
		prepareResult == MMSYSERR_NOERROR &&
		writeResult == MMSYSERR_NOERROR;
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

	auto openResult = waveOutOpen(&waveOutputHandle, WAVE_MAPPER, &format, NULL, 0, CALLBACK_NULL);
	if (openResult != MMSYSERR_NOERROR) { return false; }
	for (int i = (0), iEnd = int(blockCount); i < iEnd; ++i) {
		blocks.push_back(std::make_shared<WaveBlock>(GetBlockSampleCount()));
	} return true;
}
bool WaveOutput::Play() {
	while (playingBlockCount > 0 && blocks.front()->IsDone()) {
		bool finished = blocks.front()->Finish(waveOutputHandle);
		if (!finished) { return false; } playingBlockCount--;
		blocks.push_back(blocks.front());
		blocks.pop_front();
	} 
	
	int blockSampleIndex = 0;
	int availableBlockCount = max(0, safeBlockCount - playingBlockCount);
	for (auto sampleValue : inputSamples) {
		blocks[playingBlockCount]->samples[blockSampleIndex++] =
			clamp(int(sampleValue * 256 * 128), -128 * 256, 128 * 256 - 1);
		if (blockSampleIndex == GetBlockSampleCount()) {
			bool played = blocks[playingBlockCount++]->Play(waveOutputHandle);
			if (!played) { return false; } blockSampleIndex = 0;
		}
	} inputSamples = vector<double>(availableBlockCount * GetBlockSampleCount());
	return true;
}
void WaveOutput::Shutdown() {
	while (playingBlockCount > 0) {
		while (!blocks.front()->IsDone()) { Sleep(0); }
		blocks.front()->Finish(waveOutputHandle);
		blocks.pop_front(); playingBlockCount--;
	} waveOutClose(waveOutputHandle);
}

