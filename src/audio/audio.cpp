#include "audio.h"

#include <cstring>
#include <thread>
#include <chrono>

Audio::Audio() {
    audio_engine.init();
}

Audio::~Audio() {
    audio_engine.deinit();
}

void Audio::play_sample(std::string filepath) {
    sample.load(filepath.c_str());
    audio_engine.play(sample);
}
