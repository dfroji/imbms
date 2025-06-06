#include "filedialog.h"

#include "SFML/Window/Event.hpp"
#include "SFML/System/Clock.hpp"

#include <cstring>
#include <thread>
#include <algorithm>

#include "font.h"

const int FRAMES = 2;
const int INNER_SPACES = 6;

FileDialog::FileDialog(State* state) {
    is_open = true;
    selected = 0;
    this->state = state;
}

FileDialog::~FileDialog() {
    if (window != nullptr) {
        window->close();
        ImGui::SFML::Shutdown(*window);
        delete window;
    }
}

fs::path FileDialog::open_file(fs::path path, FDMode mode) {
    if (!init_window("Open file")) {return "";}
    this->button_label = "Open";

    this->path = path;
    this->mode = mode;
    set_extensions(mode);

    // this->path and filename are modified in render()
    while (is_open) {
        render();
    }

    this->path /= filename;
    return this->path;
}

fs::path FileDialog::save_file(fs::path path, FDMode mode) {
    if (!init_window("Save file")) {return "";}
    button_label = "Save";

    this->path = path;
    this->mode = mode;
    set_extensions(mode);

    // this->path and filename are modified in render()
    while (is_open) {
        render();
    }
    
    this->path /= filename;
    return this->path;
}

void FileDialog::set_extensions(FDMode mode) {
    switch (mode) {
        case FDMode::BMSFiles:
            extensions = BMS_EXTENSIONS;
            break;
        case FDMode::Keysounds:
            extensions = KEYSOUND_EXTENSIONS;
            break;
        case FDMode::Graphics:
            extensions = GRAPHIC_EXTENSIONS;
            break;
    }
}

bool FileDialog::init_window(std::string window_name) {
    window = new sf::RenderWindow(sf::VideoMode(640, 480), window_name);
    window->setVerticalSyncEnabled(true);
    if (!ImGui::SFML::Init(*(this->window))) {
        delete window;
        return false;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL; // disable the generation of imgui.ini
    ImBMS::Font::load_imgui(io);

    return true;
}

void FileDialog::render() {
    sf::Event event;
    sf::Clock delta_clock;
    bool enter_pressed = false;
    
    while (window->pollEvent(event)) {
        ImGui::SFML::ProcessEvent(*window, event);

        if (event.type == sf::Event::Closed) {
            path = "";
            filename = "";
            is_open = false;
            return;
        }

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.scancode == sf::Keyboard::Scan::LShift || event.key.scancode == sf::Keyboard::Scan::RShift) {
                state->set_shift(true);
            }

            if (event.key.scancode == sf::Keyboard::Scan::LControl || event.key.scancode == sf::Keyboard::Scan::RControl) {
                state->set_control(true);
            }

            if (event.key.scancode == sf::Keyboard::Scan::Enter) {
                enter_pressed = true;
            }
        }

        if (event.type == sf::Event::KeyReleased) {
            if (event.key.scancode == sf::Keyboard::Scan::LShift || event.key.scancode == sf::Keyboard::Scan::RShift) {
                state->set_shift(false);
            }

            if (event.key.scancode == sf::Keyboard::Scan::LControl || event.key.scancode == sf::Keyboard::Scan::RControl) {
                state->set_control(false);
            }
        }
    }

    ImGui::SFML::Update(*window, delta_clock.restart());

    is_open = window->isOpen();

    window->clear();

    std::vector<fs::path> files = get_files();

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;

    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
    ImGui::Begin("File dialog", &is_open, window_flags);

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
            if (fs::is_regular_file(files[i]) && !extensions.contains(files[i].extension().string())) {
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
                if (fs::is_regular_file(files[i]) && mode == FDMode::Keysounds) {
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

    // close the file dialog when pressed
    if (ImGui::Button(first_button_label.c_str(), BUTTON_SIZE)) {
        this->path = "";
        filename = "";
        is_open = false;
        std::strcpy(file, ""); // clear file so it is not used the next time a file dialog is opened
        return;
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetMainViewport()->WorkSize.x - 2*(BUTTON_SIZE.x + 2*ImGui::GetStyle().FramePadding.x + 2*ImGui::GetStyle().ItemInnerSpacing.x));

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
    if (ImGui::Button(button_label.c_str(), BUTTON_SIZE) || enter_pressed) {
        fs::path fpath = this->path.string() + filename;
        if (extensions.contains(fpath.extension().string())) {
            is_open = false;
            std::strcpy(file, ""); // clear file so it is not used the next time a file dialog is opened
        }
    }

    ImGui::End();

    ImGui::SFML::Render(*window);
    window->display();
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
