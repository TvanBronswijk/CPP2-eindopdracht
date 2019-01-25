#include "server/Server.hpp"
#include <algorithm>
using namespace server::connection;

namespace server {
	void Server::accept(std::function<void(Socket)> on_connect) {
		try {
			log("Server started [" + _socket.get_dotted_ip() + ":" + std::to_string(_tcp_port) + "(" + std::to_string(_socket.get_socket()) + ")].\n");
			log("Listening...\n");
			while (is_running()) {
				_socket.accept(on_connect);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
		catch (const std::exception& ex) {
			(*this) << ex.what() << '\n';
		}
		catch (...) {
			(*this) << "An error has occured.\n";
		}
	}

	void Server::announce(char character) {
		std::for_each(_registry.begin(), _registry.end(), [this, character](std::weak_ptr<ClientInfo> client) { if (auto clientinfo = client.lock()) clientinfo->get_socket() << "\r" << character << '\n' << this->prompt(); });
	}

	void Server::announce(std::string message) {
		std::for_each(_registry.begin(), _registry.end(), [this, message](std::weak_ptr<ClientInfo> client) { if (auto clientinfo = client.lock()) clientinfo->get_socket() << "\r" << message << '\n' << this->prompt(); });
	}

	void close_server(Server& server) {
		server._command_thread.join();
		for (auto &t : server._registry.threads) {
			t.join();
		}
	}
}

