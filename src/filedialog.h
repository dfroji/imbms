#pragma once

#include <vector>
#include <set>
#include <filesystem>

#include "imgui.h"
#include "imgui-SFML.h"

#include "SFML/Graphics/RenderWindow.hpp"

namespace fs = std::filesystem;

class FileDialog {
public:
    FileDialog();
    ~FileDialog();

    std::string open_file(fs::path path, std::set<std::string> extensions);
    std::string save_file(fs::path path, std::set<std::string> extensions);
private:
    void init_window(std::string window_name);
    void render();
    std::vector<fs::path> get_files();
    bool is_root();
    double get_button_width(std::string text);

    sf::RenderWindow* window;   
    bool is_open;

    int selected;

    fs::path path;
    std::set<std::string> extensions;
    std::string button_label;

    std::string filename;

};
