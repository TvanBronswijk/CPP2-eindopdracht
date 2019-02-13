#pragma once
#include "BaseState.hpp"

class ActionState : public BaseState {
public:
    explicit ActionState(Context& ctx);
private:
    bool _take_gold_or_card;
    bool _built_building;
    int _built_buildings;
    bool _character_action;
};