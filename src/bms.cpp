#include "bms.h"

BMS::BMS() {
    this->artist = "";
    this->subartist = "";
    this->title = "";
    this->subtitle = "";
    this->bpm = 130;
    this->genre = "";
    this->player = Player::sp;
    this->difficulty = Difficulty::easy;
    this->rank = Rank::rank_easy;
    this->total = 300;
    this->stagefile = "";
    this->banner = "";

    this->keysounds.resize(DATA_LIMIT, "");
    this->graphics.resize(DATA_LIMIT, "");
    this->bpm_changes.resize(DATA_LIMIT, 0);
}

BMS::~BMS() {

}

std::string BMS::get_artist() {
    return this->artist;
}

void BMS::set_artist(std::string artist) {
    this->artist = artist;
}

std::string BMS::get_subartist() {
    return this->subartist;
}

void BMS::set_subartist(std::string subartist) {
    this->subartist = subartist;
}

std::string BMS::get_title() {
    return this->title;
}

void BMS::set_title(std::string title) {
    this->title = title;
}

std::string BMS::get_subtitle() {
    return this->subtitle;
}

void BMS::set_subtitle(std::string subtitle) {
    this->title = title;
}

double BMS::get_bpm() {
    return this->bpm;
}

void BMS::set_bpm(double bpm) {
    this->bpm = bpm;
}

std::string BMS::get_genre() {
    return this->genre;
}

void BMS::set_genre(std::string genre) {
    this->genre = genre;
}

Player BMS::get_player() {
    return this->player;
}

void BMS::set_player(Player player) {
    this->player = player;
}

Difficulty BMS::get_difficulty() {
    return this->difficulty;
}

void BMS::set_difficulty(Difficulty difficulty) {
    this->difficulty = difficulty;
}

Rank BMS::get_rank() {
    return this->rank;
}

void BMS::set_rank(Rank rank) {
    this->rank = rank;
}

double BMS::get_total() {
    return this->total;
}

void BMS::set_total(double total) {
    this->total = total;
}

std::string BMS::get_stagefile() {
    return this->stagefile;
}

void BMS::set_stagefile(std::string stagefile) {
    this->stagefile = stagefile;
}

std::string BMS::get_banner() {
    return this->banner;
}

void BMS::set_banner(std::string banner) {
    this->banner = banner;
}

std::vector<std::string> BMS::get_keysounds() {
    return this->keysounds;
}

void BMS::set_keysound(std::string filepath, int index) {
    this->keysounds[index] = filepath;
}

std::vector<std::string> BMS::get_graphics() {
    return this->graphics;
}

void BMS::set_graphic(std::string filepath, int index) {
    this->graphics[index] = filepath;
}

std::vector<double> BMS::get_bpm_changes() {
    return this->bpm_changes;
}

void BMS::set_bpm_change(double bpm, int index) {
    this->bpm_changes[index] = bpm;
}

