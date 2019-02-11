#pragma once
#include <functional>
#include <unordered_map>
#include <server/ClientInfo.hpp>
#include "machiavelli/cards/Hand.hpp"
#include "machiavelli/cards/CharacterCard.hpp"
#include "machiavelli/cards/BuildingCard.hpp"

class Context;
class Game {
    using CharacterAction = std::function<void(server::player::Player&, server::connection::Socket&, Context&)>;
public:
    Game() = default;
    Game(std::weak_ptr<server::ClientInfo> player1, std::weak_ptr<server::ClientInfo> player2);

    void start(bool);
    void next_turn();
    void calculate_score();

    std::weak_ptr<server::ClientInfo> player_one() { return _player1; }
    std::weak_ptr<server::ClientInfo> player_two() { return _player2; }

    std::weak_ptr<server::ClientInfo> other_player(server::player::Player &player) {
        auto poneptr = player_one().lock();
        auto ptwoptr = player_two().lock();
        if(poneptr && ptwoptr) {
            if(poneptr->get_player().get_name() == player.get_name())
                return player_one();
            else
                return player_two();
        } else {
            //TODO
        }
    }

    size_t get_current_turn() { return _curr_turn; }
    CharacterCard get_card(size_t index) { return _ccards.at(index); }
    CharacterAction get_action(size_t index) { return _cactions.at(index); }

    Hand<CharacterCard> character_cards;
    Hand<BuildingCard> deck;

    std::unordered_map<size_t, std::weak_ptr<server::ClientInfo>> game_order;
private:
    size_t _curr_turn;
    std::weak_ptr<server::ClientInfo> _player1;
    std::weak_ptr<server::ClientInfo> _player2;
    std::unordered_map<size_t, CharacterCard> _ccards;
    std::unordered_map<size_t, CharacterAction> _cactions;
};