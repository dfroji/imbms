#include "menu_bar.h"

#include "imgui.h"

#include "filedialog.h"

MenuBar::MenuBar() {

}

MenuBar::~MenuBar() {

}

void MenuBar::render(State* state) {
    state->set_menu_bar_interacted(false);

    fs::path current_path = state->get_current_path();

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("File")) {
        state->set_menu_bar_interacted(true);

        if (ImGui::MenuItem("New", "Ctrl+N")) {
            state->new_bms();
        }
        if (ImGui::MenuItem("Open", "Ctrl+O")) {
            FileDialog fd(state);
            fs::path filepath = fd.open_file(current_path, FDMode::BMSFiles);
            state->load_bms(filepath);
        }
        if (ImGui::MenuItem("Save", "Ctrl+S")) {
            if (state->is_modified()) {
                if (!state->has_filepath()) {
                    FileDialog fd(state);
                    fs::path filepath = fd.save_file(current_path, FDMode::BMSFiles);
                    state->save_bms(filepath);

                } else {
                    state->save_bms(state->get_filename());
                }
            }
        }
        if (ImGui::MenuItem("Save as", "Ctrl+Shift+S")) {
            FileDialog fd(state);
            fs::path filepath = fd.save_file(current_path, FDMode::BMSFiles);
            state->save_bms(filepath);
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit")) {
        state->set_menu_bar_interacted(true);

        if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
            state->undo();
        }
        if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
            state->redo();
        }

        ImGui::EndMenu();
    }
    
    if (ImGui::BeginMenu("View")) {
        state->set_menu_bar_interacted(true);

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}
