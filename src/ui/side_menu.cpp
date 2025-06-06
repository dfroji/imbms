#include "side_menu.h"

#include <cstring>

#include "filedialog.h"

const float SCALE_MIN = 1.0f;
const float SCALE_MAX = 20.0f;

SideMenu::SideMenu() {

}

SideMenu::~SideMenu() {

}

void SideMenu::render(State* state, BMS* bms) {
    this->state = state;
    this->bms = bms;

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;

    ImGuiTreeNodeFlags cheader_flags = 0;
    cheader_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    ImGuiWindowFlags popup_flags = 0;
    popup_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    popup_flags |= ImGuiWindowFlags_NoMove;

    ImVec2 viewport_pos = ImGui::GetMainViewport()->GetWorkCenter();
    ImVec2 viewport_size = ImGui::GetMainViewport()->WorkSize;

    ImVec2 size;
    size.x = viewport_size.x * SIDE_MENU_WIDTH;
    size.y = viewport_size.y;

    ImVec2 pos;
    pos.x = viewport_pos.x + viewport_size.x / 2 - size.x;
    pos.y = viewport_pos.y - viewport_size.y / 2;

    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowBgAlpha(1.0f);

    ImGui::Begin("Side menu", NULL, window_flags);

    // static values used in a collapsing header are initialized outside of it
    // as they wouldn't update propely when loading a file and the header is open
    static int current_mode = bms->get_playstyle();
    static int playlevel = std::stoi(bms->get_header_data("#PLAYLEVEL"));
    static double bpm = std::stod(bms->get_header_data("#BPM"));
    static int total = std::stoi(bms->get_header_data("#TOTAL"));
    static int current_rank = std::stoi(bms->get_header_data("#RANK"));
    static int current_difficulty = std::stoi(bms->get_header_data("#DIFFICULTY"));
    static int selected_keysound = state->get_selected_keysound() - 1;
    static int selected_lnobj = ImBMS::base36_to_int(bms->get_header_data("#LNOBJ"));
    if (ImGui::CollapsingHeader("Metadata", NULL, cheader_flags)) {
        static char title[1024] = "", subtitle[1024] = "";
        static char artist[1024] = "", subartist[1024] = "";
        static char genre[1024] = "";
        std::strcpy(title, ImBMS::sjis_to_utf8(bms->get_header_data("#TITLE")).c_str());
        std::strcpy(subtitle, ImBMS::sjis_to_utf8(bms->get_header_data("#SUBTITLE")).c_str());
        std::strcpy(artist, ImBMS::sjis_to_utf8(bms->get_header_data("#ARTIST")).c_str());
        std::strcpy(subartist, ImBMS::sjis_to_utf8(bms->get_header_data("#SUBARTIST")).c_str());
        std::strcpy(genre, ImBMS::sjis_to_utf8(bms->get_header_data("#GENRE")).c_str());

        ImGui::InputText("Title", title, IM_ARRAYSIZE(title));
        bms->insert_header_data("#TITLE", ImBMS::utf8_to_sjis(title));

        ImGui::InputText("Subtitle", subtitle, IM_ARRAYSIZE(subtitle));        
        bms->insert_header_data("#SUBTITLE", ImBMS::utf8_to_sjis(subtitle));

        ImGui::InputText("Artist", artist, IM_ARRAYSIZE(artist));
        bms->insert_header_data("#ARTIST", ImBMS::utf8_to_sjis(artist));

        ImGui::InputText("Subartist", subartist, IM_ARRAYSIZE(subartist));
        bms->insert_header_data("#SUBARTIST", ImBMS::utf8_to_sjis(subartist));

        ImGui::InputText("Genre", genre, IM_ARRAYSIZE(genre));
        bms->insert_header_data("#GENRE", ImBMS::utf8_to_sjis(genre));

        const char* modes[] = {"SP", "DP", "PM"};
        current_mode = bms->get_playstyle();
        if (ImGui::Combo("Mode", &current_mode, modes, IM_ARRAYSIZE(modes))) {
            bms->set_playstyle(static_cast<Playstyle>(current_mode));
        }

        playlevel = std::stoi(bms->get_header_data("#PLAYLEVEL"));
        if (ImGui::InputInt("Level", &playlevel)) {
            if (playlevel < 0) {playlevel = 0;} 
            bms->insert_header_data("#PLAYLEVEL", std::to_string(playlevel));
        }

        bpm = std::stod(bms->get_header_data("#BPM"));
        if (ImGui::InputDouble("BPM", &bpm, 1.0f, 10.0f, "%.0f")) {
            bms->insert_header_data("#BPM", std::to_string(bpm));
        }

        total = std::stoi(bms->get_header_data("#TOTAL"));
        if (ImGui::InputInt("Total", &total)) {
            if (total < 1) {total = 1;} 
            bms->insert_header_data("#TOTAL", std::to_string(total));
        }

        const char* ranks[] = {"Very Hard", "Hard", "Normal", "Easy"};
        current_rank = std::stoi(bms->get_header_data("#RANK"));
        if (ImGui::Combo("Judge", &current_rank, ranks, IM_ARRAYSIZE(ranks))) {
            bms->insert_header_data("#RANK", std::to_string(static_cast<Rank>(current_rank)));
        }

        const char* difficulties[] = {"Beginner", "Normal", "Hyper", "Another", "Insane"};
        current_difficulty = std::stoi(bms->get_header_data("#DIFFICULTY")) - 1;
        if (ImGui::Combo("Difficulty", &current_difficulty, difficulties, IM_ARRAYSIZE(difficulties))) {
            bms->insert_header_data("#DIFFICULTY", std::to_string(static_cast<Difficulty>(current_difficulty + 1)));
        }

        std::vector<char*> index_labels = get_index_labels(DATA_LIMIT, 2);
        selected_lnobj = ImBMS::base36_to_int(bms->get_header_data("#LNOBJ"));
        // +1 to count for the "None" selection
        if (ImGui::Combo("LNObj", &selected_lnobj, index_labels.data(), DATA_LIMIT+1)) {
            if (selected_lnobj == 0) {
                bms->remove_header_data("#LNOBJ");

            } else {
                bms->insert_header_data("#LNOBJ", ImBMS::format_base36(selected_lnobj, 2));
            }
        }

        // free the memory allocated for the index labels
        for (auto c : index_labels) {
            delete c;
        }

    }

    // header and listbox for keysounds
    if (ImGui::CollapsingHeader("Keysounds")) {
        std::vector<char*> keysound_labels = get_keysound_labels(DATA_LIMIT, 2);

        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::ListBox("##keysounds_list", &selected_keysound, keysound_labels.data(), DATA_LIMIT, 10)) {
            state->set_selected_keysound(selected_keysound + 1);
        }
        if (ImGui::IsItemHovered()) {
            // open a file dialog if an item on the keysound list is double clicked
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                FileDialog fd(state);
                fs::path filepath = fd.open_file(state->get_current_path(), FDMode::Keysounds);
                std::string file = filepath.filename().string();
                if (file != "") {
                    bms->set_keysound(file, state->get_selected_keysound());
                }
            }
        }

        // free the memory allocated for the labels
        for (auto c : keysound_labels) {
            delete c;
        }
    }

    // header and listbox for graphics
    if (ImGui::CollapsingHeader("Graphics")) {
        std::vector<char*> graphic_labels = get_graphic_labels(DATA_LIMIT, 2);

        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::ListBox("##graphics_list", &selected_keysound, graphic_labels.data(), DATA_LIMIT, 10)) {
            state->set_selected_keysound(selected_keysound + 1);
        }

        // open a file dialog if an item is double clicked
        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                FileDialog fd(state);
                fs::path filepath = fd.open_file(state->get_current_path(), FDMode::Graphics);
                std::string file = filepath.filename().string();
                if (file != "") {
                    bms->set_graphic(file, state->get_selected_keysound());
                }
            }
        }

        // free allocated memory
        for (auto c : graphic_labels) {
            delete c;
        }
    }
    
    // header and listbox for bpm changes
    // also implementes a modal popup for bpm input
    static double exbpm;
    if (ImGui::CollapsingHeader("Extended BPM")) {
        std::vector<char*> exbpm_labels = get_exbpm_labels(DATA_LIMIT, 2);

        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::ListBox("##exbpms_list", &selected_keysound, exbpm_labels.data(), DATA_LIMIT, 10)) {
            state->set_selected_keysound(selected_keysound + 1);

            // set exbpm here so the correct value is shown in the modal popup
            std::string exbpm_str = bms->get_bpm_changes()[selected_keysound + 1];
            if (exbpm_str != "") {
                exbpm = std::stod(exbpm_str);

            } else {
                exbpm = DEFAULT_BPM;
            }
        }

        // open a modal popup when an item is double clicked
        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                ImGui::OpenPopup("Input exBPM"); 
                state->set_popup(true);
            }
        }

        // set the modal popup to always be in the center of the viewport
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Input exBPM", NULL, popup_flags)) {
            std::string selected_exbpm_str = bms->get_bpm_changes()[selected_keysound + 1];
            if (selected_exbpm_str != "") {
                static double exbpm = std::stod(selected_exbpm_str);
            }

            (ImGui::InputDouble("BPM", &exbpm, 0.0f, 10000.0f, "%.2f"));

            ImGui::Separator();

            if (ImGui::Button("Cancel", BUTTON_SIZE)) {
                ImGui::CloseCurrentPopup();
                state->set_popup(false);
            }

            ImGui::SameLine();

            // right align the ok button
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - BUTTON_SIZE.x);

            if (ImGui::Button("OK", BUTTON_SIZE)) {
                bms->set_bpm_change(std::to_string(exbpm), selected_keysound + 1);
                ImGui::CloseCurrentPopup();
                state->set_popup(false);
            }

            ImGui::EndPopup();
        }

        // free allocated memory
        for (auto c : exbpm_labels) {
            delete c;
        }
    }

    // header for grid settings
    int quantization = state->get_quantization();
    fVec2 grid_scale = state->get_grid_scale();
    static bool timewise_lock = state->is_timewise_locked();
    if (ImGui::CollapsingHeader("Grid", NULL, cheader_flags)) {
        if (ImGui::InputInt("Quantization", &quantization)) {
            if (quantization < 1) {quantization = 1;}
            state->set_quantization(quantization);

        }

        if (ImGui::SliderFloat("X scale", &grid_scale.x, SCALE_MIN, SCALE_MAX, "%.1f")) {
            if (grid_scale.x < SCALE_MIN) {grid_scale.x = SCALE_MIN;}
            if (grid_scale.x > SCALE_MAX) {grid_scale.x = SCALE_MAX;}
            state->set_grid_scale(grid_scale);
        }

        if (ImGui::SliderFloat("Y scale", &grid_scale.y, SCALE_MIN, SCALE_MAX, "%.1f")) {
            if (grid_scale.y < SCALE_MIN) {grid_scale.y = SCALE_MIN;}
            if (grid_scale.y > SCALE_MAX) {grid_scale.y = SCALE_MAX;}
            state->set_grid_scale(grid_scale);
        }

        ImGui::Separator();

        if (ImGui::Checkbox("Timewise lock", &timewise_lock)) {
            state->set_timewise_lock(timewise_lock);
        }
        ImGui::SameLine();
        ImGui::TextDisabled("?");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Disables moving notes vertically");
        }
    }

    ImGui::End();
}

std::vector<char*> SideMenu::get_keysound_labels(int size, int digits) {
    std::vector<std::string> labels = {};
    std::vector<std::string> keysounds = bms->get_keysounds();
 
    // push labels to labels in a loop
    // start at 1 as keysounds[0] is reserved for empty notes and isn't displayed
    for (int i = 1; i < size+1; i++) {
        fs::path keysound = "";
        if (i < keysounds.size()) {
            keysound += keysounds[i];

        }

        // push the index in base36 as the label if the keysound is unassigned
        // continue to skip extension checks
        if (keysound == "") {
            labels.push_back(ImBMS::format_base36(i, 2));
            continue;
        }

        // change the extension of the label in the case the actual sound files
        // are different format than in the parsed bms file.
        fs::path fullpath = state->get_current_path();
        fullpath /= keysound;
        if (fs::exists(fullpath.replace_extension(".wav"))) {
            keysound.replace_extension(".wav");
            labels.push_back(ImBMS::format_base36(i, 2) + " " + keysound.generic_string());

        } else if (fs::exists(fullpath.replace_extension(".ogg"))) {
            keysound.replace_extension(".ogg");
            labels.push_back(ImBMS::format_base36(i, 2) + " " + keysound.generic_string());

        // if the keysound file doesn't exist push the same label as if it was unassigned
        } else {
            labels.push_back(ImBMS::format_base36(i, 2) + " MISSING FILE");
        }
    }

    // transform the vector of strings to vector of char* for imgui's listbox to use
    // ImBMS::cstr allocates memory for each label so it has to be freed wherever this function is called
    std::vector<char*> labels_c;
    std::transform(labels.begin(), labels.end(), std::back_inserter(labels_c), ImBMS::cstr);

    return labels_c;
}

std::vector<char*> SideMenu::get_graphic_labels(int size, int digits) {
    std::vector<std::string> labels;
    std::vector<std::string> graphics = bms->get_graphics();

    // push labels into a vector in loop
    // start at 1 as index 0 is reserved for empty notes
    for (int i = 1; i < size; i++) {
        fs::path graphic = "";
        if (i < graphics.size()) {
            graphic += graphics[i];
        }

        if (graphic == "") {
            labels.push_back(ImBMS::format_base36(i, 2));
            continue;
        } 

        // also search for .png and .jpg files if the format defined in the file can't be found
        fs::path fullpath = state->get_current_path();
        fullpath /= graphic;
        if (fs::exists(fullpath)) {
            labels.push_back(ImBMS::format_base36(i, 2) + " " + graphic.generic_string());

        } else if (fs::exists(fullpath.replace_extension(".png"))) {
            graphic.replace_extension(".png");
            labels.push_back(ImBMS::format_base36(i, 2) + " " + graphic.generic_string());

        } else if (fs::exists(fullpath.replace_extension(".jpg"))) {
            graphic.replace_extension(".jpg");
            labels.push_back(ImBMS::format_base36(i, 2) + " " + graphic.generic_string());

        } else {
            labels.push_back(ImBMS::format_base36(i, 2) + " MISSING FILE");
        }
    }

    // transform the vector of strings to a vector of char*
    // ImBMS::cstr allocates memory which needs to be freed later
    std::vector<char*> labels_c;
    std::transform(labels.begin(), labels.end(), std::back_inserter(labels_c), ImBMS::cstr);

    return labels_c;
}

std::vector<char*> SideMenu::get_exbpm_labels(int size, int digits) {
    std::vector<std::string> labels;
    std::vector<std::string> exbpms = bms->get_bpm_changes();

    // push labels into a vector in a loop
    // start at 1 as index 0 is reserved for empty notes
    for (int i = 1; i < size; i++) {
        fs::path exbpm = "";
        if (i < exbpms.size()) {
            exbpm += exbpms[i];
        }

        if (exbpm == "") {
            labels.push_back(ImBMS::format_base36(i, 2));
        } else {
            labels.push_back(ImBMS::format_base36(i, 2) + " " + exbpm.generic_string());
        }
    }

    // transform strings to char*
    // ImBMS::cstr allocates memory that needs to be freed later
    std::vector<char*> labels_c;
    std::transform(labels.begin(), labels.end(), std::back_inserter(labels_c), ImBMS::cstr);

    return labels_c;
}

std::vector<char*> SideMenu::get_index_labels(int size, int digits) {
    std::vector<std::string> labels = {};


    // push indices to labels starting with "None" as the first label
    labels.push_back("None");
    for (int i = 1; i < size+1; i++) {
        labels.push_back(ImBMS::format_base36(i, 2));
    }

    // transform the vector of strings to vector of char* for imgui's listbox to use
    // ImBMS::cstr allocates memory for each label so it has to be freed wherever this function is called
    std::vector<char*> labels_c;
    std::transform(labels.begin(), labels.end(), std::back_inserter(labels_c), ImBMS::cstr);

    return labels_c;
}
