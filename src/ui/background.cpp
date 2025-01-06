#include "background.h"

#include <vector>

#include "channels_and_colors.h"

Background::Background() {

}

Background::~Background() {

}

void Background::render(State* state, sf::RenderWindow* window) {
    this->state = state;

    VRectangleShape channel_bgs = {};

    render_play_channel_bgs(channel_bgs);
    render_other_channel_bgs(channel_bgs);

    for (const auto& channel_bg : channel_bgs) {
        window->draw(channel_bg);
    }
}

void Background::render_play_channel_bgs(VRectangleShape& channel_bgs) {
    std::vector<sf::Color> play_channel_colors = get_play_channel_colors();
    for (int i = 0; i < play_channel_colors.size(); i++) {
        render_channel_bg(channel_bgs, play_channel_colors[i]);
    }
}

void Background::render_other_channel_bgs(VRectangleShape& channel_bgs) {
    int other_channels_size = OTHER_CHANNELS.size();
    if (!state->view_bpm()) {other_channels_size -= BPM_CHANNELS.size();}
    if (!state->view_bga()) {other_channels_size -= BGA_CHANNELS.size();}

    for (int i = 0; i < other_channels_size; i++) {
        render_channel_bg(channel_bgs, OTHER_COLOR*DIMMING_COLOR);
    }
}

void Background::render_channel_bg(VRectangleShape& channel_bgs, sf::Color color) {
    iVec2 absolute_pos = state->get_absolute_pos();
    ImVec2 viewport_size = state->get_viewport_size();
    ImVec2 viewport_pos = state->get_viewport_pos();
    fVec2 grid_scale = state->get_grid_scale();
    float note_width = state->get_note_width();

    sf::Vector2f size = {note_width, viewport_size.y};
    if (absolute_pos.y < 0) {size.y+=absolute_pos.y*grid_scale.y;}

    sf::RectangleShape channel_bg(size);
    channel_bg.setFillColor(color);

    sf::Vector2f position = {viewport_pos.x + channel_bgs.size()*note_width - absolute_pos.x*grid_scale.x,
                             viewport_pos.y
                            };
    channel_bg.setPosition(position);

    channel_bgs.push_back(channel_bg);
}

std::vector<sf::Color> Background::get_play_channel_colors() {
    std::vector<sf::Color> play_channel_colors = {};

    std::vector<sf::Color> bm_colors = {BM_BOTTOM_NOTE_COLOR, BM_TOP_NOTE_COLOR};

    switch (state->get_bms()->get_playstyle()) {
        case Playstyle::SP:
            play_channel_colors.push_back(SCRATCH_COLOR);
            ImBMS::insert(play_channel_colors, bm_colors, P1_VISIBLE.size() - 1);
            break;
        case Playstyle::DP:
            play_channel_colors.push_back(SCRATCH_COLOR);
            ImBMS::insert(play_channel_colors, bm_colors, P1_VISIBLE.size() - 1);
            ImBMS::insert(play_channel_colors, bm_colors, P2_VISIBLE.size() - 1);
            play_channel_colors.push_back(SCRATCH_COLOR);
            break;
        case Playstyle::PM:
            std::vector<sf::Color> pm_colors = {
                                    PM_WHITE_COLOR, PM_YELLOW_COLOR, PM_GREEN_COLOR, PM_BLUE_COLOR, 
                                    PM_RED_COLOR,
                                    PM_BLUE_COLOR, PM_GREEN_COLOR, PM_YELLOW_COLOR, PM_WHITE_COLOR
                                    };

            play_channel_colors = pm_colors;
            break;
    }

    for (auto& color : play_channel_colors) {
        color *= DIMMING_COLOR;
    }
    return play_channel_colors;
}
