#pragma once

#include <filesystem>
#include <functional>

#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/Graphics/Text.hpp"

#include "channels_and_colors.h"
#include "bms.h"
#include "audio.h"

namespace fs = std::filesystem;

const int FONT_SIZE_SFML = 12;
const int SCROLL_SPEED = 20;
const int PADDING = 3;
const int DEFAULT_SCALING_DIV = 20;
const float SIDE_MENU_WIDTH = 0.2f;

struct iVec2 {
    int x;
    int y;
};

struct fVec2 {
    float x;
    float y;
};

struct Note {
    Channel* channel = nullptr;
    int component_i = -1;
    int component = -1;
    int measure_i = -1;
    int channel_i = -1;
    bool operator==(const Note& rhs) {
        return (this->channel == rhs.channel && this->component_i == rhs.component_i);
    }
    bool operator!=(const Note& rhs) {
        return (this->channel != rhs.channel && this->component_i != rhs.component_i);
    }
};
const Note NULL_NOTE = Note(nullptr, -1, -1, -1, -1);

class State {
    public:
        State();
        ~State();

        int get_quantization(); 
        void set_quantization(int new_quantization);

        fVec2 get_grid_scale();
        void set_grid_scale(fVec2 new_gs);

        int get_selected_keysound();
        void set_selected_keysound(int keysound);

        fs::path get_current_path();
        void set_current_path(fs::path path);

        fs::path get_filename();

        sf::Font* get_font();

        iVec2 get_absolute_pos();
        void set_absolute_pos(iVec2 new_pos);

        void update();

        ImVec2 get_viewport_size();
        ImVec2 get_viewport_pos();
        fVec2 get_default_scaling();
        int get_visible_measures();
        int get_measures_wrapped();
        fVec2 get_relative_pos();
        iVec2 get_wraps();
        fVec2 get_wrapping_offset();
        float get_note_width();

        bool is_shift();
        void set_shift(bool b);

        bool is_control();
        void set_control(bool b);

        bool is_mouse_left();
        void set_mouse_left(bool b);

        bool is_selected_notes_moved();
        void set_selected_notes_moved(bool b);
        
        bool is_movable();
        void set_movable(bool b);

        bool is_menu_bar_interacted();
        void set_menu_bar_interacted(bool b);

        bool is_popup();
        void set_popup(bool b);

        bool view_bpm();
        void set_view_bpm(bool b);

        bool view_bga();
        void set_view_bga(bool b);

        bool is_modified();

        bool has_filepath();

        BMS* get_bms();
        void set_bms(BMS* new_bms);
        bool load_bms(fs::path filepath);
        bool save_bms(fs::path filepath);
        void new_bms();

        void add_undo(std::function<void()> command);
        void pop_undo();
        void clear_redo();
        void undo(bool pop = true);

        void add_redo(std::function<void()> command);
        void pop_redo();
        void redo(bool pop = true);

        std::vector<Note*> get_selected_notes();
        void set_selected_notes(std::vector<Note> notes);
        void add_selected_note(Note note);
        void clear_selected_notes(); 

        void play_keysound(fs::path keysound);

        std::vector<std::string> get_other_channels();

    private:
        int quantization;
        fVec2 grid_scale;
        int selected_keysound;
        fs::path current_path;
        fs::path filename;
        sf::Font* font;
        iVec2 absolute_pos;

        ImVec2 viewport_size;
        ImVec2 viewport_pos;
        fVec2 default_scaling;
        int visible_measures;
        int measures_wrapped;
        fVec2 relative_pos;
        iVec2 wraps;
        fVec2 wrapping_offset;
        float note_width;

        bool is_shift_;
        bool is_control_;
        bool is_mouse_left_;
        bool is_selected_notes_moved_;
        bool is_movable_;
        bool is_menu_bar_interacted_;
        bool is_popup_;
        bool view_bpm_;
        bool view_bga_;

        BMS* bms;
        BMS* saved_bms;

        std::vector<std::function<void()>> undo_list;
        std::vector<std::function<void()>> redo_list;

        std::vector<Note*> selected_notes;
        
        Audio audio_player;
};
