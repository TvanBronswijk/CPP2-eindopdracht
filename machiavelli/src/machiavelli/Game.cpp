#include "machiavelli/Game.hpp"
#include "machiavelli/MachiavelliData.hpp"
#include "server/Server.hpp"
#include "machiavelli/util.hpp"
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

using namespace validate;
Game::Game() : charactercards_(parsing::make_characters()), buildingcards_(parsing::make_buildings()), players_turn(1), characters_turn(0), king(1), skip(-1) {
	_characterfunctions = {
		{1, [](Game& game, std::weak_ptr<ClientInfo> client) { //moordenaar
			if (auto clientinfo = client.lock()) {
				auto& sock = clientinfo->get_socket();
				auto& player = clientinfo->get_player();
				auto& data = player.get_data<MachiavelliData>();
				
				game.charactercards_ = parsing::make_characters();
				data.in_option = MachiavelliData::PlayerOptions::Character;
				std::vector<CharacterCard*> characters{};
				std::transform(game.charactercards_.begin(), game.charactercards_.end(), std::back_inserter(characters),
					[](auto& pair) { return &(pair.second); });
				data.character_card_options = { characters, [&](CharacterCard& character) { game.skip = character.get_number(); } };
				sock << "Choose one:" << "\r\n";
				for (int i = 0; i < characters.size(); i++) {
					sock << "[" << std::to_string(i) << "] " << characters[i]->get_name() << "\r\n";
				}
			}
		}},
		{2, [](Game& game, std::weak_ptr<ClientInfo> client) { //dief
			if (auto clientinfo = client.lock()) {
				auto& sock = clientinfo->get_socket();
				auto& player = clientinfo->get_player();
				auto& data = player.get_data<MachiavelliData>();

				game.charactercards_ = parsing::make_characters();
				data.in_option = MachiavelliData::PlayerOptions::Character;
				std::vector<CharacterCard*> characters{};
				std::transform(game.charactercards_.begin(), game.charactercards_.end(), std::back_inserter(characters),
					[](auto& pair) { return &(pair.second); });
				data.character_card_options = { characters, [&](CharacterCard& character) { data.gold_coins += 2; } };
				sock << "Choose one:" << "\r\n";
				for (int i = 0; i < characters.size(); i++) {
					sock << "[" << std::to_string(i) << "] " << characters[i]->get_name() << "\r\n";
				}
			}
		}},
		{3, [](Game& game, std::weak_ptr<ClientInfo> client) { //magier
			if (auto clientinfo = client.lock()) {
				auto& sock = clientinfo->get_socket();
				auto& player = clientinfo->get_player();
				auto& data = player.get_data<MachiavelliData>();
				int size = data.building_cards.size();
				data.building_cards.clear();
				for(int i = 0; i < size; i++)
					data.building_cards.push_back(game.buildingcards_[random_int(i, game.buildingcards_.size())]);

			}
		}},
		{4, [](Game& game, std::weak_ptr<ClientInfo> client) { //koning
			if (auto clientinfo = client.lock()) {
				auto& sock = clientinfo->get_socket();
				auto& player = clientinfo->get_player();
				auto& data = player.get_data<MachiavelliData>();
				data.gold_coins += data.count_color("geel");
				game.king = game.king == 1 ? 2 : 1;
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
				if (!data.took_gold_or_cards) {
					data.took_gold_or_cards = true;
					data.building_cards.push_back(game.buildingcards_[random_int(0, game.buildingcards_.size() - 1)]);
					data.building_cards.push_back(game.buildingcards_[random_int(0, game.buildingcards_.size() - 1)]);
					sock << "drew 2 cards.\r\n";
				}
				else {
					sock << "you can't do that right now.\r\n";
				}

			}
		}},
		{8, [](Game& game, std::weak_ptr<ClientInfo> client) { //condotiere
			if (auto clientinfo = client.lock()) {
				auto& sock = clientinfo->get_socket();
				auto& player = clientinfo->get_player();
				auto& data = player.get_data<MachiavelliData>();
				data.gold_coins += data.count_color("rood");

				auto clients = game._server->registry().clients;
				Player* other;
				std::for_each(clients.begin(), clients.end(), [&player, &other](std::weak_ptr<ClientInfo> client)
				{
					if (auto clientinfo = client.lock()) {
						auto op = &clientinfo->get_player();
						if (op != &player) {
							other = op;
						}
					}
				});

				auto& other_data = other->get_data<MachiavelliData>();
				data.in_option = MachiavelliData::PlayerOptions::Building;
				std::vector<BuildingCard*> buildings{};
				std::transform(other_data.building_cards.begin(), other_data.building_cards.end(), std::back_inserter(buildings),
					[](auto& ent) { return &(ent); });
				data.building_card_options = { buildings, [&](BuildingCard& building) { 
					other_data.building_cards.erase(std::remove_if(other_data.building_cards.begin(), other_data.building_cards.end(), [&](BuildingCard& b) 
					{ return &b == &building; }),
						other_data.building_cards.end());
				} };
				sock << "Choose one:" << "\r\n";
				for (int i = 0; i < buildings.size(); i++) {
					sock << "[" << std::to_string(i) << "] " << buildings[i]->get_name() << "\r\n";
				}
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
					if (clientinfo->get_player().get_data<MachiavelliData>().player_state == Player_state::Playing) {
						auto& sock = clientinfo->get_socket();
						auto& player = clientinfo->get_player();
						auto& data = player.get_data<MachiavelliData>();
						if (!data.took_gold_or_cards) {
							data.gold_coins += 2;
							data.took_gold_or_cards = true;
							sock << "recieved 2 gold." << "\r\n";
						}
						else {
							sock << "you can't do this right now.\r\n";
						}

					}
				}
			}}
		},
		{
			"!draw_cards",
			{"Get Cards at Start of Turn [No Arguments]",
			[&](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
			[&](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {

				if (auto clientinfo = client.lock()) {
					auto& sock = clientinfo->get_socket();
					if (clientinfo->get_player().get_data<MachiavelliData>().player_state == Player_state::Playing) {
						auto& sock = clientinfo->get_socket();
						auto& player = clientinfo->get_player();
						auto& data = player.get_data<MachiavelliData>();
						if (!data.took_gold_or_cards) {
							data.took_gold_or_cards = true;
							data.building_cards.push_back(buildingcards_[random_int(0, buildingcards_.size() - 1)]);
							sock << "you drew a card.\r\n";
						}
						else {
							sock << "you can't do this right now.\r\n";
						}

					}
				}
			}}
		},
		{
			"!use_card",
			{"Use one of your special cards.",
			[&](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
			[this](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
				this->_characterfunctions.at(characters_turn)(*this, client);
			}}
		},
		{
			"!hand",
			{"Show your hand",
			[&](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
			[this](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
				if (auto clientinfo = client.lock()) {
					if (clientinfo->get_player().get_data<MachiavelliData>().player_state == Player_state::Playing) {
						auto& sock = clientinfo->get_socket();
						auto& player = clientinfo->get_player();
						auto& data = player.get_data<MachiavelliData>();
						sock << "hand:\r\n";
						for (int i = 0; i < data.building_cards.size(); i++) {
							sock << "[" << i << "] " << data.building_cards[i].get_name() << " (" << data.building_cards[i].get_coins() << ")" << "\r\n";
						}
					}
				}
			}}
		},
		{
			"!build",
			{"Build a building from your hand. [ARG1: Card Number In Hand]",
			[&](StringArgs args) { return validate_that<StringArgs>(args, has_arg<std::string>(0))
			        && validate_that<std::string>(args[0], is_single_digit<std::string>); },
			[this](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
				if (auto clientinfo = client.lock()) {
					if (clientinfo->get_player().get_data<MachiavelliData>().player_state == Player_state::Playing) {
						auto& sock = clientinfo->get_socket();
						auto& player = clientinfo->get_player();
						auto& data = player.get_data<MachiavelliData>();

						auto& chosen_card = data.building_cards[std::stoi(args[0])];
						if (chosen_card.get_coins() < data.gold_coins) {
							data.gold_coins -= chosen_card.get_coins();
							data.build_buildings.push_back(chosen_card);
							sock << "you build a " << chosen_card.get_name();
						}
						else {
							sock << "you can't do this right now.\r\n";
						}
					}
				}
			}}
		},
		{
			"!take_cc",
			{"Take a character card",
			[&](StringArgs args) { return validate_that<StringArgs>(args, has_arg<std::string>(0))
			        && validate_that<std::string>(args[0], is_single_digit<std::string>)
			        && validate_that<std::string>(args[0], is_between<std::string>(1,9)); },
			[&](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
				auto number = std::stoi(args[0]);
				auto card = game.charactercards_.find(number);
				if (auto clientInfo = client.lock()) {
					if (clientInfo->get_player().get_data<MachiavelliData>().player_state == Player_state::Collecting) {
						auto& data = clientInfo->get_player().get_data<MachiavelliData>();
						if (!data.take_card) {
							data.character_cards[card->first] = card->second;
							data.take_card = true;
							game.charactercards_.erase(number);
							if (data.drop_card && data.take_card) {
								data.drop_card = false;
								data.take_card = false;
								if (game.charactercards_.size() != 0) {
									data.player_state = Player_state::Waiting;
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
				}
			}}
		},
		{
			"!drop_cc",
			{"Remove a character card from turn",
			[&](StringArgs args) { return validate_that<StringArgs>(args, has_arg<std::string>(0))
			        && validate_that<std::string>(args[0], is_single_digit<std::string>)
			        && validate_that<std::string>(args[0], is_between<std::string>(1,9)); },
			[&](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
				auto number = std::stoi(args[0]);
				game.charactercards_.erase(number);
				if (auto clientInfo = client.lock()) {
					if (clientInfo->get_player().get_data<MachiavelliData>().player_state == Player_state::Collecting) {
						auto& data = clientInfo->get_player().get_data<MachiavelliData>();
						if (!data.drop_card) {
							data.drop_card = true;
							if (data.drop_card && data.take_card) {
								data.drop_card = false;
								data.take_card = false;
								if (game.charactercards_.size() != 0) {
									data.player_state = Player_state::Waiting;
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
				}
			}}
		},
			{
				"!finish",
				{ "finish a turn",
				[&](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
				[this](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
					if (client.lock()->get_player().get_data<MachiavelliData>().player_state == Player_state::Playing) {
					this->next_turn(client);
					}
				} }
			},
				{
				"!get_random",
				{ "get random starting characters",
				[&](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
				[this](StringArgs args, Game& game, std::weak_ptr<ClientInfo> client) {
					if (client.lock()->get_player().get_data<MachiavelliData>().player_state == Player_state::Collecting) {
						if (this->random_start_of_round(client)) {
							this->next_turn(client);
						}
					}
				} 
			}
		}
	};
}

bool Game::random_start_of_round(std::weak_ptr<ClientInfo> client) {
	if (charactercards_.size() == 7) {
		auto clients = _server->registry().clients;
		std::for_each(clients.begin(), clients.end(), [this](std::weak_ptr<ClientInfo> clientinfo)
		{
			if (auto clientInfo = clientinfo.lock()) {
				int random1 = random_int(1, 8);
				int random2 = random_int(1, 8);
				auto card1 = charactercards_.find(random1);
				auto card2 = charactercards_.find(random2);
				while (random1 == random2 || card1 == charactercards_.end() || card2 == charactercards_.end()) {
					random1 = random_int(1, 8);
					random2 = random_int(1, 8);
					card1 = charactercards_.find(random1);
					card2 = charactercards_.find(random2);
				}
				auto& client_character_cards = clientInfo->get_player().get_data<MachiavelliData>().character_cards;
				client_character_cards[random1] = charactercards_.find(random1)->second;
				client_character_cards[random2] = charactercards_.find(random2)->second;
				charactercards_.erase(random1);
				charactercards_.erase(random2);
			}
		});
		charactercards_.clear();
		return true;
	}
	else {
		if (auto clientInfo = client.lock()) {
			clientInfo->get_socket() << "Sorry but this only if all cards are still in the deck...\r\n";
		}
		return false;
	}
}

std::weak_ptr<ClientInfo> Game::get_next_player() {
	std::weak_ptr<ClientInfo> clientinfo;
	players_turn++;
	if (players_turn == 3) {
		players_turn = 1;
	}
	auto clients = _server->registry().clients;
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


void Game::next_turn(std::weak_ptr<ClientInfo> client) {
	characters_turn++;
	if (characters_turn == skip)
		characters_turn++;
	bool found = false;
	auto& characters_turn_ = characters_turn;
	if (auto clientInfo = client.lock()) {
		auto& socket = clientInfo->get_socket();
		socket << "Your turn is over\r\n";
		auto& player_data = clientInfo->get_player().get_data<MachiavelliData>();
		player_data.player_state = Player_state::Waiting;
		player_data.took_gold_or_cards = false;

		auto clients = _server->registry().clients;
		if (characters_turn_ == 9) found = true;
		while (!found) {
			std::for_each(clients.begin(), clients.end(), [&characters_turn_, &found, this](std::weak_ptr<ClientInfo> clientinfo)
			{
				if (characters_turn_ == 9) found = true;
				if (auto clientInfo = clientinfo.lock()) {
					auto& client_character_cards = clientInfo->get_player().get_data<MachiavelliData>().character_cards;
					if (client_character_cards.find((characters_turn_)) != client_character_cards.end()) {
						auto& socket = clientInfo->get_socket();
						auto& player_data = clientInfo->get_player().get_data<MachiavelliData>();
						socket << "It is the turn of the: " + client_character_cards.find(characters_turn_)->second.get_name() + "!\r\n" << _server->prompt();
						player_data.player_state = Player_state::Playing;
						found = true;
					}
				}
			});
			if(!found)
			characters_turn++;
		}
	}
	if (characters_turn >= 9) {
		if (check_for_eight_buildings()) {
			calculate_highscore();
		}
		else {
			prepare_round();
			auto player = get_next_player();
			turn_to_pick_Character_cards(player);
		}
	}
}
void Game::calculate_highscore() {
	auto clients = _server->registry().clients;
	std::for_each(clients.begin(), clients.end(), [](std::weak_ptr<ClientInfo> clientinfo)
	{
		if (auto clientInfo = clientinfo.lock()) {
			auto amount_of_buildings = clientInfo->get_player().get_data<MachiavelliData>().build_buildings.size();
			clientInfo->get_socket() << "Game ended. You build: " << std::to_string(amount_of_buildings) << " buildings." << "\r\n";
		}
	});
}

void Game::prepare_round() {
	players_turn = king;
	charactercards_ = parsing::make_characters();
	characters_turn = 0;
	auto clients = _server->registry().clients;
	int number = 1;
	std::for_each(clients.begin(), clients.end(), [this,&number](std::weak_ptr<ClientInfo> clientinfo)
	{
		if (auto clientInfo = clientinfo.lock()) {
			clientInfo->get_player().get_data<MachiavelliData>().character_cards.clear();
			clientInfo->get_player().get_data<MachiavelliData>().player_state = Player_state::Waiting;

			clientinfo.lock()->get_player().get_data<MachiavelliData>().drop_card = false;
			clientinfo.lock()->get_player().get_data<MachiavelliData>().take_card = false;
			if (number == get_players_turn()) {
				clientinfo.lock()->get_player().get_data<MachiavelliData>().drop_card = true;
			}
		}
	});
}

bool Game::check_for_eight_buildings() {
	int buildings = 0;
	auto clients = _server->registry().clients;

	std::for_each(clients.begin(), clients.end(), [&buildings](std::weak_ptr<ClientInfo> clientinfo)
	{
		if (auto clientInfo = clientinfo.lock()) {
			auto amount_of_buildings = clientInfo->get_player().get_data<MachiavelliData>().build_buildings.size();
			if (amount_of_buildings > buildings) buildings = amount_of_buildings;
		}
	});

	return (buildings >= 8);
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
	return false;
}

std::shared_ptr<ClientInfo> Game::on_client_register(Socket sock) const {
	sock << "Welcome to " << _server->name() << "! To exit, type '!quit'. To see all commands, type '!help'.\r\nWhat's your name?\r\n" << _server->prompt();
	std::string name;
	while (!sock.readline([&name](std::string input) { name = input; }));


	auto clients = _server->registry().clients;
	if (clients.size() == 1) {//start a game
		sock << "lets start the game!\r\n" << _server->prompt();
		std::for_each(clients.begin(), clients.end(), [this](std::weak_ptr<ClientInfo> clientinfo)
		{
			clientinfo.lock()->get_socket() << "lets start the game!\r\n";
			clientinfo.lock()->get_player().get_data<MachiavelliData>().drop_card = true;
			clientinfo.lock()->get_player().get_data<MachiavelliData>().player_state = Player_state::Collecting;
			turn_to_pick_Character_cards(clientinfo);
			for (int i = 0; i < 4; i++) {
				clientinfo.lock()->get_player().get_data<MachiavelliData>().building_cards.push_back(buildingcards_[random_int(i, buildingcards_.size() - 1)]);
			}
		});
	}
	auto data = std::make_unique<MachiavelliData>();
	for (int i = 0; i < 4; i++) {
		data->building_cards.push_back(buildingcards_[random_int(i, buildingcards_.size() - 1)]);
	}
	return std::make_shared<ClientInfo>(std::move(sock), Player{ name, std::move(data) });
}

void Game::turn_to_pick_Character_cards(std::weak_ptr<ClientInfo> clientinfo) const {
	if (auto clientInfo = clientinfo.lock()) {
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
		return Event::text;
	}
	else {
		_server->enqueue_command(ClientCommand{ input, client });
		return Event::command;
	}
}

Game::~Game() {
	_commands.clear();
	_characterfunctions.clear();
	charactercards_.clear();
	buildingcards_.clear();
}
