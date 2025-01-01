#pragma once

#if defined(__linux__)
    #define PLATFORM_NAME "linux"
#elif define(_WIN32)
    #define PLATFORM_NAME "windows"
#endif

#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/Graphics/Text.hpp"

const float FONT_SIZE_IMGUI = 17.5f;
const int FONT_SIZE_SFML = 12;

const std::string LINUX_FONT_PATH = "/usr/share/fonts/noto/NotoSansMono-Medium.ttf";
const std::string LINUX_FONT_PATH_2 = "~/.local/share/fonts/noto/NotoSansMono-Medium.ttf";
const std::string WIN_FONT_PATH = "fonts/NotoSansMono-Medium.ttf";

namespace ImBMS {
    namespace Font {
        std::string get_font();
        void load_imgui(ImGuiIO& io);
        void load_sfml(sf::Font* sfml_font);
    }
}
