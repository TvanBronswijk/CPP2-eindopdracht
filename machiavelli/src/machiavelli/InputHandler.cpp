#include "machiavelli/InputHandler.hpp"
#include "machiavelli/player/GameData.hpp"
#include "machiavelli/states/LobbyState.hpp"

using namespace server;
using namespace server::connection;
using namespace server::player;

bool InputHandler::on_command(input::Command cmd) const {
    if(auto clientptr = cmd.get_client_info().lock()) {
        auto& socket = clientptr->get_socket();
        auto& player = clientptr->get_player();
        if(player.get_states().handle(std::move(cmd))) {
            return true;
        }else {
            socket << "try typing '!help'.\r\n";
            return false;
        }
    }
    return false;
}

std::shared_ptr<ClientInfo> InputHandler::on_client_register(Socket socket) const {
    socket << "Welcome to " << _server->name() << "! To exit, type '/quit'.\n";
    socket << "What is your name? ";
    std::string name{};
    while (!socket.readline([&name](std::string input) { name = input; }));

    return std::make_shared<ClientInfo>(std::move(socket),
            player::Player{ name, std::make_unique<GameData>(), std::make_unique<LobbyState>(*_ctx) });
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
