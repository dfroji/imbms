#include "side_menu.h"

#include <cstring>

#include "filedialog.h"

SideMenu::SideMenu() {

}

SideMenu::~SideMenu() {

}

void SideMenu::render(State* state, BMS* bms) {
    this->bms = bms;

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;

    ImVec2 viewport_pos = ImGui::GetMainViewport()->GetWorkCenter();
    ImVec2 viewport_size = ImGui::GetMainViewport()->WorkSize;

    ImVec2 size;
    size.x = viewport_size.x * 0.2f;
    size.y = viewport_size.y;
    ImGui::SetNextWindowSize(size);

    ImVec2 pos;
    pos.x = viewport_pos.x + viewport_size.x / 2 - size.x;
    pos.y = viewport_pos.y - viewport_size.y / 2;
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
        char* keysound_labels[DATA_LIMIT];
        std::vector<std::string> label_strings = get_keysound_labels(DATA_LIMIT, 2);
        for (int i = 0; i < DATA_LIMIT; i++) {
            keysound_labels[i] = const_cast<char*>(label_strings[i].c_str()); 
        } 

        ImGui::SetNextItemWidth(-FLT_MIN);
        static int selected_keysound = state->get_selected_keysound() - 1;
        if (ImGui::ListBox("##keysounds_list", &selected_keysound, keysound_labels, DATA_LIMIT, 10)) {
            state->set_selected_keysound(selected_keysound + 1);
        }
        if (ImGui::IsItemHovered()) {
            // this->is_keysounds_hovered = true;
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                FileDialog fd;
                fs::path filepath = fd.open_file(state->get_current_path(), FDMode::Keysounds);
                std::string file = filepath.filename();
                if (file != "") {
                    bms->set_keysound(file, state->get_selected_keysound());
                }
            }
        } else {
            // this->is_keysounds_hovered = false;
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

std::vector<std::string> SideMenu::get_keysound_labels(int size, int digits) {
    std::vector<std::string> labels = {};
    std::vector<std::string> keysounds = bms->get_keysounds();
    for (int i = 1; i < size+1; i++) {
        std::string keysound = "";
        if (i < keysounds.size()) {
            keysound += keysounds[i];
        }
        labels.push_back(ImBMS::format_base36(i, 2) + " " + keysound);
    }
    return labels;
}
