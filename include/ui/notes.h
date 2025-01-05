#pragma once

#include <vector>
#include <thread>
#include <mutex>

#include "state.h"
#include "bms.h"
#include "channels_and_colors.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

using VRectangleShape = std::vector<sf::RectangleShape>;
using VText = std::vector<sf::Text>;

const int NOTE_HEIGHT = 10;
const int SELECTION_OUTLINE_THICKNESS = 2;

class Notes {
    public:
        Notes();
        ~Notes();

        void render(State* state, sf::RenderWindow* window, sf::Vector2i mouse_pos);
    private:
        void render_thread(int measure_i, std::vector<std::string> channels, State* state);
        void render_play_channels(int measure_i, std::vector<std::string> channels, State* state);
        void render_bgm_channels(int measure_i, int offset, State* state);
        void render_bga_channels(int measure_i, std::vector<std::string> channels, int offset, State* state);
        void render_channel_notes(int measure_i, int channel_i, Channel* channel, sf::Color color, State* state);
        void render_moving_selection(State* state, sf::RenderWindow* window, sf::Vector2i mouse_pos);

        sf::Color get_channel_color(int channel_i, State* state);
        std::string get_note_label(int component, int channel_i, State* state);

        VRectangleShape notes_render_v;
        VText labels_render_v;
        std::mutex notes_m;
        std::mutex labels_m;

        VRectangleShape moving_notes_render_v;
        VText moving_labels_render_v;
};
