#pragma once

#include "SFML/Graphics/Color.hpp"

// channels
// *6 are scratches for 1p and 2p
const std::vector<std::string> P1_VISIBLE = {"16", "11", "12", "13", "14", "15", "18", "19"};
const std::vector<std::string> P2_VISIBLE = {"21", "22", "23", "24", "25", "28", "29", "26"};
const std::vector<std::string> PM_VISIBLE = {"11", "12", "13", "14", "15", "22", "23", "24", "25"};
const std::vector<std::string> BGA_CHANNELS = {"04", "07", "06"};

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
const sf::Color BGA_COLOR = sf::Color(0,255,0);
