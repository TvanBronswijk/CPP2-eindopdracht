#include "machiavelli/InputHandler.hpp"
#include "machiavelli/player/GameData.hpp"

using namespace server;
using namespace server::connection;

bool InputHandler::on_command(input::Command cmd) const {
    if(auto clientptr = cmd.get_client_info().lock()) {
        auto& player = clientptr->get_player();
        return player.get_states().handle(std::move(cmd));
    }
    return false;
}

std::shared_ptr<ClientInfo> InputHandler::on_client_register(Socket socket) const {
    socket << "Welcome to " << _server->name() << "! To exit, type '/quit'.\n";
    socket << "What is your name? ";
    std::string name{};
    while (!socket.readline([&name](std::string input) { name = input; }));

    auto data = std::make_unique<GameData>();
    return std::make_shared<ClientInfo>(std::move(socket), player::Player{ name, std::move(data) });
}

ServerCallbackHandler::Event InputHandler::on_client_input(std::weak_ptr<ClientInfo> client, std::string input) const {
    if (auto clientptr = client.lock()) {
        auto& socket = clientptr->get_socket();
        auto& player = clientptr->get_player();
        if (input[0] == '!') {
            _server->enqueue_command(input::Command{input, client});
            return Event::command;
        } else if (input == "/quit") {
            return Event::quit;
        } else if (input == "/stop-server") {
            return Event::server_stop;
        } else {
            _server->announce(player.get_name() + ": " + input);
            return Event::text;
        }
    } else {
        return Event::quit;
    }
}
