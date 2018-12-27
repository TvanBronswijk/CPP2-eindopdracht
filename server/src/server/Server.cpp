#include "Server.hpp"

Server::Builder create_server(std::function<void(Server&)> handle_command, std::function<void(Server&, Socket)> handle_connection) {
	return Server::Builder(handle_command, handle_connection);
}

void close_server(Server& server) {
	for (auto &t : server._threads) {
		t.join();
	}
}

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
		log("An error has occured. :(\n");
	}
}
