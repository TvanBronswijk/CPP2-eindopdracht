#pragma once
#include <memory>
#include <server/Server.hpp>
#include "Game.hpp"

class Context {
    std::unique_ptr<Game> game;
    server::Server* server;
};