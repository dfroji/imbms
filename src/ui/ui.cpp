#include "ui.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <cstring>
#include <thread>

#include "SFML/Window/Event.hpp"
#include "SFML/System/Clock.hpp"

#include "utils.h"
#include "write.h"
#include "eventhandler.h"

UI::UI() {
    window = new sf::RenderWindow(sf::VideoMode(640, 480), "ImBMS");
    window->setVerticalSyncEnabled(true);
    ImGui::SFML::Init(*window);

    state = new State();

    is_open_ = window->isOpen();

    clicked_note = NULL_NOTE;
    moved_note = NULL_NOTE;
    is_moved_note_removed = false;
    is_moved_note_rendered = false;
}

UI::~UI() {
    window->close();
    ImGui::SFML::Shutdown(*window);

    delete window;
    delete state;
}

void UI::render() {
    sf::Clock delta_clock;

    state->update();

    EventHandler event;
    event.poll_event(state, window);

    ImGui::SFML::Update(*window, delta_clock.restart());

    this->is_open_ = window->isOpen();

    window->clear();

    menu_bar_renderer.render(state);
    side_menu_renderer.render(state, state->get_bms());
    grid_renderer.render(state, window);
    notes_renderer.render(state, window, event.get_mouse_pos());
    
    // if (this->is_moved_note_rendered) {
    //     this->render_moved_note();
    // }

    for (auto& t : this->threads) {
        if (t.joinable()) {
            t.join();
        }
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



void UI::limit_mouse() {
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->window);
    sf::Vector2i new_pos = mouse_pos;
    ImVec2 viewport_size = state->get_viewport_size();

    if (mouse_pos.x < 0) {
        new_pos.x = 0;
    } else if (mouse_pos.x > viewport_size.x) {
        new_pos.x = viewport_size.x;
    }

    if (mouse_pos.y < 0) {
        new_pos.y = 0;
    } else if (mouse_pos.y > viewport_size.y) {
        new_pos.y = viewport_size.y;
    }

    if (new_pos != mouse_pos) {
        sf::Mouse::setPosition(new_pos, *window);
    }
}

// void UI::render_moved_note() {
//     if (this->moved_note.channel == nullptr) {return;}
//
//     sf::Vector2i mouse_pos = sf::Mouse::getPosition(*this->window);
//     sf::RectangleShape note(sf::Vector2f((this->default_scaling.x*this->grid_scale.x)/4, 10));
//     note.setFillColor(get_channel_color(get_pointed_channel(get_mouse_pos())));
//     note.setOrigin(0, 10);
//     note.setPosition(mouse_pos.x - (this->default_scaling.y*this->grid_scale.x)/8,
//                      mouse_pos.y + 5
//                      );
//
//     sf::Text text;
//     text.setString(ImBMS::format_base36(this->moved_note.component, 2));
//     text.setFont(this->font);
//     text.setPosition(note.getPosition().x, note.getPosition().y - 12);
//     text.setCharacterSize(FONT_SIZE);
//     text.setFillColor(sf::Color::White);
//     text.setOutlineThickness(1.f);
//     text.setOutlineColor(sf::Color::Black);
//
//     this->window->draw(note);
//     this->window->draw(text);
// }

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


