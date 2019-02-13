#pragma once
#include "BaseState.hpp"
#include <server/command/_options.hpp>

class OptionState : public BaseState {
public:
    explicit OptionState(Context& ctx, server::command::options::OptionHandler handler);
	~OptionState() override = default;
    bool handle(server::input::Command) override;
private:
    server::command::options::OptionHandler _handler;
};
