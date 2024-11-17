#pragma once

#include "soloud.h"
#include "soloud_wav.h"

#include <string>

class Audio {
    public:
        Audio();
        ~Audio();

        void play_sample(std::string filepath);
    private:
        SoLoud::Soloud audio_engine;
        SoLoud::Wav sample;
};
