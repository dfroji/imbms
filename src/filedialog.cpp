#include "filedialog.h"

#include "SFML/Window/Event.hpp"
#include "SFML/System/Clock.hpp"

#include <cstring>

const int FRAMES = 2;
const int INNER_SPACES = 6;

FileDialog::FileDialog() {
    this->is_open = true;
    this->selected = 0;
}

FileDialog::~FileDialog() {
    if (this->window != nullptr) {
        this->window->close();
        ImGui::SFML::Shutdown(*(this->window));
        delete this->window;
    }
}

std::string FileDialog::open_file(fs::path path, std::set<std::string> extensions) {
    init_window("Open file"); 
    this->button_label = "Open";

    this->path = path;
    this->extensions = extensions;

    while (this->is_open) {
        render();
    }

    return this->path.string() + fs::path::preferred_separator + this->filename;
}

std::string FileDialog::save_file(fs::path path, std::set<std::string> extensions) {
    init_window("Save file");
    this->button_label = "Save";

    this->path = path;
    this->extensions = extensions;

    while (this->is_open) {
        render();
    }
    
    return this->path.string() + fs::path::preferred_separator + this->filename;
}

void FileDialog::init_window(std::string window_name) {
    this->window = new sf::RenderWindow(sf::VideoMode(640, 480), window_name);
    this->window->setVerticalSyncEnabled(true);
    ImGui::SFML::Init(*(this->window));

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
        path_label = this->path.string() + fs::path::preferred_separator;
    }
    std::strcpy(path, path_label.c_str());
    ImGui::InputText("##Path", path, IM_ARRAYSIZE(path)); 

    bool was_selected = false;
    if (ImGui::BeginListBox("##Files", ImVec2(-FLT_MIN, ImGui::GetMainViewport()->Size.y - FRAMES*ImGui::GetFrameHeight() - INNER_SPACES*ImGui::GetStyle().ItemInnerSpacing.y))) {
        for (int i = 0; i < files.size(); i++) {
            if (fs::is_regular_file(files[i]) && !this->extensions.contains(files[i].extension())) {
                continue;
            }
            if (files[i] == this->path) {
                continue;
            }

            std::string label = "";
            if (files[i] == this->path.parent_path()) {
                label = "..";
            } else {
                label = files[i].filename();
            }
            if (fs::is_directory(files[i])) {
                label.push_back(fs::path::preferred_separator);
            }

            const bool is_selected = (this->selected == i);
            if (ImGui::Selectable(label.c_str(), is_selected)) {
                was_selected = true;
                this->selected = i;
            }
            if (ImGui::IsItemHovered()) {
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


    if (ImGui::Button(first_button_label.c_str())) {
        this->path = "";
        this->is_open = false;
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetMainViewport()->WorkSize.x - first_button_width - second_button_width);

    static char file[1024] = "";
    if (fs::is_regular_file(files[this->selected]) 
            && this->extensions.contains(files[this->selected].extension())
            && was_selected
            ) {
        std::strcpy(file, files[this->selected].filename().c_str());

    } else if (was_selected) {
        std::string s = "";
        std::strcpy(file, s.c_str());
    }
    ImGui::InputText("##File", file, IM_ARRAYSIZE(path)); 
    this->filename = file;

    ImGui::SameLine();

    if (ImGui::Button(button_label.c_str())) {
        fs::path fpath = this->path.string() + filename;
        if (this->extensions.contains(fpath.extension())) {
            this->is_open = false;
        }
    }

    ImGui::End();

    ImGui::SFML::Render(*(this->window));
    this->window->display();
}

std::vector<fs::path> FileDialog::get_files() {
    std::vector<fs::path> files;
    if (this->path.has_parent_path()) {
        files.push_back(this->path.parent_path());
    }
    for (const auto& file : fs::directory_iterator(this->path)) {
        files.push_back(file);
    }
    return files;
}

bool FileDialog::is_root() {
    return this->path == this->path.root_path();
}

double FileDialog::get_button_width(std::string text) {
    return ImGui::CalcTextSize(text.c_str()).x + INNER_SPACES*ImGui::GetStyle().ItemInnerSpacing.x;
}

