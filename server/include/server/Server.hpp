#pragma once
#include "server/command/ClientCommand.hpp"
#include "server/connection/Socket.hpp"
#include "server/connection/Sync_queue.hpp"
#include "server/ClientRegistry.hpp"
#include "server/ServerCallbackHandler.hpp"
#include <memory>
#include <string>

namespace server {
	class Server {
	public:
		class Builder {
		public:
			Builder() : _server_name("server"), _prompt("server> "), _port(1080) {}
			Builder& with_name(std::string name) { _server_name = name; return *this; }
			Builder& with_prompt(std::string prompt) { _prompt = prompt; return *this; }
			Builder& at_port(int port) { _port = port; return *this; }
			std::unique_ptr<Server> build(std::unique_ptr<ServerCallbackHandler> handler);
		private:
			std::string _server_name;
			std::string _prompt;
			int _port;
		};

		Server(std::string name, std::string prompt, int port, std::unique_ptr<ServerCallbackHandler> handler)
				: _running(true), _socket(port), _server_name(name), _prompt(prompt), _tcp_port(port), _handler(std::move(handler)) {
			_handler->_server = this;
		};

		std::string name() const { return _server_name; }
		std::string prompt() const { return _prompt; }
		int port() const { return _tcp_port; }

		bool is_running() const { return _running; }
		void accept(std::function<void(connection::Socket)> on_connect);

		void enqueue_command(command::ClientCommand command) { _queue.put(command); }
		command::ClientCommand dequeue_command() { return command::ClientCommand{ _queue.get() }; }

		ClientRegistry& registry() { return _registry; }
		const ClientRegistry& registry() const { return _registry; }

		void announce(char character);
		void announce(std::string str);
		void log(char character) const { std::cerr << character; }
		void log(std::string message) const { std::cerr << message; }
		const Server& operator << (char character) const { log(character); return *this; }
		const Server& operator << (std::string message) const { log(message); return *this; }
	private:
		std::string _server_name;
		std::string _prompt;
		int _tcp_port;

		bool _running;
		connection::ServerSocket _socket;
		ClientRegistry _registry;
		connection::Sync_queue<command::ClientCommand> _queue;
		std::unique_ptr<ServerCallbackHandler> _handler;
		std::thread _command_thread;

		friend void client_thread(Server&, connection::Socket);
		friend void command_thread(Server&);
		friend void close_server(Server&);
	};

	Server::Builder create_server();
}

