#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>  
#include <crtdbg.h>
#endif

#include "machiavelli/InputHandler.hpp"
#include <_server.hpp>
using namespace server;

void run_game() 
{
	std::unique_ptr<Context> ctx = std::make_unique<Context>();
	auto input = std::make_unique<InputHandler>(*ctx);
	auto server = create_server()
		.with_name("Machiavelli")
		.with_prompt("machiavelli> ")
		.at_port(1080)
		.build(std::move(input));
	ctx->set_server(*server);

	server->accept([&server](connection::Socket sock) {
		(*server) << "A new connection from " << sock.get_dotted_ip() << " has been established.\n";
		server->registry().register_client(*server, std::move(sock));
	});
}

int main(int argc, const char * argv[])
{
#ifdef _MSC_VER
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	run_game();
	return 0;
}


