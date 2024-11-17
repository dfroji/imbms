#pragma once

#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>

#include "imgui.h"
#include "imgui-SFML.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

#include "state.h"

#include "filedialog.h"

#include "bms.h"
#include "parser.h"
#include "utils.h"

#include "menu_bar.h"
#include "side_menu.h"
#include "grid.h"
#include "notes.h"

class UI {
    public:
        UI();
        ~UI();

        void render();

        bool is_open();

        sf::Color get_channel_color(int channel_i);

    private:
        bool load_bms(fs::path filepath);
        bool save_bms(fs::path filepath);

        void limit_mouse();

        std::vector<std::string> get_play_channels();

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

        void update_state();

        State* state;
        MenuBar menu_bar_renderer;
        SideMenu side_menu_renderer;
        Grid grid_renderer;
        Notes notes_renderer;

        sf::RenderWindow* window;
        bool is_open_;
        sf::Font font;

        sf::Vector2i prev_mouse_pos;
        Note clicked_note;
        Note moved_note;
        bool is_moved_note_removed;
        bool is_moved_note_rendered;

        fVec2 grid_scale;
        iVec2 absolute_pos;
        int quantization;
        int keysound;

        std::vector<std::function<void()>> undo_list;

        BMS* bms;

        std::string filename;
        fs::path current_path;

        std::vector<std::thread> threads;
};
