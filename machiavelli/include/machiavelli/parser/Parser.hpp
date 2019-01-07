#pragma once
#include "machiavelli/objects/CharacterCard.hpp"
#include "machiavelli/objects/BuildingCard.hpp"
#include <vector>

class Parser
{
private:

public:
	void make_buildings();
	void make_characters();
	void save_characters(std::vector<BuildingCard> buildingcards);
	void save_buildings(std::vector<CharacterCard> charactercards);
};
