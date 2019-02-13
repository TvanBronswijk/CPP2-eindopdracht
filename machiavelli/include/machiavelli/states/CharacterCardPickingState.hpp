#pragma once
#include "BaseState.hpp"

class CharacterCardPickingState : public BaseState {
public:
    explicit CharacterCardPickingState(Context& ctx, bool pick);
	~CharacterCardPickingState() override = default;
    bool handle(server::input::Command) override;
    bool pick_card(std::weak_ptr<server::ClientInfo>, int);
private:
    bool _pick;
};