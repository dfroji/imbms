#include "utils.h"

#include <cmath>

#include <iostream>

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

int ImBMS::base36_to_int(std::string number) {
    int dec = 0;
    int exp = number.length() - 1;
    for (auto& c : number) {
        if (c < 'A') {c += 'A' - '9' - 1;}
        dec += (c - '0' - ('A' - '9' - 1)) * std::pow(36, exp);
        exp -= 1;
    }

    return dec;
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
