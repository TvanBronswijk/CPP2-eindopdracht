#pragma once
#include <memory>
#include <stack>
#include "State.h"

class StateMachine {
public:
    StateMachine() = default;
    explicit StateMachine(std::unique_ptr<State> init) : _stack() { _stack.push(std::move(init)); }
    State& pop() { _stack.pop(); return peek(); }
    State& put(std::unique_ptr<State> state) { _stack.push(std::move(state)); return peek(); }
    State& peek() const { return *_stack.top(); }
    void handle(ClientCommand cmd) const { peek().handle(std::move(cmd)); }
private:
    std::stack<std::unique_ptr<State>> _stack;
};