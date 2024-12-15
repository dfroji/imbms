#include "ui.h"

#include "SFML/System/Clock.hpp"

#include "eventhandler.h"

UI::UI() {
    window = new sf::RenderWindow(sf::VideoMode(640, 480), "ImBMS");
    window->setVerticalSyncEnabled(true);
    ImGui::SFML::Init(*window);

    ImGui::GetIO().IniFilename = NULL; // disable the generation of imgui.ini

    state = new State();

    is_open_ = window->isOpen();
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
    
    ImGui::SFML::Render(*window);
    window->display();

}

bool UI::is_open() {
    return this->is_open_; 
}
