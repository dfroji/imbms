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
    this->is_lshift_pressed = false;
    this->is_lcontrol_pressed = false;
    this->is_mouse1_held = false;

    this->clicked_note = NULL_NOTE;
    this->moved_note = NULL_NOTE;
    this->is_moved_note_removed = false;
    this->is_moved_note_rendered = false;

    this->grid_scale = {2.5f, 2.5f};
    this->absolute_pos = {0, 0};

    this->quantization = 16;
    this->measure_length = 4;

    this->keysound = 1;

    this->undo_list = {};

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
                this->is_lshift_pressed = true;
            }

            if (event.key.scancode == sf::Keyboard::Scan::LControl) {
                this->is_lcontrol_pressed = true;
            }

            if (event.key.scancode == sf::Keyboard::Scan::Z) {
                if (this->undo_list.size() > 0 && this->is_lcontrol_pressed) {
                    undo();
                }
            }

            // for debugging
            if (event.key.scancode == sf::Keyboard::Scan::E) {
                load_bms("test.bme");
            }
        }

        if (event.type == sf::Event::KeyReleased) {
            if (event.key.scancode == sf::Keyboard::Scan::LShift) {
                this->is_lshift_pressed = false;
            }
            if (event.key.scancode == sf::Keyboard::Scan::LControl) {
                this->is_lcontrol_pressed = false;
            }
        }

        if (event.type == sf::Event::MouseWheelScrolled && !this->is_keysounds_hovered) {
            if (!this->is_lshift_pressed) {
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

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            limit_mouse();
            get_pointed_cell(get_mouse_pos());
            if (!this->is_mouse1_held) {
                this->prev_mouse_pos = this->get_mouse_pos();
                this->clicked_note = get_pointed_note();
                if (this->clicked_note == NULL_NOTE || this->clicked_note.component <= 0) {
                    add_note(this->keysound);
                }
                this->is_mouse1_held = true;

            } else if (this->prev_mouse_pos != this->get_mouse_pos()) {
                if (this->clicked_note != NULL_NOTE && this->clicked_note.component != 0) {
                    if (!this->is_moved_note_removed) {
                        remove_note(this->clicked_note);
                        this->is_moved_note_removed = true;
                    }
                    this->moved_note = this->clicked_note;
                    this->is_moved_note_rendered = true;
                }
            }
        } else {
            if (this->is_mouse1_held) {
                if (this->is_moved_note_rendered) {
                    if (get_pointed_note().component > 0 || !add_note(this->moved_note.component)) {
                        undo();
                    } else {
                        this->undo_list.push_back(std::bind(undo_move_note, &this->undo_list));
                    }
                    this->is_moved_note_rendered = false;
                    this->is_moved_note_removed = false;
                }
                this->is_mouse1_held = false;
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            remove_note(get_pointed_note());
        }

        if (event.type == sf::Event::Resized) {
            sf::FloatRect visible_area(0.f, 0.f, event.size.width, event.size.height);
            this->window->setView(sf::View(visible_area));
        }
    }

    ImGui::SFML::Update(*window, delta_clock.restart());

    this->is_open_ = window->isOpen();

    window->clear();


    this->render_main_menu_bar();
    this->render_side_section();
    this->render_grid();
    this->render_notes();
    if (this->is_moved_note_rendered) {
        this->render_moved_note();
    }

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

    // static values used in a collapsing header are initialized outside of it
    // as they wouldn't update propely when loading a file and the header is open
    static int current_mode = this->bms->get_playstyle();
    static double bpm = this->bms->get_bpm();
    static int total = this->bms->get_total();
    static int current_rank = this->bms->get_rank();
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
        current_mode = this->bms->get_playstyle();
        if (ImGui::Combo("Mode", &current_mode, modes, IM_ARRAYSIZE(modes))) {
            this->bms->set_playstyle(static_cast<Playstyle>(current_mode));
        }

        bpm = this->bms->get_bpm();
        if (ImGui::InputDouble("BPM", &bpm, 1.0f, 10.0f, "%.0f")) {
            this->bms->set_bpm(bpm);
        }

        total = this->bms->get_total();
        if (ImGui::InputInt("Total", &total)) {
            this->bms->set_total(total);
        }

        const char* ranks[] = {"Very Hard", "Hard", "Normal", "Easy"};
        current_rank = this->bms->get_rank();
        if (ImGui::Combo("Rank", &current_rank, ranks, IM_ARRAYSIZE(ranks))) {
            this->bms->set_rank(static_cast<Rank>(current_rank));
        }

        this->bms->set_title(title);
        this->bms->set_subtitle(subtitle);
        this->bms->set_artist(artist);
        this->bms->set_subartist(subartist);
        this->bms->set_genre(genre);


        this->bms->set_bpm(bpm);

        this->bms->set_rank(static_cast<Rank>(current_rank));
    }

    if (ImGui::CollapsingHeader("Keysounds")) {
        char* keysound_labels[DATA_LIMIT];
        std::vector<std::string> label_strings = get_keysound_labels(DATA_LIMIT, 2);
        for (int i = 0; i < DATA_LIMIT; i++) {
            keysound_labels[i] = const_cast<char*>(label_strings[i].c_str()); 
        } 

        static int selected_keysound = this->keysound - 1;
        if (ImGui::ListBox("##keysounds_list", &selected_keysound, keysound_labels, DATA_LIMIT, 10)) {
            this->keysound = selected_keysound + 1;
        }
        if (ImGui::IsItemHovered()) {
            this->is_keysounds_hovered = true;
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                // todo: file dialog
            }
        } else {
            this->is_keysounds_hovered = false;
        }
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
                break;
        } 
        render_play_channels(measure, channels);
        render_bga_channels(measure, BGA_CHANNELS, channels.size());
        render_bgm_channels(measure, channels.size()+BGA_CHANNELS.size());
    }
}

void UI::render_play_channels(int measure_index, std::vector<std::string> channels) {
    Measure* measure = this->bms->get_measures()[measure_index];
    for (int channel_index = 0; channel_index < channels.size(); channel_index++) {
        Channel* channel = measure->channels[ImBMS::base36_to_int(channels[channel_index])];
        if (channel == nullptr) {continue;}
        render_channel_notes(measure_index, channel_index, channel->components, get_channel_color(channel_index));
    }
}

void UI::render_bgm_channels(int measure_index, int offset) {
    Measure* measure = this->bms->get_measures()[measure_index];
    for (int channel_index = 0; channel_index < measure->bgm_channels.size(); channel_index++) {
        Channel* channel = measure->bgm_channels[channel_index];
        render_channel_notes(measure_index, channel_index + offset, channel->components, BGM_COLOR);
    } 
}

void UI::render_bga_channels(int measure_index, std::vector<std::string> channels, int offset) {
    Measure* measure = this->bms->get_measures()[measure_index];
    for (int channel_index = 0; channel_index < channels.size(); channel_index++) {
        Channel* channel = measure->channels[ImBMS::base36_to_int(channels[channel_index])];
        if (channel == nullptr) {continue;}
        render_channel_notes(measure_index, channel_index + offset, channel->components, BGA_COLOR);
    }
}

void UI::render_channel_notes(int measure_index, int channel_index, std::vector<int> components, sf::Color color) {
    for (int i = 0; i < components.size(); i++) {
        if (components[i] == 0) {continue;}
        sf::RectangleShape note(sf::Vector2f((this->default_scaling.x*this->grid_scale.x)/4, 10));
        note.setFillColor(color);
        note.setOrigin(0, 10);
        note.setPosition(
            -this->absolute_pos.x*this->grid_scale.x + channel_index*((this->default_scaling.x*this->grid_scale.x)/4),
            this->absolute_pos.y*this->grid_scale.y + this->viewport_size.y - this->viewport_pos.y - this->wrapping_offset.y - 2*measure_index*this->default_scaling.y*this->grid_scale.y - ((2*this->default_scaling.y*this->grid_scale.y)/(components.size()))*i
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

void UI::calculate_values() {
    this->viewport_size = ImGui::GetMainViewport()->Size;
    this->viewport_pos = ImGui::GetMainViewport()->WorkPos;

    this->default_scaling = {this->viewport_size.x/DEFAULT_SCALING_DIV, this->viewport_size.y/DEFAULT_SCALING_DIV};

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

    this->undo_list = {};
    
    return true;
}

void UI::limit_mouse() {
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->window);
    sf::Vector2i new_pos = mouse_pos;

    if (mouse_pos.x < 0) {
        new_pos.x = 0;
    } else if (mouse_pos.x > this->viewport_size.x) {
        new_pos.x = this->viewport_size.x;
    }

    if (mouse_pos.y < 0) {
        new_pos.y = 0;
    } else if (mouse_pos.y > this->viewport_size.y) {
        new_pos.y = this->viewport_size.y;
    }

    if (new_pos != mouse_pos) {
        sf::Mouse::setPosition(new_pos, *this->window);
    }
}

std::vector<std::string> UI::get_play_channels() {
    std::vector<std::string> play_channels;
    switch(this->bms->get_playstyle()) {
        case Playstyle::SP:
            play_channels = P1_VISIBLE;
            break;
        case Playstyle::DP:
            play_channels = P1_VISIBLE;
            play_channels.insert(std::end(play_channels), std::begin(P2_VISIBLE), std::end(P2_VISIBLE));
            break;
        case Playstyle::PM:
            play_channels = PM_VISIBLE;
            break;
    }

    return play_channels;
}


sf::Vector2i UI::get_mouse_pos() {
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->window);
    mouse_pos.x += this->relative_pos.x;
    mouse_pos.y = this->viewport_size.y - mouse_pos.y + this->relative_pos.y - 20; 
    return mouse_pos;
}

int UI::get_pointed_measure(sf::Vector2i mouse_pos) {
    int pointed_measure = static_cast<int>(
        ((mouse_pos.y-this->wrapping_offset.y)/2)/(this->default_scaling.y*this->grid_scale.y)+(this->wraps.y*this->measures_wrapped)
    );
    return pointed_measure;
}

int UI::get_pointed_channel(sf::Vector2i mouse_pos) {
    int pointed_channel = static_cast<int>(
        ((mouse_pos.x-this->wrapping_offset.x)*4)/(this->default_scaling.x*this->grid_scale.x)+(this->wraps.x*(this->viewport_size.x / this->default_scaling.x / 2))
    );
    return pointed_channel;
}

int UI::get_pointed_cell(sf::Vector2i mouse_pos) {
    int pointed_cell = static_cast<int>(
        (((mouse_pos.y-this->wrapping_offset.y)/2)/(this->default_scaling.y*this->grid_scale.y)+(this->wraps.y*this->measures_wrapped))*this->quantization
    );
    pointed_cell = pointed_cell % this->quantization;
    return pointed_cell;
}

Note UI::get_pointed_note() {
    Note note = Note();

    sf::Vector2i mouse_pos = get_mouse_pos();
    int clicked_measure = get_pointed_measure(mouse_pos);
    int clicked_channel = get_pointed_channel(mouse_pos);

    if (clicked_measure >= this->bms->get_measures().size()) {return NULL_NOTE;}
    Measure* measure = this->bms->get_measures()[clicked_measure];
    if (measure == nullptr) {return NULL_NOTE;}

    Channel* channel = nullptr;

    std::vector<std::string> play_channels = get_play_channels();
    if (clicked_channel < play_channels.size()) {
        channel = measure->channels[ImBMS::base36_to_int(play_channels[clicked_channel])];
    } else if (clicked_channel < play_channels.size() + BGA_CHANNELS.size()) {
        channel = measure->channels[ImBMS::base36_to_int(BGA_CHANNELS[clicked_channel - play_channels.size()])];
    } else if (clicked_channel - play_channels.size() - BGA_CHANNELS.size() < measure->bgm_channels.size()) {
        channel = measure->bgm_channels[clicked_channel - play_channels.size() - BGA_CHANNELS.size()];
    }
    if (channel == nullptr) {return NULL_NOTE;}
    note.channel = channel;

    std::vector<int> components = channel->components;
    for (int i = 0; i < components.size(); i++) {
        float note_pos_y = 2*clicked_measure*this->default_scaling.y*this->grid_scale.y - 2*this->default_scaling.y*this->grid_scale.y*this->wraps.y*this->measures_wrapped + this->wrapping_offset.y + 2*i*((this->default_scaling.y*this->grid_scale.y)/components.size());
        if (std::abs(mouse_pos.y - (note_pos_y + 5)) < 5) {
            note.component_i = i;
            note.component = components[i];
            break;
        }
    }

    return note;

}

void UI::render_moved_note() {
    if (this->moved_note.channel == nullptr) {return;}

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->window);
    sf::RectangleShape note(sf::Vector2f((this->default_scaling.x*this->grid_scale.x)/4, 10));
    note.setFillColor(get_channel_color(get_pointed_channel(get_mouse_pos())));
    note.setOrigin(0, 10);
    note.setPosition(mouse_pos.x - (this->default_scaling.y*this->grid_scale.x)/8,
                     mouse_pos.y + 5
                     );

    sf::Text text;
    text.setString(ImBMS::format_base36(this->moved_note.component, 2));
    text.setFont(this->font);
    text.setPosition(note.getPosition().x, note.getPosition().y - 12);
    text.setCharacterSize(12);
    text.setFillColor(sf::Color::White);
    text.setOutlineThickness(1.f);
    text.setOutlineColor(sf::Color::Black);

    this->window->draw(note);
    this->window->draw(text);
}

void UI::undo() {
    this->undo_list.back()();
    this->undo_list.pop_back();
}

bool UI::add_note(int component) {
    sf::Vector2i mouse_pos = get_mouse_pos();
    int measure_i = get_pointed_measure(mouse_pos);
    int channel_i = get_pointed_channel(mouse_pos);
    int cell = get_pointed_cell(mouse_pos); 

    if (this->bms->get_measures().size() - 1 <= measure_i) {
        this->bms->resize_measure_v(measure_i + 1);
    }

    if (this->bms->get_measures()[measure_i] == nullptr) {
        this->bms->new_measure(measure_i);
    }

    std::vector<std::string> play_channels = get_play_channels();
    if (channel_i < play_channels.size()) {
        channel_i = ImBMS::base36_to_int(play_channels[channel_i]);
        return add_play_or_bga_note(component, mouse_pos, measure_i, channel_i, cell); 
    }
    else if (channel_i < play_channels.size() + BGA_CHANNELS.size()) {
        channel_i = ImBMS::base36_to_int(BGA_CHANNELS[channel_i-play_channels.size()]);
        return add_play_or_bga_note(component, mouse_pos, measure_i, channel_i, cell); 
    } else {
        channel_i = channel_i - play_channels.size() - BGA_CHANNELS.size();
        return add_bgm_note(component, mouse_pos, measure_i, channel_i, cell); 
    }
}

bool UI::add_play_or_bga_note(int component, sf::Vector2i mouse_pos, int measure_i, int channel_i, int cell) {
    std::vector<Measure*> measures = this->bms->get_measures();
    Channel* channel = nullptr;
    std::vector<int> old_components = {};
    if (measure_i >= measures.size()) {return false;}
    if (measures[measure_i]->channels[channel_i] == nullptr) {
        channel = new Channel();
        measures[measure_i]->channels[channel_i] = channel;
        std::vector<int> components = {};
        components.resize(this->quantization, 0);
        components[cell] = component;
        channel->components = components;
    } else {
        channel = measures[measure_i]->channels[channel_i];
        old_components = channel->components;
        if (this->quantization != channel->components.size()) {
            int common_divisor = ImBMS::get_gcd(this->quantization, channel->components.size());
            if (channel->components.size() < this->quantization || 
                (this->quantization != common_divisor && channel->components.size() != common_divisor)) 
            {
                int components_old_size = channel->components.size();
                channel->resize(this->quantization/common_divisor);
                if (this->quantization < components_old_size) {
                    cell *= common_divisor;
                } else {
                    cell *= components_old_size/common_divisor;
                }
            } else if (this->quantization < channel->components.size()) {
                cell *= channel->components.size()/common_divisor;
            }
        }
        channel->components[cell] = component;
    }

    this->undo_list.push_back(std::bind(undo_add_note, measures[measure_i], channel, channel_i, old_components));

    return true;
}

bool UI::add_bgm_note(int component, sf::Vector2i mouse_pos, int measure_i, int channel_i, int cell) {
    std::vector<Measure*> measures = this->bms->get_measures();
    
    if (measures.size() <= measure_i) {
        return false;
    }
    
    Measure* measure = measures[measure_i];
    Channel* channel = nullptr;
    std::vector<int> old_components;

    if (measure->bgm_channels.size() <= channel_i) {
        for (int i = 0; i < channel_i + 1; i++) {
            measure->bgm_channels.resize(measure->bgm_channels.size() + 1, new Channel({0}));
        }
    }

    channel = measure->bgm_channels[channel_i];
    old_components = channel->components;
    if (this->quantization != channel->components.size()) {
        int common_divisor = ImBMS::get_gcd(this->quantization, channel->components.size());
        if (channel->components.size() < this->quantization || 
            (this->quantization != common_divisor && channel->components.size() != common_divisor)) 
        {
            int components_old_size = channel->components.size();
            channel->resize(this->quantization/common_divisor);
            if (this->quantization < components_old_size) {
                cell *= common_divisor;
            } else {
                cell *= components_old_size/common_divisor;
            }
        } else if (this->quantization < channel->components.size()) {
            cell *= channel->components.size()/common_divisor;
        }
    }
    channel->components[cell] = component;

    this->undo_list.push_back(std::bind(undo_add_note, measures[measure_i], channel, channel_i, old_components));

    return true;
}

void UI::undo_add_note(Measure* measure, Channel* channel, int channel_i, std::vector<int> old_components) {
    if (old_components.size() == 0) {
        measure->channels[channel_i] = nullptr;
        delete channel;
    } else {
        channel->components = old_components;
    }
}

void UI::remove_note(Note note) {
    if (note.channel != nullptr && note.component_i != -1) {
        int component = note.channel->components[note.component_i];
        if (component == 0) {return;}
        note.channel->components[note.component_i] = 0;
        this->undo_list.push_back(std::bind(undo_remove_note, note.channel, note.component_i, component));
    }
}

void UI::undo_remove_note(Channel* channel, int component_i, int component) {
    channel->components[component_i] = component;
}

void UI::undo_move_note(std::vector<std::function<void()>>* undo_list) {
    undo_list->pop_back();
    int i_end = 2;
    for (int i = 0; i < i_end; i++) {
        undo_list->back()();
        if (i != i_end - 1) {
            undo_list->pop_back();
        }
    }
}

sf::Color UI::get_channel_color(int channel_i) {
    std::vector<sf::Color> bm_colors = {BM_BOTTOM_NOTE_COLOR, BM_TOP_NOTE_COLOR};
    std::vector<sf::Color> pm_colors = {
                                        PM_WHITE_COLOR, PM_YELLOW_COLOR, PM_GREEN_COLOR, PM_BLUE_COLOR, 
                                        PM_RED_COLOR,
                                        PM_BLUE_COLOR, PM_GREEN_COLOR, PM_YELLOW_COLOR, PM_WHITE_COLOR
                                       };

    std::vector<sf::Color> play_channel_colors;
    switch (this->bms->get_playstyle()) {
        case Playstyle::SP:
            play_channel_colors.push_back(SCRATCH_COLOR);
            ImBMS::insert(play_channel_colors, bm_colors, P1_VISIBLE.size() - 1);
            break;
        case Playstyle::DP:
            play_channel_colors.push_back(SCRATCH_COLOR);
            ImBMS::insert(play_channel_colors, bm_colors, P1_VISIBLE.size() - 1);
            ImBMS::insert(play_channel_colors, bm_colors, P2_VISIBLE.size() - 1);
            play_channel_colors.push_back(SCRATCH_COLOR);
            break;
        case Playstyle::PM:
            ImBMS::insert(play_channel_colors, pm_colors, PM_VISIBLE.size());
            break;
    }

    if (channel_i < play_channel_colors.size()) {return play_channel_colors[channel_i];}
    else if (channel_i < play_channel_colors.size() + BGA_CHANNELS.size()) {return BGA_COLOR;}
    else {return BGM_COLOR;}
}

std::vector<std::string> UI::get_keysound_labels(int size, int digits) {
    std::vector<std::string> labels = {};
    std::vector<std::string> keysounds = this->bms->get_keysounds();
    for (int i = 1; i < size+1; i++) {
        std::string keysound = "";
        if (i < keysounds.size()) {
            keysound += keysounds[i];
        }
        labels.push_back(ImBMS::format_base36(i, 2) + " " + keysound);
    }
    return labels;
}
