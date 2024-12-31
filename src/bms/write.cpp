#include "write.h"

#include <fstream>

void ImBMS::write(BMS* bms, std::string filename) {
    std::ofstream file(filename);

    std::vector<std::string> keysounds = bms->get_keysounds();
    std::vector<std::string> graphics = bms->get_graphics();
    std::vector<std::string> bpm_changes = bms->get_bpm_changes();

    file << "" << std::endl;
    file << DATA_FIELD_TAG + " HEADER FIELD" << std::endl;
    file << "" << std::endl;
    
    for (const auto& item : bms->get_header_data()) {
        file << item.first << " " << item.second << std::endl;
    }

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

    for (int i = 1; i < bpm_changes.size(); i++) {
        if (bpm_changes[i] != "") {
            file << "#BPM" << ImBMS::format_base36(i, 2) << " " << bpm_changes[i] << std::endl;
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
