#include "MusicPlayer.h"

#include "AudioClip.h"
#include "Utils.h"

void MusicPlayer::Initialize()
{
    bool loaded = false;
    firstClip = std::make_shared<AudioClip>(L"sound/bgm0.clip", &loaded);
    if (!loaded)
    {
        firstClip.reset();
    }
    secondClip = std::make_shared<AudioClip>(L"sound/bgm1.clip", &loaded);
    if (!loaded)
    {
        secondClip.reset();
    }
    currentClip = GenerateRandomFloat(1) < 0.5 ? firstClip.get() : secondClip.get();
}

void MusicPlayer::Update(std::vector<double>& output)
{
    // 沿用两首均加载成功才播放的规则，不在职责迁移中改变缺失资源时的行为。
    if (!firstClip || !secondClip)
    {
        return;
    }
    if (currentClip->sampleIndex >= currentClip->GetSampleCount())
    {
        currentClip->sampleIndex = 0;
        currentClip = currentClip == firstClip.get() ? secondClip.get() : firstClip.get();
    }
    if (!isMuted)
    {
        currentClip->Play(output);
    }
}
