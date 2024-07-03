#include "ui.h"

#include <iostream>

#include "SFML/Window/Event.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

const sf::Color LINE_COLOR(50, 50, 50);
const sf::Color BEAT_COLOR(100, 100, 100);
const sf::Color MEASURE_COLOR(255, 255, 255);

UI::UI() {
    window = new sf::RenderWindow(sf::VideoMode(640, 480), "ImBMS");
    window->setFramerateLimit(60);
    ImGui::SFML::Init(*window);

    this->is_open_ = window->isOpen();

    this->grid_scale = {1.2f, 1.0f};
    this->position = 0;

    this->quantization = 16;
    this->measure_length = 4;
}

UI::~UI() {
    ImGui::SFML::Shutdown();

    delete window;
}

void UI::render() {
    sf::Event event;
    sf::Clock delta_clock;

    while (window->pollEvent(event)) {
        ImGui::SFML::ProcessEvent(*window, event);

        if (event.type == sf::Event::Closed) {
            window->close();
        }

        if (event.type == sf::Event::MouseWheelScrolled) {
            this->position -= event.mouseWheelScroll.delta;
        }
    }

    ImGui::SFML::Update(*window, delta_clock.restart());

    this->is_open_ = window->isOpen();

    window->clear();

    this->render_main_menu_bar();
    this->render_side_section();
    this->render_grid();

    ImGui::SFML::Render(*window);
    window->display();
}

bool UI::is_open() {
    return this->is_open_; 
}

  ///////////////
 /// PRIVATE ///
///////////////

void UI::render_main_menu_bar() {

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("File")) {

    }

    if (ImGui::BeginMenu("Edit")) {

    }
    
    if (ImGui::BeginMenu("View")) {

    }

    ImGui::EndMainMenuBar();
}

void UI::render_side_section() {
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;

    ImVec2 viewport_pos = ImGui::GetMainViewport()->GetWorkCenter();
    ImVec2 viewport_size = ImGui::GetMainViewport()->WorkSize;

    ImVec2 size;
    size.x = viewport_size.x * 0.2f;
    size.y = viewport_size.y;
    ImGui::SetNextWindowSize(size);

    ImVec2 pos;
    pos.x = viewport_pos.x + viewport_size.x / 2 - size.x;
    pos.y = viewport_pos.y - viewport_size.y / 2;
    ImGui::SetNextWindowPos(pos);

    ImGui::SetNextWindowBgAlpha(1.0f);

    ImGui::Begin("Side section", &this->is_open_, window_flags);

    if (ImGui::CollapsingHeader("Metadata")) {
        static char title[1024] = "", artist[1024] = "", genre[1024] = "";
        ImGui::InputText("Title", title, IM_ARRAYSIZE(title));
        ImGui::InputText("Artist", title, IM_ARRAYSIZE(title));
        ImGui::InputText("Genre", title, IM_ARRAYSIZE(title));

        const char* modes[] = {"SP", "DP", "PM"};
        static int current_mode = 0;
        ImGui::Combo("Mode", &current_mode, modes, IM_ARRAYSIZE(modes));

        static int bpm = 0, total = 300;
        ImGui::InputInt("BPM", &bpm);
        ImGui::InputInt("Total", &total);

        const char* judges[] = {"Easy", "Normal", "Hard", "Very Hard"};
        static int current_judge = 0;
        ImGui::Combo("Judge", &current_judge, judges, IM_ARRAYSIZE(judges));

    }

    if (ImGui::CollapsingHeader("Keysounds")) {

    }

    if (ImGui::CollapsingHeader("Grid")) {
        ImGui::InputInt("Beats per measure", &this->measure_length);
        ImGui::InputInt("Quantization", &this->quantization);
    }

    ImGui::End();
}

void UI::render_grid() {
    ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
    ImVec2 viewport_pos = ImGui::GetMainViewport()->WorkPos;

    int default_size = 8;

    int rows = viewport_size.y / (default_size * this->grid_scale.y);
    int cols = viewport_size.x / (default_size * this->grid_scale.x);

    sf::VertexArray grid_cols(sf::Lines, 2*cols);
    for (int i = 0; i < cols; i += 2) {
        grid_cols[i].position = {i*default_size*this->grid_scale.x, viewport_pos.y};
        grid_cols[i+1].position = {i*default_size*this->grid_scale.x, viewport_size.y + viewport_pos.y};

        grid_cols[i].color = LINE_COLOR;
        grid_cols[i+1].color = LINE_COLOR;
    }

    sf::VertexArray grid_rows(sf::Lines, 2*rows);
    for (int i = 0; i < rows; i += 2) {
        grid_rows[i].position = {viewport_pos.x, i*default_size*this->grid_scale.y + viewport_pos.y};         
        grid_rows[i+1].position = {viewport_size.x, i*default_size*this->grid_scale.y + viewport_pos.y};

        if ((i + this->position*2) % (this->quantization*2) == 0) {
            grid_rows[i].color = MEASURE_COLOR;
            grid_rows[i+1].color = MEASURE_COLOR;               
        } else if ((i + this->position*2) % ((this->quantization/this->measure_length)*2) == 0) {
            grid_rows[i].color = BEAT_COLOR;
            grid_rows[i+1].color = BEAT_COLOR;
        } else {
            grid_rows[i].color = LINE_COLOR;
            grid_rows[i+1].color = LINE_COLOR;
        }
    }

    window->draw(grid_cols);
    window->draw(grid_rows);
}
