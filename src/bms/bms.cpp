#include "bms.h"

BMS::BMS() {
    header_data = {};
    insert_header_data("#BPM", std::to_string(DEFAULT_BPM));
    insert_header_data("#PLAYER", std::to_string(Player::sp));
    insert_header_data("#DIFFICULTY", std::to_string(Difficulty::beginner));
    insert_header_data("#RANK", std::to_string(Rank::rank_easy));
    insert_header_data("#TOTAL", std::to_string(DEFAULT_TOTAL));
    insert_header_data("#PLAYLEVEL", "1");

    playstyle = Playstyle::SP;

    keysounds.resize(DATA_LIMIT, "");
    graphics.resize(DATA_LIMIT, "");
    bpm_changes.resize(DATA_LIMIT, "");

    resize_measure_v(1);
}

BMS::~BMS() {
    for (auto measure : measures) {
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

std::map<std::string, std::string> BMS::get_header_data() {
    return header_data;
}

std::string BMS::get_header_data(std::string key) {
    if (auto search = header_data.find(key); search != header_data.end()) {
        return search->second;

    } else {
        return "";
    }
}

void BMS::insert_header_data(std::string key, std::string value) {
    // value is not inserted if the value is empty and the key does not exists in the map
    if (value == "") {
        if (auto search = header_data.find(key); search == header_data.end()) {
            return;
        }
    }

    header_data.insert_or_assign(key, value);
}

std::vector<std::string> BMS::get_keysounds() {
    return keysounds;
}

void BMS::set_keysound(std::string filepath, int index) {
    keysounds[index] = filepath;
}

std::vector<std::string> BMS::get_graphics() {
    return graphics;
}

void BMS::set_graphic(std::string filepath, int index) {
    graphics[index] = filepath;
}

std::vector<Measure*> BMS::get_measures() {
    return measures;
}

std::vector<std::string> BMS::get_bpm_changes() {
    return bpm_changes;
}

void BMS::set_bpm_change(std::string bpm, int index) {
    bpm_changes[index] = ImBMS::trim_dstr(bpm);
}
void BMS::new_measure(int index) {
    Measure* to_be_deleted = measures[index];
    measures[index] = new Measure();
    measures[index]->channels.resize(DATA_LIMIT, nullptr);

    if (to_be_deleted != nullptr) {delete to_be_deleted;}
}

void BMS::resize_measure_v(int new_size) {
    measures.resize(new_size, nullptr);
}

Playstyle BMS::get_playstyle() {
    return playstyle;
}

void BMS::set_playstyle(Playstyle playstyle) {
    this->playstyle = playstyle;

    Player plr = Player::player_null;
    switch (playstyle) {
        case Playstyle::SP:
            plr = Player::sp;
            break;
        case Playstyle::DP:
            plr = Player::dp;
            break;
        case Playstyle::PM:
            plr = Player::dp;
            break;
        default:
            return;
    }
    insert_header_data("#PLAYER", std::to_string(plr));
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

    copy->header_data = header_data;

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
