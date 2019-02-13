#pragma once
#include "BaseState.hpp"

class LobbyState : public BaseState {
public:
    explicit LobbyState(Context& ctx);
	~LobbyState() override = default;
};
