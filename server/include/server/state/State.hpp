#pragma once
#include "server/command/ClientCommand.hpp"

class State {
public:
    State() = default;
    virtual ~State() = default;
    State(const State&) = delete;
    State(State&&) = delete;
    State& operator=(const State&) = delete;
    State& operator=(State&&) = delete;
    virtual void handle(ClientCommand) = 0;
};