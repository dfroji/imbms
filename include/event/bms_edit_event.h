#pragma once

#include <vector>

#include "imgui.h"
#include "imgui-SFML.h"

#include "bms.h"
#include "state.h"
#include "channels_and_colors.h"

class BMSEditEvent {
    public:
        static bool add_note(int component, sf::Vector2i mouse_pos, State* state);
        static void remove_note(sf::Vector2i mouse_pos, State* state);
        static void move_notes(sf::Vector2i mouse_pos, State* state);
        static void remove_selected_notes(State* state); 
    private:
        static bool add_play_or_bga_note(int component, sf::Vector2i mouse_pos, int measure_i, int channel_i, int cell, State* state);
        static bool add_bgm_note(int component, sf::Vector2i mouse_pos, int measure_i, int channel_i, int cell, State* state);
        static void undo_add_note(Channel* channel, std::vector<int> old_components, State* state);
        static void redo_add_note(Channel* channel, std::vector<int> components, State* state);

        static void undo_remove_note(Channel* channel, int component_i, int component, State* state);
        static void redo_remove_note(Channel* channel, int component_i, int component, State* state);

        static void undo_move_notes(int moved_notes, State* state);
        static void redo_move_notes(int moved_notes, State* state);
};
