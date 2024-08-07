#include "ui.h"

#include <iostream>
#include <vector>
#include <cmath>

#include "SFML/Window/Event.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Graphics/Text.hpp"

const sf::Color LINE_COLOR(50, 50, 50);
const sf::Color BEAT_COLOR(100, 100, 100);
const sf::Color MEASURE_COLOR(255, 255, 255);

const int SCROLL_SPEED = 20;

UI::UI() {
    window = new sf::RenderWindow(sf::VideoMode(640, 480), "ImBMS");
    window->setFramerateLimit(60);
    ImGui::SFML::Init(*window);

    this->is_open_ = window->isOpen();

    this->grid_scale = {1.0f, 1.0f};
    this->position = 0;

    this->quantization = 16;
    this->measure_length = 4;

    this->bms = new BMS();

    this->font.loadFromFile("../fonts/Cousine-Regular.ttf");
}

UI::~UI() {
    ImGui::SFML::Shutdown();

    delete this->window;
    delete this->bms;
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
            this->position += event.mouseWheelScroll.delta*SCROLL_SPEED;

            ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
            if (this->position < -viewport_size.y/2) {
                this->position = -viewport_size.y/2;
            }
        }

        if (event.type == sf::Event::Resized) {
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

        static double bpm = this->bms->get_bpm();
        ImGui::InputDouble("BPM", &bpm, 1.0f, 10.0f, "%.0f");
        this->bms->set_bpm(bpm);

        static int total = 300;
        ImGui::InputInt("Total", &total);

        const char* judges[] = {"Easy", "Normal", "Hard", "Very Hard"};
        static int current_judge = 0;
        ImGui::Combo("Judge", &current_judge, judges, IM_ARRAYSIZE(judges));

    }

    if (ImGui::CollapsingHeader("Keysounds")) {

    }

    if (ImGui::CollapsingHeader("Grid")) {
        ImGui::InputInt("##quantization", &this->quantization);
        ImGui::InputInt("##measure_length", &this->measure_length);
        ImGui::DragFloat("x scale", &this->grid_scale.x, 0.1f, 1.0f, 100.0f, "%.1f");
        ImGui::DragFloat("y scale", &this->grid_scale.y, 0.1f, 1.0f, 100.0f, "%.1f");
    }

    ImGui::End();
}

void UI::render_grid() {
    ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
    ImVec2 viewport_pos = ImGui::GetMainViewport()->WorkPos;

    int default_scaling = viewport_size.y/10;
    int padding = 3;

    // Calculate the count of measures within the screen
    // Limit the count to keep the scrolling seamless at higher grid scales
    int measures = viewport_size.y / (default_scaling*this->grid_scale.y);
    int measures_wrapped = viewport_size.y / default_scaling / 2;
    if (measures < measures_wrapped) {measures = measures_wrapped;}

    // Setup measure lines with their respective numbers for drawing
    sf::VertexArray measure_lines(sf::Lines, 2*measures*padding);
    std::vector<sf::Text> texts;
    for (int i = 0; i < measures*padding; i += 2) {
        
        // Calculate the relative position as absolute position % viewport size accounting for scaling
        float pos = std::fmod(this->position*this->grid_scale.y, viewport_size.y*this->grid_scale.y);

        // Numbers of wraps is absolute position divided by viewport size accounting for scaling
        int wraps = (this->position*this->grid_scale.y) / (viewport_size.y*this->grid_scale.y);

        // Calculate the position of the given measure line
        float wrapping_offset = wraps*((std::sqrt(default_scaling)*2*this->grid_scale.y)-SCROLL_SPEED*this->grid_scale.y);
        float measure_distance = i*default_scaling*this->grid_scale.y;
        float measure_y_position = pos + viewport_size.y - viewport_pos.y - measure_distance - wrapping_offset;

        // Set the line in the VertexArray
        // i corresponds to the left end of the line, and i+1 corresponds to the right end of the line
        measure_lines[i].position = {viewport_pos.x, measure_y_position};
        measure_lines[i+1].position = {viewport_size.x, measure_y_position}; 
        measure_lines[i].color = MEASURE_COLOR;
        measure_lines[i+1].color = MEASURE_COLOR;

        // Setup the number of the measure line
        int measure_number = (i/2)+wraps*measures_wrapped;
        sf::Text text;
        text.setString(std::to_string(measure_number));
        text.setFont(this->font);
        text.setPosition(viewport_pos.x + 2, pos + viewport_size.y - measure_distance - viewport_pos.y*2 - wrapping_offset);
        text.setCharacterSize(12);
        text.setFillColor(sf::Color::White);
        texts.push_back(text);
    }

    this->window->draw(measure_lines);
    for (const auto& text : texts) {
        this->window->draw(text);
    }

}
