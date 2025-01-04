#pragma once

#include "state.h"

class MenuBar {
    public:
        MenuBar();
        ~MenuBar();

        void render(State* state, sf::RenderWindow* window);
    private:
        void discard_changes_popup(State* state, sf::RenderWindow* window);
};
