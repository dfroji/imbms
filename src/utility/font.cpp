#if defined(__linux__)
    #define PLATFORM_NAME "linux"
#elif define(_WIN32)
    #define PLATFORM_NAME "windows"
#endif

#include "font.h"

#include <filesystem>

namespace fs = std::filesystem;

const std::string LINUX_FONT_PATH = "/usr/share/fonts/noto/NotoSansMono-Medium.ttf";
const std::string LINUX_FONT_PATH_2 = "~/.local/share/fonts/noto/NotoSansMono-Medium.ttf";
const std::string WIN_FONT_PATH = "fonts/NotoSansMono-Medium.ttf";

std::string ImBMS::Font::get_font() {
    if (PLATFORM_NAME == "linux") {
        if (fs::exists(LINUX_FONT_PATH)) {
            return LINUX_FONT_PATH;

        } else if (fs::exists(LINUX_FONT_PATH_2)) {
            return LINUX_FONT_PATH_2;
        }

    } else if (PLATFORM_NAME == "windows") {
        return WIN_FONT_PATH;
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
        ImGui::SFML::UpdateFontTexture();
    }
}

void ImBMS::Font::load_sfml(sf::Font* sfml_font) {
    sfml_font->loadFromFile(get_font());
}
