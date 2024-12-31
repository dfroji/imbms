#include "utils.h"

#include <cmath>
#include <algorithm>

#include "NotoSansMonoCJKjp-Regular.cpp"

std::vector<std::string> ImBMS::split_line(std::string line, const char* delimiter, int number_of_splits) {
    std::vector<std::string> splits;

    std::string split = "";
    for (const auto& c : line) {
        if (c != *delimiter || !splits.size() < number_of_splits - 1) {split.push_back(c);}
        else {
            splits.push_back(split);
            split = "";
        }
    }
    if (split != "") {splits.push_back(split);}

    return splits;
}

int ImBMS::to_decimal(int base, std::string number) {
    int dec = 0;
    int exp = number.length() - 1;
    for (auto& c : number) {
        if (c < 'A') {c += 'A' - '9' - 1;}
        dec += (c - '0' - ('A' - '9' - 1)) * std::pow(base, exp);
        exp -= 1;
    }

    return dec;
}

int ImBMS::base36_to_int(std::string number) {
    return to_decimal(36, number);
}

std::string ImBMS::int_to_base36(int number) {
    std::string base36 = "";

    int div = number / 36;
    int remainder = number % 36;

    if (div > 0) {base36 = int_to_base36(div);}
    
    int c = remainder + 55;
    if (c < 'A') {c -= ('A' - '9' - 1);}

    base36.push_back(c);
    return base36;
}

std::string ImBMS::format_base36(int number, int digits) {
    std::string base36 = int_to_base36(number);
    if (base36.length() < digits) {
        std::string tmp = base36;
        base36 = "";
        for (int i = 0; i < digits - tmp.length(); i++) {
            base36.push_back('0');
        }
        for (const auto& c : tmp) {
            base36.push_back(c);
        }
    }
    return base36;
}

// silly function for casting base36 to hexadecimal
// shouldn't really be used with anything except basic bpm channel
int ImBMS::hexify(int number) {
    std::string base36 = int_to_base36(number);

    for (const auto& c : base36) {
        if (c > 'F') {return -1;}
    }

    return to_decimal(16, base36);
}

std::string ImBMS::rtrim(std::string s) {
    if (!std::isspace(s[s.length()-1])) {return s;}

    std::string new_s = "";
    for (int i = 0; i < s.length() - 1; i++) {
        new_s.push_back(s[i]);
    }
    return new_s;
}

int ImBMS::get_gcd(int lhs, int rhs) {
    if (lhs <= 0 || rhs <= 0) {return 0;}

    int i_start;
    if (lhs < rhs) {i_start = lhs;}
    else {i_start = rhs;}

    for (int i = i_start; i > 0; i--) {
        if (lhs % i == 0 && rhs % i == 0) {return i;}
    }
    return 0;
}

std::string ImBMS::fint(int number, int digits) {
    std::string number_str = std::to_string(number);
    std::string new_str = "";
    if (number_str.length() < digits) {
        for (int i = 0; i < digits - number_str.length(); i++) {
             new_str.push_back('0');
        }
        for (const auto& c : number_str) {
                new_str.push_back(c);
        }
    } else if (digits < number_str.length()) {
        for (int i = number_str.length() - digits; i < number_str.length(); i++) {
            new_str.push_back(number_str[i]);
        }
    } else {
        return number_str;
    }

    return new_str;
}

char* ImBMS::cstr(std::string& str) {
    char* c = new char[str.size() + 1];
    std::strcpy(c, str.c_str());
    return c;
}

// trim trailing zeros from a double formatted string
std::string ImBMS::trim_dstr(std::string d) {
    std::string str = "";

    bool trim = true;
    for (int i = d.size() - 1; i >= 0; i--) {
        if (trim) {
            if (d[i] == '0') {
                continue;

            } else if (d[i] == '.') {
                trim = false;
                continue;

            } else {
                trim = false;
                str.push_back(d[i]);
            }

        } else {
            str.push_back(d[i]);
        }
    }

    std::reverse(str.begin(), str.end());

    return str;
}

void ImBMS::load_font(ImGuiIO& io) {
    io.Fonts->Clear();
    io.Fonts->AddFontFromMemoryCompressedTTF(NotoSansMonoCJKjpRegular_compressed_data, 
                                             NotoSansMonoCJKjpRegular_compressed_size, 
                                             FONT_SIZE_IMGUI, 
                                             nullptr, 
                                             io.Fonts->GetGlyphRangesJapanese()
                                            );
    ImGui::SFML::UpdateFontTexture();
}
