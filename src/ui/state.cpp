#include "state.h"

#include <cmath>

#include "parse.h"
#include "write.h"
#include "font.h"

State::State() {
    quantization = 16;
    grid_scale = {5, 5};
    selected_keysound = 1;

    font = new sf::Font();
    ImBMS::Font::load_sfml(font);

    current_path = fs::current_path();
    absolute_pos = {0, 0};

    viewport_size = {0, 0};
    viewport_pos = {0, 0};
    default_scaling = {0, 0};
    visible_measures = 0;
    measures_wrapped = 0;
    relative_pos = {0, 0};
    wraps = {0, 0};
    wrapping_offset = {0, 0};

    is_shift_ = false;
    is_control_ = false;
    is_mouse_left_ = false;
    is_selected_notes_moved_ = false;
    is_movable_ = false;
    is_menu_bar_interacted_ = false;
    is_popup_ = false;
    view_bpm_ = true;
    view_bga_ = true;

    bms = new BMS();
    saved_bms = bms->copy();

    undo_list = {};
}

State::~State() {
    delete bms;
    delete saved_bms;
    delete font;
    clear_selected_notes();
}

int State::get_quantization() {
    return quantization;
}

void State::set_quantization(int new_quantization) {
    quantization = new_quantization;
}

fVec2 State::get_grid_scale() {
    return grid_scale;
}

void State::set_grid_scale(fVec2 new_gs) {
    grid_scale = new_gs;
}


int State::get_selected_keysound() {
    return selected_keysound;
}

void State::set_selected_keysound(int keysound) {
    selected_keysound = keysound;
}

fs::path State::get_current_path() {
    return current_path;
}

void State::set_current_path(fs::path new_path) {
    current_path = new_path;
}

fs::path State::get_filename() {
    return filename;
}

sf::Font* State::get_font() {
    return font;
}

iVec2 State::get_absolute_pos() {
    return absolute_pos;
}

void State::set_absolute_pos(iVec2 new_pos) {
    absolute_pos = new_pos;
    limit_absolute_pos();
}

void State::limit_absolute_pos() {
    if (absolute_pos.x < -viewport_size.x/grid_scale.x/2) {absolute_pos.x = -viewport_size.x/grid_scale.x/2;}
    if (absolute_pos.y < -viewport_size.y/grid_scale.y/2) {absolute_pos.y = -viewport_size.y/grid_scale.y/2;}
}

void State::update() {
    viewport_size = ImGui::GetMainViewport()->WorkSize;
    viewport_pos = ImGui::GetMainViewport()->WorkPos;

    default_scaling = {viewport_size.x/DEFAULT_SCALING_DIV, viewport_size.y/DEFAULT_SCALING_DIV};

    // Calculate the count of measures within the screen
    // Limit the count to keep the scrolling seamless at higher grid scales
    visible_measures = viewport_size.y / (default_scaling.y*grid_scale.y);
    measures_wrapped = viewport_size.y / default_scaling.y / 2;
    if (visible_measures < measures_wrapped) {visible_measures = measures_wrapped;}

    // Calculate the relative position as absolute position % viewport size accounting for scaling
    relative_pos = {std::fmod(absolute_pos.x*grid_scale.x, viewport_size.x*grid_scale.x),
                    std::fmod(absolute_pos.y*grid_scale.y, viewport_size.y*grid_scale.y)
                   };

    // Numbers of wraps is absolute position divided by viewport size accounting for scaling
    wraps = {static_cast<int>((absolute_pos.x*grid_scale.x) / (viewport_size.x*grid_scale.x)), 
             static_cast<int>((absolute_pos.y*grid_scale.y) / (viewport_size.y*grid_scale.y))
            };

    // The offsets are needed when wrapping
    wrapping_offset = {wraps.x*((std::sqrt(default_scaling.x)*2*grid_scale.x)-SCROLL_SPEED*grid_scale.x),
                       wraps.y*((std::sqrt(default_scaling.y)*2*grid_scale.y)-SCROLL_SPEED*grid_scale.y)
                      };

    note_width = (default_scaling.x*grid_scale.x)/4;

    limit_absolute_pos();
}

ImVec2 State::get_viewport_size() {
    return viewport_size;
}

ImVec2 State::get_viewport_pos() {
    return viewport_pos;
}

fVec2 State::get_default_scaling() {
    return default_scaling;
}

int State::get_visible_measures() {
    return visible_measures;
}
int State::get_measures_wrapped() {
    return measures_wrapped;
}

fVec2 State::get_relative_pos() {
    return relative_pos;
}

iVec2 State::get_wraps() {
    return wraps;
}

fVec2 State::get_wrapping_offset() {
    return wrapping_offset;
}

float State::get_note_width() {
    return note_width;
}

bool State::is_shift() {
    return is_shift_;
}

void State::set_shift(bool b) {
    is_shift_ = b;
}

bool State::is_control() {
    return is_control_;
}

void State::set_control(bool b) {
    is_control_ = b;
}

bool State::is_mouse_left() {
    return is_mouse_left_;
}

void State::set_mouse_left(bool b) {
    is_mouse_left_ = b;
}

bool State::is_selected_notes_moved() {
    return is_selected_notes_moved_; 
}

void State::set_selected_notes_moved(bool b) {
    is_selected_notes_moved_ = b;
}

bool State::is_movable() {
    return is_movable_;
}

void State::set_movable(bool b) {
    is_movable_ = b;
}

bool State::is_menu_bar_interacted() {
    return is_menu_bar_interacted_;
}

void State::set_menu_bar_interacted(bool b) {
    is_menu_bar_interacted_ = b;
}

bool State::is_popup() {
    return is_popup_;
}

void State::set_popup(bool b) {
    is_popup_ = b;
}

bool State::view_bpm() {
    return view_bpm_;
}

void State::set_view_bpm(bool b) {
    view_bpm_ = b;
}

bool State::view_bga() {
    return view_bga_;
}

void State::set_view_bga(bool b) {
    view_bga_ = b;
}

bool State::is_modified() {
    return *bms != saved_bms;
}

bool State::has_filepath() {
    return filename.has_filename();
}

BMS* State::get_bms() {
    return bms;
}

void State::set_bms(BMS* new_bms) {
    bms = new_bms;
}

bool State::load_bms(fs::path filepath) {
    if (filepath == "" || !fs::is_regular_file(filepath)) {return false;}

    // parse the given file and return if the parsing fails 
    BMS* new_bms = ImBMS::parse_bms(filepath.string());
    if (new_bms == nullptr) {return false;}

    BMS* bms_to_be_deleted = bms;
    delete bms_to_be_deleted;
    bms = new_bms;
    delete saved_bms;
    saved_bms = bms->copy();

    filename = filepath;
    current_path = filepath.parent_path();

    undo_list.clear();

    return true;
}

bool State::save_bms(fs::path filepath) {
    if (filepath == "" || !filepath.has_filename() || fs::is_directory(filepath)) {return false;}

    ImBMS::write(bms, filepath.string());
    filename = filepath;
    current_path == filepath.parent_path();
    delete saved_bms;
    saved_bms = bms->copy();

    return true;
}

void State::new_bms() {
    absolute_pos = {0, 0};

    viewport_size = {0, 0};
    viewport_pos = {0, 0};
    default_scaling = {0, 0};
    visible_measures = 0;
    measures_wrapped = 0;
    relative_pos = {0, 0};
    wraps = {0, 0};
    wrapping_offset = {0, 0};

    is_shift_ = false;
    is_control_ = false;
    is_mouse_left_ = false;
    is_selected_notes_moved_ = false;
    is_movable_ = false;
    is_menu_bar_interacted_ = false;

    filename = "";

    delete bms;
    bms = new BMS();
    delete saved_bms;
    saved_bms = bms->copy();

    undo_list = {};
}

void State::add_undo(std::function<void()> command) {
    undo_list.push_back(command);
}

void State::pop_undo() {
    undo_list.pop_back();
}

void State::undo(bool pop) {
    if (undo_list.size() < 1) {return;}

    undo_list.back()();

    if (pop) {undo_list.pop_back();}
}

void State::add_redo(std::function<void()> command) {
    redo_list.push_back(command);
}

void State::pop_redo() {
    redo_list.pop_back();
}

void State::clear_redo() {
    redo_list.clear();
}


void State::redo(bool pop) {
    if (redo_list.size() < 1) {return;}

    redo_list.back()();

    if (pop) {redo_list.pop_back();}
}

std::vector<Note*> State::get_selected_notes() {
    return selected_notes;
}

void State::set_selected_notes(std::vector<Note> notes) {
    clear_selected_notes();

    for (const auto& note : notes) {
        add_selected_note(note);
    }
}

void State::add_selected_note(Note note) {
    Note* new_note = new Note();
    *new_note = note;
    selected_notes.push_back(new_note);
}

void State::clear_selected_notes() {
    for (auto note : selected_notes) {
        delete note;
    }
    selected_notes.clear();
}

void State::play_keysound(fs::path keysound) {
    if (keysound == "") {
        return;
    }

    fs::path filepath = current_path;
    filepath /= keysound;

    // check which format the keysound actually has
    if (fs::exists(filepath.replace_extension(".wav"))) {
        audio_player.play_sample(filepath.string());

    } else if (fs::exists(filepath.replace_extension(".ogg"))) {
        audio_player.play_sample(filepath.string());
    }
}

std::vector<std::string> State::get_other_channels() {
    if (view_bpm_ && view_bga_) {
        return OTHER_CHANNELS;
    } else if (view_bpm_) {
        return BPM_CHANNELS;
    } else if (view_bga_) {
        return BGA_CHANNELS;
    }

    return {};
}
