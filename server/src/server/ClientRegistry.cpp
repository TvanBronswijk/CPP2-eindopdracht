#include "server/ClientRegistry.hpp"
#include "server/Server.hpp"

using namespace server::connection;
namespace server {
    void client_thread(Server& server, Socket socket)
    {
        try {
            auto client_info = server._handler->on_client_register(std::move(socket));
            server._registry.clients.push_back(client_info);
            auto& client = client_info->get_socket();
            auto& player = client_info->get_player();
            client << server.prompt();

            while (server.is_running()) {
                try {
                    std::string cmd;
                    if (client.readline([&cmd](std::string input) { cmd = input; }) && !cmd.empty()) {
                        server << '[' << client.get_dotted_ip() << " (" << std::to_string(client.get_socket()) << ") " << player.get_name() << "] " << cmd << "\r\n";

                        bool quit = false;
                        switch (server._handler->on_client_input(client_info, cmd)) {
                            case ServerCallbackHandler::Event::quit:
                                quit = true;
                                break;
                            case ServerCallbackHandler::Event::server_stop:
                                server._running = false;
                                server.enqueue_command(input::Command{ "___SIG_KILL___", client_info });
                                break;
                            case ServerCallbackHandler::Event::text:
                                client << server.prompt();
                                break;
                            case ServerCallbackHandler::Event::command:
                            default:
                                break;
                        }
                        if (quit) break;
                    }
                }
                catch (const std::exception& ex) {
                    client << "ERROR: " << ex.what() << "\r\n";
                }
                catch (...) {
                    client << "ERROR: something went wrong during handling of your request. Sorry!\r\n";
                }
            }
        }
        catch (std::exception &ex) {
            server << "Client thread exception: " << ex.what() << "\n";
        }
        catch (...) {
            server << "The client thread crashed.\n";
        }
    }

    void ClientRegistry::register_client(Server& server, Socket socket) {
        threads.emplace_back(client_thread, std::ref(server), std::move(socket));
    }
}
