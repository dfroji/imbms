#pragma once

#include "imgui.h"
#include "imgui-SFML.h"

#include "SFML/Graphics/RenderWindow.hpp"

#include "state.h"
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

    private:
        State* state;
        MenuBar menu_bar_renderer;
        SideMenu side_menu_renderer;
        Grid grid_renderer;
        Notes notes_renderer;

        sf::RenderWindow* window;
        bool is_open_;

};
