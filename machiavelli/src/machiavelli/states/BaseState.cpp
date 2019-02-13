#include "machiavelli/states/BaseState.hpp"
#include <istream>
#include <iterator>

using namespace server;
using namespace server::command;
using namespace server::command::validate;
using namespace server::connection;
using namespace server::player;


BaseState::BaseState(Context& ctx, std::vector<command::Command<Player&, Socket&, Context&>> commands) : _ctx{&ctx} {
    commands.emplace_back(
        "help",
        "Get description of all commands.",
        [](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
        [&](StringArgs args, Player& player, Socket& socket, Context& context) {
            socket << "Commands are prefixed with a '!' (including numbered options).\r\n";
            _cmds.help(socket);
        });
    _cmds = { '!', commands };
}

bool BaseState::handle(input::Command cmd) {
    if(auto clientinfoptr = cmd.get_client_info().lock()) {
        auto& player = clientinfoptr->get_player();
        auto& socket = clientinfoptr->get_socket();

        std::istringstream iss(cmd.get_cmd());
        std::vector<std::string> words{ std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{} };
        if (auto command = _cmds.get_command(words[0])) {
            try {
                std::vector<std::string> args(words.begin() + 1, words.end());
                command->try_action(args, player, socket, *_ctx);
                return true;
            }
            catch (std::exception e) {
                socket << "Command Failed ERROR: " << e.what() << "\r\n";
                return false;
            }
        }
        else {
            return false;
        }
    }
    return false;
}