#pragma once

#include <string>
#include <cstring>
#include <vector>

namespace ImBMS {
    std::vector<std::string> split_line(std::string line, const char* delimiter, int number_of_splits);
    int to_decimal(int base, std::string number);
    int base36_to_int(std::string number);
    std::string int_to_base36(int number);
    std::string format_base36(int number, int digits);
    int hexify(int number);
    std::string rtrim(std::string s);
    int get_gcd(int lhs, int rhs);
    std::string fint(int number, int digits);
    char* cstr(std::string& str);
    std::string trim_dstr(std::string d);

    template<typename T>
    void insert(std::vector<T>& target, std::vector<T> v, int elements_added) {
        for (int i = 0; i < elements_added; i++) {
            target.push_back(v[i % v.size()]);
        } 
    }

}
