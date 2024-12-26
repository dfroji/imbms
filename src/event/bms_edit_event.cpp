#include "bms_edit_event.h"

#include <iostream>

#include "eventhandler.h"
#include "utils.h"

bool BMSEditEvent::add_note(int component, sf::Vector2i mouse_pos, State* state) {
    if (mouse_pos.x < 0 || mouse_pos.y < 0) {return false;}

    BMS* bms = state->get_bms();
    int measure_i = EventHandler::get_pointed_measure(mouse_pos, state);
    int channel_i = EventHandler::get_pointed_channel(mouse_pos, state);
    int cell = EventHandler::get_pointed_cell(mouse_pos, state); 

    if (bms->get_measures().size() - 1 <= measure_i) {
        bms->resize_measure_v(measure_i + 1);
    }

    if (bms->get_measures()[measure_i] == nullptr) {
        bms->new_measure(measure_i);
    }

    std::vector<std::string> play_channels = bms->get_play_channels();
    if (channel_i < play_channels.size()) {
        channel_i = ImBMS::base36_to_int(play_channels[channel_i]);
        return BMSEditEvent::add_play_or_bga_note(component, mouse_pos, measure_i, channel_i, cell, state);

    } else if (channel_i < play_channels.size() + OTHER_CHANNELS.size()) {
        channel_i = ImBMS::base36_to_int(OTHER_CHANNELS[channel_i-play_channels.size()]);
        return BMSEditEvent::add_play_or_bga_note(component, mouse_pos, measure_i, channel_i, cell, state); 

    } else {
        channel_i = channel_i - play_channels.size() - OTHER_CHANNELS.size();
        return BMSEditEvent::add_bgm_note(component, mouse_pos, measure_i, channel_i, cell, state); 
    }
}

void BMSEditEvent::remove_note(sf::Vector2i mouse_pos, State* state) {
    Note note = EventHandler::get_pointed_note(mouse_pos, state);
    if (note.channel != nullptr && note.component_i != -1) {
        int component = note.channel->components[note.component_i];
        if (component == 0) {return;}
        note.channel->components[note.component_i] = 0;
        state->add_undo(std::bind(BMSEditEvent::undo_remove_note, note.channel, note.component_i, component, state));
        state->clear_redo();
    }
}

void BMSEditEvent::move_notes(sf::Vector2i mouse_pos, State* state) {
    int moved_notes = state->get_selected_notes().size();

    // cancel the move if mouse points outside the grid
    if (mouse_pos.x < 0 || mouse_pos.y < 0) {
        for (int i = 0; i < moved_notes; i++) {
            state->undo();
        }
        return;
    }

    int pointed_measure = EventHandler::get_pointed_measure(mouse_pos, state);
    int pointed_channel = EventHandler::get_pointed_channel(mouse_pos, state);
    int pointed_cell = EventHandler::get_pointed_cell(mouse_pos, state);
    BMS* bms = state->get_bms();
    bool is_bgm = false;


    // get the pointed channel adjusting for the type of channel and playstyle
    std::vector<std::string> play_channels = bms->get_play_channels();
    if (pointed_channel < play_channels.size()) {
        pointed_channel = ImBMS::base36_to_int(play_channels[pointed_channel]);

    } else if (pointed_channel < play_channels.size() + OTHER_CHANNELS.size()) {
        pointed_channel = ImBMS::base36_to_int(OTHER_CHANNELS[pointed_channel-play_channels.size()]);

    } else {
        pointed_channel = pointed_channel - play_channels.size() - OTHER_CHANNELS.size();
        is_bgm = true;
    }

    for (auto note : state->get_selected_notes()) {
        // initialize the measure if needed
        if (bms->get_measures().size() - 1 <= pointed_measure) {
            bms->resize_measure_v(pointed_measure + 1);
        }
        if (bms->get_measures()[pointed_measure] == nullptr) {
            bms->new_measure(pointed_measure);
        }

        // get the new channel's pointer for the note
        // todo: something like offsets to get correct channels for multiple notes
        // instead of just pointing to the exact same channel
        Measure* measure = bms->get_measures()[pointed_measure];
        Channel* new_channel = nullptr;
        if (!is_bgm) {
            // get the play or bga channel
            // initialize it if it's null
            new_channel = measure->channels[pointed_channel];
            if (new_channel == nullptr) {
                new_channel = new Channel();
                measure->channels[pointed_channel] = new_channel;
                std::vector<int> components = {};
                components.resize(state->get_quantization(), 0);
                new_channel->components = components;
            }
        } else {
            // rezise the measure's bgm channel vector if needed
            if (measure->bgm_channels.size() <= pointed_channel) {
                for (int i = 0; i < pointed_channel + 1; i++) {
                    measure->bgm_channels.resize(measure->bgm_channels.size() + 1, new Channel({0}));
                }
            }

            // get the bgm channel
            // initialize it if it's null
            new_channel = measure->bgm_channels[pointed_channel];
            if (new_channel == nullptr) {
                new_channel = new Channel();
                measure->channels[pointed_channel] = new_channel;
            }
        }

        note->channel = new_channel;
        note->component_i = pointed_cell;
        note->measure_i = pointed_measure;
        note->channel_i = pointed_channel;

        // add the notes to the bms object
        // cancel the move if adding any of the notes fails
        if (!BMSEditEvent::add_note(note->component, mouse_pos, state)) {
            for (int i = 0; i < moved_notes; i++) {
                state->undo();
            }
            return;
        }
    }

    state->add_undo(std::bind(BMSEditEvent::undo_move_notes, moved_notes, state));
    state->clear_redo();
}

void BMSEditEvent::remove_selected_notes(State* state) {
    for (auto note : state->get_selected_notes()) {
        int component = note->channel->components[note->component_i];
        note->channel->components[note->component_i] = 0;
        state->add_undo(std::bind(BMSEditEvent::undo_remove_note, note->channel, note->component_i, component, state));
    }
}

bool BMSEditEvent::add_play_or_bga_note(int component, sf::Vector2i mouse_pos, int measure_i, int channel_i, int cell, State* state) {
    BMS* bms = state->get_bms();
    std::vector<Measure*> measures = bms->get_measures();
    Channel* channel = nullptr;

    int quantization = state->get_quantization();

    std::vector<int> old_components = {};

    if (measure_i >= measures.size()) {return false;}

    // create a new channel if it doesn't exist yet
    // otherwise modify the channel
    if (measures[measure_i]->channels[channel_i] == nullptr) {
        channel = new Channel();
        measures[measure_i]->channels[channel_i] = channel;
        std::vector<int> components = {};
        components.resize(quantization, 0);
        components[cell] = component;
        channel->components = components;

    } else {
        channel = measures[measure_i]->channels[channel_i];
        old_components = channel->components;

        // change the channel's quantization if it doesn't match with the quantization used
        BMSEditEvent::adjust_quantization(cell, quantization, channel);

        // channel is not modified if a note on the specific cell already exists
        if (channel->components[cell] != 0) {return false;}

        // add the note
        channel->components[cell] = component;
    }

    state->add_undo(std::bind(undo_add_note, channel, old_components, state));
    state->clear_redo();

    return true;
}

bool BMSEditEvent::add_bgm_note(int component, sf::Vector2i mouse_pos, int measure_i, int channel_i, int cell, State* state) {
    BMS* bms = state->get_bms();
    std::vector<Measure*> measures = bms->get_measures();
    
    if (measures.size() <= measure_i) {return false;}
    
    Measure* measure = measures[measure_i];
    Channel* channel = nullptr;
    std::vector<int> old_components;

    int quantization = state->get_quantization();

    // add new bgm channels if needed
    int bgm_channels_size = measure->bgm_channels.size();
    if (bgm_channels_size <= channel_i) {
        for (int i = bgm_channels_size; i < channel_i + 1; i++) {
            measure->bgm_channels.resize(measure->bgm_channels.size() + 1, new Channel({0}));
        }
    }

    channel = measure->bgm_channels[channel_i];
    old_components = channel->components;

    // adjust quantization if needed
    BMSEditEvent::adjust_quantization(cell, quantization, channel);

    if (channel->components[cell] != 0)  {return false;}
    channel->components[cell] = component;

    state->add_undo(std::bind(undo_add_note, channel, old_components, state));
    state->clear_redo();
    return true;
}

void BMSEditEvent::undo_add_note(Channel* channel, std::vector<int> old_components, State* state) {
    std::vector<int> components = channel->components;
    channel->components = old_components;

    // if the channel's component vector now has no items (ie. the channel object was created when the note was added),
    // resize it to the current quantization instead of keeping it 0-size.
    // this may or may not have consequences along the line so I'll have to keep this in mind
    if (channel->components.size() == 0) {
        channel->components.resize(state->get_quantization(), 0);
    }

    state->add_redo(std::bind(redo_add_note, channel, components, state));
}

void BMSEditEvent::redo_add_note(Channel* channel, std::vector<int> components, State* state) {
    std::vector<int> old_components = channel->components;
    channel->components = components;

    state->add_undo(std::bind(undo_add_note, channel, old_components, state));
}

void BMSEditEvent::undo_remove_note(Channel* channel, int component_i, int component, State* state) {
    channel->components[component_i] = component;
    state->add_redo(std::bind(redo_remove_note, channel, component_i, component, state));
}

void BMSEditEvent::redo_remove_note(Channel* channel, int component_i, int component, State* state) {
    channel->components[component_i] = 0;
    state->add_undo(std::bind(undo_remove_note, channel, component_i, component, state));
}

void BMSEditEvent::undo_move_notes(int moved_notes, State* state) {
    // undo list needs to be popped or undo_move_notes() will be called indefinitely 
    state->pop_undo();
    
    // moved notes multiplied by two as there are two queued undos for each moved note,
    // except one undo needs to be passed with pop=false to count for the manual pop above.
    // not particularly elegant solution but whatever
    for (int i = 0; i < moved_notes*2-1; i++) {
        state->undo();
    }
    state->undo(false);
    state->add_redo(std::bind(redo_move_notes, moved_notes, state));
}

void BMSEditEvent::redo_move_notes(int moved_notes, State* state) {
    // the comments for undo_move_notes() also apply here

    state->pop_redo();

    for (int i = 0; i < moved_notes*2-1; i++) {
        state->redo();
    }
    state->redo(false);
    state->add_undo(std::bind(undo_move_notes, moved_notes, state));
}

void BMSEditEvent::adjust_quantization(int& cell, int quantization, Channel* channel) {
    int components_size = channel->components.size();

    if (quantization != components_size) {
        int common_divisor = ImBMS::get_gcd(quantization, components_size);
        cell *= components_size/common_divisor; // adjust the cell to match the resulting quantization

        // adjust the quantization of the channel if the used quantization is higher
        // or if either the used quantization or the quantization of the channel is not the common divisor
        if (components_size < quantization || 
           (quantization != common_divisor && components_size != common_divisor)) {
                channel->resize(quantization/common_divisor);
        }
    }
}
