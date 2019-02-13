#include <machiavelli/Game.hpp>
#include <machiavelli/Context.hpp>
#include <machiavelli/player/GameData.hpp>
#include <machiavelli/states/IdleState.hpp>
#include <machiavelli/states/OptionState.hpp>
#include <machiavelli/states/CharacterCardPickingState.hpp>
#include <machiavelli/states/ActionState.hpp>
#include <server/command/_options.hpp>
#include <util.hpp>
#include <machiavelli/io/IO.hpp>

using namespace server;
using namespace server::command::options;
using namespace server::connection;
using namespace server::player;

Game::Game(std::weak_ptr<server::ClientInfo> player1, std::weak_ptr<server::ClientInfo> player2)
        : _player1(player1), _player2(player2), _king(player1), _curr_turn(0) {
    std::vector<CharacterCard> ccs = io::parse<CharacterCard>("layouts/karakterkaarten.csv");
    std::for_each(ccs.begin(), ccs.end(), [&](CharacterCard &cc) {
        _ccards[cc.order()] = cc;
    });
    std::vector<BuildingCard> bcs = io::parse<BuildingCard>("layouts/Bouwkaarten.csv");
    deck = Hand<BuildingCard>(bcs);
    _cactions = {
            {1, [](Player &player, Socket &socket, Context &context) { //moordenaar
                auto &data = player.get_data<GameData>();
                OptionHandler handler{
                        {"Dief", "Magier", "Koning", "Prediker", "Koopman", "Bouwman", "Condotierre"},
                        [&](int i) {
                            context.game().game_order.erase(i + 2);
                            return true;
                        }
                };
                socket << handler;
                player.get_states().put(std::make_unique<OptionState>(context, handler));
            }},
            {2, [](Player &player, Socket &socket, Context &context) { //dief
                auto &data = player.get_data<GameData>();
                OptionHandler handler{
                        {"Magier", "Koning", "Prediker", "Koopman", "Bouwman", "Condotierre"},
                        [&](int i) {
                            auto other = context.game().game_order.at(i + 3);
                            if (auto otherptr = other.lock()) {
                                auto &otherdata = otherptr->get_player().get_data<GameData>();
                                player.get_data<GameData>().gold_coins += otherdata.gold_coins;
                                otherdata.gold_coins = 0;
                            } else {
                                socket << "could not steal from this person";
                            }
                            return true;
                        }
                };
                socket << handler;
                player.get_states().put(std::make_unique<OptionState>(context, handler));
            }},
            {3, [](Player &player, Socket &socket, Context &context) { //magier
                auto &data = player.get_data<GameData>();
                //TODO
            }},
            {4, [](Player &player, Socket &socket, Context &context) { //koning
                auto &data = player.get_data<GameData>();
                data.gold_coins += data.count_color("geel");
            }},
            {5, [](Player &player, Socket &socket, Context &context) { //prediker
                auto &data = player.get_data<GameData>();
                data.gold_coins += data.count_color("blauw");
            }},
            {6, [](Player &player, Socket &socket, Context &context) { //koopman
                auto &data = player.get_data<GameData>();
                data.gold_coins += 1;
                data.gold_coins += data.count_color("groen");
            }},
            {7, [](Player &player, Socket &socket, Context &context) { //bouwman
                auto &data = player.get_data<GameData>();
                //TODO
            }},
            {8, [](Player &player, Socket &socket, Context &context) { //condotiere
                auto &data = player.get_data<GameData>();
                //TODO
            }}
    };
}

void Game::start(Context &ctx, bool random) {
    auto poneptr = player_one().lock();
    auto ptwoptr = player_two().lock();
    if (poneptr && ptwoptr) {
        auto &datone = poneptr->get_player().get_data<GameData>();
        auto &dattwo = ptwoptr->get_player().get_data<GameData>();
        datone.gold_coins = 2;
        dattwo.gold_coins = 2;
        for (int i = 0; i < 4; ++i) {
            datone.building_cards.add(deck.take_random());
            dattwo.building_cards.add(deck.take_random());
        }
        poneptr->get_socket() << "Game has started!\r\n";
        ptwoptr->get_socket() << "Game has started!\r\n";
        if (random) {
            draw_random(ctx);
        } else {
            draw_order(ctx);
        }
    } else {
        throw -1;
    }
}

void Game::draw_random(Context &ctx) {
    character_cards = Hand<CharacterCard>();
    std::for_each(_ccards.begin(), _ccards.end(), [&](auto &pair) {
        character_cards.add(pair.second);
    });

    auto poneptr = player_one().lock();
    auto ptwoptr = player_two().lock();
    if (poneptr && ptwoptr) {
        auto &datone = poneptr->get_player().get_data<GameData>();
        auto &dattwo = ptwoptr->get_player().get_data<GameData>();
        datone.character_cards = Hand<CharacterCard>();
        dattwo.character_cards = Hand<CharacterCard>();
        auto cc = character_cards.take_random();
        game_order[cc.order()] = poneptr;
        datone.character_cards.add(std::move(cc));
        cc = character_cards.take_random();
        game_order[cc.order()] = ptwoptr;
        dattwo.character_cards.add(std::move(cc));
        cc = character_cards.take_random();
        game_order[cc.order()] = poneptr;
        datone.character_cards.add(std::move(cc));
        cc = character_cards.take_random();
        game_order[cc.order()] = ptwoptr;
        dattwo.character_cards.add(std::move(cc));
        next_turn(ctx);
    } else {
        throw -1;
    }
}

void Game::draw_order(Context &ctx) {
    character_cards = Hand<CharacterCard>();
    std::for_each(_ccards.begin(), _ccards.end(), [&](auto &pair) {
        character_cards.add(pair.second);
    });

    auto poneptr = player_one().lock();
    auto ptwoptr = player_two().lock();
    if (poneptr && ptwoptr) {
        auto &datone = poneptr->get_player().get_data<GameData>();
        auto &dattwo = ptwoptr->get_player().get_data<GameData>();
        character_cards.take_random(); //throw away first
        if (auto kingptr = _king.lock()) {
            kingptr->get_socket() << "Pick one to draw:\r\n";
            for(int i = 0; i < character_cards.size(); i++)
                kingptr->get_socket() << '[' << std::to_string(i) << "] " << character_cards.peek(i).name() << "\r\n";
            kingptr->get_player().get_states().put(std::make_unique<CharacterCardPickingState>(ctx, true));
        }

    } else {
        throw -1;
    }
}


void Game::next_turn(Context &ctx) {
    std::weak_ptr<ClientInfo> player;

    do {
        auto it = game_order.find(++_curr_turn);
        if (it != game_order.end())
            player = (*it).second;
    } while (!player.lock() && _curr_turn <= 8);
    if (_curr_turn > 8) {
        if(!calculate_score(ctx)) {
            player_one().lock()->get_player().get_data<GameData>().character_cards.clear();
            player_two().lock()->get_player().get_data<GameData>().character_cards.clear();
            draw_order(ctx);
            _curr_turn = 0;
        }
    } else {
        if (auto playerptr = player.lock()) {
            playerptr->get_player().get_states().put(std::make_unique<ActionState>(ctx));
            playerptr->get_socket() << "It is your turn! You are the " << _ccards[_curr_turn].name() << "!\r\n";
        }
    }
}

int calculate_building_score(Hand<BuildingCard> &bchand) {
    int result = 0;
    std::for_each(bchand.begin(), bchand.end(), [&](BuildingCard& bc){
        result += bc.coins();
    });
    if(bchand.size() >= 8) result += 4;
    return result;
}

bool Game::calculate_score(Context& ctx) {
    auto poneptr = player_one().lock();
    auto ptwoptr = player_two().lock();
    if (poneptr && ptwoptr) {
        auto &datone = poneptr->get_player().get_data<GameData>();
        auto &dattwo = ptwoptr->get_player().get_data<GameData>();
        if(datone.built_buildings.size() >= 8 || dattwo.built_buildings.size() >= 8) {
            int scorepone = calculate_building_score(datone.built_buildings);
            int scoreptwo = calculate_building_score(dattwo.built_buildings);

            poneptr->get_socket() << "You " << ((scorepone > scoreptwo) ? "Won!" : "Lost. ") << std::to_string(scorepone);
            ptwoptr->get_socket() << "You " << ((scoreptwo > scorepone) ? "Won!" : "Lost. ") << std::to_string(scoreptwo);

            player_one().lock()->get_player().get_states().pop();
            player_two().lock()->get_player().get_states().pop();
            return true;
        }
    }
    return false;
}


