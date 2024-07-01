
#include "ui.h"

int main() {

    UI ui;

    while (ui.is_open()) {
        ui.render();
    }

    return 0;
}
