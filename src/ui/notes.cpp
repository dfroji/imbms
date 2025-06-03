#include "notes.h"

#include <limits>
#include <cmath>

#include "eventhandler.h"
#include "font.h"

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

    // limit the rendered measures of notes to only visible measures
    // further limitations occur when the notes are processed 
    int first_visible = state->get_wraps().y*state->get_measures_wrapped();
    int last_visible = first_visible+999; // arbitrary number with which long notes get rendered in most cases.
                                          // i think only very specific incredibly high bpm gimmicks with lns break this,
                                          // but i'm not sure if that's worth caring about

    std::vector<std::string> channels = state->get_bms()->get_play_channels();

    // process the notes in threads
    // one thread per measure
    std::vector<std::thread> threads = {};
    for (int measure_i = first_visible; measure_i < measures.size() && measure_i < last_visible; measure_i++) {
        if (measures[measure_i] == nullptr) {continue;}
        threads.push_back(std::thread(&Notes::render_thread, this, measure_i, channels, state));
    }
    render_moving_selection(state, window, mouse_pos); // also render moved notes

    // join the threads and draw all the elements processed in the threads
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
    render_bga_channels(measure_i, state->get_other_channels(), channels.size(), state);
    render_bgm_channels(measure_i, channels.size()+state->get_other_channels().size(), state);
}

void Notes::render_play_channels(int measure_i, std::vector<std::string> channels, State* state) {
    Measure* measure = state->get_bms()->get_measures()[measure_i];
    for (int channel_i = 0; channel_i < channels.size(); channel_i++) {
        int channel_id = ImBMS::base36_to_int(channels[channel_i]);
        Channel* channel = measure->channels[channel_id];
        if (channel == nullptr) {continue;}
        render_channel_notes(measure_i, channel_i, channel, get_channel_color(channel_i, state), state, channel_id);
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
        render_channel_notes(measure_i, channel_i + offset, channel, OTHER_COLOR, state);
    }
}

// get the position of the long note head
float Notes::get_ln_start_pos(int measure_i, int channel_id, State* state, int ln_end_i) {
    // long note head doesn't exist if; return NaN
    if (measure_i < 0) {return std::numeric_limits<float>::quiet_NaN();}

    BMS* bms = state->get_bms();
    Measure* measure = bms->get_measures()[measure_i];
    if (measure == nullptr) {return Notes::get_ln_start_pos(measure_i-1, channel_id, state);} // continue to next measure if current measure is a nullptr

    Channel* channel = measure->channels[channel_id];
    if (channel == nullptr) {return Notes::get_ln_start_pos(measure_i-1, channel_id, state);} // continue to next measure if the channel of the current measure is a nullptr

    std::vector<int> components = channel->components;

    iVec2 absolute_pos = state->get_absolute_pos();
    fVec2 grid_scale = state->get_grid_scale();
    fVec2 default_scaling = state->get_default_scaling();
    ImVec2 viewport_size = state->get_viewport_size();
    ImVec2 viewport_pos = state->get_viewport_pos();
    float note_width = (default_scaling.x*grid_scale.x)/4;

    // start iteration from the last component of the channel
    // or if the measure is the same as the long note end's, start iteration from the component before the end
    int last = components.size();
    if (ln_end_i > 0) {last = ln_end_i;}

    // if a note exists, calcualte and return its y position
    for (int i = last-1; i >= 0; i--) {
        if (components[i] != 0) {

            return absolute_pos.y*grid_scale.y + viewport_size.y + viewport_pos.y - 2*measure_i*default_scaling.y*grid_scale.y - ((2*default_scaling.y*grid_scale.y)/(components.size()))*i;

        }
    }

    // continue to next measure if long note head was not found in the current measure
    return Notes::get_ln_start_pos(measure_i-1, channel_id, state);
}

void Notes::render_channel_notes(int measure_i, int channel_i, Channel* channel, sf::Color color, State* state, int channel_id) {
    iVec2 absolute_pos = state->get_absolute_pos();
    fVec2 grid_scale = state->get_grid_scale();
    fVec2 default_scaling = state->get_default_scaling();
    ImVec2 viewport_size = state->get_viewport_size();
    ImVec2 viewport_pos = state->get_viewport_pos();
    float note_width = (default_scaling.x*grid_scale.x)/4;

    std::vector<int> components = channel->components;

    for (int i = 0; i < components.size(); i++) {
        if (components[i] == 0) {continue;}

        // calculate the position of the note with this hell
        fVec2 note_pos = {-absolute_pos.x*grid_scale.x + channel_i*note_width,
                          absolute_pos.y*grid_scale.y + viewport_size.y + viewport_pos.y - 2*measure_i*default_scaling.y*grid_scale.y - ((2*default_scaling.y*grid_scale.y)/(components.size()))*i
                         };

        // long note (strict bml spec)
        // if the processed note matches LNOBJ, draw a long note until the long notr head (if it exists)
        if (ImBMS::format_base36(components[i], 2) == state->get_bms()->get_header_data("#LNOBJ")) {
            float ln_width = note_width*0.7;

            float ln_start_pos = get_ln_start_pos(measure_i, channel_id, state, i);

            if (!std::isnan(ln_start_pos)) {
                sf::RectangleShape ln(sf::Vector2f(ln_width, ln_start_pos-note_pos.y));
                ln.setFillColor(color);
                ln.setOrigin(0, NOTE_HEIGHT);
                ln.setPosition(note_pos.x+(note_width-ln_width)/2, note_pos.y);

                notes_m.lock();
                notes_render_v.push_back(ln);
                notes_m.unlock();
            }
        }

        // continue if the note is not visible
        if (note_pos.y < 0 || note_pos.y > viewport_size.y + viewport_pos.y) {continue;}

        // the rectangle for the note
        sf::RectangleShape note(sf::Vector2f(note_width, NOTE_HEIGHT));
        note.setFillColor(color);
        note.setOrigin(0, NOTE_HEIGHT);
        note.setPosition(note_pos.x, note_pos.y);

        // give selected notes an outline
        for (auto n : state->get_selected_notes()) {
            if (n->channel == channel && n->component == components[i] && n->component_i == i) {
                note.setOutlineThickness(SELECTION_OUTLINE_THICKNESS);
                note.setOutlineColor(SELECTION_OUTLINE_COLOR);
            }
        }

        // label for the note
        std::string note_label = get_note_label(components[i], channel_i, state);
        sf::Text component_text;
        component_text.setString(note_label);
        component_text.setFont(*state->get_font());
        component_text.setPosition(note.getPosition().x, note.getPosition().y - 12);
        component_text.setCharacterSize(FONT_SIZE_SFML);
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
    else if (channel_i < play_channel_colors.size() + state->get_other_channels().size()) {return OTHER_COLOR;}
    else {return BGM_COLOR;}
}

std::string Notes::get_note_label(int component, int channel_i, State* state) {
    int play_channels_size = state->get_bms()->get_play_channels().size();

    // bpm channels get unique labels that show the bpm value instead of base36 index
    if (state->view_bpm() && play_channels_size <= channel_i && channel_i < play_channels_size + BPM_CHANNELS.size()) {
        // channel 3 components need to be hexified because they're parsed in base36 despite being hexadecimal
        // case 0 is basic bpm (channel 3)
        // case 1 is extended bpm (channel 8)
        int hex = ImBMS::hexify(component);
        switch (channel_i - play_channels_size) {
            case 0:
                if (hex < HEX_LIMIT && hex != -1) {
                    return std::to_string(hex);
                } else {
                    return "ERROR";
                }
            case 1:
                std::string label = "N/A";
                std::vector<std::string> bpm_changes = state->get_bms()->get_bpm_changes();
                if (component < bpm_changes.size()) {
                    std::string bpm_change = bpm_changes[component];
                    if (bpm_change != "") {
                        label = bpm_change;
                    }
                }

                return label;
        }

    }
    
    return ImBMS::format_base36(component, 2);
}

void Notes::render_moving_selection(State* state, sf::RenderWindow* window, sf::Vector2i mouse_pos) {
    ImVec2 viewport_pos = state->get_viewport_pos();
    ImVec2 viewport_size = state->get_viewport_size();
    iVec2 absolute_pos = state->get_absolute_pos();
    fVec2 default_scaling = state->get_default_scaling();
    fVec2 grid_scale = state->get_grid_scale();
    sf::Color color = Notes::get_channel_color(EventHandler::get_pointed_channel(mouse_pos, state), state);
    sf::Vector2i absolute_mouse_pos = sf::Mouse::getPosition(*window);
    float note_width = state->get_note_width(); 

    if (state->is_selected_notes_moved()) {
        for (auto note : state->get_selected_notes()) {
            // right now it just renders the notes to the mouse as selection of multiple notes is not implemented
            // todo: support for multiple moved notes with offsets perhaps 

            // rectangle for the note
            // selection outline included as it is a selected note
            sf::RectangleShape note_render(sf::Vector2f(note_width, NOTE_HEIGHT));
            note_render.setFillColor(color);

            sf::Vector2f note_position = {0.f, 0.f};
            sf::Vector2f origin = {0.f, 0.f};
            if (state->is_timewise_locked()) {
                note_position.x = absolute_mouse_pos.x;
                note_position.y = absolute_pos.y*grid_scale.y + viewport_size.y + viewport_pos.y - 2*note->measure_i*default_scaling.y*grid_scale.y - ((2*default_scaling.y*grid_scale.y)/note->quantization*note->component_i);
                origin = {note_width/2, NOTE_HEIGHT};

            } else {
                note_position.x = absolute_mouse_pos.x;
                note_position.y = absolute_mouse_pos.y;
                origin = {note_width/2, NOTE_HEIGHT/2};
            }

            note_render.setPosition(note_position);
            note_render.setOrigin(origin);
            note_render.setOutlineThickness(SELECTION_OUTLINE_THICKNESS);
            note_render.setOutlineColor(SELECTION_OUTLINE_COLOR);

            // label for the note
            sf::Text component_text;
            component_text.setString(ImBMS::format_base36(note->component, 2));
            component_text.setFont(*state->get_font());
            component_text.setCharacterSize(FONT_SIZE_SFML);
            component_text.setOrigin(origin);
            component_text.setPosition(note_render.getPosition().x, note_render.getPosition().y-3); // i have no idea where the -3 comes from
            component_text.setFillColor(sf::Color::White);
            component_text.setOutlineThickness(1.f);
            component_text.setOutlineColor(sf::Color::Black);

            moving_notes_render_v.push_back(note_render);
            moving_labels_render_v.push_back(component_text);
        }
    }
}
