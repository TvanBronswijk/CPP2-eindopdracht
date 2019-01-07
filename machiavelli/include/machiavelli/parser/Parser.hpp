#pragma once
#include "machiavelli/objects/CharacterCard.hpp"
#include "machiavelli/objects/BuildingCard.hpp"
#include <vector>
#include <map>

class Parser
{
private:

public:
	Parser() = default;
	std::vector<BuildingCard> make_buildings();
	std::map<int, CharacterCard> make_characters();
	void save_characters(std::vector<BuildingCard> buildingcards);
	void save_buildings(std::vector<CharacterCard> charactercards);
};
