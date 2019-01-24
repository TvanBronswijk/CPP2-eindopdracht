#pragma once
#include <vector>
#include <string>
#include <functional>
#include "validation/Validators.hpp"

namespace server::command {
	template<class T, class ... Args>
	class Command {
	public:
		using Action = std::function<void(validate::StringArgs, T, Args...)>;
		Command(std::string command, validate::Validator validator, Action action) : _desc(command), _validator(validator), _action(action) {}
		void try_action(validate::StringArgs strargs, T arg0, Args ... varargs) {
			if (_validator(strargs))
				_action(strargs, arg0, std::forward<Args>(varargs) ...);
			else
				throw validate::ValidationException("The arguments were invalid.");
		}

		std::string get_description() {
			return _desc;
		}
	private:
		std::string _desc;
		validate::Validator _validator;
		Action _action;
	};
}

