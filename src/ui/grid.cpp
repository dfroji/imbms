#include "grid.h"

Grid::Grid() {

}

Grid::~Grid() {

}

void Grid::render(State* state, sf::RenderWindow* window) {
    int quantization = state->get_quantization();
    fVec2 grid_scale = state->get_grid_scale();

    ImVec2 viewport_size = state->get_viewport_size();
    ImVec2 viewport_pos = state->get_viewport_pos();
    fVec2 default_scaling = state->get_default_scaling();
    int visible_measures = state->get_visible_measures();
    int measures_wrapped = state->get_measures_wrapped();
    fVec2 relative_pos = state->get_relative_pos();
    iVec2 wraps = state->get_wraps();
    fVec2 wrapping_offset = state->get_wrapping_offset();

    // Setup horizontal lines for drawing
    int horizontal_line_count = visible_measures*quantization;
    sf::VertexArray horizontal_lines(sf::Lines, 2*horizontal_line_count*PADDING);
    for (int i = 0; i < horizontal_line_count*PADDING; i += 2) {
        
        // Calculate the position of the given line
        float line_distance = i*((default_scaling.y*grid_scale.y)/quantization);
        float line_y_position = relative_pos.y + viewport_size.y - viewport_pos.y - line_distance - wrapping_offset.y;

        // Offset the left end of the line if scrolled beyond the beginning of the grid
        float x_offset = 0;
        if (relative_pos.x < 0) {
            x_offset = relative_pos.x;
        }

        // Set the line in the VertexArray
        // i corresponds to the left end of the line, and i+1 corresponds to the right end of the line
        horizontal_lines[i].position = {viewport_pos.x - x_offset, line_y_position};
        horizontal_lines[i+1].position = {viewport_size.x, line_y_position};
        horizontal_lines[i].color = LINE_COLOR;
        horizontal_lines[i+1].color = LINE_COLOR;
    }

    // Arbitrary multiplier for the vertical lines
    int vertical_line_multiplier = 8;
    // Limit the vertical line count for seamless scrolling
    int vertical_line_count_min = (viewport_size.x / default_scaling.x / 2)*vertical_line_multiplier;

    // Setup vertical lines for drawing
    int vertical_line_count = (viewport_size.x / (default_scaling.x*grid_scale.x))*vertical_line_multiplier;
    if (vertical_line_count < vertical_line_count_min) {vertical_line_count = vertical_line_count_min;}
    sf::VertexArray vertical_lines(sf::Lines, 2*vertical_line_count*PADDING);
    for (int i = 0; i < vertical_line_count*PADDING; i += 2) {

        // Calculate the position of the given line
        float line_distance = i*((default_scaling.x*grid_scale.x)/vertical_line_multiplier);
        float line_x_position = -relative_pos.x + viewport_pos.x + line_distance + wrapping_offset.x;

        // Offset the bottom end of the line if scrolled beyond the beginning of the grid
        float y_offset = 0;
        if (relative_pos.y < viewport_pos.y) {
            y_offset = relative_pos.y - viewport_pos.y;
        }

        // Set the line in the VertexArray
        // i corresponds to the top end of the line, and i+1 corresponds to the bottom end of the line
        vertical_lines[i].position = {line_x_position, viewport_pos.y};
        vertical_lines[i+1].position = {line_x_position, viewport_size.y + y_offset};
        vertical_lines[i].color = LINE_COLOR;
        vertical_lines[i+1].color = LINE_COLOR;
    }


    // Setup measure lines with their respective numbers for drawing
    sf::VertexArray measure_lines(sf::Lines, 2*visible_measures*PADDING);
    std::vector<sf::Text> texts;
    for (int i = 0; i < visible_measures*PADDING; i += 2) {
        

        // Calculate the position of the given measure line

        float measure_distance = i*default_scaling.y*grid_scale.y;
        float measure_y_position = relative_pos.y + viewport_size.y - viewport_pos.y - measure_distance - wrapping_offset.y;

        // Set the line in the VertexArray
        // i corresponds to the left end of the line, and i+1 corresponds to the right end of the line
        measure_lines[i].position = {viewport_pos.x, measure_y_position};
        measure_lines[i+1].position = {viewport_size.x, measure_y_position}; 
        measure_lines[i].color = MEASURE_COLOR;
        measure_lines[i+1].color = MEASURE_COLOR;

        // Setup the number of the measure line
        int measure_number = (i/2)+wraps.y*measures_wrapped;
        sf::Text text;
        text.setString(std::to_string(measure_number));
        text.setFont(*state->get_font());
        text.setPosition(viewport_pos.x + 2, 
                         relative_pos.y + viewport_size.y - measure_distance - viewport_pos.y*2 - wrapping_offset.y
                        );
        text.setCharacterSize(FONT_SIZE);
        text.setFillColor(sf::Color::White);
        texts.push_back(text);
    }

    window->draw(horizontal_lines);
    window->draw(vertical_lines);
    window->draw(measure_lines);
    for (const auto& text : texts) {
        window->draw(text);
    }
}


