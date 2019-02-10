#pragma once
#include <server/player/Player.hpp>
#include <server/player/state/State.hpp>
#include <server/connection/Socket.hpp>
#include <server/_command.hpp>
#include "machiavelli/Context.hpp"

class BaseState : public server::player::state::State {
public:
    BaseState(Context&, std::vector<server::command::Command<server::player::Player&, server::connection::Socket&, Context&>>);
    ~BaseState() override = default;
    BaseState(const BaseState&) = delete;
    BaseState(BaseState&&) = delete;
    BaseState& operator=(const BaseState&) = delete;
    BaseState& operator=(BaseState&&) = delete;
    bool handle(server::input::Command) override;
protected:
    server::command::Commands<server::player::Player&, server::connection::Socket&, Context&> _cmds;
    Context* _ctx;
};