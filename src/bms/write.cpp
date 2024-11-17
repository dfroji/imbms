#include "write.h"

#include <fstream>

#include <iostream>

void ImBMS::write(BMS* bms, std::string filename) {
    std::ofstream file(filename);

    std::string artist = bms->get_artist();
    std::string subartist = bms ->get_subartist();
    std::string title = bms->get_title();
    std::string subtitle = bms->get_subtitle();
    double bpm = bms->get_bpm();
    std::string genre = bms->get_genre();
    int player = static_cast<int>(bms->get_player());
    int difficulty = static_cast<int>(bms->get_difficulty());
    int rank = static_cast<int>(bms->get_rank());
    double total = bms->get_total();
    std::string stagefile = bms->get_stagefile();
    std::string banner = bms->get_banner();

    std::vector<std::string> keysounds = bms->get_keysounds();
    std::vector<std::string> graphics = bms->get_graphics();

    file << "" << std::endl;
    file << DATA_FIELD_TAG + " HEADER FIELD" << std::endl;
    file << "" << std::endl;
    
    file << "#ARTIST " << artist << std::endl;
    file << "#SUBARTIST " << subartist << std::endl;
    file << "#TITLE " << title << std::endl;
    file << "#SUBTITLE " << subtitle << std::endl;
    file << "#BPM " << bpm << std::endl;
    file << "#GENRE " << genre << std::endl;
    file << "#PLAYER " << player << std::endl;
    file << "#DIFFICULTY " << difficulty << std::endl;
    file << "#RANK " << rank << std::endl;
    file << "#TOTAL " << total << std::endl;
    file << "#STAGEFILE " << stagefile << std::endl;
    file << "#BANNER " << banner << std::endl;

    file << "" << std::endl;

    for (int i = 1; i < keysounds.size(); i++) {
        if (keysounds[i] != "") {
            file << "#WAV" << ImBMS::format_base36(i, 2) << " " << keysounds[i] << std::endl;
        }
    }

    file << "" << std::endl;

    for (int i = 1; i < graphics.size(); i++) {
        if (graphics[i] != "") {
            file << "#BMP" << ImBMS::format_base36(i, 2) << " " << graphics[i] << std::endl;
        }
    }

    file << "" << std::endl;
    file << DATA_FIELD_TAG + " MAIN DATA FIELD" << std::endl;
    file << "" << std::endl;

    std::vector<Measure*> measures = bms->get_measures();
    for (int measure_i = 0; measure_i < measures.size(); measure_i++) {
        Measure* measure = measures[measure_i];
        if (measure == nullptr) {continue;} 
        std::vector<Channel*> bgm_channels = measure->bgm_channels;
        std::vector<Channel*> channels = measure->channels;
        for (int bgm_i = 0; bgm_i < bgm_channels.size(); bgm_i++) {
            if (bgm_channels[bgm_i] == nullptr) {continue;}
            Channel* bgm_channel = bgm_channels[bgm_i];
            file << "#" << ImBMS::fint(measure_i, 3) << "01:" << bgm_channel->components_to_s() << std::endl;
        }
        for (int channel_i = 2; channel_i < channels.size(); channel_i++) {
            if (channels[channel_i] == nullptr) {continue;}
            Channel* channel = channels[channel_i];
            file << "#" << ImBMS::fint(measure_i, 3) << ImBMS::format_base36(channel_i, 2) << ":" << channel->components_to_s() << std::endl; 
        }
        file << "" << std::endl;
    }

    file.close();

}
