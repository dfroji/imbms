#pragma once

#include <string>
#include <vector>

#include "utils.h"
#include "channels_and_colors.h"

const int DATA_LIMIT = ImBMS::base36_to_int("ZZ");
const int HEX_LIMIT = 256;

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
enum Difficulty {difficulty_null, easy, normal, hyper, another, insane};

enum Playstyle {SP, DP, PM};

class BMS {
public:
    BMS();
    ~BMS();

    std::string get_artist();
    void set_artist(std::string artist);

    std::string get_subartist();
    void set_subartist(std::string subartist);

    std::string get_title();
    void set_title(std::string title);

    std::string get_subtitle();
    void set_subtitle(std::string subtitle);

    double get_bpm();
    void set_bpm(double bpm);

    std::string get_genre();
    void set_genre(std::string genre);

    Player get_player();
    void set_player(Player player);

    Difficulty get_difficulty();
    void set_difficulty(Difficulty difficulty);

    Rank get_rank();
    void set_rank(Rank rank);

    double get_total();
    void set_total(double total);

    std::string get_stagefile();
    void set_stagefile(std::string stagefile);

    std::string get_banner();
    void set_banner(std::string banner);

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

        return this->artist != rhs->artist ||
            this->subartist != rhs->subartist ||
            this->title != rhs->title ||
            this->subtitle != rhs->subtitle ||
            this->bpm != rhs->bpm ||
            this->genre != rhs->genre ||
            this->player != rhs->player ||
            this->difficulty != rhs->difficulty ||
            this->rank != rhs->rank ||
            this->total != rhs->total ||
            this->stagefile != rhs->stagefile ||
            this->banner != rhs->banner ||
            this->playstyle != rhs->playstyle ||
            this->keysounds != rhs->keysounds ||
            this->graphics != rhs->graphics ||
            this->bpm_changes != rhs->bpm_changes;
    }

private:
    std::string artist;
    std::string subartist;
    std::string title;
    std::string subtitle;
    double bpm;
    std::string genre;
    Player player;
    Difficulty difficulty;
    Rank rank;
    double total;
    std::string stagefile;
    std::string banner;

    Playstyle playstyle;

    std::vector<std::string> keysounds;
    std::vector<std::string> graphics;
    std::vector<std::string> bpm_changes;

    std::vector<Measure*> measures;
};
