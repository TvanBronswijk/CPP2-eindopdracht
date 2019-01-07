#pragma once
#include <server\ServerCallbackHandler.hpp>
#include <server\command\ServerCommand.hpp>
#include "machiavelli/objects/CharacterCard.hpp"
#include "machiavelli/objects/BuildingCard.hpp"
#include "machiavelli/parser/Parser.hpp"
#include <unordered_map>

class Game : public ServerCallbackHandler {
public:
	Game();
	bool on_command(ClientCommand) override;
	std::shared_ptr<ClientInfo> on_client_register(Socket) const override;
	Event on_client_input(std::weak_ptr<ClientInfo> client, std::string input) const override;

private:
	std::unordered_map<std::string, ServerCommand<Game&, std::weak_ptr<ClientInfo>>> _commands;
	std::unordered_map<int, CharacterCard> charactercards_;
	std::vector<BuildingCard> buildingcards_;

	void next_turn(std::weak_ptr<ClientInfo> client);
};
