#include "font.h"

#include <filesystem>

#include "fontconfig/fontconfig.h"

namespace fs = std::filesystem;

const char* FONT_NAME = "VL Gothic";

std::string ImBMS::Font::get_font() {
    FcConfig* config = FcInitLoadConfigAndFonts();
    FcPattern* pat = FcNameParse((FcChar8*) FONT_NAME);
    FcObjectSet* objset = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, (char*) 0);
    FcFontSet* fontset = FcFontList(config, pat, objset);

    for (int i = 0; fontset && i < fontset->nfont; i++) {
        FcPattern* fontpat = fontset->fonts[i];
        FcChar8* file;
        if (FcPatternGetString(fontpat, FC_FILE, 0, &file) == FcResultMatch) {
            return (char*)file;
        }
    }

    FcConfigDestroy(config);
    FcPatternDestroy(pat);
    FcObjectSetDestroy(objset);
    FcFontSetDestroy(fontset);

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
