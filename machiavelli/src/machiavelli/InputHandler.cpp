#include <machiavelli/InputHandler.hpp>

using namespace server;
using namespace server::connection;

bool InputHandler::on_command(input::Command) const {
    return false;
}

std::shared_ptr<ClientInfo> InputHandler::on_client_register(Socket) const {
    return std::shared_ptr<ClientInfo>();
}

ServerCallbackHandler::Event InputHandler::on_client_input(std::weak_ptr<ClientInfo>, std::string) const {
    return ServerCallbackHandler::Event::quit;
}
