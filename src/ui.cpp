#include "ui.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <cstring>

#include "SFML/Window/Event.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

#include "utils.h"

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
    this->absolute_pos = {0, 0};

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

    this->calculate_values();

    while (window->pollEvent(event)) {
        ImGui::SFML::ProcessEvent(*window, event);

        if (event.type == sf::Event::Closed) {
            window->close();
        }

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.scancode == sf::Keyboard::Scan::LShift) {
                this->is_shift_pressed = true;
            }

            // for debugging
            if (event.key.scancode == sf::Keyboard::Scan::E) {
                load_bms("test.bme");
            }
        }

        if (event.type == sf::Event::KeyReleased) {
            if (event.key.scancode == sf::Keyboard::Scan::LShift) {
                this->is_shift_pressed = false;
            }
        }

        if (event.type == sf::Event::MouseWheelScrolled) {
            if (!this->is_shift_pressed) {
                this->absolute_pos.y += event.mouseWheelScroll.delta*SCROLL_SPEED;

                if (this->absolute_pos.y < -this->viewport_size.y/2) {
                    this->absolute_pos.y = -this->viewport_size.y/2;
                }
            } else {
                this->absolute_pos.x += event.mouseWheelScroll.delta*SCROLL_SPEED;

                ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
                if (this->absolute_pos.x < -this->viewport_size.x/2) {
                    this->absolute_pos.x = -this->viewport_size.x/2;
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
    this->render_notes();

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
        static int current_mode = this->bms->get_playstyle();
        ImGui::Combo("Mode", &current_mode, modes, IM_ARRAYSIZE(modes));

        static double bpm = this->bms->get_bpm();
        ImGui::InputDouble("BPM", &bpm, 1.0f, 10.0f, "%.0f");

        static int total = this->bms->get_total();
        ImGui::InputInt("Total", &total);

        const char* ranks[] = {"Very Hard", "Hard", "Normal", "Easy"};
        static int current_rank = this->bms->get_rank();
        ImGui::Combo("Rank", &current_rank, ranks, IM_ARRAYSIZE(ranks));

        this->bms->set_title(title);
        this->bms->set_subtitle(subtitle);
        this->bms->set_artist(artist);
        this->bms->set_subartist(subartist);
        this->bms->set_genre(genre);

        this->bms->set_playstyle(static_cast<Playstyle>(current_mode));

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

    // Setup horizontal lines for drawing
    int horizontal_line_count = this->visible_measures*this->quantization;
    sf::VertexArray horizontal_lines(sf::Lines, 2*horizontal_line_count*PADDING);
    for (int i = 0; i < horizontal_line_count*PADDING; i += 2) {
        
        // Calculate the position of the given line
        float line_distance = i*((this->default_scaling.y*this->grid_scale.y)/this->quantization);
        float line_y_position = this->relative_pos.y + this->viewport_size.y - this->viewport_pos.y - line_distance - this->wrapping_offset.y;

        // Offset the left end of the line if scrolled beyond the beginning of the grid
        float x_offset = 0;
        if (this->relative_pos.x < 0) {
            x_offset = this->relative_pos.x;
        }

        // Set the line in the VertexArray
        // i corresponds to the left end of the line, and i+1 corresponds to the right end of the line
        horizontal_lines[i].position = {this->viewport_pos.x - x_offset, line_y_position};
        horizontal_lines[i+1].position = {this->viewport_size.x, line_y_position};
        horizontal_lines[i].color = LINE_COLOR;
        horizontal_lines[i+1].color = LINE_COLOR;
    }

    // Arbitrary multiplier for the vertical lines
    int vertical_line_multiplier = 8;
    // Limit the vertical line count for seamless scrolling
    int vertical_line_count_min = (this->viewport_size.x / this->default_scaling.x / 2)*vertical_line_multiplier;

    // Setup vertical lines for drawing
    int vertical_line_count = (this->viewport_size.x / (this->default_scaling.x*this->grid_scale.x))*vertical_line_multiplier;
    if (vertical_line_count < vertical_line_count_min) {vertical_line_count = vertical_line_count_min;}
    sf::VertexArray vertical_lines(sf::Lines, 2*vertical_line_count*PADDING);
    for (int i = 0; i < vertical_line_count*PADDING; i += 2) {

        // Calculate the position of the given line
        float line_distance = i*((this->default_scaling.x*this->grid_scale.x)/vertical_line_multiplier);
        float line_x_position = -this->relative_pos.x + this->viewport_pos.x + line_distance + this->wrapping_offset.x;

        // Offset the bottom end of the line if scrolled beyond the beginning of the grid
        float y_offset = 0;
        if (this->relative_pos.y < this->viewport_pos.y) {
            y_offset = this->relative_pos.y - this->viewport_pos.y;
        }

        // Set the line in the VertexArray
        // i corresponds to the top end of the line, and i+1 corresponds to the bottom end of the line
        vertical_lines[i].position = {line_x_position, this->viewport_pos.y};
        vertical_lines[i+1].position = {line_x_position, this->viewport_size.y + y_offset};
        vertical_lines[i].color = LINE_COLOR;
        vertical_lines[i+1].color = LINE_COLOR;
    }


    // Setup measure lines with their respective numbers for drawing
    sf::VertexArray measure_lines(sf::Lines, 2*this->visible_measures*PADDING);
    std::vector<sf::Text> texts;
    for (int i = 0; i < this->visible_measures*PADDING; i += 2) {
        

        // Calculate the position of the given measure line

        float measure_distance = i*this->default_scaling.y*this->grid_scale.y;
        float measure_y_position = this->relative_pos.y + this->viewport_size.y - viewport_pos.y - measure_distance - this->wrapping_offset.y;

        // Set the line in the VertexArray
        // i corresponds to the left end of the line, and i+1 corresponds to the right end of the line
        measure_lines[i].position = {this->viewport_pos.x, measure_y_position};
        measure_lines[i+1].position = {this->viewport_size.x, measure_y_position}; 
        measure_lines[i].color = MEASURE_COLOR;
        measure_lines[i+1].color = MEASURE_COLOR;

        // Setup the number of the measure line
        int measure_number = (i/2)+this->wraps.y*this->measures_wrapped;
        sf::Text text;
        text.setString(std::to_string(measure_number));
        text.setFont(this->font);
        text.setPosition(this->viewport_pos.x + 2, 
                         this->relative_pos.y + this->viewport_size.y - measure_distance - this->viewport_pos.y*2 - this->wrapping_offset.y
                        );
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

void UI::render_notes() {
    std::vector<Measure*> measures = this->bms->get_measures();
    for (int measure = 0; measure < measures.size(); measure++) {
        if (measures[measure] == nullptr) {continue;}
        std::vector<std::string> channels = {};
        switch (this->bms->get_playstyle()) {
            case Playstyle::SP:
                channels = P1_VISIBLE;
                break;
            case Playstyle::DP:
                channels = P1_VISIBLE;
                channels.insert(std::end(channels), std::begin(P2_VISIBLE), std::end(P2_VISIBLE));
                break;
            case Playstyle::PM:
                channels = PM_VISIBLE;
        } 
        render_channel(measure, channels);
    }
}

void UI::render_channel(int measure, std::vector<std::string> channels) {
    std::vector<Measure*> measures = this->bms->get_measures();
    for (int channel_i = 0; channel_i < channels.size(); channel_i++) {
        Channel* channel = measures[measure]->channels[ImBMS::base36_to_int(channels[channel_i])];
        if (channel == nullptr) {continue;}

        std::vector<int> components = channel->components;
        for (int i = 0; i < components.size(); i++) {
            if (components[i] == 0) {continue;}
            sf::RectangleShape note(sf::Vector2f((this->default_scaling.x*this->grid_scale.x)/4 - 1, 10));
            note.setFillColor(sf::Color(255,0,0));
            note.setOrigin(0, 10);
            note.setOutlineThickness(1.f);
            note.setOutlineColor(sf::Color(100,0,0));
            note.setPosition(
                -this->absolute_pos.x*this->grid_scale.x + channel_i*((this->default_scaling.x*this->grid_scale.x)/4),
                this->absolute_pos.y*this->grid_scale.y + this->viewport_size.y - this->viewport_pos.y - this->wrapping_offset.y - 2*measure*this->default_scaling.y*this->grid_scale.y - ((2*this->default_scaling.y*this->grid_scale.y)/(components.size()))*i
            );
            this->window->draw(note);

            sf::Text component_text;
            component_text.setString(ImBMS::format_base36(components[i], 2));
            component_text.setFont(this->font);
            component_text.setPosition(note.getPosition().x, note.getPosition().y - 12);
            component_text.setCharacterSize(12);
            component_text.setFillColor(sf::Color::White);
            component_text.setOutlineThickness(1.f);
            component_text.setOutlineColor(sf::Color::Black);
            this->window->draw(component_text);
        }
    }
}

void UI::calculate_values() {
    this->viewport_size = ImGui::GetMainViewport()->Size;
    this->viewport_pos = ImGui::GetMainViewport()->WorkPos;

    this->default_scaling = {this->viewport_size.x/10, this->viewport_size.y/10};

    // Calculate the count of measures within the screen
    // Limit the count to keep the scrolling seamless at higher grid scales
    this->visible_measures = this->viewport_size.y / (this->default_scaling.y*this->grid_scale.y);
    this->measures_wrapped = this->viewport_size.y / this->default_scaling.y / 2;
    if (this->visible_measures < this->measures_wrapped) {this->visible_measures = this->measures_wrapped;}

    // Calculate the relative position as absolute position % viewport size accounting for scaling
    this->relative_pos = {std::fmod(this->absolute_pos.x*this->grid_scale.x, this->viewport_size.x*this->grid_scale.x),
                          std::fmod(this->absolute_pos.y*this->grid_scale.y, this->viewport_size.y*this->grid_scale.y)
                         };

    // Numbers of wraps is absolute position divided by viewport size accounting for scaling
    this->wraps = {static_cast<int>((this->absolute_pos.x*this->grid_scale.x) / (this->viewport_size.x*this->grid_scale.x)), 
                   static_cast<int>((this->absolute_pos.y*this->grid_scale.y) / (this->viewport_size.y*this->grid_scale.y))
                  };

    // The offsets are needed when wrapping
    this->wrapping_offset = {this->wraps.x*((std::sqrt(this->default_scaling.x)*2*this->grid_scale.x)-SCROLL_SPEED*this->grid_scale.x),
                             this->wraps.y*((std::sqrt(this->default_scaling.y)*2*this->grid_scale.y)-SCROLL_SPEED*this->grid_scale.y)
                            };
}

bool UI::load_bms(std::string filename) {
    BMS* new_bms = ImBMS::parse_bms(filename);
    if (new_bms == nullptr) {return false;}

    BMS* bms_to_be_deleted = this->bms;
    this->bms = new_bms;
    delete bms_to_be_deleted;
    
    return true;
}
