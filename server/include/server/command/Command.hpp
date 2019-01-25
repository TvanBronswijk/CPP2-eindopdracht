#pragma once
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <algorithm>
#include <ostream>
#include "validation/Validators.hpp"

namespace server::command {
	template<class T, class ... Args>
	class Command {
	public:
		using Action = std::function<void(validate::StringArgs, T, Args...)>;
        Command(std::string command, std::string description, validate::Validator validator, Action action)
                : _command(command), _desc(description), _validator(validator), _action(action) {}
		Command(std::string command, std::string description, std::map<std::string, std::string> args, validate::Validator validator, Action action)
		: _command(command), _desc(description), _args(args), _validator(validator), _action(action) {}
		void try_action(validate::StringArgs strargs, T arg0, Args ... varargs) const {
			if (_validator(strargs))
				_action(strargs, arg0, std::forward<Args>(varargs) ...);
			else
				throw validate::ValidationException("The arguments were invalid.");
		}

		std::string name() const { return _command; }
		std::string description() const { return _desc; }
		std::map<std::string, std::string> arguments() const { return _args; }
		template<class Stream, class fT, class ... fArgs> friend Stream& operator<<(Stream&, const Command<fT, fArgs...>&);
	private:
		std::string _command;
		std::string _desc;
		std::map<std::string, std::string> _args;
		validate::Validator _validator;
		Action _action;
	};

	template<class Stream, class T, class ... Args> Stream& operator<<(Stream& os, const Command<T, Args...>& command) {
		os << command._command << " - " << command._desc << " [" << command._args.size() << " argument(s)]\r\n";
		std::for_each(command._args.begin(), command._args.end(), [&](std::pair<std::string, std::string> pair) {
			os << '\t' << pair.first << " - " << pair.second << "\r\n";
		});
		return os;
	}
}

