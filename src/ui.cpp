#include "ui.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <cstring>

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
    this->is_shift_pressed = false;

    this->grid_scale = {2.5f, 2.5f};
    this->y_position = 0;
    this->x_position = 0;

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

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.scancode == sf::Keyboard::Scan::LShift) {
                this->is_shift_pressed = true;
            }

            if (event.key.scancode == sf::Keyboard::Scan::E) {
                load_bms("Asgard_[7-A_Another].bme");
            }
        }

        if (event.type == sf::Event::KeyReleased) {
            if (event.key.scancode == sf::Keyboard::Scan::LShift) {
                this->is_shift_pressed = false;
            }
        }

        if (event.type == sf::Event::MouseWheelScrolled) {
            if (!this->is_shift_pressed) {
                this->y_position += event.mouseWheelScroll.delta*SCROLL_SPEED;

                ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
                if (this->y_position < -viewport_size.y/2) {
                    this->y_position = -viewport_size.y/2;
                }
            } else {
                this->x_position += event.mouseWheelScroll.delta*SCROLL_SPEED;

                ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
                if (this->x_position < -viewport_size.x/2) {
                    this->x_position = -viewport_size.x/2;
                }
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
        // ImGui::MenuItem("Open file");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit")) {

        ImGui::EndMenu();
    }
    
    if (ImGui::BeginMenu("View")) {

        ImGui::EndMenu();
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
        static char title[1024] =  "", subtitle[1024] = "";
        static char artist[1024] = "", subartist[1024] = "";
        static char genre[1024] = "";
        std::strcpy(title, this->bms->get_title().c_str());
        std::strcpy(subtitle, this->bms->get_subtitle().c_str());
        std::strcpy(artist, this->bms->get_artist().c_str());
        std::strcpy(subartist, this->bms->get_subartist().c_str());
        std::strcpy(genre, this->bms->get_genre().c_str());

        ImGui::InputText("Title", title, IM_ARRAYSIZE(title));
        ImGui::InputText("Subtitle", subtitle, IM_ARRAYSIZE(subtitle));        
        ImGui::InputText("Artist", artist, IM_ARRAYSIZE(title));
        ImGui::InputText("Subartist", subartist, IM_ARRAYSIZE(subtitle));
        ImGui::InputText("Genre", genre, IM_ARRAYSIZE(title));

        const char* modes[] = {"SP", "DP", "PM"};
        static int current_mode = 0;
        ImGui::Combo("Mode", &current_mode, modes, IM_ARRAYSIZE(modes));

        static double bpm = this->bms->get_bpm();
        ImGui::InputDouble("BPM", &bpm, 1.0f, 10.0f, "%.0f");

        static int total = 300;
        ImGui::InputInt("Total", &total);

        const char* ranks[] = {"Very Hard", "Hard", "Normal", "Easy"};
        static int current_rank = this->bms->get_rank();
        ImGui::Combo("Rank", &current_rank, ranks, IM_ARRAYSIZE(ranks));

        this->bms->set_title(title);
        this->bms->set_subtitle(subtitle);
        this->bms->set_artist(artist);
        this->bms->set_subartist(subartist);

        this->bms->set_bpm(bpm);

        this->bms->set_rank(static_cast<Rank>(current_rank));
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

    int default_y_scaling = viewport_size.y/10;
    int default_x_scaling = viewport_size.x/10;
    int padding = 3;

    // Calculate the count of measures within the screen
    // Limit the count to keep the scrolling seamless at higher grid scales
    int measures = viewport_size.y / (default_y_scaling*this->grid_scale.y);
    int measures_wrapped = viewport_size.y / default_y_scaling / 2;
    if (measures < measures_wrapped) {measures = measures_wrapped;}

    // Calculate the relative position as absolute position % viewport size accounting for scaling
    float y_pos = std::fmod(this->y_position*this->grid_scale.y, viewport_size.y*this->grid_scale.y);
    float x_pos = std::fmod(this->x_position*this->grid_scale.x, viewport_size.x*this->grid_scale.x);

    // Numbers of wraps is absolute position divided by viewport size accounting for scaling
    int y_wraps = (this->y_position*this->grid_scale.y) / (viewport_size.y*this->grid_scale.y);
    int x_wraps = (this->x_position*this->grid_scale.x) / (viewport_size.x*this->grid_scale.x);

    // The offsets needed for lines to match up when wrapping
    float y_wrapping_offset = y_wraps*((std::sqrt(default_y_scaling)*2*this->grid_scale.y)-SCROLL_SPEED*this->grid_scale.y);
    float x_wrapping_offset = x_wraps*((std::sqrt(default_x_scaling)*2*this->grid_scale.x)-SCROLL_SPEED*this->grid_scale.x);

    // Setup horizontal lines for drawing
    int horizontal_line_count = measures*this->quantization;
    sf::VertexArray horizontal_lines(sf::Lines, 2*horizontal_line_count*padding);
    for (int i = 0; i < horizontal_line_count*padding; i += 2) {
        
        // Calculate the position of the given line
        float line_distance = i*((default_y_scaling*this->grid_scale.y)/this->quantization);
        float line_y_position = y_pos + viewport_size.y - viewport_pos.y - line_distance - y_wrapping_offset;

        // Offset the left end of the line if scrolled beyond the beginning of the grid
        float x_offset = 0;
        if (x_pos < 0) {
            x_offset = x_pos;
        }

        // Set the line in the VertexArray
        // i corresponds to the left end of the line, and i+1 corresponds to the right end of the line
        horizontal_lines[i].position = {viewport_pos.x - x_offset, line_y_position};
        horizontal_lines[i+1].position = {viewport_size.x, line_y_position};
        horizontal_lines[i].color = LINE_COLOR;
        horizontal_lines[i+1].color = LINE_COLOR;
    }

    // Arbitrary multiplier for the vertical lines
    int vertical_line_multiplier = 8;
    // Limit the vertical line count for seamless scrolling
    int vertical_line_count_min = (viewport_size.x / default_x_scaling / 2)*vertical_line_multiplier;

    // Setup vertical lines for drawing
    int vertical_line_count = (viewport_size.x / (default_x_scaling*this->grid_scale.x))*vertical_line_multiplier;
    if (vertical_line_count < vertical_line_count_min) {vertical_line_count = vertical_line_count_min;}
    sf::VertexArray vertical_lines(sf::Lines, 2*vertical_line_count*padding);
    for (int i = 0; i < vertical_line_count*padding; i += 2) {

        // Calculate the position of the given line
        float line_distance = i*((default_x_scaling*this->grid_scale.x)/vertical_line_multiplier);
        float line_x_position = -x_pos + viewport_pos.x + line_distance + x_wrapping_offset;

        // Offset the bottom end of the line if scrolled beyond the beginning of the grid
        float y_offset = 0;
        if (y_pos < viewport_pos.y) {
            y_offset = y_pos - viewport_pos.y;
        }

        // Set the line in the VertexArray
        // i corresponds to the top end of the line, and i+1 corresponds to the bottom end of the line
        vertical_lines[i].position = {line_x_position, viewport_pos.y};
        vertical_lines[i+1].position = {line_x_position, viewport_size.y + y_offset};
        vertical_lines[i].color = LINE_COLOR;
        vertical_lines[i+1].color = LINE_COLOR;
    }


    // Setup measure lines with their respective numbers for drawing
    sf::VertexArray measure_lines(sf::Lines, 2*measures*padding);
    std::vector<sf::Text> texts;
    for (int i = 0; i < measures*padding; i += 2) {
        

        // Calculate the position of the given measure line

        float measure_distance = i*default_y_scaling*this->grid_scale.y;
        float measure_y_position = y_pos + viewport_size.y - viewport_pos.y - measure_distance - y_wrapping_offset;

        // Set the line in the VertexArray
        // i corresponds to the left end of the line, and i+1 corresponds to the right end of the line
        measure_lines[i].position = {viewport_pos.x, measure_y_position};
        measure_lines[i+1].position = {viewport_size.x, measure_y_position}; 
        measure_lines[i].color = MEASURE_COLOR;
        measure_lines[i+1].color = MEASURE_COLOR;

        // Setup the number of the measure line
        int measure_number = (i/2)+y_wraps*measures_wrapped;
        sf::Text text;
        text.setString(std::to_string(measure_number));
        text.setFont(this->font);
        text.setPosition(viewport_pos.x + 2, y_pos + viewport_size.y - measure_distance - viewport_pos.y*2 - y_wrapping_offset);
        text.setCharacterSize(12);
        text.setFillColor(sf::Color::White);
        texts.push_back(text);
    }

    this->window->draw(horizontal_lines);
    this->window->draw(vertical_lines);
    this->window->draw(measure_lines);
    for (const auto& text : texts) {
        this->window->draw(text);
    }

}

bool UI::load_bms(std::string filename) {
    BMS* new_bms = ImBMS::parse_bms(filename);
    if (new_bms == nullptr) {return false;}

    BMS* bms_to_be_deleted = this->bms;
    this->bms = new_bms;
    delete bms_to_be_deleted;
    
    return true;
}
