#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class BuildingCard {
public:
    std::string name() { return name_; }
    std::string color() { return color_; }
    int coins() { return gold_coins_; }

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
    std::getline(lineStream, building.name_, ';');

    std::string gc;
    std::getline(lineStream, gc, ';');
    building.gold_coins_ = std::stoi(gc);

    std::getline(lineStream, building.color_, ';');
    return str;
}

inline std::ostream &operator<<(std::ostream &str, BuildingCard &building) {
    str << (building.name_ + ';' + std::to_string(building.gold_coins_) + ';' + building.color_ + "\r\n");
    return str;
}
