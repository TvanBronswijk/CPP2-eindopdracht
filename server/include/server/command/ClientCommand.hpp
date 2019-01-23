#pragma once
//
//  ClientCommand.h
//  socketexample
//
//  Created by Bob Polis on 27/11/14.
//  Modified by Jeroen de Haas on 23/11/2016.
//  Copyright (c) 2016 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#include <string>
#include <memory>

namespace server { class ClientInfo; }
namespace server::command {
	class ClientCommand {
	public:
		ClientCommand(const std::string& command_text, std::weak_ptr<ClientInfo> client_info)
				: cmd{ command_text }, info{ client_info } {}
		std::string get_cmd() const { return cmd; }
		std::weak_ptr<ClientInfo> get_client_info() const { return info; }
	private:
		std::string cmd;
		std::weak_ptr<ClientInfo> info;
	};

}
