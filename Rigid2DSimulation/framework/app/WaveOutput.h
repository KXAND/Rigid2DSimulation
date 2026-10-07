#pragma once
#include "Utils.h"

// 这里反正能用，不知道有没有更好的实现方式。
class WaveBlock
{
public:
    WAVEHDR waveHeader{};
    vector<int16_t> samples;

    WaveBlock(int sampleCount = 0);
    int GetSampleCount() const;
    bool IsDone() const;
    bool Play(HWAVEOUT waveOutputHandle);
    bool Finish(HWAVEOUT waveOutputHandle);
};

class WaveOutput
{
public:
    HWAVEOUT waveOutputHandle{};
    int sampleRate = 0;
    int safeBlockCount = 0;
    int blockCount = 0;
    double blockDuration = 0;
    WAVEFORMATEX format{};

    int playingBlockCount = 0;
    vector<double> inputSamples;
    // 实践发现下面用指针是必要的, 否则涉及到复制会有错误，可能跟 winapi 有关。
    deque<std::shared_ptr<WaveBlock>> blocks;

    WaveOutput();
    int GetBlockSampleCount() const;
    bool Initialize();
    bool Play();
    void Shutdown();
};
