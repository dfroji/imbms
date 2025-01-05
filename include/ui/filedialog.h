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

enum FDMode {BMSFiles, Keysounds, Graphics};
const std::set<std::string> BMS_EXTENSIONS = {".bms", ".bme", ".bml", ".pms"};
const std::set<std::string> KEYSOUND_EXTENSIONS = {".wav", ".ogg"};
const std::set<std::string> GRAPHIC_EXTENSIONS = {".png", ".jpg", ".gif", ".tga", ".dds", ".mpg", ".avi", ".wma", ".ogv", ".webm", ".mp4"};

class FileDialog {
public:
    FileDialog(State* state);
    ~FileDialog();

    fs::path open_file(fs::path path, FDMode mode);
    fs::path save_file(fs::path path, FDMode mode);
private:
    void set_extensions(FDMode mode);
    bool init_window(std::string window_name);
    void render();
    std::vector<fs::path> get_files();
    bool is_root();

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
