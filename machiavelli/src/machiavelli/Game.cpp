#include "machiavelli/Game.hpp"
#include "machiavelli/MachiavelliData.hpp"
#include "server/Server.hpp"
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

using namespace validate;
Game::Game() : charactercards_(parsing::make_characters()), buildingcards_(parsing::make_buildings()) {
	_characterfunctions = {
		{1, [](Game& game, std::weak_ptr<ClientInfo> client) { //moordenaar
			if (auto clientinfo = client.lock()) {
				auto& sock = clientinfo->get_socket();
				auto& player = clientinfo->get_player();
				auto& data = player.get_data<MachiavelliData>();
				
				data.in_option = MachiavelliData::PlayerOptions::Character;
				std::vector<CharacterCard*> characters{};
				std::transform(game.charactercards_.begin(), game.charactercards_.end(), std::back_inserter(characters),
					[](auto& pair) { return &pair.second; });
				sock << "Choose one:" << "\r\n";
				for (int i = 0; i < my_characters.size(); i++) {
					sock << "[" << std::to_string(i) << "] " << my_characters[i]->get_name() << "\r\n";
				}
			}
		}},
		{2, [](Game& game, std::weak_ptr<ClientInfo> client) { //dief
			if (auto clientinfo = client.lock()) {
				auto& sock = clientinfo->get_socket();
				auto& player = clientinfo->get_player();
				auto& data = player.get_data<MachiavelliData>();
			}
		}},
		{3, [](Game& game, std::weak_ptr<ClientInfo> client) { //magier
			if (auto clientinfo = client.lock()) {
				auto& sock = clientinfo->get_socket();
				auto& player = clientinfo->get_player();
				auto& data = player.get_data<MachiavelliData>();
			}
		}},
		{4, [](Game& game, std::weak_ptr<ClientInfo> client) { //koning
			if (auto clientinfo = client.lock()) {
				auto& sock = clientinfo->get_socket();
				auto& player = clientinfo->get_player();
				auto& data = player.get_data<MachiavelliData>();
				data.gold_coins += data.count_color("geel");
			}
		}},
		{5, [](Game& game, std::weak_ptr<ClientInfo> client) { //prediker
			if (auto clientinfo = client.lock()) {
				auto& sock = clientinfo->get_socket();
				auto& player = clientinfo->get_player();
				auto& data = player.get_data<MachiavelliData>();
				data.gold_coins += data.count_color("blauw");
			}
		}},
		{6, [](Game& game, std::weak_ptr<ClientInfo> client) { //koopman
			if (auto clientinfo = client.lock()) {
				auto& sock = clientinfo->get_socket();
				auto& player = clientinfo->get_player();
				auto& data = player.get_data<MachiavelliData>();
				data.gold_coins += 1;
				data.gold_coins += data.count_color("groen");
			}
		}},
		{7, [](Game& game, std::weak_ptr<ClientInfo> client) { //bouwman
			if (auto clientinfo = client.lock()) {
				auto& sock = clientinfo->get_socket();
				auto& player = clientinfo->get_player();
				auto& data = player.get_data<MachiavelliData>();
			}
		}},
		{8, [](Game& game, std::weak_ptr<ClientInfo> client) { //condotiere
			if (auto clientinfo = client.lock()) {
				auto& sock = clientinfo->get_socket();
				auto& player = clientinfo->get_player();
				auto& data = player.get_data<MachiavelliData>();
				data.gold_coins += data.count_color("rood");
			}
		}}
	};
	
	_commands =
	{
		{
			"!hello_world",
			{"Hello World!",
			[&](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
			[&](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
				if (auto clientinfo = client.lock()) {
					auto& sock = clientinfo->get_socket();
					sock << "Hello to you too." << "\r\n";
				}
			}}
		},
		{
			"!take_gold",
			{"Get Gold at Start of Turn [No Arguments]",
			[&](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
			[&](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
				if (auto clientinfo = client.lock()) {
					auto& sock = clientinfo->get_socket();
					auto& player = clientinfo->get_player();
					auto& data = player.get_data<MachiavelliData>();
					data.gold_coins += 2;
					sock << "recieved 2 gold." << "\r\n";
				}
			}}
		},
		{
			"!draw_cards",
			{"Get Cards at Start of Turn [No Arguments]",
			[&](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
			[&](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
				client.lock()->get_socket() << "not yet implemented";
			}}
		},
		{
			"!use_card",
			{"Use one of your special cards.",
			[&](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
			[this](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
				if (auto clientinfo = client.lock()) {
					auto& sock = clientinfo->get_socket();
					auto& player = clientinfo->get_player();
					auto& data = player.get_data<MachiavelliData>();
					data.in_option = MachiavelliData::PlayerOptions::Character;

					std::vector<CharacterCard*> my_characters{};
					std::transform(data.character_cards.begin(), data.character_cards.end(), std::back_inserter(my_characters), 
						[](auto& pair) { return &pair.second; });
					data.character_card_options = { my_characters, [this, &client](CharacterCard& character) { this->_characterfunctions.at(character.get_number())(*this, client); } };
					sock << "Choose one:" << "\r\n";
					for (int i = 0; i < my_characters.size(); i++) {
						sock << "[" << std::to_string(i) << "] " << my_characters[i]->get_name() << "\r\n";
					}
				}
			}}
		},
		{
			"!hand",
			{"Show your hand",
			[&](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
			[this](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
				if (auto clientinfo = client.lock()) {
					auto& sock = clientinfo->get_socket();
					auto& player = clientinfo->get_player();
					auto& data = player.get_data<MachiavelliData>();
					for (int i = 0; i < data.building_cards.size(); i++) {
						sock << "[" << i << "] " << data.building_cards[i].get_name() << "\r\n";
					}
				}
			}}
		},
		{
			"!build",
			{"Build a building from your hand. [ARG1: Card Number In Hand]",
			[&](StringArgs args) { return validate_that<std::string>(args[0], ); },
			[this](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
				if (auto clientinfo = client.lock()) {
					auto& sock = clientinfo->get_socket();
					auto& player = clientinfo->get_player();
					auto& data = player.get_data<MachiavelliData>();
					
					auto& chosen_card = data.building_cards[std::stoi(args[0])];
					if (chosen_card.get_coins() < data.gold_coins) {
						data.gold_coins -= chosen_card.get_coins();
						data.build_buildings.push_back(chosen_card);
						data.building_cards.erase(std::remove(data.building_cards.begin(), data.building_cards.end(), chosen_card), data.building_cards.end());
					}
				}
			}}
		}
	};
}

void Game::next_turn(std::weak_ptr<ClientInfo> client) {
	if (auto clientInfo = client.lock()) {
		auto& client = clientInfo->get_socket();
		auto& player_data = clientInfo->get_player().get_data<MachiavelliData>();
		player_data.player_state = Player_state::Waiting;

	}
}

bool Game::on_command(ClientCommand com) {
	if (auto clientinfo = com.get_client_info().lock()) {
		auto& sock = clientinfo->get_socket();
		auto& player = clientinfo->get_player();
		auto& data = player.get_data<MachiavelliData>();

		if (data.in_option == MachiavelliData::PlayerOptions::None) {
			std::istringstream iss(com.get_cmd());
			std::vector<std::string> words{ std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{} };
			auto it = _commands.find(words[0]);
			if (it != _commands.end()) {
				try {
					std::vector<std::string> args(words.begin() + 1, words.end());
					(*it).second.try_action(args, *this, clientinfo);
					return true;
				}
				catch (std::exception e) {
					sock << e.what() << "\r\n";
					return false;
				}
			}
			else {
				return false;
			}
		}
		else {
			try {
				int i = std::stoi(com.get_cmd());
				if (data.in_option == MachiavelliData::PlayerOptions::Character) {
					data.character_card_options.choose(i);
					data.in_option = MachiavelliData::PlayerOptions::None;
				}
				if (data.in_option == MachiavelliData::PlayerOptions::Building) {
					data.building_card_options.choose(i);
					data.in_option = MachiavelliData::PlayerOptions::None;
				}
			}
			catch (...) {
				sock << "invalid option." << "\r\n";
			}

		}
	}
}

std::shared_ptr<ClientInfo> Game::on_client_register(Socket sock) const {
	sock << "Welcome to " << _server->name() << "! To exit, type '!quit'. To see all commands, type '!help'.\r\nWhat's your name?\r\n" << _server->prompt();
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
	else if (input == "!help") {
		auto& sock = client.lock()->get_socket();
		std::for_each(_commands.begin(), _commands.end(), [&](auto pair) { sock << pair.first << " - " << pair.second.get_description() << "\r\n"; });
		sock << _server->prompt();
		return Event::text;
	}
	else {
		_server->enqueue_command(ClientCommand{ input, client });
		return Event::command;
	}
}
