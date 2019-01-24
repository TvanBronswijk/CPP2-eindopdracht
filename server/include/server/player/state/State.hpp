#pragma once
#include "server/input/Command.hpp"

namespace server::player::state {
    class State {
    public:
        State() = default;
        virtual ~State() = default;
        State(const State&) = delete;
        State(State&&) = delete;
        State& operator=(const State&) = delete;
        State& operator=(State&&) = delete;
        virtual void handle(input::Command) = 0;
    };
}
