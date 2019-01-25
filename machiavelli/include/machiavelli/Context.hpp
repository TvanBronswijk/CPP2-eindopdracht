#pragma once
#include <memory>
#include <server/Server.hpp>
#include "Game.hpp"

struct Context {
public:
    Context(std::unique_ptr<Game> game, server::Server& serv) : _game(std::move(game)), _server(&serv) {}
    Game& game() { return *_game; }
    const Game& game() const { return *_game; }
    server::Server& server() { return *_server; }
    const server::Server& server() const { return *_server; }
private:
    std::unique_ptr<Game> _game;
    server::Server* _server;
};