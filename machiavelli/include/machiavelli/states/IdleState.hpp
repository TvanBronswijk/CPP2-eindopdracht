#pragma once
#include "BaseState.hpp"

class IdleState : public BaseState {
public:
    explicit IdleState(Context& ctx);
	~IdleState() override = default;
};
