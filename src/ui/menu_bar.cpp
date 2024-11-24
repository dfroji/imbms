#include "menu_bar.h"

#include "imgui.h"

#include "filedialog.h"

MenuBar::MenuBar() {

}

MenuBar::~MenuBar() {

}

void MenuBar::render(State* state) {
    state->set_menu_bar_interacted(false);

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("File")) {
        state->set_menu_bar_interacted(true);

        if (ImGui::MenuItem("Open", "Ctrl+O")) {
            FileDialog fd(state);
        }
        if (ImGui::MenuItem("Save", "Ctrl+S")) {
        }
        if (ImGui::MenuItem("Save as", "Ctrl+Shift+S")) {
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit")) {
        state->set_menu_bar_interacted(true);

        ImGui::EndMenu();
    }
    
    if (ImGui::BeginMenu("View")) {
        state->set_menu_bar_interacted(true);

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}
