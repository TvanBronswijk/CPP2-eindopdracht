#pragma once
#include <memory>
#include <string>
#include "server\Sync_queue.hpp"
#include "server\ClientCommand.hpp"
#include "server\Socket.hpp"

class Server {
public:
	Server(std::string name, std::string prompt, int port) : _running(true), _server_name(name), _prompt(prompt), _tcp_port(port) {};
	int run();
private:
	std::string _server_name;
	std::string _prompt;
	int _tcp_port;

	bool _running;

	Sync_queue<ClientCommand> _queue;
	friend void consume_command(Server& server);
	friend std::shared_ptr<ClientInfo> init_client_session(Server& server, Socket client);
	friend void handle_client(Server& server, Socket client);
};
