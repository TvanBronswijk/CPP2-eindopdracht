#include <machiavelli/Game.hpp>

Game::Game(std::weak_ptr<server::ClientInfo> player1, std::weak_ptr<server::ClientInfo> player2)
: _player1(std::move(player1)), _player2(std::move(player2)), _curr_turn(1){}

void Game::start(bool random) {

}

void Game::next_turn() {

}

void Game::calculate_score() {

}
