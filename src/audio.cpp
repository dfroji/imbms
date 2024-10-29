#include "audio.h"

#include <cstring>
#include <thread>
#include <chrono>

Audio::Audio() {
    this->audio_engine.init();
}

Audio::~Audio() {
}

void Audio::play_sample(std::string filepath) {
    this->sample.load(filepath.c_str());
    this->audio_engine.play(this->sample);
}
