#pragma once
#include <server/player/PlayerData.hpp>
#include "machiavelli/cards/CharacterCard.hpp"
#include "machiavelli/cards/BuildingCard.hpp"
#include "machiavelli/cards/Hand.hpp"

struct GameData : public server::player::PlayerData {
	int gold_coins;
	Hand<CharacterCard> character_cards;
	Hand<BuildingCard> building_cards;

	Hand<BuildingCard> built_buildings;
};
