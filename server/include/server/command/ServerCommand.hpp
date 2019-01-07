#pragma once
#include <vector>
#include <string>
#include <functional>
#include "validation/Validators.hpp"

template<class T, class ... Args>
class ServerCommand {
public:
	using Action = std::function<void(validate::StringArgs, T, Args...)>;
	ServerCommand(std::string command, validate::Validator validator, Action action) : _command(command), _validator(validator), _action(action) {}
	void try_action(validate::StringArgs strargs, T arg0, Args ... varargs) {
		if (_validator(strargs))
			_action(strargs, arg0, std::forward<Args>(varargs) ...);
		else
			throw validate::ValidationException("The arguments were invalid.");
	}
private:
	std::string _command;
	validate::Validator _validator;
	Action _action;
};
