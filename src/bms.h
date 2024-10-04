#pragma once

#include <string>
#include <vector>

#include "utils.h"

const int DATA_LIMIT = ImBMS::base36_to_int("ZZ");

struct Channel {
    std::vector<int> components;
};

struct Measure {
    std::vector<Channel*> channels;
    std::vector<Channel*> bgm_channels;
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

    std::vector<double> get_bpm_changes();
    void set_bpm_change(double bpm, int index);

    std::vector<Measure*> get_measures();
    void new_measure(int index);
    void resize_measure_v(int new_size);

    Playstyle get_playstyle();
    void set_playstyle(Playstyle playstyle);

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
    std::vector<double> bpm_changes;

    std::vector<Measure*> measures;
};
