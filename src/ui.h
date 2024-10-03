#pragma once

#include "imgui.h"
#include "imgui-SFML.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Text.hpp"

#include "bms.h"
#include "bms_parser.h"

const int PADDING = 3;

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
