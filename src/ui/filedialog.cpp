#include "filedialog.h"

#include "SFML/Window/Event.hpp"
#include "SFML/System/Clock.hpp"

#include <cstring>
#include <thread>
#include <algorithm>

const int FRAMES = 2;
const int INNER_SPACES = 6;

FileDialog::FileDialog(State* state) {
    this->is_open = true;
    this->selected = 0;
    this->state = state;
}

FileDialog::~FileDialog() {
    if (this->window != nullptr) {
        this->window->close();
        ImGui::SFML::Shutdown(*(this->window));
        delete this->window;
    }
}

fs::path FileDialog::open_file(fs::path path, FDMode mode) {
    init_window("Open file"); 
    this->button_label = "Open";

    this->path = path;
    this->mode = mode;
    set_extensions(mode);

    // this->path and this->filename are modified in render()
    while (this->is_open) {
        render();
    }

    this->path /= this->filename;
    return this->path;
}

fs::path FileDialog::save_file(fs::path path, FDMode mode) {
    init_window("Save file");
    this->button_label = "Save";

    this->path = path;
    this->mode = mode;
    set_extensions(mode);

    // this->path and this->filename are modified in render()
    while (this->is_open) {
        render();
    }
    
    this->path /= this->filename;
    return this->path;
}

void FileDialog::set_extensions(FDMode mode) {
    switch (mode) {
        case FDMode::BMSFiles:
            this->extensions = BMS_EXTENSIONS;
            break;
        case FDMode::Keysounds:
            this->extensions = KEYSOUND_EXTENSIONS;
            break;
    }
}

void FileDialog::init_window(std::string window_name) {
    this->window = new sf::RenderWindow(sf::VideoMode(640, 480), window_name);
    this->window->setVerticalSyncEnabled(true);
    ImGui::SFML::Init(*(this->window));

    ImGui::GetIO().IniFilename = NULL; // disable the generation of imgui.ini
}

void FileDialog::render() {
    sf::Event event;
    sf::Clock delta_clock;
    
    while (this->window->pollEvent(event)) {
        ImGui::SFML::ProcessEvent(*(this->window), event);
    }

    ImGui::SFML::Update(*(this->window), delta_clock.restart());

    this->is_open = this->window->isOpen();

    this->window->clear();

    std::vector<fs::path> files = get_files();

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;

    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
    ImGui::Begin("File dialog", &this->is_open, window_flags);

    ImGui::SetNextItemWidth(-FLT_MIN);
    static char path[1024] = "";
    std::string path_label = ""; 
    if (is_root()) {
        path_label = fs::path::preferred_separator;
    } else {
        path_label = this->path.string();
    }
    std::strcpy(path, path_label.c_str());
    ImGui::InputText("##Path", path, IM_ARRAYSIZE(path)); 

    bool was_selected = false; // this is used to display the selected file in a widget
    static char file[1024] = "";
    if (ImGui::BeginListBox("##Files", ImVec2(-FLT_MIN, ImGui::GetMainViewport()->Size.y - FRAMES*ImGui::GetFrameHeight() - INNER_SPACES*ImGui::GetStyle().ItemInnerSpacing.y))) {
        for (int i = 0; i < files.size(); i++) {
            // files that don't have a valid extension are not displayed
            if (fs::is_regular_file(files[i]) && !this->extensions.contains(files[i].extension().string())) {
                continue;
            }

            // don't display if the directory is the current directory (i.e. root)
            if (files[i] == this->path) {
                continue;
            }

            std::string label = "";
            if (files[i] == this->path.parent_path()) {
                label = "..";
            } else {
                label = files[i].filename().string();
            }
            if (fs::is_directory(files[i])) {
                label.push_back(fs::path::preferred_separator);
            }

            const bool is_selected = (this->selected == i);
            if (ImGui::Selectable(label.c_str(), is_selected)) {
                was_selected = true;
                this->selected = i;

                // play audio file of a valid format when clicked
                if (fs::is_regular_file(files[i]) && this->mode == FDMode::Keysounds) {
                    state->play_keysound(files[i]);
                }

            }
            if (ImGui::IsItemHovered()) {
                // move to directory on double click
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    if (fs::is_directory(files[i])) {
                        this->path = files[i];
                        this->selected = 0;
                    }
                }
            }
        }
        ImGui::EndListBox();
    }
   
    std::string first_button_label = "Cancel";
    double first_button_width = get_button_width(first_button_label);
    double second_button_width = get_button_width(this->button_label);

    // close the file dialog when pressed
    if (ImGui::Button(first_button_label.c_str())) {
        this->path = "";
        this->filename = "";
        this->is_open = false;
        std::strcpy(file, ""); // clear file so it is not used the next time a file dialog is opened
        return;
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetMainViewport()->WorkSize.x - first_button_width - second_button_width);

    if (fs::is_regular_file(files[this->selected]) 
            && this->extensions.contains(files[this->selected].extension().string())
            && was_selected
            ) {
        std::strcpy(file, files[this->selected].filename().string().c_str());

    // this clears the label for the selected file if a directory etc is selected
    } else if (was_selected) {
        std::string s = "";
        std::strcpy(file, s.c_str());
    }
    ImGui::InputText("##File", file, IM_ARRAYSIZE(path)); 
    this->filename = file;

    ImGui::SameLine();

    // close the file dialog and change the path if the file is valid
    if (ImGui::Button(button_label.c_str())) {
        fs::path fpath = this->path.string() + filename;
        if (this->extensions.contains(fpath.extension().string())) {
            this->is_open = false;
            std::strcpy(file, ""); // clear file so it is not used the next time a file dialog is opened
        }
    }

    ImGui::End();

    ImGui::SFML::Render(*(this->window));
    this->window->display();
}

std::vector<fs::path> FileDialog::get_files() {
    std::vector<fs::path> files;

    // parent path always first
    if (this->path.has_parent_path()) {
        files.push_back(this->path.parent_path());
    }

    // use directory iterator to get every item in the directory
    // todo: some kind of sorting
    for (const auto& file : fs::directory_iterator(this->path)) {
        files.push_back(file);
    }

    // sort the files (except for parent path) in alphabetical order
    std::sort(files.begin() + 1, files.end());

    return files;
}

bool FileDialog::is_root() {
    return this->path == this->path.root_path();
}

double FileDialog::get_button_width(std::string text) {
    return ImGui::CalcTextSize(text.c_str()).x + INNER_SPACES*ImGui::GetStyle().ItemInnerSpacing.x;
}

