#include "menu_bar.h"

#include "imgui.h"

#include <filedialog.h>

MenuBar::MenuBar() {

}

MenuBar::~MenuBar() {

}

void MenuBar::render(State* state) {

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Open", "Ctrl+O")) {
            FileDialog fd;
        }
        if (ImGui::MenuItem("Save", "Ctrl+S")) {
        }
        if (ImGui::MenuItem("Save as", "Ctrl+Shift+S")) {
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit")) {

        ImGui::EndMenu();
    }
    
    if (ImGui::BeginMenu("View")) {

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}
