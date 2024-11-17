#pragma once

#include "state.h"

#include "SFML/Graphics/RenderWindow.hpp"

const sf::Color LINE_COLOR(50, 50, 50);
const sf::Color BEAT_COLOR(100, 100, 100);
const sf::Color MEASURE_COLOR(255, 255, 255);

class Grid {
    public:
        Grid();
        ~Grid();

        void render(State* state, sf::RenderWindow* window);
    private:
};
