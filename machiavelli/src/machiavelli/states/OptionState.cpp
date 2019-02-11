#include <utility>

#include <machiavelli/states/OptionState.hpp>

using namespace server;
using namespace server::command;
using namespace server::command::options;
using namespace server::command::validate;
using namespace server::connection;
using namespace server::player;

OptionState::OptionState(Context &ctx, OptionHandler handler) : BaseState(ctx, {
        //TODO
}), _handler(std::move(handler)) {}

bool OptionState::handle(server::input::Command cmd) {
    if(auto ptr = cmd.get_client_info().lock()) {
        try {
            int i = std::stoi(cmd.get_cmd());
            if(_handler.choose(i)) {
                ptr->get_player().get_states().pop();
                return true;
            }
        }catch(...) {
            ptr->get_socket() << "Something went wrong.\r\n";
        }
    }
    return false;
}
