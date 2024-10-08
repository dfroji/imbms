#include "bms_parser.h"

#include <fstream>
#include <cmath>

#include <iostream>

#include "utils.h"

BMS* ImBMS::parse_bms(std::string filename) {
    BMS* p_bms = new BMS(); 
    p_bms->resize_measure_v(1);

    DataField current_field = DataField::field_header;
    std::ifstream file(filename);

    if (!file.is_open()) {
        delete p_bms;
        return nullptr;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (current_field == DataField::field_header) {
            std::vector<std::string> splits = split_line(line, " ", 2);
            if (splits.size() < 2 && current_field == DataField::field_header) {continue;}

            std::string type = splits[0];
            std::string argument = ImBMS::rtrim(splits[1]);
            if (type == DATA_FIELD_TAG) {
                if (argument == "MAIN DATA FIELD") {current_field = DataField::field_main;}

            } else {
                if (type == "#ARTIST") {p_bms->set_artist(argument);}
                else if (type == "#SUBARTIST") {p_bms->set_subartist(argument);}
                else if (type == "#TITLE") {p_bms->set_title(argument);}
                else if (type == "#SUBTITLE") {p_bms->set_subtitle(argument);}
                else if (type == "#BPM") {p_bms->set_bpm(std::stod(argument));}
                else if (type == "#GENRE") {p_bms->set_genre(argument);}
                else if (type == "#PLAYER") {p_bms->set_player(static_cast<Player>(std::stoi(argument)));}
                else if (type == "#DIFFICULTY") {p_bms->set_difficulty(static_cast<Difficulty>(std::stoi(argument)));}
                else if (type == "#RANK") {p_bms->set_rank(static_cast<Rank>(std::stoi(argument)));}
                else if (type == "#TOTAL") {p_bms->set_total(std::stod(argument));}
                else if (type == "#STAGEFILE") {p_bms->set_stagefile(argument);}
                else if (type == "#BANNER") {p_bms->set_banner(argument);}
                else {
                    std::string tag_type = get_tag_type(type);
                    int tag_index = get_tag_index(type);
                    if (AUDIO_FORMATS.contains(tag_type) && tag_index >= 0) {
                        p_bms->set_keysound(argument, tag_index);
                    } else if (tag_type == "BMP") {
                        p_bms->set_graphic(argument, tag_index);
                    }
                }
            }
        } else if (current_field == DataField::field_main) {
            parse_data(p_bms, line);
        }
    }

    file.close();

    if (p_bms->get_player() == Player::dp) {
        if (ImBMS::split_line(filename, ".", 2)[1] != "pms") {
            p_bms->set_playstyle(Playstyle::DP);
        } else {
            p_bms->set_playstyle(Playstyle::PM);
        }
    }

    return p_bms;
}

std::vector<std::string> parse_tag(std::string s) {
    if (s.length() != 6) {return INVALID_TAG;}
    else {
        std::string type = "";
        std::string index = "";
        for (int i = 1; i < 4; i++) {
            type.push_back(s[i]);
        }
        for (int i = 4; i < 6; i++) {
            index.push_back(s[i]);
        }

        if (!TAG_TYPES.contains(type)) {
            return INVALID_TAG;
        } else {
            return {type, index};
        }
    }
}

std::string get_tag_type(std::string s) {
    std::vector<std::string> sound_tag = parse_tag(s);
    return sound_tag[0];
}

int get_tag_index(std::string s) {
    std::vector<std::string> sound_tag = parse_tag(s);
    return ImBMS::base36_to_int(sound_tag[1]);
}

void parse_data(BMS* p_bms, std::string s) {
    std::vector<std::string> splits = ImBMS::split_line(s, ":", 2);
    if (splits.size() != 2) {}
    else {
        Channel* channel = parse_measure_and_channel(p_bms, splits[0]);
        channel->components = parse_components(splits[1]);
    }
}

Channel* parse_measure_and_channel(BMS* p_bms, std::string s) {
    std::string measure_index_str = "";
    std::string channel_index_str = "";
    for (int i = 1; i < 4; i++) {
        measure_index_str.push_back(s[i]);
    }
    for (int i = 4; i < 6; i++) {
        channel_index_str.push_back(s[i]);
    }

    int measure_index = std::stoi(measure_index_str);
    if (p_bms->get_measures().size() - 1 < measure_index) {
        p_bms->resize_measure_v(measure_index + 1);
    }
    
    if (p_bms->get_measures()[measure_index] == nullptr) {
        p_bms->new_measure(measure_index); 
    }

    Measure* measure = p_bms->get_measures()[measure_index];
    Channel* channel = nullptr;
    int channel_index = ImBMS::base36_to_int(channel_index_str);
    if (channel_index == 1) {
        channel = new Channel();
        measure->bgm_channels.push_back(channel);
    } else if (measure->channels[channel_index] == nullptr) {
        channel = new Channel();
        measure->channels[channel_index] = channel;
    } else {
        channel = measure->channels[channel_index];
    }

    return channel;

}

std::vector<int> parse_components(std::string s) {
    std::vector<int> components;
    for (int i = 0; i+1 < s.length(); i += 2) {
        std::string number = "";
        for (int j = i; j < i+2; j++) {
            number.push_back(s[j]);
        }
        components.push_back(ImBMS::base36_to_int(number));
    }
    return components;
}

