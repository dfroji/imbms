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
        std::vector<std::string> get_keysound_labels(int size, int digits);

        BMS* bms;
};
