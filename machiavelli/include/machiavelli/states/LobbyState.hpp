#pragma once
#include "BaseState.hpp"

class LobbyState : public BaseState {
public:
    LobbyState(Context& ctx) : BaseState{ctx, {}} {}
};