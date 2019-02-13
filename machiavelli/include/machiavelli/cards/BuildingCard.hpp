#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class BuildingCard {
public:
    std::string name() const { return name_; }
    std::string color() const { return color_; }
    int coins() const { return gold_coins_; }
    std::string description() const { return std::to_string(gold_coins_) + " Gold - " + name_ + "(" + color_ + ")"; }


    friend std::istream &operator>>(std::istream &str, BuildingCard &building);
    friend std::ostream &operator<<(std::ostream &str, const BuildingCard &building);
    friend bool operator==(const BuildingCard &lhs, const BuildingCard& rhs);
    friend bool operator!=(const BuildingCard &lhs, const BuildingCard& rhs);

private:
    std::string name_;
    std::string color_;
    int gold_coins_;
};


inline std::istream &operator>>(std::istream &str, BuildingCard &building) {
	std::string         line;
	std::getline(str, line);

	std::stringstream   lineStream(line);
	std::string         cell;

	for (int i = 0; std::getline(lineStream, cell, ';'); i++)
	{
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
		}
	}
	return str;
}

inline std::ostream &operator<<(std::ostream &str, const BuildingCard &building) {
    str << (building.name_ + ';' + std::to_string(building.gold_coins_) + ';' + building.color_ + "\r\n");
    return str;
}

inline bool operator==(const BuildingCard &lhs, const BuildingCard& rhs) { return lhs.name_ == rhs.name_; }
inline bool operator!=(const BuildingCard &lhs, const BuildingCard& rhs) { return !(lhs == rhs); }
