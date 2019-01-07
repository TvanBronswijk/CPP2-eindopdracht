#include "machiavelli/Game.hpp"
#include "machiavelli/MachiavelliData.hpp"
#include "server/Server.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

using namespace validate;
Game::Game() {
	_commands =
	{
		{
			"!hello",
			{"Hello", [&](StringArgs args) {
		return validate_that<StringArgs>(args, is_empty<std::string>);
	}, [&](validate::StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
		auto& sock = client.lock()->get_socket();
		sock << "Hello to you too." << "\r\n";
	}}
		}
	};
}

bool Game::on_command(ClientCommand com) {
	
	std::istringstream iss(com.get_cmd());
	std::vector<std::string> words{ std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{} };
	auto it = _commands.find(words[0]);
	if (it != _commands.end()) {
		try {
			std::vector<std::string> args(words.begin() + 1, words.end());
			(*it).second.try_action(args, *this, com.get_client_info());
			return true;
		}
		catch (std::exception e) {
			com.get_client_info().lock()->get_socket() << e.what() << "\r\n";
			return false;
		}
	}
	else {
		return false;
	}
}

std::shared_ptr<ClientInfo> Game::on_client_register(Socket sock) const {
	sock << "Welcome to " << _server->name() << "! To exit, type '!quit'.\r\nWhat's your name?\r\n" << _server->prompt();
	std::string name;
	while (!sock.readline([&name](std::string input) { name = input; }));
	return std::make_shared<ClientInfo>(std::move(sock), Player{ name, std::make_unique<MachiavelliData>() });
}

ServerCallbackHandler::Event Game::on_client_input(std::weak_ptr<ClientInfo> client, std::string input) const {
	if (input == "!quit") {
		return Event::quit;
	}
	else if (input == "___quit_server") {
		return Event::server_stop;
	}
	else {
		_server->enqueue_command(ClientCommand{ input, client });
		return Event::command;
	}
}
