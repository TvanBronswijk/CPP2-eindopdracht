#pragma once
#include <memory>
#include <stack>
#include "State.hpp"

namespace server::player::state {

    struct StateException : public std::exception
    {
        const char* what() const throw() override
        {
            return "Your current state is invalid.";
        }
    };

    class StateMachine {
    public:
        StateMachine() = default;
        explicit StateMachine(std::unique_ptr<State> init) : _stack() { _stack.push(std::move(init)); }
        State& pop() { if(_stack.empty()) throw StateException(); _stack.pop(); return peek(); }
        State& put(std::unique_ptr<State> state) { _stack.push(std::move(state)); return peek(); }
        State& peek() const { if(_stack.empty()) throw StateException(); return *_stack.top(); }
        bool handle(input::Command cmd) const { return peek().handle(std::move(cmd)); }
    private:
        std::stack<std::unique_ptr<State>> _stack;
    };
}
