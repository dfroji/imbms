#include "eventhandler.h"

#include "filedialog.h"
#include "bms_edit_event.h"
#include "notes.h"

EventHandler::EventHandler() {
}

EventHandler::~EventHandler() {
}

void EventHandler::poll_event(State* state, sf::RenderWindow* window) {
    this->state = state;
    this->window = window;

    sf::Event event;

    while (window->pollEvent(event)) {
        ImGui::SFML::ProcessEvent(*window, event);

        if (event.type == sf::Event::Closed) {
            window->close();
        }

        if (event.type == sf::Event::KeyPressed) {
            key_pressed_event(event);
        }

        if (event.type == sf::Event::KeyReleased) {
            key_released_event(event);
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            mouse_button_pressed_event(event);
        }
        
        if (event.type == sf::Event::MouseButtonReleased) {
            mouse_button_released_event(event);
        }

        if (event.type == sf::Event::MouseWheelScrolled) {
            mouse_wheel_scrolled_event(event);
        }

        if (event.type == sf::Event::MouseMoved) {
            mouse_moved_event(event);
        }

        if (event.type == sf::Event::Resized) {
            sf::FloatRect visible_area(0.f, 0.f, event.size.width, event.size.height);
            window->setView(sf::View(visible_area));
        }
    }
}

sf::Vector2i EventHandler::get_mouse_pos() {
    ImVec2 viewport_pos = state->get_viewport_pos();
    ImVec2 viewport_size = state->get_viewport_size();
    fVec2 relative_pos = state->get_relative_pos();

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*window);
    mouse_pos.x += relative_pos.x;
    mouse_pos.y = viewport_pos.y + viewport_size.y - mouse_pos.y + relative_pos.y; 

    return mouse_pos;
}

int EventHandler::get_pointed_measure(sf::Vector2i mouse_pos, State* state) {
    fVec2 wrapping_offset = state->get_wrapping_offset();
    fVec2 default_scaling = state->get_default_scaling();
    fVec2 grid_scale = state->get_grid_scale();
    iVec2 wraps = state->get_wraps();
    int measures_wrapped = state->get_measures_wrapped();

    // get the pointed measure with this funny calculation
    int pointed_measure = static_cast<int>(
        ((mouse_pos.y-wrapping_offset.y)/2)/(default_scaling.y*grid_scale.y)+(wraps.y*measures_wrapped)
    );
    return pointed_measure;
}

int EventHandler::get_pointed_channel(sf::Vector2i mouse_pos, State* state) {
    fVec2 wrapping_offset = state->get_wrapping_offset();
    fVec2 default_scaling = state->get_default_scaling();
    fVec2 grid_scale = state->get_grid_scale();
    iVec2 wraps = state->get_wraps();
    ImVec2 viewport_size = state->get_viewport_size();
    float note_width = state->get_note_width(); 

    // another funny calculation
    int pointed_channel = static_cast<int>(
        ((mouse_pos.x-wrapping_offset.x)*4)/(default_scaling.x*grid_scale.x)+(wraps.x*((viewport_size.x*grid_scale.x)/note_width))
    );
    return pointed_channel;
}

int EventHandler::get_pointed_cell(sf::Vector2i mouse_pos, State* state) {
    fVec2 wrapping_offset = state->get_wrapping_offset();
    fVec2 default_scaling = state->get_default_scaling();
    fVec2 grid_scale = state->get_grid_scale();
    iVec2 wraps = state->get_wraps();
    int measures_wrapped = state->get_measures_wrapped();
    int quantization = state->get_quantization();

    // another one who would've thought
    int pointed_cell = static_cast<int>(
        (((mouse_pos.y-wrapping_offset.y)/2)/(default_scaling.y*grid_scale.y)+(wraps.y*measures_wrapped))*quantization
    );
    pointed_cell = pointed_cell % quantization;
    return pointed_cell;
}

Note EventHandler::get_pointed_note(sf::Vector2i mouse_pos, State* state) {
    Note note = Note();
    BMS* bms = state->get_bms();

    ImVec2 viewport_pos = state->get_viewport_pos();
    fVec2 default_scaling = state->get_default_scaling();
    fVec2 grid_scale = state->get_grid_scale();
    iVec2 wraps = state->get_wraps();
    int measures_wrapped = state->get_measures_wrapped();
    fVec2 wrapping_offset = state->get_wrapping_offset(); 

    int measure_i = EventHandler::get_pointed_measure(mouse_pos, state);
    int channel_i = EventHandler::get_pointed_channel(mouse_pos, state);

    std::vector<Measure*> measures = bms->get_measures();
    if (measure_i >= measures.size()) {return NULL_NOTE;}
    Measure* measure = measures[measure_i];
    if (measure == nullptr) {return NULL_NOTE;}

    Channel* channel = nullptr;

    std::vector<std::string> play_channels = bms->get_play_channels();
    std::vector<std::string> other_channels = state->get_other_channels();
    if (channel_i < play_channels.size()) {
        channel = measure->channels[ImBMS::base36_to_int(play_channels[channel_i])];

    } else if (channel_i < play_channels.size() + other_channels.size()) {

        channel = measure->channels[ImBMS::base36_to_int(other_channels[channel_i - play_channels.size()])];

    } else if (channel_i - play_channels.size() - other_channels.size() < measure->bgm_channels.size()) {
        channel = measure->bgm_channels[channel_i - play_channels.size() - other_channels.size()];
    }

    if (channel == nullptr) {return NULL_NOTE;}
    note.channel = channel;

    std::vector<int> components = channel->components;
    for (int i = 0; i < components.size(); i++) {
        float note_pos_y = 2*measure_i*default_scaling.y*grid_scale.y - 2*default_scaling.y*grid_scale.y*wraps.y*measures_wrapped + wrapping_offset.y + 2*i*((default_scaling.y*grid_scale.y)/components.size());
        if (std::abs(mouse_pos.y - (note_pos_y + NOTE_HEIGHT/2)) < NOTE_HEIGHT/2) {
            note.component_i = i;
            note.component = components[i];
            note.measure_i = measure_i;
            note.channel_i = channel_i;
            break;
        }
    }

    return note;
}

void EventHandler::key_pressed_event(sf::Event event) {
    if (event.key.scancode == sf::Keyboard::Scan::LShift || event.key.scancode == sf::Keyboard::Scan::RShift) {
        state->set_shift(true);
    }

    if (event.key.scancode == sf::Keyboard::Scan::LControl || event.key.scancode == sf::Keyboard::Scan::RControl) {
        state->set_control(true);
    }

    if (event.key.scancode == sf::Keyboard::Scan::N) {
        if (state->is_control()) {
            state->new_bms();
        }
    }

    if (event.key.scancode == sf::Keyboard::Scan::O) {
        if (state->is_control()) {
            FileDialog fd(state);
            fs::path filepath = fd.open_file(state->get_current_path(), FDMode::BMSFiles);
            state->load_bms(filepath);
        }
    }

    if (event.key.scancode == sf::Keyboard::Scan::S) {
        if (state->is_control()) {
            if (state->is_shift()) {
                FileDialog fd(state);
                fs::path filepath = fd.save_file(state->get_current_path(), FDMode::BMSFiles);
                state->save_bms(filepath);

            } else if (state->is_modified()) {
                if (!state->has_filepath()) {
                    FileDialog fd(state);
                    fs::path filepath = fd.save_file(state->get_current_path(), FDMode::BMSFiles);
                    state->save_bms(filepath);

                } else {
                    state->save_bms(state->get_filename());
                }
            }
        } 
    }

    if (event.key.scancode == sf::Keyboard::Scan::Z) {
        if (state->is_control()) {
            state->undo();
        }
    }

    if (event.key.scancode == sf::Keyboard::Scan::Y) {
        if (state->is_control()) {
            state->redo();
        }
    }

}

void EventHandler::key_released_event(sf::Event event) {
    if (event.key.scancode == sf::Keyboard::Scan::LShift || event.key.scancode == sf::Keyboard::Scan::RShift) {
        state->set_shift(false);
    }

    if (event.key.scancode == sf::Keyboard::Scan::LControl || event.key.scancode == sf::Keyboard::Scan::RControl) {
        state->set_control(false);
    }
}

void EventHandler::mouse_button_pressed_event(sf::Event event) {
    if (!is_mouse_on_main_field() || state->is_popup()) {
        return;
    }

    Note pointed_note = EventHandler::get_pointed_note(get_mouse_pos(), state);
    state->clear_selected_notes();

    if (event.mouseButton.button == sf::Mouse::Left) {
        state->set_mouse_left(true);

        if (pointed_note.component <= 0) {
            BMSEditEvent::add_note(state->get_selected_keysound(), get_mouse_pos(), state);
            state->set_movable(false);

        } else {
            state->play_keysound(state->get_bms()->get_keysounds()[pointed_note.component]);
            state->set_selected_notes({pointed_note});
        }
    }

    if (event.mouseButton.button == sf::Mouse::Right) {
        if (pointed_note != NULL_NOTE && pointed_note.component != 0) {
            BMSEditEvent::remove_note(get_mouse_pos(), state);
        }
    }
}

void EventHandler::mouse_button_released_event(sf::Event event) {
    Note pointed_note = EventHandler::get_pointed_note(get_mouse_pos(), state);

    if (event.mouseButton.button == sf::Mouse::Left) {
        state->set_mouse_left(false);

        if (state->is_selected_notes_moved()) {
            BMSEditEvent::move_notes(get_mouse_pos(), state);
            state->set_selected_notes_moved(false);
        }

        if (!state->is_movable()) {
            state->set_movable(true);
        }
    }
}

void EventHandler::mouse_wheel_scrolled_event(sf::Event event) {
    if (!is_mouse_on_main_field() || state->is_popup()) {return;}

    iVec2 pos = state->get_absolute_pos();
    ImVec2 viewport_size = state->get_viewport_size();

    if (!state->is_shift()) {
        pos.y += event.mouseWheelScroll.delta*SCROLL_SPEED;

        if (pos.y < -viewport_size.y/2) {
            pos.y = -viewport_size.y/2;
        }
    } else {
        pos.x -= event.mouseWheelScroll.delta*SCROLL_SPEED;

        if (pos.x < -viewport_size.x/2) {
            pos.x = -viewport_size.x/2;
        }
    }

    state->set_absolute_pos(pos);
}

void EventHandler::mouse_moved_event(sf::Event event) {
    if (state->is_mouse_left() &&
        state->get_selected_notes().size() > 0 && 
        !state->is_selected_notes_moved() &&
        state->is_movable()
        ) {

        state->set_selected_notes_moved(true);
        BMSEditEvent::remove_selected_notes(state);
    }
}

bool EventHandler::is_mouse_on_main_field() {
    if (state->is_menu_bar_interacted()) {return false;}

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*window);
    ImVec2 viewport_size = state->get_viewport_size();
    int menu_bar_height = ImGui::CalcTextSize("").y + ImGui::GetStyle().FramePadding.y;

    if (mouse_pos.x > viewport_size.x * (1-SIDE_MENU_WIDTH) || mouse_pos.y < menu_bar_height) {
        return false;
    } else {
        return true;
    }

}
