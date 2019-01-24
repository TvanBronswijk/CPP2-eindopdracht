#pragma once
#include <string>
#include "server/connection/Socket.hpp"
#include "ClientInfo.hpp"
#include "server/input/Command.hpp"

namespace server {
	class Server;
	class ServerCallbackHandler {
		friend class Server;
	public:
		enum class Event { quit, server_stop, command, text };
		ServerCallbackHandler() : _server(nullptr) {};
		explicit ServerCallbackHandler(Server& server) : _server(&server) {};
		virtual ~ServerCallbackHandler() = default;
		ServerCallbackHandler(const ServerCallbackHandler&) = delete;
		ServerCallbackHandler(ServerCallbackHandler&&) = delete;
		ServerCallbackHandler& operator=(const ServerCallbackHandler&) = delete;
		ServerCallbackHandler& operator=(ServerCallbackHandler&&) = delete;

		virtual bool on_command(input::Command) const = 0;
		virtual std::shared_ptr<ClientInfo> on_client_register(connection::Socket) const = 0;
		virtual Event on_client_input(std::weak_ptr<ClientInfo>, std::string) const = 0;
	protected:
		Server* _server;
	};

}
