#include "machiavelli/parser/Parser.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include "machiavelli/util.hpp"

namespace parsing {
	std::vector<BuildingCard> make_buildings() {
		std::vector<BuildingCard> buildingcards;

		std::ifstream file;

		file.open("layouts/Bouwkaarten.csv");
		BuildingCard building;
		while (file >> building)
		{
			buildingcards.emplace_back(building);
		}
		return buildingcards;
	}

	std::unordered_map<int, CharacterCard> make_characters() {
		std::unordered_map<int, CharacterCard> charactercards{};

		std::ifstream file;

		file.open("layouts/karakterkaarten.csv");
		CharacterCard character{};
		while (file >> character)
		{
			charactercards[character.get_number()] = character;
		}
		auto random1 = random_int(1, 8);
		charactercards.erase(random1);
		return charactercards;
	}
	//TODO::end line in for_each loop not in object
	void save_characters(std::vector<BuildingCard> buildingcards) {
		std::ofstream file;

		file.open("layouts/Bouwkaarten.csv");
		std::for_each(buildingcards.begin(), buildingcards.end(), [&](BuildingCard buildingcard) { file << buildingcard; });
	}

	void save_buildings(std::vector<CharacterCard> charactercards) {
		std::ofstream file;

		file.open("layouts/karakterkaarten.csv");
		std::for_each(charactercards.begin(), charactercards.end(), [&](CharacterCard charactercard) { file << charactercard; });
	}
}


