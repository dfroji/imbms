#pragma once

#include <string>
#include <vector>
#include <functional>

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

struct Note {
    Channel* channel = nullptr;
    int component_i = -1;
    int component = -1;
    bool operator==(const Note& rhs) {
        return (this->channel == rhs.channel && this->component_i == rhs.component_i);
    }
    bool operator!=(const Note& rhs) {
        return (this->channel != rhs.channel && this->component_i != rhs.component_i);
    }
};
const Note NULL_NOTE = Note(nullptr, -1, -1);

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
    void render_play_channels(int measure_index, std::vector<std::string> channels);
    void render_bgm_channels(int measure_index, int offset);
    void render_bga_channels(int measure_index, std::vector<std::string> channels, int offset);
    void render_channel_notes(int measure_index, int channel_index, std::vector<int> components, sf::Color color);

    void calculate_values();

    bool load_bms(std::string filename);

    void limit_mouse();

    std::vector<std::string> get_play_channels();

    sf::Vector2i get_mouse_pos();
    int get_pointed_measure(sf::Vector2i mouse_pos);
    int get_pointed_channel(sf::Vector2i mouse_pos);
    int get_pointed_cell(sf::Vector2i mouse_pos);

    Note get_pointed_note();
    void render_moved_note();

    void undo();

    bool add_note(int component);
    bool add_play_or_bga_note(int component, sf::Vector2i mouse_pos, int measure_i, int channel_i, int cell);
    bool add_bgm_note(int component, sf::Vector2i mouse_pos, int measure_i, int channel_i, int cell);
    static void undo_add_note(Measure* measure, Channel* channel, int channel_i, std::vector<int> components);

    void remove_note(Note note);
    static void undo_remove_note(Channel* channel, int component_i, int component);

    static void undo_move_note(std::vector<std::function<void()>>* undo_list);

    sf::Color get_channel_color(int channel_i);

    std::vector<std::string> get_keysound_labels(int size, int digits);

    sf::RenderWindow* window;
    bool is_open_;
    sf::Font font;

    bool is_lshift_pressed;
    bool is_lcontrol_pressed;
    bool is_mouse1_held;

    sf::Vector2i prev_mouse_pos;
    Note clicked_note;
    Note moved_note;
    bool is_moved_note_removed;
    bool is_moved_note_rendered;

    fVec2 grid_scale;
    iVec2 absolute_pos;
    int quantization;
    int measure_length;
    int keysound;

    // these are calculated by calculate_values()
    ImVec2 viewport_size;
    ImVec2 viewport_pos;
    fVec2 default_scaling;
    int visible_measures;
    int measures_wrapped;
    fVec2 relative_pos;
    iVec2 wraps;
    fVec2 wrapping_offset;
    
    std::vector<std::function<void()>> undo_list;

    BMS* bms;

};
