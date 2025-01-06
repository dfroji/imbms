#pragma once

#include <vector>
#include <map>

#include "SFML/Graphics/Color.hpp"

// channels
// *6 are scratches for 1p and 2p
const std::vector<std::string> P1_VISIBLE = {"16", "11", "12", "13", "14", "15", "18", "19"};
const std::vector<std::string> P2_VISIBLE = {"21", "22", "23", "24", "25", "28", "29", "26"};
const std::vector<std::string> PM_VISIBLE = {"11", "12", "13", "14", "15", "22", "23", "24", "25"};
const std::vector<std::string> OTHER_CHANNELS = {"03", "08", "04", "07", "06"};
const std::vector<std::string> BPM_CHANNELS = {"03", "08"};
const std::vector<std::string> BGA_CHANNELS = {"04", "07", "06"};

const std::map<std::string, std::string> SP_LABELS = {
    {"16", "SCR"},
    {"11", "1"},
    {"12", "2"},
    {"13", "3"},
    {"14", "4"},
    {"15", "5"},
    {"18", "6"},
    {"19", "7"},
};

const std::map<std::string, std::string> DP_LABELS = {
    {"16", "P1 SCR"},
    {"11", "P1 1"},
    {"12", "P1 2"},
    {"13", "P1 3"},
    {"14", "P1 4"},
    {"15", "P1 5"},
    {"18", "P1 6"},
    {"19", "P1 7"},
    {"21", "P2 1"},
    {"22", "P2 2"},
    {"23", "P2 3"},
    {"24", "P2 4"},
    {"25", "P2 5"},
    {"28", "P2 6"},
    {"29", "P2 7"},
    {"26", "P2 SCR"}
};

const std::map<std::string, std::string> PM_LABELS = {
    {"11", "1"},
    {"12", "2"},
    {"13", "3"},
    {"14", "4"},
    {"15", "5"},
    {"22", "6"},
    {"23", "7"},
    {"24", "8"},
    {"25", "9"},
};

const std::map<std::string, std::string> OTHER_LABELS = {
    {"03", "BPM"},
    {"08", "exBPM"},
    {"04", "BGA"},
    {"07", "OVERLAY"},
    {"06", "POOR"}
};

// colors
const sf::Color SCRATCH_COLOR = sf::Color(255,0,0);
const sf::Color BM_BOTTOM_NOTE_COLOR = sf::Color(255,255,255);
const sf::Color BM_TOP_NOTE_COLOR = sf::Color(0, 150, 255);
const sf::Color PM_WHITE_COLOR = sf::Color(255,255,255);
const sf::Color PM_YELLOW_COLOR = sf::Color(255,255,0);
const sf::Color PM_GREEN_COLOR = sf::Color(0,255,0);
const sf::Color PM_BLUE_COLOR = sf::Color(0,0,255);
const sf::Color PM_RED_COLOR = sf::Color(255,0,0);
const sf::Color BGM_COLOR = sf::Color(255,0,0);
const sf::Color OTHER_COLOR = sf::Color(0,255,0);
const sf::Color SELECTION_OUTLINE_COLOR = sf::Color(255,100,100);
const sf::Color DIMMING_COLOR = {255, 255, 255, 30};
