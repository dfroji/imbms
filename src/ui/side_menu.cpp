#include "side_menu.h"

#include <cstring>

#include "filedialog.h"

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
    static double bpm = bms->get_bpm();
    static int total = bms->get_total();
    static int current_rank = bms->get_rank();
    if (ImGui::CollapsingHeader("Metadata")) {
        static char title[1024] =  "", subtitle[1024] = "";
        static char artist[1024] = "", subartist[1024] = "";
        static char genre[1024] = "";
        std::strcpy(title, bms->get_title().c_str());
        std::strcpy(subtitle, bms->get_subtitle().c_str());
        std::strcpy(artist, bms->get_artist().c_str());
        std::strcpy(subartist, bms->get_subartist().c_str());
        std::strcpy(genre, bms->get_genre().c_str());

        ImGui::InputText("Title", title, IM_ARRAYSIZE(title));
        ImGui::InputText("Subtitle", subtitle, IM_ARRAYSIZE(subtitle));        
        ImGui::InputText("Artist", artist, IM_ARRAYSIZE(title));
        ImGui::InputText("Subartist", subartist, IM_ARRAYSIZE(subtitle));
        ImGui::InputText("Genre", genre, IM_ARRAYSIZE(title));

        const char* modes[] = {"SP", "DP", "PM"};
        current_mode = bms->get_playstyle();
        if (ImGui::Combo("Mode", &current_mode, modes, IM_ARRAYSIZE(modes))) {
            bms->set_playstyle(static_cast<Playstyle>(current_mode));
        }

        bpm = bms->get_bpm();
        if (ImGui::InputDouble("BPM", &bpm, 1.0f, 10.0f, "%.0f")) {
            bms->set_bpm(bpm);
        }

        total = bms->get_total();
        if (ImGui::InputInt("Total", &total)) {
            bms->set_total(total);
        }

        const char* ranks[] = {"Very Hard", "Hard", "Normal", "Easy"};
        current_rank = bms->get_rank();
        if (ImGui::Combo("Rank", &current_rank, ranks, IM_ARRAYSIZE(ranks))) {
            bms->set_rank(static_cast<Rank>(current_rank));
        }

        bms->set_title(title);
        bms->set_subtitle(subtitle);
        bms->set_artist(artist);
        bms->set_subartist(subartist);
        bms->set_genre(genre);


        bms->set_bpm(bpm);

        bms->set_rank(static_cast<Rank>(current_rank));
    }

    if (ImGui::CollapsingHeader("Keysounds")) {
        std::vector<char*> keysound_labels = get_keysound_labels(DATA_LIMIT, 2);

        ImGui::SetNextItemWidth(-FLT_MIN);
        static int selected_keysound = state->get_selected_keysound() - 1;
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
    
    if (ImGui::CollapsingHeader("exBPM")) {
        std::vector<char*> exbpm_labels = get_exbpm_labels(DATA_LIMIT, 2);

        ImGui::SetNextItemWidth(-FLT_MIN);
        static int selected_exbpm = state->get_selected_bpm_change() - 1;
        if (ImGui::ListBox("##exbpms_list", &selected_exbpm, exbpm_labels.data(), DATA_LIMIT, 10)) {
            state->set_selected_bpm_change(selected_exbpm + 1);
        }
        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                ImGui::OpenPopup("Input exBPM"); 
                state->set_popup(true);
            }
        }

        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Input exBPM", NULL, popup_flags)) {
            static double exbpm = 130;
            std::string selected_exbpm_str = bms->get_bpm_changes()[selected_exbpm + 1];
            if (selected_exbpm_str != "") {
                static double exbpm = std::stod(selected_exbpm_str);
            }

            (ImGui::InputDouble("BPM", &exbpm, 0.0f, 10000.0f, "%.00000f"));

            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
                state->set_popup(false);
            }

            ImGui::SameLine();

            if (ImGui::Button("OK")) {
                bms->set_bpm_change(std::to_string(exbpm), selected_exbpm + 1);
                ImGui::CloseCurrentPopup();
                state->set_popup(false);
            }

            ImGui::EndPopup();
        }

        for (auto c : exbpm_labels) {
            delete c;
        }
    }

    int quantization = state->get_quantization();
    fVec2 grid_scale = state->get_grid_scale();
    if (ImGui::CollapsingHeader("Grid")) {
        ImGui::InputInt("##quantization", &quantization);
        ImGui::DragFloat("x scale", &grid_scale.x, 0.1f, 1.0f, 100.0f, "%.1f");
        ImGui::DragFloat("y scale", &grid_scale.y, 0.1f, 1.0f, 100.0f, "%.1f");
    }
    state->set_quantization(quantization);
    state->set_grid_scale(grid_scale);

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
            labels.push_back(ImBMS::format_base36(i, 2));
        }
    }

    // transform the vector of strings to vector of char* for imgui's listbox to use
    // ImBMS::cstr allocates memory for each label so it has to be freed wherever this function is called
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
