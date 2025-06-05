#pragma once

#include <vector>

#include "state.h"
#include "bms.h"

class SideMenu {
    public:
        SideMenu();
        ~SideMenu();

        void render(State* state, BMS* bms);
        
    private:
        std::vector<char*> get_keysound_labels(int size, int digits);
        std::vector<char*> get_graphic_labels(int size, int digits);
        std::vector<char*> get_exbpm_labels(int size, int digits);
        std::vector<char*> get_index_labels(int size, int digits);

        State* state;
        BMS* bms;
};
