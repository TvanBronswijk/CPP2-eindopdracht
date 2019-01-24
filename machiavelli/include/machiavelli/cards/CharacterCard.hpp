#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class CharacterCard {
public:
    int get_order() const { return order_; }
    std::string get_name() const { return name_; }

    friend std::istream &operator>>(std::istream &str, CharacterCard &charactercard);
    friend std::ostream &operator<<(std::ostream &str, CharacterCard &charactercard);

private:
    int order_;
    std::string name_;
};


inline std::istream &operator>>(std::istream &str, CharacterCard &character) {
    std::string line;
    std::getline(str, line);
    std::stringstream lineStream(line);
    std::string cell;

    for (int i = 0; std::getline(lineStream, cell, ';'); i++) {
        switch (i) {
            case 0:
                character.order_ = std::stoi(cell);
                break;
            case 1:
                character.name_ = cell;
                break;
            default:
                break;
        }
    }
    return str;
}

inline std::ostream &operator<<(std::ostream &str, CharacterCard &charactercard) {
    str << (std::to_string(charactercard.order_) + "," + charactercard.name_ + ",\n");
    return str;
}