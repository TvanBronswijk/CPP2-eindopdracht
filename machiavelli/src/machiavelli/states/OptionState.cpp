#include <utility>

#include <machiavelli/states/OptionState.hpp>

using namespace server;
using namespace server::command;
using namespace server::command::options;
using namespace server::command::validate;
using namespace server::connection;
using namespace server::player;

OptionState::OptionState(Context &ctx, OptionHandler handler)
: BaseState(ctx, {}), _handler(std::move(handler)) {}

bool OptionState::handle(server::input::Command cmd) {
    if(auto ptr = cmd.get_client_info().lock()) {
        try {
            if(cmd.get_cmd().length() >= 2) {
                std::string num = cmd.get_cmd();
                num.erase(std::remove(num.begin(), num.end(), '!'), num.end());
                int i = std::stoi(num);
                if(_handler.choose(i)) {
                    ptr->get_player().get_states().pop();
                    return true;
                }
            }
        }catch(...) {
            return BaseState::handle(cmd);
        }
    }
    return false;
}
