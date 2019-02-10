#pragma once
#include <unordered_map>
#include <server/ClientInfo.hpp>
#include "machiavelli/cards/Hand.hpp"
#include "machiavelli/cards/CharacterCard.hpp"
#include "machiavelli/cards/BuildingCard.hpp"


class Game {
public:
    Game() = default;
    Game(std::weak_ptr<server::ClientInfo> player1, std::weak_ptr<server::ClientInfo> player2);

    void start(bool);
    void next_turn();
    void calculate_score();

    Hand<CharacterCard> character_cards;
    Hand<BuildingCard> deck;
private:
    size_t _curr_turn;
    std::weak_ptr<server::ClientInfo> _player1;
    std::weak_ptr<server::ClientInfo> _player2;
    std::unordered_map<size_t, CharacterCard> _ccards;
};