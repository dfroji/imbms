#include "ui.h"

#include "SFML/System/Clock.hpp"

#include "eventhandler.h"
#include "utils.h"
#include "font.h"

UI::UI() {
    window = new sf::RenderWindow(sf::VideoMode(640, 480), "ImBMS");
    window->setVerticalSyncEnabled(true);
    ImGui::SFML::Init(*window);

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL; // disable the generation of imgui.ini
    ImBMS::Font::load_imgui(io);

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

    // change the title of the window to the filename
    std::string filename = state->get_filename().filename().generic_string();
    if (filename == "") {filename = "Untitled";} // use Untitled if if filepath is empty
    std::string new_title = "ImBMS - " + filename;
    if (state->is_modified()) {new_title += "*";} // add asterix if changes are made
    window->setTitle(new_title);

    EventHandler event;
    event.poll_event(state, window);

    ImGui::SFML::Update(*window, delta_clock.restart());

    this->is_open_ = window->isOpen();

    window->clear();

    menu_bar_renderer.render(state, window);
    side_menu_renderer.render(state, state->get_bms());
    grid_renderer.render(state, window);
    notes_renderer.render(state, window, event.get_mouse_pos());
    
    ImGui::SFML::Render(*window);
    window->display();

}

bool UI::is_open() {
    return this->is_open_; 
}
