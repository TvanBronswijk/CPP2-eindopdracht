#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#include "server.hpp"

void consume_command(Server& server)
{
	try {
		while (server.is_running()) {
			ClientCommand command = server.dequeue_command();
			if (auto clientInfo = command.get_client_info().lock()) {
				auto& client = clientInfo->get_socket();
				auto& player = clientInfo->get_player();
				try {
					// TODO handle command here
					client << player.get_name() << ", you wrote: '" << command.get_cmd() << "', but I'll ignore that for now.\r\n" << server.prompt();
				}
				catch (const std::exception& ex) {
					server << "*** exception in consumer thread for player " << player.get_name() << ": " << ex.what() << '\n';
					if (client.is_open()) {
						client.write("Sorry, something went wrong during handling of your request.\r\n");
					}
				}
				catch (...) {
					server << "*** exception in consumer thread for player " << player.get_name() << '\n';
					if (client.is_open()) {
						client.write("Sorry, something went wrong during handling of your request.\r\n");
					}
				}
			}
		}
	}
	catch (...) {
		server << "consume_command crashed :(\n";
	}
}

std::shared_ptr<ClientInfo> init_client_session(Server& server, Socket client) {
	client << "Welcome to " << server.name() << "!\r\n"
		<< "What's your name?\r\n"
		<< server.prompt();
	std::string name;
	bool done{ false };
	while (!done) {
		done = client.readline([&name](std::string input) {
			name = input;
		});
	}
	return std::make_shared<ClientInfo>(std::move(client), Player{ name });
}

void handle_client(Server& server, Socket client)
{
	try {
		auto client_info = init_client_session(server, std::move(client));
		auto &socket = client_info->get_socket();
		auto &player = client_info->get_player();
		socket << "Welcome, " << player.get_name() << ", have fun playing our game!\r\n" << server.prompt();

		while (server.is_running()) {
			try {
				std::string cmd;
				if (socket.readline([&cmd](std::string input) { cmd = input; })) {
					server << '[' << socket.get_dotted_ip() << " (" << socket.get_socket() << ") " << player.get_name() << "] " << cmd << "\r\n";

					if (cmd == "quit") {
						socket.write("Bye!\r\n");
						break;
					}
					else if (cmd == "quit_server") {
						server.stop();
					}

					server.enqueue_command(ClientCommand{ cmd, client_info });
				};

			}
			catch (const std::exception& ex) {
				socket << "ERROR: " << ex.what() << "\r\n";
			}
			catch (...) {
				socket.write("ERROR: something went wrong during handling of your request. Sorry!\r\n");
			}
		}
	}
	catch (std::exception &ex) {
		server << "handle_client " << ex.what() << "\n";
	}
	catch (...) {
		server << "handle_client crashed\n";
	}
}


int main(int argc, const char * argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	auto server = create_server(consume_command, handle_client)
		.with_name("machiavelli")
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


