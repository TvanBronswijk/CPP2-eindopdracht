#pragma once
#include "machiavelli/*cards.hpp"
#include <vector>

namespace parsing {
	std::vector<BuildingCard> parse_buildings();
	std::vector<CharacterCard> parse_characters();
	void write_characters(std::vector<BuildingCard> buildingcards);
	void write_buildings(std::vector<CharacterCard> charactercards);
};
