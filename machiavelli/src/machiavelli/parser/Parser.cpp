#include "machiavelli/parser/Parser.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<BuildingCard> Parser::make_buildings() {
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

std::map<int,CharacterCard> Parser::make_characters() {
	std::map<int, CharacterCard> charactercards;

	std::ifstream file;

	file.open("layouts/karakterkaarten.csv");
	CharacterCard character;
	int rank = 0;
	while (file >> character)
	{
		charactercards.at(rank) = character;
		rank++;
	}
	return charactercards;
}
//TODO::end line in for_each loop not in object
void Parser::save_characters(std::vector<BuildingCard> buildingcards){
	std::ofstream file;

	file.open("layouts/Bouwkaarten.csv");
	std::for_each(buildingcards.begin(), buildingcards.end(), [&](BuildingCard buildingcard) { file << buildingcard; });
}

void Parser::save_buildings(std::vector<CharacterCard> charactercards) {
	std::ofstream file;

	file.open("layouts/karakterkaarten.csv");
	std::for_each(charactercards.begin(), charactercards.end(), [&](CharacterCard charactercard) { file << charactercard; });
}

