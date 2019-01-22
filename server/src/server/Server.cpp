#include "server/Server.hpp"
#include <algorithm>

void command_thread(Server& server)
{
	try {
		while (server.is_running()) {
			ClientCommand command = server.dequeue_command();
			if (auto clientInfo = command.get_client_info().lock()) {
				auto& client = clientInfo->get_socket();
				auto& player = clientInfo->get_player();
				try {
					if (server._handler->on_command(command)) {
						client << server.prompt();
					}
					else {
						server.announce(player.get_name() + ": " + command.get_cmd() + "\r\n");
					}
				}
				catch (const std::exception& ex) {
					server << "*** exception in consumer thread for player " << player.get_name() << ": " << ex.what() << '\n';
					if (client.is_open()) {
						client.write("Sorry, something went wrong during handling of your request.\r\n");
					}
				}
				catch (...) {
					server << "*** exception in consumer thread for player " << player.get_name() << '\n';
					if (client.is_open()) {
						client.write("Sorry, something went wrong during handling of your request.\r\n");
					}
				}
			}
		}
	}
	catch (...) {
		server << "The command thread crashed.\n";
	}
}

void client_thread(Server& server, Socket socket)
{
	try {
		auto client_info = server._handler->on_client_register(std::move(socket));
		server._clients.push_back(client_info);
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
						server.stop();
						server.enqueue_command(ClientCommand{ cmd, client_info });
						break;
					default:
						break;
					}
					if (quit) break;
				};
			}
			catch (const std::exception& ex) {
				client << "ERROR: " << ex.what() << "\r\n";
			}
			catch (...) {
				client.write("ERROR: something went wrong during handling of your request. Sorry!\r\n");
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

void close_server(Server& server) {
	for (auto &t : server._threads) {
		t.join();
	}
}

Server::Builder create_server() {
	return Server::Builder();
}

void Server::accept(std::function<void(Socket)> on_connect) {
	try {
		log("Server started @" + _socket.get_dotted_ip() + ":" + std::to_string(_tcp_port) + "(" + std::to_string(_socket.get_socket()) + ").\n");
		log("Listening...\n");
		while (_running) {
			_socket.accept(on_connect);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	catch (const std::exception& ex) {
		(*this) << ex.what() << '\n';
	}
	catch (...) {
		log("An error has occured.\n");
	}
}

void Server::add_client(Socket socket) {
	_threads.emplace_back(client_thread, std::ref(*this), std::move(socket));
}

void Server::announce(std::string message) {
	std::for_each(_clients.begin(), _clients.end(), [this, message](std::weak_ptr<ClientInfo> client) { if (auto clientinfo = client.lock()) clientinfo->get_socket() << "\r\n" << message << this->prompt(); });
}

std::unique_ptr<Server> Server::Builder::build(std::unique_ptr<ServerCallbackHandler> handler)
{
	auto server = std::make_unique<Server>(_server_name, _prompt, _port, std::move(handler));
	server->_threads.emplace_back(command_thread, std::ref(*server));
	return std::move(server);
}
