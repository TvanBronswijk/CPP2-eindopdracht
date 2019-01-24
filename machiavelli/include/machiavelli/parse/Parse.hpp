#pragma once
#include "machiavelli/cards/CharacterCard.hpp"
#include "machiavelli/cards/BuildingCard.hpp"
#include <vector>
#include <unordered_map>

namespace parsing {
	std::vector<BuildingCard> parse_buildings();
	std::vector<CharacterCard> parse_characters();
	void write_characters(std::vector<BuildingCard> buildingcards);
	void write_buildings(std::vector<CharacterCard> charactercards);
};
