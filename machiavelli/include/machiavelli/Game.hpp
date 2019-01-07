#pragma once
#include <server\ServerCallbackHandler.hpp>
#include <server\command\ServerCommand.hpp>

class Game : public ServerCallbackHandler {
public:
	Game();
	bool on_command(ClientCommand) override;
	std::shared_ptr<ClientInfo> on_client_register(Socket) const override;
	Event on_client_input(std::weak_ptr<ClientInfo> client, std::string input) const override;

private:
	std::unordered_map<std::string, ServerCommand<Game&, std::weak_ptr<ClientInfo>>> _commands;
};
