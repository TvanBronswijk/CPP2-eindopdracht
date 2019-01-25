#pragma once
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <ostream>
#include "validation/Validators.hpp"

namespace server::command {
	template<class T, class ... Args>
	class Command {
	public:
		using Action = std::function<void(validate::StringArgs, T, Args...)>;
		Command(std::string command, std::string description, std::unordered_map<std::string, std::string> args, validate::Validator validator, Action action)
		: _command(command), _desc(command), _args(args), _validator(validator), _action(action) {}
		void try_action(validate::StringArgs strargs, T arg0, Args ... varargs) {
			if (_validator(strargs))
				_action(strargs, arg0, std::forward<Args>(varargs) ...);
			else
				throw validate::ValidationException("The arguments were invalid.");
		}

		std::string name() const { return _command; }
		std::string description() const { return _desc; }
		std::unordered_map<std::string, std::string> arguments() const { return _args; }
		friend std::ostream &operator<<(std::ostream&, const Command&);
	private:
		std::string _command;
		std::string _desc;
		std::unordered_map<std::string, std::string> _args;
		validate::Validator _validator;
		Action _action;
	};

	template<class T, class ... Args> std::ostream& operator<<(std::ostream& os, const Command<T, Args...>& command) {
		os << command._command << " - " << command._desc << " [" << command._args.size() << " argument(s)]\r\n";
		std::for_each(command._args.begin(), command._args.end(), [&](std::pair<std::string, std::string> pair) {
			os << '\t' << pair.first << " - " << pair.second << "\r\n";
		});
		return os;
	}
}

