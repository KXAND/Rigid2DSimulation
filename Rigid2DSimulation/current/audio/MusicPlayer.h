#pragma once
#include <memory>
#include <vector>

class AudioClip;

class MusicPlayer
{
public:
    bool isMuted = false;
    double volume = 1;

    void Initialize();
    void Update(std::vector<double>& output);

private:
    std::shared_ptr<AudioClip> firstClip, secondClip;
    AudioClip* currentClip = nullptr;
};
