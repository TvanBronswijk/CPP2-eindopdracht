#pragma once
#include <vector>
#include <server/player/PlayerData.hpp>
#include "machiavelli/cards/CharacterCard.hpp"
#include "machiavelli/cards/BuildingCard.hpp"

struct GameData : public server::player::PlayerData {
	int gold_coins;
	std::vector<CharacterCard> character_cards;
	std::vector<BuildingCard> building_cards;

	std::vector<BuildingCard> build_buildings;
};
