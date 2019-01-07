#include "machiavelli/Game.hpp"
#include "machiavelli/MachiavelliData.hpp"
#include "server/Server.hpp"
#include "machiavelli/util.hpp"
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

using namespace validate;
Game::Game() : charactercards_(parsing::make_characters()), buildingcards_(parsing::make_buildings()), characters_turn(1),players_turn(1) {
	_commands =
	{
		{
			"!hello_world",
			{"Hello World!",
			[&](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
			[&](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
				auto& sock = client.lock()->get_socket();
				sock << "Hello to you too." << "\r\n";
			}}
		},
		{
			"!take_gold",
			{"Get Gold at Start of Turn",
			[&](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
			[&](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {

			}}
		},
		{
			"!draw_cards",
			{"Get Cards at Start of Turn",
			[&](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
			[&](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {

			}}
		},
		{
			"!take_cc",
			{"Take a character card",
			[&](StringArgs args) { return validate_that<std::string>(args[0], is_single_digit) && validate_that<std::string>(args[0], is_between(1,9)); },
			[&](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
				auto number = std::stoi(args[0]);
				auto card = game.charactercards_.find(number);
				if (auto clientInfo = client.lock()) {
					auto& data = clientInfo->get_player().get_data<MachiavelliData>();
					if (!data.take_card) {
						data.character_cards[card->first] = card->second;
						data.take_card = true;
						game.charactercards_.erase(number);
						if (data.drop_card && data.take_card) {
							data.drop_card = false;
							data.take_card = false;
							if (game.charactercards_.size() != 0) {
								auto player = this->get_next_player();
								this->turn_to_pick_Character_cards(player);
							}
							else {
								this->next_turn(client);
							}
						}
						else {
							this->turn_to_pick_Character_cards(client);
						}
					}
				}
			}}
		},
		{
			"!drop_cc",
			{"Remove a character card from turn",
			[&](StringArgs args) { return validate_that<std::string>(args[0], is_single_digit) && validate_that<std::string>(args[0], is_between(1,9)); },
			[&](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
				auto number = std::stoi(args[0]);
				game.charactercards_.erase(number);
				if (auto clientInfo = client.lock()) {
					auto& data = clientInfo->get_player().get_data<MachiavelliData>();
					if (!data.drop_card) {
						data.drop_card = true;
						if (data.drop_card && data.take_card) {
							data.drop_card = false;
							data.take_card = false;
							if (game.charactercards_.size() != 0) {
								auto player = this->get_next_player();
								this->turn_to_pick_Character_cards(player);
							}
							else {
								this->next_turn(client);
							}
						}
						else {
							this->turn_to_pick_Character_cards(client);
						}
					}
				}
			}}
		}
	};
}

std::weak_ptr<ClientInfo> Game::get_next_player() {
	std::weak_ptr<ClientInfo> clientinfo;
	players_turn++;
	if(players_turn == 3) {
		players_turn = 1;
	}
	auto clients = _server->get_clients();
	int number = 1;
	std::for_each(clients.begin(), clients.end(), [this, &number, &clientinfo](std::weak_ptr<ClientInfo> client)
	{
		if (number == this->get_players_turn()) {
			clientinfo = client;
		}
		number++;
	});
	return clientinfo;
}


void Game::next_turn(std::weak_ptr<ClientInfo> client){
	auto characters_turn_ = characters_turn;
	if (auto clientInfo = client.lock()) {
		auto& socket = clientInfo->get_socket();
		socket << "Your turn is over";
		auto& player_data = clientInfo->get_player().get_data<MachiavelliData>();
		player_data.player_state = Player_state::Waiting;

		auto clients = _server->get_clients();
		while (characters_turn < 9) {
			std::for_each(clients.begin(), clients.end(), [characters_turn_](std::weak_ptr<ClientInfo> clientinfo)
			{
				if (auto clientInfo = clientinfo.lock()) {
					auto& client_character_cards = clientInfo->get_player().get_data<MachiavelliData>().character_cards;
					if (client_character_cards.find((characters_turn_)) != client_character_cards.end()) {
						auto& socket = clientInfo->get_socket();
						auto& player_data = clientInfo->get_player().get_data<MachiavelliData>();
						socket << "It is your turn!";
						player_data.player_state = Player_state::Playing;
						return;
					}
				}
			});
			characters_turn++;
		}
	}
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
	sock << "Welcome to " << _server->name() << "! To exit, type '!quit'. To see all commands, type '!help'.\r\nWhat's your name?\r\n" << _server->prompt();
	std::string name;
	while (!sock.readline([&name](std::string input) { name = input; }));


	auto clients = _server->get_clients();
	if (clients.size() == 1) {//start a game
		sock << "lets start the game!" << _server->prompt();
		std::for_each(clients.begin(), clients.end(), [this](std::weak_ptr<ClientInfo> clientinfo)
		{
			clientinfo.lock()->get_player().get_data<MachiavelliData>().drop_card = true;
			turn_to_pick_Character_cards(clientinfo);
		});
	}
	return std::make_shared<ClientInfo>(std::move(sock), Player{ name, std::make_unique<MachiavelliData>() });
}

void Game::turn_to_pick_Character_cards(std::weak_ptr<ClientInfo> clientinfo) const{
		int random1 = 0;
		int random2 = 0;
		if (auto clientInfo = clientinfo.lock()) {
			while (random1 == random2) {
				random1 = random_int(1, 8);
				random2 = random_int(1, 8);
			}
			auto& client_data = clientInfo->get_player().get_data<MachiavelliData>();
			client_data.player_state = Player_state::Collecting;
			auto& client_socket = clientInfo->get_socket();
			client_socket << "Pick or drop a card!\r\nWhich card do you want:\r\n";
			std::for_each(charactercards_.begin(), charactercards_.end(), [this, &client_socket](std::pair<int, CharacterCard> card)
			{
				client_socket << "[" + std::to_string(card.first) + "] " + card.second.get_name() + "\r\n";
			});
			client_socket << _server->prompt();
	}
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
	}
	else {
		_server->enqueue_command(ClientCommand{ input, client });
		return Event::command;
	}
}
