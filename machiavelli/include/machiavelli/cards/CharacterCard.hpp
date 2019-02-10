#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class CharacterCard {
public:
    int order() const { return order_; }
    std::string name() const { return name_; }

    friend std::istream &operator>>(std::istream &str, CharacterCard &charactercard);
    friend std::ostream &operator<<(std::ostream &str, CharacterCard &charactercard);
    bool operator==(CharacterCard &card) { return card.name_ == this->name_; }
    bool operator!=(CharacterCard &card) { return !(*this == card); }
private:
    int order_;
    std::string name_;
};


inline std::istream &operator>>(std::istream &str, CharacterCard &character) {
    std::string line;
    std::getline(str, line);
    std::stringstream lineStream(line);

    std::string ord;
    std::getline(lineStream, ord, ';');
    character.order_ = std::stoi(ord);

    std::getline(lineStream, character.name_, ';');
    return str;
}

inline std::ostream &operator<<(std::ostream &str, CharacterCard &charactercard) {
    str << (std::to_string(charactercard.order_) + ';' + charactercard.name_ + "\r\n");
    return str;
}