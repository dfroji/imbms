#include "bms_parser.h"

#include <fstream>
#include <cmath>

BMS* ImBMS::parse_bms(std::string filename) {
    BMS* p_bms = new BMS(); 

    DataField current_field = DataField::field_header;
    std::ifstream file(filename);

    if (!file.is_open()) {
        delete p_bms;
        return nullptr;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> splits = split_line(line, " ", 2);
        if (splits.size() < 2) {continue;}

        std::string type = splits[0];
        std::string argument = splits[1];
        if (type == DATA_FIELD_TAG) {
            if (argument == "HEADER FIELD") {current_field = DataField::field_header;}
            else if (argument == "MAIN DATA FIELD") {current_field = DataField::field_main;}

        } else if (current_field == DataField::field_header) {
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
                } else if (tag_type == "BPM") {
                   p_bms->set_bpm_change(std::stod(argument), tag_index); 
                }
            }
        }
    }

    file.close();

    return p_bms;
}

void ImBMS::save_bms(std::string filename, BMS* p_bms) {

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
    return base36_to_int(sound_tag[1]);
}

std::vector<std::string> split_line(std::string line, const char* delimiter, int number_of_splits) {
    std::vector<std::string> splits;

    std::string split = "";
    for (const auto& c : line) {
        if (c != *delimiter || !splits.size() < number_of_splits - 1) {split.push_back(c);}
        else {
            splits.push_back(split);
            split = "";
        }
    }
    if (split != "") {splits.push_back(split);}

    return splits;
}

int base36_to_int(std::string number) {
    int dec = 0;
    int exp = number.length() - 1;
    for (auto& c : number) {
        if (c < 'A') {c += 'A' - '9' - 1;}
        dec += (c - '0' - ('A' - '9' - 1)) * std::pow(36, exp);
        exp -= 1;
    }

    return dec;
}
