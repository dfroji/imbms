#pragma once

#include "state.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

using VRectangleShape = std::vector<sf::RectangleShape>;

class Background {
public:
    Background();
    ~Background();

    void render(State* state, sf::RenderWindow* window);
private:
    void render_play_channel_bgs(VRectangleShape& channel_bgs);
    void render_other_channel_bgs(VRectangleShape& channel_bgs);
    void render_channel_bg(VRectangleShape& channel_bgs, sf::Color color);

    std::vector<sf::Color> get_play_channel_colors();

    State* state;
};
