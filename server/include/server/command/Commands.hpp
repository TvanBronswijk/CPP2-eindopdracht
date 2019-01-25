#pragma once
#include <algorithm>
#include <unordered_map>
#include "Command.hpp"

namespace server::command {
    template<class T, class ... Args>
    class Commands {
    public:
        Commands() = default;
        Commands(char prefix, std::vector<Command<T, Args...>> commands) : _commands{} {
            std::transform(commands.begin(), commands.end(), std::inserter(_commands, _commands.end()), [prefix](const Command<T, Args...>& command){
                return std::make_pair(prefix + command.name(), std::move(command));
            });
        }
        const Command<T, Args...>* get_command(std::string cmd) const {
            auto it = _commands.find(cmd);
            if(it != _commands.end())
                return &(*it).second;
            return nullptr;
        }

        template<class Stream>Stream& help(Stream& os) {
            std::for_each(_commands.begin(), _commands.end(), [&](std::pair<std::string, Command<T, Args...>> pair) {
                os << pair.second;
            });
            return os;
        }
    private:
        std::unordered_map<std::string, Command<T, Args...>> _commands;
    };
}
