#pragma once

#include <string>
#include <vector>
#include <map>

#include "utils.h"
#include "channels_and_colors.h"

const int DATA_LIMIT = ImBMS::base36_to_int("ZZ");
const int HEX_LIMIT = 256;

const int DEFAULT_BPM = 130;
const int DEFAULT_TOTAL = 300;

struct Channel {
    std::vector<int> components;
    void resize(int factor) {
        std::vector<int> v;
        for (const auto& comp : this->components) {
            v.push_back(comp);
            for (int i = 0; i < factor - 1; i++) {
                v.push_back(0);
            }
        }
        this->components = v;
    }
    std::string components_to_s() {
        std::string str = "";
        for (const auto& comp : this->components) {
            std::string fcomp = ImBMS::format_base36(comp, 2);
            for (const auto& c : fcomp) {
                str.push_back(c);
            }
        }
        return str;
    }
};

struct Measure {
    std::vector<Channel*> channels;
    std::vector<Channel*> bgm_channels;

    bool operator!=(Measure* rhs) {
        if (this->channels.size() != rhs->channels.size() ||
            this->bgm_channels.size() != rhs->bgm_channels.size()) {
            return true;
        }

        for (int i = 0; i < channels.size(); i++) {
            if (this->channels[i] == nullptr && rhs->channels[i] == nullptr) {
                continue;
            } else if (this->channels[i] == nullptr || rhs->channels[i] == nullptr) {
                return true;
            }

            if (this->channels[i]->components != rhs->channels[i]->components) {
                return true;
            }
        }

        for (int i = 0; i < bgm_channels.size(); i++) {
            if (this->bgm_channels[i] == nullptr && rhs->bgm_channels[i] == nullptr) {
                continue;
            } else if (this->bgm_channels[i] == nullptr || rhs->bgm_channels[i] == nullptr) {
                return true;
            }

            if (this->bgm_channels[i]->components != rhs->bgm_channels[i]->components) {
                return true;
            }
        }

        return false;
    }
};

enum Player {player_null, sp, couple, dp};
enum Rank {rank_vhard, rank_hard, rank_normal, rank_easy};
enum Difficulty {difficulty_null, beginner, normal, hyper, another, insane};

enum Playstyle {SP, DP, PM};

class BMS {
public:
    BMS();
    ~BMS();

    std::map<std::string, std::string> get_header_data();
    std::string get_header_data(std::string key);
    void insert_header_data(std::string key, std::string value);
    void remove_header_data(std::string key);

    std::vector<std::string> get_keysounds();
    void set_keysound(std::string filepath, int index);

    std::vector<std::string> get_graphics();
    void set_graphic(std::string filepath, int index);

    std::vector<std::string> get_bpm_changes();
    void set_bpm_change(std::string bpm, int index);

    std::vector<Measure*> get_measures();
    void new_measure(int index);
    void resize_measure_v(int new_size);

    Playstyle get_playstyle();
    void set_playstyle(Playstyle playstyle);

    std::vector<std::string> get_play_channels();

    BMS* copy();

    bool operator!=(BMS* rhs) {
        if (this->measures.size() != rhs->measures.size()) {
            return true;
        } else {
            for (int i = 0; i < this->measures.size(); i++) {
                if (this->measures[i] == nullptr && rhs->measures[i] == nullptr) {
                    continue;
                } else if (this->measures[i] == nullptr || rhs->measures[i] == nullptr) {
                    return true;
                }

                if (*(this->measures[i]) != rhs->measures[i]) {
                    return true;
                }
            }
        }

        return this->header_data != rhs->header_data;
    }

private:
    std::map<std::string, std::string> header_data;

    Playstyle playstyle;

    std::vector<std::string> keysounds;
    std::vector<std::string> graphics;
    std::vector<std::string> bpm_changes;

    std::vector<Measure*> measures;
};
