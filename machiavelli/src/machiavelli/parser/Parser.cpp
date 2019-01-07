#include "machiavelli/parser/Parser.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>

void Parser::make_buildings() {
	std::ifstream file;

	file.open("layouts/Bouwkaarten.csv");
	BuildingCard building;
	while (file >> building)
	{
	}
}

void Parser::make_characters() {
	std::ifstream file;

	file.open("layouts/karakterkaarten.csv");
	CharacterCard character;
	while (file >> character)
	{

	}
}

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

