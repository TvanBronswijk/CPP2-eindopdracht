#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#include "server.hpp"

bool consume_command(Server& server, ClientCommand command)
{
	return false;
}

bool handle_client_input(Server& server, std::weak_ptr<ClientInfo> client, std::string input) {
	if (input == "quit") {
		return true;
	}
	else if (input == "quit_server") {
		server.stop();
	}
	else {
		server.enqueue_command(ClientCommand{ input, client });
	}
}

int main(int argc, const char * argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	auto server = create_server(consume_command, handle_client_input)
		.with_name("Machiavelli")
		.with_prompt("machiavelli> ")
		.at_port(1080)
		.build();

	server->accept([&server](Socket sock) {
		(*server) << "A new connection from " << sock.get_dotted_ip() << " has been established.\n";
		server->add_client(std::move(sock));
	});

	close_server(*server);
	return 0;
}


