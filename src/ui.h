#pragma once

#include "imgui.h"
#include "imgui-SFML.h"

#include "SFML/Graphics/RenderWindow.hpp"

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

    sf::RenderWindow* window;
    bool is_open_;

    fVec2 grid_scale;
    int position;
    int quantization;
    int measure_length;
};
