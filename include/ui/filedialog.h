#pragma once

#include <vector>
#include <thread>
#include <set>
#include <filesystem>

#include "imgui.h"
#include "imgui-SFML.h"

#include "SFML/Graphics/RenderWindow.hpp"

#include "state.h"

namespace fs = std::filesystem;

enum FDMode {BMSFiles, Keysounds};
const std::set<std::string> BMS_EXTENSIONS = {".bms", ".bme", ".bml", ".pms"};
const std::set<std::string> KEYSOUND_EXTENSIONS = {".wav", ".ogg"};

class FileDialog {
public:
    FileDialog(State* state);
    ~FileDialog();

    std::string open_file(fs::path path, FDMode mode);
    std::string save_file(fs::path path, FDMode mode);
private:
    void set_extensions(FDMode mode);
    void init_window(std::string window_name);
    void render();
    std::vector<fs::path> get_files();
    bool is_root();
    double get_button_width(std::string text);

    sf::RenderWindow* window;   
    bool is_open;

    int selected;

    fs::path path;
    FDMode mode;
    std::set<std::string> extensions;
    std::string button_label;

    std::string filename;

    State* state;
};
