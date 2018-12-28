#pragma once
#include <server\ServerCallbackHandler.hpp>

class Game : public ServerCallbackHandler {
public:
	Game() = default;
	bool on_command(ClientCommand) override;
	std::shared_ptr<ClientInfo> on_client_register(Socket) const override;
	void on_client_input(std::weak_ptr<ClientInfo> client, std::string input) const override;
};
