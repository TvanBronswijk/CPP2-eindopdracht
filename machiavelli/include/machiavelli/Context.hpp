#pragma once
#include <memory>
#include <server/Server.hpp>
#include "Game.hpp"

struct Context {
public:
    Context() = default;
    Context(std::unique_ptr<Game> game, server::Server& serv) : _game(std::move(game)), _server(&serv) {}
    Game& game() { return *_game; }
    const Game& game() const { return *_game; }
    server::Server& server() { return *_server; }
    const server::Server& server() const { return *_server; }

    server::Server& set_server(server::Server& serv) {
        _server = &serv;
        return server();
    }
    Game& new_game() {
        _game = std::make_unique<Game>();
        return game();
    }
private:
    std::unique_ptr<Game> _game;
    server::Server* _server;
};