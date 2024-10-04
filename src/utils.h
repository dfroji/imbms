#pragma once

#include <string>
#include <vector>

namespace ImBMS {
    std::vector<std::string> split_line(std::string line, const char* delimiter, int number_of_splits);
    int base36_to_int(std::string number);
    std::string int_to_base36(int number);
    std::string format_base36(int number, int digits);
    std::string rtrim(std::string s);
}
