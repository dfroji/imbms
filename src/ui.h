#pragma once

#include <string>
#include <vector>

#include "imgui.h"
#include "imgui-SFML.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Text.hpp"

#include "bms.h"
#include "bms_parser.h"
#include "utils.h"

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

const int PADDING = 3;
const int DEFAULT_SCALING_DIV = 20;

struct iVec2 {
    int x;
    int y;
};

struct fVec2 {
    float x;
    float y;
};

class UI {
public:
    UI();
    ~UI();

    void render();

    bool is_open();

private:
    void render_main_menu_bar();
    void render_side_section();
    void render_grid();
    void render_notes();
    void render_channels(int measure_index, std::vector<std::string> channels, std::vector<sf::Color> colors);
    void render_bgm_channels(int measure_index, int offset);
    void render_bga_channels(int measure_index, std::vector<std::string> channels, int offset);
    void render_channel_notes(int measure_index, int channel_index, std::vector<int> components, sf::Color color);

    void calculate_values();

    bool load_bms(std::string filename);

    sf::RenderWindow* window;
    bool is_open_;
    sf::Font font;

    bool is_shift_pressed;

    fVec2 grid_scale;
    iVec2 absolute_pos;
    int quantization;
    int measure_length;

    // these are calculated by calculate_values()
    ImVec2 viewport_size;
    ImVec2 viewport_pos;
    fVec2 default_scaling;
    int visible_measures;
    int measures_wrapped;
    fVec2 relative_pos;
    iVec2 wraps;
    fVec2 wrapping_offset;

    BMS* bms;

};
