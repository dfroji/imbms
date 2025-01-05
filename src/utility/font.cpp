#include "font.h"

#include <filesystem>

namespace fs = std::filesystem;

const std::string FONT_PATH_USR = "/usr/share/fonts/TTF/VL-Gothic-Regular.ttf";
const std::string FONT_PATH_LOCAL = "~/.local/share/fonts/TTF/VL-Gothic-Regular.ttf";

std::string ImBMS::Font::get_font() {
    if (fs::exists(FONT_PATH_USR)) {
        return FONT_PATH_USR;

    } else if (fs::exists(FONT_PATH_LOCAL)) {
        return FONT_PATH_LOCAL;
    }

    return "";
}

void ImBMS::Font::load_imgui(ImGuiIO& io) {
    std::string font = get_font();

    // imgui will use its default font if the font isn't found
    if (font != "") {
        io.Fonts->Clear();
        io.Fonts->AddFontFromFileTTF(font.c_str(), 
                                     FONT_SIZE_IMGUI, 
                                     nullptr, 
                                     io.Fonts->GetGlyphRangesJapanese()
                                    );
        if (!ImGui::SFML::UpdateFontTexture()) {return;}
    }
}

void ImBMS::Font::load_sfml(sf::Font* sfml_font) {
    sfml_font->loadFromFile(get_font());
}
