#include "menu_bar.h"

#include "imgui.h"

#include "filedialog.h"
#include "eventhandler.h"

MenuBar::MenuBar() {

}

MenuBar::~MenuBar() {

}

void MenuBar::render(State* state, sf::RenderWindow* window) {
    state->set_menu_bar_interacted(false);

    fs::path current_path = state->get_current_path();

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("File")) {
        state->set_menu_bar_interacted(true);

        if (ImGui::MenuItem("New", "Ctrl+N")) {
            if (state->is_modified()) {
                state->set_unsaved_changes_popup(true, DiscardAction::New);

            } else {
                state->new_bms();
            }
        }
        if (ImGui::MenuItem("Open", "Ctrl+O")) {
            if (state->is_modified()) {
                state->set_unsaved_changes_popup(true, DiscardAction::Open);

            } else {
                EventHandler::open_file(state);
            }
        }
        if (ImGui::MenuItem("Save", "Ctrl+S")) {
            if (state->is_modified()) {
                if (!state->has_filepath()) {
                    EventHandler::save_file(state);

                } else {
                    state->save_bms(state->get_filename());
                }
            }
        }
        if (ImGui::MenuItem("Save as", "Ctrl+Shift+S")) {
            EventHandler::save_file(state);
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
        static bool view_bpm = state->view_bpm();
        static bool view_bga = state->view_bga();

        ImGui::MenuItem("BPM Channels", "", &view_bpm);
        ImGui::MenuItem("BGA Channels", "", &view_bga);

        state->set_view_bpm(view_bpm);
        state->set_view_bga(view_bga);

        ImGui::EndMenu();
    }

    discard_changes_popup(state, window);

    ImGui::EndMainMenuBar();
}

void MenuBar::discard_changes_popup(State* state, sf::RenderWindow* window) {
    ImGuiWindowFlags popup_flags = 0;
    popup_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    popup_flags |= ImGuiWindowFlags_NoMove;

    if (state->is_unsaved_changes_popup()) {
        ImGui::OpenPopup("Warning");
    }

    // set the modal popup to always be in the center of the viewport
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Warning", NULL, popup_flags)) {
        ImGui::Text("Discard unsaved changes?");

        ImGui::Separator();

        if (ImGui::Button("Cancel", BUTTON_SIZE)) {
            state->set_unsaved_changes_popup(false);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        // right align the ok button
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - BUTTON_SIZE.x);

        if (ImGui::Button("OK", BUTTON_SIZE)) {
            state->set_unsaved_changes_popup(false);
            ImGui::CloseCurrentPopup();

            switch (state->get_discard_action()) {
                case DiscardAction::New:
                    state->new_bms();
                    break;
                case DiscardAction::Open:
                    EventHandler::open_file(state);
                    break;
                case DiscardAction::Close:
                    window->close();
                    break;
                    
            }
        }

        ImGui::EndPopup();
    }
}
