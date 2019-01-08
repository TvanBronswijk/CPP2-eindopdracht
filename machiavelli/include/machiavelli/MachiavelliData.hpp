#pragma once
#include <server/PlayerData.hpp>
#include <server/command/options/OptionHandler.hpp>
#include "machiavelli/objects/CharacterCard.hpp"
#include "machiavelli/objects/BuildingCard.hpp"
#include <vector>
#include <unordered_map>

enum class Player_state {Waiting, Playing, Collecting};

struct MachiavelliData : public PlayerData {
	enum class PlayerOptions { Character, Building, None };
	MachiavelliData() : gold_coins(2), player_state(Player_state::Waiting), in_option(PlayerOptions::None), drop_card(false), take_card(false) {};
	
	Player_state player_state;
	int gold_coins;
	std::unordered_map<int, CharacterCard> character_cards;
	std::vector<BuildingCard> building_cards;
	std::vector<BuildingCard> build_buildings;

	PlayerOptions in_option;
	OptionHandler<CharacterCard> character_card_options;
	OptionHandler<BuildingCard> building_card_options;
	bool drop_card;
	bool take_card;

	int count_color(std::string color) {
		int i = 0;
		std::for_each(build_buildings.begin(), build_buildings.end(), [&](BuildingCard& card) {
			if (card.get_color() == color)
				i++;
		});
		return i;
	}
};
