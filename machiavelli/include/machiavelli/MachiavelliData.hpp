#pragma once
#include <server/PlayerData.hpp>
#include "machiavelli/objects/CharacterCard.hpp"
#include "machiavelli/objects/BuildingCard.hpp"
#include <vector>
#include <map>

enum class Player_state {Waiting, Playing};

struct MachiavelliData : public PlayerData {
	MachiavelliData() : gold_coins(0), player_state(Player_state::Waiting) {};
	
	Player_state player_state;
	int gold_coins;
	std::map<int, CharacterCard> character_cards;
	std::vector<BuildingCard> building_cards;
	std::vector<BuildingCard> build_buildings;
};
