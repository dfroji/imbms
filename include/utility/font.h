#pragma once

#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/Graphics/Text.hpp"

const int FONT_SIZE_IMGUI = 17;
const int FONT_SIZE_SFML = 13;

namespace ImBMS {
    namespace Font {
        std::string get_font();
        void load_imgui(ImGuiIO& io);
        void load_sfml(sf::Font* sfml_font);
    }
}
