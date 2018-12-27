#include "server/Server.hpp"
#include "server/ClientInfo.hpp"
#include "server/Player.hpp"
#include <chrono>
#include <exception>
#include <iostream>
#include <thread>
#include <utility>

void consume_command(Server& server) // runs in its own thread
{
	try {
		while (server._running) {
			ClientCommand command{ server._queue.get() }; // will block here unless there are still command objects in the queue
			if (auto clientInfo = command.get_client_info().lock()) {
				auto &client = clientInfo->get_socket();
				auto &player = clientInfo->get_player();
				try {
					// TODO handle command here
					client << player.get_name() << ", you wrote: '" << command.get_cmd() << "', but I'll ignore that for now.\r\n" << server._prompt;
				}
				catch (const std::exception& ex) {
					std::cerr << "*** exception in consumer thread for player " << player.get_name() << ": " << ex.what() << '\n';
					if (client.is_open()) {
						client.write("Sorry, something went wrong during handling of your request.\r\n");
					}
				}
				catch (...) {
					std::cerr << "*** exception in consumer thread for player " << player.get_name() << '\n';
					if (client.is_open()) {
						client.write("Sorry, something went wrong during handling of your request.\r\n");
					}
				}
			}
		}
	}
	catch (...) {
		std::cerr << "consume_command crashed\n";
	}
}

std::shared_ptr<ClientInfo> init_client_session(Server& server, Socket client) {
	client.write("Welcome to " + server._server_name + "!\r\n");
	client.write("What's your name?\r\n");
	client.write(server._prompt);
	std::string name;
	bool done{ false };
	while (!done) {
		done = client.readline([&name](std::string input) {
			name = input;
		});
	}
	return std::make_shared<ClientInfo>(std::move(client), Player{ name });
}

void handle_client(Server& server, Socket client) // this function runs in a separate thread
{
	try {
		auto client_info = init_client_session(server, std::move(client));
		auto &socket = client_info->get_socket();
		auto &player = client_info->get_player();
		socket << "Welcome, " << player.get_name() << ", have fun playing our game!\r\n" << server._prompt;

		while (server._running) { // game loop
			try {
				// read first line of request
				std::string cmd;
				if (socket.readline([&cmd](std::string input) { cmd = input; })) {
					std::cerr << '[' << socket.get_dotted_ip() << " (" << socket.get_socket() << ") " << player.get_name() << "] " << cmd << "\r\n";

					if (cmd == "quit") {
						socket.write("Bye!\r\n");
						break; // out of game loop, will end this thread and close connection
					}
					else if (cmd == "quit_server") {
						server._running = false;
					}

					ClientCommand command{ cmd, client_info };
					server._queue.put(command);
				};

			}
			catch (const std::exception& ex) {
				socket << "ERROR: " << ex.what() << "\r\n";
			}
			catch (...) {
				socket.write("ERROR: something went wrong during handling of your request. Sorry!\r\n");
			}
		}
		// close weg
	}
	catch (std::exception &ex) {
		std::cerr << "handle_client " << ex.what() << "\n";
	}
	catch (...) {
		std::cerr << "handle_client crashed\n";
	}
}

int Server::run()
{
	// start command consumer thread
	std::vector<std::thread> all_threads;
	all_threads.emplace_back(consume_command, std::ref(*this));

	// create a server socket
	ServerSocket server{ _tcp_port };

	try {
		std::cerr << "server listening" << '\n';
		while (_running) {
			// wait for connection from client; will create new socket
			server.accept([&all_threads, this](Socket client) {
				std::cerr << "Connection accepted from " << client.get_dotted_ip() << "\n";
				all_threads.emplace_back(handle_client, std::ref(*this), std::move(client));
			});
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	catch (const std::exception& ex) {
		std::cerr << ex.what() << ", resuming..." << '\n';
	}
	catch (...) {
		std::cerr << "problems, problems, but: keep calm and carry on!\n";
	}

	for (auto &t : all_threads) {
		t.join();
	}

	return 0;
}
