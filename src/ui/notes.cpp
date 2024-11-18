#include "notes.h"

#include "eventhandler.h"
#include <iostream>

Notes::Notes() {
    notes_render_v = {};
    labels_render_v = {};
    moving_notes_render_v = {};
    moving_labels_render_v = {};
}

Notes::~Notes() {

}

void Notes::render(State* state, sf::RenderWindow* window, sf::Vector2i mouse_pos) {
    notes_render_v.clear();
    labels_render_v.clear();
    moving_notes_render_v.clear();
    moving_labels_render_v.clear();

    std::vector<Measure*> measures = state->get_bms()->get_measures();

    int first_visible = state->get_wraps().y*state->get_measures_wrapped();
    int last_visible = first_visible+state->get_visible_measures()+NOTE_PADDING;

    std::vector<std::string> channels = state->get_bms()->get_play_channels();

    std::vector<std::thread> threads = {};
    for (int measure_i = first_visible; measure_i < measures.size() && measure_i < last_visible; measure_i++) {
        if (measures[measure_i] == nullptr) {continue;}
        threads.push_back(std::thread(&Notes::render_thread, this, measure_i, channels, state));
    }
    render_moving_selection(state, window, mouse_pos);

    for (auto& thread : threads) {
        thread.join();
    }
    for (const auto& n : notes_render_v) {
        window->draw(n);
    }
    for (const auto& l : labels_render_v) {
        window->draw(l);
    } 
    for (const auto& mn : moving_notes_render_v) {
        window->draw(mn);
    }
    for (const auto& ml : moving_labels_render_v) {
        window->draw(ml);
    }
}

void Notes::render_thread(int measure_i, std::vector<std::string> channels, State* state) {
    render_play_channels(measure_i, channels, state);
    render_bga_channels(measure_i, BGA_CHANNELS, channels.size(), state);
    render_bgm_channels(measure_i, channels.size()+BGA_CHANNELS.size(), state);
}

void Notes::render_play_channels(int measure_i, std::vector<std::string> channels, State* state) {
    Measure* measure = state->get_bms()->get_measures()[measure_i];
    for (int channel_i = 0; channel_i < channels.size(); channel_i++) {
        Channel* channel = measure->channels[ImBMS::base36_to_int(channels[channel_i])];
        if (channel == nullptr) {continue;}
        render_channel_notes(measure_i, channel_i, channel, get_channel_color(channel_i, state), state);
    }
}

void Notes::render_bgm_channels(int measure_i, int offset, State* state) {
    Measure* measure = state->get_bms()->get_measures()[measure_i];
    for (int channel_i = 0; channel_i < measure->bgm_channels.size(); channel_i++) {
        Channel* channel = measure->bgm_channels[channel_i];
        render_channel_notes(measure_i, channel_i + offset, channel, BGM_COLOR, state);
    } 
}

void Notes::render_bga_channels(int measure_i, std::vector<std::string> channels, int offset, State* state) {
    Measure* measure = state->get_bms()->get_measures()[measure_i];
    for (int channel_i = 0; channel_i < channels.size(); channel_i++) {
        Channel* channel = measure->channels[ImBMS::base36_to_int(channels[channel_i])];
        if (channel == nullptr) {continue;}
        render_channel_notes(measure_i, channel_i + offset, channel, BGA_COLOR, state);
    }
}

void Notes::render_channel_notes(int measure_i, int channel_i, Channel* channel, sf::Color color, State* state) {
    iVec2 absolute_pos = state->get_absolute_pos();
    fVec2 grid_scale = state->get_grid_scale();
    fVec2 default_scaling = state->get_default_scaling();
    ImVec2 viewport_size = state->get_viewport_size();
    ImVec2 viewport_pos = state->get_viewport_pos();
    fVec2 wrapping_offset = state->get_wrapping_offset();
    float note_width = (default_scaling.x*grid_scale.x)/4;

    std::vector<int> components = channel->components;

    for (int i = 0; i < components.size(); i++) {
        if (components[i] == 0) {continue;}

        fVec2 note_pos = {-absolute_pos.x*grid_scale.x + channel_i*note_width,
                          absolute_pos.y*grid_scale.y + viewport_size.y - viewport_pos.y - wrapping_offset.y - 2*measure_i*default_scaling.y*grid_scale.y - ((2*default_scaling.y*grid_scale.y)/(components.size()))*i
                         };

        if (note_pos.y < 0 || note_pos.y > viewport_size.y) {continue;}

        sf::RectangleShape note(sf::Vector2f(note_width, NOTE_HEIGHT));
        note.setFillColor(color);
        note.setOrigin(0, NOTE_HEIGHT);
        note.setPosition(note_pos.x, note_pos.y);

        for (auto n : state->get_selected_notes()) {
            if (n->channel == channel && n->component == components[i] && n->component_i == i) {
                note.setOutlineThickness(SELECTION_OUTLINE_THICKNESS);
                note.setOutlineColor(SELECTION_OUTLINE_COLOR);
            }
        }

        sf::Text component_text;
        component_text.setString(ImBMS::format_base36(components[i], 2));
        component_text.setFont(*state->get_font());
        component_text.setPosition(note.getPosition().x, note.getPosition().y - 12);
        component_text.setCharacterSize(FONT_SIZE);
        component_text.setFillColor(sf::Color::White);
        component_text.setOutlineThickness(1.f);
        component_text.setOutlineColor(sf::Color::Black);

        notes_m.lock();
        notes_render_v.push_back(note);
        notes_m.unlock();
        labels_m.lock();
        labels_render_v.push_back(component_text);
        labels_m.unlock();
    }
}

sf::Color Notes::get_channel_color(int channel_i, State* state) {
    std::vector<sf::Color> bm_colors = {BM_BOTTOM_NOTE_COLOR, BM_TOP_NOTE_COLOR};
    std::vector<sf::Color> pm_colors = {
                                        PM_WHITE_COLOR, PM_YELLOW_COLOR, PM_GREEN_COLOR, PM_BLUE_COLOR, 
                                        PM_RED_COLOR,
                                        PM_BLUE_COLOR, PM_GREEN_COLOR, PM_YELLOW_COLOR, PM_WHITE_COLOR
                                       };

    std::vector<sf::Color> play_channel_colors;
    switch (state->get_bms()->get_playstyle()) {
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

void Notes::render_moving_selection(State* state, sf::RenderWindow* window, sf::Vector2i mouse_pos) {
    ImVec2 viewport_size = state->get_viewport_size();
    fVec2 default_scaling = state->get_default_scaling();
    fVec2 grid_scale = state->get_grid_scale();
    sf::Color color = Notes::get_channel_color(EventHandler::get_pointed_channel(mouse_pos, state), state);
    sf::Vector2i absolute_mouse_pos = sf::Mouse::getPosition(*window);
    float note_width = (default_scaling.x*grid_scale.x)/4;

    if (state->is_selected_notes_moved()) {
        for (auto note : state->get_selected_notes()) {
            sf::RectangleShape note_render(sf::Vector2f(note_width, NOTE_HEIGHT));
            note_render.setFillColor(color);
            note_render.setOrigin(note_width/2, NOTE_HEIGHT/2);
            note_render.setPosition(absolute_mouse_pos.x, absolute_mouse_pos.y);
            note_render.setOutlineThickness(SELECTION_OUTLINE_THICKNESS);
            note_render.setOutlineColor(SELECTION_OUTLINE_COLOR);

            sf::Text component_text;
            component_text.setString(ImBMS::format_base36(note->component, 2));
            component_text.setFont(*state->get_font());
            component_text.setCharacterSize(FONT_SIZE);
            component_text.setOrigin(note_width/2, NOTE_HEIGHT/2);
            component_text.setPosition(note_render.getPosition().x, note_render.getPosition().y-3); // i have no idea where the -3 comes from
            component_text.setFillColor(sf::Color::White);
            component_text.setOutlineThickness(1.f);
            component_text.setOutlineColor(sf::Color::Black);

            moving_notes_render_v.push_back(note_render);
            moving_labels_render_v.push_back(component_text);
        }
    }
}
