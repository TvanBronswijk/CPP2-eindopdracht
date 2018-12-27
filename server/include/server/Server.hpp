#pragma once
#include "server/command/ClientCommand.hpp"
#include "server/connection/Socket.hpp"
#include "server/connection/Sync_queue.hpp"
#include <memory>
#include <string>

class Server;
using CommandHandler = std::function<bool(Server&, ClientCommand)>;
using ClientHandler = std::function<void(Server&, std::weak_ptr<ClientInfo> client, std::string input)>;

class Server {
public:
	class Builder {
	public:
		Builder(CommandHandler handle_command, ClientHandler handle_connection)
			: _handle_command(handle_command), _handle_connection(handle_connection), _server_name("server"), _prompt("server> "), _port(1080) {}
		Builder& with_name(std::string name) { _server_name = name; return *this; }
		Builder& with_prompt(std::string prompt) { _prompt = prompt; return *this; }
		Builder& at_port(int port) { _port = port; return *this; }
		std::unique_ptr<Server> build();
	private:
		CommandHandler _handle_command;
		ClientHandler _handle_connection;
		std::string _server_name;
		std::string _prompt;
		int _port;
	};

	Server(std::string name, std::string prompt, int port, ClientHandler handle_connection)
		: _running(true), _socket(port), _server_name(name), _prompt(prompt), _tcp_port(port), _handle_connection(handle_connection) {};

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
	Sync_queue<ClientCommand> _queue;
	ClientHandler _handle_connection;

	friend void close_server(Server&);
};

void command_thread(Server&, CommandHandler);
void client_thread(Server&, Socket, ClientHandler);

Server::Builder create_server(CommandHandler, ClientHandler);
void close_server(Server&);


