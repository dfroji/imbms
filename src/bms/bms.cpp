#include "bms.h"

BMS::BMS() {
    this->artist = "";
    this->subartist = "";
    this->title = "";
    this->subtitle = "";
    this->bpm = DEFAULT_BPM;
    this->genre = "";
    this->player = Player::sp;
    this->difficulty = Difficulty::easy;
    this->rank = Rank::rank_easy;
    this->total = DEFAULT_TOTAL;
    this->stagefile = "";
    this->banner = "";

    this->playstyle = Playstyle::SP;

    this->keysounds.resize(DATA_LIMIT, "");
    this->graphics.resize(DATA_LIMIT, "");
    this->bpm_changes.resize(DATA_LIMIT, "");

    resize_measure_v(1);
}

BMS::~BMS() {
    for (auto measure : this->measures) {
        if (measure == nullptr) {continue;}
        for (auto channel : measure->channels) {
            if (channel == nullptr) {continue;}
            delete channel;
        }
        for (auto bgm_channel : measure->bgm_channels) {
            delete bgm_channel;
        }
        delete measure;
    }
}

std::string BMS::get_artist() {
    return this->artist;
}

void BMS::set_artist(std::string artist) {
    this->artist = artist;
}

std::string BMS::get_subartist() {
    return this->subartist;
}

void BMS::set_subartist(std::string subartist) {
    this->subartist = subartist;
}

std::string BMS::get_title() {
    return this->title;
}

void BMS::set_title(std::string title) {
    this->title = title;
}

std::string BMS::get_subtitle() {
    return this->subtitle;
}

void BMS::set_subtitle(std::string subtitle) {
    this->subtitle = subtitle;
}

double BMS::get_bpm() {
    return this->bpm;
}

void BMS::set_bpm(double bpm) {
    this->bpm = bpm;
}

std::string BMS::get_genre() {
    return this->genre;
}

void BMS::set_genre(std::string genre) {
    this->genre = genre;
}

Player BMS::get_player() {
    return this->player;
}

void BMS::set_player(Player player) {
    this->player = player;
}

Difficulty BMS::get_difficulty() {
    return this->difficulty;
}

void BMS::set_difficulty(Difficulty difficulty) {
    this->difficulty = difficulty;
}

Rank BMS::get_rank() {
    return this->rank;
}

void BMS::set_rank(Rank rank) {
    this->rank = rank;
}

double BMS::get_total() {
    return this->total;
}

void BMS::set_total(double total) {
    this->total = total;
}

std::string BMS::get_stagefile() {
    return this->stagefile;
}

void BMS::set_stagefile(std::string stagefile) {
    this->stagefile = stagefile;
}

std::string BMS::get_banner() {
    return this->banner;
}

void BMS::set_banner(std::string banner) {
    this->banner = banner;
}

std::vector<std::string> BMS::get_keysounds() {
    return this->keysounds;
}

void BMS::set_keysound(std::string filepath, int index) {
    this->keysounds[index] = filepath;
}

std::vector<std::string> BMS::get_graphics() {
    return this->graphics;
}

void BMS::set_graphic(std::string filepath, int index) {
    this->graphics[index] = filepath;
}

std::vector<Measure*> BMS::get_measures() {
    return this->measures;
}

std::vector<std::string> BMS::get_bpm_changes() {
    return this->bpm_changes;
}

void BMS::set_bpm_change(std::string bpm, int index) {
    this->bpm_changes[index] = ImBMS::trim_dstr(bpm);
}
void BMS::new_measure(int index) {
    Measure* to_be_deleted = this->measures[index];
    this->measures[index] = new Measure();
    this->measures[index]->channels.resize(DATA_LIMIT, nullptr);

    if (to_be_deleted != nullptr) {delete to_be_deleted;}
}

void BMS::resize_measure_v(int new_size) {
    this->measures.resize(new_size, nullptr);
}

Playstyle BMS::get_playstyle() {
    return this->playstyle;
}

void BMS::set_playstyle(Playstyle playstyle) {
    this->playstyle = playstyle;
}

std::vector<std::string> BMS::get_play_channels() {
    std::vector<std::string> play_channels;
    switch(playstyle) {
        case Playstyle::SP:
            play_channels = P1_VISIBLE;
            break;
        case Playstyle::DP:
            play_channels = P1_VISIBLE;
            play_channels.insert(std::end(play_channels), std::begin(P2_VISIBLE), std::end(P2_VISIBLE));
            break;
        case Playstyle::PM:
            play_channels = PM_VISIBLE;
            break;
    }

    return play_channels;
}

// creates a copy of this
// couldn't be arsed to write a cleaner implementation
BMS* BMS::copy() {
    BMS* copy = new BMS();
    copy->measures.clear();

    copy->artist = artist;
    copy->subartist = subartist;
    copy->title = title;
    copy->subtitle = subtitle;
    copy->bpm = bpm;
    copy->genre = genre;
    copy->player = player;
    copy->difficulty = difficulty;
    copy->rank = rank;
    copy->total = total;
    copy->stagefile = stagefile;
    copy->banner = banner;
    copy->playstyle = playstyle;
    copy->keysounds = keysounds;
    copy->graphics = graphics;
    copy->bpm_changes = bpm_changes;

    // copy measures in a loop
    for (auto m : measures) {
        if (m == nullptr) {
            copy->measures.push_back(nullptr);
        } else {
            Measure* new_measure = new Measure();

            // copy channels to the currently processed measure
            for (int i = 0; i < m->channels.size(); i++) {
                if (m->channels[i] == nullptr) {
                    new_measure->channels.push_back(nullptr);
                } else {
                    Channel* new_channel = new Channel();
                    new_channel->components = m->channels[i]->components;
                    new_measure->channels.push_back(new_channel);
                }
            }

            // copy bgm channels to the currently processed measure
            for (int i = 0; i < m->bgm_channels.size(); i++) {
                if (m->bgm_channels[i] == nullptr) {
                    new_measure->bgm_channels.push_back(nullptr);
                } else {
                    Channel* new_channel = new Channel();
                    new_channel->components = m->bgm_channels[i]->components;
                    new_measure->bgm_channels.push_back(new_channel);
                }
            }

            copy->measures.push_back(new_measure);
        }
    }

    return copy;
}
