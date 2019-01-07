#include "machiavelli/Game.hpp"
#include "machiavelli/MachiavelliData.hpp"
#include "server/Server.hpp"

bool Game::on_command(ClientCommand com) {
	return false;
}

std::shared_ptr<ClientInfo> Game::on_client_register(Socket sock) const {
	sock << "Welcome to " << _server->name() << "!\r\nWhat's your name?\r\n" << _server->prompt();
	std::string name;
	while (!sock.readline([&name](std::string input) { name = input; }));
	return std::make_shared<ClientInfo>(std::move(sock), Player{ name, std::make_unique<MachiavelliData>() });
}

void Game::on_client_input(std::weak_ptr<ClientInfo> client, std::string input) const {
	if (input == "quit") {
		return;
	}
	else if (input == "quit_server") {
		_server->stop();
	}
	else {
		_server->enqueue_command(ClientCommand{ input, client });
	}
}
