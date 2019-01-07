#pragma once
#include "server/command/ClientCommand.hpp"
#include "server/connection/Socket.hpp"
#include "server/connection/Sync_queue.hpp"
#include "server/ServerCallbackHandler.hpp"
#include <memory>
#include <string>

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

	std::string name() { return _server_name; }
	std::string prompt() { return _prompt; }
	int port() { return _tcp_port; }

	bool is_running() { return _running; }
	bool is_stopped() { return !_running; }
	void stop() { _running = false; }

	void accept(std::function<void(Socket)> on_connect);
	void add_client(Socket socket);

	void enqueue_command(ClientCommand command) { _queue.put(command); }
	ClientCommand dequeue_command() { return ClientCommand{ _queue.get() }; }

	void announce(std::string str);

	void log(char character) { std::cerr << character; }
	void log(std::string message) { std::cerr << message; }
	Server& operator << (char character) { log(character); return *this; }
	Server& operator << (std::string message) { log(message); return *this; }
private:
	std::string _server_name;
	std::string _prompt;
	int _tcp_port;

	bool _running;
	ServerSocket _socket;
	std::vector<std::thread> _threads;
	std::vector<std::weak_ptr<ClientInfo>> _clients;
	Sync_queue<ClientCommand> _queue;
	std::unique_ptr<ServerCallbackHandler> _handler;

	friend void command_thread(Server&);
	friend void client_thread(Server&, Socket);
	friend void close_server(Server&);
};

Server::Builder create_server();
