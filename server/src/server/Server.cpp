#include "Server.hpp"

void Server::accept(std::function<void(Socket)> on_connect) {
	try {
		log("Server started.\nListening...\n");
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

std::unique_ptr<Server> Server::Builder::build(std::unique_ptr<ServerCallbackHandler> handler)
{
	auto server = std::make_unique<Server>(_server_name, _prompt, _port, std::move(handler));
	server->_threads.emplace_back(command_thread, std::ref(*server));
	return std::move(server);
}

void command_thread(Server& server)
{
	try {
		while (server.is_running()) {
			ClientCommand command = server.dequeue_command();
			if (auto clientInfo = command.get_client_info().lock()) {
				auto& client = clientInfo->get_socket();
				auto& player = clientInfo->get_player();
				try {
					if (!server._handler->on_command(command)) {
						client << player.get_name() << ": " << command.get_cmd() << "\r\n" << server.prompt();
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
		server << "consume_command crashed :(\n";
	}
}

void client_thread(Server& server, Socket socket)
{
	try {
		auto client_info = server._handler->on_client_register(std::move(socket));
		auto& client = client_info->get_socket();
		auto& player = client_info->get_player();
		client << "Welcome, " << player.get_name() << ", have fun playing our game!\r\n" << server.prompt();
		while (server.is_running()) {
			try {
				std::string cmd;
				if (client.readline([&cmd](std::string input) { cmd = input; })) {
					server << '[' << client.get_dotted_ip() << " (" << std::to_string(client.get_socket()) << ") " << player.get_name() << "] " << cmd << "\r\n";
					server._handler->on_client_input(client_info, cmd);
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
		server << "handle_client " << ex.what() << "\n";
	}
	catch (...) {
		server << "handle_client crashed\n";
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
