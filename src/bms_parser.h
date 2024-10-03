#pragma once

#include <string>
#include <vector>
#include <set>

#include "bms.h"

const std::string DATA_FIELD_TAG = "*----------------------";
const std::set<std::string> TAG_TYPES = {"WAV", "OGG", "BMP", "BPM"};
const std::set<std::string> AUDIO_FORMATS = {"WAV", "OGG"};
const std::vector<std::string> INVALID_TAG = {"", "-1"};

enum DataField {field_header, field_main};

namespace ImBMS {
    BMS* parse_bms(std::string filename);
    void save_bms(std::string filename, BMS* p_bms);
}

std::vector<std::string> parse_tag(std::string s);
std::string get_tag_type(std::string s);
int get_tag_index(std::string s);

void parse_data(BMS* p_bms, std::string s);
Channel* parse_measure_and_channel(BMS* p_bms, std::string s);
std::vector<int> parse_components(std::string s);

std::vector<std::string> split_line(std::string line, const char* delimiter, int number_of_splits);

int base36_to_int(std::string number);
std::string int_to_base36(int number);
std::string format_base36(int number, int digits);
std::string rtrim(std::string s);
