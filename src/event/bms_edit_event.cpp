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

    } else if (channel_i < play_channels.size() + BGA_CHANNELS.size()) {
        channel_i = ImBMS::base36_to_int(BGA_CHANNELS[channel_i-play_channels.size()]);
        return BMSEditEvent::add_play_or_bga_note(component, mouse_pos, measure_i, channel_i, cell, state); 

    } else {
        channel_i = channel_i - play_channels.size() - BGA_CHANNELS.size();
        return BMSEditEvent::add_bgm_note(component, mouse_pos, measure_i, channel_i, cell, state); 
    }
}

void BMSEditEvent::remove_note(sf::Vector2i mouse_pos, State* state) {
    Note note = EventHandler::get_pointed_note(mouse_pos, state);
    if (note.channel != nullptr && note.component_i != -1) {
        int component = note.channel->components[note.component_i];
        if (component == 0) {return;}
        note.channel->components[note.component_i] = 0;
        state->add_undo(std::bind(BMSEditEvent::undo_remove_note, note.channel, note.component_i, component));
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

    } else if (pointed_channel < play_channels.size() + BGA_CHANNELS.size()) {
        pointed_channel = ImBMS::base36_to_int(BGA_CHANNELS[pointed_channel-play_channels.size()]);

    } else {
        pointed_channel = pointed_channel - play_channels.size() - BGA_CHANNELS.size();
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
}

void BMSEditEvent::remove_selected_notes(State* state) {
    for (auto note : state->get_selected_notes()) {
        int component = note->channel->components[note->component_i];
        note->channel->components[note->component_i] = 0;
        state->add_undo(std::bind(BMSEditEvent::undo_remove_note, note->channel, note->component_i, component));
    }
}

bool BMSEditEvent::add_play_or_bga_note(int component, sf::Vector2i mouse_pos, int measure_i, int channel_i, int cell, State* state) {
    BMS* bms = state->get_bms();
    std::vector<Measure*> measures = bms->get_measures();
    Channel* channel = nullptr;

    int quantization = state->get_quantization();

    std::vector<int> old_components = {};

    if (measure_i >= measures.size()) {return false;}

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

        if (quantization != channel->components.size()) {
            int common_divisor = ImBMS::get_gcd(quantization, channel->components.size());

            if (channel->components.size() < quantization || 
               (quantization != common_divisor && channel->components.size() != common_divisor)) {
                    int components_old_size = channel->components.size();
                    channel->resize(quantization/common_divisor);

                    if (quantization < components_old_size) {
                        cell *= common_divisor;
                    } else {
                        cell *= components_old_size/common_divisor;
                    }

            } else if (quantization < channel->components.size()) {
                cell *= channel->components.size()/common_divisor;
            }
        }

        if (channel->components[cell] != 0) {return false;}
        channel->components[cell] = component;
    }

    state->add_undo(std::bind(undo_add_note, measures[measure_i], channel, channel_i, old_components));
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

    if (measure->bgm_channels.size() <= channel_i) {
        for (int i = 0; i < channel_i + 1; i++) {
            measure->bgm_channels.resize(measure->bgm_channels.size() + 1, new Channel({0}));
        }
    }

    channel = measure->bgm_channels[channel_i];

    old_components = channel->components;
    if (quantization != channel->components.size()) {
        int common_divisor = ImBMS::get_gcd(quantization, channel->components.size());
        if (channel->components.size() < quantization || 
            (quantization != common_divisor && channel->components.size() != common_divisor)) 
        {
            int components_old_size = channel->components.size();
            channel->resize(quantization/common_divisor);
            if (quantization < components_old_size) {
                cell *= common_divisor;
            } else {
                cell *= components_old_size/common_divisor;
            }
        } else if (quantization < channel->components.size()) {
            cell *= channel->components.size()/common_divisor;
        }
    }
    if (channel->components[cell] != 0)  {return false;}
    channel->components[cell] = component;

    state->add_undo(std::bind(undo_add_note, measures[measure_i], channel, channel_i, old_components));
    return true;
}

void BMSEditEvent::undo_add_note(Measure* measure, Channel* channel, int channel_i, std::vector<int> old_components) {
    if (old_components.size() == 0) {
        measure->channels[channel_i] = nullptr;
        delete channel;
    } else {
        channel->components = old_components;
    }
}

void BMSEditEvent::undo_remove_note(Channel* channel, int component_i, int component) {
    channel->components[component_i] = component;
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
}
