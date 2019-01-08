#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class CharacterCard{
private:
	std::string name_;
	int number_;
public:
	friend std::istream& operator>>(std::istream& str, CharacterCard& charactercard);
	friend std::ostream& operator<<(std::ostream& str, CharacterCard& charactercard);

	int get_number() const { return number_; }
	std::string get_name() const { return name_; }
};


inline std::istream& operator>>(std::istream& str, CharacterCard& character)
{
	std::string         line;
	std::getline(str, line);

	std::stringstream   lineStream(line);
	std::string         cell;

	int number = 0;
	while (std::getline(lineStream, cell, ';'))
	{
		switch (number) {
		case 0:
			character.number_ = std::stoi(cell);
			break;
		case 1:
			character.name_ = cell;
			break;
		}
		number++;
	}
	return str;
}

inline std::ostream& operator<<(std::ostream& str, CharacterCard& charactercard)
{
	std::string object = std::to_string(charactercard.number_) + "," + charactercard.name_ +",\n";
	str << object;

	return str;
}