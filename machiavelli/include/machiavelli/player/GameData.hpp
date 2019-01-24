#pragma once
#include <vector>
#include <server/player/PlayerData.hpp>
#include "machiavelli/*cards.hpp"

struct MachiavelliData : public server::player::PlayerData {
	int gold_coins;
	std::vector<CharacterCard> character_cards;
	std::vector<BuildingCard> building_cards;

	std::vector<BuildingCard> build_buildings;
};
