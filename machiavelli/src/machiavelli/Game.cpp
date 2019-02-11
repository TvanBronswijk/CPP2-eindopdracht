#include <machiavelli/Game.hpp>
#include <machiavelli/Context.hpp>
#include <machiavelli/player/GameData.hpp>
#include <machiavelli/states/IdleState.hpp>
#include <machiavelli/states/OptionState.hpp>
#include <machiavelli/states/ActionState.hpp>
#include <server/command/_options.hpp>
#include <util.hpp>
#include <machiavelli/io/IO.hpp>

using namespace server;
using namespace server::command::options;
using namespace server::connection;
using namespace server::player;

Game::Game(std::weak_ptr<server::ClientInfo> player1, std::weak_ptr<server::ClientInfo> player2)
        : _player1(std::move(player1)), _player2(std::move(player2)), _curr_turn(0) {
    std::vector<CharacterCard> ccs = io::parse<CharacterCard>("layouts/karakterkaarten.csv");
    std::for_each(ccs.begin(), ccs.end(), [&](CharacterCard& cc){
        _ccards[cc.order()] = cc;
    });

    _cactions = {
            {1, [](Player &player, Socket &socket, Context &context) { //moordenaar
                auto &data = player.get_data<GameData>();
                OptionHandler handler{
                        {"Dief", "Magier", "Koning", "Prediker", "Koopman", "Bouwman", "Condotierre"},
                        [&](int i){
                            if(i >= 0 && i < 7) {
                                context.game().game_order.erase(i+2);
                                return true;
                            }
                            return false;
                        }
                };
                player.get_states().put(std::make_unique<OptionState>(context, handler));
            }},
            {2, [](Player &player, Socket &socket, Context &context) { //dief
                auto &data = player.get_data<GameData>();
                OptionHandler handler{
                        {"Magier", "Koning", "Prediker", "Koopman", "Bouwman", "Condotierre"},
                        [&](int i){
                            if(i >= 0 && i < 6) {
                                auto other = context.game().game_order.at(i+3);
                                if(auto otherptr = other.lock()) {
                                    auto& otherdata = otherptr->get_player().get_data<GameData>();
                                    player.get_data<GameData>().gold_coins += otherdata.gold_coins;
                                    otherdata.gold_coins = 0;
                                }else {
                                    socket << "could not steal from this person";
                                }
                                return true;
                            }
                            return false;
                        }
                };
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

void Game::start(Context& ctx, bool random) {
    character_cards = Hand<CharacterCard>();
    std::for_each(_ccards.begin(), _ccards.end(), [&](auto& pair){
       character_cards.add(pair.second);
    });

    auto poneptr = player_one().lock();
    auto ptwoptr = player_two().lock();
    if(poneptr && ptwoptr) {
        auto& datone = poneptr->get_player().get_data<GameData>();
        auto& dattwo = ptwoptr->get_player().get_data<GameData>();
        if(random){
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
        }else{
            //TODO
        }
    } else {
        throw -1;
    }
}

void Game::next_turn(Context& ctx) {
    std::weak_ptr<ClientInfo> player;
    do{
        player = game_order.at(++_curr_turn);
    }while(!player.lock() && _curr_turn <= 8);
    if(_curr_turn > 8){
        start(ctx, false);
    }else {
        if(auto playerptr = player.lock()) {
            playerptr->get_player().get_states().put(std::make_unique<ActionState>(ctx));
        }
    }
}

void Game::calculate_score() {
    //TODO
}
