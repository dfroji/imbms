#pragma once

#include "state.h"
#include "audio.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"

class EventHandler {
    public:
        EventHandler();
        ~EventHandler();

        void poll_event(State* state, sf::RenderWindow* window);
        sf::Vector2i get_mouse_pos();
        static sf::Vector2i get_mouse_pos_st();

        static int get_pointed_measure(sf::Vector2i mouse_pos, State* state);
        static int get_pointed_channel(sf::Vector2i mouse_pos, State* state);
        static int get_pointed_cell(sf::Vector2i mouse_pos, State* state);
        static Note get_pointed_note(sf::Vector2i mouse_pos, State* state);

        static void open_file(State* state);
        static void save_file(State* state);

    private:
        void key_pressed_event(sf::Event event);
        void key_released_event(sf::Event event);
        void mouse_button_pressed_event(sf::Event event);
        void mouse_button_released_event(sf::Event event);
        void mouse_wheel_scrolled_event(sf::Event event);
        void mouse_moved_event(sf::Event event);
        bool is_mouse_on_main_field();

        State* state;
        sf::RenderWindow* window;

        bool is_lshift_pressed;
        bool is_lcontrol_pressed;
        bool is_mouse1_held;
};
