#pragma once
#include <string>
#include "server/connection/Socket.hpp"
#include "server/connection/ClientInfo.hpp"
#include "server/command/ClientCommand.hpp"

class ServerCallbackHandler {
	friend class Server;
public:
	enum class Event { quit, server_stop, command, text };
	ServerCallbackHandler() = default;
	~ServerCallbackHandler() = default;
	//TODO copy en move semantics
	virtual bool on_command(ClientCommand) = 0;
	virtual std::shared_ptr<ClientInfo> on_client_register(Socket) const = 0;
	virtual Event on_client_input(std::weak_ptr<ClientInfo>, std::string) const = 0;
protected:
	Server* _server;
};
