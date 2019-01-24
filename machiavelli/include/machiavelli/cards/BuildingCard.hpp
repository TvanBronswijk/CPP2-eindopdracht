#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class BuildingCard {
public:
    std::string get_name() { return name_; }
    std::string get_color() { return color_; }
    int get_coins() { return gold_coins_; }

    friend std::istream &operator>>(std::istream &str, BuildingCard &building);
    friend std::ostream &operator<<(std::ostream &str, BuildingCard &building);
    bool operator==(BuildingCard &card) { return card.name_ == this->name_; }
    bool operator!=(BuildingCard &card) { return !(*this == card); }

private:
    std::string name_;
    std::string color_;
    int gold_coins_;
};


inline std::istream &operator>>(std::istream &str, BuildingCard &building) {
    std::string line;
    std::getline(str, line);
    std::stringstream lineStream(line);
    std::string cell;

    for(int i = 0; std::getline(lineStream, cell, ';'); i++) {
        switch (i) {
            case 0:
                building.name_ = cell;
                break;
            case 1:
                building.gold_coins_ = std::stoi(cell);
                break;
            case 2:
                building.color_ = cell;
                break;
            default:
                break;
        }
    }
    return str;
}

inline std::ostream &operator<<(std::ostream &str, BuildingCard &building) {
    str << (building.name_ + "," + std::to_string(building.gold_coins_) + "," + building.color_ + ",\n");
    return str;
}
