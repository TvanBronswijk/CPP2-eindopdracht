#pragma once
#include <server\ServerCallbackHandler.hpp>
#include <server\command\ServerCommand.hpp>
#include "machiavelli/objects/CharacterCard.hpp"
#include "machiavelli/objects/BuildingCard.hpp"
#include "machiavelli/parser/Parser.hpp"
#include <unordered_map>

class Game : public ServerCallbackHandler {
	using CharacterFunction = std::function<void(Game&, std::weak_ptr<ClientInfo>)>;
public:
	Game();
	bool on_command(ClientCommand) override;
	std::shared_ptr<ClientInfo> on_client_register(Socket) const override;
	Event on_client_input(std::weak_ptr<ClientInfo> client, std::string input) const override;
	
	void next_turn(std::weak_ptr<ClientInfo> client);
	void turn_to_pick_Character_cards(std::weak_ptr<ClientInfo> clientinfo) const;
	std::weak_ptr<ClientInfo> get_next_player();
	bool random_start_of_round(std::weak_ptr<ClientInfo> client);
	int get_players_turn() { return players_turn; }

	std::unordered_map<int, CharacterCard> charactercards_;
private:
	std::unordered_map<std::string, ServerCommand<Game&, std::weak_ptr<ClientInfo>>> _commands;
	std::unordered_map<int, CharacterFunction> _characterfunctions;
	std::vector<BuildingCard> buildingcards_;
	bool check_for_eight_buildings();

	void calculate_highscore();
	void prepare_round();
	int characters_turn;
	int players_turn;
	int king;

	int skip;
};
