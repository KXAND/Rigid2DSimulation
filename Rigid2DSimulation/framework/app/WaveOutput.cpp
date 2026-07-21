#include "WaveOutput.h"

WaveBlock::WaveBlock(int n) : dat(n) {
	hdr.lpData = (LPSTR)dat.data();
	hdr.dwBufferLength = 2 * n;
}
int WaveBlock::n() const { return dat.size(); }
bool WaveBlock::done() const { return hdr.dwFlags & WHDR_DONE; }
bool WaveBlock::play(HWAVEOUT hwo) {
	auto ret0 = waveOutPrepareHeader(hwo, &hdr, sizeof(hdr));
	auto ret1 = waveOutWrite(hwo, &hdr, sizeof(hdr));
	return
		ret0 == MMSYSERR_NOERROR &&
		ret1 == MMSYSERR_NOERROR;
}
bool WaveBlock::finish(HWAVEOUT hwo) {
	return waveOutUnprepareHeader(hwo, &hdr, sizeof(hdr)) == MMSYSERR_NOERROR;
}

WaveOutput::WaveOutput() {
	// 这里实时播放的处理我还不知道怎么做的更好。
	sp = 44100; len_block = 5e-4;
	n_safe = 120; n_blocks = 512;
}
int WaveOutput::len_block_n() const { return len_block * sp; }
bool WaveOutput::init() {
	wofm.wFormatTag = WAVE_FORMAT_PCM;
	wofm.nChannels = 1;
	wofm.nSamplesPerSec = sp;
	// 好像这里不能是其它数了，没怎么搞清楚。
	wofm.wBitsPerSample = 16;
	wofm.nBlockAlign = 2;
	wofm.nAvgBytesPerSec = 2 * sp;

	auto ret = waveOutOpen(&hwo, WAVE_MAPPER, &wofm, NULL, 0, CALLBACK_NULL);
	if (ret != MMSYSERR_NOERROR) { return false; }
	for (int i = (0), iEnd = int(n_blocks); i < iEnd; ++i) {
		wvblks.push_back(std::make_shared<WaveBlock>(len_block_n()));
	} return true;
}
bool WaveOutput::play() {
	while (n_play > 0 && wvblks.front()->done()) {
		bool ret = wvblks.front()->finish(hwo);
		if (!ret) { return false; } n_play--;
		wvblks.push_back(wvblks.front());
		wvblks.pop_front();
	} 
	
	int cur = 0;
	int n_in = max(0, n_safe - n_play);
	for (auto v : wvin) {
		wvblks[n_play]->dat[cur++] =
			clamp(int(v * 256 * 128), -128 * 256, 128 * 256 - 1);
		if (cur == len_block_n()) {
			bool ret = wvblks[n_play++]->play(hwo);
			if (!ret) { return false; } cur = 0;
		}
	} wvin = vector<double>(n_in * len_block_n());
	return true;
}
void WaveOutput::exit() {
	while (n_play > 0) {
		while (!wvblks.front()->done()) { Sleep(0); }
		wvblks.front()->finish(hwo);
		wvblks.pop_front(); n_play--;
	} waveOutClose(hwo);
}

